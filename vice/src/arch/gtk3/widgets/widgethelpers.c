/** \file   src/arch/gtk3/uihelpers.c
 * \brief   Helpers for creating Gtk3 widgets
 *
 * This file is supposed to contain some helper functions for boiler plate
 * code, such as creating layout widgets, creating lists of radio or check
 * boxes, etc.
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
 *
 *  TODO:   turn the margin/padding values into defines and move into a file
 *          like uidefs.h
 *
 *  TODO:   rename functions
 */


#include "vice.h"

#include <gtk/gtk.h>

#include "lib.h"
#include "resources.h"
#include "vsync.h"

#include "debug_gtk3.h"

#include "widgethelpers.h"


/** \brief  Size of the buffer used for snprintf() calls to generate labels
 */
#define LABEL_BUFFER_SIZE   256


/** \brief  Create a GtkGrid with a bold GtkLabel as its first widget
 *
 * This creates a GtkGrid with a left-aligned, bold label, optionally spread
 * over multiple columns. If you don't what pass as the \a columns argument,
 * just pass 1.
 *
 * \param[in]   text    label text
 * \param[in]   columns number of columns in the grid the label should span
 *
 * \return  GtkGrid with a label
 */
GtkWidget *uihelpers_create_grid_with_label(const gchar *text, gint columns)
{
    GtkWidget *grid;
    GtkWidget *label;
    gchar buffer[LABEL_BUFFER_SIZE];

    /* sanitize columns input */
    if (columns < 1) {
        columns = 1;
    }

    /* use HTML-ish markup to make the label bold */
    g_snprintf(buffer, LABEL_BUFFER_SIZE, "<b>%s</b>", text);

    grid = gtk_grid_new();
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), buffer);
    gtk_widget_set_halign(label, GTK_ALIGN_START);    /* align left */
    g_object_set(label, "margin-bottom", 8, NULL);  /* add 8 units of margin
                                                       to the bottom */
    gtk_widget_show(label);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, columns, 1);
    return grid;
}


/** \brief  Create a GtkGrid with a label radio buttons with text/id pairs
 *
 * \param[in]   label       label text
 * \param[in]   data        text/value pairs for the radio buttons
 * \param[in]   callback    optional callback to trigger when a radio button
 *                          is toggled (`NULL` == no callback)
 *
 * \note    keep in mind that the callback is also triggered when a radio button
 *          is deactivated, so use gtk_toggle_button_get_active() if you only
 *          want to respond to the currently activated radio button
 *
 * \return  GtkGrid
 */
GtkWidget *uihelpers_create_int_radiogroup_with_label(
        const gchar *label,
        ui_text_int_pair_t *data,
        void (*callback)(GtkWidget *, gpointer),
        int active)
{
    GtkWidget *grid;
    GtkRadioButton *last;
    GSList *group = NULL;
    size_t i;

    grid = uihelpers_create_grid_with_label(label, 1);

    last = NULL;
    for (i = 0; data[i].text != NULL; i++) {
        GtkWidget *radio = gtk_radio_button_new_with_label(group, data[i].text);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio), last);
        gtk_grid_attach(GTK_GRID(grid), radio, 0, i + 1, 1, 1);
        g_object_set(radio, "margin-left", 16, NULL);  /* indent 16 units */

        if (active == i) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
        }

        if (callback != NULL) {
            g_signal_connect(radio, "toggled", G_CALLBACK(callback),
                    GINT_TO_POINTER(data[i].value));
        }
        gtk_widget_show(radio);
        last = GTK_RADIO_BUTTON(radio);
    }
    g_object_set(grid, "margin", 8, NULL);
    gtk_widget_show(grid);
    return grid;
}


/** \brief  Create a GtkButtonBox
 *
 * \param[in]   buttons     list of (title, callback) tuples, NULL-terminated
 * \param[in]   orientation orientation of the box (\see GtkOrientation)
 *
 * \return  GtkButtonBox
 */
GtkWidget *uihelpers_create_button_box(
        ui_button_t *buttons,
        GtkOrientation orientation)
{
    GtkWidget *box = gtk_button_box_new(orientation);
    size_t i;

    for (i = 0; buttons[i].text != NULL; i++) {
        GtkWidget *button = gtk_button_new_with_label(buttons[i].text);
        if (buttons[i].callback != NULL) {
            g_signal_connect(button,
                    "clicked",
                    G_CALLBACK(buttons[i].callback),
                    NULL);
        }
        gtk_widget_show(button);
        gtk_container_add(GTK_CONTAINER(box), button);
    }
    gtk_widget_show(box);
    return box;
}


/** \brief  Set a radio button to active in a GktGrid
 *
 * This function only checks for radio buttons in the first row of the \a grid,
 * so it works fine with widgets created through
 * uihelpers_uihelpers_create_int_radiogroup_with_label(), but not much else.
 * So it might need some refactoring
 *
 * \param[in]   grid    GtkGrid containing radio buttons
 * \param[in[   index   index of the radio button (the actual index of the
 *                      radio button, other widgets are skipped)
 */
void uihelpers_set_radio_button_grid_by_index(GtkWidget *grid, int index)
{
    GtkWidget *radio;
    int row = 0;
    int radio_index = 0;


    debug_gtk3("Looking for index %d\n", index);

    if (index < 0) {
        return;
    }

    do {
        radio = gtk_grid_get_child_at(GTK_GRID(grid), 0, row);
        if (GTK_IS_TOGGLE_BUTTON(radio)) {
            debug_gtk3("got toggle button at row %d\n", row);
            if (radio_index == index) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
                return;
            }
            radio_index++;
        }
        row++;
    } while (radio != NULL);
}



/** \brief  Create the bold title label of a settings widget's grid
 *
 * \return  label
 */
GtkWidget *uihelpers_create_grid_label(const char *text)
{
    GtkWidget *label;
    gchar buffer[LABEL_BUFFER_SIZE];

    label = gtk_label_new(NULL);
    g_snprintf(buffer, LABEL_BUFFER_SIZE, "<b>%s</b>", text);
    gtk_label_set_markup(GTK_LABEL(label), buffer);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-bottom", 8, NULL);
    return label;
}



/** \brief  Create a left-aligned, 16 units indented label
 *
 * \param[in]   text    label text
 *
 * \return  label
  */
GtkWidget *uihelpers_create_indented_label(const char *text)
{
    GtkWidget *label = gtk_label_new(text);

    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    return label;
}



/** \brief  Handler for the ui_helpers_create_resource_checkbox() toggled event
 *
 * \param[in]   widget      checkbox
 * \param[in]   user_data   resource name
 */
static void resource_callback(GtkWidget *widget, gpointer user_data)
{
    const char *resource = (const char *)user_data;
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    debug_gtk3("resource '%s' -> %d\n", resource, state);
    resources_set_int(resource, state);
}


/** \brief  Create a checkbox connected to a VICE resource
 *
 * \param[in]   label       checkbox text
 * \param[in]   resource    name of resource to set/unset
 *
 * \return  checkbox
 */
GtkWidget *uihelpers_create_resource_checkbox(const char *label,
                                              const char *resource)
{
    GtkWidget *check;
    int state;

    check =  gtk_check_button_new_with_label(label);
    resources_get_int(resource, &state);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), state);

    g_signal_connect(check, "toggled", G_CALLBACK(resource_callback),
            (gpointer)(resource));

    return check;
}

