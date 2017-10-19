/** \file   src/arch/gtk3/uivideosettings.c
 * \brief   Widget to control video settings
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  CrtcDoubleSize
 *  CrtcDoubleScan
 *  CrtcVideoCache
 *  CrtcStretchVertical
 *  CrtcAudioLeak
 *  CrtcHwScale
 *  TEDDoubleSize
 *  TEDDoubleScan
 *  TEDVideoCache
 *  TEDAudioLeak
 *  TEDHwScale
 *  VDCDoubleSize
 *  VDCDoubleScan
 *  VDCVideoCache
 *  VDCStretchVertical
 *  VDCAudioLeak
 *  VDCHwScale
 *  VICDoubleSize
 *  VICDoubleScan
 *  VICVideoCache
 *  VICAudioLeak
 *  VICHwScale
 *  VICIIDoubleSize
 *  VICIIDoubleScan
 *  VICIIVideoCache
 *  VICIIAudioLeak
 *  VICIICheckSbColl        - check sprite-background collisions
 *  VICIICheckSsColl        - check sprite-sprite collisions
 *  VICIIVSPBug             - emulate VSP bug
 *  VICIIHwScale
 *  KeepAspectRatio
 *  TrueAspectRatio
 *
 *  (see included widgets for more resources)
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
#include "resourcecheckbutton.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "machine.h"
#include "uivideo.h"
#include "videopalettewidget.h"
#include "videorenderfilterwidget.h"
#include "videobordermodewidget.h"

#include "uivideosettings.h"


/** \brief  Heap allocated titles for the sub-widgets
 */
static char *widget_title[2] = { NULL, NULL };

/** \brief  References to the chip names passed to create_layout()
 */
static const char *chip_name[2] = { NULL, NULL };

/*
 * A bunch of heap allocated resource names to use in the calls to
 * uihelpers_create_resource_checkbox(). Since that function only passes a
 * pointer to the resource name to its event handler, we can't use a temp buffer
 * to set up the resource names, these need to live on the heap until the
 * checkbox is destroyed.
 */
static char *double_size_resname[2] = { NULL, NULL };
static char *double_scan_resname[2] = { NULL, NULL };
static char *video_cache_resname[2] = { NULL, NULL };
static char *vert_stretch_resname[2] = { NULL, NULL };
static char *audio_leak_resname[2] = { NULL, NULL };
static char *sprite_sprite_resname[2] = { NULL, NULL };
static char *sprite_background_resname[2] = { NULL, NULL };
static char *vsp_bug_resname[2] = { NULL, NULL };
static char *hw_scale_resname[2] = { NULL, NULL };
static char *keep_aspect_resname[2] = { NULL, NULL };
static char *true_aspect_resname[2] = { NULL, NULL };


/* These are required for x128, since these resources are chip-independent, but
 * there's a TODO to make these resources chip-dependent. For now toggling a
 * checkbox in VICII settings should update the checkbox in VDC settings, and
 * vice-versa. Once the resources are chip-dependent, this can be removed.
 */
static GtkWidget *keep_aspect_widget[2] = { NULL, NULL };
static GtkWidget *true_aspect_widget[2] = { NULL, NULL };



/** \brief  Handler for the "destroy" event of the main widget
 *
 * Cleans up heap-allocated resources
 *
 * \param[in]   widget  main widget
 */
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
        if (hw_scale_resname[i] != NULL) {
            lib_free(hw_scale_resname[i]);
        }
        if (keep_aspect_resname[i] != NULL) {
            lib_free(keep_aspect_resname[i]);
        }
        if (true_aspect_resname[i] != NULL) {
            lib_free(true_aspect_resname[i]);
        }
        if (widget_title[i] != NULL) {
            lib_free(widget_title[i]);
        }
    }
}




static GtkWidget *create_double_size_widget(int index)
{
    double_size_resname[index] = lib_msprintf("%sDoubleSize", chip_name[index]);
    return resource_check_button_create(
            double_size_resname[index],
            "Double size");
}


static GtkWidget *create_double_scan_widget(int index)
{
    double_scan_resname[index] = lib_msprintf("%sDoubleScan", chip_name[index]);
    return resource_check_button_create(
            double_scan_resname[index],
            "Double scan");
}


