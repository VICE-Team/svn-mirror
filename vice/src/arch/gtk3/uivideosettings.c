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

#include "uivideosettings.h"



static GtkWidget *double_size_widget = NULL;
static GtkWidget *double_scan_widget = NULL;
static GtkWidget *video_cache_widget = NULL;
static GtkWidget *vert_stretch_widget = NULL;

static char *double_size_resname = NULL;
static char *double_scan_resname = NULL;
static char *video_cache_resname = NULL;
static char *vert_stretch_resname = NULL;



static void on_destroy(GtkWidget *widget)
{
    double_size_widget = NULL;
    double_scan_widget = NULL;
    video_cache_widget = NULL;
    vert_stretch_widget = NULL;

    if (double_size_resname != NULL) {
        lib_free(double_size_resname);
    }
    if (double_scan_resname != NULL) {
        lib_free(double_scan_resname);
    }
    if (video_cache_resname != NULL) {
        lib_free(video_cache_resname);
    }
    if (vert_stretch_resname != NULL) {
        lib_free(vert_stretch_resname);
    }

}




static const char *video_chip_prefix(void)
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


static GtkWidget *create_double_size_widget(void)
{
    double_size_resname = lib_msprintf("%sDoubleSize", video_chip_prefix());
    return uihelpers_create_resource_checkbox(
            "Double size", double_size_resname);
}


static GtkWidget *create_double_scan_widget(void)
{
    double_scan_resname = lib_msprintf("%sDoubleScan", video_chip_prefix());
    return uihelpers_create_resource_checkbox(
            "Double scan", double_scan_resname);
}


static GtkWidget *create_video_cache_widget(void)
{
    video_cache_resname = lib_msprintf("%sVideoCache", video_chip_prefix());
    return uihelpers_create_resource_checkbox(
        "Video cache", video_cache_resname);
}


static GtkWidget *create_vert_stretch_widget(void)
{
    vert_stretch_resname = lib_msprintf("%sStretchVertical",
            video_chip_prefix());
    return uihelpers_create_resource_checkbox(
        "Stretch vertically", vert_stretch_resname);
}




GtkWidget *uivideosettings_widget_create(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *wrapper;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    g_object_set(layout, "margin-left", 16, NULL);

    wrapper = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(wrapper), 8);
/*    g_object_set(wrapper, "margin-left", 16, NULL);*/

    double_size_widget = create_double_size_widget();
    double_scan_widget = create_double_scan_widget();
    video_cache_widget = create_video_cache_widget();
    vert_stretch_widget = create_vert_stretch_widget();

    gtk_grid_attach(GTK_GRID(wrapper), double_size_widget, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), double_scan_widget, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), video_cache_widget, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), vert_stretch_widget, 3, 0, 1, 1);

    /* row 0, columns 0-2 */
    gtk_grid_attach(GTK_GRID(layout), wrapper, 0, 0, 3, 1);

    /* row 1, columns 0-2 */
    gtk_grid_attach(GTK_GRID(layout),
            video_palette_widget_create(video_chip_prefix()),
            0, 1, 3, 1);


    gtk_widget_show_all(layout);

    g_signal_connect(layout, "destroy", G_CALLBACK(on_destroy), NULL);
    return layout;

}

