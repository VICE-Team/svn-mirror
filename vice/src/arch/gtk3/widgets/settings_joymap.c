/** \file   settings_joymap.c
 * \brief   Widget to edit joystick mappings
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  groepaz <groepaz@gmx.net>
 */


#define DEBUG_JOYMAPSETTINGS

/* TODO:
 *  - in the input tab, the list of axis/buttons/hats should automatically
 *    scroll so the currently pressed one is actually visible
 *  - the list of axis/buttons/hats should resize when the dialog is resized
 *  - implement the calibration tab
 *
 * Some places of the GUI will have to explicitly poll the controllers to make
 * some features work as expected:
 *
 *  - mapping a button to "toggle-pause" works, pressing the button to "pause"
 *    works, but pressing the button to unpause does not
 *  - mapping a button to "activate ui" works, but pressing it to close the
 *    menu again does not
 */

#include "vice.h"
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdint.h>

#include "log.h"
#include "mainlock.h"
#include "joyport.h"
#include "joystick.h"
#include "resources.h"
#include "util.h"
#include "joybuttonledwidget.h"
#include "joyhatledwidget.h"
#include "vice_gtk3.h"
#include "vsync.h"

#include "uiactions.h"
#include "settings_joymap.h"
#include "joymapdialog.h"

#ifdef DEBUG_JOYMAPSETTINGS
#define DBG(x)  log_printf x
#else
#define DBG(x)
#endif

static GtkWidget *event_widget_new(joystick_device_t *joydev);
static GtkWidget *mapping_widget_new(joystick_device_t *joydev);
static GtkWidget *calibration_widget_new(joystick_device_t *joydev);

/* TODO: Remove this hack that was added to make avoiding crash on second dialog open easier */
typedef struct global_widgets_s {
    /* parent widgets for the tabbed dialog pages */
    GtkWidget *inputs_grid;
    GtkWidget *mappings_grid;
    GtkWidget *calibration_grid;

    GtkWidget *inputs_axis;
    GtkWidget *inputs_buttons;
    GtkWidget *inputs_hats;

    GtkWidget *layout;
    GtkWidget *device_combo;

    GtkWidget *event_widget;
    GtkWidget *mapping_widget;
    GtkWidget *calibration_widget;
} global_widgets_t;
static global_widgets_t global_widgets;

static guint poll_timeout_id;

/** \brief  Columns for the mappings table
 */
enum {
    COL_INDEX,          /**< column ID/index (integer) */
    COL_ID,             /**< input ID (integer) */
    COL_INPUT_TYPE,     /**< input type (string) */
    COL_INPUT_NAME,     /**< input name (string) */
    COL_MAPPED_TYPE,    /**< mapping type (string) */
    COL_MAPPED_VAL,     /**< mapped value (string) */
};

static gboolean poll_callback(gpointer joydev)
{
    mainlock_assert_is_not_vice_thread();
    joystick_ui_poll();
    return G_SOURCE_CONTINUE;
}

static void start_polling(joystick_device_t *joydev)
{
    mainlock_assert_is_not_vice_thread();
    if (joystick_ui_poll_setup(joydev)) {
        poll_timeout_id = g_timeout_add(50, poll_callback, (gpointer)joydev);
        joydev->status |= JOY_REOPEN_MAIN;
    }
}

static void stop_polling(void)
{
    if (poll_timeout_id > 0) {
        g_source_remove(poll_timeout_id);
        poll_timeout_id = 0;
    }
}


/** \brief  Create left-aligned label
 *
 * \param[in]   markup  label text using Pango markup
 *
 * \return  new GtkLabel
 */
static GtkWidget *label_helper(const char *markup)
{
    GtkWidget *label = gtk_label_new(NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    /*DBG(("label_helper(%s): %p", markup, label));*/
    return label;
}

static GtkWidget *grid_helper(const char *markup, int colspacing, int rowspacing, int colspan)
{
    GtkWidget *grid  = gtk_grid_new();
    GtkWidget *label = label_helper(markup);

    gtk_grid_set_column_spacing(GTK_GRID(grid), colspacing);
    gtk_grid_set_row_spacing(GTK_GRID(grid), rowspacing);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, colspan, 1);

    return grid;
}

/* host device drop down list (changed) */
static void on_device_changed(GtkComboBox *self, gpointer data)
{
    GtkTreeIter iter;

    stop_polling();
    if (gtk_combo_box_get_active_iter(self, &iter)) {
        GtkTreeModel *model = gtk_combo_box_get_model(self);
        int           index = -1;

        gtk_tree_model_get(model, &iter, 0, &index, -1);
        debug_gtk3("Got index %d for device", index);

        /* create 'inputs' tab */
        if (global_widgets.event_widget != NULL) {
            gtk_widget_destroy(global_widgets.event_widget);
        }
        global_widgets.event_widget = event_widget_new(joystick_device_by_index(index));
        /* attach events widget to 'inputs' tab */
        gtk_grid_attach(GTK_GRID(global_widgets.inputs_grid), global_widgets.event_widget, 0, 2, 2, 1);

        /* create 'mappings' tab */
        if (global_widgets.mapping_widget != NULL) {
            gtk_widget_destroy(global_widgets.mapping_widget);
        }
        global_widgets.mapping_widget = mapping_widget_new(joystick_device_by_index(index));
        gtk_grid_attach(GTK_GRID(global_widgets.mappings_grid), global_widgets.mapping_widget, 0, 2, 2, 1);

        /* create 'calibration' tab */
        if (global_widgets.calibration_widget != NULL) {
            gtk_widget_destroy(global_widgets.calibration_widget);
        }
        global_widgets.calibration_widget = calibration_widget_new(joystick_device_by_index(index));
        gtk_grid_attach(GTK_GRID(global_widgets.calibration_grid), global_widgets.calibration_widget, 0, 2, 2, 1);

        gtk_widget_show_all(global_widgets.calibration_widget);
        gtk_widget_show_all(global_widgets.mapping_widget);
        gtk_widget_show_all(global_widgets.event_widget);

        start_polling(joystick_device_by_index(index));
    }
}

