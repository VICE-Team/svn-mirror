/** \file   settings_digimax.c
 * \brief   Setting widget controlling DigiMAX resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES DIGIMAX         x64 x64sc xscpu64 x128 xvic
 * $VICERES DIGIMAXbase     x64 x64sc xscpu64 x128 xvic
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
#include <stdlib.h>

#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "vice_gtk3.h"

#include "settings_digimax.h"


/** \brief  Cart IO area base on C64/C128 */
#define IOBASE_C64              0xde00
/** \brief  Cart IO area base on VIC-20 */
#define IOBASE_VIC20            0x9800
/** \brief  Size of an IO area segment */
#define IOBASE_SEGMENT_SIZE     0x20
/** \brief  Number of segments on C64 */
#define IOBASE_SEGMENTS_C64     ((0xe000 - 0xde00) / IOBASE_SEGMENT_SIZE)
/** \brief  Number of segements on VIC-20 (for DigiMAX) */
#define IOBASE_SEGMENTS_VIC20   ((0x9900 - 0x9800) / IOBASE_SEGMENT_SIZE)

/* column indexes for the combo box model */
enum {
    COLUMN_ID,
    COLUMN_TEXT,
    NUM_COLUMNS
};


/** \brief  Generate model for the combo box
 *
 */
static GtkListStore *create_model(void)
{
    GtkListStore *model;
    guint base;
    guint segments;
    guint s;

    if (machine_class == VICE_MACHINE_VIC20) {
        base = IOBASE_VIC20;
        segments = IOBASE_SEGMENTS_VIC20;
    } else {
        base = IOBASE_C64;
        segments = IOBASE_SEGMENTS_C64;
    }

    model = gtk_list_store_new(NUM_COLUMNS,
                               G_TYPE_INT,      /* id */
                               G_TYPE_STRING    /* text */);
    for (s = 0; s < segments; s++) {
        GtkTreeIter iter;
        guint b;
        char text[32];

        b = base + (s * IOBASE_SEGMENT_SIZE);
        g_snprintf(text, sizeof text, "0x%04x", b);
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter,
                           COLUMN_ID, b,
                           COLUMN_TEXT, text,
                           -1);
    }
    return model;
}

/** \brief  Create combo box for the 'DIGIMAXbase' resource
 *
 * \return  GtkComboBox
 */
static GtkWidget *create_combo(void)
{
    GtkWidget *combo;
    GtkCellRenderer *renderer;

    combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(create_model()));
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo),
                                   renderer,
                                   "text", COLUMN_TEXT,
                                   NULL);
    return combo;
}

/** \brief  Get combo box ID for current selection
 *
 * \param[in]   combo   combo box
 * \param[out]  id      ID of row
 *
 * \return  `TRUE` if a selection was active
 */
static gboolean get_id(GtkComboBox *combo, int *id)
{
    if (gtk_combo_box_get_active(combo) >= 0) {
        GtkTreeModel *model;
        GtkTreeIter iter;

        model = gtk_combo_box_get_model(combo);
        if (gtk_combo_box_get_active_iter(combo, &iter)) {
            gtk_tree_model_get(model, &iter, COLUMN_ID, id, -1);
            return TRUE;
        }
    }
    return FALSE;
}

/** \brief  Set combo box ID
 *
 * \param[in]   combo   combo box
 * \param[in]   id      ID for the selection
 *
 * \return  `TRUE` if \a id was valid
 */
static gboolean set_id(GtkComboBox *combo, int id)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_combo_box_get_model(combo);
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            int current;

            gtk_tree_model_get(model, &iter, COLUMN_ID, &current, -1);
            if (id == current) {
                gtk_combo_box_set_active_iter(combo, &iter);
                return TRUE;
            }
        } while (gtk_tree_model_iter_next(model, &iter));
    }
    return FALSE;
}

/** \brief  Handler for the "changed" event of the DIGImax combo box
 *
 * \param[in]   widget      combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_changed(GtkWidget *widget, gpointer user_data)
{
    int id = 0;

    if (get_id(GTK_COMBO_BOX(widget), &id)) {
        resources_set_int("DIGIMAXbase", id);
    }
}


/** \brief  Create DIGIMAX widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *settings_digimax_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *digimax;
    GtkWidget *combo;
    int current = 0;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    digimax = vice_gtk3_resource_check_button_new("DIGIMAX", "Enable DigiMAX");
    gtk_grid_attach(GTK_GRID(grid), digimax, 0, 0, 2, 1);

    label = gtk_label_new("DigiMAX I/O base");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    combo = create_combo();
    resources_get_int("DIGIMAXbase", &current);
    set_id(GTK_COMBO_BOX(combo), current);
    g_signal_connect(combo, "changed", G_CALLBACK(on_combo_changed), NULL);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
