/** \file   src/arch/gtk3/widgets/drivetypewidget.h
 * \brief   Drive settings dialog - header
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


#ifndef VICE_UIDRIVESETTINGS_H
#define VICE_UIDRIVESETTINGS_H

#include "vice.h"
#include <gtk/gtk.h>

void        drive_type_widget_set_extend_widget(GtkWidget *widget);
void        drive_type_widget_set_expansion_widget(GtkWidget *widget);
void        drive_type_widget_set_parallel_cable_widget(GtkWidget *widget);
void        drive_type_widget_set_options_widget(GtkWidget *widget);

GtkWidget * drive_type_widget_create(int unit, void (*callback)(int));
void        drive_type_widget_update(GtkWidget *widget, int unit);
void        drive_type_widget_connect_signals(GtkWidget *widget);

#endif