/* host device drop down list (add device) */
static GtkListStore *device_combo_model_new(void)
{
    GtkListStore *model;
    int           index;

    model = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    for (index = 0; index < joystick_device_count(); index++) {
        GtkTreeIter        iter;
        joystick_device_t *joydev = joystick_device_by_index(index);
        char               buffer[256];

        g_snprintf(buffer, sizeof buffer, "%s (%d %s, %d %s, %d %s)",
                   joydev->name,
                   joydev->num_axes, joydev->num_axes == 1 ? "axis" : "axes",
                   joydev->num_buttons, joydev->num_buttons == 1 ? "button" : "buttons",
                   joydev->num_hats, joydev->num_hats == 1 ? "hat" : "hats");

        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter, 0, index, 1, buffer, -1);

        DBG(("device_combo_model_new: %p mode: %u", (void*)joydev, joydev->status & JOY_POLL_MASK));
    }
    return model;
}


/* host device drop down list (create) */
static GtkWidget *device_combo_new(void)
{
    GtkWidget       *combo;
    GtkListStore    *model;
    GtkCellRenderer *renderer;

    combo    = gtk_combo_box_new();
    model    = device_combo_model_new();
    renderer = gtk_cell_renderer_text_new();

    gtk_combo_box_set_model(GTK_COMBO_BOX(combo), GTK_TREE_MODEL(model));
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer, "text", 1, NULL);
    g_signal_connect(G_OBJECT(combo),
                     "changed",
                     G_CALLBACK(on_device_changed),
                     NULL);
    return combo;
}

/* 'input' tab, 'axis' column */
static GtkWidget *create_axes_grid(joystick_device_t *joydev)
{
    GtkWidget *grid;
    int        i;

    grid = grid_helper("<b>Axes</b>", 8, 8, 2);
    for (i = 0; i < joydev->num_axes; i++) {
        GtkWidget       *label;
        GtkWidget       *scale;
        joystick_axis_t *axis   = joydev->axes[i];
        gdouble          range  = (gdouble)(axis->maximum - axis->minimum + 1);
        gdouble          center = (gdouble)axis->minimum + range / 2.0;

        label = gtk_label_new(axis->name);
        gtk_widget_set_halign(label, GTK_ALIGN_START);

        scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
                                         (gdouble)axis->minimum,
                                         (gdouble)axis->maximum,
                                         1.0);
        gtk_scale_set_digits(GTK_SCALE(scale), 0);
        gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
        gtk_range_set_value(GTK_RANGE(scale), center);
        gtk_widget_set_size_request(scale, 200, -1);

        gtk_grid_attach(GTK_GRID(grid), label, 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), scale, 1, i + 1, 1, 1);
    }
    return grid;
}


/* 'input' tab, 'buttons' column */
static GtkWidget *create_buttons_grid(joystick_device_t *joydev)
{
    GtkWidget *grid;
    int        i;

    grid = grid_helper("<b>Buttons</b>", 8, 8, 2);
    for (i = 0; i < joydev->num_buttons; i++) {
        GtkWidget         *label;
        GtkWidget         *led;
        joystick_button_t *button = joydev->buttons[i];

        label = gtk_label_new(button->name);
        gtk_widget_set_halign(label, GTK_ALIGN_START);

        led = joy_button_led_widget_new(NULL, NULL);
        joy_button_led_widget_set_active(led, FALSE);

        gtk_grid_attach(GTK_GRID(grid), label, 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), led,   1, i + 1, 1, 1);
    }
    return grid;
}

/* 'input' tab, 'hats' column */
static GtkWidget *create_hats_grid(joystick_device_t *joydev)
{
    GtkWidget *grid;
    int        i;

    grid = grid_helper("<b>Hats</b>", 8, 8, 2);
    for (i = 0; i < joydev->num_hats; i++) {
        GtkWidget         *label;
        GtkWidget         *led;
        joystick_hat_t *hat = joydev->hats[i];

        label = gtk_label_new(hat->name);
        gtk_widget_set_halign(label, GTK_ALIGN_START);

        led = joy_hat_led_widget_new(NULL, NULL);
        joy_hat_led_widget_set_active(led, FALSE, FALSE, FALSE, FALSE);

        gtk_grid_attach(GTK_GRID(grid), label, 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), led,   1, i + 1, 1, 1);
    }
    return grid;
}

/* now the buttons at the bottom of the mapping tab */

static GtkWidget *mappings_path;
static void update_treeview_full(joystick_device_t *joydev);

/** \brief  Callback for the dialog's response handler
 *
 * Loads a mapping file, triggering parsing of file and destroys dialog.
 *
 * \param[in]   self        dialog
 * \param[in]   filename    file to load (`NULL` means cancelled)
 * \param[in]   extra       extra event data (joystick_device_t)
 */
static void load_from_callback(GtkDialog *self, gchar *filename, gpointer extra)
{
    gboolean result = FALSE;
    joystick_device_t *joydev = (joystick_device_t *)extra;

    if (filename != NULL) {
        debug_gtk3("Got filename '%s'", filename);
        if (joy_arch_mapping_load(filename, joydev) != -1) {
            result = TRUE;
        }
    }
    gtk_widget_destroy(GTK_WIDGET(self));

    if (result) {
        update_treeview_full(joydev);
    }
}

