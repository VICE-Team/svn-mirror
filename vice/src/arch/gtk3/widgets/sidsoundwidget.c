/** \file   sidsoundwidget.c
 * \brief   Settings for SID emulation
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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
 */

/* Note: These only make sense with a Sid Cartridge attached for certain machines
 *
 * $VICERES SidEngine                   all
 * $VICERES SidStereo                   all
 * $VICERES SidResidSampling            all
 * $VICERES SidResidPassband            all
 * $VICERES SidResidGain                all
 * $VICERES SidResidFilterBias          all
 * $VICERES SidResid8580Passband        all
 * $VICERES SidResid8580Gain            all
 * $VICERES SidResid8580FilterBias      all
 * $VICERES SidFilters                  all
 * $VICERES Sid2AddressStart            all
 * $VICERES Sid3AddressStart            all
 *
 * Until PSID files support more than three SIDs, these will be -vsid:
 * $VICERES Sid4AddressStart            -vsid
 * $VICERES Sid5AddressStart            -vsid
 * $VICERES Sid6AddressStart            -vsid
 * $VICERES Sid7AddressStart            -vsid
 * $VICERES Sid8AddressStart            -vsid
 */

#include "vice.h"
#include <gtk/gtk.h>

#include "machine.h"
#include "mixerwidget.h"
#include "resources.h"
#include "sid.h"
#include "sidenginemodelwidget.h"
#include "sound.h"
#include "vice_gtk3.h"

#include "sidsoundwidget.h"

#ifdef HAVE_RESID
/** \brief  ReSID filter slider declaration
 */
typedef struct slider_s {
    const char *label;      /**< label text */
    const char *resource;   /**< resource name */
    int         min;        /**< scale minimum value */
    int         max;        /**< scale maximum value */
    int         step;       /**< scale stepping */
} slider_t;

/** \brief  ReSID filter sliders for 6581 */
static const slider_t sliders_6581[] = {
    { "6581 passband", "SidResidPassband",           0,   90, 5 },
    { "6581 gain",     "SidResidGain",              90,  100, 1 },
    { "6581 bias",     "SidResidFilterBias",     -5000, 5000, 1 },
    { NULL,            NULL,                         0,    0, 0 }
};

/** \brief  ReSID filter sliders for 8580 */
static const slider_t sliders_8580[] = {
    { "8580 passband", "SidResid8580Passband",       0,   90, 5 },
    { "8580 gain",     "SidResid8580Gain",          90,  100, 1 },
    { "8580 bias",     "SidResid8580FilterBias", -5000, 5000, 1 },
    { NULL,            NULL,                         0,    0, 0 }
};

/** \brief  Values for the "SidResidSampling" resource
 */
static const vice_gtk3_radiogroup_entry_t resid_sampling_modes[] = {
    { "Fast",            SID_RESID_SAMPLING_FAST },
    { "Interpolation",   SID_RESID_SAMPLING_INTERPOLATION },
    { "Resampling",      SID_RESID_SAMPLING_RESAMPLING },
    { "Fast resampling", SID_RESID_SAMPLING_FAST_RESAMPLING },
    { NULL,              -1 }
};
#endif

/** \brief  I/O addresses for extra SID's for the C64 */
static int sid_addr_list_c64[] = {
    /*N/A*/ 0xd420, 0xd440, 0xd460, 0xd480, 0xd4a0, 0xd4c0, 0xd4e0,
    0xd500, 0xd520, 0xd540, 0xd560, 0xd580, 0xd5a0, 0xd5c0, 0xd5e0,
    0xd600, 0xd620, 0xd640, 0xd660, 0xd680, 0xd6a0, 0xd6c0, 0xd6e0,
    0xd700, 0xd720, 0xd740, 0xd760, 0xd780, 0xd7a0, 0xd7c0, 0xd7e0,
    0xde00, 0xde20, 0xde40, 0xde60, 0xde80, 0xdea0, 0xdec0, 0xdee0,
    0xdf00, 0xdf20, 0xdf40, 0xdf60, 0xdf80, 0xdfa0, 0xdfc0, 0xdfe0,
    -1
};

