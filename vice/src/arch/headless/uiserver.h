/** \file   uiserver.h
 * \brief   User interface server
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

#ifndef UI_SERVER_H
#define UI_SERVER_H

#include "videoarch.h"

int uiserver_init(void);
void uiserver_add_screen(video_canvas_t *canvas);
void uiserver_await_ready(void);
void uiserver_poll(void);
void uiserver_on_new_backbuffer(video_canvas_t *canvas);
void uiserver_shutdown(void);

#endif /* #ifndef UI_SERVER_H */
