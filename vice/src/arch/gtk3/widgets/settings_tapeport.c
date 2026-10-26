/** \file   settings_tapeport.c
 * \brief   Tape port settings dialog widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES TrapDevice1                     -xscpu64 -vsid
 * $VICERES TrapDevice2                     -xscpu64 -vsid -x64sc -x64 -xvic -xplus4 -xcbm2 -xcbm5x0
 * $VICERES TapePort1Device                 -xscpu64 -vsid
 * $VICERES TapePort2Device                 -xscpu64 -vsid -x64sc -x64 -xvic -xplus4 -xcbm2 -xcbm5x0
 * $VICERES DatasetteResetWithCPU           -xscpu64 -vsid
 * $VICERES DatasetteSound                  -xscpu64 -vsid
 * $VICERES DatasetteSoundVolume            -xscpu64 -vsid
 * $VICERES DatasetteSpeedTuning            -xscpu64 -vsid
 * $VICERES DatasetteTapeAzimuthError       -xscpu64 -vsid
 * $VICERES DatasetteTapeWobbleAmplitude    -xscpu64 -vsid
 * $VICERES DatasetteTapeWobbleFrequency    -xscpu64 -vsid
 * $VICERES DatasetteZeroGapDelay           -xscpu64 -vsid
 * $VICERES CPClockF83Save                  -xscpu64 -vsid
 * $VICERES TapecartUpdateTCRT              x64 x64sc x128
 * $VICERES TapecartOptimizeTCRT            x64 x64sc x128
 * $VICERES TapecartLogLevel                x64 x64sc x128
 * $VICERES TapecartTCRTFilename            x64 x64sc x128
 * $VICERES CascomMode                      x64 x64sc x128
 * $VICERES CascomTxRate                    x64 x64sc x128
 * $VICERES CascomRxRate                    x64 x64sc x128
 * $VICERES CascomTxAudioDev                x64 x64sc x128
 * $VICERES CascomRxAudioDev                x64 x64sc x128
 * $VICERES CascomTxGain                    x64 x64sc x128
 * $VICERES CascomPttDevice                 x64 x64sc x128
 * $VICERES CascomPttLine                   x64 x64sc x128
 * $VICERES CascomPttInvert                 x64 x64sc x128
 * $VICERES CascomTxGate                    x64 x64sc x128
 * $VICERES CascomNetPeer                   x64 x64sc x128
 * $VICERES CascomNetPort                   x64 x64sc x128
 * $VICERES CascomNetListenPort             x64 x64sc x128
 * $VICERES CascomNetRxFilter               x64 x64sc x128
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

#include "archdep_serial_control.h"
#include "cascom.h"
#include "datasette.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "tapecart.h"
#include "tapeport.h"
#include "vice_gtk3.h"

#include "settings_tapeport.h"

/** \brief  Column indexes in the tapeport devices model
 */
enum {
    COL_DEVICE_ID,          /**< device ID (int) */
    COL_DEVICE_NAME,        /**< device name (str) */
    COL_DEVICE_TYPE_ID,     /**< device type (int) */
    COL_DEVICE_TYPE_DESC    /**< device type description (str) */
};

/** \brief  List of log levels and their descriptions for the Tapecart
 */
static const vice_gtk3_combo_entry_int_t tcrt_loglevels[] = {
    { "0 (errors only)",                         0 },
    { "1 (0 + mode changes and command bytes)",  1 },
    { "2 (1 + command parameter details)",       2 },
    { NULL,                                     -1 }
};


/*
 * Reference to widgets to be able to enable/disabled them through event
 * handlers
 */

/** \brief  Tape port #1 device combo */
static GtkWidget *port1_type = NULL;

/** \brief  Tape port #2 device combo */
static GtkWidget *port2_type = NULL;

/** \brief  Datasette 1 device traps toggle button */
static GtkWidget *ds_traps1 = NULL;

/** \brief  Datasette 2 device traps toggle button */
static GtkWidget *ds_traps2 = NULL;

/** \brief  Datasette reset toggle button */
static GtkWidget *ds_reset = NULL;

/** \brief  Datasette zerogap delay spine button */
static GtkWidget *ds_zerogap = NULL;

/** \brief  Datasette speed tuning spin button */
static GtkWidget *ds_speed = NULL;

/** \brief  Datasette wobble frequency spin button */
static GtkWidget *ds_wobblefreq = NULL;

/** \brief  Datasette wobble amplitude spin button */
static GtkWidget *ds_wobbleamp = NULL;

/** \brief  Datasette align spin button */
static GtkWidget *ds_align = NULL;

/** \brief  Datasette sound emulation toggle button */
static GtkWidget *ds_sound = NULL;

/** \brief  F83 RTC toggle button */
static GtkWidget *f83_rtc = NULL;

