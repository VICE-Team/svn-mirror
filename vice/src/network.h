/*
 * network.c - Connecting emulators via network.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
 *
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

#ifndef _NETWORK_H
#define _NETWORK_H

extern int network_resources_init(void);
extern int network_start_server(void);
extern int network_connect_client(void);
extern void network_disconnect(void);
extern void network_hook(void);
extern int network_connected(void);
extern void network_hook(void);
extern void network_event_record(unsigned int type, void *data, unsigned int size);
extern void network_shutdown(void);

#endif
