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
 * $VICERES SidFilters                  all
 *
 * These are related to ReSID
 * $VICERES SidResidPassband            all
 * $VICERES SidResidGain                all
 * $VICERES SidResidFilterBias          all
 * $VICERES SidResid8580Passband        all
 * $VICERES SidResid8580Gain            all
 * $VICERES SidResid8580FilterBias      all
 *
 * These are related to ReSIDfp
 * $VICERES SidResid6581FilterCurve     all
 * $VICERES SidResid6581FilterBias      all
 * $VICERES SidResid6581OldCaps         all
 * $VICERES SidResid8580FilterCurve     all
 * $VICERES SidResidCombinedWaveformStrength     all
 *
 * These are related to USBSID
 * $VICERES SidUSBSIDReadMode           all
 * $VICERES SidUSBSIDAudioMode          all
 * $VICERES SidUSBSIDDiffSize           all
 * $VICERES SidUSBSIDBufferSize         all
 *
 * $VICERES Sid2AddressStart            all
 * $VICERES Sid3AddressStart            all
 *
 * $VICERES Sid4AddressStart            all
 * $VICERES Sid5AddressStart            all
 * $VICERES Sid6AddressStart            all
 * $VICERES Sid7AddressStart            all
 * $VICERES Sid8AddressStart            all
 * $VICERES Sid9AddressStart            all
 * $VICERES Sid10AddressStart            all
 * $VICERES Sid2Model                   all
 * $VICERES Sid3Model                   all
 * $VICERES Sid4Model                   all
 * $VICERES Sid5Model                   all
 * $VICERES Sid6Model                   all
 * $VICERES Sid7Model                   all
 * $VICERES Sid8Model                   all
 * $VICERES Sid9Model                   all
 * $VICERES Sid10Model                  all
 */

#include "vice.h"
#include <gtk/gtk.h>
#include <math.h>

#include "machine.h"
#include "mixerwidget.h"
#include "resources.h"
#include "sid.h"
#include "sidenginemodelwidget.h"
#include "sound.h"
#include "unicodehelpers.h"
#include "vsidmixerwidget.h"
#include "vice_gtk3.h"

#include "sidsoundwidget.h"


/** \brief  CSS applied to scale to fix up value display
 *
 * Fixed width font to get proper alignment of the value display of the sliders,
 * slightly reduced font size to try to keep it roughly the same size as the
 * sans-serif fonts used in the Adwaita theme (on Gnome Shell anyway).
 */
#define SCALE_CSS \
    "scale value {\n" \
    "  font-family: monospace;\n" \
    "  font-size: 90%;\n" \
    "}"


#if defined(HAVE_RESID) || defined(HAVE_RESIDFP)
/** \brief  ReSID filter slider declaration
 */
typedef struct slider_s {
    const char *label;      /**< label text */
    const char *resource;   /**< resource name */
    const char *format;     /**< format string for Gtk.Scale::draw_value */
    int         min;        /**< scale minimum value */
    int         max;        /**< scale maximum value */
    int         resmin;     /**< resource minimum value */
    int         resmax;     /**< resource maximum value */
    float       step;       /**< scale stepping */
} slider_t;
#endif

#ifdef HAVE_RESID
/** \brief  ReSID filter sliders for 6581
 *
 * Extra leading space in format string is used to keep the slider knob from
 * touching the text.
 */
static const slider_t sliders_6581[] = {
    { "6581 passband", "SidResidPassband",       "%3.0f%%",
      RESID_6581_PASSBAND_MIN, RESID_6581_PASSBAND_MAX,
      RESID_6581_PASSBAND_MIN, RESID_6581_PASSBAND_MAX, 1 },
    { "6581 gain",     "SidResidGain",           "%3.0f%%",
      RESID_6581_FILTER_GAIN_MIN, RESID_6581_FILTER_GAIN_MAX,
      RESID_6581_FILTER_GAIN_MIN, RESID_6581_FILTER_GAIN_MAX, 1 },
    { "6581 bias",     "SidResidFilterBias",     "%+1.2fmV",
      (RESID_6581_FILTER_BIAS_MIN / RESID_6581_FILTER_BIAS_ONE),
      (RESID_6581_FILTER_BIAS_MAX / RESID_6581_FILTER_BIAS_ONE),
      RESID_6581_FILTER_BIAS_MIN, RESID_6581_FILTER_BIAS_MAX, 0.01f },
    { NULL,            NULL,                     NULL,          0,    0, 0,    0, 0 }
};