/** \brief  I/O addresses for extra SID's for the C128 */
static int sid_addr_list_c128[] = {
    /*N/A*/ 0xd420, 0xd440, 0xd460, 0xd480, 0xd4a0, 0xd4c0, 0xd4e0,
    0xd700, 0xd720, 0xd740, 0xd760, 0xd780, 0xd7a0, 0xd7c0, 0xd7e0,
    0xde00, 0xde20, 0xde40, 0xde60, 0xde80, 0xdea0, 0xdec0, 0xdee0,
    0xdf00, 0xdf20, 0xdf40, 0xdf60, 0xdf80, 0xdfa0, 0xdfc0, 0xdfe0,
    -1
};


#ifdef HAVE_RESID
/** \brief  ReSID sampling widget */
static GtkWidget *resid_sampling;

/** \brief  SID filters checkbox */
static GtkWidget *resid_filters;

/** \brief  6581 widgets grid */
static GtkWidget *resid_6581_grid;

/** \brief  8580 widgets grid */
static GtkWidget *resid_8580_grid;
#endif

/** \brief  Number of extra SIDs widget */
static GtkWidget *num_sids_widget;

/** \brief  Reference to the extra SID address widgets
 *
 * Used to enable/disable depending on the number of SIDs active
 */
static GtkWidget *address_widgets[SOUND_SIDS_MAX];


/** \brief  Set sensitivity of SID address widgets based on number of SIDs
 *
 * \param[in]   count   number of enabled SIDs
 */
static void update_sid_addresses_sensitivity(int count)
{
    if (sid_machine_can_have_multiple_sids()) {
        gtk_widget_set_sensitive(address_widgets[0], count > 0);
        gtk_widget_set_sensitive(address_widgets[1], count > 1);
        if (machine_class != VICE_MACHINE_VSID) {
            gtk_widget_set_sensitive(address_widgets[2], count > 2);
            gtk_widget_set_sensitive(address_widgets[3], count > 3);
            gtk_widget_set_sensitive(address_widgets[4], count > 4);
            gtk_widget_set_sensitive(address_widgets[5], count > 5);
            gtk_widget_set_sensitive(address_widgets[6], count > 6);
        }
    }
}

/** \brief  Extra callback registered to the 'number of SIDs' radiogroup
 *
 * \param[in]   widget  widget triggering the event
 * \param[in]   data    unused
 */
static void on_sid_count_changed(GtkWidget *widget, gpointer data)
{
    int count = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

    update_sid_addresses_sensitivity(count);
}

#ifdef HAVE_RESID
/** \brief  Extra callback for the SID engine/model widget
 *
 * \param[in]   engine  SID engine ID
 * \param[in]   model   SID model ID
 */
static void engine_model_changed_callback(int engine, int model)
{
    gboolean is_resid = (engine == SID_ENGINE_RESID);
    /* Show proper ReSID slider widgets
     *
     * We can't check old model vs new model here, since the resource
     * SidModel has already been updated.
     */
    if (model == SID_MODEL_6581) {
        gtk_widget_show(resid_6581_grid);
        gtk_widget_hide(resid_8580_grid);
    } else {
        gtk_widget_hide(resid_6581_grid);
        gtk_widget_show(resid_8580_grid);
    }

    /* Update mixer widget in the statusbar */
    mixer_widget_sid_type_changed();

    gtk_widget_set_sensitive(resid_filters,   is_resid);
    gtk_widget_set_sensitive(resid_6581_grid, is_resid);
    gtk_widget_set_sensitive(resid_8580_grid, is_resid);
    gtk_widget_set_sensitive(resid_sampling,  is_resid);
}

/** \brief  Create widget to control ReSID sampling method
 *
 * \return  GtkGrid
 */
static GtkWidget *create_resid_sampling_widget(void)
{
    GtkWidget *grid;
    GtkWidget *group;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "ReSID sampling method", 1);
    vice_gtk3_grid_set_title_margin(grid, 8);

    group = vice_gtk3_resource_radiogroup_new("SidResidSampling",
                                              resid_sampling_modes,
                                              GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_margin_start(group, 8);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}
#endif

/** \brief  Create widget to set the number of emulated SID's
 *
 * \return  GtkGrid
 */
