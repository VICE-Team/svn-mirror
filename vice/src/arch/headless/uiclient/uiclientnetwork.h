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

#ifndef UI_CLIENT_NETWORK_H
#define UI_CLIENT_NETWORK_H

#include <stdint.h>
#include <sys/poll.h>

typedef struct socket_s socket_t;

void uiclient_network_init(void);
void uiclient_network_shutdown(void);

/* Create a socket and initiate a connection to the uiserver */
socket_t *uiclient_network_connect(uint16_t port);
void uiclient_network_close(socket_t *sock);

int uiclient_network_poll(socket_t *sock);
ssize_t uiclient_network_available_bytes(socket_t *sock);
ssize_t uiclient_network_recv(socket_t *sock, void *buffer, uint32_t buffer_length);
ssize_t uiclient_network_send(socket_t *sock, void *buffer, uint32_t buffer_length);

#endif /* #ifndef UI_CLIENT_NETWORK_H */
