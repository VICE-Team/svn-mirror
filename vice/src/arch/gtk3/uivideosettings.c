/** \file   src/arch/gtk3/uivideosettings.c
 * \brief   Widget to control video settings
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  CrtcDoubleSize
 *  TEDDoubleSize
 *  VDCDoubleSize
 *  VICDoubleSize
 *  VICIIDoubleSize
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
 */

#include "vice.h"

#include <gtk/gtk.h>
#include <stdlib.h>

#include "lib.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "machine.h"
#include "videopalettewidget.h"
#include "videorenderfilterwidget.h"
#include "videobordermodewidget.h"

#include "uivideosettings.h"

static char *widget_title[2] = { NULL, NULL };
static const char *chip_name[2] = { NULL, NULL };


static char *double_size_resname[2] = { NULL, NULL };
static char *double_scan_resname[2] = { NULL, NULL };
static char *video_cache_resname[2] = { NULL, NULL };
static char *vert_stretch_resname[2] = { NULL, NULL };
static char *audio_leak_resname[2] = { NULL, NULL };
static char *sprite_sprite_resname[2] = { NULL, NULL };
static char *sprite_background_resname[2] = { NULL, NULL };
static char *vsp_bug_resname[2] = { NULL, NULL };

static void on_destroy(GtkWidget *widget)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (double_size_resname[i] != NULL) {
            lib_free(double_size_resname[i]);
        }
        if (double_scan_resname[i] != NULL) {
            lib_free(double_scan_resname[i]);
        }
        if (video_cache_resname[i] != NULL) {
            lib_free(video_cache_resname[i]);
        }
        if (vert_stretch_resname[i] != NULL) {
            lib_free(vert_stretch_resname[i]);
        }
        if (audio_leak_resname[i] != NULL) {
            lib_free(audio_leak_resname[i]);
        }
        if (sprite_sprite_resname[i] != NULL) {
            lib_free(sprite_sprite_resname[i]);
        }
        if (sprite_background_resname[i] != NULL) {
            lib_free(sprite_background_resname[i]);
        }
        if (vsp_bug_resname[i] != NULL) {
            lib_free(vsp_bug_resname[i]);
        }
        if (widget_title[i] != NULL) {
            lib_free(widget_title[i]);
        }
    }
}


static const char *get_video_chip_name(void)
{
    switch (machine_class) {
        /* VIC */
        case VICE_MACHINE_VIC20:
            return "VIC";

        /* VIC-II */
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C64DTV:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_CBM5x0:   /* fall through */
        case VICE_MACHINE_VSID:
            return "VICII";

        /* TED */
        case VICE_MACHINE_PLUS4:
            return "TED";

        /* CRTC */
        case VICE_MACHINE_PET:      /* fall through */
        case VICE_MACHINE_CBM6x0:
            return "Crtc";

        default:
            /* should never get here */
            fprintf(stderr, "%s:%d:%s(): error: got machine class %d\n",
                    __FILE__, __LINE__, __func__, machine_class);
            exit(1);
    }
}


static GtkWidget *create_double_size_widget(int index)
{
    double_size_resname[index] = lib_msprintf("%sDoubleSize", chip_name[index]);
    return uihelpers_create_resource_checkbox(
            "Double size", double_size_resname[index]);
}


static GtkWidget *create_double_scan_widget(int index)
{
    double_scan_resname[index] = lib_msprintf("%sDoubleScan", chip_name[index]);
    return uihelpers_create_resource_checkbox(
            "Double scan", double_scan_resname[index]);
}


static GtkWidget *create_video_cache_widget(int index)
{
    video_cache_resname[index] = lib_msprintf("%sVideoCache", chip_name[index]);
    return uihelpers_create_resource_checkbox(
        "Video cache", video_cache_resname[index]);
}


static GtkWidget *create_vert_stretch_widget(int index)
{
    vert_stretch_resname[index] = lib_msprintf("%sStretchVertical",
            chip_name[index]);
    return uihelpers_create_resource_checkbox(
        "Stretch vertically", vert_stretch_resname[index]);
}


static GtkWidget *create_audio_leak_widget(int index)
{
    audio_leak_resname[index] = lib_msprintf("%sAudioLeak", chip_name[index]);
    return uihelpers_create_resource_checkbox(
            "Audio leak emulation", audio_leak_resname[index]);
}


static GtkWidget *create_sprite_sprite_widget(int index)
{
    /* really fucked up resource name by the way */
    sprite_sprite_resname[index] = lib_msprintf("%sCheckSsColl",
            chip_name[index]);
    return uihelpers_create_resource_checkbox(
            "Sprite-sprite collisions", sprite_sprite_resname[index]);
}