/** \brief  ReSID filter sliders for 8580 */
static const slider_t sliders_8580[] = {
    { "8580 passband", "SidResid8580Passband",   "%3.0f%%",
      RESID_8580_PASSBAND_MIN, RESID_8580_PASSBAND_MAX,
      RESID_8580_PASSBAND_MIN, RESID_8580_PASSBAND_MAX, 1 },
    { "8580 gain",     "SidResid8580Gain",       "%3.0f%%",
      RESID_8580_FILTER_GAIN_MIN, RESID_8580_FILTER_GAIN_MAX,
      RESID_8580_FILTER_GAIN_MIN, RESID_8580_FILTER_GAIN_MAX, 1 },
    { "8580 bias",     "SidResid8580FilterBias", "%+1.2fmV",
      (RESID_8580_FILTER_BIAS_MIN / RESID_8580_FILTER_BIAS_ONE),
      (RESID_8580_FILTER_BIAS_MAX / RESID_8580_FILTER_BIAS_ONE),
      RESID_8580_FILTER_BIAS_MIN, RESID_8580_FILTER_BIAS_MAX, 0.01f },
    { NULL,            NULL,                     NULL,          0,    0,0,    0,  0 }
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

#ifdef HAVE_RESIDFP
/** \brief  ReSIDfp filter sliders for 6581
 *
 * Extra leading space in format string is used to keep the slider knob from
 * touching the text.
 */
static const slider_t fp_sliders_6581[] = {
    { "6581 filter curve", "SidResid6581FilterCurve",       "%1.2f",
      RESIDFP_6581_FILTER_CURVE_MIN / RESIDFP_6581_FILTER_CURVE_ONE,
      RESIDFP_6581_FILTER_CURVE_MAX / RESIDFP_6581_FILTER_CURVE_ONE,
      RESIDFP_6581_FILTER_CURVE_MIN, RESIDFP_6581_FILTER_CURVE_MAX, 0.01f },
    { "6581 filter range", "SidResid6581FilterRange",       "%1.2f",
      RESIDFP_6581_FILTER_RANGE_MIN / RESIDFP_6581_FILTER_RANGE_ONE,
      RESIDFP_6581_FILTER_RANGE_MAX / RESIDFP_6581_FILTER_RANGE_ONE,
      RESIDFP_6581_FILTER_RANGE_MIN, RESIDFP_6581_FILTER_RANGE_MAX, 0.01f },
    { NULL,            NULL,                     NULL,          0,    0, 0,    0, 0 }
};

/** \brief  ReSIDfp filter sliders for 8580 */
static const slider_t fp_sliders_8580[] = {
    { "8580 filter curve", "SidResid8580FilterCurve",   "%1.2f",
      RESIDFP_8580_FILTER_CURVE_MIN / RESIDFP_8580_FILTER_CURVE_ONE,
      RESIDFP_8580_FILTER_CURVE_MAX / RESIDFP_8580_FILTER_CURVE_ONE,
      RESIDFP_8580_FILTER_CURVE_MIN, RESIDFP_8580_FILTER_CURVE_MAX, 0.01f },
    { NULL,            NULL,                     NULL,          0,    0,0,    0,  0 }
};

/** \brief  ReSIDfp sliders for both models */
static const slider_t fp_sliders_common[] = {
    /* padded with three figure spaces (U+2007, as wide as a digit) to the
       width of the "%1.2f" of the others, so that the sliders are as long */
    { "Combined waveform strength", "SidResidCombinedWaveformStrength",
      "\xe2\x80\x87\xe2\x80\x87\xe2\x80\x87%1.0f",
      RESIDFP_COMBINED_WAVEFORM_STRENGTH_MIN / RESIDFP_COMBINED_WAVEFORM_STRENGTH_ONE,
      RESIDFP_COMBINED_WAVEFORM_STRENGTH_MAX / RESIDFP_COMBINED_WAVEFORM_STRENGTH_ONE,
      RESIDFP_COMBINED_WAVEFORM_STRENGTH_MIN, RESIDFP_COMBINED_WAVEFORM_STRENGTH_MAX, 1.00f },
    { NULL,            NULL,                     NULL,          0,    0,0,    0,  0 }
};

/** \brief  Values for the "SidResidSampling" resource
 */
static const vice_gtk3_radiogroup_entry_t residfp_sampling_modes[] = {
    { "Linear Interpolation",   SID_RESID_SAMPLING_INTERPOLATION },
    { "SINC Resampling",        SID_RESID_SAMPLING_RESAMPLING },
    { NULL,              -1 }
};
#endif

#ifdef HAVE_USBSID
/** \brief  Values for the "SidUSBSIDDiffSize" resource
 */
static const vice_gtk3_radiogroup_entry_t us_diffsizes_radio[] = {
    { "32",   32 },
    { "64",   64 },
    { "128", 128 },
    { "256", 256 },
    { NULL,   -1 }
};

/** \brief  Values for the "SidUSBSIDBufferSize" resource
 */
static const vice_gtk3_radiogroup_entry_t us_buffsizes_radio[] = {
    { "512",     512 },
    { "1024",   1024 },
    { "2048",   2048 },
    { "4092",   4092 },
    { "8192",   8192 },
    { "16384", 16384 },
    { NULL,   -1 }
};
#endif

#if defined(HAVE_FASTSID) || defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)
/** \brief  Models for extra SIDs */
static const vice_gtk3_combo_entry_int_t sid_chip_models[] = {
    { "Same as SID #1",    SID_MODEL_SAME_AS_FIRST },
    { "6581",              SID_MODEL_6581 },
    { "8580",              SID_MODEL_8580 },
    { "8580 + digi boost", SID_MODEL_8580D },
    VICE_GTK3_COMBO_ENTRY_INT_LIST_END
};

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
#endif

static GtkWidget *sid_grid;

#if defined(HAVE_FASTSID) || defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)
/** \brief  SID filters checkbox */
static GtkWidget *sid_filters;
#endif

#ifdef HAVE_RESID
/** \brief  ReSID sampling widget */
static GtkWidget *resid_sampling;

/** \brief  6581 widgets grid */
static GtkWidget *resid_6581_grid;

/** \brief  8580 widgets grid */
static GtkWidget *resid_8580_grid;
#endif

#ifdef HAVE_RESIDFP
/** \brief  ReSIDfp sampling widget */
static GtkWidget *residfp_sampling;

/** \brief  ReSIDfp 6581 old caps checkbox */
static GtkWidget *residfp_oldcaps;

/** \brief  ReSIDfp 6581 widgets grid */
static GtkWidget *residfp_6581_grid;

/** \brief  ReSIDfp 8580 widgets grid */
static GtkWidget *residfp_8580_grid;

/** \brief  ReSIDfp sliders for both models */
static GtkWidget *residfp_common_grid;

/** \brief  ReSIDfp 6581 chip profile drop down box */
static GtkWidget *chip_profile;

/** \brief  ReSIDfp 6581 chip profile label */
static GtkWidget *chip_profile_label;

/** \brief  ReSIDfp 6581 chip profile last selected */
static int chip_profile_selected = -1;
#endif

#ifdef HAVE_USBSID
/** \brief  USBSID read mode / audio mode checkbox */
static GtkWidget *us_switches;
/** \brief  USBSID diffsizes radio buttons */
static GtkWidget *us_diffsizes;
/** \brief  USBSID buffsizes radio buttons */
static GtkWidget *us_buffsizes;
#endif

#if defined(HAVE_FASTSID) || defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)
/** \brief  Number of extra SIDs widget */
static GtkWidget *num_sids_widget;

/** \brief  Reference to the widgets of each extra SID: label, address, model
 *
 * Used to enable/disable depending on the number of SIDs active
 */
static GtkWidget *extra_sid_widgets[SOUND_SIDS_MAX][3];

static void update_filter_widgets(void);

/** \brief  Set sensitivity of SID address widgets based on number of SIDs
 *
 * \param[in]   count   number of enabled SIDs
 */
static void update_sid_addresses_sensitivity(int count)
{
    int n;
    if (sid_machine_can_have_multiple_sids()) {
        for (n = 0; n < sid_machine_get_max_sids() - 1; n++) {
            int w;

            for (w = 0; w < 3; w++) {
                if (extra_sid_widgets[n][w] != NULL) {
                    gtk_widget_set_sensitive(extra_sid_widgets[n][w], count > n);
                }
            }
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
    update_filter_widgets();
}
#endif

static GtkWidget *label_helper(const char *text)
{
    GtkWidget *label = gtk_label_new(NULL);

    gtk_label_set_markup(GTK_LABEL(label), text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    return label;
}

#if defined(HAVE_RESID)
static GtkWidget *create_resid_sampling_widget(void);
#endif
#if defined(HAVE_RESIDFP)
static GtkWidget *create_residfp_sampling_widget(void);
#endif

#if defined(HAVE_FASTSID) || defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)
/** \brief  Extra callback for the SID engine/model widget
 *
 * \param[in]   engine  SID engine ID
 * \param[in]   model   SID model ID
 */
/** \brief  Show the filter settings of each model the active SIDs use
 *
 * With SIDs of both models, both the 6581 and the 8580 settings show.
 */
static void update_filter_widgets(void)
{
    int engine = 0;
    int stereo = 0;
    int chipno;
    gboolean has_6581 = FALSE;
    gboolean has_8580 = FALSE;

    resources_get_int("SidEngine", &engine);
    if (resources_get_int("SidStereo", &stereo) < 0) {
        stereo = 0;
    }
    for (chipno = 0; chipno <= stereo && chipno < SOUND_SIDS_MAX; chipno++) {
        if (sid_get_chip_model(chipno) == SID_MODEL_6581) {
            has_6581 = TRUE;
        } else {
            has_8580 = TRUE;
        }
    }
#ifdef HAVE_RESID
    if (resid_6581_grid != NULL) {
        gtk_widget_set_visible(resid_6581_grid, engine == SID_ENGINE_RESID && has_6581);
        gtk_widget_set_visible(resid_8580_grid, engine == SID_ENGINE_RESID && has_8580);
    }
#endif
#ifdef HAVE_RESIDFP
    if (residfp_6581_grid != NULL) {
        gtk_widget_set_visible(residfp_6581_grid, engine == SID_ENGINE_RESIDFP && has_6581);
        gtk_widget_set_visible(residfp_8580_grid, engine == SID_ENGINE_RESIDFP && has_8580);
        gtk_widget_set_visible(residfp_common_grid, engine == SID_ENGINE_RESIDFP);
        /* the profile and the capacitors are about the 6581 */
        gtk_widget_set_visible(chip_profile, engine == SID_ENGINE_RESIDFP && has_6581);
        gtk_widget_set_visible(chip_profile_label, engine == SID_ENGINE_RESIDFP && has_6581);
        gtk_widget_set_visible(residfp_oldcaps, engine == SID_ENGINE_RESIDFP && has_6581);
    }
#endif
}

/** \brief  Extra callback registered to the model widget of each further SID
 *
 * \param[in]   widget  combo box (unused)
 * \param[in]   data    unused
 */
static void on_sid_chip_model_changed(GtkWidget *widget, gpointer data)
{
    update_filter_widgets();
}

static void engine_model_changed_callback(int engine, int model)
{
    gboolean is_fastsid = (engine == SID_ENGINE_FASTSID);
    gboolean is_resid = (engine == SID_ENGINE_RESID);
    gboolean is_residfp = (engine == SID_ENGINE_RESIDFP);

    /* Show proper ReSID slider widgets
     *
     * We can't check old model vs new model here, since the resource
     * SidModel has already been updated.
     */
    gtk_widget_hide(sid_filters);

#ifdef HAVE_RESID
    gtk_widget_hide(resid_6581_grid);
    gtk_widget_hide(resid_8580_grid);
#endif
#ifdef HAVE_RESIDFP
    gtk_widget_hide(residfp_6581_grid);
    gtk_widget_hide(residfp_8580_grid);
    gtk_widget_hide(chip_profile);
    gtk_widget_hide(chip_profile_label);
#endif

    if (is_residfp) {
#ifdef HAVE_RESIDFP
        gtk_widget_show(chip_profile);
        gtk_widget_show(chip_profile_label);
#endif
    }
    update_filter_widgets();

    /* Update mixer widget in the statusbar */
    mixer_widget_sid_type_changed();

    /* handle the "resampling type" widget */

    /* first remove/delete the old sampling mode widget */
#ifdef HAVE_RESID
    resid_sampling = gtk_grid_get_child_at(GTK_GRID(sid_grid), 1, 0);
    if (resid_sampling != NULL) {
        gtk_widget_destroy(resid_sampling);
    }
    resid_sampling = NULL;
#endif
#ifdef HAVE_RESIDFP
    residfp_sampling = gtk_grid_get_child_at(GTK_GRID(sid_grid), 1, 0);
    if (residfp_sampling != NULL) {
        gtk_widget_destroy(residfp_sampling);
    }
    residfp_sampling = NULL;
#endif

    /* attach both lists to same place, hide what what we don't need */
#ifdef HAVE_RESID
    if (engine == SID_ENGINE_RESID) {
        resid_sampling = create_resid_sampling_widget();
        gtk_widget_hide(resid_sampling);
        gtk_grid_attach(GTK_GRID(sid_grid), resid_sampling, 1, 0, 1, 1);
    }
#endif
#ifdef HAVE_RESIDFP
    if (engine == SID_ENGINE_RESIDFP) {
        residfp_sampling = create_residfp_sampling_widget();
        gtk_widget_hide(residfp_sampling);
        gtk_grid_attach(GTK_GRID(sid_grid), residfp_sampling, 1, 0, 1, 1);
    }
#endif

    gtk_widget_set_sensitive(sid_filters, TRUE);

#ifdef HAVE_RESID
    gtk_widget_set_sensitive(resid_6581_grid, is_resid);
    gtk_widget_set_sensitive(resid_8580_grid, is_resid);
#endif

#ifdef HAVE_RESIDFP
    gtk_widget_set_sensitive(residfp_oldcaps,   is_residfp);
    gtk_widget_set_sensitive(residfp_6581_grid, is_residfp);
    gtk_widget_set_sensitive(residfp_8580_grid, is_residfp);
    gtk_widget_set_sensitive(residfp_common_grid, is_residfp);
    gtk_widget_set_sensitive(chip_profile, is_residfp);
#endif

    if (is_residfp) {
#ifdef HAVE_RESIDFP
        gtk_widget_show(residfp_oldcaps);
        if (residfp_sampling) { gtk_widget_show(residfp_sampling); }
#endif
#ifdef HAVE_RESID
        if (resid_sampling) { gtk_widget_hide(resid_sampling); }
#endif
    } else if (is_resid) {
#ifdef HAVE_RESIDFP
        gtk_widget_hide(residfp_oldcaps);
        if (residfp_sampling) { gtk_widget_hide(residfp_sampling); }
#endif
#ifdef HAVE_RESID
        if (resid_sampling) {gtk_widget_show(resid_sampling); }
#endif
    } else if (is_fastsid) {
#ifdef HAVE_RESIDFP
        gtk_widget_hide(residfp_oldcaps);
        if (residfp_sampling) { gtk_widget_hide(residfp_sampling); }
#endif
#ifdef HAVE_RESID
        if (resid_sampling) { gtk_widget_hide(resid_sampling); }
#endif
    }
    gtk_widget_show(sid_filters);

    if (machine_class == VICE_MACHINE_VSID) {
        vsid_mixer_widget_update();
    }
}
#endif

#ifdef HAVE_RESID
/** \brief  Create widget to control ReSID sampling method
 *
 * \return  GtkGrid
 */
static GtkWidget *create_resid_sampling_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *group;

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("<b>ReSID sampling method</b>");
    group = vice_gtk3_resource_radiogroup_new("SidResidSampling",
                                              resid_sampling_modes,
                                              GTK_ORIENTATION_VERTICAL);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}
#endif


#ifdef HAVE_RESIDFP
/** \brief  Create widget to control ReSIDfp sampling method
 *
 * \return  GtkGrid
 */
static GtkWidget *create_residfp_sampling_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *group;

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("<b>ReSIDfp sampling method</b>");
    group = vice_gtk3_resource_radiogroup_new("SidResidSampling",
                                              residfp_sampling_modes,
                                              GTK_ORIENTATION_VERTICAL);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}
#endif

#ifdef HAVE_USBSID
/** \brief  Create widget to control USBSID switches
 *
 * \return  GtkGrid
 */
static GtkWidget *create_us_switches_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *us_readmode;
    GtkWidget *us_audiomode;

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("<b>USBSID options</b>");
    us_readmode = vice_gtk3_resource_check_button_new("SidUSBSIDReadMode",
                                                        "Enable read mode");
    us_audiomode = vice_gtk3_resource_check_button_new("SidUSBSIDAudioMode",
                                                        "Enable stereo mode");

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), us_readmode, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), us_audiomode, 0, 2, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Create widget to control USBSID buffer diff
 *
 * \return  GtkGrid
 */
