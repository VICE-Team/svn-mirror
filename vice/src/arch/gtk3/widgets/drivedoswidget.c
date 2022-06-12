/** \file   drivedoswidget.c
 * \brief   Drive DOS expansions widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES Drive8ProfDOS       x64 x64sc xscpu64 x128
 * $VICERES Drive9ProfDOS       x64 x64sc xscpu64 x128
 * $VICERES Drive10ProfDOS      x64 x64sc xscpu64 x128
 * $VICERES Drive11ProfDOS      x64 x64sc xscpu64 x128
 * $VICERES Drive8StarDos       x64 x64sc xscpu64 x128
 * $VICERES Drive9StarDos       x64 x64sc xscpu64 x128
 * $VICERES Drive10StarDos      x64 x64sc xscpu64 x128
 * $VICERES Drive11StarDos      x64 x64sc xscpu64 x128
 * $VICERES Drive8SuperCard     x64 x64sc xscpu64 x128
 * $VICERES Drive9SuperCard     x64 x64sc xscpu64 x128
 * $VICERES Drive10SuperCard    x64 x64sc xscpu64 x128
 * $VICERES Drive11SuperCard    x64 x64sc xscpu64 x128
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
 */

#include "vice.h"
#include <gtk/gtk.h>

#include "drive-check.h"
#include "drive.h"
#include "machine.h"
#include "resources.h"
#include "vice_gtk3.h"

#include "drivedoswidget.h"


/** \brief  Create DOS expansion check button
 *
 * \param[in]   unit    unit number (8-11)
 * \param[in]   dos     final part of the resource name (ie 'ProfDOS')
 * \param[in]   label   label for the check button
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_dos_check_button(int unit,
                                          const char *dos,
                                          const char *label)
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new_sprintf(
            "Drive%d%s", label, unit, dos);
    gtk_widget_set_margin_start(check, 16);
    return check;
}


/** \brief  Create drive DOS widget
 *
 * Create widget to select a DOS expansion for \a unit.
 *
 * \param[in]   unit    drive unit (8-11)
 *
 * \return  GtkGrid
 */
GtkWidget *drive_dos_widget_create(int unit)
{
    GtkWidget *grid;
    GtkWidget *profdos;
    GtkWidget *stardos;
    GtkWidget *supercard;
    int model = drive_get_disk_drive_type(unit - DRIVE_UNIT_MIN);

    grid = vice_gtk3_grid_new_spaced_with_label(-1, -1, "DOS expansions", 1);
    g_object_set_data(G_OBJECT(grid), "UnitNumber", GINT_TO_POINTER(unit));

    profdos = create_dos_check_button(unit, "ProfDos", "Professional DOS");
    stardos = create_dos_check_button(unit, "StarDOS", "StarDOS");
    supercard = create_dos_check_button(unit, "Supercard", "Supercard+");

    /* enable/disable widgets based on drive model */
    gtk_widget_set_sensitive(profdos, drive_check_profdos(model));
    gtk_widget_set_sensitive(stardos, drive_check_stardos(model));
    gtk_widget_set_sensitive(supercard, drive_check_supercard(model));

    gtk_grid_attach(GTK_GRID(grid), profdos, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stardos, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), supercard, 0, 3, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}



/*****************************************************************************
 * Reimplementation using a combo box. The model will be updated on drive    *
 * type change to only show the DOS expansions valid for the current drive   *
 * type.                                                                     *
 ****************************************************************************/

/** \brief  Combo box model columns */
enum {
    COL_NAME,   /**< expansion name */
    COL_INDEX,  /**< index in the list of expansions */
    COL_COUNT   /**< number of columns in the model */
};


/** \brief  DOS expansion entry in the list
 */
typedef struct drive_dos_exp_s {
    const char *name;       /**< expansion name */
    const char *resource;   /**< resource (printf format string) */
    int (*valid)(int);      /**< function to test if the expansion is valid for
                                 the current drive type (NULL == always valid) */
} drive_dos_exp_t;


