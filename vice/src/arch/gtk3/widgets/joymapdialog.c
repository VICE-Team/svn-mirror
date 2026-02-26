/** \file   joymapdialog.c
 * \brief   Widget to set joystick joymap mappings
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  groepaz <groepaz@gmx.net>
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


#define DEBUG_JOYMAPDIALOG

/* TODO: (some of this must be done in settings_joymap.c)
 *  - handle (re)mapping for keyboard/keypresses
 *    - create the widget to change kbd row/column/flag
 *    - instead of manually choosing row/column, pressing the respective key
 *      should select the correct value (needs some stunts via keymap code)
 *  - handle (re)mapping for POT axis
 *    - create widget for changing the POT mapping
 */


#include "vice.h"

#include <gtk/gtk.h>

#include "basewidgets.h"
#include "debug_gtk3.h"
#include "log.h"
#include "resources.h"
#include "widgethelpers.h"
#include "ui.h"
#include "uiactions.h"
#include "kbd.h"

#include "joystick.h"
#include "joymapdialog.h"

#ifdef DEBUG_JOYMAPDIALOG
#define DBG(x)  log_printf x
#else
#define DBG(x)
#endif

static void (*user_callback)(joystick_device_t*);
static joystick_device_t *cached_joydev = NULL;

static GtkWidget *top_grid;
static GtkWidget *combobox_types;
static GtkWidget *value_grid;
static GtkWidget *cpvalue;
static GtkWidget *actionvalue;
static GtkWidget *todolabel;

/** \brief get actual ui-action id from the index in the dropdown box
 */
static int actionvalue_get_action_by_index(int index)
{
    int n = 0;
    ui_action_info_t *list;
    const ui_action_info_t *action;
    list = ui_action_get_info_list();
    for (action = list; action->id > ACTION_NONE; action++) {
        if (n == index) {
            return action->id;
        }
        n++;
    }
    return -1;
}

/** \brief apply selected mapping from dialog to the mapping of the joymap
 */
static void set_mapping(joystick_mapping_t *mapping)
{
    int index, value;
    index = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox_types));
    DBG(("set_mapping action %u->%d", mapping->action, index));
    mapping->action = index;
    switch (mapping->action) {
        case JOY_ACTION_JOYSTICK:
                value = gtk_combo_box_get_active(GTK_COMBO_BOX(cpvalue));
                DBG(("set_mapping (joystick) value %d->%d", mapping->value.joy_pin, 1 << value));
                mapping->value.joy_pin = 1 << value;
            break;
        case JOY_ACTION_KEYBOARD:
                /* TODO: get values from dialog, apply to key mapping */
                DBG(("TODO: set_mapping (keyboard) row:%d column:%d flags:%02x ->",
                       mapping->value.key[0], mapping->value.key[1], (unsigned int)mapping->value.key[2]));
                log_warning(LOG_DEFAULT, "Mapping for key presses axes is not implemented.");
            break;
        case JOY_ACTION_UI_FUNCTION:
                value = actionvalue_get_action_by_index(gtk_combo_box_get_active(GTK_COMBO_BOX(actionvalue)));
                DBG(("set_mapping (ui action) value %d->%d", mapping->value.ui_action, value));
                if (value > -1) {
                    mapping->value.ui_action = value;
                }
            break;
        case JOY_ACTION_POT_AXIS:
                /* TODO: get values from dialog, apply to POT mapping */
                log_warning(LOG_DEFAULT, "Mapping for POT axes is not implemented.");
            break;
            /* "Map" and "Activate UI" have no value */
        case JOY_ACTION_MAP:
            /* fall through */
        case JOY_ACTION_UI_ACTIVATE:
            /* fall through */
        default:
            break;
    }
}

/** \brief destroy all widgets in the value grid
 */
static void destroy_value_widgets(void)
{
    DBG(("destroy_value_widgets"));
    if (cpvalue != NULL) {
        gtk_widget_destroy(cpvalue);
    }
    if (actionvalue != NULL) {
        gtk_widget_destroy(actionvalue);
    }
    if (todolabel != NULL) {
        gtk_widget_destroy(todolabel);
    }
    cpvalue = NULL;
    actionvalue = NULL;
    todolabel = NULL;
    DBG(("destroy_value_widgets done"));
}

/** \brief destroy all widgets when closing the dialog
 */
