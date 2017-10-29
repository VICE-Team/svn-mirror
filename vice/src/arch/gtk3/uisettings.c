/*
 * uisettings.c - GTK3 settings dialog
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


/* The settings_grid is supposed to become this:
 *
 * +--------------+---------------------------+
 * | treeview     |                           |
 * |  with        |                           |
 * |   settings   |    central widget,        |
 * |  more        |    depending on which     |
 * |   foo        |    item is selected in    |
 * |   bar        |    the treeview           |
 * |    whatever  |                           |
 * | burp         |                           |
 * +--------------+---------------------------+
 *
 * And this is handled by the dialog itself:
 * +------------------------------------------+
 * | load | save | load... | save... | close  |
 * +------------------------------------------+
 */


#include "vice.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "vsync.h"

#include "debug_gtk3.h"
#include "resourcecheckbutton.h"
#include "widgethelpers.h"
#include "openfiledialog.h"
#include "savefiledialog.h"

#include "uispeed.h"
#include "uikeyboard.h"
#include "uisound.h"
#include "uiautostart.h"
#include "uidrivesettings_new.h"
#include "uimodel.h"
#include "uimisc.h"
#include "ramresetwidget.h"
#include "uivideosettings.h"
#include "uisamplersettings.h"
#include "uiprintersettings.h"
#include "uicontrolport.h"
#include "uijoystick.h"
#include "uimousesettings.h"
#include "uisoundchipsettings.h"

/* I/O extension widgets */
#include "c64memoryexpansionhackswidget.h"
#include "georamwidget.h"
#include "reuwidget.h"
#include "ramcartwidget.h"

#include "uisettings.h"


#define NUM_COLUMNS 2


#define DIALOG_WIDTH 800
#define DIALOG_HEIGHT 560


#define DIALOG_WIDTH_MAX 1024
#define DIALOG_HEIGHT_MAX 640


/** \brief  Enum used for the "response" callback of the settings dialog
 *
 * All values must be positive since Gtk reserves standard responses in its
 * GtkResponse enum as negative values.
 */
enum {
    RESPONSE_LOAD = 1,  /**< "Load" -> load settings from default file */
    RESPONSE_SAVE,      /**< "Save" -> save settings from default file */
    RESPONSE_LOAD_FILE, /**< "Load ..." -> load settings via dialog */
    RESPONSE_SAVE_FILE  /**< "Save ..." -> save settings via dialog */
};


/** \brief  List of C64 I/O extensions (x64, x64sc)
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t c64_io_extensions[] = {
    { "Memory Expansions Hack",     c64_memory_expansion_hacks_widget_create, NULL },

    { "GEO-RAM",                    georam_widget_create, NULL },
    { "RAM Expansion Module",       reu_widget_create, NULL },
    { "RamCart",                    ramcart_widget_create, NULL },

    { "Double Quick Brown Box",     NULL, NULL },
    { "Expert Cartridge",           NULL, NULL },
    { "ISEPIC",                     NULL, NULL },

    { "EasyFlash",                  NULL, NULL },
    { "GMod2",                      NULL, NULL },
    { "IDE64",                      NULL, NULL },
    { "MMC64",                      NULL, NULL },
    { "MMC Replay",                 NULL, NULL },
    { "Retro Replay",               NULL, NULL },
    { "Super Snapshot V5",          NULL, NULL },

    { "Ethernet Cartridge",         NULL, NULL },
    { "RR-Net Mk3",                 NULL, NULL },

    { "IEEE-448 Interface",         NULL, NULL },
    { "Burst Mode Modification",    NULL, NULL },

    { "DigiMAX",                    NULL, NULL },
    { "Magic Voice",                NULL, NULL },
    { "MIDI emulation",             NULL, NULL },
    { "SFX Sound Expander",         NULL, NULL },
    { "SFX Sound Sampler",          NULL, NULL },   /* checkmark in Gtk2 */
    { "CP/M Cartridge",             NULL, NULL },   /* checkmark in Gtk2 */

    { "DS12C887 Real Time Clock",   NULL, NULL },
    { "Userport devices",           NULL, NULL },
    { "Tape port devices",          NULL, NULL },

    { "I/O collision handling ($d800-$d8ff)", NULL, NULL },
    { "Reset on cart change",       NULL, NULL },   /* checkmark in Gtk2 */
    { NULL, NULL, NULL }
};