/** \brief  Tapecart save-when-changed toggle button */
static GtkWidget *tcrt_update = NULL;

/** \brief  Tapecart optimize-when-saving toggle button */
static GtkWidget *tcrt_optimize = NULL;

/** \brief  Tapecart log level radiogroup */
static GtkWidget *tcrt_loglevel = NULL;

/** \brief  Tapecart image file browser */
static GtkWidget *tcrt_filename = NULL;

/** \brief  Tapecart flush button */
static GtkWidget *tcrt_flush = NULL;

/** \brief  Cascom mode combo box */
static GtkWidget *cascom_mode_combo = NULL;

#ifdef USE_PORTAUDIO
/** \brief  Cascom audio-mode options grid */
static GtkWidget *cascom_audio_widget = NULL;
#endif

#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
/** \brief  Cascom PTT options grid */
static GtkWidget *cascom_ptt_widget = NULL;
#endif

#ifdef HAVE_NETWORK
/** \brief  Cascom network-mode options grid */
static GtkWidget *cascom_net_widget = NULL;
#endif

/** \brief  Whether the Cascom device is the one selected on the port */
static gboolean cascom_device_active = FALSE;

/** \brief  Cascom operating modes
 *
 * Modes whose backend is not compiled into this build are left out entirely
 * (the CascomMode resource setter rejects them as well).
 */
static const vice_gtk3_combo_entry_int_t cascom_modes[] = {
    { "Loopback (self-test)",   0 },
#ifdef USE_PORTAUDIO
    { "Audio (soundcard AFSK)", 1 },
#endif
#ifdef HAVE_NETWORK
    { "Network (AXUDP)",        2 },
#endif
    { NULL, -1 }
};

#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
/** \brief  Cascom PTT control line */
static const vice_gtk3_combo_entry_int_t cascom_ptt_lines[] = {
    { "RTS", 0 },
    { "DTR", 1 },
    { NULL, -1 }
};
#endif

#ifdef USE_PORTAUDIO
/** \brief  Cascom host audio sample rates */
static const vice_gtk3_combo_entry_int_t cascom_rates[] = {
    { "8000",  8000 },
    { "11025", 11025 },
    { "16000", 16000 },
    { "22050", 22050 },
    { "24000", 24000 },
    { "32000", 32000 },
    { "44100", 44100 },
    { "48000", 48000 },
    { "96000", 96000 },
    { NULL, -1 }
};
#endif /* USE_PORTAUDIO */


/** \brief  Determine if current machine supports tapecart
 *
 * \return  `TRUE` if tapecart emulation is available
 */
static gboolean machine_has_tapecart(void)
{
    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_C128:
            return TRUE;
        default:
            return FALSE;
    }
}

/** \brief  Determine if the current machine has a second tape port
 *
 * \return  `TRUE` if the machine has a second tape port
 */
static gboolean machine_has_second_tape_port(void)
{
    /* only a PET has a second tape port */
    return (gboolean)(machine_class == VICE_MACHINE_PET);
}

/** \brief  Set Datasette widget active/inactive
 *
 * \param[in]   state   status
 */
static void set_datasette_active(gboolean state)
{
    gtk_widget_set_sensitive(ds_reset,      state);
    gtk_widget_set_sensitive(ds_zerogap,    state);
    gtk_widget_set_sensitive(ds_speed,      state);
    gtk_widget_set_sensitive(ds_wobblefreq, state);
    gtk_widget_set_sensitive(ds_wobbleamp,  state);
    gtk_widget_set_sensitive(ds_align,      state);
    gtk_widget_set_sensitive(ds_sound,      state);
    /* xPET does not have device traps right now, grey out the selection */
    if (machine_class == VICE_MACHINE_PET) {
        state = FALSE;
    }
    gtk_widget_set_sensitive(ds_traps1, state);
    if (machine_has_second_tape_port()) {
        gtk_widget_set_sensitive(ds_traps2, state);
    }
}

/** \brief Set CP Clock F83 widget active/inactive
 *
 * \param[in]   state   status
 */
static void set_f83_active(gboolean state)
{
    gtk_widget_set_sensitive(f83_rtc, state);
}

/** \brief  Set tapecart active/inactive
 *
 * \param[in]   state   status
 */
static void set_tapecart_active(gboolean state)
{
    if (machine_has_tapecart()) {
        gtk_widget_set_sensitive(tcrt_update,   state);
        gtk_widget_set_sensitive(tcrt_optimize, state);
        gtk_widget_set_sensitive(tcrt_loglevel, state);
        gtk_widget_set_sensitive(tcrt_filename, state);
        gtk_widget_set_sensitive(tcrt_flush,    state);
    }
}

/** \brief  Show only the option group(s) relevant to the current mode
 *
 * Hides the inactive groups (rather than greying them) to keep the dialog
 * compact: Audio+PTT for Mode=Audio, Network for Mode=Network, neither for
 * Loopback (or when the Cascom device isn't selected on the port).
 */
