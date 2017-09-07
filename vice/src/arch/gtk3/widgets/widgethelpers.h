/*
 * uihelpers.h - GTK3 helper function for create widgets - header
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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
 *  TODO:   rename functions and types
 */

#ifndef HAVE_WIDGETHELPERS_H
#define HAVE_WIDGETHELPERS_H

#include "vice.h"

#include <gtk/gtk.h>


typedef struct ui_text_int_pair_s {
    char *text;
    int value;
} ui_text_int_pair_t;

typedef struct ui_button_s {
    char *text;
    void (*callback)(GtkWidget *, void *);
    /* XXX: maybe add keyboard shortcut stuff ? */
} ui_button_t;

GtkWidget *uihelpers_create_button_box(
        ui_button_t *buttons,
        GtkOrientation orientation);

GtkWidget *uihelpers_create_grid_with_label(const gchar *text, gint columns);

GtkWidget *uihelpers_create_int_radiogroup_with_label(
        const gchar *label,
        ui_text_int_pair_t *data,
        void (*callback)(GtkWidget *, gpointer),
        int active);


void uihelpers_set_radio_button_grid_by_index(GtkWidget *grid, int index);

GtkWidget *uihelpers_create_grid_label(const char *text);

GtkWidget *uihelpers_create_indented_label(const char *text);


GtkWidget *uihelpers_create_resource_checkbox(
        const char *label, const char *resource);

#endif