/** \brief  Main tree nodes
 *
 *
 * TODO: When creating the SID model/SID settings widget: The fastSiD/ReSID
 *       selection ("SidEngine" resource) should be moved to 'SID Settings',
 *       while the SID model (6581/8580(D), "SidModel" resource should be in
 *       the SID Model widget, with the reSID/fastSID options removed.
 *
 * -- compyx 2017-09-24
 */
static ui_settings_tree_node_t main_nodes[] = {
    { "Speed settings", uispeed_create_central_widget, NULL },
    { "Keyboard settings", uikeyboard_create_central_widget, NULL },
    { "Sound settings", uisound_create_central_widget, NULL },
    { "Sampler settings", uisamplersettings_widget_create, NULL },
    { "Autostart settings", uiautostart_create_central_widget, NULL },
    { "Drive settings", uidrivesettings_widget_create, NULL },
    { "Printer settings", uiprintersettings_widget_create, NULL },
    { "Control port settings", uicontrolport_widget_create, NULL },
    { "Joystick settings", uijoystick_widget_create, NULL },
    { "Mouse settings", uimousesettings_widget_create, NULL },
    { "Model settings", uimodel_create_central_widget, NULL },
    { "RAM reset pattern", create_ram_reset_central_widget, NULL },
    { "Miscellaneous", uimisc_create_central_widget, NULL },
    { "Video settings", uivideosettings_widget_create, NULL },
    /* TODO: only enable this item when a SID is supported or can be supported
     *       through an I/O extension
     */
    { "SID settings", uisoundchipsettings_widget_create, NULL },
    { "I/O extensions", NULL, c64_io_extensions },  /* C64 only for now */
    { NULL, NULL, NULL }
};




/** \brief  Reference to the current 'central' widget in the settings dialog
 */
static void ui_settings_set_central_widget(GtkWidget *widget);


/** \brief  Reference to the settings dialog
 *
 * Used to show/hide the widget without rebuilding it each time. Clean up
 * with ui_settings_dialog_shutdown()
 */
static GtkWidget *settings_window = NULL;


/** \brief  Reference to the 'content area' widget of the settings dialog
 */
static GtkWidget *settings_grid = NULL;


/** \brief  Handler for the "changed" event of the tree view
 *
 * \param[in]   selection   GtkTreeSelection associated with the tree model
 * \param[in]   user_data   data for the event (unused for now)
 *
 */
static void on_tree_selection_changed(
        GtkTreeSelection *selection,
        gpointer user_data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *name;
        GtkWidget *(*callback)(void *) = NULL;
        gtk_tree_model_get(model, &iter, 0 /* col 0 */, &name, -1);
        debug_gtk3("item '%s' clicked\n", name);
        gtk_tree_model_get(model, &iter, 1, &callback, -1);
        if (callback != NULL) {
            ui_settings_set_central_widget(callback(NULL));
        }
        g_free(name);
    }
}


/** \brief  Create the 'Save on exit' checkbox
 *
 * The current position/display of the checkbox is a little lame at the moment
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_save_on_exit_checkbox(void)
{
    return resource_check_button_create("SaveResourcesOnExit",
            "Save settings on exit");
}


/** \brief  Create the 'Confirm on exit' checkbox
 *
 * The current position/display of the checkbox is a little lame at the moment
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_confirm_on_exit_checkbox(void)
{
    return resource_check_button_create("ConfirmOnExit", "Confirm on exit");
}


/** \brief  Create tree store containing settings items and children
 *
 * \return  GtkTreeStore
 */
