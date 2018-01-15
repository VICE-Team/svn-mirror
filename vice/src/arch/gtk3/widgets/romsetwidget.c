/** \file   src/arch/gtk3/widget/romsetwidget.c
 * \brief   GTK3 ROM set widget
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

#include "vice.h"

#include <gtk/gtk.h>

#include "debug_gtk3.h"
#include "basedialogs.h"
#include "basewidgets.h"
#include "filechooserhelpers.h"
#include "machine.h"
#include "widgethelpers.h"
#include "ui.h"

#include "romsetwidget.h"

#define ROMSET_DEFAULT  "default.vrs"

typedef enum rom_type_e {
    ROM_BASIC,
    ROM_KERNAL,
    ROM_CHARGEN
} rom_type_t;


typedef struct romset_entry_s {
    const char *resource;
    const char *label;
    void (*callback)(GtkWidget *, gpointer);
} romset_entry_t;


static void on_kernal_browse_clicked(GtkWidget *widget, gpointer entry);
static void on_basic_browse_clicked(GtkWidget *widget, gpointer entry);
static void on_chargen_browse_clicked(GtkWidget *widget, gpointer entry);


static ui_combo_entry_int_t rom_types[] = {
    { "BASIC",      ROM_BASIC },
    { "KERNAL",     ROM_KERNAL },
    { "CHARGEN",    ROM_CHARGEN },
    { NULL, -1 }
};


static const romset_entry_t c64_machine_roms[] = {
    { "KernalName", "Kernal", on_kernal_browse_clicked },
    { "BasicName", "Basic", on_basic_browse_clicked },
    { "ChargenName", "Chargen", on_chargen_browse_clicked },
    { NULL, NULL, NULL }
};


static const char *rom_file_patterns[] = {
    "*.rom", "*.bin", "*.raw", NULL
};

static GtkWidget *layout = NULL;
static GtkWidget *stack = NULL;
static GtkWidget *switcher = NULL;

static GtkWidget *child_c64_roms = NULL;
static GtkWidget *child_c64_sets = NULL;


static GtkWidget *rom_basic = NULL;
static GtkWidget *rom_kernal = NULL;
static GtkWidget *rom_chargen = NULL;



static void on_default_romset_load_clicked(void)
{
    debug_gtk3("trying to load '%s' ..", ROMSET_DEFAULT);
    if (machine_romset_file_load(ROMSET_DEFAULT) < 0) {
        debug_gtk3("FAILED!\n");
    } else {
        debug_gtk3("OK\n");
    }
}


static int add_stack_switcher(void)
{
    stack = gtk_stack_new();
    switcher = gtk_stack_switcher_new();

    gtk_stack_set_transition_type(GTK_STACK(stack),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 500);
    gtk_stack_set_homogeneous(GTK_STACK(stack), TRUE);
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher),
            GTK_STACK(stack));
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(switcher, TRUE);

    /* switcher goes first, otherwise it ends up a the bottom of the widget,
     * which we don't want, although maybe in a few years having the 'tabs'
     * at the bottom suddenly becomes popular, in which case we simply swap
     * the row number of the stack and the switcher :) */
    gtk_grid_attach(GTK_GRID(layout), switcher, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), stack, 0, 1, 1, 1);

    gtk_widget_show(switcher);
    gtk_widget_show(stack);

    return 2;
}


/** \brief  Add a child widget to the stack
 *
 * \param[in]   child   child widget
 * \param[in]   title   displayed title
 * \param[in]   name    ID to use when referencing the stack's children
 *
 */
static void add_stack_child(GtkWidget *child,
                            const gchar *title,
                            const gchar *name)
{
    gtk_stack_add_titled(GTK_STACK(stack), child, title, name);
}


/** \brief  Create push button to load the default ROMs for the current machine
 *
 * \return  GtkButton
 */
static GtkWidget *button_default_romset_load_create(void)
{
    GtkWidget *button = gtk_button_new_with_label("Load default ROMs");
    g_signal_connect(button, "clicked",
            G_CALLBACK(on_default_romset_load_clicked), NULL);
    return button;
}


static void on_kernal_browse_clicked(GtkWidget *widget, gpointer entry)
{
    char *filename;

    filename = ui_open_file_dialog(widget, "Select Kernal ROM image",
            "ROMs", rom_file_patterns, NULL);
    if (filename != NULL) {
        debug_gtk3("got ROM image '%s'\n", filename);
        vice_gtk3_resource_entry_full_update(GTK_WIDGET(entry), filename);
    }
}


static void on_basic_browse_clicked(GtkWidget *widget, gpointer entry)
{
    debug_gtk3("button clicked\n");
}

static void on_chargen_browse_clicked(GtkWidget *widget, gpointer entry)
{
    debug_gtk3("button clicked\n");
}


static GtkWidget *create_browse_button(
        void (*callback)(GtkWidget *widget, gpointer data),
        GtkWidget *entry)
{
    GtkWidget *button = gtk_button_new_with_label("Browse ...");
    g_signal_connect(button, "clicked", G_CALLBACK(callback), (gpointer)entry);
    return button;
}


static GtkWidget* create_machine_roms_widget(const romset_entry_t *roms)
{
    GtkWidget *grid;
    int row;


    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    for (row = 0; roms[row].resource != NULL; row++) {
        GtkWidget *label;
        GtkWidget *entry;
        GtkWidget *button;

        label = gtk_label_new(roms[row].label);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        entry = vice_gtk3_resource_entry_full_create(roms[row].resource);
        gtk_widget_set_hexpand(entry, TRUE);
        button = create_browse_button(roms[row].callback, entry);

        gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), entry, 1, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), button, 2, row, 1, 1);
    }

    gtk_widget_show_all(grid);
    return grid;

}



static GtkWidget *create_c64_roms_widget(void)
{
    GtkWidget *grid;
#if 0
    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid), button_default_romset_load_create(),
            0, 2, 1, 1);

    gtk_widget_show_all(grid);
#endif

    grid = create_machine_roms_widget(c64_machine_roms);

    return grid;
}


static GtkWidget *create_c64_sets_widget(void)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid),
            gtk_label_new("Here go the ROM archive handling widgets"),
            0, 2, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}



/** \brief  Create layout for C64, C64DTV, C128, SCPU64
 *
 * The C64DTV does not have the 'Drive Expansion ROM' widget.
 *
 */
static void create_c64_layout(void)
{
    GtkWidget *rom_widget;
    int row;

    row = add_stack_switcher();
    child_c64_roms = create_c64_roms_widget();
    child_c64_sets = create_c64_sets_widget();
    add_stack_child(child_c64_roms, "ROM settings", "rom-settings");
    add_stack_child(child_c64_sets, "ROM archives", "rom-archives");
}



/** \brief  Create the main ROM settings widget
 *
 * \param[in]   parent  parent widget (ignored)
 *
 * \return  GtkGrid
 */
GtkWidget *romset_widget_create(GtkWidget *parent)
{
    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 16);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:
            create_c64_layout();
            break;
        default:
            debug_gtk3("ROMset stuff not supported (yet) for %s\n", machine_name);
            break;
    }

    gtk_widget_show_all(layout);
    return layout;
}

