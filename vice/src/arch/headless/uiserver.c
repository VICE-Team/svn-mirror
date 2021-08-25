#include "vice.h"

#include <errno.h>
#include <string.h>
#include <sys/poll.h>

#include "uiserver.h"

#include "archdep.h"
#include "lib.h"
#include "log.h"
#include "vicesocket.h"

/*
 * DESIGN THOUGHTS.
 *
 * VICE will launch a single UI child process. The UI can choose to
 * launch a further child UI process for handling two screens concurrently.
 * The UI server supports multiple UI processes connected to the same screen.
 */

/* Represents a connected uiclient */
typedef struct client_s {
    vice_network_socket_t *socket;
} client_t;

/* Represents a single video output, such as VICII or VDC. */
typedef struct screen_s {
    video_canvas_t *canvas;
    client_t *clients;
} screen_t;

static vice_network_socket_t *server_socket;
static screen_t *screens;
static int screen_count;
static client_t *clients;
static int client_count;

static struct pollfd *poll_fds;
static int poll_fd_count;

static void rebuild_poll_fds(void);

int uiserver_init(void)
{
    int retval = 0;
    
    vice_network_socket_address_t *address;
    
    /*
     * We let the OS choose the listen port so multiple emu
     * can run at the same time. The port is passed to the UI
     * process when launched.
     *
     * If a use case is found for running the emulation on one
     * machine and the ui on another (old ARM devices?) then it
     * will be straightforward to bind to 0.0.0.0 or take a
     * specific address string from a resource.
     */
    
    address = vice_network_address_generate("ip4://127.0.0.1", 0);
    
    server_socket = vice_network_server("UI server", address);
    if (!server_socket) {
        retval = -1;
        goto done;
    }
    
    rebuild_poll_fds();
    
done:
    vice_network_address_close(address);
    return retval;
}

void uiserver_add_screen(video_canvas_t *canvas)
{
    int i;
    int new_screen_index;

    /*
     * Called when a video chip first initialises. The uiserver
     * makes this screen available uiclients.
     */

    for (i = 0; i < screen_count; i++) {
        if (screens[i].canvas == canvas) {
            log_error(LOG_DEFAULT, "UI Server: Attempt to re-add screen for %s", canvas->videoconfig->chip_name);
            return;
        }
    }

    log_message(LOG_DEFAULT, "UI Server: Adding screen for %s", canvas->videoconfig->chip_name);
    
    new_screen_index = screen_count;
    screen_count++;

    screens = lib_realloc(screens, screen_count * sizeof(screen_t));

    screens[new_screen_index].canvas = canvas;
}

void uiserver_await_ready(void)
{

}

static void handle_new_client(void)
{
    vice_network_socket_t *client_socket;
    
    client_socket = vice_network_accept(server_socket);
    vice_network_send(client_socket, "hello\n", 6, 0);
    vice_network_socket_close(client_socket);
}

/** \brief Handle all uiclient IO */
void uiserver_poll(void)
{
    int i;
    int read_fd_count;

    /*
     * Do any of our sockets have any pending events?
     */

    read_fd_count = poll(poll_fds, poll_fd_count, 0);

    if (read_fd_count == 0) {
        /* No. */
        return;
    }

    if (read_fd_count == -1) {
        log_error(LOG_DEFAULT, "UI Server: poll error: %s", strerror(errno));
        archdep_vice_exit(1);
    }

    /* Something is waiting for us. Check client IO first. */
    for (i = 0; i < client_count; i++) {
        if (poll_fds[i].events == POLLIN) {
            log_message(LOG_DEFAULT, "POLLIN on client socket %d", i);
        } else if (poll_fds[i].events) {
            /* Any other event is an error */
            log_message(LOG_DEFAULT, "ERROR on client socket %d", i);
        }
    }

    /* Last, check for new client connections */
    if (poll_fds[i].events == POLLIN) {
        log_message(LOG_DEFAULT, "New client connection");
        handle_new_client();
    } else if (poll_fds[i].events) {
        log_error(LOG_DEFAULT, "UI Server: server socket error");
        archdep_vice_exit(1);
    }
}

void uiserver_shutdown(void)
{
    /* Notifiy all connected clients that we're shutting down */
    // TODO
    
    if (server_socket) {
        vice_network_socket_close(server_socket);
        server_socket = NULL;
    }
}

/************/

static void rebuild_poll_fds(void)
{
    int i;

    if (poll_fds) {
        lib_free(poll_fds);
    }

    poll_fd_count = client_count + 1;
    poll_fds = lib_calloc(poll_fd_count, sizeof(struct pollfd));

    /* Add all client sockets */
    for (i = 0; i < client_count; i++) {
        poll_fds[i].fd = vice_network_get_socket(clients[i].socket);
        poll_fds[i].events = POLLIN;
    }

    /* Add the server socket to the end */
    poll_fds[i].fd = vice_network_get_socket(server_socket);
    poll_fds[i].events = POLLIN;
}
