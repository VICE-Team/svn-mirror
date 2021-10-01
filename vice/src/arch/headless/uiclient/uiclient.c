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
#include "uiclientnetwork.h"
#include "uiclientutil.h"
#include "uiprotocol.h"

typedef void (*recv_message_t)(uiclient_t *uiclient);

typedef struct uiclient_s {
    uiclient_on_disconnected_t on_disconnected;
    socket_t *socket;
    
    uint8_t *recv_buffer;
    uint32_t recv_buffer_recieved_bytes;
    uint32_t recv_buffer_remaining_bytes;
    recv_message_t recv_handler;
    
    uiprotocol_server_hello_header_t server_hello;
} uiclient_t;

static void expect(uiclient_t *uiclient, void *buffer, uint32_t size, recv_message_t handler)
{
    uiclient->recv_buffer                   = buffer;
    uiclient->recv_buffer_recieved_bytes    = 0;
    uiclient->recv_buffer_remaining_bytes   = size;
    uiclient->recv_handler                  = handler;
}

static void handle_state_machine_bug(uiclient_t *uiclient);
static void handle_server_hello(uiclient_t *uiclient);

uiclient_t *uiclient_new(
    uiclient_on_disconnected_t on_disconnected_callback
    )
{
    uiclient_t *uiclient;

    INFO("Initialising");

    uiclient_util_init();
    uiclient_network_init();

    uiclient = calloc(1, sizeof(uiclient_t));
    uiclient->on_disconnected = on_disconnected_callback;
    
    expect(uiclient, &uiclient->server_hello, sizeof(uiclient->server_hello), handle_server_hello);

    return uiclient;
}

bool uiclient_connect(uiclient_t *uiclient, uint16_t server_port)
{
    INFO("Connecting to port %d", server_port);

    /* Initiate the socket connection */
    uiclient->socket = uiclient_network_connect(server_port);
    return uiclient->socket ? true : false;
}

void uiclient_subscribe_screen(uiclient_t *uiclient, char *chip_name)
{
    INFO("Subscribing to screen %s", chip_name);
}

void uiclient_poll(uiclient_t *uiclient)
{
    ssize_t result;
    recv_message_t handler;
    
    result = uiclient_network_poll(uiclient->socket);
    if (result == 0) {
        return;
    } else if (result == -1) {
        uiclient->on_disconnected(uiclient);
        return;
    }
    
    result = uiclient_network_recv(uiclient->socket,
                                   uiclient->recv_buffer + uiclient->recv_buffer_recieved_bytes,
                                   uiclient->recv_buffer_remaining_bytes);
    if(result == 0) {
        /* Closed */
        uiclient->on_disconnected(uiclient);
        return;
    } else if(result == -1) {
        /* Socket error - is it fatal? */
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
        return;
    }
    
    uiclient->recv_buffer_recieved_bytes += result;
    uiclient->recv_buffer_remaining_bytes -= result;
    
    if (uiclient->recv_buffer_remaining_bytes == 0) {
        /*
         * Set the next handler to something invalid before calling this one -
         * it's the called handler's responsibility to set the next handler.
         */
        
        handler = uiclient->recv_handler;
        uiclient->recv_handler = handle_state_machine_bug;
        handler(uiclient);
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
    uiclient_util_shutdown();
}

/****/

static void handle_state_machine_bug(uiclient_t *uiclient)
{
    /*
     * If this is executed, there is a bug in the handler logic.
     * Probably something forgot to 'expect' the next thing.
     */
    
    ERR("uiclient fatal state machine bug");
    uiclient->on_disconnected(uiclient);
}

static void handle_server_hello(uiclient_t *uiclient)
{
    uiprotocol_server_hello_header_t *server_hello = &uiclient->server_hello;
    
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
    
    INFO("Server hello\n\tlittle endian: %d\n\tbig endian: %d\n\t64-bit: %d\n\t32-bit: %d\n\temu: %d\n\tprotocols: %x",
         server_hello->cpu_arch_flags & UI_PROTOCOL_CPU_LITTLE_ENDIAN   ? 1 : 0,
         server_hello->cpu_arch_flags & UI_PROTOCOL_CPU_BIG_ENDIAN      ? 1 : 0,
         server_hello->cpu_arch_flags & UI_PROTOCOL_CPU_64_BIT          ? 1 : 0,
         server_hello->cpu_arch_flags & UI_PROTOCOL_CPU_32_BIT          ? 1 : 0,
         server_hello->emulator,
         server_hello->supported_protocols & UI_PROTOCOL_VERSION_1      ? 1 : 0
         );
    
    uiclient_network_send(uiclient->socket, "hello!", 6);
}
