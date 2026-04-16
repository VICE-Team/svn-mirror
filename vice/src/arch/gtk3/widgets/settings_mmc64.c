/** \file   settings_mmc64.c
 * \brief   Settings widget to control MMC64 resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES MMC64               x64 x64sc xscpu64 x128
 * $VICERES MMC64BIOSfilename   x64 x64sc xscpu64 x128
 * $VICERES MMC64_bios_write    x64 x64sc xscpu64 x128
 * $VICERES MMC64_flashjumper   x64 x64sc xscpu64 x128
 * $VICERES MMC64_revision      x64 x64sc xscpu64 x128
 * $VICERES MMC64imagefilename  x64 x64sc xscpu64 x128
 * $VICERES MMC64_RO            x64 x64sc xscpu64 x128
 * $VICERES MMC64_sd_type       x64 x64sc xscpu64 x128
 * $VICERES MMC64ClockPort      x64 x64sc xscpu64 x128
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
#include "log.h"
#include "resources.h"
#include "vice_gtk3.h"

#include "settings_mmc64.h"


#define DEBUG_MMC64

#ifdef DEBUG_MMC64
#define DBG(x)  log_printf x
#else
#define DBG(x)
#endif


/** \brief  List of revisions
 */
static const vice_gtk3_radiogroup_entry_t revisions[] = {
    { "Rev. A", MMC64_REV_A },
    { "Rev. B", MMC64_REV_B },
    { NULL,     -1 }
};

/** \brief  List of memory card types
 */
static const vice_gtk3_radiogroup_entry_t card_types[] = {
    { "Auto", MMC64_TYPE_AUTO },
    { "MMC",  MMC64_TYPE_MMC },
    { "SD",   MMC64_TYPE_SD },
    { "SDHC", MMC64_TYPE_SDHC },
    { NULL,   -1 }
};


/** \brief  SD card widget filename entry */
static GtkWidget *card_filename = NULL;


/** \brief  Create widget to toggle the MMC64 flash jumper
 *
 * \return  GtkSwitch
 */
static GtkWidget *create_mmc64_jumper_widget(void)
{
    GtkWidget *sw;

    sw = vice_gtk3_resource_switch_new("MMC64_flashjumper");
    gtk_widget_set_hexpand(sw, FALSE);
    gtk_widget_set_vexpand(sw, FALSE);
    return sw;
}

/** \brief  Create widget to set the MMC64 revision
 *
 * \return  GtkGrid
 */
static GtkWidget *create_mmc64_revision_widget(void)
{
    GtkWidget *group;

    group = vice_gtk3_resource_radiogroup_new("MMC64_revision",
                                              revisions,
                                              GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_set_column_spacing(GTK_GRID(group), 8);
    return group;
}

/** \brief  Add widgets for the SD/MCC card
 *
 * \param[in]   grid    main grid
 * \param[in]   row     row in \a grid to add widgets
 * \param[in]   columns number of columns in \a grid, for proper column span
 *
 * \return  row in \a grid to add more widgets
 */
static int create_card_image_layout(GtkWidget *grid, int row, int columns)
{
    GtkWidget  *label;
    GtkWidget  *card_writes;
    GtkWidget  *type;
    const char *title;

    /* FIXME: SD/MMC Card Widget should also be implemented in common code */

    /* header */
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label),
                         "<b>" CARTRIDGE_NAME_MMC64 " SD/MMC Card</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_margin_top(label, 16);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, columns, 1);
    row++;

    /* card image file */
    label = gtk_label_new("Card image file");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    card_filename = vice_gtk3_resource_filechooser_new("MMC64Imagefilename",
                                                       GTK_FILE_CHOOSER_ACTION_OPEN);
    title = "Select " CARTRIDGE_NAME_MMC64 " SD/MMC card image file";
    vice_gtk3_resource_filechooser_set_custom_title(card_filename, title);
    gtk_grid_attach(GTK_GRID(grid), label,         0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), card_filename, 1, row, columns - 1, 1);
    row++;

    /* card type */
    label = gtk_label_new("Card type");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    type = vice_gtk3_resource_radiogroup_new("MMC64_sd_type",
                                             card_types,
                                             GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_set_column_spacing(GTK_GRID(type), 16);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), type,  1, row, columns - 1, 1);
    row++;

    /* card writes */
    card_writes = vice_gtk3_resource_check_button_new("MMC64_RO",
                                                      "Enable SD/MMC card read-only");
    gtk_grid_attach(GTK_GRID(grid), card_writes, 1, row, columns - 1, 1);
    row++;
    return row + 1;
}

