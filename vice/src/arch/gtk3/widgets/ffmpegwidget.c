/** \file   ffmpegwidget.c
 * \brief   FFMPEG media recording options dialog
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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "debug_gtk3.h"
#include "lib.h"
#include "resources.h"
#include "machine.h"
#include "widgethelpers.h"
#include "basewidgets.h"
#include "basedialogs.h"
#include "openfiledialog.h"
#include "savefiledialog.h"
#include "selectdirectorydialog.h"
#include "ui.h"
#include "gfxoutput.h"

#include "ffmpegwidget.h"


/** \brief  FFMPEG driver info
 *
 * Retrieved via gfxoutput_get_driver(), gets cleaned up when the gfxoutput
 * sub system exits.
 */
static gfxoutputdrv_t *driver_info = NULL;


static GtkWidget *format_widget = NULL;
static GtkWidget *video_widget = NULL;
static GtkWidget *audio_widget = NULL;


/** \brief  Create a GtkListStore for the FFMPEG output formats
 *
 * \return  GtkListStore
 */
static GtkListStore *create_format_store(void)
{
    GtkListStore *store;
    GtkTreeIter iter;
    int i;

    store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);

    for (i = 0; driver_info->formatlist[i].name != NULL; i++) {
        const char *name = driver_info->formatlist[i].name;

        debug_gtk3("adding FFMPEG format '%s'\n", name);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, name, 1, i, -1);
    }

    return store;
}


/** \brief  Create a GtkListStore for the video codecs of \a fmt
 *
 * \return  GtkListStore
 */
static GtkListStore *create_video_store(int fmt)
{
    GtkListStore *store;
    GtkTreeIter iter;
    gfxoutputdrv_codec_t *codec_list;
    int i;


    codec_list = driver_info->formatlist[fmt].video_codecs;

    store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);

    for (i = 0; codec_list[i].name != NULL; i++) {
        const char *name = codec_list[i].name;
        int id = codec_list[i].id;

        debug_gtk3("adding FFMPEG video codec '%s' (%d)\n", name, id);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, name, 1, id, -1);
    }

    return store;
}


/** \brief  Create a GtkListStore for the audio codecs of \a fmt
 *
 * \return  GtkListStore
 */
static GtkListStore *create_audio_store(int fmt)
{
    GtkListStore *store;
    GtkTreeIter iter;
    gfxoutputdrv_codec_t *codec_list;
    int i;


    codec_list = driver_info->formatlist[fmt].audio_codecs;

    store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);

    for (i = 0; codec_list[i].name != NULL; i++) {
        const char *name = codec_list[i].name;
        int id = codec_list[i].id;

        debug_gtk3("adding FFMPEG audio codec '%s' (%d)\n", name, id);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, name, 1, id, -1);
    }

    return store;
}



/** \brief  Create a left-aligned, 16px indented label
 *
 * \param[in]   text    label text
 *
 * \return GtkLabel
 */
static GtkWidget *create_indented_label(const gchar *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    return label;
}


/** \brief  Create combo box with supported output formats
 *
 * \return  GtkComboBox
 */
static GtkWidget *create_format_combo_box(void)
{
    GtkWidget *combo;
    GtkListStore *store;
    GtkCellRenderer *renderer;

    store = create_format_store();
    combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
    /* combo takes ownership, so we can unref the store now */
    g_object_unref(store);

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer,
            "text", 0, NULL);
    return combo;
}


/** \brief  Update the format combo box
 *
 * \param[in]   fmt FFMPEG driver format name
 */
static void update_format_combo_box(const char * fmt)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    int index = 0;

    /* get the model and get an iterator to its first element */
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(format_widget));
    gtk_tree_model_get_iter_first(model, &iter);

    do {
        const gchar *s;

        gtk_tree_model_get(model, &iter, 0, &s, -1);

        if (strcmp(s, fmt) == 0) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(format_widget), index);
            break;
        }
        index++;
    } while (gtk_tree_model_iter_next(model, &iter));

    /* not found, set index to 0 (shouldn't happen) */
    gtk_combo_box_set_active(GTK_COMBO_BOX(format_widget), 0);
}



/** \brief  Create combo box with supported video codecs for \a fmt
 *
 * \return  GtkComboBox
 * ne
 */
static GtkWidget *create_video_combo_box(int fmt)
{
    GtkWidget *combo;
    GtkListStore *store;
    GtkCellRenderer *renderer;

    store = create_video_store(fmt);
    combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
    /* combo takes ownership, so we can unref the store now */
    g_object_unref(store);

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer,
            "text", 0, NULL);

    /* XXX: replace with resource lookup */
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    return combo;
}


/** \brief  Create combo box with supported audio codecs for \a fmt
 *
 * \return  GtkComboBox
 */
static GtkWidget *create_audio_combo_box(int fmt)
{
    GtkWidget *combo;
    GtkListStore *store;
    GtkCellRenderer *renderer;

    store = create_audio_store(fmt);
    combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
    /* combo takes ownership, so we can unref the store now */
    g_object_unref(store);

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer,
            "text", 0, NULL);

    /* XXX: replace with resource lookup */
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    return combo;
}


/** \brief  Create widget to control FFMPEG output options
 *
 * \return  GtkGrid
 */
GtkWidget *ffmpeg_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *fps;
    const char *current_format = NULL;

    /* retrieve FFMPEG driver info */
    driver_info = gfxoutput_get_driver("FFMPEG");

    /* get current FFMPEG format */
    if (resources_get_string("FFMPEGFormat", &current_format) < 0) {
        current_format = "avi"; /* hope this works out */
    }

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    /* format selection */
    label = create_indented_label("format");
    format_widget = create_format_combo_box();
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), format_widget, 1, 0, 3, 1);
    update_format_combo_box(current_format);

    /* video codec selection */
    label = create_indented_label("video codec");
    video_widget = create_video_combo_box(0);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 1, 1, 1);

    /* audio codec selection */
    label = create_indented_label("audio codec");
    audio_widget = create_audio_combo_box(0);
    gtk_grid_attach(GTK_GRID(grid), label, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), audio_widget, 3, 1, 1, 1);

    /* video codec bitrate */
    label = create_indented_label("video bitrate");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
            resource_spin_button_int_create("FFMPEGVideoBitrate",
                VICE_FFMPEG_VIDEO_RATE_MIN, VICE_FFMPEG_VIDEO_RATE_MAX,
                10000),
            1, 2, 1, 1);

    /* audio codec bitrate */
    label = create_indented_label("audio bitrate");
    gtk_grid_attach(GTK_GRID(grid), label, 2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
            resource_spin_button_int_create("FFMPEGAudioBitrate",
                VICE_FFMPEG_AUDIO_RATE_MIN, VICE_FFMPEG_AUDIO_RATE_MAX,
                1000),
            3, 2, 1, 1);

    /* half-FPS widget */
    fps = resource_check_button_create("FFMPEGVideoHalveFramerate",
            "Half framerate (25/30 FPS)"),
    gtk_widget_set_halign(fps, GTK_ALIGN_START);
    g_object_set(fps, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), fps, 0, 3, 4, 1);

    gtk_widget_show_all(grid);
    return grid;
}