static GtkTreeStore *create_tree_store(void)
{
    GtkTreeStore *store;
    GtkTreeIter iter;
    GtkTreeIter child;
    int i;

    store = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_POINTER);

    for (i = 0; main_nodes[i].name != NULL; i++) {
        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(store, &iter,
                0, main_nodes[i].name,
                1, main_nodes[i].callback,
                -1);
        /* this bit will need proper recursion if we need more than two
         * levels of subitems */
        if (main_nodes[i].children != NULL) {
            int c;
            ui_settings_tree_node_t *list = main_nodes[i].children;

            for (c = 0; list[c].name != NULL; c++) {
                gtk_tree_store_append(store, &child, &iter);
                gtk_tree_store_set(store, &child,
                        0, list[c].name,
                        1, list[c].callback,
                        -1);
            }
        }
    }
    return store;
}


/** \brief  Create treeview for settings side-menu
 *
 * Reads items from `main_nodes` and adds them to the tree view.
 *
 * \return  GtkTreeView
 *
 * TODO:    handle nested items, and write up somewhere how the hell I finally
 *          got the callbacks working
 */
static GtkWidget *create_treeview(void)
{
    GtkWidget *tree;
    GtkTreeStore *store;
    GtkCellRenderer *text_renderer;
    GtkTreeViewColumn *text_column;

    store = create_tree_store();
    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    text_renderer = gtk_cell_renderer_text_new();
    text_column = gtk_tree_view_column_new_with_attributes(
            NULL,
            text_renderer,
            "text", 0,
            NULL);
    /*    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), obj_column); */
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), text_column);
    return tree;
}


/** \brief  Set the 'central'/action widget for the settings dialog
 *
 * Destroys the old 'central' widget and sets the new one.
 *
 *  \param[in,out]  widget  widget to use as the new 'central' widget
 */
static void ui_settings_set_central_widget(GtkWidget *widget)
{
    GtkWidget *child;

    child = gtk_grid_get_child_at(GTK_GRID(settings_grid), 1, 0);
    if (child != NULL) {
        gtk_widget_destroy(child);
    }
    gtk_grid_attach(GTK_GRID(settings_grid), widget, 1, 0, 1, 1);
}


/** \brief  Create the 'content widget' of the settings dialog
 *
 * This creates the widget in the dialog used to display the treeview and room
 * for the widget connected to that tree's currently selected item.
 *
 * \param[in]   widget  parent widget
 *
 * \return  GtkGrid (as a GtkWidget)
 */
static GtkWidget *create_content_widget(GtkWidget *widget)
{
    GtkWidget *tree;
    GtkTreeSelection *selection;
    GtkWidget *scroll;
    GtkWidget *extra;

    debug_gtk3("called\n");

    settings_grid = gtk_grid_new();
    tree = create_treeview();
    g_print("tree created\n");

    /* pack the tree in a scrolled window to allow scrolling of the tree when
     * it gets too large for the dialog
     */
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), tree);

    gtk_grid_attach(GTK_GRID(settings_grid), scroll, 0, 0, 1, 1);

    /* TODO: remember the previously selected setting/widget and set it here */
    ui_settings_set_central_widget(uispeed_create_central_widget(widget));

    /* create container for generic settings */
    extra = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(extra), 8);
    gtk_grid_set_row_spacing(GTK_GRID(extra), 8);
    g_object_set(extra, "margin", 16, NULL);

    gtk_grid_attach(GTK_GRID(extra), create_save_on_exit_checkbox(),
            0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(extra), create_confirm_on_exit_checkbox(),
            1, 0, 1, 1);

    /* add to main layout */
    gtk_grid_attach(GTK_GRID(settings_grid), extra, 0, 2, 2, 1);

    gtk_widget_show(settings_grid);
    gtk_widget_show(tree);

    gtk_widget_set_size_request(scroll, 180, 500);
    gtk_widget_set_size_request(settings_grid, DIALOG_WIDTH, DIALOG_HEIGHT);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(selection), "changed",
            G_CALLBACK(on_tree_selection_changed), NULL);


    return settings_grid;
}


/** \brief  Properly destroy the settings window if required
 */
void ui_settings_dialog_shutdown(void)
{
    if (settings_window != NULL && GTK_IS_WIDGET(settings_window)) {
        gtk_widget_destroy(settings_window);
    }
}



