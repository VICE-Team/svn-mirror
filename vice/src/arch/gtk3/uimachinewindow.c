/*
 * uimachinewindow.c - Native GTK3 main emulator window code.
 *
 * Written by
 *  Marcus Sutton <loggedoubt@gmail.com>
 *
 * based on code by
 *  TODO: move code by other people into this file
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

/* XXX: It should be possible to compile, link and run vsid while this
 *      entire file (amongst others) is contained inside an #if 0 wrapper.
 */
#if 1

#include "vice.h"

#include <gtk/gtk.h>

#include "videoarch.h"

#include "ui.h"
#include "uimachinewindow.h"

static void machine_window_create(struct video_canvas_s *canvas)
{
    GtkWidget *new_drawing_area;

    new_drawing_area = vice_renderer_backend->create_widget(canvas);

    canvas->drawing_area = new_drawing_area;
    canvas->event_box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(canvas->event_box), new_drawing_area);

    return;
}

void ui_machine_window_init(void)
{
    ui_set_create_window_func(machine_window_create);
    return;
}

#endif
