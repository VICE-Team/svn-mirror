/** \file   mixerwidget.c
 * \brief   GTK3 mixer widget for emus other than VSID
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES SoundVolume         -vsid
 * $VICERES SidResidPassband    -vsid
 * $VICERES SidResidGain        -vsid
 * $VICERES SidResidFilterBias  -vsid
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

#include <stdlib.h>
#include <stdbool.h>
#include <gtk/gtk.h>

#include "vice_gtk3.h"
#include "debug.h"
#include "machine.h"
#include "lib.h"
#include "log.h"
#include "resources.h"

#include "mixerwidget.h"


/** \brief  CSS for the scales
 *
 * This makes the sliders take up less vertical space. The margin can be set
 * to a negative value (in px) to allow the slider to be larger than the scale
 * itself.
 *
 * Probably will require some testing/tweaking to get this to look acceptable
 * with various themes (and OSes).
 */
#define SLIDER_CSS "scale slider { min-width: 10px; min-height: 10px; margin: -3px; } scale { margin-top: -4px; margin-bottom: -4px; }"


/** \brief  CSS for the labels
 *
 * Make font smaller and reduce the vertical size the labels use
 *
 * Here Be Dragons!
 */
#define LABEL_CSS "label { font-size: 80%; margin-top: -2px; margin-bottom: -2px; }"


/** \brief  Main volume slider */
static GtkWidget *volume;

#ifdef HAVE_RESID

/** \brief  ReSID passband slider */
static GtkWidget *passband;

/** \brief  ReSID gain slider */
static GtkWidget *gain;

/** \brief  ReSID filter bias slider */
static GtkWidget *bias;

#endif


/** \brief  Handler for the 'clicked' event of the reset button
 *
 * Resets the slider to when the widget was created.
 *
 * \param[in]   widget  button (unused)
 * \param[in]   data    extra event data (unused)
 */
static void on_reset_clicked(GtkWidget *widget, gpointer data)
{
    int value;

    resources_get_default_value("SoundVolume", &value);
    gtk_range_set_value(GTK_RANGE(volume), (gdouble)value);
#ifdef HAVE_RESID
    resources_get_default_value("SidResidPassband", &value);
    gtk_range_set_value(GTK_RANGE(passband), (gdouble)value);
    resources_get_default_value("SidResidGain", &value);
    gtk_range_set_value(GTK_RANGE(gain), (gdouble)value);
    resources_get_default_value("SidResidFilterBias", &value);
    gtk_range_set_value(GTK_RANGE(bias), (gdouble)value);
#endif
}


/** \brief  Create a right-align label
 *
 * \param[in]   text    text for the label
 * \param[in]   minimal use CSS to reduce size of use as statusbar widget
 *
 * \return  GtkLabel
 */
static GtkWidget *create_label(const char *text, gboolean minimal,
                               GtkAlign alignment)
{
    GtkWidget *label;
    GtkCssProvider *provider;
    GtkStyleContext *context;
    GError *err = NULL;

    label = gtk_label_new(text);
    gtk_widget_set_halign(label, alignment);

    if (minimal) {
        provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(provider, LABEL_CSS, -1, &err);
        if (err != NULL) {
            fprintf(stderr, "CSS error: %s\n", err->message);
            g_error_free(err);
        }

        context = gtk_widget_get_style_context(label);
        if (context != NULL) {
            gtk_style_context_add_provider(context,
                    GTK_STYLE_PROVIDER(provider),
                    GTK_STYLE_PROVIDER_PRIORITY_USER);
        }
    }

    return label;
}


/** \brief  Create a customized GtkScale for \a resource
 *
 * \param[in]   resource    resource name without the video \a chip name prefix
 * \param[in]   low         lower bound
 * \param[in]   high        upper bound
 * \param[in]   step        step used to increase/decrease slider value
 * \param[in]   minimal     reduce slider size to be used in the statusbar
 *
 * \return  GtkScale
 */