/** \brief  Handler for the 'clicked' event handler of the 'Load from' button
 *
 * \param[in]   button  button (unused)
 * \param[in]   extra   extra event data (joystick_device_t)
 */
static void on_load_from_clicked(GtkButton *button, gpointer extra)
{
    GtkWidget *dialog;
    joystick_device_t *joydev = (joystick_device_t *)extra;

    dialog = vice_gtk3_open_file_dialog("Select a mappings file to load",
                                        "Joymap files",
                                        file_chooser_pattern_joymap,
                                        NULL,
                                        load_from_callback,
                                        joydev);
    gtk_widget_show(dialog);
}

/** \brief  Call for the save-as dialog
 *
 * Save current mapping as \a path, update source path and type widgets.
 *
 * \param[in]   dialog  file dialog
 * \param[in]   path    path to save mapping to
 * \param[in]   extra   extra callback data (joystick_device_t)
 */
static void save_as_callback(GtkDialog *dialog, gchar *path, gpointer extra)
{
    joystick_device_t *joydev = (joystick_device_t *)extra;

    if (path != NULL) {
        char *fullpath;

        /* add extension if not present (cannot use util_add_extension() here
         * since that function might realloc its argument using lib_realloc()
         * and path is owned by GLib not VICE */
        fullpath = util_add_extension_const(path, "vjm");
        g_free(path);

        if (joy_arch_mapping_dump(fullpath, joydev) != -1) {
            vice_gtk3_message_info(GTK_WINDOW(dialog),
                                   "Joymap saved",
                                   "Joymap succesfully saved as '%s'.",
                                   fullpath);
        } else {
            vice_gtk3_message_error(GTK_WINDOW(dialog),
                                    "Joymap error",
                                    "Failed to save joymap as '%s'.",
                                    fullpath);
        }
        lib_free(fullpath);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

/** \brief  Handler for the 'clicked' event handler of the 'Save as' button
 *
 * \param[in]   button  button (unused)
 * \param[in]   extra   extra event data (joystick_device_t)
 */
static void on_save_as_clicked(GtkButton *button, gpointer extra)
{
    joystick_device_t *joydev = (joystick_device_t *)extra;
    GtkWidget *dialog;
    const char *filename = NULL;

    /* FIXME: set default path/filename */

    dialog = vice_gtk3_save_file_dialog("Save Joymap to file",
                                        filename,
                                        TRUE,
                                        NULL,
                                        save_as_callback,
                                        joydev);
    gtk_widget_show_all(dialog);
}

/** \brief  Handler for the 'clicked' event handler of the 'Reset' button
 *
 * \param[in]   btn     button (unused)
 * \param[in]   extra   extra event data (joystick_device_t)
 */
static void on_reset_clicked(GtkButton *btn, gpointer extra)
{
    int i;
    joystick_device_t *joydev = (joystick_device_t *)extra;

    /* FIXME: we might want to use some API function for this */

    /* clear mapping */
    for (i = 0; i < joydev->num_buttons; i++) {
        joystick_button_t *button   = joydev->buttons[i];
        button->mapping.action = JOY_ACTION_NONE;
    }
    for (i = 0; i < joydev->num_axes; i++) {
        joystick_axis_t *axis   = joydev->axes[i];
        axis->mapping.negative.action = JOY_ACTION_NONE;
        axis->mapping.positive.action = JOY_ACTION_NONE;
    }
    for (i = 0; i < joydev->num_hats; i++) {
        joystick_hat_t *hat   = joydev->hats[i];
        hat->mapping.up.action = JOY_ACTION_NONE;
        hat->mapping.down.action = JOY_ACTION_NONE;
        hat->mapping.left.action = JOY_ACTION_NONE;
        hat->mapping.right.action = JOY_ACTION_NONE;
    }

    update_treeview_full(joydev);
}

/** \brief  Handler for the 'clicked' event handler of the 'Load default' button
 *
 * \param[in]   button  button (unused)
 * \param[in]   extra   extra event data (joystick_device_t)
 */
static void on_defaults_clicked(GtkButton *button, gpointer extra)
{
    joystick_device_t *joydev = (joystick_device_t *)extra;
    GtkWidget *parent;
    const char *filename = NULL;

    resources_get_string("JoyMapFile", &filename);

    /* try to set settings dialog as parent */
    parent = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (!GTK_IS_WINDOW(parent)) {
        parent = NULL;  /* revert to active emulator window */
    }

    if (joy_arch_mapping_load(filename, NULL) != -1) {
        vice_gtk3_message_info(GTK_WINDOW(parent),
                               "Joymaps loaded",
                               "Joymaps loaded succesfully from '%s'.",
                               filename);
    } else {
        vice_gtk3_message_error(GTK_WINDOW(parent),
                                "Joymaps error",
                                "Failed to load joymaps.");
    }

    update_treeview_full(joydev);
}

/** \brief  Handler for the 'clicked' event handler of the 'Save default' button
 *
 * \param[in]   button  button (unused)
 * \param[in]   extra   extra event data (joystick_device_t)
 */
static void on_save_clicked(GtkButton *button, gpointer extra)
{
    /*joystick_device_t *joydev = (joystick_device_t *)extra;*/
    GtkWidget *parent;
    const char *filename = NULL;

    resources_get_string("JoyMapFile", &filename);

    /* try to set settings dialog as parent */
    parent = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (!GTK_IS_WINDOW(parent)) {
        parent = NULL;  /* revert to active emulator window */
    }

    if (joy_arch_mapping_dump(filename, NULL) != -1) {
        vice_gtk3_message_info(GTK_WINDOW(parent),
                               "Joymaps saved",
                               "Joymaps saved succesfully as '%s'.",
                               filename);
    } else {
        vice_gtk3_message_error(GTK_WINDOW(parent),
                                "Joymaps error",
                                "Failed to save joymaps.");
    }

}


/** \brief  Create grid with mapping file info and buttons
 *
 * \return  GtkGrid
 */
static GtkWidget *create_mapping_file_grid(joystick_device_t *joydev)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *box;
    GtkWidget *button;
    int        row = 0;
    const char *mappingfile = NULL;

    resources_get_string("JoyMapFile", &mappingfile);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("Mapping file:");
    mappings_path = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(mappings_path), FALSE);
    gtk_widget_set_hexpand(mappings_path, TRUE);
    if (mappingfile) {
        gtk_entry_set_text(GTK_ENTRY(mappings_path), mappingfile);
    }
    gtk_grid_attach(GTK_GRID(grid), label,        0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mappings_path, 1, row, 1, 1);
    row++;

    box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);

    button = gtk_button_new_with_label("Load from");
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(button),
                     "clicked",
                     G_CALLBACK(on_load_from_clicked),
                     joydev);

    button = gtk_button_new_with_label("Save as");
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(button),
                     "clicked",
                     G_CALLBACK(on_save_as_clicked),
                     joydev);

    button = gtk_button_new_with_label("Reset");
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(button),
                     "clicked",
                     G_CALLBACK(on_reset_clicked),
                     joydev);

    button = gtk_button_new_with_label("Load defaults");
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(button),
                     "clicked",
                     G_CALLBACK(on_defaults_clicked),
                     joydev);

    button = gtk_button_new_with_label("Save defaults");
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(button),
                     "clicked",
                     G_CALLBACK(on_save_clicked),
                     joydev);

    gtk_grid_attach(GTK_GRID(grid), box, 0, row, 2, 1);
    row++;

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create grid with calibration stuff in it
 *
 * \return  GtkGrid
 */
