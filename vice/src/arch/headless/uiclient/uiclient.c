/*
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "uiclient.h"

#include "netexpect.h"
#include "uiclientmachine.h"
#include "uiclientnetwork.h"
#include "uiclientutil.h"
#include "uiprotocol.h"

static int socket_recv(void *context, void *buffer, unsigned int size);
static int socket_available(void *context);

static void send_byte(uiclient_t *uiclient, uint8_t byte);
static void send_string(uiclient_t *uiclient, char *str);

typedef void (*recv_message_t)(uiclient_t *uiclient, void *received_buffer);

typedef union scalar_u {
    uint8_t  u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t   s8;
    int16_t  s16;
    int32_t  s32;
    int64_t  s64;
} scalar_t;

typedef struct string16_s {
    uint16_t u16;
    char bytes[UI_PROTOCOL_V1_STRING_MAX + 1];
} string16_t;

typedef struct uiclient_s {
    uiclient_on_disconnected_t on_disconnected;
    uiclient_on_connected_t on_connected;
    uiclient_on_screen_available_t on_screen_available;
    
    socket_t *socket;
    netexpect_t *netexpect;
    
    uiprotocol_server_hello_t server_hello;
    uiprotocol_client_hello_t client_hello;
    
    uint8_t r_message;
    scalar_t r_scalar_0;
    string16_t r_string_0;
} uiclient_t;

static void build_client_hello(uiclient_t *uiclient, uint32_t protocol);

static void handle_server_hello(void *context);
static void handle_post_hello_message(void *context);
static void handle_server_message(void *context);

uiclient_t *uiclient_new(
    uiclient_on_disconnected_t on_disconnected_callback,
    uiclient_on_connected_t on_connected_callback,
    uiclient_on_screen_available_t on_screen_available_callback
    )
{
    uiclient_t *uiclient;

    INFO("Initialising");

    uiclient_network_init();

    uiclient                        = calloc(1, sizeof(uiclient_t));
    uiclient->on_disconnected       = on_disconnected_callback;
    uiclient->on_connected          = on_connected_callback;
    uiclient->on_screen_available   = on_screen_available_callback;

    return uiclient;
}

bool uiclient_connect(uiclient_t *uiclient, uint16_t server_port)
{
    INFO("Connecting to port %d", server_port);

    /* Initiate the socket connection */
    uiclient->socket = uiclient_network_connect(server_port);
    if (!uiclient->socket) {
        return false;
    }
    
    uiclient->netexpect = netexpect_new(socket_available, socket_recv, uiclient->socket);
    
    netexpect_byte_array(uiclient->netexpect, &uiclient->server_hello, sizeof(uiclient->server_hello), handle_server_hello, uiclient);
    
    return true;
}

void uiclient_subscribe_screen(uiclient_t *uiclient, char *chip_name)
{
    INFO("Subscribing to screen %s", chip_name);
    
    send_byte(uiclient, UI_PROTOCOL_V1_SUBSCRIBE_SCREEN);
    send_string(uiclient, chip_name);
}

void uiclient_unsubscribe_screen(uiclient_t *uiclient, char *chip_name)
{
    INFO("Unsubscribing from screen %s", chip_name);
    
    send_byte(uiclient, UI_PROTOCOL_V1_UNSUBSCRIBE_SCREEN);
    send_string(uiclient, chip_name);
}

void uiclient_poll(uiclient_t *uiclient)
{
    ssize_t result;
    
    result = uiclient_network_poll(uiclient->socket);
    if (result == 0) {
        return;
    } else if (result == -1) {
        uiclient->on_disconnected(uiclient);
        return;
    }
    
    result = netexpect_do_recv(uiclient->netexpect);
    
    if (result < 0) {
        /* Socket closed or error - is it fatal? */
        switch (errno) {
            case ENOMEM:
            case ENOBUFS:
            case EINTR:
            case EAGAIN:
                /* Transient error, ignore */
                ERR("uiclient transient socket error: %s", strerror(errno));
                break;
            default:
                ERR("uiclient fatal socket error: %s", strerror(errno));
                uiclient->on_disconnected(uiclient);
        }
    }
}

