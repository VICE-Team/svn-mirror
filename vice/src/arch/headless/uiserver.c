#include "vice.h"

#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

#include "archdep.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "netexpect.h"
#include "resources.h"
#include "uiserver.h"
#include "uiprotocol.h"
#include "vicesocket.h"
#include "videoarch.h"

/*
 * DESIGN THOUGHTS.
 *
 * VICE will launch a single UI client process. The UI can choose to
 * launch a further child UI process for handling two screens concurrently,
 * or it can handle multiple screens itself.
 *
 * The UI server supports multiple UI clients connected to the same screen,
 * as well as a single UI client connected to multiple screens.
 */

typedef struct client_s client_t;
typedef void (*recv_message_t)(client_t *client, void *recieved_buffer);

/* Represents a connected client */
typedef struct client_s {
    int id;
    vice_network_socket_t *socket;
    netexpect_t *netexpect;
    
    uiprotocol_client_hello_t client_hello;
    
    uint8_t recieved_message;
    uint16_t recieved_string_size;
    char recieved_string[UI_PROTOCOL_V1_STRING_MAX + 1];
} client_t;

/* Represents a single video output, such as VICII or VDC. */
typedef struct screen_s {
    char *chip_name;
    video_canvas_t *canvas;
    client_t **subscribed_clients;
    unsigned int subscribed_clients_alloc_size;
    unsigned int subscribed_client_count;
} screen_t;

static vice_network_socket_t *server_socket;
static uiprotocol_server_hello_t server_hello;

static screen_t *screens;
static int screen_count;
static client_t *clients;
static int clients_size;
static int client_count;
static int client_ready_count;
static int next_client_id;
static bool remove_disconnected_clients_next_poll;

static struct pollfd *poll_fds;
static int poll_fd_count;
static bool rebuild_poll_fds_next_poll = true;

static int socket_available(void *context);
static int socket_recv(void *context, void *buffer, unsigned int size);

static void build_server_hello(void);
static void rebuild_poll_fds(void);
static void disconnect_client(client_t *client);
static void remove_disconnected_clients(void);
static void remove_screen_subscription(client_t *client, screen_t *screen);

static screen_t *find_screen(char *chip_name);

static void send_byte(client_t *client, uint8_t byte);
static void send_string(client_t *client, char *str);

static void advertise_screen(client_t *client, int screen_index);
static void advertise_all_screens(client_t *client);

static void handle_client_hello(void *context);
static void handle_post_hello_message(void *context);
static void handle_client_message(void *context);
static void handle_subscribe_screen(void *context);
static void handle_unsubscribe_screen(void *context);

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
     * makes this screen available for clients to subscribe to.
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

    screens[new_screen_index].chip_name                     = lib_strdup(canvas->videoconfig->chip_name);
    screens[new_screen_index].canvas                        = canvas;
    screens[new_screen_index].subscribed_clients            = NULL;
    screens[new_screen_index].subscribed_clients_alloc_size = 0;
    screens[new_screen_index].subscribed_client_count       = 0;
    
    /*
     * TODO: If any clients are connected already, notify them of the new screen.
     *
     * This won't happen in practice, unless we need to support hot-plugging of screens,
     * for example if we end up supporting that 1541 display hack and enabling it at
     * runtime.
     */
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
     * Wait for a client process to connect and signal that it's ready.
     */

    log_message(LOG_DEFAULT, "Waiting for UI");
    
    while (client_count == 0 || client_ready_count != client_count) {
        tick_sleep(tick_per_second() / 500);
        uiserver_poll();
    }
}

static void handle_new_client(void)
{
    vice_network_socket_t *client_socket;
    int client_index;
    client_t *client;
    
    client_socket = vice_network_accept(server_socket);
    client_index = client_count++;
    
    if (client_count > clients_size) {
        clients = lib_realloc(clients, client_count * sizeof(client_t));
        clients_size = client_count;
    }
    
    client              = &clients[client_index];
    client->id          = next_client_id++;
    client->socket      = client_socket;
    client->netexpect   = netexpect_new(socket_available, socket_recv, client_socket);

    log_message(LOG_DEFAULT, "New client %d connected", client->id);
    
    rebuild_poll_fds_next_poll = true;
    
    /* Send the server hello for the client to respond to */
    vice_network_send(client_socket, &server_hello, sizeof(server_hello), 0);
    
    /* expect a client hello in response */
    netexpect_byte_array(client->netexpect, &client->client_hello, sizeof(client->client_hello), handle_client_hello, client);
}