/** \brief  List of possible expansions, terminated by .name == NULL */
static const drive_dos_exp_t expansions[] = {
    { "None",               NULL,               NULL },
    { "Professional DOS",   "Drive%dProfDOS",   drive_check_profdos },
    { "StarDOS",            "Drive%dStarDOS",   drive_check_stardos },
    { "Supercard+",         "Drive%dSuperCard", drive_check_supercard },
    { NULL,                 NULL,               NULL }
};


/* Forward declarations */
static int get_unit_number(GtkWidget *self);
static int get_drive_type(GtkWidget *self);


static void on_combo_changed(GtkWidget *self, gpointer data)
{
    debug_gtk3("Called.");

    if (gtk_combo_box_get_active(GTK_COMBO_BOX(self)) >= 0) {
        GtkTreeModel *model;
        GtkTreeIter iter;

        /* enable the correct resource, disable the others */
        model = gtk_combo_box_get_model(GTK_COMBO_BOX(self));
        if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(self), &iter)) {
            gchar *name;
            int index;
            int unit;

            unit = get_unit_number(self);
            gtk_tree_model_get(model, &iter,
                               COL_NAME, &name,
                               COL_INDEX, &index,
                               -1);

            if (expansions[index].resource != NULL) {
                debug_gtk3("Enabling %s for unit #%d.", name, unit);
                resources_set_int_sprintf(expansions[index].resource, 1, unit);
            }

            /* disable other expansions */
            for (int i = 0; expansions[i].name != NULL; i++) {
                if (expansions[i].resource != NULL && i != index) {
                    debug_gtk3("Disabling %s for unit #%d.", expansions[i].name, unit);
                    resources_set_int_sprintf(expansions[i].resource, 0, unit);
                }
            }
            g_free(name);
        }
    }
}


/** \brief  Get drive unit number
 *
 * \param[in]   self    combo box
 *
 * \return  unit number
 */
static int get_unit_number(GtkWidget *self)
{
    return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(self), "UnitNumber"));
}


/** \brief  Get drive type (called model in the UI)
 *
 * \param[in]   self    combo box
 *
 * \return  drive type
 */
static int get_drive_type(GtkWidget *self)
{
    int unit;
    int type;

    unit = get_unit_number(self);
    resources_get_int_sprintf("Drive%dType", &type, unit);
    return type;
}


static GtkListStore *create_combo_model(GtkWidget *self)
{
    GtkListStore *model;
    GtkTreeIter iter;
    int type = 0;

    type = get_drive_type(self);

    model = gtk_list_store_new(COL_COUNT, G_TYPE_STRING, G_TYPE_INT);
    for (int i = 0; expansions[i].name != NULL; i++) {
        /* Only add item if valid for current drive model */
        if (expansions[i].valid == NULL || expansions[i].valid(type)) {
            gtk_list_store_append(model, &iter);
            gtk_list_store_set(model, &iter,
                               COL_NAME, expansions[i].name,
                               COL_INDEX, i,
                               -1);
        }
    }
    /* set the drive model of the current combo box model */
    g_object_set_data(G_OBJECT(self), "ModelDriveType", GINT_TO_POINTER(type));
    return model;
}


/** \brief  Create combo box to select drive DOS expansion
 *
 * Create combo box that allows selecting a DOS expansion that is valid for the
 * current \a unit's drive type.
 *
 * Contains the following GObject keys:
 *  - UnitNumber:       Drive unit number (8-11)
 *  - ModelDriveType:   Drive type used for (re)construction of the model of the
 *                      combo box, to avoid rebuilding the model on every sync()
 *                      call
 *  - ChangedHandlerID: ID of the 'changed' event handler, used to block the
 *                      changed event when calling sync()
 *
 * \param[in]   unit    drive unit number (8-11)
 *
 *
 * \return  GtkComboBox
 */