static GtkWidget *create_calibration_grid(joystick_device_t *joydev)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("TODO");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    /* TODO: calibration tab */

    gtk_widget_show_all(grid);
    return grid;
}


/* KLUDGES: using this wrapper somehow makes it oddly hard to track the contained
   child widget, without resorting to global variables */
#define USE_SCROLLED_WINDOW_HELPER

#ifdef USE_SCROLLED_WINDOW_HELPER
/** \brief  Create GtkScrolledWindow wrapping a widget
 *
 * Create scrolled window with vertical scroll bars, visible only when the
 * content doesn't fit the preset height, and add \a child as its child.
 *
 * \param[in]   child   child widget for the scrolled window
 *
 * \return  new \c GtkScrolledWindow
 */
static GtkWidget *scrolled_window_helper(GtkWidget *child)
{
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, FALSE);
    /* FIXME: can we avoid setting "fixed" width? */
    gtk_widget_set_size_request(scrolled, -1, 380);
    gtk_container_add(GTK_CONTAINER(scrolled), child);
    return scrolled;
}
#else
#define scrolled_window_helper(child) child
#endif

/* create 'inputs' tab */
/* TODO: Repaint widgets once after all events have been processed, with
 *       Gtk fram tick or so?
 */
static GtkWidget *event_widget_new(joystick_device_t *joydev)
{
    GtkWidget *grid;
    GtkWidget *agrid;
    GtkWidget *bgrid;
    GtkWidget *hgrid;

    grid  = gtk_grid_new();

    /* KLUDGES: track grids in global vars */
    global_widgets.inputs_axis = agrid = create_axes_grid(joydev);
    global_widgets.inputs_buttons = bgrid = create_buttons_grid(joydev);
    global_widgets.inputs_hats = hgrid = create_hats_grid(joydev);

    gtk_grid_attach(GTK_GRID(grid), scrolled_window_helper(agrid), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window_helper(bgrid), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window_helper(hgrid), 2, 0, 1, 1);

    return grid;
}

static GtkWidget *mappings_view;

static char *get_action_string(int action)
{
    switch (action) {
        case JOY_ACTION_NONE: return "none";
        case JOY_ACTION_JOYSTICK: return "control port";
        case JOY_ACTION_KEYBOARD: return "keyboard";
        case JOY_ACTION_MAP: return "map";
        case JOY_ACTION_UI_ACTIVATE: return "activate ui";
        case JOY_ACTION_UI_FUNCTION: return "action";
        case JOY_ACTION_POT_AXIS: return "pot";
    }
    return "invalid";
}

static char *get_flag_string(int flags)
{
    if (flags & 1) {
        return "+ shift";
    }
    return "";
}

/* FIXME: the mapping system actually would support using more than one bit,
          but the GUI does not (yet) */
static char *get_direction_string(int val)
{
    switch (val) {
        case 0x0001: return "up";
        case 0x0002: return "down";
        case 0x0004: return "left";
        case 0x0008: return "right";
        case 0x0010: return "fire";
        case 0x0020: return "fire2";
        case 0x0040: return "fire3";
        case 0x0080: return "fire4";
        default: break;
    }
    return "-";
}

