/** \file   uiprotocol.h
 *
 * \author  David Hogan <david.q.hogan@gmail.com>
 */

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

#ifndef UI_PROTOCOL_H
#define UI_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

/*
 * Server CPU dictates the byte order of structure members for the connection.
 * In 2021 most people are using x86 arch so it doesn't make sense for server
 * and client to be doing conversions to generic network byte order.
 * 
 * Beyond that, we're not initially supporting running VICE on a separate
 * machine, let alone a separate machine with different arch.
 * 
 * But, let's politely make clients aware of the server CPU arch.
 */

#define UI_PROTOCOL_CPU_LITTLE_ENDIAN    1 << 0
#define UI_PROTOCOL_CPU_BIG_ENDIAN       1 << 1
#define UI_PROTOCOL_CPU_32_BIT           1 << 2
#define UI_PROTOCOL_CPU_64_BIT           1 << 3

/*
 * Protocol versions supported by the server. If a connecting client requiests
 * an unsupported version the connection will be rejected.
 */

#define UI_PROTOCOL_FEATURE_VERSION_1        1 << 1
/*
#define UI_PROTOCOL_FEATURE_VERSION_2        1 << 2
#define UI_PROTOCOL_FEATURE_VERSION_3        1 << 3
*/

/*
 * Handshake structures common to all protocols.
 * On connection, the server sends a uiserver_hello_header_t.
 * The client uses this to understand the running emulator, the server cpu arch,
 * and which protocol versions are supported.
 * 
 * Then the client replies with a uiclient_hello_header_t requesting a protocol.
 */

typedef struct uiprotocol_server_hello_header_s {
    char magic[4]; /* always "VICE" */
    uint8_t cpu_arch_flags;
    uint32_t emulator;
    uint32_t supported_protocols;
} __attribute__((packed)) uiprotocol_server_hello_header_t;

typedef struct uiprotocol_client_hello_header_s {
    char magic[4]; /* always "VICE" */
    uint32_t protocol;
} __attribute__((packed)) uiprotocol_client_hello_header_t;

/*
 * Strings are sent as a uin16_t size followed by string bytes.
 */

void uiprotocol_write_string(char *str);
void uiprotocol_read_string(char **str, uint32_t str_length);

/**********************************
 * Messages from client to server *
 **********************************/

/* This client is ready for emulation to begin or resume */
#define UI_PROTOCOL_V1_CLIENT_IS_READY      0   

/*
 * The client would like to recieve updates relating to the specified screen.
 *
 * string: chip_name
 */
#define UI_PROTOCOL_V1_SUBSCRIBE_SCREEN     1

/**********************************
 * Messages from server to client *
 **********************************/

#endif /* #ifndef UI_SERVER_H */