static GtkWidget *create_num_sids_widget(void)
{
    GtkWidget *grid;
    GtkWidget *spin;
    int        max_sids = SOUND_SIDS_MAX;

    if (machine_class == VICE_MACHINE_VSID) {
        max_sids = SOUND_SIDS_MAX_PSID;
    }

    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "Extra SIDs", 2);
    vice_gtk3_grid_set_title_margin(grid, 8);

    /* create spinbutton for the 'SidStereo' resource */
    spin = vice_gtk3_resource_spin_int_new("SidStereo", 0, max_sids - 1, 1);
    num_sids_widget = spin;
    gtk_widget_set_margin_start(spin, 8);
    gtk_widget_set_halign(spin, GTK_ALIGN_START);
    gtk_widget_set_hexpand(spin, FALSE);
    g_signal_connect(spin,
                     "value-changed",
                     G_CALLBACK(on_sid_count_changed),
                     NULL);
    gtk_grid_attach(GTK_GRID(grid), spin, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Create widget for extra SID addresses
 *
 * \param[in]   sid     extra SID number (1-7)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_extra_sid_address_widget(int sid)
{
    GtkWidget *grid;
    GtkWidget *combo;
    GtkWidget *label;
    char       text[32];
    char       resource[64];

    g_snprintf(resource, sizeof resource, "SID%dAddressStart", sid + 1);
    g_snprintf(text, sizeof text, "SID #%d", sid + 1);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    label = gtk_label_new(text);
    if (machine_class == VICE_MACHINE_C128) {
        combo = vice_gtk3_resource_combo_hex_list_new(resource,
                                                      sid_addr_list_c128);
    } else {
        combo = vice_gtk3_resource_combo_hex_list_new(resource,
                                                      sid_addr_list_c64);
    }
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, 0, 1, 1);
    return grid;
}

#ifdef HAVE_RESID
/** \brief  Handler for the 'clicked' event of a reset button
 *
 * \param[in]   button  reset button (unused)
 * \param[in]   slider  slider to reset to factory
 */
static void on_reset_clicked(GtkWidget *button, gpointer slider)
{
    vice_gtk3_resource_scale_int_reset(GTK_WIDGET(slider));
}

/** \brief  Create "Reset" (to factory) button for a slider
 *
 * \param[in]   callback    callback for the button
 *
 * \return  GtkButton
 */
static GtkWidget *create_reset_button(GtkWidget *slider)
{
    GtkWidget *button;

    button = gtk_button_new_with_label("Reset");
    gtk_widget_set_valign(button, GTK_ALIGN_END);
    gtk_widget_set_hexpand(button, FALSE);

    g_signal_connect(button,
                     "clicked",
                     G_CALLBACK(on_reset_clicked),
                     (gpointer)slider);
    gtk_widget_show(button);
    return button;
}

/** \brief  Create grid with ReSID sliders
 *
 * Create grid with GtkLabel, GtkScale and GtkButton for \a slider_decls.
 *
 * \param[in]   slider_decls    slider declarations
 * \param[in]   title           grid title
 *
 * \return  GtkGrid
 */
static GtkWidget *create_sliders(const slider_t *slider_decls,
                                 const char     *title)
{
    GtkWidget *grid;
    int        i;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, title, 3);
    vice_gtk3_grid_set_title_margin(grid, 8);

    for (i = 0; slider_decls[i].label != NULL; i++) {
        GtkWidget      *label;
        GtkWidget      *scale;
        GtkWidget      *button;
        const slider_t *decl;

        decl   = &slider_decls[i];
        label  = gtk_label_new(decl->label);
        scale  = vice_gtk3_resource_scale_int_new(decl->resource,
                                                  GTK_ORIENTATION_HORIZONTAL,
                                                  decl->min,
                                                  decl->max,
                                                  decl->step);
        button = create_reset_button(scale);

        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_widget_set_margin_start(label, 8);
        gtk_widget_show(label);
        gtk_widget_set_hexpand(scale, TRUE);

        gtk_grid_attach(GTK_GRID(grid), label,  0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), scale,  1, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), button, 2, i + 1, 1, 1);
    }
    return grid;
}
#endif

/** \brief  Create grid with extra SID I/O address widgets
 *
 * \return  GtkGrid
 */