static char *get_mapped_string(joystick_mapping_t *mapping)
{
    static char str[0x40];
    switch (mapping->action) {
         case JOY_ACTION_NONE:
             return "-";
         case JOY_ACTION_JOYSTICK:
         {
             sprintf(str, "0x%04x  %s", mapping->value.joy_pin,
                        get_direction_string(mapping->value.joy_pin));
             return str;
         }
         case JOY_ACTION_KEYBOARD:
         {
             /* key[0] = row, key[1] = column, key[2] = flags */
             /*sprintf(str, "row: %d col: %d flags: 0x%02x", mapping->value.key[0], mapping->value.key[1], mapping->value.key[2]);*/
             sprintf(str, "row: %d col: %d %s",
                     mapping->value.key[0],
                     mapping->value.key[1],
                     get_flag_string(mapping->value.key[2]));
             return str;
         }
         case JOY_ACTION_MAP:
             return "-";
         case JOY_ACTION_UI_ACTIVATE:
             return "-";
         case JOY_ACTION_UI_FUNCTION:
             /*sprintf(str, "ui action: 0x%04x %s", mapping->value.ui_action, ui_action_get_name(mapping->value.ui_action));*/
             sprintf(str, "%s", ui_action_get_name(mapping->value.ui_action));
             return str;
         case JOY_ACTION_POT_AXIS:
             log_warning(LOG_DEFAULT, "Mapping for POT axes is not implemented.");
             return "TODO";
    }
    return "invalid";
}

/** \brief  Create model for the joymaps table
 *
 * \return  new list store
 */
static GtkListStore *create_mappings_model(joystick_device_t *joydev)
{
    int i;
    int n = 0;
    GtkListStore *model;

    model = gtk_list_store_new(6,
                               G_TYPE_INT,      /* ID */
                               G_TYPE_INT,      /* ID */
                               G_TYPE_STRING,   /* input type */
                               G_TYPE_STRING,   /* input name */
                               G_TYPE_STRING,   /* mapping type */
                               G_TYPE_STRING    /* mapped value */
                               );

    DBG(("create_mappings_model joydev:%p", (void*)joydev));

    DBG(("%d buttons:", joydev->num_buttons));
    for (i = 0; i < joydev->num_buttons; i++) {
        GtkTreeIter      iter;
        joystick_button_t *button   = joydev->buttons[i];
        DBG(("%d: %s (mapping: action:%s val:%s)", n + i, button->name, get_action_string(button->mapping.action), get_mapped_string(&button->mapping)));
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                            &iter,
                            COL_INDEX, n + i,
                            COL_ID, n + i,
                            COL_INPUT_TYPE, "button",
                            COL_INPUT_NAME, button->name,
                            COL_MAPPED_TYPE, get_action_string(button->mapping.action),
                            COL_MAPPED_VAL, get_mapped_string(&button->mapping),
                            -1);
    }
    n+=i;

    DBG(("%d axis:", joydev->num_axes));
    for (i = 0; i < joydev->num_axes; i++) {
        GtkTreeIter      iter;
        joystick_axis_t *axis   = joydev->axes[i];
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                            &iter,
                            COL_INDEX, n + (i * 2) + 0,
                            COL_ID, n + i,
                            COL_INPUT_TYPE, "axis min",
                            COL_INPUT_NAME, axis->name,
                            COL_MAPPED_TYPE, get_action_string(axis->mapping.negative.action),
                            COL_MAPPED_VAL, get_mapped_string(&axis->mapping.negative),
                            -1);
        DBG(("%d: %s (mapping: action:%s val:%s)", n + i, axis->name, get_action_string(axis->mapping.negative.action), get_mapped_string(&axis->mapping.negative)));
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                            &iter,
                            COL_INDEX, n + (i * 2) + 1,
                            COL_ID, n + i,
                            COL_INPUT_TYPE, "axis max",
                            COL_INPUT_NAME, axis->name,
                            COL_MAPPED_TYPE, get_action_string(axis->mapping.positive.action),
                            COL_MAPPED_VAL, get_mapped_string(&axis->mapping.positive),
                            -1);
        DBG(("%d: %s (mapping: action:%s val:%s)", n + i, axis->name, get_action_string(axis->mapping.positive.action), get_mapped_string(&axis->mapping.positive)));
    }
    n+=i;

    DBG(("%d hats:", joydev->num_hats));
    for (i = 0; i < joydev->num_hats; i++) {
        GtkTreeIter      iter;
        joystick_hat_t *hat   = joydev->hats[i];
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                            &iter,
                            COL_INDEX, n + (i * 4) + 0,
                            COL_ID, n + i,
                            COL_INPUT_TYPE, "hat up",
                            COL_INPUT_NAME, hat->name,
                            COL_MAPPED_TYPE, get_action_string(hat->mapping.up.action),
                            COL_MAPPED_VAL, get_mapped_string(&hat->mapping.up),
                            -1);
        DBG(("%d: %s (mapping: action:%s val:%s)", n + i, hat->name, get_action_string(hat->mapping.up.action), get_mapped_string(&hat->mapping.up)));
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                            &iter,
                            COL_INDEX, n + (i * 4) + 1,
                            COL_ID, n + i,
                            COL_INPUT_TYPE, "hat down",
                            COL_INPUT_NAME, hat->name,
                            COL_MAPPED_TYPE, get_action_string(hat->mapping.down.action),
                            COL_MAPPED_VAL, get_mapped_string(&hat->mapping.down),
                            -1);
        DBG(("%d: %s (mapping: action:%s val:%s)", n + i, hat->name, get_action_string(hat->mapping.down.action), get_mapped_string(&hat->mapping.down)));
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                            &iter,
                            COL_INDEX, n + (i * 4) + 2,
                            COL_ID, n + i,
                            COL_INPUT_TYPE, "hat left",
                            COL_INPUT_NAME, hat->name,
                            COL_MAPPED_TYPE, get_action_string(hat->mapping.left.action),
                            COL_MAPPED_VAL, get_mapped_string(&hat->mapping.left),
                            -1);
        DBG(("%d: %s (mapping: action:%s val:%s)", n + i, hat->name, get_action_string(hat->mapping.left.action), get_mapped_string(&hat->mapping.left)));
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                            &iter,
                            COL_INDEX, n + (i * 4) + 3,
                            COL_ID, n + i,
                            COL_INPUT_TYPE, "hat right",
                            COL_INPUT_NAME, hat->name,
                            COL_MAPPED_TYPE, get_action_string(hat->mapping.right.action),
                            COL_MAPPED_VAL, get_mapped_string(&hat->mapping.right),
                            -1);
        DBG(("%d: %s (mapping: action:%s val:%s)", n + i, hat->name, get_action_string(hat->mapping.right.action), get_mapped_string(&hat->mapping.right)));
    }
    n+=i;
    return model;
}