static GtkWidget *create_us_diffsizes_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *group;

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("<b>USBSID buffer diff</b>");
    group = vice_gtk3_resource_radiogroup_new("SidUSBSIDDiffSize",
                                              us_diffsizes_radio,
                                              GTK_ORIENTATION_VERTICAL);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Create widget to control USBSID buffer size
 *
 * \return  GtkGrid
 */
static GtkWidget *create_us_buffsizes_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *group;

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("<b>USBSID buffer size</b>");
    group = vice_gtk3_resource_radiogroup_new("SidUSBSIDBufferSize",
                                              us_buffsizes_radio,
                                              GTK_ORIENTATION_VERTICAL);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}
#endif

#if defined(HAVE_FASTSID) || defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)
/** \brief  Create widget to set the number of emulated SID's
 *
 * \return  GtkGrid
 */
static GtkWidget *create_num_sids_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *spin;
    int        max_sids = SOUND_SIDS_MAX;

#if 0
    if (machine_class == VICE_MACHINE_VSID) {
        max_sids = SOUND_SIDS_MAX_PSID;
    }
#endif
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("<b>Extra SIDs</b>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    /* create spinbutton for the 'SidStereo' resource */
    spin = vice_gtk3_resource_spin_int_new("SidStereo", 0, max_sids - 1, 1);
    num_sids_widget = spin;
    gtk_widget_set_halign(spin, GTK_ALIGN_START);
    gtk_widget_set_hexpand(spin, FALSE);
    g_signal_connect_unlocked(G_OBJECT(spin),
                              "value-changed",
                              G_CALLBACK(on_sid_count_changed),
                              NULL);
    gtk_grid_attach(GTK_GRID(grid), spin, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Create the widgets of an extra SID: label, address and model
 *
 * \param[in]   sid     extra SID number, 1 for SID #2
 * \param[out]  widgets label, address combo and model combo
 */
static void create_extra_sid_widgets(int sid, GtkWidget *widgets[3])
{
    char text[32];
    char resource[64];

    g_snprintf(resource, sizeof resource, "SID%dAddressStart", sid + 1);
    g_snprintf(text, sizeof text, "SID #%d", sid + 1);

    widgets[0] = gtk_label_new(text);
    gtk_widget_set_halign(widgets[0], GTK_ALIGN_START);
    if (machine_class == VICE_MACHINE_C128) {
        widgets[1] = vice_gtk3_resource_combo_hex_new_list(resource,
                                                           sid_addr_list_c128);
    } else {
        widgets[1] = vice_gtk3_resource_combo_hex_new_list(resource,
                                                           sid_addr_list_c64);
    }
    widgets[2] = vice_gtk3_resource_combo_int_new_sprintf("Sid%dModel",
                                                          sid_chip_models,
                                                          sid + 1);
    g_signal_connect_after(G_OBJECT(widgets[2]), "changed",
                           G_CALLBACK(on_sid_chip_model_changed), NULL);
}
#endif

#if defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)

/** \brief  Handler for the 'clicked' event of a reset button
 *
 * \param[in]   button  reset button (unused)
 * \param[in]   slider  slider to reset to factory
 */
static void on_reset_clicked(GtkWidget *button, gpointer slider)
{
    vice_gtk3_resource_scale_custom_factory(GTK_WIDGET(slider));
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

    button = gtk_button_new_from_icon_name("edit-undo-symbolic",
                                           GTK_ICON_SIZE_BUTTON);
    gtk_widget_set_valign(button, GTK_ALIGN_END);
    gtk_widget_set_hexpand(button, FALSE);

    g_signal_connect(G_OBJECT(button),
                     "clicked",
                     G_CALLBACK(on_reset_clicked),
                     (gpointer)slider);
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
/** \brief  Labels of all slider blocks, one width for all */
static GtkSizeGroup *slider_label_group = NULL;

static GtkWidget *create_sliders(const slider_t *slider_decls,
                                 const char     *title)
{
    GtkWidget      *grid;
    GtkWidget      *label;
    GtkCssProvider *provider;
    int             row = 0;
    int             i;
    char            buffer[256];

    provider = vice_gtk3_css_provider_new(SCALE_CSS);
    if (provider == NULL) {
        return NULL;
    }

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    g_snprintf(buffer, sizeof buffer, "<b>%s</b>", title);
    label = label_helper(buffer);
    gtk_widget_set_margin_bottom(label, 8);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 3, 1);
    row++;

    for (i = 0; slider_decls[i].label != NULL; i++) {
        GtkWidget      *scale;
        GtkWidget      *button;
        const slider_t *decl;

        decl   = &slider_decls[i];
        label  = label_helper(decl->label);
        /* the label column is as wide in every slider block, so that the
           sliders of the 6581, the 8580 and both line up */
        if (slider_label_group == NULL) {
            slider_label_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
        }
        gtk_size_group_add_widget(slider_label_group, label);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0);
        scale = vice_gtk3_resource_scale_custom_new_printf("%s",
                                                        GTK_ORIENTATION_HORIZONTAL,
                                                        decl->resmin,
                                                        decl->resmax,
                                                        decl->min,
                                                        decl->max,
                                                        decl->step,
                                                        decl->format,
                                                        decl->resource);
        button = create_reset_button(scale);

        /* needed since we use no-show-all */
        gtk_widget_show(label);
        gtk_widget_show(button);
        gtk_widget_show(scale);

        gtk_widget_set_hexpand(scale, TRUE);
        gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);

        gtk_grid_attach(GTK_GRID(grid), label,  0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), scale,  1, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), button, 2, row, 1, 1);
        row++;
    }

    g_object_unref(provider);

    return grid;
}
#endif

