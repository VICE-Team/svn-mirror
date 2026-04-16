/** \file   settings_mmcr.c
 * \brief   Settings widget to control MMC Replay resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES MMCRCardImage   x64 x64sc xscpu64 x128
 * $VICERES MMCREEPROMImage x64 x64sc xscpu64 x128
 * $VICERES MMCREEPROMRW    x64 x64sc xscpu64 x128
 * $VICERES MMCRRescueMode  x64 x64sc xscpu64 x128
 * $VICERES MMCRImageWrite  x64 x64sc xscpu64 x128
 * $VICERES MMCRCardRW      x64 x64sc xscpu64 x128
 * $VICERES MMCRSDType      x64 x64sc xscpu64 x128
 * $VICERES MMCRClockPort   x64 x64sc xscpu64 x128
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

#include "c64cart.h"
#include "cartridge.h"
#include "resources.h"
#include "vice_gtk3.h"

#include "settings_mmcr.h"

/** \brief  Temporary define to make editing easier */
#define CARTNAME    CARTRIDGE_NAME_MMC_REPLAY


/** \brief  List of memory card types
 */
static const vice_gtk3_radiogroup_entry_t card_types[] = {
    { "Auto", MMCR_TYPE_AUTO },
    { "MMC",  MMCR_TYPE_MMC },
    { "SD",   MMCR_TYPE_SD },
    { "SDHC", MMCR_TYPE_SDHC },
    { NULL,   -1 }
};


/** \brief  Create grid with switch and label for the MMCRRescueMode resource
 *
 * \return  GtkGrid
 */
static GtkWidget *create_rescue_mode_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *rescue;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    label = gtk_label_new("Rescue mode");
    gtk_widget_set_halign(label, GTK_ALIGN_END);

    rescue = vice_gtk3_resource_switch_new("MMCRRescueMode");
    gtk_widget_set_hexpand(rescue, FALSE);
    gtk_widget_set_vexpand(rescue, FALSE);
    gtk_widget_set_halign(rescue, GTK_ALIGN_END);
    gtk_widget_set_valign(rescue, GTK_ALIGN_CENTER);

    gtk_grid_attach(GTK_GRID(grid), label,  0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rescue, 1, 0, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Add widgets for the SD/MCC card
 *
 * \param[in]   grid    main grid
 * \param[in]   row     row in \a grid to add widgets
 * \param[in]   columns number of columns in \a grid, for proper column span
 *
 * \return  row in \a grid to add more widgets
 */
static int create_card_layout(GtkWidget *grid, int row, int columns)
{
    GtkWidget  *label;
    GtkWidget  *image;
    GtkWidget  *card_writes;
    GtkWidget  *card_type;
    const char *title;

    /* FIXME: SD/MMC Card Widget should also be implemented in common code */

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label),
                         "<b>" CARTNAME "SD/MMC Card</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_margin_top(label, 16);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, columns, 1);
    row++;

    label = gtk_label_new("Image file");
    image = vice_gtk3_resource_filechooser_new("MMCRCardImage",
                                               GTK_FILE_CHOOSER_ACTION_OPEN);
    title = "Select " CARTNAME " SD/MMC card image file";
    vice_gtk3_resource_filechooser_set_custom_title(image, title);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), image, 1, row, columns - 1, 1);
    row++;

    card_writes = vice_gtk3_resource_check_button_new("MMCRCardRW",
                                                      "Enable SD/MMC card writes");

    label = gtk_label_new("Card type");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    card_type = vice_gtk3_resource_radiogroup_new("MMCRSDType",
                                                  card_types,
                                                  GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_set_column_spacing(GTK_GRID(card_type), 16);
    gtk_grid_attach(GTK_GRID(grid), label,     0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), card_type, 1, row, columns - 1, 1);
    row++;

    gtk_grid_attach(GTK_GRID(grid), card_writes, 1, row, columns - 1, 1);

    return row + 1;
}



/** \brief  Create widget to control MMC Replay resources
 *
 * \param[in]   parent  parent widget (used for dialogs)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_mmcr_widget_create(GtkWidget *parent)
{
#define NUM_COLS 4
    GtkWidget *grid;
    GtkWidget *rescue_widget;
    GtkWidget *clockport_label;
    GtkWidget *clockport_widget;
    GtkWidget *primary;
    GtkWidget *secondary;
    int        row = 0;

    grid = vice_gtk3_grid_new_spaced(8, 8);

    rescue_widget = create_rescue_mode_widget();
    gtk_widget_set_halign(rescue_widget, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), rescue_widget, 0, row, NUM_COLS, 1);
    row++;

    clockport_label  = gtk_label_new("ClockPort device");
    clockport_widget = clockport_device_widget_create("MMCRClockPort");
    gtk_widget_set_margin_top(clockport_label, 16);
    gtk_widget_set_margin_top(clockport_widget, 16);
    gtk_widget_set_halign(clockport_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), clockport_label,  0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), clockport_widget, 1, row, 1, 1);
    row++;

    primary = cart_image_widget_new(CARTRIDGE_MMC_REPLAY,        /* cart id */
                                    CARTRIDGE_NAME_MMC_REPLAY,   /* cart name */
                                    CART_IMAGE_PRIMARY,     /* image number */
                                    "cartridge",            /* image tag */
                                    NULL,                   /* resource name */
                                    TRUE,                   /* flush button */
                                    TRUE                    /* save button */
                                    );
    cart_image_widget_append_check(primary,
                                   "MMCRImageWrite",
                                   "Save image when changed");


    gtk_grid_attach(GTK_GRID(grid), primary,   0, row, 4, 1);
    row++;

    secondary = cart_image_widget_new(CARTRIDGE_MMC_REPLAY,
                                      CARTRIDGE_NAME_MMC_REPLAY,
                                      CART_IMAGE_SECONDARY,
                                      "EEPROM",
                                      "MMCREEPROMImage",
                                      TRUE,
                                      TRUE);
    cart_image_widget_append_check(secondary,
                                   "MMCREEPROMRW",
                                   "Enable writes to " CARTRIDGE_NAME_MMC_REPLAY
                                   " EEPROM image");
    gtk_grid_attach(GTK_GRID(grid), secondary,   0, row, 4, 1);
    row++;

    row = create_card_layout      (grid, row, NUM_COLS);

#undef NUM_COLS
    gtk_widget_show_all(grid);
    return grid;
}

#undef CARTNAME