static int mapping_dialog_last_selected = -1;

static void on_mapping_close(joystick_device_t *joydev)
{
    DBG(("on_mapping_close"));
    update_treeview_full(joydev);
}

static void show_mapping_dialog(joystick_device_t *joydev, int id)
{
    static char *input_type_str[8] = { "button", "axis min", "axis max",
        "hat up" , "hat down" , "hat left", "hat right", "invalid"  };
    int input_type = 7;
    char *name = NULL;
    joystick_mapping_t *mapping = NULL;

    DBG(("TODO: show_mapping_dialog id:%d joydev:%p", id, (void*)joydev));

    if (id < joydev->num_buttons) {
        /* is a button */
        int idx = id;
        joystick_button_t *button = joydev->buttons[idx];
        DBG(("button idx:%d", idx));
        name = button->name;
        mapping = &button->mapping;
        input_type = 0;
    } else if (id < (joydev->num_buttons + (joydev->num_axes * 2))) {
        /* is an axis */
        int idx = id - joydev->num_buttons;
        joystick_axis_t *axis = joydev->axes[idx / 2];
        DBG(("axis id:%d idx:%d", id, idx));
        name = axis->name;
        mapping = (idx & 1) ? &axis->mapping.positive : &axis->mapping.negative;
        input_type = 1 + (idx & 1);
    } else if (id < (joydev->num_buttons + (joydev->num_axes * 2) + (joydev->num_hats * 4))) {
        /* is a hat */
        int idx = id - joydev->num_buttons + (joydev->num_axes * 2);
        joystick_hat_t *hat   = joydev->hats[idx / 4];
        DBG(("hat idx:%d", idx));
        name = hat->name;
        if (idx & 1) {
            mapping = &hat->mapping.up;
        } else if (idx & 2) {
            mapping = &hat->mapping.down;
        } else if (idx & 4) {
            mapping = &hat->mapping.left;
        } else if (idx & 8) {
            mapping = &hat->mapping.right;
        }
        input_type = 3 + (idx & 3);
    }
    DBG(("input type: (%d) %s", input_type, input_type_str[input_type]));
    DBG(("input name: %s", name ? name : "NULL"));
    DBG(("mapping type: %u", mapping->action));

    joymap_dialog_show(joydev, mapping, input_type_str[input_type], name, on_mapping_close);
}


/** \brief  Handler for the 'row-activated' event of the treeview
 *
 * Handler triggered by double-clicked or pressing Return on a tree row.
 *
 * \param[in]   view    tree view (unused)
 * \param[in]   path    tree path to activated row
 * \param[in]   column  activated column (unused)
 * \param[in]   extra   extra event data (joystick_device_t)
 */
static void on_row_activated(GtkTreeView *view,
                             GtkTreePath *path,
                             GtkTreeViewColumn *column,
                             gpointer extra)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    joystick_device_t *joydev = (joystick_device_t *)extra;

    DBG(("on_row_activated joydev:%p", (void*)joydev));
#if 1
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(mappings_view));
    if (gtk_tree_model_get_iter(model, &iter, path)) {

        gint idx = 0;
        gint id = 0;
        gchar *input_name = NULL;
        gchar *input_type = NULL;

        /* get current mapping info */
        gtk_tree_model_get(model, &iter,
                           COL_INDEX, &idx,
                           COL_ID, &id,
                           COL_INPUT_TYPE, &input_type,
                           COL_INPUT_NAME, &input_name,
                           -1);
        DBG(("id: %d type:%s name:%s", id, input_type, input_name));

        /* remember selected row */
        DBG(("mapping_dialog_last_selected: %d", idx));
        mapping_dialog_last_selected = idx;

        show_mapping_dialog(joydev, idx);
    }
#endif
}

/** \brief  Create the table view of the mappings
 *
 * Create a table with 'type', 'name', 'mapping type' and 'value' columns.
 * Columns are resizable and the table is sortable by clicking the column
 * headers.
 *
 * \return  GtkTreeView
 */
static GtkWidget *create_mappings_view(joystick_device_t *joydev)
{
    GtkWidget *view;
    GtkListStore *model;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    model = create_mappings_model(joydev);
    view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));

    /* type */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Type",
                                                      renderer,
                                                      "text", COL_INPUT_TYPE,
                                                      NULL);
    gtk_tree_view_column_set_sort_column_id(column, COL_INPUT_TYPE);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    /* name */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name",
                                                      renderer,
                                                      "text", COL_INPUT_NAME,
                                                      NULL);
    gtk_tree_view_column_set_sort_column_id(column, COL_INPUT_NAME);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    /* mapping type */
    renderer = gtk_cell_renderer_text_new();
    /* g_object_set(G_OBJECT(renderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL); */
    column = gtk_tree_view_column_new_with_attributes("Mapping type",
                                                      renderer,
                                                      "text", COL_MAPPED_TYPE,
                                                      NULL);
    gtk_tree_view_column_set_sort_column_id(column, COL_MAPPED_TYPE);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    /* mapped value */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Mapped value",
                                                      renderer,
                                                      "text", COL_MAPPED_VAL,
                                                      NULL);
    gtk_tree_view_column_set_sort_column_id(column, COL_MAPPED_VAL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    g_signal_connect(view, "row-activated", G_CALLBACK(on_row_activated), joydev);
    /*g_signal_connect(view, "button-press-event", G_CALLBACK(on_button_press_event), NULL);*/

    return view;
}