static void disconnect_client(client_t *client)
{
    vice_network_socket_close(client->socket);
    client->socket = NULL;
    
    remove_disconnected_clients_next_poll = true;
}

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
            if (netexpect_do_recv(clients[i].netexpect) < 0) {
                disconnect_client(&clients[i]);
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
        disconnect_client(&clients[i]);
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
    
    for (i = 0; i < screen_count; i++) {
        lib_free(screens[i].chip_name);
        screens[i].chip_name                        = NULL;
        lib_free(screens[i].subscribed_clients);
        screens[i].subscribed_clients               = NULL;
        screens[i].subscribed_clients_alloc_size    = 0;
        screens[i].subscribed_client_count          = 0;
    }
    
    lib_free(screens);
    screens = NULL;
    screen_count = 0;
}

/************/

static void build_server_hello(void)
{
    memset(&server_hello, 0, sizeof(server_hello));
    
    server_hello.magic[0] = 'V';
    server_hello.magic[1] = 'I';
    server_hello.magic[2] = 'C';
    server_hello.magic[3] = 'E';

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    server_hello.cpu_arch_flags |= UI_PROTOCOL_CPU_LITTLE_ENDIAN;
#endif
    
#if __LP64__
    server_hello.cpu_arch_flags |= UI_PROTOCOL_CPU_64_BIT;
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
        
        /* remove any screen subscriptions */
        for (j = 0; j < screen_count; j++) {
            remove_screen_subscription(&clients[i], &screens[j]);
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

static int socket_available(void *context)
{
    vice_network_socket_t *sock = (vice_network_socket_t*)context;
    
    return vice_network_available_bytes(sock);
}

static int socket_recv(void *context, void *buffer, unsigned int size)
{
    vice_network_socket_t *sock = (vice_network_socket_t*)context;
    
    return vice_network_receive(sock, buffer, size, 0);
}

static void send_byte(client_t *client, uint8_t byte)
{
    vice_network_send(client->socket, &byte, 1, 0);
}

static void send_string(client_t *client, char *str)
{
    size_t full_strlen;
    uint16_t message_strlen;
    
    /*
     * Max protocol string length is 64 kilobytes
     */
    
    full_strlen = strlen(str);
    
    if (full_strlen > UI_PROTOCOL_V1_STRING_MAX) {
        log_error(LOG_DEFAULT, "Fatal attempt to send string of length %zu", full_strlen);
        disconnect_client(client);
        return;
    }
    
    message_strlen = (uint16_t)full_strlen;
    
    vice_network_send(client->socket, &message_strlen, sizeof(message_strlen), 0);
    vice_network_send(client->socket, str, message_strlen, 0);
}

/************/

static void advertise_screen(client_t *client, int screen_index)
{
    send_byte(client, UI_PROTOCOL_V1_SCREEN_IS_AVAILABLE);
    send_string(client, screens[screen_index].chip_name);
}

static void advertise_all_screens(client_t *client)
{
    int i;
    
    for (i = 0; i < screen_count; i++) {
        advertise_screen(client, i);
    }
}

static screen_t *find_screen(char *chip_name)
{
    int i;
    
    for (i = 0; i < screen_count; i++) {
        if (!strcmp(chip_name, screens[i].chip_name)) {
            return &screens[i];
        }
    }
    
    return NULL;
}

static void handle_client_hello(void *context)
{
    client_t *client = (client_t*)context;
    uiprotocol_client_hello_t *client_hello = &client->client_hello;
    
    if (    client_hello->magic[0] != 'V'
        ||  client_hello->magic[1] != 'I'
        ||  client_hello->magic[2] != 'C'
        ||  client_hello->magic[3] != 'E'
        ) {
        log_error(LOG_DEFAULT,
                  "Bad client hello magic: %c%c%c%c",
                  client_hello->magic[0],
                  client_hello->magic[1],
                  client_hello->magic[2],
                  client_hello->magic[3]);
        
        disconnect_client(client);
        return;
    }
    
    log_message(LOG_DEFAULT, "Client hello: protocol 0x%x", client_hello->protocol);
    
    if (client_hello->protocol == UI_PROTOCOL_VERSION_1) {
        advertise_all_screens(client);
        send_byte(client, UI_PROTOCOL_V1_SERVER_IS_READY);
        netexpect_u8(client->netexpect, &client->recieved_message, handle_post_hello_message, client);
    } else {
        log_error(LOG_DEFAULT, "Client %d requested unsupported protocol 0x%c", client->id, client_hello->protocol);
        disconnect_client(client);
    }
}

static void handle_subscribe_screen(void *context)
{
    client_t *client = (client_t*)context;
    screen_t *screen;
    int i;
    
    netexpect_u8(client->netexpect, &client->recieved_message, handle_post_hello_message, client);
    
    screen = find_screen(client->recieved_string);
    if (!screen) {
        log_error(LOG_DEFAULT, "Client %d attempting to subscribe to invalid screen: %s", client->id, client->recieved_string);
        return;
    }
    
    /* Client is requesting a valid screen */
    for(i = 0; i < screen->subscribed_client_count; i++) {
        if (screen->subscribed_clients[i] == client) {
            log_error(LOG_DEFAULT, "Client %d attempting to resubscribe to screen: %s", client->id, client->recieved_string);
            return;
        }
    }
    
    if (screen->subscribed_client_count == screen->subscribed_clients_alloc_size) {
        /* Increase space allocated for screen subscriptions */
        screen->subscribed_clients_alloc_size++;
        screen->subscribed_clients = lib_realloc(screen->subscribed_clients,
                                                 screen->subscribed_clients_alloc_size * sizeof(client_t*));
    }
    
    /* Store the newly subscribed client */
    screen->subscribed_clients[screen->subscribed_client_count++] = client;
    
    log_message(LOG_DEFAULT, "Client %d subscribed to screen: %s", client->id, client->recieved_string);
}

static void remove_screen_subscription(client_t *client, screen_t *screen)
{
    int i, j;
    
    for (i = 0; i < screen->subscribed_client_count; i++) {
        if (client == screen->subscribed_clients[i]) {
            /* remove the client from the array, shift newer clients back an index */
            for (j = i; j < screen->subscribed_client_count - 1; j++) {
                screen->subscribed_clients[j] = screens->subscribed_clients[j + 1];
            }
            screens->subscribed_client_count--;
            screens->subscribed_clients[j] = NULL;
            log_message(LOG_DEFAULT, "Client %d unsubscribed from screen: %s", client->id, screen->chip_name);
            return;
        }
    }
    
    log_error(LOG_DEFAULT, "Client %d attempting to remove non-existing subscription to screen: %s", client->id, client->recieved_string);
}

static void handle_unsubscribe_screen(void *context)
{
    client_t *client = (client_t*)context;
    screen_t *screen;
    
    netexpect_u8(client->netexpect, &client->recieved_message, handle_post_hello_message, client);
    
    screen = find_screen(client->recieved_string);
    if (!screen) {
        log_error(LOG_DEFAULT, "Client %d attempting to unsubscribe from invalid screen: %s", client->id, client->recieved_string);
        return;
    }
    
    remove_screen_subscription(client, screen);
}

static void handle_post_hello_message(void *context)
{
    client_t *client = (client_t*)context;
    
    switch (client->recieved_message) {
            
        case UI_PROTOCOL_V1_SUBSCRIBE_SCREEN:
            /* Expect a string: chip_name */
            netexpect_u16_prefixed_string(client->netexpect,
                                          client->recieved_string,
                                          &client->recieved_string_size,
                                          handle_subscribe_screen,
                                          client);
            break;
        
        case UI_PROTOCOL_V1_UNSUBSCRIBE_SCREEN:
            /* Expect a string: chip_name */
            netexpect_u16_prefixed_string(client->netexpect,
                                          client->recieved_string,
                                          &client->recieved_string_size,
                                          handle_unsubscribe_screen,
                                          client);
            break;

        case UI_PROTOCOL_V1_CLIENT_IS_READY:
            /*
             *
             */
            
            log_message(LOG_DEFAULT, "Client %d is ready", client->id);
            client_ready_count++;

            /* Move to the post-ready phase */
            netexpect_u8(client->netexpect, &client->recieved_message, handle_client_message, client);
            break;

        default:
            log_message(LOG_DEFAULT, "Did not understand client %d message %d, disconnecting", client->id, client->recieved_message);
            disconnect_client(client);
            break;
    }
}

static void handle_client_message(void *context)
{
    client_t *client = (client_t*)context;
    
    switch (client->recieved_message) {

        case UI_PROTOCOL_V1_CLIENT_IS_READY:
            /*
             *
             */
            
            log_message(LOG_DEFAULT, "Client %d is ready", client->id);
            client_ready_count++;

            /* Move to the post-ready phase */
            netexpect_u8(client->netexpect, &client->recieved_message, handle_client_message, client);
            break;

        default:
            log_message(LOG_DEFAULT, "Did not understand client %d message %d, disconnecting", client->id, client->recieved_message);
            disconnect_client(client);
            break;
    }
}