void uiclient_destroy(uiclient_t *uiclient)
{
    INFO("Destroying");

    if (uiclient->socket) {
        uiclient_network_close(uiclient->socket);
        uiclient->socket = NULL;
    }

    free(uiclient);

    uiclient_network_shutdown();
}

/****/

static void send_byte(uiclient_t *uiclient, uint8_t byte)
{
    uiclient_network_send(uiclient->socket, &byte, 1);
}

static void send_string(uiclient_t *uiclient, char *str)
{
    size_t full_strlen;
    uint16_t message_strlen;
    
    /*
     * Max protocol string length is 64 kilobytes - 1
     */
    
    full_strlen = strlen(str);
    
    if (full_strlen > UI_PROTOCOL_V1_STRING_MAX) {
        ERR("Fatal attempt to send string of length %zu", full_strlen);
        uiclient->on_disconnected(uiclient);
        return;
    }
    
    message_strlen = (uint16_t)full_strlen;
    
    uiclient_network_send(uiclient->socket, &message_strlen, sizeof(message_strlen));
    uiclient_network_send(uiclient->socket, str, message_strlen);
}

static const char *emulator_name(uint32_t emulator)
{
    switch (emulator) {
        case VICE_MACHINE_C64:
            return "x64";
        case VICE_MACHINE_C128:
            return "x128";
        case VICE_MACHINE_VIC20:
            return "xvic";
        case VICE_MACHINE_PET:
            return "xpet";
        case VICE_MACHINE_CBM5x0:
            return "xcbm5x0";
        case VICE_MACHINE_CBM6x0:
            return "xcbm2";
        case VICE_MACHINE_PLUS4:
            return "xplus4";
        case VICE_MACHINE_C64DTV:
            return "x64dtv";
        case VICE_MACHINE_C64SC:
            return "x64sc";
        case VICE_MACHINE_VSID:
            return "vsid";
        case VICE_MACHINE_SCPU64:
            return "xscpu64";

        default:
            return "unknown emulator";
    }
}

static void build_client_hello(uiclient_t *uiclient, uint32_t protocol)
{
    uiprotocol_client_hello_t *client_hello = &uiclient->client_hello;
    
    memset(client_hello, 0, sizeof(*client_hello));
    
    client_hello->magic[0] = 'V';
    client_hello->magic[1] = 'I';
    client_hello->magic[2] = 'C';
    client_hello->magic[3] = 'E';
    
    client_hello->protocol = protocol;
}

static int socket_recv(void *context, void *buffer, unsigned int size)
{
    socket_t *sock = (socket_t*)context;
    
    return (int)uiclient_network_recv(sock, buffer, size);
}

static int socket_available(void *context)
{
    socket_t *sock = (socket_t*)context;
    
    return (int)uiclient_network_available_bytes(sock);
}

static void expect_post_hello_message(uiclient_t *uiclient)
{
    netexpect_u8(uiclient->netexpect, &uiclient->r_message, handle_post_hello_message, uiclient);
}

static void expect_server_message(uiclient_t *uiclient)
{
    netexpect_u8(uiclient->netexpect, &uiclient->r_message, handle_server_message, uiclient);
}