/** \brief  Select active row in the treeview by index
 */
static void mappings_select_by_index(int idx)
{
    GtkTreeSelection  *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(mappings_view));
    GtkTreePath *path = gtk_tree_path_new_from_indices(idx, -1);
    gtk_tree_selection_select_path(selection, path);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(mappings_view), path, NULL, FALSE, 0, 0);
    gtk_tree_path_free(path);
}

/** \brief  Update treeview by regenerating model with current joymaps
 */
static void update_treeview_full(joystick_device_t *joydev)
{
    GtkListStore *model = create_mappings_model(joydev);

    gtk_tree_view_set_model(GTK_TREE_VIEW(mappings_view), GTK_TREE_MODEL(model));

    DBG(("update_treeview_full"));

    if (mapping_dialog_last_selected > -1) {
        /* select the row with the calculated index */
        DBG(("select row mapping_dialog_last_selected: %d", mapping_dialog_last_selected));
        mappings_select_by_index(mapping_dialog_last_selected);
    }
}


/* create 'mapping' tab */
static GtkWidget *mapping_widget_new(joystick_device_t *joydev)
{
    GtkWidget *scroll;
    GtkWidget *mapping_file_grid;
    GtkWidget *grid = gtk_grid_new();

    DBG(("mapping_widget_new"));

    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    /* create view, pack into scrolled window and add to grid */
    mappings_view = create_mappings_view(joydev);
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_container_add(GTK_CONTAINER(scroll), mappings_view);
    gtk_widget_show_all(scroll);
    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 0, 1, 1);

    mapping_file_grid = create_mapping_file_grid(joydev);
    gtk_grid_attach(GTK_GRID(grid), mapping_file_grid, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}

/* create 'calibrate' tab */
static GtkWidget *calibration_widget_new(joystick_device_t *joydev)
{
    GtkWidget *calib_grid;
    GtkWidget *grid = gtk_grid_new();

    calib_grid = create_calibration_grid(joydev);
    gtk_grid_attach(GTK_GRID(grid), calib_grid, 0, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}

/* reopen all devices used by the mapping gui for the main emulation thread */
static void reopen_host_devices(void)
{
    int           index;

    for (index = 0; index < joystick_device_count(); index++) {
        joystick_device_t *joydev = joystick_device_by_index(index);
        if ((joydev->status & JOY_REOPEN_MASK) == JOY_REOPEN_MAIN) {
            DBG(("reopen_host_device_callback: %p mode: %u", (void*)joydev, joydev->status & JOY_POLL_MASK));
            joystick_device_close(joydev);
            joystick_device_open(joydev, JOY_POLL_MAIN);
            joydev->status &= ~JOY_REOPEN_MAIN;
        }
    }
}

static void on_joymap_widget_destroy(GtkWidget *self, gpointer unused)
{
    DBG(("on_joymap_widget_destroy"));
    stop_polling();

    /* reopen previously closed host devices */
    reopen_host_devices();

    /* HACK: zero all the now invalid global widget references. We shouldn't be keeping these references globally. */
    memset(&global_widgets, 0, sizeof(global_widgets));
}


static GtkWidget *joymap_stack;
static GtkWidget *joymap_switcher;

GtkWidget *settings_joymap_widget_create(GtkWidget *parent)
{
    GtkWidget *label;
    int        row = 0;

    poll_timeout_id = 0;

    global_widgets.layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(global_widgets.layout), 16);
    gtk_grid_set_row_spacing(GTK_GRID(global_widgets.layout), 8);

    label = gtk_label_new("Host device");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    global_widgets.device_combo = device_combo_new();
    gtk_widget_set_hexpand(global_widgets.device_combo, TRUE);
    gtk_grid_attach(GTK_GRID(global_widgets.layout), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(global_widgets.layout), global_widgets.device_combo, 1, row, 1, 1);
    row++;

    joymap_stack = gtk_stack_new();
    joymap_switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(joymap_switcher), GTK_STACK(joymap_stack));

    gtk_grid_attach(GTK_GRID(global_widgets.layout), joymap_switcher, 0, row++, 2, 1);
    gtk_grid_attach(GTK_GRID(global_widgets.layout), joymap_stack,    0, row++, 2, 1);

    global_widgets.inputs_grid = gtk_grid_new();
    global_widgets.mappings_grid = gtk_grid_new();
    global_widgets.calibration_grid = gtk_grid_new();
    gtk_stack_add_titled(GTK_STACK(joymap_stack), global_widgets.inputs_grid,      "inputs",      "Inputs");
    gtk_stack_add_titled(GTK_STACK(joymap_stack), global_widgets.mappings_grid,    "mappings",    "Mappings");
    gtk_stack_add_titled(GTK_STACK(joymap_stack), global_widgets.calibration_grid, "calibration", "Calibration");


    g_signal_connect(G_OBJECT(global_widgets.layout),
                     "destroy",
                     G_CALLBACK(on_joymap_widget_destroy),
                     NULL);

    /* if there is one or more entries, select the first by default */
    if (joystick_device_count() > 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(global_widgets.device_combo), 0);
    }

    gtk_widget_show_all(global_widgets.layout);
    return global_widgets.layout;
}