#if defined(HAVE_FASTSID) || defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)
/** \brief  Create grid with extra SID I/O address widgets
 *
 * \return  GtkGrid
 */
static GtkWidget *create_sid_address_widgets(void)
{
    GtkWidget *grid;
    GtkWidget *header;
    int        max = sid_machine_get_max_sids();
    int        extras = max - 1;
    int        blocks = (extras > 6) ? 2 : 1;
    int        rows = (extras + blocks - 1) / blocks;
    int        block;
    int        extra;
    int        w;

    grid = vice_gtk3_grid_new_spaced_with_label(16, 8, "SID I/O addresses and models", 3);

    /* a table, one row for each extra SID: SID #n, address, model; more than
       six go in two blocks side by side */
    for (block = 0; block < blocks; block++) {
        header = gtk_label_new("Address");
        gtk_widget_set_halign(header, GTK_ALIGN_START);
        gtk_grid_attach(GTK_GRID(grid), header, (block * 3) + 1, 1, 1, 1);
        header = gtk_label_new("Model");
        gtk_widget_set_halign(header, GTK_ALIGN_START);
        gtk_grid_attach(GTK_GRID(grid), header, (block * 3) + 2, 1, 1, 1);
    }
    for (extra = 0; extra < extras; extra++) {
        block = extra / rows;
        create_extra_sid_widgets(extra + 1, extra_sid_widgets[extra]);
        for (w = 0; w < 3; w++) {
            gtk_grid_attach(GTK_GRID(grid), extra_sid_widgets[extra][w],
                            (block * 3) + w, (extra % rows) + 2, 1, 1);
        }
        if (block > 0) {
            gtk_widget_set_margin_start(extra_sid_widgets[extra][0], 32);
        }
    }
    return grid;
}
#endif