static void cascom_update_mode_widgets(void)
{
    int mode = 0;

    resources_get_int("CascomMode", &mode);
    /* mode: 0 = loopback, 1 = audio, 2 = net; groups whose backend is not
       compiled into this build do not exist at all */
#ifdef USE_PORTAUDIO
    gtk_widget_set_visible(cascom_audio_widget, cascom_device_active && mode == 1);
#endif
#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
    gtk_widget_set_visible(cascom_ptt_widget,   cascom_device_active && mode == 1);
#endif
#ifdef HAVE_NETWORK
    gtk_widget_set_visible(cascom_net_widget,   cascom_device_active && mode == 2);
#endif
}

/** \brief  Set Cascom widgets active/inactive
 *
 * \param[in]   state   status
 */
static void set_cascom_active(gboolean state)
{
    cascom_device_active = state;
    gtk_widget_set_sensitive(cascom_mode_combo, state);
    cascom_update_mode_widgets();
}

/** \brief  Set individual options active/inactive
 *
 * \param[in]   id      id of active device
 */
static void set_options_widgets_sensitivity(int id)
{
    set_datasette_active(id == TAPEPORT_DEVICE_DATASETTE);
    set_f83_active(id == TAPEPORT_DEVICE_CP_CLOCK_F83);
    set_tapecart_active(id == TAPEPORT_DEVICE_TAPECART);
    set_cascom_active(id == TAPEPORT_DEVICE_CASCOM);
}

/** \brief  Handler for the 'clicked' event of the tapecart flush button
 *
 * \param[in]   widget  button (unused)
 * \param[in]   data    extra event data (unused)
 */
static void on_tcrt_flush_clicked(GtkWidget *widget, gpointer data)
{
    tapecart_flush_tcrt();
}

/** \brief  Create left-aligned label with Pango markup
 *
 * \param[in]   text    text for label (can use Pango markup)
 *
 * \return  GtkLabel
 */
static GtkWidget *label_helper(const char *text)
{
    GtkWidget *label = gtk_label_new(NULL);

    gtk_label_set_markup(GTK_LABEL(label), text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    return label;
}

/** \brief  Create widgets for the datasette
 *
 * TODO:    Someone needs to check the spin button bounds and steps for sane
 *          values.
 *
 * \return  GtkGrid
 */
static GtkWidget *create_datasette_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *scale;
    int        row = 0;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    /* TRUE looks better but makes the dialog too wide :( */
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);

    /* header */
    label = label_helper("<b>Datasette C2N</b>");
    gtk_widget_set_margin_bottom(label, 8);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 4, 1);
    row++;

    /* device traps for datasette #1 and #2 */
    ds_traps1 = vice_gtk3_resource_check_button_new("TrapDevice1",
                                                    "Kernal traps for Device #1 (required for t64)");
    gtk_grid_attach(GTK_GRID(grid), ds_traps1, 0, row, 2, 1);
    if (machine_has_second_tape_port()) {
        ds_traps2 = vice_gtk3_resource_check_button_new("TrapDevice2",
                                                        "Kernal traps for Device #2 (required for t64)");
        gtk_grid_attach(GTK_GRID(grid), ds_traps2, 2, row, 2, 1);
    }
    row++;

    ds_reset = vice_gtk3_resource_check_button_new("DatasetteResetWithCPU",
                                                   "Reset datasette with CPU");
    gtk_widget_set_margin_bottom(ds_reset, 8);
    gtk_grid_attach(GTK_GRID(grid), ds_reset, 0, row, 2, 1);

    row++;

    ds_sound = vice_gtk3_resource_check_button_new("DatasetteSound",
                                                   "Datasette sound");
    gtk_widget_set_margin_bottom(ds_sound, 8);
    gtk_grid_attach(GTK_GRID(grid), ds_sound, 0, row, 1, 1);

    scale = vice_gtk3_resource_scale_custom_new_printf("%s",
                                                       GTK_ORIENTATION_HORIZONTAL,
                                                       0,
                                                       TAPE_SOUND_VOLUME_MAX,
                                                       0,
                                                       (TAPE_SOUND_VOLUME_MAX * 100) / TAPE_SOUND_VOLUME_ONE,
                                                       10,
                                                       "%3.0f%%",
                                                       "DatasetteSoundVolume");
    gtk_widget_set_hexpand(scale, TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
    gtk_grid_attach(GTK_GRID(grid), scale, 1, row, 3, 1);
    row++;

    label      = label_helper("TAP v0 Zero gap delay");
    ds_zerogap = vice_gtk3_resource_spin_int_new("DatasetteZeroGapDelay",
                                                 0, TAP_ZERO_GAP_DELAY_MAX, 1);
    gtk_widget_set_margin_bottom(ds_zerogap, 8);
    gtk_grid_attach(GTK_GRID(grid), label,      0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ds_zerogap, 1, row, 1, 1);

    label    = label_helper("Tape speed tuning");
    ds_speed = vice_gtk3_resource_spin_custom_new("DatasetteSpeedTuning",
                                                  -TAP_SPEED_TUNING_MAX,
                                                  TAP_SPEED_TUNING_MAX,
                                                  -50.0,
                                                  50.0,
                                                  1.0,
                                                  "%4.1f%%");
    gtk_widget_set_margin_bottom(ds_speed, 8);
    gtk_grid_attach(GTK_GRID(grid), label,    2, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ds_speed, 3, row, 1, 1);
    row++;

    label = label_helper("Wobble frequency");
    ds_wobblefreq = vice_gtk3_resource_spin_custom_new("DatasetteTapeWobbleFrequency",
                                                       0, TAP_WOBBLE_FREQ_MAX,
                                                       0.0, 50.0, 1.0,
                                                       "%4.1fHz");
    gtk_widget_set_margin_bottom(ds_wobblefreq, 8);
    gtk_grid_attach(GTK_GRID(grid), label,         0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ds_wobblefreq, 1, row, 1, 1);

    label    = label_helper("Alignment error");
    ds_align = vice_gtk3_resource_spin_int_new("DatasetteTapeAzimuthError",
                                               0,
                                               TAP_AZIMUTH_ERROR_MAX,
                                               TAP_AZIMUTH_ERROR_MAX / 1000);
    gtk_widget_set_margin_bottom(ds_align, 8);
    gtk_grid_attach(GTK_GRID(grid), label,    2, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ds_align, 3, row, 1, 1);
    row++;

    label        = label_helper("Wobble amplitude");
    ds_wobbleamp = vice_gtk3_resource_spin_custom_new("DatasetteTapeWobbleAmplitude",
                                                      0, TAP_WOBBLE_AMPLITUDE_MAX,
                                                      0.0, 50.0, 0.5,
                                                      "%5.1f%%");
    gtk_grid_attach(GTK_GRID(grid), label,        0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ds_wobbleamp, 1, row, 1, 1);

    return grid;
}