static void inputs_joystick_event(void *input, joystick_input_t type, int32_t value)
{
    GtkWidget         *grid;
    joystick_axis_t   *axis;
    joystick_button_t *button;
    joystick_hat_t    *hat;
    GtkWidget         *led;
    GtkWidget         *scale;

    /* the following updates the "inputs" tab */
    switch (type) {
        case JOY_INPUT_AXIS:
            axis = input;
            if (global_widgets.event_widget != NULL) {
#ifdef USE_SCROLLED_WINDOW_HELPER
                grid = global_widgets.inputs_axis;
#else
                grid = gtk_grid_get_child_at(GTK_GRID(event_widget), 0, 0);
#endif
                if (grid != NULL && GTK_IS_GRID(grid)) {
                    scale = gtk_grid_get_child_at(GTK_GRID(grid), 1, 1 + axis->index);
                    if (scale != NULL) {
                        gtk_range_set_value(GTK_RANGE(scale), (gdouble)value);
                    }
                }
            }
            /* FIXME: scroll list to make the "active" axis visible if scrolled
                      out of sight */
            break;
        case JOY_INPUT_BUTTON:
            button = input;
            /* debug_gtk3("Button %s, Index %d, Value %d", button->name, button->index, value); */
            if (global_widgets.event_widget != NULL) {
                /* button grid */
#ifdef USE_SCROLLED_WINDOW_HELPER
                grid = global_widgets.inputs_buttons;
#else
                grid = gtk_grid_get_child_at(GTK_GRID(event_widget), 1, 0);
#endif
                if (grid != NULL && GTK_IS_GRID(grid)) {
                    led = gtk_grid_get_child_at(GTK_GRID(grid), 1, 1 + button->index);
                    if (led != NULL) {
                        joy_button_led_widget_set_active(led, value ? TRUE : FALSE);
                    }
                }
            }
            /* FIXME: scroll list to make the "active" button visible if scrolled
                      out of sight */
            break;
        case JOY_INPUT_HAT:
            hat = input;
            /* debug_gtk3("Hat %s, Index %d, Value %d", hat->name, hat->index, value); */
            if (global_widgets.event_widget != NULL) {
                /* hat grid */
#ifdef USE_SCROLLED_WINDOW_HELPER
                grid = global_widgets.inputs_hats;
#else
                grid = gtk_grid_get_child_at(GTK_GRID(event_widget), 1, 0);
#endif
                if (grid != NULL && GTK_IS_GRID(grid)) {
                    led = gtk_grid_get_child_at(GTK_GRID(grid), 1, 1 + hat->index);
                    if (led != NULL) {
                        joy_hat_led_widget_set_active(led, (value & 1) ? TRUE : FALSE,
                        (value & 2) ? TRUE : FALSE, (value & 4) ? TRUE : FALSE, (value & 8) ? TRUE : FALSE);
                    }
                }
            }
            /* FIXME: scroll list to make the "active" hat visible if scrolled
                      out of sight */
            break;
        default:
            break;
    }

}

static void mappings_joystick_event(void *input, joystick_input_t type, int32_t value)
{
    int n = -1;

    joystick_axis_t   *axis;
    joystick_button_t *button;
    joystick_hat_t    *hat;

    GtkTreeIter       iter;
    joystick_device_t *joydev = NULL;

    /*DBG(("mappings_joystick_event type:%u value:%d input:%p", type, value, input));*/

    /* get joydev from host device drop down list */
    {
        int index = -1;

        if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(global_widgets.device_combo), &iter)) {
            GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(global_widgets.device_combo));
            gtk_tree_model_get(model, &iter, 0, &index, -1);
            if (index > -1) {
                joydev = joystick_device_by_index(index);
            }
        }
    }

    switch (type) {
        case JOY_INPUT_BUTTON:
            button = input;
            n = button->index;
            if (value == 0) {
                /* ignore button release events */
                n = -1;
            }
            break;
        case JOY_INPUT_AXIS:
            axis = input;
            n = (axis->index * 2);
            if (joydev) {
                n += joydev->num_buttons;
            }

            if (value > (axis->maximum - (axis->range / 4))) {
                /* upper quarter of the range -> advance index to "axis max" */
                n++;
            } else if (value > (axis->minimum + (axis->range / 4))) {
                /* values not in the bottom quarter of the range must be
                   "set to neutral", ignore these */
                n = -1;
            }

            break;
        case JOY_INPUT_HAT:
            hat = input;
            n = (hat->index * 4);
            if (joydev) {
                n += joydev->num_buttons;
                n += joydev->num_axes;
            }
            if (value == 0) {
                /* ignore hat release events */
                n = -1;
            } else {
                if (value & 2) {
                    n++; /* down */
                }
                if (value & 4) {
                    n++; /* left */
                }
                if (value & 8) {
                    n++; /* right */
                }
            }
            break;
        default:
            break;
    }

    if (n > -1) {
        DBG(("mappings_joystick_event select row:%d", n));
        /* select the row with the calculated index */
        mappings_select_by_index(n);
    }
}

void joystick_ui_event(void *input, joystick_input_t type, int32_t value)
{
    mainlock_assert_is_not_vice_thread();

    /*DBG(("joystick_ui_event type:%d value:%d input:%p", type, value, input));*/

    /* FIXME: serve the "calibration" tab with data as needed */

    if (gtk_stack_get_visible_child(GTK_STACK(joymap_stack)) == global_widgets.inputs_grid) {
        inputs_joystick_event(input, type, value);
    } else if (gtk_stack_get_visible_child(GTK_STACK(joymap_stack)) == global_widgets.mappings_grid) {
        mappings_joystick_event(input, type, value);
    }


}
