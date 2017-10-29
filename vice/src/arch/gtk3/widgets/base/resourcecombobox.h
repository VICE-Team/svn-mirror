/** \file   src/arch/gtk3/widgets/base/resourcecombobox.h
 * \brief   Combo box connected to a resource - header
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
 */

#ifndef VICE_RESOURCECOMBOBOX_H
#define VICE_RESOURCECOMBOBOX_H

#include "vice.h"
#include <gtk/gtk.h>
#include "basewidget_types.h"

GtkWidget * resource_combo_box_int_create(const char *resource,
                                          const ui_combo_entry_int_t *entries);

GtkWidget *resource_combo_box_int_create_sprintf(
        const char *fmt,
        const ui_combo_entry_int_t *entries,
        ...);

GtkWidget * resource_combo_box_int_create_with_label(const char *resource,
                                                     const ui_combo_entry_int_t *entries,
                                                     const char *label);

void        resource_combo_box_int_update(GtkWidget *widget, int id);

void        resource_combo_box_int_reset(GtkWidget *widget);

GtkWidget * resource_combo_box_str_create(const char *resource,
                                          const ui_combo_entry_str_t *entries);

GtkWidget *resource_combo_box_str_create_sprintf(
        const char *fmt,
        const ui_combo_entry_str_t *entries,
        ...);


GtkWidget * resource_combo_box_str_create_with_label(const char *resource,
                                                     const ui_combo_entry_str_t *entries,
                                                     const char *label);

void        resource_combo_box_str_update(GtkWidget *widget, const char *id);

void        resource_combo_box_str_reset(GtkWidget *widget);

#endif
