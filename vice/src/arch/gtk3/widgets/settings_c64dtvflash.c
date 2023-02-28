/** \file   settings_c64dtvflash.c
 * \brief   Settings widget controlling C64DTV Flash resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES c64dtvromfilename   x64dtv
 * $VICERES c64dtvromrw         x64dtv
 * $VICERES FSFlashDir          x64dtv
 * $VICERES FlashTrueFS         x64dtv
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

#include "vice_gtk3.h"

#include "settings_c64dtvflash.h"


/** \brief  Create resource filechooser widget for DTV ROM
 *
 * \return  GtkEntry
 */
static GtkWidget *create_rom_widget(void)
{
    GtkWidget  *chooser;
    const char *patterns[] = { "*.bin", "*.rom", NULL };

    chooser = vice_gtk3_resource_filechooser_new("c64dtvromfilename",
                                                 GTK_FILE_CHOOSER_ACTION_OPEN);
    vice_gtk3_resource_filechooser_set_custom_title(chooser,
                                                    "Select C64DTV ROM");
    vice_gtk3_resource_filechooser_set_filter(chooser,
                                              "ROM files",
                                              patterns,
                                              TRUE);
    return chooser;
}

/** \brief  Create resource filechooser widget for Flash filesystem directory
 *
 * \return  GtkEntry
 */
static GtkWidget *create_flash_dir_widget(void)
{
    GtkWidget *chooser;

    chooser = vice_gtk3_resource_filechooser_new("FSFlashDir",
                                                 GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER);
    vice_gtk3_resource_filechooser_set_custom_title(chooser,
                                                    "Select Flash filesystem directory");
    return chooser;
}

/** \brief  Create left-aligned label
 *
 * \param[in]   text    label text (uses Pango markup)
 *
 * \return  GtkLabel
 */
static GtkWidget *label_helper(const char *text)
{
    GtkWidget *label = gtk_label_new(NULL);

    gtk_label_set_markup(GTK_LABEL(label), text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    return label;
}


/** \brief  Create C64DTV Flash settings widget
 *
 * \param[in]   parent  parent widget (ignored)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_c64dtvflash_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *rom_file;
    GtkWidget *rom_write;
    GtkWidget *flash_dir;
    GtkWidget *flash_hw;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    /* DTV ROM file chooser and R/W check button */
    label     = label_helper("C64DTV ROM file");
    rom_file  = create_rom_widget();
    rom_write = vice_gtk3_resource_check_button_new(
            "c64dtvromrw",
            "Enable writes to C64DTV ROM image");
    gtk_grid_attach(GTK_GRID(grid), label,     0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rom_file,  1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rom_write, 1, 1, 1,1);

    /* Flash dir and true HW flash check button */
    label     = label_helper("Flash FS directory");
    flash_dir = create_flash_dir_widget();
    flash_hw  = vice_gtk3_resource_check_button_new(
            "FlashTrueFS",
            "Enable true hardware flash file system");
    gtk_widget_set_margin_top(label,     16);
    gtk_widget_set_margin_top(flash_dir, 16);
    gtk_grid_attach(GTK_GRID(grid), label,     0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), flash_dir, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), flash_hw,  1, 3, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
