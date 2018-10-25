/** \file   crtpreviewwidget.c
 * \brief   Widget to show CRT cart image data before attaching
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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

#include "vice.h"

#include <gtk/gtk.h>

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "cartridge.h"
#include "c64/cart/crt.h"

#include "crtpreviewwidget.h"


static FILE *(*open_func)(const char *, crt_header_t *header) = NULL;
static int (*chip_func)(crt_chip_header_t *, FILE *) = NULL;


static GtkWidget *crtid_label;
static GtkWidget *crtname_label;
static GtkWidget *exrom_label;
static GtkWidget *game_label;


static GtkWidget *create_label(const char *s)
{
    GtkWidget *label = gtk_label_new(s);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    return label;
}



/** \brief  Create cartridge preview widget
 *
 * \return  GtkGrid
 */
GtkWidget *crt_preview_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    int row;

    grid = uihelpers_create_grid_with_label("CRT data", 2);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    row = 1;

    label = create_label("ID:");
    crtid_label = create_label("<unknown>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), crtid_label, 1, row, 1, 1);
    row++;

    label = create_label("Name:");
    crtname_label = create_label("<unknown>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), crtname_label, 1, row, 1, 1);
    row++;

    label = create_label("EXROM:");
    exrom_label = create_label("<unknown>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), exrom_label, 1, row, 1, 1);
    row++;

    label = create_label("GAME:");
    game_label = create_label("<unknown>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), game_label, 1, row, 1, 1);
    row++;

    gtk_widget_show_all(grid);
    return grid;
}


void crt_preview_widget_set_open_func(FILE *(*func)(const char *, crt_header_t *))
{
    open_func = func;
}


void crt_preview_widget_set_chip_func(int (*func)(crt_chip_header_t *, FILE *))
{
    chip_func = func;
}


void crt_preview_widget_update(const gchar *path)
{
    FILE *fd;
    crt_header_t header;
    gchar buffer[1024];

    const gchar *romstate[2] = {
        "active (lo)",
        "inactive (hi)"
    };


    debug_gtk3("Got path '%s'.", path);

    fd = open_func(path, &header);
    if (fd == NULL) {
        debug_gtk3("failed to open crt image");
        gtk_label_set_text(GTK_LABEL(crtid_label), "<unknown>");
        gtk_label_set_text(GTK_LABEL(crtname_label), "<unknown>");
        gtk_label_set_text(GTK_LABEL(exrom_label), "<unknown>");
        gtk_label_set_text(GTK_LABEL(game_label), "<unknown>");
        return;
    }

    /* type */
    g_snprintf(buffer, sizeof(buffer), "%d", (int)header.type);
    gtk_label_set_text(GTK_LABEL(crtid_label), buffer);

    /* name */
    gtk_label_set_text(GTK_LABEL(crtname_label), header.name);

    /* exrom */
    gtk_label_set_text(GTK_LABEL(exrom_label),
            romstate[header.exrom ? 1 : 0]);

    /* game */
    gtk_label_set_text(GTK_LABEL(game_label),
            romstate[header.game ? 1 : 0]);


    fclose(fd);
}