static void on_enable_clicked(GtkWidget *widget, gpointer user_data)
{
    /*GtkWidget *parent = gtk_widget_get_toplevel(widget);*/
    int enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    DBG(("MMC64 on_enable_clicked: %d", enabled));
    /* FIXME: update save/flush button(s) */
}

/** \brief  Create widget to control MMC64 resources
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_mmc64_widget_create(GtkWidget *parent)
{
    /* number of columns in grid */
#define NUM_COLS 4
    GtkWidget *grid;
    GtkWidget *enable_widget;
    GtkWidget *jumper_wrapper;
    GtkWidget *jumper_label;
    GtkWidget *jumper_widget;
    GtkWidget *revision_label;
    GtkWidget *revision_widget;
    GtkWidget *clockport_label;
    GtkWidget *clockport_widget;
    GtkWidget *primary;
    int        row = 0;

    DBG(("MMC64 settings_mmc64_widget_create"));

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    /* all rows need at least 8 pixels space between widgets, so we set the
     * minimum spacing here */
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    /* enable emulation check button */
    enable_widget = carthelpers_create_enable_check_button(CARTRIDGE_NAME_MMC64,
                                                           CARTRIDGE_MMC64);
    g_signal_connect(G_OBJECT(enable_widget),
                     "clicked",
                     G_CALLBACK(on_enable_clicked),
                     NULL);

    /* jumper switch and label */
    jumper_wrapper = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(jumper_wrapper), 16);
    jumper_label   = gtk_label_new(CARTRIDGE_NAME_MMC64 " Flash jumper");
    jumper_widget  = create_mmc64_jumper_widget();
    gtk_widget_set_halign(jumper_label, GTK_ALIGN_END);
    gtk_widget_set_hexpand(jumper_label, TRUE);
    gtk_widget_set_halign(jumper_widget, GTK_ALIGN_END);
    gtk_widget_set_valign(jumper_widget, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(jumper_wrapper), jumper_label,  0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(jumper_wrapper), jumper_widget, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), enable_widget,  0, row, 2 ,1);
    gtk_grid_attach(GTK_GRID(grid), jumper_wrapper, 2, row, 2, 1);
    row++;

    revision_label   = gtk_label_new(CARTRIDGE_NAME_MMC64 " Revision");
    revision_widget  = create_mmc64_revision_widget();
    gtk_widget_set_halign(revision_label, GTK_ALIGN_START);
    gtk_widget_set_valign(revision_widget, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), revision_label,   0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), revision_widget,  1, row, 1, 1);
    row++;

    clockport_label  = gtk_label_new("ClockPort device");
    clockport_widget = clockport_device_widget_create("MMC64ClockPort");
    gtk_widget_set_halign(clockport_label, GTK_ALIGN_START);
    gtk_widget_set_margin_top(clockport_label, 16);
    gtk_widget_set_margin_top(clockport_widget, 16);
    gtk_grid_attach(GTK_GRID(grid), clockport_label,  0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), clockport_widget, 1, row, 1, 1);
    row++;


    primary = cart_image_widget_new(CARTRIDGE_MMC64,        /* cart id */
                                    CARTRIDGE_NAME_MMC64,   /* cart name */
                                    CART_IMAGE_PRIMARY,     /* image number */
                                    "BIOS",            /* image tag */
                                    "MMC64BIOSfilename",                   /* resource name */
                                    TRUE,                   /* flush button */
                                    TRUE                    /* save button */
                                    );
    cart_image_widget_append_check(primary,
                                   "MMC64_bios_write",
                                   "Enable BIOS image writes");


    gtk_grid_attach(GTK_GRID(grid), primary,   0, row, 4, 1);
    row++;

    row = create_card_image_layout(grid, row, NUM_COLS);

#undef NUM_COLS

    gtk_widget_show_all(grid);
    return grid;
}
