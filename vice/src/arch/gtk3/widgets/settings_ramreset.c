/** \file   settings_ramreset.c
 * \brief   Widget to control the RAM reset pattern settings
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES RAMInitStartValue           all
 * $VICERES RAMInitValueInvert          all
 * $VICERES RAMInitPatternInvert        all
 * $VICERES RAMInitValueOffset          all
 * $VICERES RAMInitPatternInvertValue   all
 * $VICERES RAMInitStartRandom          all  
 * $VICERES RAMInitRepeatRandom         all
 * $VICERES RAMInitRandomChance         all
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
#include <string.h>
#include <math.h>

#include "vice_gtk3.h"
#include "ram.h"
#include "resources.h"

#include "settings_ramreset.h"


/** \brief  List of powers of two used for the widgets
 *
 * Yes, this looks silly, but allows me to use vice-gtk3 widgets.
 */
static const vice_gtk3_combo_entry_int_t powers_of_two[] = {
    { "0 bytes", 0 }, { "1 byte", 1 }, { "2 bytes", 2 }, { "4 bytes", 4 }, 
    { "8 bytes", 8 }, { "16 bytes", 16 }, { "32 bytes", 32 }, { "64 bytes", 64 }, 
    { "128 bytes", 128 }, { "256 bytes", 256 }, { "512 bytes", 512 }, { "1024 bytes", 1024 }, 
    { "2048 bytes", 2048 }, { "4096 bytes", 4096 }, { "8192 bytes", 8192 }, { "16384 bytes", 16384 }, 
    { "32768 bytes", 32768 }, { NULL, -1 }
};

#define PREVIEWPATTERNBYTES 0x10000
#define PREVIEWTEXTBYTES    (PREVIEWPATTERNBYTES * 4)

/** \brief  Handler for the 'value changed' event of the widgets in this dialog
 *
 * Updates the preview text widget
 */
static void on_value_changed(GtkWidget *widget, gpointer data)
{
    char printbuffer[PREVIEWTEXTBYTES];    

    ram_init_print_pattern(printbuffer, PREVIEWPATTERNBYTES, "\n");
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER(data), printbuffer, -1);  
}

/** \brief  Create widget to control RAM init settings
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_ramreset_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *start_value_widget;
    GtkWidget *value_invert_widget;
    GtkWidget *value_offset_widget;
    GtkWidget *pattern_invert_widget;
    GtkWidget *pattern_invert_value_widget;
    GtkWidget *start_random_widget;
    GtkWidget *repeat_random_widget;
    GtkWidget *chance_random_widget;
    GtkWidget *textview_widget;
    GtkTextBuffer *textview_buffer;
    GtkWidget *scrolled;
    
    grid = uihelpers_create_grid_with_label("RAM reset pattern", 2);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("Value of first byte");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    start_value_widget = vice_gtk3_resource_spin_int_new("RAMInitStartValue", 0, 255, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), start_value_widget, 1, 1, 1, 1);

    label = gtk_label_new("First byte offset");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    value_offset_widget = vice_gtk3_resource_combo_box_int_new("RAMInitValueOffset", powers_of_two);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), value_offset_widget, 1, 2, 1, 1);

    label = gtk_label_new("Invert first byte every");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    value_invert_widget = vice_gtk3_resource_combo_box_int_new(
            "RAMInitValueInvert", powers_of_two);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), value_invert_widget, 1, 3, 1, 1);

    label = gtk_label_new("Value of second byte");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    pattern_invert_value_widget = vice_gtk3_resource_spin_int_new("RAMInitPatternInvertValue", 0, 255, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), pattern_invert_value_widget, 1, 4, 1, 1);
    
    label = gtk_label_new("Invert with second byte every");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    pattern_invert_widget = vice_gtk3_resource_combo_box_int_new(
            "RAMInitPatternInvert", powers_of_two);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), pattern_invert_widget, 1, 5, 1, 1);

    label = gtk_label_new("Length of random pattern");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    start_random_widget = vice_gtk3_resource_combo_box_int_new(
            "RAMInitStartRandom", powers_of_two);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), start_random_widget, 1, 6, 1, 1);

    label = gtk_label_new("Repeat random pattern every");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    repeat_random_widget = vice_gtk3_resource_combo_box_int_new(
            "RAMInitRepeatRandom", powers_of_two);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), repeat_random_widget, 1, 7, 1, 1);

    label = gtk_label_new("Global random chance");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    chance_random_widget = vice_gtk3_resource_spin_int_new("RAMInitRandomChance", 0, 0xfff, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), chance_random_widget, 1, 8, 1, 1);
    
    label = gtk_label_new("Preview");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 9, 2, 1);
    
    textview_widget = gtk_text_view_new ();
    textview_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(textview_widget));
    on_value_changed(NULL, textview_buffer);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(textview_widget), TRUE);
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled, 400, 300);
    gtk_container_add(GTK_CONTAINER(scrolled), textview_widget);
    g_object_set(scrolled, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), scrolled, 0, 10, 2, 1);
    
    g_signal_connect(start_value_widget, "value-changed", G_CALLBACK(on_value_changed), textview_buffer);
    g_signal_connect(value_offset_widget, "changed", G_CALLBACK(on_value_changed), textview_buffer);
    g_signal_connect(value_invert_widget, "changed", G_CALLBACK(on_value_changed), textview_buffer);
    
    g_signal_connect(pattern_invert_widget, "changed", G_CALLBACK(on_value_changed), textview_buffer);
    g_signal_connect(pattern_invert_value_widget, "value-changed", G_CALLBACK(on_value_changed), textview_buffer);
    
    g_signal_connect(start_random_widget, "changed", G_CALLBACK(on_value_changed), textview_buffer);
    g_signal_connect(repeat_random_widget, "changed", G_CALLBACK(on_value_changed), textview_buffer);
    g_signal_connect(chance_random_widget, "value-changed", G_CALLBACK(on_value_changed), textview_buffer);

    gtk_widget_show_all(grid);
    return grid;
}
