#include "vice.h"

#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

#include "uiserver.h"
#include "uiprotocol.h"

#include "archdep.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "tick.h"
#include "vicesocket.h"

/*
 * DESIGN THOUGHTS.
 *
 * VICE will launch a single UI client process. The UI can choose to
 * launch a further child UI process for handling two screens concurrently.
 * The UI server supports multiple UI clients connected to the same screen,
 * as well as a single UI client connected to multiple screens.
 */

/* Represents a connected uiclient */
typedef struct client_s {
    int id;
    vice_network_socket_t *socket;
} client_t;

/* Represents a single video output, such as VICII or VDC. */
typedef struct screen_s {
    video_canvas_t *canvas;
    client_t *clients;
} screen_t;

static vice_network_socket_t *server_socket;
static uiprotocol_server_hello_header_t server_hello;

static screen_t *screens;
static int screen_count;
static client_t *clients;
static int clients_size;
static int client_count;
static int next_client_id;
static bool remove_disconnected_clients_next_poll;

static struct pollfd *poll_fds;
static int poll_fd_count;
static bool rebuild_poll_fds_next_poll = true;

static void build_server_hello(void);
static void rebuild_poll_fds(void);
static void disconnect_client(client_t *client);
static void remove_disconnected_clients(void);

int uiserver_init(void)
{
    int retval = 0;
    
    vice_network_socket_address_t *address;
    
    /* The same hello is sent to all connecting clients */
    build_server_hello();
    
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
     * makes this screen available for uiclients to subscribe to.
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
    const char *ui_path;
    pid_t child_pid;
    char server_port_str[6];
    char ui_filename[PATH_MAX];

    resources_get_string("UiFilepath", &ui_path);

    /*
     * Launch the ui process if one is set
     */

    if (!strlen(ui_path)) {
        log_message(LOG_DEFAULT, "No UI requested, continuing");
        return;
    }

    child_pid = fork();
    if (child_pid == -1) {
        log_error(LOG_DEFAULT, "Failed to fork for ui process: %s", strerror(errno));
        archdep_vice_exit(1);
    }

    if (child_pid == 0) {
        /* Child process. Execute the ui executable, passing the server port number */
        basename_r(ui_path, ui_filename);
        snprintf(server_port_str, 6, "%hu", vice_network_get_ipv4_port(server_socket));

        execlp(ui_path, ui_filename, server_port_str, NULL);

        log_error(LOG_DEFAULT, "Execution has continued past execlp: %s", strerror(errno));
        exit(1);
    }
    
    /*
     * Wait for a uiclient process to connect and signal that it's ready.
     */

    log_message(LOG_DEFAULT, "Waiting for UI process");
    
    while (client_count == 0) {
        tick_sleep(tick_per_second() / 60);
        uiserver_poll();
    }
}

static void handle_new_client(void)
{
    vice_network_socket_t *client_socket;
    int client_index;
    
    client_socket = vice_network_accept(server_socket);
    client_index = client_count++;
    
    if (client_count > clients_size) {
        clients = lib_realloc(clients, client_count * sizeof(client_t));
        clients_size = client_count;
    }
    
    clients[client_index].id     = next_client_id++;
    clients[client_index].socket = client_socket;

    log_message(LOG_DEFAULT, "New client %d connected", clients[client_index].id);
    
    rebuild_poll_fds_next_poll = true;
    
    /* Send the server hello for the client to respond to */
    vice_network_send(client_socket, &server_hello, sizeof(server_hello), 0);
}

static void disconnect_client(client_t *client)
{
    vice_network_socket_close(client->socket);
    client->socket = NULL;
    
    remove_disconnected_clients_next_poll = true;
}

static void handle_client_error(client_t *client)
{
    log_message(LOG_DEFAULT, "Socket error on client %d: %s", client->id, strerror(errno));
    
    switch  (errno) {
        case ENOMEM:
        case ENOBUFS:
        case EINTR:
        case EAGAIN:
            /* Transient error, ignore */
            return;
        default:
            break;
    }
    
    disconnect_client(client);
}