static void handle_server_hello(void *context)
{
    uiclient_t *uiclient = (uiclient_t*)context;
    uiprotocol_server_hello_t *server_hello = &uiclient->server_hello;
    
    if (    server_hello->magic[0] != 'V'
        ||  server_hello->magic[1] != 'I'
        ||  server_hello->magic[2] != 'C'
        ||  server_hello->magic[3] != 'E'
        ) {
        ERR("Bad server hello magic: %c%c%c%c",
            server_hello->magic[0],
            server_hello->magic[1],
            server_hello->magic[2],
            server_hello->magic[3]);
        
        uiclient->on_disconnected(uiclient);
        return;
    }
    
    INFO("Server hello: %s, %d-bit %s endian, protocol support: 0x%x",
         emulator_name(server_hello->emulator),
         server_hello->cpu_arch_flags & UI_PROTOCOL_CPU_64_BIT          ? 64 : 32,
         server_hello->cpu_arch_flags & UI_PROTOCOL_CPU_LITTLE_ENDIAN   ? "little" : "big",
         server_hello->supported_protocols
         );
    
    /*
     * Test each supported protocol in order of preference
     */
    
    if (server_hello->supported_protocols & UI_PROTOCOL_VERSION_1) {
        build_client_hello(uiclient, UI_PROTOCOL_VERSION_1);
    } else {
        ERR("Server does not support a compatible protocol");
        uiclient->on_disconnected(uiclient);
        return;
    }
    
    /* Send a client hello nominating our desired protocol version */
    uiclient_network_send(uiclient->socket, &uiclient->client_hello, sizeof(uiclient->client_hello));
    
    /* Move to the post-hello pre-ready phase */
    expect_post_hello_message(uiclient);
}

static void handle_screen_available(void *context)
{
    uiclient_t *uiclient = (uiclient_t*)context;
    
    INFO("Screen %d available: %s", uiclient->r_scalar_0.u8, uiclient->r_string_0.bytes);
    
    /* Notify the client application */
    uiclient->on_screen_available(uiclient, uiclient->r_string_0.bytes);
    
    expect_post_hello_message(uiclient);
}

static void handle_post_hello_message(void *context)
{
    uiclient_t *uiclient = (uiclient_t*)context;
    
    /*
     * The server will advertise each available screen and finish up with a
     * server ready message when done.
     */
    
    switch (uiclient->r_message) {
        case UI_PROTOCOL_V1_SCREEN_IS_AVAILABLE:
            /* chip index */
            netexpect_u8(uiclient->netexpect, &uiclient->r_scalar_0.u8, NULL, NULL);
            /* chip name */
            netexpect_u16_prefixed_string(uiclient->netexpect,
                                          uiclient->r_string_0.bytes,
                                          &uiclient->r_string_0.u16,
                                          handle_screen_available,
                                          uiclient);
            break;

        case UI_PROTOCOL_V1_SERVER_IS_READY:
            /*
             * Let the application know that the server is ready.
             * Expected next steps are the subscription of screens followed
             * notifying the server that the client is ready to continue.
             */
            
            INFO("Server is ready");
            uiclient->on_connected(uiclient, uiclient->server_hello.emulator);

            /* Move to the post-ready phase */
            expect_server_message(uiclient);
            break;

        default:
            ERR("Did not understand server message %d, disconnecting", uiclient->r_message);
            uiclient->on_disconnected(uiclient);
            return;
    }
}

void uiclient_ready(uiclient_t *uiclient)
{
    /* Let server know that this client is ready for emulation to continue */
    send_byte(uiclient, UI_PROTOCOL_V1_CLIENT_IS_READY);
}

static void handle_screen_updated(void *context)
{
    uiclient_t *uiclient = (uiclient_t*)context;
    
    INFO("Screen %d updated", uiclient->r_scalar_0.u8);
    
    expect_server_message(uiclient);
}

static void handle_server_message(void *context)
{
    uiclient_t *uiclient = (uiclient_t*)context;
    
    /*
     * Everyting is established, this is the main io loop for
     * receiving screen updates etc.
     */
    
    switch (uiclient->r_message) {
        case UI_PROTOCOL_V1_SCREEN_UPDATED:
            /* chip index */
            netexpect_u8(uiclient->netexpect, &uiclient->r_scalar_0.u8, handle_screen_updated, uiclient);
            break;

        default:
            ERR("Did not understand server message %d, disconnecting", uiclient->r_message);
            uiclient->on_disconnected(uiclient);
            return;
    }
}