static GtkWidget *create_slider(
        const char *resource,
        int low, int high, int step,
        gboolean minimal)
{
    GtkWidget *scale;
    GtkCssProvider *provider;
    GtkStyleContext *context;
    GError *err = NULL;

    scale = vice_gtk3_resource_scale_int_new(resource,
            GTK_ORIENTATION_HORIZONTAL, low, high, step);
    gtk_widget_set_hexpand(scale, TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
    vice_gtk3_resource_scale_int_set_marks(scale, step);

    if (minimal) {
        provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(provider, SLIDER_CSS, -1, &err);
        if (err != NULL) {
            fprintf(stderr, "CSS error: %s\n", err->message);
            g_error_free(err);
        }

        context = gtk_widget_get_style_context(scale);
        if (context != NULL) {
            gtk_style_context_add_provider(context,
                    GTK_STYLE_PROVIDER(provider),
                    GTK_STYLE_PROVIDER_PRIORITY_USER);
        }
    }

    /*    gtk_scale_set_draw_value(GTK_SCALE(scale), FALSE); */

    return scale;
}



/** \brief  Create slider for main volume
 *
 * \param[in]   minimal resize slider to minimal size
 *
 * \return  GtkScale
 */
static GtkWidget *create_volume_widget(gboolean minimal)
{
    return create_slider("SoundVolume", 0, 100, 5, minimal);
}


#ifdef HAVE_RESID

/** \brief  Create slider for ReSID passband
 *
 * \param[in]   minimal resize slider to minimal size
 *
 * \return  GtkScale
 */
static GtkWidget *create_passband_widget(gboolean minimal)
{
    return create_slider("SidResidPassBand", 0, 90, 5, minimal);
}


/** \brief  Create slider for ReSID gain
 *
 * \param[in]   minimal resize slider to minimal size
 *
 * \return  GtkScale
 */
static GtkWidget *create_gain_widget(gboolean minimal)
{
    return create_slider("SidResidGain", 90, 100, 1, minimal);
}


/** \brief  Create slider for ReSID filter bias
 *
 * \param[in]   minimal resize slider to minimal size
 *
 * \return  GtkScale
 */
static GtkWidget *create_bias_widget(gboolean minimal)
{
    return create_slider("SidResidFilterBias", -5000, 5000, 1000, minimal);
}
#endif  /* ifdef HAVE_RESID */


/** \brief  Create mixer widget
 *
 * \param[in]   minimal     minimize side of sliders and labels
 * \param[in]   alignment   alignment of labels
 *
 * \return  GtkGrid
 *
 */
GtkWidget *mixer_widget_create(gboolean minimal, GtkAlign alignment)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *button;
    int row = 0;
#ifdef HAVE_RESID
    bool sid_present = true;
    int tmp;

    if (machine_class == VICE_MACHINE_PET
            || machine_class == VICE_MACHINE_VIC20
            || machine_class == VICE_MACHINE_PLUS4) {
        /* check for presence of SidCart */
        if (resources_get_int("SidCart", &tmp) < 0) {
            debug_gtk3("failed to get value for resource SidCart, disabling.");
            sid_present = false;
        } else {
            sid_present = (bool)tmp;
        }
    }
#else
    bool sid_present = false;
#endif

    grid = vice_gtk3_grid_new_spaced(16, 0);
    g_object_set(G_OBJECT(grid), "margin-left", 8, "margin-right", 8, NULL);
    gtk_widget_set_hexpand(grid, TRUE);

    if (minimal) {
        /*
         * 'minimal' is used when this widget is used under the statusbar,
         * in which case we add a label to make the difference between the
         * CRT and the mixer controls more clear
         */
        label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label),
                "<b><small>Mixer settings</small></b>");
        gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
    }


    button = gtk_button_new_with_label("Reset");
    gtk_grid_attach(GTK_GRID(grid), button, 1, 0, 1, 1);
    gtk_widget_set_halign(button, GTK_ALIGN_END);
    gtk_widget_set_hexpand(button, FALSE);
    g_signal_connect(button, "clicked", G_CALLBACK(on_reset_clicked), NULL);
    row++;

    label = create_label("Volume", minimal, alignment);
    volume = create_volume_widget(minimal);
    gtk_widget_set_hexpand(volume, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), volume, 1,row, 1, 1);
    row++;

#ifdef HAVE_RESID
    label = create_label("ReSID Passband", minimal, alignment);
    passband = create_passband_widget(minimal);
    gtk_widget_set_sensitive(passband, sid_present);
    gtk_widget_set_hexpand(passband, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), passband, 1, row, 1, 1);
    row++;

    label = create_label("ReSID Gain", minimal, alignment);
    gain = create_gain_widget(minimal);
    gtk_widget_set_sensitive(gain, sid_present);
    gtk_widget_set_hexpand(gain, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gain, 1, row, 1, 1);
    row++;

    label = create_label("ReSID Filter Bias", minimal, alignment);
    bias = create_bias_widget(minimal);
    gtk_widget_set_hexpand(bias, TRUE);
    gtk_widget_set_sensitive(bias, sid_present);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), bias, 1, row, 1, 1);
    row++;
#endif

   gtk_widget_show_all(grid);
    return grid;
}