static GtkWidget *create_sprite_background_widget(int index)
{
    /* really fucked up resource name by the way */
    sprite_background_resname[index] = lib_msprintf("%sCheckSbColl",
            chip_name[index]);
    return uihelpers_create_resource_checkbox(
            "Sprite-background collisions", sprite_background_resname[index]);
}


static GtkWidget *create_vsp_bug_widget(int index)
{
    vsp_bug_resname[index] = lib_msprintf("%sVSPBug", chip_name[index]);
    return uihelpers_create_resource_checkbox(
            "VSP bug emulation", vsp_bug_resname[index]);
}


static GtkWidget *create_layout(GtkWidget *parent, const char *chip, int index)
{
    GtkWidget *layout;
    GtkWidget *wrapper;
    GtkWidget *double_size_widget = NULL;
    GtkWidget *double_scan_widget = NULL;
    GtkWidget *video_cache_widget = NULL;
    GtkWidget *vert_stretch_widget = NULL;
    GtkWidget *video_render_widget = NULL;
    GtkWidget *audio_leak_widget = NULL;
    GtkWidget *sprite_sprite_widget = NULL;
    GtkWidget *sprite_background_widget = NULL;
    GtkWidget *vsp_bug_widget = NULL;

    widget_title[index] = lib_msprintf("%s Settings", chip);
    chip_name[index] = chip;

    /* row 0, columns 0-2 */
    layout = uihelpers_create_grid_with_label(widget_title[index], 3);
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    g_object_set(layout, "margin-left", 16, NULL);

    wrapper = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(wrapper), 8);

    double_size_widget = create_double_size_widget(index);
    g_object_set(double_size_widget, "margin-left", 16, NULL);
    double_scan_widget = create_double_scan_widget(index);
    video_cache_widget = create_video_cache_widget(index);
    vert_stretch_widget = create_vert_stretch_widget(index);
    audio_leak_widget = create_audio_leak_widget(index);
    sprite_sprite_widget = create_sprite_sprite_widget(index);
    sprite_background_widget = create_sprite_background_widget(index);
    vsp_bug_widget = create_vsp_bug_widget(index);


    gtk_widget_set_vexpand(audio_leak_widget, FALSE);

    gtk_grid_attach(GTK_GRID(wrapper), double_size_widget, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), double_scan_widget, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), video_cache_widget, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), vert_stretch_widget, 3, 0, 1, 1);

    /* row 1, columns 0-2 */
    gtk_grid_attach(GTK_GRID(layout), wrapper, 0, 1, 3, 1);

    /* row 2, columns 0-2 */
    gtk_grid_attach(GTK_GRID(layout),
            video_palette_widget_create(chip),
            0, 2, 3, 1);

    /* row 3, column 0 */
    gtk_grid_attach(GTK_GRID(layout),
            video_render_filter_widget_create(chip),
            0, 3, 1, 1);
    /* row 3, column 1 */
    if (machine_class != VICE_MACHINE_PET
            && machine_class != VICE_MACHINE_CBM6x0) {
        /* add border mode widget */
        gtk_grid_attach(GTK_GRID(layout),
                video_border_mode_widget_create(chip),
                1, 3, 1, 1);
    }
    /* row 3, column2 */
    wrapper = uihelpers_create_grid_with_label("Other shit", 1);
    gtk_grid_set_column_spacing(GTK_GRID(wrapper), 8);

    gtk_grid_attach(GTK_GRID(wrapper), audio_leak_widget, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), sprite_sprite_widget, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), sprite_background_widget, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), vsp_bug_widget, 0, 4, 1, 1);
    if (machine_class != VICE_MACHINE_C64SC
            && machine_class != VICE_MACHINE_SCPU64) {
        gtk_widget_set_sensitive(vsp_bug_widget, FALSE);
    }
    gtk_grid_attach(GTK_GRID(layout), wrapper, 2, 3, 1, 1);

    gtk_widget_show_all(layout);

    return layout;
}






GtkWidget *uivideosettings_widget_create(GtkWidget *parent)
{
    GtkWidget *grid = gtk_grid_new();
    const char *chip = get_video_chip_name();

    gtk_grid_set_row_spacing(GTK_GRID(grid), 16);

    gtk_grid_attach(GTK_GRID(grid),
            create_layout(parent, chip, 0),
            0, 0, 1, 1);
    if (machine_class == VICE_MACHINE_C128) {
        gtk_grid_attach(GTK_GRID(grid),
                create_layout(parent, "VDC", 1),
                0, 1, 1, 1);
    }


    g_signal_connect(grid, "destroy", G_CALLBACK(on_destroy), NULL);
    gtk_widget_show_all(grid);
    return grid;
}