/** \brief  Create widget to handler the Cassette Port Clock F83 resources
 *
 * \return  GtkGrid
 */
static GtkWidget *create_cpcf83_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label   = label_helper("<b>Cassette Port Clock F83</b>");
    f83_rtc = vice_gtk3_resource_check_button_new("CPClockF83Save",
                                                   "Save RTC data when changed");
    gtk_grid_attach(GTK_GRID(grid), label,   0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), f83_rtc, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Create widget to handle the tapecart resources
 *
 * \return  GtkGrid
 */
static GtkWidget *create_tapecart_widget(void)
{
    GtkWidget  *grid;
    GtkWidget  *label;
    GtkWidget  *wrapper;
    const char *patterns[] = { "*.tcrt", NULL };
    int         row = 0;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    /* header */
    label = label_helper("<b>Tapecart</b>");
    gtk_widget_set_margin_bottom(label, 8);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 3, 1);
    row++;

    /* TapecartTCRTFilename */
    label         = label_helper("TCRT filename");
    tcrt_filename = vice_gtk3_resource_filechooser_new("TapecartTCRTFilename",
                                                       GTK_FILE_CHOOSER_ACTION_OPEN);
    vice_gtk3_resource_filechooser_set_custom_title(tcrt_filename,
                                                    "Select tapecart image");
    vice_gtk3_resource_filechooser_set_filter(tcrt_filename,
                                              "Tapecart images",
                                              patterns,
                                              TRUE);
    tcrt_flush = gtk_button_new_with_label("Flush image");
    gtk_widget_set_hexpand(tcrt_flush, FALSE);
    gtk_grid_attach(GTK_GRID(grid), label,         0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), tcrt_filename, 1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), tcrt_flush,    2, row, 1, 1);
    g_signal_connect(G_OBJECT(tcrt_flush),
                     "clicked",
                     G_CALLBACK(on_tcrt_flush_clicked),
                     NULL);
    row++;

    /* TCRT log level */
    label = label_helper("Log level");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    tcrt_loglevel = vice_gtk3_resource_combo_int_new("TapecartLogLevel",
                                                     tcrt_loglevels);
    gtk_widget_set_margin_top(tcrt_loglevel, 8);
    gtk_widget_set_margin_bottom(tcrt_loglevel, 8);
    gtk_grid_attach(GTK_GRID(grid), tcrt_loglevel, 1, row, 2, 1);
    row++;

    /* wrapper for update/optimize check buttons */
    wrapper = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(wrapper), TRUE);
    /* TapecartUpdateTCRT */
    tcrt_update = vice_gtk3_resource_check_button_new("TapecartUpdateTCRT",
                                                      "Save data when changed");
    gtk_grid_attach(GTK_GRID(wrapper), tcrt_update, 0, 0, 1, 1);
    /* TapecartOptimizeTCRT */
    tcrt_optimize = vice_gtk3_resource_check_button_new("TapecartOptimizeTCRT",
                                                        "Optimize data when changed");
    gtk_grid_attach(GTK_GRID(wrapper), tcrt_optimize, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, row, 3, 1);

    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Handler for the Cascom mode combo 'changed' event
 *
 * \param[in]   widget  mode combo (unused)
 * \param[in]   data    extra event data (unused)
 */