#ifdef HAVE_RESIDFP
typedef struct {
    char *name;
    double value;
} filter_map_t;

/* taken from https://github.com/libsidplayfp/sidplayfp/blob/c44e4c0e74c12401a6dbef0ea371e34a30de099d/src/player.cpp#L138 */
static const filter_map_t filterRangeMap[] =
{
    { "Anthony Lees",                        1.3 },
    { "Antony Crowther (Ratt)",              1.1 },
    { "Barry Leitch (The Jackal)",           0.3 },
    { "Ben Daglish",                         0.6 },
    { "Carsten Berggreen (Scarzix)",         0.7 },
    { "Charles Deenen",                      0.2 },
    { "Chris H\xc3\xbclsbeck",               0.9 },
    { "David Dunn",                          0.1 },
    { "David Dunn & Aidan Bell",             0.1 },
    { "David Whittaker",                     0.15 },
    { "Edwin van Santen",                    0.5 },
    { "Edwin van Santen & Falco Paul",       0.4 },
    { "Edwin van Santen & Venom",            0.4 },
    { "Falco Paul",                          0.15 },
    { "Falco Paul & Edwin van Santen",       0.4 },
    { "Figge Wasberger (Fegolhuzz)",         0.25 },
    { "Fred Gray",                           0.4 },
    { "Geir Tjelta",                         0.5 },
    { "Geoff Follin",                        0.85 },
    { "Georg Feil",                          0.2 },
    { "Glenn Rune Gallefoss",                1.3 },
    { "Graham Jarvis & Rob Hartshorne",      0.25 },
    { "Jason Page",                          0.35 },
    { "Jeroen Tel",                          0.35 },
    { "Johannes Bjerregaard",                0.35 },
    { "Jonathan Dunn",                       0.25 },
    { "Jouni Ikonen (Mixer)",                0.25 },
    { "Jori Olkkonen",                       0.15 },
    { "Jori Olkkonen (Yip)",                 0.35 },
    { "Kim Christensen (Future Freak)",      0.35 },
    { "Linus \xc3\x85kesson (lft)",          0.3 },
    { "Mark Cooksey",                        0.4 },
    { "Mark Wilson",                         0.2 },
    { "Markus M\xc3\xbcller (Superbrain)",   0.5 },
    { "Martin Galway",                       0.65 },
    { "Martin Walker",                       0.15 },
    { "Matt Gray",                           0.3 },
    { "Michael Hendriks",                    0.35 },
    { "Mitch & Dane",                        0.85 },
    { "M. Nilsson-Vonderburgh (Mic)",        0.3 },
    { "M. Nilsson-Vonderburgh (Mitch)",      0.3 },
    { "M. Nilsson-Vonderburgh (Yankee)",     0.3 },
    { "NM156",                               0.7 },
    { "Neil Brennan",                        0.25 },
    { "Peter Clarke",                        0.2 },
    { "Pex Tufvesson (Mahoney)",             0.35 },
    { "Pex Tufvesson (Zax)",                 0.35 },
    { "Renato Brosowski (Zoci-Joe)",         0.3 },
    { "Reyn Ouwehand",                       0.8 },
    { "Richard Joseph",                      0.3 },
    { "Rob Hubbard",                         0.35 },
    { "Russell Lieblich",                    0.25 },
    { "Stellan Andersson (Dane)",            0.85 },
    { "Steve Turner",                        0.6 },
    { "Tim Follin",                          0.5 },
    { "Thomas E. Petersen (Laxity)",         0.3 },
    { "Thomas E. Petersen (TSS)",            0.3 },
    { "Thomas Mogensen (DRAX)",              0.3 },
    { NULL, 0.0 },
};

