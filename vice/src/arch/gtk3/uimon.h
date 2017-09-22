/*
 * uimon.h - GTK3 Monitor - header
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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


#ifndef VICE_UIMON_H
#define VICE_UIMON_H

#include "vice.h"

#include <gtk/gtk.h>
#include "console.h"
#include "monitor.h"

void ui_monitor_activate_callback(GtkWidget *widget, gpointer user_data);
int uimon_out(const char *buffer);
char *uimon_in(const char *prompt);
char *uimon_get_in(char **ppchCommandLine, const char *prompt);
void uimon_notify_change(void);
console_t *uimon_window_resume(void);
console_t *uimon_window_open(void);
void uimon_window_suspend(void);
void uimon_window_close(void);
void uimon_set_interface(monitor_interface_t **monitor_interface_init, int count);

#endif