static void on_cascom_mode_changed(GtkWidget *widget, gpointer data)
{
    cascom_update_mode_widgets();
}

/** \brief  Attach a "label: widget" row to a grid
 *
 * \param[in]   grid    target grid
 * \param[in]   row     row index
 * \param[in]   text    label markup
 * \param[in]   widget  control for column 1
 * \param[in]   expand  let \a widget fill the column (combos, sliders) or keep
 *                      it at its natural size (small spin buttons)
 *
 * \return  next row index
 */
#ifdef HAVE_NETWORK
static int cascom_row(GtkWidget *grid, int row, const char *text,
                      GtkWidget *widget, gboolean expand)
{
    gtk_grid_attach(GTK_GRID(grid), label_helper(text), 0, row, 1, 1);
    if (expand) {
        gtk_widget_set_hexpand(widget, TRUE);
    } else {
        gtk_widget_set_halign(widget, GTK_ALIGN_START);
    }
    gtk_grid_attach(GTK_GRID(grid), widget, 1, row, 1, 1);
    return row + 1;
}
#endif /* HAVE_NETWORK */

/** \brief  Create a resource-bound combo populated from a device-name list
 *
 * \param[in]   resource    string resource holding the selected name
 * \param[in]   devices     NULL-terminated name list (entry [0] is always "")
 * \param[in]   empty_text  display text for the "" (default/none) entry
 *
 * \return  GtkComboBox
 */
#if defined(USE_PORTAUDIO) || defined(ARCHDEP_HAVE_SERIAL_CONTROL)
static GtkWidget *create_name_combo(const char *resource,
                                    char       **devices,
                                    const char  *empty_text)
{
    const vice_gtk3_combo_entry_str_t defaults[] = {
        { (char *)"", (char *)empty_text },
        VICE_GTK3_COMBO_ENTRY_STR_LIST_END
    };
    GtkWidget *combo = vice_gtk3_resource_combo_str_new(resource, defaults);
    int        i;

    for (i = 0; devices[i] != NULL; i++) {
        if (devices[i][0] != '\0') {
            vice_gtk3_resource_combo_str_append(combo, devices[i], devices[i]);
        }
    }
    vice_gtk3_resource_combo_str_sync(combo);
    return combo;
}
#endif /* USE_PORTAUDIO || ARCHDEP_HAVE_SERIAL_CONTROL */

#ifdef USE_PORTAUDIO
/** \brief  Create the Cascom audio-mode options grid
 *
 * \return  GtkGrid
 */