/** \brief Handle all uiclient IO */
void uiserver_poll(void)
{
    int i;
    int read_fd_count;
    
    /* Remove any disconnected clients if needed */
    if (remove_disconnected_clients_next_poll) {
        remove_disconnected_clients();
        remove_disconnected_clients_next_poll = false;
    }
    
    /* Rebuild our poll_fd array if needed */
    if (rebuild_poll_fds_next_poll) {
        rebuild_poll_fds();
        rebuild_poll_fds_next_poll = false;
    }

    /* Do any of our sockets have any pending events? */
    read_fd_count = poll(poll_fds, poll_fd_count, 0);

    if (read_fd_count == 0) {
        /* No. */
        return;
    }

    if (read_fd_count == -1) {
        log_error(LOG_DEFAULT, "UI Server: general poll error: %s", strerror(errno));
        if (errno != EINTR && errno != EAGAIN) {
            archdep_vice_exit(1);
        }
    }

    /* Something is waiting for us. Check client IO first. */
    for (i = 0; i < client_count; i++) {
        if (poll_fds[i].revents) {
            log_message(LOG_DEFAULT, "POLLIN on client %d", clients[i].id);
            // HACK
            char b[5 + 1];
            int r;
            r = vice_network_receive(clients[i].socket, b, 5, 0);
            if (r < 0) {
                handle_client_error(&clients[i]);
            } else if (r == 0) {
                /* Closed */
                disconnect_client(&clients[i]);
            } else {
                b[r] = '\0';
                log_message(LOG_DEFAULT, "Client %d says [%s]", clients[i].id, b);
            }
        }
    }

    /* Last, check for new client connections */
    if (poll_fds[i].revents == POLLIN) {
        handle_new_client();
    } else if (poll_fds[i].revents) {
        log_error(LOG_DEFAULT, "UI Server: server socket error");
        archdep_vice_exit(1);
    }
}

void uiserver_shutdown(void)
{
    int i;
    
    /* Notifiy all connected clients that we're shutting down */
    for (i = 0; i < client_count; i++) {
        vice_network_socket_close(clients[i].socket);
        clients[i].socket = NULL;
    }
    
    lib_free(clients);
    clients = NULL;
    clients_size = 0;
    client_count = 0;
    
    if (server_socket) {
        vice_network_socket_close(server_socket);
        server_socket = NULL;
    }
    
    lib_free(poll_fds);
    poll_fds = NULL;
    
    lib_free(screens);
    screens = NULL;
    screen_count = 0;
}

/************/

static void build_server_hello(void)
{
    server_hello.magic[0] = 'V';
    server_hello.magic[1] = 'I';
    server_hello.magic[2] = 'C';
    server_hello.magic[3] = 'E';

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    server_hello.cpu_arch_flags |= UI_PROTOCOL_CPU_LITTLE_ENDIAN;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    server_hello.cpu_arch_flags |= UI_PROTOCOL_CPU_BIG_ENDIAN;
#endif
    
#if __LP64__
    server_hello.cpu_arch_flags |= UI_PROTOCOL_CPU_64_BIT;
#else
    server_hello.cpu_arch_flags |= UI_PROTOCOL_CPU_32_BIT;
#endif
    
    server_hello.emulator = machine_class;
    server_hello.supported_protocols = UI_PROTOCOL_VERSION_1;
}

static void rebuild_poll_fds(void)
{
    int i;

    lib_free(poll_fds);
    poll_fds = NULL;

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

static void remove_disconnected_clients(void)
{
    int i, j;
    
    for (i = 0; i < client_count; i++) {
        if (clients[i].socket) {
            continue;
        }
        
        /* remove the client from the array, shift newer clients back an index */
        for (j = i; j < client_count - 1; j++) {
            clients[j] = clients[j + 1];
        }
        client_count--;
        
        memset(&clients[j], 0, sizeof(client_t));
        
        /* Recheck this index */
        i--;
    }

    rebuild_poll_fds_next_poll = true;
    
    /* If the last client disconnected, exit the emulator */
    if (client_count == 0) {
        log_message(LOG_DEFAULT, "Last client disconnected, exiting");
        archdep_vice_exit(0);
    }
}
