/** \file   src/arch/gtk3/widgets/widgethelpers.h
 * \brief   GTK3 helper functions for widgets - header
 *
 * TODO:    Remove once the 'base widgets' are implemented properly
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
 */

#ifndef VICE_WIDGETHELPERS_H
#define VICE_WIDGETHELPERS_H

#include "vice.h"
#include <gtk/gtk.h>

#include "basewidget_types.h"


/* XXX: either deprecated or needs renaming */

typedef struct ui_button_s {
    char *text;
    void (*callback)(GtkWidget *, void *);
    /* XXX: maybe add keyboard shortcut stuff ? */
} ui_button_t;

GtkWidget *uihelpers_create_button_box(
        ui_button_t *buttons,
        GtkOrientation orientation);

GtkWidget *uihelpers_create_grid_with_label(const gchar *text, gint columns);

GtkWidget *uihelpers_radiogroup_create(
        const gchar *label,
        ui_radiogroup_entry_t *data,
        void (*callback)(GtkWidget *, gpointer),
        int active);

int uihelpers_radiogroup_get_index(ui_radiogroup_entry_t *list, int value);

void uihelpers_radiogroup_set_index(GtkWidget *grid, int index);

GtkWidget *uihelpers_create_grid_label(const char *text);

GtkWidget *uihelpers_create_indented_label(const char *text);


/* new stuff */

GtkWidget *vice_gtk3_grid_new_spaced(int column_spacing, int row_spacing);



#endif