/** \brief create "chip profile" values drop down list (create)
 */
static GtkListStore *chip_profile_combo_model_new(void)
{
    GtkListStore *model;
    int           index;

    model = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    for (index = 0; filterRangeMap[index].name != NULL; index++) {
        GtkTreeIter        iter;
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter, 0, index, 1, filterRangeMap[index].name, -1);
    }
    return model;
}

/** \brief chip profile drop down list (changed)
 */
static void on_chip_profile_changed(GtkComboBox *self, gpointer data)
{
    GtkTreeIter iter;
    double value;
    int value_int;

    if (gtk_combo_box_get_active_iter(self, &iter)) {
        GtkTreeModel *model = gtk_combo_box_get_model(self);
        int           index = -1;

        gtk_tree_model_get(model, &iter, 0, &index, -1);
        if (index >= 0) {
            value = ((filterRangeMap[index].value * 20.0f) - 1.0f) / 39.0f;
            value_int = (int)round(value * 1000.0f);
#if 0
            printf("%d: %s (%f) -> %f (%d)\n", index, filterRangeMap[index].name,
                   filterRangeMap[index].value, value, value_int);
#endif
            resources_set_int("SidResid6581FilterCurve", RESIDFP_6581_FILTER_CURVE_DEFAULT);
            resources_set_int("SidResidCombinedWaveformStrength", RESIDFP_COMBINED_WAVEFORM_STRENGTH_DEFAULT);
            resources_set_int("SidResid6581FilterRange", value_int);

            /* FIXME: we should update the sliders here, but how do we obtain the widget pointers? */
#if 0
            vice_gtk3_resource_scale_custom_set(GtkWidget *widget, RESIDFP_6581_FILTER_CURVE_DEFAULT);
            vice_gtk3_resource_scale_custom_set(GtkWidget *widget, RESIDFP_COMBINED_WAVEFORM_STRENGTH_DEFAULT);
            vice_gtk3_resource_scale_custom_set(GtkWidget *widget, value_int);
#endif
            chip_profile_selected = index; /* remember selected profile */
        }
    }

}