static GtkWidget *create_sid_address_widgets(void)
{
    GtkWidget *grid;
    int        column;
    int        extra;
    int        max = sid_machine_get_max_sids();

    grid = vice_gtk3_grid_new_spaced_with_label(16, 8, "SID I/O addresses", 3);

    for (extra = 1; extra < max; extra++) {
        address_widgets[extra - 1] = create_extra_sid_address_widget(extra);
    }

    /* lay out address widgets in a grid of four columns max, skip the first SID */
    extra  = 0;
    column = 1;
    while (extra < max - 1) {
        while ((column < 4) && (extra < max - 1)) {
            gtk_grid_attach(GTK_GRID(grid),
                            address_widgets[extra],
                            column, ((extra + 1) / 4) + 1, 1, 1);
            column++;
            extra++;
        }
        column = 0;
    }
    return grid;
}


/** \brief  Create widget to control SID settings
 *
 * \return  GtkGrid
 */
GtkWidget *sid_sound_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *engine;
    int        row = 0;
    int        current_engine = 0;
    int        current_model  = 0;
    int        current_stereo = 0;

    resources_get_int("SidEngine", &current_engine);
    resources_get_int("SidModel",  &current_model);
    resources_get_int("SidStereo", &current_stereo);

    grid = vice_gtk3_grid_new_spaced(8, 0);

    engine = sid_engine_model_widget_create();
#ifdef HAVE_RESID
    sid_engine_model_widget_set_callback(engine_model_changed_callback);
#endif
    gtk_grid_attach(GTK_GRID(grid), engine, 0, row, 1, 1);

#ifdef HAVE_RESID
    resid_sampling = create_resid_sampling_widget();
    gtk_grid_attach(GTK_GRID(grid), resid_sampling, 1, row, 1, 1);
#endif
    row++;

    if (sid_machine_can_have_multiple_sids()) {
        GtkWidget *num_sids;
        GtkWidget *addresses;

        num_sids  = create_num_sids_widget();
        addresses = create_sid_address_widgets();
        gtk_widget_set_margin_top(addresses, 16);
        gtk_grid_attach(GTK_GRID(grid), num_sids,  2,   0, 1, 1); /* fixed at row 0 */
        gtk_grid_attach(GTK_GRID(grid), addresses, 0, row, 3, 1);
        row++;
    }

#ifdef HAVE_RESID
    resid_filters = vice_gtk3_resource_check_button_new("SidFilters",
                                                        "Enable SID filter emulation");
    gtk_widget_set_margin_top(resid_filters, 16);
    gtk_widget_set_margin_bottom(resid_filters, 16);
    gtk_grid_attach(GTK_GRID(grid), resid_filters, 0, row, 3, 1);
    gtk_widget_set_sensitive(resid_filters, current_engine == SID_ENGINE_RESID);
    gtk_widget_set_sensitive(resid_sampling, current_engine == SID_ENGINE_RESID);

    resid_6581_grid = create_sliders(sliders_6581, "ReSID 6581 filter settings");
    resid_8580_grid = create_sliders(sliders_8580, "ReSID 8580 filter settings");
    gtk_grid_attach(GTK_GRID(grid), resid_6581_grid, 0, row + 1, 3 ,1);
    gtk_grid_attach(GTK_GRID(grid), resid_8580_grid, 0, row + 2, 3, 1);

    /* only enable appropriate widgets */
    gtk_widget_set_no_show_all(resid_6581_grid, TRUE);
    gtk_widget_set_no_show_all(resid_8580_grid, TRUE);
    gtk_widget_set_sensitive(resid_6581_grid, current_engine == SID_ENGINE_RESID);
    gtk_widget_set_sensitive(resid_8580_grid, current_engine == SID_ENGINE_RESID);
    if (current_model == SID_MODEL_6581) {
        gtk_widget_show(resid_6581_grid);
        gtk_widget_hide(resid_8580_grid);
    } else {
        gtk_widget_hide(resid_6581_grid);
        gtk_widget_show(resid_8580_grid);
    }
#endif

    if (machine_class != VICE_MACHINE_PLUS4 &&
            machine_class != VICE_MACHINE_CBM5x0 &&
            machine_class != VICE_MACHINE_CBM6x0) {
        /* set sensitivity of SID address widgets */
        update_sid_addresses_sensitivity(current_stereo);
    }

    gtk_widget_show_all(grid);
    return grid;
}