static GtkWidget *create_video_cache_widget(int index)
{
    video_cache_resname[index] = lib_msprintf("%sVideoCache", chip_name[index]);
    return resource_check_button_create(
            video_cache_resname[index],
            "Video cache");
}


static GtkWidget *create_vert_stretch_widget(int index)
{
    vert_stretch_resname[index] = lib_msprintf("%sStretchVertical",
            chip_name[index]);
    return resource_check_button_create(
            vert_stretch_resname[index],
            "Stretch vertically");
}


static GtkWidget *create_audio_leak_widget(int index)
{
    audio_leak_resname[index] = lib_msprintf("%sAudioLeak", chip_name[index]);
    return resource_check_button_create(
            audio_leak_resname[index],
            "Audio leak emulation");
}


static GtkWidget *create_sprite_sprite_widget(int index)
{
    /* really fucked up resource name by the way */
    sprite_sprite_resname[index] = lib_msprintf("%sCheckSsColl",
            chip_name[index]);
    return resource_check_button_create(
            sprite_sprite_resname[index],
            "Sprite-sprite collisions");
}


static GtkWidget *create_sprite_background_widget(int index)
{
    /* really fucked up resource name by the way */
    sprite_background_resname[index] = lib_msprintf("%sCheckSbColl",
            chip_name[index]);
    return resource_check_button_create(
            sprite_background_resname[index],
            "Sprite-background collisions");
}


static GtkWidget *create_vsp_bug_widget(int index)
{
    vsp_bug_resname[index] = lib_msprintf("%sVSPBug", chip_name[index]);
    return resource_check_button_create(
            vsp_bug_resname[index],
            "VSP bug emulation");
}


static GtkWidget *create_hw_scale_widget(int index)
{
    hw_scale_resname[index] = lib_msprintf("%sHwScale", chip_name[index]);
    return resource_check_button_create(
            hw_scale_resname[index],
            "Hardware scaling");
}


static GtkWidget *create_keep_aspect_widget(int index)
{
    /* change lib_stralloc() to lib_msprintf("%sKeepAspectRatio", chip_name[i])
     * once per-chip KeepAspectRatio is implemented */
    keep_aspect_resname[index] = lib_stralloc("KeepAspectRatio");
    return resource_check_button_create(
            keep_aspect_resname[index],
            "Keep aspect ratio");
}

static GtkWidget *create_true_aspect_widget(int index)
{
    /* change lib_stralloc() to lib_msprintf("%sTrueAspectRatio", chip_name[i])
     * once per-chip TrueAspectRatio is implemented */
    true_aspect_resname[index] = lib_stralloc("TrueAspectRatio");
    return resource_check_button_create(
            true_aspect_resname[index],
            "True aspect ratio");
}


static void on_hw_scale_toggled(GtkWidget *check, gpointer user_data)
{
    int enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));
    int index = GPOINTER_TO_INT(user_data);

    gtk_widget_set_sensitive(keep_aspect_widget[index], enabled);
    gtk_widget_set_sensitive(true_aspect_widget[index], enabled);
}


static void on_keep_aspect_toggled(GtkWidget *check, gpointer user_data)
{
    int index = GPOINTER_TO_INT(user_data);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(keep_aspect_widget[index]),
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check)));
}


static void on_true_aspect_toggled(GtkWidget *check, gpointer user_data)
{
    int index = GPOINTER_TO_INT(user_data);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(true_aspect_widget[index]),
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check)));
}


/** \brief  Create a per-chip video settings layout
 *
 * \param[in]   parent  parent widget, required for dialogs
 * \param[in]   chip    chip name ("Crtc", "TED", "VDC", "VIC", or "VICII")
 * \param[in[   index   index in the general layout (0 or 1 (x128))
 *
 * \return  GtkGrid
 */