/** \brief create "chip profile" values drop down list
 */
static GtkWidget *chip_profile_combo_new(void)
{
    GtkWidget       *combo;
    GtkListStore    *model;
    GtkCellRenderer *renderer;

    combo    = gtk_combo_box_new();
    model    = chip_profile_combo_model_new();
    renderer = gtk_cell_renderer_text_new();

    gtk_combo_box_set_model(GTK_COMBO_BOX(combo), GTK_TREE_MODEL(model));
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer, "text", 1, NULL);
    g_signal_connect(G_OBJECT(combo),
                     "changed",
                     G_CALLBACK(on_chip_profile_changed),
                     NULL);

    if (chip_profile_selected >= 0) {
        int curve, range, strength;
        double value;
        int value_int;
        /* check if the profile value(s) still match the sliders */
        if (resources_get_int("SidResid6581FilterCurve", &curve) < 0) {
            return 0;
        }
        if (resources_get_int("SidResid6581FilterRange", &range) < 0) {
            return 0;
        }
        if (resources_get_int("SidResidCombinedWaveformStrength", &strength) < 0) {
            return 0;
        }
        value = ((filterRangeMap[chip_profile_selected].value * 20.0f) - 1.0f) / 39.0f;
        value_int = (int)round(value * 1000.0f);
        /* if the sliders were not changed, use the last selected profile */
        if ((strength == RESIDFP_COMBINED_WAVEFORM_STRENGTH_DEFAULT) &&
            (curve == RESIDFP_6581_FILTER_CURVE_DEFAULT) &&
            (range == value_int)) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(combo), chip_profile_selected);
        }
    }
    return combo;
}

#endif


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
    int        current_stereo = 0;

    resources_get_int("SidEngine", &current_engine);
    resources_get_int("SidStereo", &current_stereo);

    sid_grid = grid = vice_gtk3_grid_new_spaced(8, 0);

    engine = sid_engine_model_widget_create();
#if defined(HAVE_FASTSID) || defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)
    sid_engine_model_widget_set_callback(engine_model_changed_callback);
#endif
    gtk_grid_attach(GTK_GRID(grid), engine, 0, row, 1, 1);

#ifdef HAVE_RESID
    resid_sampling = gtk_grid_get_child_at(GTK_GRID(grid), 1, row);
    if (resid_sampling != NULL) {
        gtk_widget_destroy(resid_sampling);
    }
    resid_sampling = NULL;
#endif
#ifdef HAVE_RESIDFP
    residfp_sampling = gtk_grid_get_child_at(GTK_GRID(grid), 1, row);
    if (residfp_sampling != NULL) {
        gtk_widget_destroy(residfp_sampling);
    }
    residfp_sampling = NULL;
#endif

    /* attach both lists to same place, hide what what we don't need */
#ifdef HAVE_RESID
    if (current_engine == SID_ENGINE_RESID) {
        resid_sampling = create_resid_sampling_widget();
        gtk_grid_attach(GTK_GRID(grid), resid_sampling, 1, row, 1, 1);
    }
#endif
#ifdef HAVE_RESIDFP
    if (current_engine == SID_ENGINE_RESIDFP) {
        residfp_sampling = create_residfp_sampling_widget();
        gtk_grid_attach(GTK_GRID(grid), residfp_sampling, 1, row, 1, 1);
    }
#endif

#if defined(HAVE_FASTSID) || defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)
    if (sid_machine_can_have_multiple_sids()) {
        GtkWidget *num_sids;
        GtkWidget *addresses;

        row++;
        num_sids  = create_num_sids_widget();
        addresses = create_sid_address_widgets();
        gtk_widget_set_margin_top(addresses, 16);
        gtk_grid_attach(GTK_GRID(grid), num_sids,  2,   0, 1, 1); /* fixed at row 0 */
        gtk_grid_attach(GTK_GRID(grid), addresses, 0, row, 3, 1);
    }
#endif

#if defined(HAVE_FASTSID) || defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)
    row++;
    sid_filters = vice_gtk3_resource_check_button_new("SidFilters",
                                                        "Enable SID filter emulation");
    gtk_widget_set_margin_top(sid_filters, 16);
    gtk_widget_set_margin_bottom(sid_filters, 16);
    gtk_grid_attach(GTK_GRID(grid), sid_filters, 0, row, 1, 1);

    gtk_widget_hide(sid_filters);