/** \brief  Handler for the "response" event of the settings dialog
 *
 * This determines what to do based on the 'reponse ID' emitted by the dialog.
 *
 * \param[in]   widget      widget triggering the event (button pushed)
 * \param[in]   response_id response ID
 * \param[in]   user_data   extra data (unused)
 */
static void response_callback(GtkWidget *widget, gint response_id,
                              gpointer user_data)
{
    gchar *filename;

    switch (response_id) {

        /* close dialog */
        case GTK_RESPONSE_DELETE_EVENT:
            debug_gtk3("destroying settings widget\n");
            gtk_widget_destroy(widget);
            break;

        /* load vicerc from default location */
        case RESPONSE_LOAD:
            debug_gtk3("loading resources from default file\n");
            if(resources_load(NULL) != 0) {
                debug_gtk3("failed\n");
            }
            break;

        /* load vicerc from a user-specified location */
        case RESPONSE_LOAD_FILE:
            filename = ui_open_file_dialog(widget, "Load settings file",
                    NULL, NULL, NULL);
            if (filename!= NULL) {
                debug_gtk3("loading settings from '%s'\n", filename);
                if (resources_load(filename) != 0) {
                    debug_gtk3("failed\n");
                }
            }
            break;

        /* save settings to default location */
        case RESPONSE_SAVE:
            debug_gtk3("saving vicerc to default location\n");
            if (resources_save(NULL) != 0) {
                debug_gtk3("failed!\n");
            }
            break;

        /* save settings to a user-specified location */
        case RESPONSE_SAVE_FILE:
            filename = ui_save_file_dialog(widget, "Save settings as ...",
                    NULL, TRUE, NULL);
            if (filename != NULL) {
                debug_gtk3("saving setting as '%s'\n", filename ? filename : "NULL");
                if (resources_save(filename) != 0) {
                    debug_gtk3("failed!\n");
                }
                g_free(filename);
            }
            break;

        default:
            break;
    }
}


static gboolean on_dialog_configure_event(GtkWidget *widget, GdkEvent *event,
        gpointer user_data)
{
    if (event->type == GDK_CONFIGURE) {
        /*
        GdkRGBA color = { 1.0, 0.0, 0.0, 1.0 };
         */
        int width = ((GdkEventConfigure*)event)->width;
        int height = ((GdkEventConfigure*)event)->height;
        debug_gtk3("width %d, height %d\n", width, height);
        if (width > DIALOG_WIDTH_MAX || height > DIALOG_HEIGHT_MAX) {
            /*
            gtk_widget_override_background_color(widget, 0, &color);
            */
            gtk_window_set_title(GTK_WINDOW(widget),
                    "HELP! --- DIALOG IS TOO BLOODY LARGE -- ERROR!");
        }

    }
    return FALSE;
}


/** \brief  Callback to create the main settings dialog from the menu
 *
 * \param[in]   widget      (direct) parent widget, the menu item
 * \param[in]   user_data   data for the event (unused)
 *
 * \note    The appearance of minimize/maximize buttons seems to depend on which
 *          Window Manager is active:
 *
 *          On MATE (marco, a Metacity fork) both buttons are hidden.
 *          On KDE (KWin) the maximize button is still visible but inactive
 *          On OpenBox both min/max are visible with only minimize working
 */
void ui_settings_dialog_create(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;
    GtkWidget *content;
    char title[256];

    g_snprintf(title, 256, "%s Settings", machine_name);

    dialog = gtk_dialog_new_with_buttons(
            title,
            GTK_WINDOW(gtk_widget_get_toplevel(widget)),
            GTK_DIALOG_MODAL,
            "Load", RESPONSE_LOAD,
            "Save", RESPONSE_SAVE,
            "Load file ...", RESPONSE_LOAD_FILE,
            "Save file ...", RESPONSE_SAVE_FILE,
            "Close", GTK_RESPONSE_DELETE_EVENT,
            NULL);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content), create_content_widget(dialog));

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    g_signal_connect(dialog, "response", G_CALLBACK(response_callback), NULL);
    g_signal_connect(dialog, "configure-event",
            G_CALLBACK(on_dialog_configure_event), NULL);
    gtk_widget_show_all(dialog);
}
