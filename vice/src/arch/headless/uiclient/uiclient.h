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

#ifndef UI_CLIENT_H
#define UI_CLIENT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct uiclient_s uiclient_t;

/* Callback for unexpected server disconnection */
typedef void (*uiclient_on_disconnected_t)(uiclient_t *uiclient);
typedef void (*uiclient_on_connected_t)(uiclient_t *uiclient, uint32_t emulator);
typedef void (*uiclient_on_screen_available_t)(uiclient_t *uiclient, char *chip_name);

/* Create and initialise a new uiclient */
uiclient_t *uiclient_new(
    uiclient_on_disconnected_t on_disconnected_callback,
    uiclient_on_connected_t on_connected_callback,
    uiclient_on_screen_available_t on_screen_available_callback
    );

/* Initiate the connection process */
bool uiclient_connect(uiclient_t *uiclient, uint16_t server_port);

/* Request ongoing updates from the named screen */
void uiclient_subscribe_screen(uiclient_t *uiclient, char *chip_name);

/* Request no more updates from the named screen */
void uiclient_unsubscribe_screen(uiclient_t *uiclient, char *chip_name);

/* Perform network IO. Callbacks may be called. */
void uiclient_poll(uiclient_t *uiclient);

/* Let server know that this client is ready for emulation to continue */
void uiclient_ready(uiclient_t *uiclient);

/* Shutdown and free uiclient */
void uiclient_destroy(uiclient_t *uiclient);

#endif /* #ifndef UI_CLIENT_H */