static void destroy_all(void)
{
    DBG(("destroy_all"));

    destroy_value_widgets();

    if (value_grid) {
        DBG(("value_grid: %p", (void*)value_grid));
        gtk_widget_destroy(GTK_WIDGET(value_grid));
    }
    value_grid = NULL;

    if (combobox_types) {
        DBG(("combobox_types: %p", (void*)combobox_types));
        gtk_widget_destroy(GTK_WIDGET(combobox_types));
    }
    combobox_types = NULL;

    if (top_grid) {
        DBG(("top_grid: %p", (void*)top_grid));
        gtk_widget_destroy(GTK_WIDGET(top_grid));
    }
    top_grid = NULL;

    DBG(("destroy_all done"));
}

/** \brief  Handler for the 'response' event of the dialog
 *
 * \param[in,out]   dialog      dialog triggering the event
 * \param[in]       response_id response ID
 * \param[in]       user_data   extra event data (unused)
 */
static void on_response(GtkDialog *dialog,
                        gint response_id,
                        gpointer user_data)
{
    joystick_mapping_t *mapping = (joystick_mapping_t *)user_data;
    DBG(("mapping dialog on_response"));
    switch (response_id) {
        case GTK_RESPONSE_ACCEPT:
            set_mapping(mapping);
            if (cached_joydev) {
                user_callback(cached_joydev);
                cached_joydev = NULL;
            }
            /* fall through */

        case GTK_RESPONSE_REJECT:
            destroy_all();
            gtk_widget_destroy(GTK_WIDGET(dialog));
            break;

        default:
            debug_gtk3("Unhandled response ID %d", response_id);
            break;
    }
    DBG(("mapping dialog on_response done"));
}

/** \brief  Create left-aligned label using Pango markup
 *
 * \param[in]   text    text using Pango markup
 *
 * \return  GtkLabel
 */
static GtkWidget *label_helper(const char *text)
{
    GtkWidget *label = gtk_label_new(NULL);

    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_label_set_markup(GTK_LABEL(label), text);
    return label;
}

/** \brief create "control port" values drop down list (create)
 */
static GtkListStore *cpvalue_combo_model_new(void)
{
    GtkListStore *model;
    int           index;

    const char *types[] = {
        "up", "down", "left", "right", "fire", "fire2", "fire3",
        NULL
    };

    model = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    for (index = 0; types[index] != NULL; index++) {
        GtkTreeIter        iter;
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter, 0, 1 << index, 1, types[index], -1);
    }
    return model;
}

static void cpvalue_select_by_joypin(int joypin)
{
    int index = 0;
    for (index = 0; index < 7; index++) {
        if ((1 << index) == joypin) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(cpvalue), index);
            return;
        }
    }
}


/** \brief create "control port" values drop down list
 */
static GtkWidget *cpvalue_combo_new(joystick_mapping_t *mapping)
{
    GtkWidget       *combo;
    GtkListStore    *model;
    GtkCellRenderer *renderer;

    combo    = gtk_combo_box_new();
    model    = cpvalue_combo_model_new();
    renderer = gtk_cell_renderer_text_new();

    gtk_combo_box_set_model(GTK_COMBO_BOX(combo), GTK_TREE_MODEL(model));
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer, "text", 1, NULL);
#if 0
    g_signal_connect(G_OBJECT(combo),
                     "changed",
                     G_CALLBACK(on_type_changed),
                     NULL);
#endif
    return combo;
}

/** \brief create "action" values drop down list
 */
static GtkListStore *actionvalue_combo_model_new(void)
{
    GtkListStore *model;
    ui_action_info_t *list;
    const ui_action_info_t *action;

    list = ui_action_get_info_list();

    model = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    for (action = list; action->id > ACTION_NONE; action++) {
        GtkTreeIter        iter;
        /*DBG(("id:%d name:%s", action->id, action->name));*/
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter, 0, action->id, 1, action->name, -1);
    }
    return model;
}

static void actionvalue_select_by_action(int id)
{
    ui_action_info_t *list;
    const ui_action_info_t *action;
    int index = 0;
    DBG(("actionvalue_select_by_action: %d", id));
    list = ui_action_get_info_list();
    for (action = list; action->id > ACTION_NONE; action++) {
        if (action->id == id) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(actionvalue), index);
            return;
        }
        index++;
    }
}

/** \brief "action" values drop down list (create)
 */