static GtkWidget *create_cascom_audio_widget(void)
{
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *tx_dev, *rx_dev, *tx_rate, *rx_rate, *gain;
    char     **devs;
    int        row  = 0;

    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 4);
    gtk_grid_attach(GTK_GRID(grid),
                    label_helper("<b>Audio (soundcard) modem</b>"), 0, row++, 4, 1);

    /* TX device + rate on one line */
    devs    = cascom_audio_device_list(0);  /* playback */
    tx_dev  = create_name_combo("CascomTxAudioDev", devs, "(system default)");
    cascom_device_list_free(devs);
    tx_rate = vice_gtk3_resource_combo_int_new("CascomTxRate", cascom_rates);
    gtk_widget_set_hexpand(tx_dev, TRUE);
    gtk_widget_set_halign(tx_rate, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_helper("TX device"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), tx_dev,                    1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_helper("rate (Hz)"), 2, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), tx_rate,                   3, row, 1, 1);
    row++;

    /* RX device + rate on one line */
    devs    = cascom_audio_device_list(1);  /* capture */
    rx_dev  = create_name_combo("CascomRxAudioDev", devs, "(system default)");
    cascom_device_list_free(devs);
    rx_rate = vice_gtk3_resource_combo_int_new("CascomRxRate", cascom_rates);
    gtk_widget_set_hexpand(rx_dev, TRUE);
    gtk_widget_set_halign(rx_rate, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_helper("RX device"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rx_dev,                    1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_helper("rate (Hz)"), 2, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rx_rate,                   3, row, 1, 1);
    row++;

    /* TX gain slider spanning the value columns */
    gain = vice_gtk3_resource_scale_custom_new_printf("CascomTxGain",
                                                      GTK_ORIENTATION_HORIZONTAL,
                                                      0, 100, 0, 100, 5, "%3.0f%%");
    gtk_scale_set_value_pos(GTK_SCALE(gain), GTK_POS_RIGHT);
    gtk_widget_set_hexpand(gain, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_helper("TX gain"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gain,                    1, row, 3, 1);
    row++;

    /* PTT-gated TX audio: only emit tones while keyed (like a real radio), with
       a short fade so opening/closing the gate doesn't click */
    gtk_grid_attach(GTK_GRID(grid),
                    vice_gtk3_resource_check_button_new("CascomTxGate",
                        "Gate TX audio on PTT (only transmit while keyed)"),
                    0, row, 4, 1);
    row++;

    return grid;
}
#endif /* USE_PORTAUDIO */

#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
/** \brief  Create the Cascom PTT options grid
 *
 * \return  GtkGrid
 */
static GtkWidget *create_cascom_ptt_widget(void)
{
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *serial, *line, *invert;
    char     **devs;
    int        row  = 0;

    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 4);
    gtk_grid_attach(GTK_GRID(grid),
                    label_helper("<b>PTT (radio keying)</b>"), 0, row++, 5, 1);

    /* serial device + control line + invert on one line */
    devs   = cascom_serial_device_list();
    serial = create_name_combo("CascomPttDevice", devs, "(none)");
    cascom_device_list_free(devs);
    line   = vice_gtk3_resource_combo_int_new("CascomPttLine", cascom_ptt_lines);
    invert = vice_gtk3_resource_check_button_new("CascomPttInvert", "Invert polarity");
    gtk_widget_set_hexpand(serial, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_helper("Serial device"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), serial,                        1, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_helper("Line"),          2, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), line,                          3, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), invert,                        4, row, 1, 1);
    row++;

    return grid;
}
#endif /* ARCHDEP_HAVE_SERIAL_CONTROL */

#ifdef HAVE_NETWORK
/** \brief  Create the Cascom network-mode options grid
 *
 * \return  GtkGrid
 */
static GtkWidget *create_cascom_net_widget(void)
{
    GtkWidget *grid = gtk_grid_new();
    int        row  = 0;

    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 4);
    gtk_grid_attach(GTK_GRID(grid),
                    label_helper("<b>Network (AXUDP) gateway</b>"), 0, row++, 2, 1);
    row = cascom_row(grid, row, "Peer host",
                     vice_gtk3_resource_entry_new("CascomNetPeer"), TRUE);
    row = cascom_row(grid, row, "Peer UDP port",
                     vice_gtk3_resource_spin_int_new("CascomNetPort", 1, 65535, 1), FALSE);
    row = cascom_row(grid, row, "Listen UDP port",
                     vice_gtk3_resource_spin_int_new("CascomNetListenPort", 1, 65535, 1), FALSE);
    row = cascom_row(grid, row, "RX callsign filter",
                     vice_gtk3_resource_entry_new("CascomNetRxFilter"), TRUE);
    return grid;
}
#endif /* HAVE_NETWORK */

/** \brief  Create widget to handle the Cascom (BayCom TCM3105 modem) resources
 *
 * \return  GtkGrid
 */