GtkWidget *drive_dos_widget_create_combo(int unit)
{
    GtkWidget *combo;
    GtkListStore *model;
    GtkCellRenderer *renderer;
    gulong handler_id;

    combo = gtk_combo_box_new();
    /* this needs to be set right away so the private methods can use it */
    g_object_set_data(G_OBJECT(combo), "UnitNumber", GINT_TO_POINTER(unit));
    g_object_set_data(G_OBJECT(combo), "ModelDriveType", GINT_TO_POINTER(get_drive_type(combo)));

    model = create_combo_model(combo);
    gtk_combo_box_set_model(GTK_COMBO_BOX(combo), GTK_TREE_MODEL(model));

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo),
                                   renderer,
                                   "text", 0,
                                   NULL);
    handler_id = g_signal_connect(combo, "changed", G_CALLBACK(on_combo_changed), NULL);
    g_object_set_data(G_OBJECT(combo), "ChangedHandlerID", GULONG_TO_POINTER(handler_id));

    /* select proper combo box item */
    drive_dos_widget_sync_combo(combo);
    gtk_widget_show_all(combo);
    return combo;
}


/** \brief  Disable invalid DOS expansions
 *
 * Disable all DOS expansions that are invalid for the current drive model.
 *
 * \param[in]   self    drive DOS expansion widget
 */
static void disable_invalid_expansions(GtkWidget *self)
{
    int unit = get_unit_number(self);
    int type = get_drive_type(self);

    debug_gtk3("Disabling invalid expansions for new drive type %d:", type);
    for (int i = 0; expansions[i].name != NULL; i++) {
        if (expansions[i].resource != NULL) {
            if (!expansions[i].valid(type)) {
                debug_gtk3("Setting %s to disabled.", expansions[i].name);
                resources_set_int_sprintf(expansions[i].resource, 0, unit);
            }
        }
    }
}


/** \brief  Synchronize \a widget with the current machine configuration
 *
 * Update the available expansions in the combo box, disable any invalid DOS
 * expansions for the current drive model.
 *
 * \param[in]   widget  drive DOS expansion widget
 */
void drive_dos_widget_sync_combo(GtkWidget *widget)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    gulong handler_id;
    int unit;
    int curr_type;
    int model_type;

    unit = get_unit_number(widget);
    curr_type = get_drive_type(widget);
    model_type = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "ModelDriveType"));

    /* regenerate model and disable now-invalid DOS expansions */
    if (curr_type != model_type) {
        GtkListStore *new_model;

        debug_gtk3("New drive type %d doesn't match the model's type %d.", curr_type, model_type);
        disable_invalid_expansions(widget);

        new_model = create_combo_model(widget);
        gtk_combo_box_set_model(GTK_COMBO_BOX(widget), GTK_TREE_MODEL(new_model));
    }

    /*
     * Select the correct item now
     */
    handler_id = GPOINTER_TO_ULONG(g_object_get_data(G_OBJECT(widget), "ChangedHandlerID"));
    g_signal_handler_block(G_OBJECT(widget), handler_id);
    /* select None by default */
    gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

    /* iterate items, checking resources for each to determine which one to
     * activate */
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(widget));
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            gchar *name;
            int index;
            int enabled;

            gtk_tree_model_get(model, &iter,
                               COL_NAME, &name,
                               COL_INDEX, &index,
                               -1);

            debug_gtk3("Name '%s', Index %d.", name, index);
            if (expansions[index].resource != NULL) {
                resources_get_int_sprintf(expansions[index].resource, &enabled, unit);
                if (enabled) {
                    debug_gtk3("Selecting DOS expansion '%s'.", name);
                    gtk_combo_box_set_active_iter(GTK_COMBO_BOX(widget), &iter);
                    g_free(name);
                    break;
                }
            }
            g_free(name);
        } while (gtk_tree_model_iter_next(model, &iter));
    }
    g_signal_handler_unblock(G_OBJECT(widget), handler_id);
}