static GtkWidget *actionvalue_combo_new(joystick_mapping_t *mapping)
{
    GtkWidget       *combo;
    GtkListStore    *model;
    GtkCellRenderer *renderer;

    combo    = gtk_combo_box_new();
    model    = actionvalue_combo_model_new();
    renderer = gtk_cell_renderer_text_new();

    gtk_combo_box_set_model(GTK_COMBO_BOX(combo), GTK_TREE_MODEL(model));
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer, "text", 1, NULL);
#if 0
    g_signal_connect(G_OBJECT(combo),
                     "changed",
                     G_CALLBACK(on_action_changed),
                     NULL);
#endif
    return combo;
}

/** \brief creates the widget to alter the value of the mapping
 */
static GtkWidget *create_value_widget(GtkWidget *grid, joystick_mapping_t *mapping, int action)
{
    DBG(("create_value_widget action:%d", action));
    /* first destroy all existing value widgets */
    destroy_value_widgets();
    DBG(("create_value_widget destroy done"));

    switch (action) {
        case JOY_ACTION_JOYSTICK:
                cpvalue = cpvalue_combo_new(mapping);
                gtk_grid_attach(GTK_GRID(grid), cpvalue, 0, 0, 1, 1);
            break;
        case JOY_ACTION_KEYBOARD:
                /* TODO */
                todolabel = label_helper("<b>TODO</b>");
                gtk_grid_attach(GTK_GRID(grid), todolabel, 0, 0, 1, 1);
            break;
        case JOY_ACTION_UI_FUNCTION:
                actionvalue = actionvalue_combo_new(mapping);
                gtk_grid_attach(GTK_GRID(grid), actionvalue, 0, 0, 1, 1);
            break;
        case JOY_ACTION_POT_AXIS:
                /* TODO */
                todolabel = label_helper("<b>TODO</b>");
                gtk_grid_attach(GTK_GRID(grid), todolabel, 0, 0, 1, 1);
            break;
        /* no value for "map" and "activate ui" */
        case JOY_ACTION_MAP:
            /* fall through */
        case JOY_ACTION_UI_ACTIVATE:
            /* fall through */
        default:
                todolabel = label_helper("n/a");
                gtk_grid_attach(GTK_GRID(grid), todolabel, 0, 0, 1, 1);
            break;
    }
    DBG(("create_value_widget done"));
    return grid;
}

/** \brief  mapping type drop down list (create)
 */
static GtkListStore *type_combo_model_new(void)
{
    GtkListStore *model;
    int           index;

    const char *types[JOY_ACTION_MAX + 2] = {
        "none",         /* JOY_ACTION_NONE */
        "control port", /* JOY_ACTION_JOYSTICK */
        "keyboard",     /* JOY_ACTION_KEYBOARD */
        "map",          /* JOY_ACTION_MAP */
        "activate ui",  /* JOY_ACTION_UI_ACTIVATE */
        "action",       /* JOY_ACTION_UI_FUNCTION */
        "pot axis",     /* JOY_ACTION_POT_AXIS */
        NULL
    };

    model = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    for (index = 0; types[index] != NULL; index++) {
        GtkTreeIter        iter;
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter, 0, index, 1, types[index], -1);
    }
    return model;
}


/** \brief mapping type drop down list (changed)
 */
static void on_type_changed(GtkComboBox *self, gpointer data)
{
    GtkTreeIter iter;
    joystick_mapping_t *mapping = (joystick_mapping_t *)data;

    DBG(("on_type_changed: mapping:%p", (void*)mapping));

    if (gtk_combo_box_get_active_iter(self, &iter)) {
        GtkTreeModel *model = gtk_combo_box_get_model(self);
        int           index = -1;

        gtk_tree_model_get(model, &iter, 0, &index, -1);
        DBG(("Got index %d for type", index));
        /* destroy the old widget first */
        if (value_grid != NULL) {
            destroy_value_widgets();
            gtk_widget_destroy(value_grid);
            value_grid = NULL;
        }
        /* create new widget for the selected mapping type */
        value_grid = vice_gtk3_grid_new_spaced(1, 1);
        create_value_widget(GTK_WIDGET(value_grid), mapping, index);
        gtk_grid_attach(GTK_GRID(top_grid), GTK_WIDGET(value_grid), 1, 2, 1, 1);
        gtk_widget_show_all(top_grid);
    }

    DBG(("on_type_changed done"));
}