#endif

#if defined(HAVE_FASTSID)
    if (current_engine == SID_ENGINE_FASTSID) {
        gtk_widget_show(sid_filters);
    }
#endif
#if defined(HAVE_RESID)
    if (current_engine == SID_ENGINE_RESID) {
        gtk_widget_show(sid_filters);
    }
#endif
#if defined(HAVE_RESIDFP)
    if (current_engine == SID_ENGINE_RESIDFP) {
        gtk_widget_show(sid_filters);
    }
#endif

#if defined(HAVE_RESID) || defined(HAVE_RESID_DTV)
    resid_6581_grid = create_sliders(sliders_6581, "ReSID 6581 filter settings");
    resid_8580_grid = create_sliders(sliders_8580, "ReSID 8580 filter settings");
    gtk_grid_attach(GTK_GRID(grid), resid_6581_grid, 0, row + 1, 3 ,1);
    gtk_grid_attach(GTK_GRID(grid), resid_8580_grid, 0, row + 2, 3, 1);

    /* only enable appropriate widgets */
    gtk_widget_set_no_show_all(resid_6581_grid, TRUE);
    gtk_widget_set_no_show_all(resid_8580_grid, TRUE);
    gtk_widget_set_sensitive(resid_6581_grid, current_engine == SID_ENGINE_RESID);
    gtk_widget_set_sensitive(resid_8580_grid, current_engine == SID_ENGINE_RESID);
#endif

#ifdef HAVE_RESIDFP
    residfp_oldcaps = vice_gtk3_resource_check_button_new("SidResid6581OldCaps",
                                                        "Old 6581 2200pf filter capacitors");
    gtk_widget_set_margin_top(residfp_oldcaps, 16);
    gtk_widget_set_margin_bottom(residfp_oldcaps, 16);
    gtk_grid_attach(GTK_GRID(grid), residfp_oldcaps, 1, row , 1, 1);
    gtk_widget_set_sensitive(residfp_oldcaps, current_engine == SID_ENGINE_RESIDFP);

    chip_profile_label = label_helper("ReSIDfp 6581 profile");
    gtk_grid_attach(GTK_GRID(grid), chip_profile_label, 0, row + 1, 1, 1);
    chip_profile = chip_profile_combo_new();
    gtk_widget_set_margin_top(chip_profile, 16);
    gtk_widget_set_margin_bottom(chip_profile, 16);
    gtk_grid_attach(GTK_GRID(grid), chip_profile, 1, row + 1 , 1, 1);
    gtk_widget_set_sensitive(chip_profile, current_engine == SID_ENGINE_RESIDFP);

    residfp_6581_grid = create_sliders(fp_sliders_6581, "ReSIDfp 6581 filter settings");
    residfp_8580_grid = create_sliders(fp_sliders_8580, "ReSIDfp 8580 filter settings");
    residfp_common_grid = create_sliders(fp_sliders_common, "ReSIDfp settings for both models");
    gtk_grid_attach(GTK_GRID(grid), residfp_6581_grid, 0, row + 2, 3 ,1);
    gtk_grid_attach(GTK_GRID(grid), residfp_8580_grid, 0, row + 3, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), residfp_common_grid, 0, row + 4, 3, 1);
    gtk_widget_set_no_show_all(residfp_common_grid, TRUE);

    /* only enable appropriate widgets */
    gtk_widget_set_no_show_all(residfp_6581_grid, TRUE);
    gtk_widget_set_no_show_all(residfp_8580_grid, TRUE);
    gtk_widget_set_sensitive(residfp_6581_grid, current_engine == SID_ENGINE_RESIDFP);
    gtk_widget_set_sensitive(residfp_8580_grid, current_engine == SID_ENGINE_RESIDFP);

    if (current_engine == SID_ENGINE_RESIDFP) {
        gtk_widget_show(residfp_oldcaps);
        gtk_widget_show(chip_profile);
        gtk_widget_show(chip_profile_label);
    } else {
        gtk_widget_hide(residfp_oldcaps);
        gtk_widget_hide(chip_profile);
        gtk_widget_hide(chip_profile_label);
    }
#endif


/* FIXME: HardSID options are missing */

#ifdef HAVE_USBSID
    us_switches = create_us_switches_widget();
    us_diffsizes = create_us_diffsizes_widget();
    us_buffsizes = create_us_buffsizes_widget();

    gtk_grid_attach(GTK_GRID(grid), us_switches, 0, row + 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), us_diffsizes, 1, row + 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), us_buffsizes, 2, row + 5, 1, 1);

    gtk_widget_set_sensitive(us_switches, current_engine == SID_ENGINE_USBSID);
    gtk_widget_set_sensitive(us_diffsizes, current_engine == SID_ENGINE_USBSID);
    gtk_widget_set_sensitive(us_buffsizes, current_engine == SID_ENGINE_USBSID);
#endif

#if defined(HAVE_FASTSID) || defined(HAVE_RESID) || defined(HAVE_RESID_DTV) || defined(HAVE_RESIDFP)
    if (machine_class != VICE_MACHINE_PLUS4 &&
            machine_class != VICE_MACHINE_CBM5x0 &&
            machine_class != VICE_MACHINE_CBM6x0) {
        /* set sensitivity of SID address widgets */
        update_sid_addresses_sensitivity(current_stereo);
    }
#endif

    gtk_widget_show_all(grid);
    update_filter_widgets();
    return grid;
}