static GtkWidget *create_cascom_widget(void)
{
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *label;
    GtkWidget *modebox;
    int        row  = 0;

    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("<b>CASCOM packet-radio modem</b>\n"
                         "<span size=\"small\">BayCom-compatible TCM3105 modem emulation</span>");
    gtk_widget_set_margin_bottom(label, 8);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row++, 2, 1);

    /* keep the label and combo adjacent, independent of the wide groups below */
    modebox           = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    cascom_mode_combo = vice_gtk3_resource_combo_int_new("CascomMode", cascom_modes);
    gtk_box_pack_start(GTK_BOX(modebox), label_helper("Mode"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(modebox), cascom_mode_combo,    FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(grid), modebox, 0, row, 2, 1);
    row++;
    g_signal_connect(G_OBJECT(cascom_mode_combo),
                     "changed",
                     G_CALLBACK(on_cascom_mode_changed),
                     NULL);

    /* Only the groups whose backend is compiled into this build exist.
       Realise each group's children, then take the group out of the ancestor's
       show_all recursion so cascom_update_mode_widgets() alone controls their
       visibility (only the active mode's group is shown, keeping it compact). */
#ifdef USE_PORTAUDIO
    cascom_audio_widget = create_cascom_audio_widget();
    gtk_widget_set_margin_top(cascom_audio_widget, 8);
    gtk_grid_attach(GTK_GRID(grid), cascom_audio_widget, 0, row++, 2, 1);
    gtk_widget_show_all(cascom_audio_widget);
    gtk_widget_set_no_show_all(cascom_audio_widget, TRUE);
    gtk_widget_set_visible(cascom_audio_widget, FALSE);
#endif
#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
    cascom_ptt_widget = create_cascom_ptt_widget();
    gtk_widget_set_margin_top(cascom_ptt_widget, 8);
    gtk_grid_attach(GTK_GRID(grid), cascom_ptt_widget, 0, row++, 2, 1);
    gtk_widget_show_all(cascom_ptt_widget);
    gtk_widget_set_no_show_all(cascom_ptt_widget, TRUE);
    gtk_widget_set_visible(cascom_ptt_widget, FALSE);
#endif
#ifdef HAVE_NETWORK
    cascom_net_widget = create_cascom_net_widget();
    gtk_widget_set_margin_top(cascom_net_widget, 8);
    gtk_grid_attach(GTK_GRID(grid), cascom_net_widget, 0, row++, 2, 1);
    gtk_widget_show_all(cascom_net_widget);
    gtk_widget_set_no_show_all(cascom_net_widget, TRUE);
    gtk_widget_set_visible(cascom_net_widget, FALSE);
#endif

    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Handler for the 'changed' event of the device combobox
 *
 * Sets the active tapeport device via the "TapePort[12]Device" resource.
 *
 * \param[in]   combo       device combo box
 * \param[in]   portnum     tape port number
 */
static void on_device_changed(GtkComboBox *combo, gpointer portnum)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;

    model = gtk_combo_box_get_model(combo);
    if (gtk_combo_box_get_active_iter(combo, &iter)) {
        gint   id;
        gchar *name = NULL;
        gint   port = GPOINTER_TO_INT(portnum);

        gtk_tree_model_get(model,
                           &iter,
                           COL_DEVICE_ID, &id,
                           COL_DEVICE_NAME, &name,
                           -1);
        resources_set_int_sprintf("TapePort%iDevice", id, port);
        set_options_widgets_sensitivity(id);

        g_free(name);
    }
}

/** \brief  Set tapeport device ID
 *
 * Sets the currently selected combobox item via device ID.
 *
 * To avoid updating the related resource via the combobox' event handler, use
 * the \a blocked argument.
 *
 * \param[in]   combo   device combo box
 * \param[in]   id      device ID
 * \param[in]   blocked block 'changed' signal handler
 */
static gboolean set_device_id(GtkComboBox *combo, gint id, gboolean blocked)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    gulong        handler_id;
    gboolean      result = FALSE;

    /* do we need to block the 'changed' event handler? */
    if (blocked) {
        /* look up handler ID by callback */
        handler_id = g_signal_handler_find(combo,
                                           G_SIGNAL_MATCH_FUNC,
                                           0,       /* signal_id */
                                           0,       /* detail */
                                           NULL,    /* closure */
                                           on_device_changed,   /* func */
                                           NULL);
        if (handler_id > 0) {
            g_signal_handler_block(combo, handler_id);
        }
    }

    /* iterate the model until we find the device ID */
    model = gtk_combo_box_get_model(combo);
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            gint current;

            gtk_tree_model_get(model, &iter, COL_DEVICE_ID, &current, -1);
            if (id == current) {
                gtk_combo_box_set_active_iter(combo, &iter);
                result = TRUE;
                break;
            }
        } while (gtk_tree_model_iter_next(model, &iter));
    }

    /* set options checkboxes "greyed-out" state */
    set_options_widgets_sensitivity(id);

    /* unblock signal, if blocked */
    if (blocked) {
        g_signal_handler_unblock(combo, handler_id);
    }

    return result;
}

/** \brief  Create model for the device combobox
 *
 * Create a model with (dev-id, dev-name, dev-type-id, dev-type-desc).
 *
 * \param[in]   port    tape port number (1 or 2 (PET))
 *
 * \return  model
 */
static GtkListStore *create_device_model(int port)
{
    GtkListStore    *model;
    tapeport_desc_t *devices;
    tapeport_desc_t *dev;

    model = gtk_list_store_new(4,
                               G_TYPE_INT,      /* ID */
                               G_TYPE_STRING,   /* name */
                               G_TYPE_INT,      /* type ID */
                               G_TYPE_STRING    /* type description */
                               );
    devices = tapeport_get_valid_devices(port - 1 /*index, not port */, TRUE);
    for (dev = devices; dev->name != NULL; dev++) {
        GtkTreeIter iter;

        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                           &iter,
                           COL_DEVICE_ID, dev->id,
                           COL_DEVICE_NAME, dev->name,
                           COL_DEVICE_TYPE_ID, dev->device_type,
                           COL_DEVICE_TYPE_DESC, tapeport_get_device_type_desc(dev->device_type),
                           -1);
    }
    lib_free(devices);

    return model;
}