/** \brief  mapping type drop down list (create)
 */
static GtkWidget *type_combo_new(joystick_mapping_t *mapping)
{
    GtkWidget       *combo;
    GtkListStore    *model;
    GtkCellRenderer *renderer;

    combo    = gtk_combo_box_new();
    model    = type_combo_model_new();
    renderer = gtk_cell_renderer_text_new();

    gtk_combo_box_set_model(GTK_COMBO_BOX(combo), GTK_TREE_MODEL(model));
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer, "text", 1, NULL);
    g_signal_connect(G_OBJECT(combo),
                     "changed",
                     G_CALLBACK(on_type_changed),
                     mapping);
    return combo;
}

/** \brief  Create content widget for the dialog
 *
 * \return  GtkGrid
 */
static GtkWidget *create_content_widget(joystick_device_t *joydev, joystick_mapping_t *mapping, const char *input, const char *name)
{
    GtkWidget *label;
    GtkWidget *grid;
    int row = 0;
    char buf[0x100];

    DBG(("create_content_widget"));

    /* setup grid with all buttons the same size */
    top_grid = grid = vice_gtk3_grid_new_spaced(2, 3);
    gtk_widget_set_margin_top(grid, 16);
    gtk_widget_set_margin_start(grid, 16);
    gtk_widget_set_margin_end(grid, 16);
    gtk_widget_set_margin_bottom(grid, 16);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);

    label = label_helper("<b>Input:</b>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    sprintf(buf, "%s (%s)", name, input);
    label = label_helper(buf);
    gtk_grid_attach(GTK_GRID(grid), label, 1, row, 1, 1);
    row++;

    label = label_helper("<b>Mapping Type:</b>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    combobox_types = type_combo_new(mapping);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_types), mapping->action);
    gtk_grid_attach(GTK_GRID(grid), combobox_types, 1, row, 1, 1);
    row++;

    label = label_helper("<b>Value:</b>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    value_grid = vice_gtk3_grid_new_spaced(1, 1);
    create_value_widget(GTK_WIDGET(value_grid), mapping, mapping->action);
    cpvalue_select_by_joypin(mapping->value.joy_pin);
    actionvalue_select_by_action(mapping->value.ui_action);
    gtk_grid_attach(GTK_GRID(grid), value_grid, 1, row, 1, 1);
    row++;

    gtk_widget_show_all(grid);

    DBG(("create_content_widget done"));

    return grid;
}

/** \brief  Handle "key pressd" event
 */
static gboolean on_key_pressed(GtkWidget *widget, GdkEventKey *event,
        gpointer data)
{
    guint key = kbd_fix_keyval((GdkEvent*)event);
    /* TODO: translate the pressed key to the right col/row and apply that */
    log_warning(LOG_DEFAULT, "setting key mapping via key presses is not implemented.");
    DBG(("pressed key value (fixed): %04x", key));
    return FALSE;
}

/** \brief  Show dialog to configure joymaps
 *
 * \param[in]   joydev  device
 */
void joymap_dialog_show(joystick_device_t *joydev, joystick_mapping_t *mapping, const char *input, const char *name, void (*callback)(joystick_device_t*))
{
    GtkWidget *dialog;
    GtkWidget *content;
    gchar title[256];

    user_callback = callback;
    cached_joydev = joydev;

    /* create title */
    g_snprintf(title, sizeof(title), "Configure mapping");

    /*
     * don't use ui_get_active_window() for the parent, that will break
     * the blocking of this dialog of other dialogs and allows the main settings
     * dialog to move in front of this one (which should not happen)
     */
    dialog = gtk_dialog_new_with_buttons(
            title, ui_get_active_window(), GTK_DIALOG_MODAL,
            "OK", GTK_RESPONSE_ACCEPT,
            "Cancel", GTK_RESPONSE_REJECT,
            NULL);

    /* add the joymap buttons */
    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_box_pack_start(GTK_BOX(content), create_content_widget(joydev, mapping, input, name), TRUE, TRUE, 16);

    /* connect key events handler */
    g_signal_connect(dialog, "key-press-event", G_CALLBACK(on_key_pressed), NULL);

    /* connect reponse handler and show dialog */
    g_signal_connect(dialog, "response",
                     G_CALLBACK(on_response),
                     mapping);
    gtk_widget_show_all(dialog);
}