static GtkWidget *create_layout(GtkWidget *parent, const char *chip, int index)
{
    GtkWidget *layout;
    GtkWidget *wrapper;
    GtkWidget *double_size_widget = NULL;
    GtkWidget *double_scan_widget = NULL;
    GtkWidget *video_cache_widget = NULL;
    GtkWidget *vert_stretch_widget = NULL;
    GtkWidget *audio_leak_widget = NULL;
    GtkWidget *sprite_sprite_widget = NULL;
    GtkWidget *sprite_background_widget = NULL;
    GtkWidget *vsp_bug_widget = NULL;
    GtkWidget *hw_scale_widget = NULL;

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
    if (uivideo_chip_has_vert_stretch(chip)) {
        vert_stretch_widget = create_vert_stretch_widget(index);
    }
    audio_leak_widget = create_audio_leak_widget(index);
    if (uivideo_chip_has_sprites(chip)) {
        sprite_sprite_widget = create_sprite_sprite_widget(index);
        sprite_background_widget = create_sprite_background_widget(index);
    }
    if (uivideo_chip_has_vsp_bug(chip)) {
        vsp_bug_widget = create_vsp_bug_widget(index);
    }

    gtk_widget_set_vexpand(audio_leak_widget, FALSE);

    gtk_grid_attach(GTK_GRID(wrapper), double_size_widget, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), double_scan_widget, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), video_cache_widget, 2, 0, 1, 1);
    if (uivideo_chip_has_vert_stretch(chip)) {
        gtk_grid_attach(GTK_GRID(wrapper), vert_stretch_widget, 3, 0, 1, 1);
    }

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
    if (uivideo_chip_has_border_mode(chip)) {
        /* add border mode widget */
        gtk_grid_attach(GTK_GRID(layout),
                video_border_mode_widget_create(chip),
                1, 3, 1, 1);
    }
    /* row 3, column2 */
    wrapper = uihelpers_create_grid_with_label("Miscellaneous", 1);
    gtk_grid_set_column_spacing(GTK_GRID(wrapper), 8);

    gtk_grid_attach(GTK_GRID(wrapper), audio_leak_widget, 0, 1, 1, 1);
    if (uivideo_chip_has_sprites(chip)) {
        gtk_grid_attach(GTK_GRID(wrapper), sprite_sprite_widget, 0, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(wrapper), sprite_background_widget, 0, 3, 1, 1);
    }
    if (uivideo_chip_has_vsp_bug(chip)) {
        gtk_grid_attach(GTK_GRID(wrapper), vsp_bug_widget, 0, 4, 1, 1);
    }

    gtk_grid_attach(GTK_GRID(layout), wrapper, 2, 3, 1, 1);


    /* row 4, column 0-2 */
    wrapper = uihelpers_create_grid_with_label("Scaling and fullscreen", 3);

    hw_scale_widget = create_hw_scale_widget(index);
    gtk_grid_attach(GTK_GRID(wrapper), hw_scale_widget, 0, 1, 1, 1);

    keep_aspect_widget[index] = create_keep_aspect_widget(index);
    /* until per-chip KeepAspectRatio is implemented, connect the VICII and
     * VDC KeepAspectRatio checkboxes, so toggling the VICII checkbox also
     * updates the VDC checkbox, and vice-versa */
    if (machine_class == VICE_MACHINE_C128) {
        g_signal_connect(keep_aspect_widget[index], "toggled",
                G_CALLBACK(on_keep_aspect_toggled),
                GINT_TO_POINTER(index == 0 ? 1: 0));
    }
    gtk_grid_attach(GTK_GRID(wrapper), keep_aspect_widget[index], 1 ,1 ,1, 1);

    true_aspect_widget[index] = create_true_aspect_widget(index);
    /* until per-chip TrueAspectRatio is implemented, connect the VICII and
     * VDC TrueAspectRatio checkboxes, so toggling the VICII checkbox also
     * updates the VDC checkbox, and vice-versa */
    if (machine_class == VICE_MACHINE_C128) {
        g_signal_connect(true_aspect_widget[index], "toggled",
                G_CALLBACK(on_true_aspect_toggled),
                GINT_TO_POINTER(index == 0 ? 1: 0));
    }

    gtk_grid_attach(GTK_GRID(wrapper), true_aspect_widget[index], 2 ,1 ,1, 1);

    g_signal_connect(hw_scale_widget, "toggled",
            G_CALLBACK(on_hw_scale_toggled), GINT_TO_POINTER(index));

    gtk_grid_attach(GTK_GRID(layout), wrapper, 0, 4, 3, 1);

    gtk_widget_show_all(layout);
    return layout;
}


/** \brief  Create video settings widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *uivideosettings_widget_create(GtkWidget *parent)
{
    GtkWidget *grid = gtk_grid_new();
    const char *chip = uivideo_chip_name();

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