/** \brief  Create combobox for the tapeport devices
 *
 * Create a combobox with valid tapeport devices for current machine.
 *
 * The model of the combobox contains device ID, name and type, of which name
 * is shown and ID is used to set the related resource.
 *
 * \param[in]   port    tape port number (1 or 2 (PET only))
 *
 * \return  GtkComboBox
 *
 * \todo    Try using the device type to create little headers in the combobox,
 *          grouping the devices by type. Might be overkill for some machines
 *          that only have a few tapeport devices, we'll see.
 *          I tried using a second column for the device type description, and
 *          althought it doesn't look bad in the popup list, when the popup
 *          isn't active it looks weird ;)
 *          So for now the device type isn't used.
 */
static GtkWidget *create_device_combobox(int port)
{
    GtkWidget       *combo;
    GtkListStore    *model;
    GtkCellRenderer *name_renderer;
#if 0
    GtkCellRenderer *type_renderer;
#endif

    /* TODO:    Check if the model can be shared for both ports, perhaps the
     *          second tape port has a different set of supported devices
     */
    model = create_device_model(port);

    /* create combobox with a single cell renderer for the device name column */
    combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));
    name_renderer = gtk_cell_renderer_text_new();
#if 0
    type_renderer = gtk_cell_renderer_text_new();
#endif
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo),
                               name_renderer,
                               TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo),
                                   name_renderer,
                                   "text", COL_DEVICE_NAME,
                                   NULL);
#if 0
    gtk_cell_layout_pack_end(GTK_CELL_LAYOUT(combo),
                             type_renderer,
                             TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo),
                                   type_renderer,
                                   "text", COL_DEVICE_TYPE_DESC,
                                   NULL);
#endif

    g_signal_connect(G_OBJECT(combo),
                     "changed",
                     G_CALLBACK(on_device_changed),
                     GINT_TO_POINTER(port));

    return combo;
}

/** \brief  Create combobox(es) to select device type for port 1 (and 2 for PET)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_device_types_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    int        second;

    second = machine_has_second_tape_port();

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    /* header */
    label = label_helper("<b>Tape port device types</b>");
    gtk_widget_set_margin_bottom(label, 8);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, second ? 4 : 2, 1);

    /* first tape port */
    label      = label_helper("Tape port #1");
    port1_type = create_device_combobox(TAPEPORT_UNIT_1);
    gtk_widget_set_hexpand(port1_type, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label,      0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), port1_type, 1, 1, 1, 1);

    /* PET has a second tape port */
    if (second) {
        label      = label_helper("Tape port #2");
        port2_type = create_device_combobox(TAPEPORT_UNIT_2);
        gtk_widget_set_hexpand(port2_type, TRUE);
        gtk_grid_attach(GTK_GRID(grid), label,      2, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), port2_type, 3, 1, 1, 1);
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to select/control tape port devices
 *
 * \param[in]   parent  parent widget (ignored)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_tapeport_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *devices;
    GtkWidget *datasette;
    GtkWidget *cpcf83;
    int        device_id = 0;
    int        row = 0;

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    /* comboboxes with the tapeport devices */
    devices = create_device_types_widget();
    gtk_widget_set_margin_bottom(devices, 8);
    gtk_grid_attach(GTK_GRID(grid), devices, 0, row, 1, 1);
    row++;

    /* datasette device settings */
    datasette = create_datasette_widget();
    gtk_widget_set_margin_bottom(datasette, 8);
    gtk_grid_attach(GTK_GRID(grid), datasette, 0, row, 1, 1);
    row++;

    /* Cassette Port Clock F83 */
    cpcf83 = create_cpcf83_widget();
    gtk_grid_attach(GTK_GRID(grid), cpcf83, 0, row, 1, 1);
    row++;

    /* TapeCart settings */
    if (machine_has_tapecart()) {
        gtk_grid_attach(GTK_GRID(grid), create_tapecart_widget(), 0, row, 1, 1);
        row++;
    }

    /* Cascom (BayCom TCM3105 modem) settings - C64/C64SC/C128 only */
    if (machine_class == VICE_MACHINE_C64 ||
        machine_class == VICE_MACHINE_C64SC ||
        machine_class == VICE_MACHINE_C128) {
        gtk_grid_attach(GTK_GRID(grid), create_cascom_widget(), 0, row, 1, 1);
        row++;
    }

    /* these need to happen here, after the above widgets are created */
    /* set port1 type using the resource */
    resources_get_int("TapePort1Device", &device_id);
    set_device_id(GTK_COMBO_BOX(port1_type), device_id, TRUE);
    if (machine_has_second_tape_port()) {
        /* set port2 type using the resource */
        resources_get_int("TapePort2Device", &device_id);
        set_device_id(GTK_COMBO_BOX(port2_type), device_id, TRUE);
    }

    gtk_widget_show_all(grid);
    return grid;
}
