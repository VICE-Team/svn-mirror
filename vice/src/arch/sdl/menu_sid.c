/*
 * menu_sid.c - Implementation of the SID settings menu for the SDL UI.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include <stdlib.h>
#include <math.h>

#include "lib.h"
#include "menu_common.h"
#include "resources.h"
#include "sid.h"
#include "sidcart.h"
#include "types.h"
#include "uiactions.h"
#include "uimenu.h"

#include "menu_sid.h"


static UI_MENU_CALLBACK(custom_SidModel_callback)
{
    int engine, model, selected;

    selected = vice_ptr_to_int(param);

    if (activated) {
        engine = selected >> 8;
        model = selected & 0xff;
        sid_set_engine_model(engine, model);
    } else {
        resources_get_int("SidEngine", &engine);
        resources_get_int("SidModel", &model);

        if (selected == ((engine << 8) | model)) {
            return sdl_menu_text_tick;
        }
    }

    return NULL;
}

static ui_menu_entry_t *sid_model_menu = NULL;

#if defined(HAVE_RESIDFP)

static ui_menu_entry_t *sid_profile_menu = NULL;

static int sid_profile_selected = -1;

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
    { "Chris Huelsbeck",                     0.9 },
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
    { "Linus Akesson (lft)",                 0.3 },
    { "Mark Cooksey",                        0.4 },
    { "Mark Wilson",                         0.2 },
    { "Markus Mueller (Superbrain)",         0.5 },
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

static UI_MENU_CALLBACK(custom_SidProfile_callback)
{
    int selected;
    double value;
    int value_int;

    selected = vice_ptr_to_int(param);
    if (activated) {
        sid_profile_selected = selected;
        value = ((filterRangeMap[selected].value * 20.0f) - 1.0f) / 39.0f;
        value_int = (int)round(value * 1000.0f);
        resources_set_int("SidResid6581FilterCurve", RESIDFP_6581_FILTER_CURVE_DEFAULT);
        resources_set_int("SidResidCombinedWaveformStrength", RESIDFP_COMBINED_WAVEFORM_STRENGTH_DEFAULT);
        resources_set_int("SidResid6581FilterRange", value_int);
    } else {
        if (selected == sid_profile_selected) {
            return sdl_menu_text_tick;
        }
    }

    return NULL;
}
#endif

#if defined(HAVE_RESID) || defined(HAVE_RESIDFP)
UI_MENU_DEFINE_RADIO(SidResidSampling)

static const ui_menu_entry_t sid_sampling_menu[] = {
    {   .string   = "Fast",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidResidSampling_callback,
        .data     = (ui_callback_data_t)SID_RESID_SAMPLING_FAST
    },
    {   .string   = "Interpolating",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidResidSampling_callback,
        .data     = (ui_callback_data_t)SID_RESID_SAMPLING_INTERPOLATION
    },
    {   .string   = "Resampling",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidResidSampling_callback,
        .data     = (ui_callback_data_t)SID_RESID_SAMPLING_RESAMPLING
    },
    {   .string   = "Fast Resampling",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidResidSampling_callback,
        .data     = (ui_callback_data_t)SID_RESID_SAMPLING_FAST_RESAMPLING
    },
    SDL_MENU_LIST_END
};
#endif

#ifdef HAVE_RESID
UI_MENU_DEFINE_SLIDER(SidResidPassband, 0, 90)
UI_MENU_DEFINE_SLIDER(SidResidGain, 90, 100)
UI_MENU_DEFINE_SLIDER(SidResidFilterBias, -5000, 5000)

/* Do we have new 8580 filter emulation? */
#ifdef HAVE_NEW_8580_FILTER

/* Yup, create 8580 filter sliders */
UI_MENU_DEFINE_SLIDER(SidResid8580Passband, 0, 90)
UI_MENU_DEFINE_SLIDER(SidResid8580Gain, 90, 100)
UI_MENU_DEFINE_SLIDER(SidResid8580FilterBias, -5000, 5000)

/* Create menu items including 8580 slider */
# define VICE_SDL_RESID_OPTIONS                                                                                               \
    {   .string   = "reSID sampling method",                                                                                  \
        .type     = MENU_ENTRY_SUBMENU,                                                                                       \
        .callback = submenu_radio_callback,                                                                                   \
        .data     = (ui_callback_data_t)sid_sampling_menu                                                                     \
    },                                                                                                                        \
    {   .string   = "reSID 6581 resampling passband",                                                                         \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResidPassband_callback,                                                                         \
        .data     = (ui_callback_data_t)"Enter passband in percentage of total bandwidth (lower is faster, higher is better)" \
    },                                                                                                                        \
    {   .string   = "reSID 6581 filter gain",                                                                                 \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResidGain_callback,                                                                             \
        .data     = (ui_callback_data_t)"Set filter gain in percent"                                                          \
    },                                                                                                                        \
    {   .string   = "reSID 6581 filter bias",                                                                                 \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResidFilterBias_callback,                                                                       \
        .data     = (ui_callback_data_t)"Set filter bias in mV"                                                               \
    },                                                                                                                        \
    {   .string   = "reSID 8580 resampling passband",                                                                         \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResid8580Passband_callback,                                                                     \
        .data     = (ui_callback_data_t)"Enter passband in percentage of total bandwidth (lower is faster, higher is better)" \
    },                                                                                                                        \
    {   .string   = "reSID 8580 filter gain",                                                                                 \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResid8580Gain_callback,                                                                         \
        .data     = (ui_callback_data_t)"Set filter gain in percent"                                                          \
    },                                                                                                                        \
    {   .string   = "reSID 8580 filter bias",                                                                                 \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResid8580FilterBias_callback,                                                                   \
        .data     = (ui_callback_data_t)"Set filter bias in mV"                                                               \
    },
#else

/* Nope, don't show 8580 filter sliders */
# define VICE_SDL_RESID_OPTIONS                                                                                               \
    {   .string   = "reSID sampling method",                                                                                  \
        .type     = MENU_ENTRY_SUBMENU,                                                                                       \
        .callback = submenu_radio_callback,                                                                                   \
        .data     = (ui_callback_data_t)sid_sampling_menu                                                                     \
    },                                                                                                                        \
    {   .string   = "reSID 6581 resampling passband",                                                                         \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResidPassband_callback,                                                                         \
        .data     = (ui_callback_data_t)"Enter passband in percentage of total bandwidth (lower is faster, higher is better)" \
    },                                                                                                                        \
    {   .string   = "reSID 6581 filter gain",                                                                                 \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResidGain_callback,                                                                             \
        .data     = (ui_callback_data_t)"Set filter gain in percent"                                                          \
    },                                                                                                                        \
    {   .string   = "reSID 6581 filter bias",                                                                                 \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResidFilterBias_callback,                                                                       \
        .data     = (ui_callback_data_t)"Set filter bias in mV"                                                               \
    },
#endif

#endif /* HAVE_RESID */

#ifdef HAVE_RESIDFP
UI_MENU_DEFINE_SLIDER(SidResid6581FilterCurve, 0, RESIDFP_6581_FILTER_CURVE_MAX)
UI_MENU_DEFINE_SLIDER(SidResid6581FilterRange, 0, RESIDFP_6581_FILTER_RANGE_MAX)
UI_MENU_DEFINE_SLIDER(SidResid8580FilterCurve, 0, RESIDFP_8580_FILTER_CURVE_MAX)
UI_MENU_DEFINE_SLIDER(SidResidCombinedWaveformStrength, 0, RESIDFP_COMBINED_WAVEFORM_STRENGTH_MAX)
UI_MENU_DEFINE_TOGGLE(SidResid6581OldCaps)

# define VICE_SDL_RESIDFP_OPTIONS                                                                                             \
    {   .string   = "reSIDfp Profile",                                                                                        \
        .type     = MENU_ENTRY_SUBMENU,                                                                                       \
        .callback = submenu_radio_callback,                                                                                   \
        .data     = (ui_callback_data_t)0xdeadc0de,                                                                           \
    },                                                                                                                        \
    {   .string   = "reSIDfp sampling method",                                                                                \
        .type     = MENU_ENTRY_SUBMENU,                                                                                       \
        .callback = submenu_radio_callback,                                                                                   \
        .data     = (ui_callback_data_t)sid_sampling_menu                                                                     \
    },                                                                                                                        \
    {   .string   = "reSIDfp 6581 filter curve",                                                                              \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResid6581FilterCurve_callback,                                                                  \
        .data     = (ui_callback_data_t)"Set filter curve"                                                                    \
    },                                                                                                                        \
    {   .string   = "reSIDfp 6581 filter range",                                                                              \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResid6581FilterRange_callback,                                                                  \
        .data     = (ui_callback_data_t)"Set filter range"                                                                    \
    },                                                                                                                        \
    {   .string   = "reSIDfp 8580 filter curve",                                                                              \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResid8580FilterCurve_callback,                                                                  \
        .data     = (ui_callback_data_t)"Set filter curve"                                                                    \
    },                                                                                                                        \
    {   .string   = "reSIDfp 6581 mixed wave strength",                                                                       \
        .type     = MENU_ENTRY_RESOURCE_INT,                                                                                  \
        .callback = slider_SidResidCombinedWaveformStrength_callback,                                                         \
        .data     = (ui_callback_data_t)"Set mixed waveform strength"                                                         \
    },                                                                                                                        \
    {   .string   = "reSIDfp 6581 old 2200pf caps",                                                                           \
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,                                                                               \
        .callback = toggle_SidResid6581OldCaps_callback,                                                                      \
    },
#endif /* HAVE_RESIDFP */


#ifdef HAVE_USBSID
UI_MENU_DEFINE_TOGGLE(SidUSBSIDReadMode)
UI_MENU_DEFINE_TOGGLE(SidUSBSIDAudioMode)
UI_MENU_DEFINE_RADIO(SidUSBSIDDiffSize)
UI_MENU_DEFINE_RADIO(SidUSBSIDBufferSize)

static const ui_menu_entry_t us_diffsize_menu[] = {
    {   .string   = "32",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidUSBSIDDiffSize_callback,
        .data     = (ui_callback_data_t)32
    },
    {   .string   = "64",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidUSBSIDDiffSize_callback,
        .data     = (ui_callback_data_t)64
    },
    {   .string   = "128",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidUSBSIDDiffSize_callback,
        .data     = (ui_callback_data_t)128
    },
    {   .string   = "256",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidUSBSIDDiffSize_callback,
        .data     = (ui_callback_data_t)256
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t us_buffsize_menu[] = {
    {   .string   = "512",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidUSBSIDBufferSize_callback,
        .data     = (ui_callback_data_t)512
    },
    {   .string   = "1024",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidUSBSIDBufferSize_callback,
        .data     = (ui_callback_data_t)1024
    },
    {   .string   = "2048",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidUSBSIDBufferSize_callback,
        .data     = (ui_callback_data_t)2048
    },
    {   .string   = "4096",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidUSBSIDBufferSize_callback,
        .data     = (ui_callback_data_t)4096
    },
    {   .string   = "8192",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidUSBSIDBufferSize_callback,
        .data     = (ui_callback_data_t)8192
    },
    {   .string   = "16384",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidUSBSIDBufferSize_callback,
        .data     = (ui_callback_data_t)16384
    },
    SDL_MENU_LIST_END
};

# define VICE_SDL_USBSID_OPTIONS                                                                                              \
    {   .string   = "USBSID enable stereo audio mode",                                                                        \
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,                                                                               \
        .callback = toggle_SidUSBSIDAudioMode_callback                                                                        \
    },                                                                                                                        \
    {   .string   = "USBSID enable read mode",                                                                                \
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,                                                                               \
        .callback = toggle_SidUSBSIDReadMode_callback                                                                         \
    },                                                                                                                        \
    {   .string   = "USBSID buffer size",                                                                                     \
        .type     = MENU_ENTRY_SUBMENU,                                                                                       \
        .callback = submenu_radio_callback,                                                                                   \
        .data     = (ui_callback_data_t)us_buffsize_menu                                                                      \
    },                                                                                                                        \
    {   .string   = "USBSID diff size",                                                                                       \
        .type     = MENU_ENTRY_SUBMENU,                                                                                       \
        .callback = submenu_radio_callback,                                                                                   \
        .data     = (ui_callback_data_t)us_diffsize_menu                                                                      \
    },                                                                                                                        \


#endif /* HAVE_USBSID */


UI_MENU_DEFINE_TOGGLE(SidFilters)
UI_MENU_DEFINE_RADIO(SidStereo)
UI_MENU_DEFINE_RADIO(Sid2AddressStart)
UI_MENU_DEFINE_RADIO(Sid3AddressStart)
UI_MENU_DEFINE_RADIO(Sid4AddressStart)
UI_MENU_DEFINE_RADIO(Sid5AddressStart)
UI_MENU_DEFINE_RADIO(Sid6AddressStart)
UI_MENU_DEFINE_RADIO(Sid7AddressStart)
UI_MENU_DEFINE_RADIO(Sid8AddressStart)

#define SID_D4XX_MENU(menu, txt, showcb, cb)    \
static const ui_menu_entry_t menu[] = {         \
    {   .string   = txt,                        \
        .type     = MENU_ENTRY_TEXT,            \
        .callback = showcb                      \
    },                                          \
    {   .string   = "$D420",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd420  \
    },                                          \
    {   .string   = "$D440",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd440  \
    },                                          \
    {   .string   = "$D460",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd460  \
    },                                          \
    {   .string   = "$D480",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd480  \
    },                                          \
    {   .string   = "$D4A0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd4a0  \
    },                                          \
    {   .string   = "$D4C0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd4c0  \
    },                                          \
    {   .string   = "$D4E0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd4e0  \
    },                                          \
    SDL_MENU_LIST_END                           \
};

#define SID_D5XX_MENU(menu, txt, showcb, cb)    \
static const ui_menu_entry_t menu[] = {         \
    {   .string   = txt,                        \
        .type     = MENU_ENTRY_TEXT,            \
        .callback = showcb                      \
    },                                          \
    {   .string   = "$D500",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd500  \
    },                                          \
    {   .string   = "$D520",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd520  \
    },                                          \
    {   .string   = "$D540",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd540  \
    },                                          \
    {   .string   = "$D560",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd560  \
    },                                          \
    {   .string   = "$D580",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd580  \
    },                                          \
    {   .string   = "$D5A0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd5a0  \
    },                                          \
    {   .string   = "$D5C0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd5c0  \
    },                                          \
    {   .string   = "$D5E0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd5e0  \
    },                                          \
    SDL_MENU_LIST_END                           \
};

#define SID_D6XX_MENU(menu, txt, showcb, cb)    \
static const ui_menu_entry_t menu[] = {         \
    {   .string   = txt,                        \
        .type     = MENU_ENTRY_TEXT,            \
        .callback = showcb                      \
    },                                          \
    {   .string   = "$D600",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd600  \
    },                                          \
    {   .string   = "$D620",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd620  \
    },                                          \
    {   .string   = "$D640",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd640  \
    },                                          \
    {   .string   = "$D660",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd660  \
    },                                          \
    {   .string   = "$D680",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd680  \
    },                                          \
    {   .string   = "$D6A0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd6a0  \
    },                                          \
    {   .string   = "$D6C0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd6c0  \
    },                                          \
    {   .string   = "$D6E0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd6e0  \
    },                                          \
    SDL_MENU_LIST_END                           \
};

#define SID_D7XX_MENU(menu, txt, showcb, cb)    \
static const ui_menu_entry_t menu[] = {         \
    {   .string   = txt,                        \
        .type     = MENU_ENTRY_TEXT,            \
        .callback = showcb                      \
    },                                          \
    {   .string   = "$D700",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd700  \
    },                                          \
    {   .string   = "$D720",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd720  \
    },                                          \
    {   .string   = "$D740",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd740  \
    },                                          \
    {   .string   = "$D760",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd760  \
    },                                          \
    {   .string   = "$D780",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd780  \
    },                                          \
    {   .string   = "$D7A0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd7a0  \
    },                                          \
    {   .string   = "$D7C0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd7c0  \
    },                                          \
    {   .string   = "$D7E0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xd7e0  \
    },                                          \
    SDL_MENU_LIST_END                           \
};

#define SID_DEXX_MENU(menu, txt, showcb, cb)    \
static const ui_menu_entry_t menu[] = {         \
    {   .string   = txt,                        \
        .type     = MENU_ENTRY_TEXT,            \
        .callback = showcb                      \
    },                                          \
    {   .string   = "$DE00",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xde00  \
    },                                          \
    {   .string   = "$DE20",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xde20  \
    },                                          \
    {   .string   = "$DE40",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xde40  \
    },                                          \
    {   .string   = "$DE60",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xde60  \
    },                                          \
    {   .string   = "$DE80",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xde80  \
    },                                          \
    {   .string   = "$DEA0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdea0  \
    },                                          \
    {   .string   = "$DEC0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdec0  \
    },                                          \
    {   .string   = "$DEE0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdee0  \
    },                                          \
    SDL_MENU_LIST_END                           \
};

#define SID_DFXX_MENU(menu, txt, showcb, cb)    \
static const ui_menu_entry_t menu[] = {         \
    {   .string   = txt,                        \
        .type     = MENU_ENTRY_TEXT,            \
        .callback = showcb                      \
    },                                          \
    {   .string   = "$DF00",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdf00  \
    },                                          \
    {   .string   = "$DF20",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdf20  \
    },                                          \
    {   .string   = "$DF40",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdf40  \
    },                                          \
    {   .string   = "$DF60",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdf60  \
    },                                          \
    {   .string   = "$DF80",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdf80  \
    },                                          \
    {   .string   = "$DFA0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdfa0  \
    },                                          \
    {   .string   = "$DFC0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdfc0  \
    },                                          \
    {   .string   = "$DFE0",                    \
        .type     = MENU_ENTRY_RESOURCE_RADIO,  \
        .callback = cb,                         \
        .data     = (ui_callback_data_t)0xdfe0  \
    },                                          \
    SDL_MENU_LIST_END                           \
};

#define SID_EXTRA_MENU(sid_nr, sid_text)                                                                                                                    \
    static UI_MENU_CALLBACK(show_Sid##sid_nr##AddressStart_callback)                                                                                        \
    {                                                                                                                                                       \
        static char buf[20];                                                                                                                                \
        int value;                                                                                                                                          \
                                                                                                                                                            \
        resources_get_int_sprintf("Sid%dAddressStart", &value, sid_nr);                                                                                     \
                                                                                                                                                            \
        sprintf(buf, "$%04x", (unsigned int)value);                                                                                                         \
        return buf;                                                                                                                                         \
    }                                                                                                                                                       \
                                                                                                                                                            \
    SID_D4XX_MENU(sid##sid_nr##_d4x0_menu, sid_text " SID base address", show_Sid##sid_nr##AddressStart_callback, radio_Sid##sid_nr##AddressStart_callback) \
    SID_D5XX_MENU(sid##sid_nr##_d5x0_menu, sid_text " SID base address", show_Sid##sid_nr##AddressStart_callback, radio_Sid##sid_nr##AddressStart_callback) \
    SID_D6XX_MENU(sid##sid_nr##_d6x0_menu, sid_text " SID base address", show_Sid##sid_nr##AddressStart_callback, radio_Sid##sid_nr##AddressStart_callback) \
    SID_D7XX_MENU(sid##sid_nr##_d7x0_menu, sid_text " SID base address", show_Sid##sid_nr##AddressStart_callback, radio_Sid##sid_nr##AddressStart_callback) \
    SID_DEXX_MENU(sid##sid_nr##_dex0_menu, sid_text " SID base address", show_Sid##sid_nr##AddressStart_callback, radio_Sid##sid_nr##AddressStart_callback) \
    SID_DFXX_MENU(sid##sid_nr##_dfx0_menu, sid_text " SID base address", show_Sid##sid_nr##AddressStart_callback, radio_Sid##sid_nr##AddressStart_callback) \
                                                                                                                                                            \
    static const ui_menu_entry_t c128_sid##sid_nr##_base_menu[] = {                                                                                         \
        {   .string   = sid_text " SID base address",                                                                                                       \
            .type     = MENU_ENTRY_TEXT,                                                                                                                    \
            .callback = show_Sid##sid_nr##AddressStart_callback,                                                                                            \
        },                                                                                                                                                  \
        {   .string   = "$D4x0",                                                                                                                            \
            .type     = MENU_ENTRY_SUBMENU,                                                                                                                 \
            .callback = submenu_callback,                                                                                                                   \
            .data     = (ui_callback_data_t)sid##sid_nr##_d4x0_menu                                                                                         \
        },                                                                                                                                                  \
        {   .string   = "$D7x0",                                                                                                                            \
            .type     = MENU_ENTRY_SUBMENU,                                                                                                                 \
            .callback = submenu_callback,                                                                                                                   \
            .data     = (ui_callback_data_t)sid##sid_nr##_d7x0_menu                                                                                         \
        },                                                                                                                                                  \
        {   .string   = "$DEx0",                                                                                                                            \
            .type     = MENU_ENTRY_SUBMENU,                                                                                                                 \
            .callback = submenu_callback,                                                                                                                   \
            .data     = (ui_callback_data_t)sid##sid_nr##_dex0_menu                                                                                         \
        },                                                                                                                                                  \
        {   .string   = "$DFx0",                                                                                                                            \
            .type     = MENU_ENTRY_SUBMENU,                                                                                                                 \
            .callback = submenu_callback,                                                                                                                   \
            .data     = (ui_callback_data_t)sid##sid_nr##_dfx0_menu                                                                                         \
        },                                                                                                                                                  \
        SDL_MENU_LIST_END                                                                                                                                   \
    };                                                                                                                                                      \
                                                                                                                                                            \
    static const ui_menu_entry_t c64_sid##sid_nr##_base_menu[] = {                                                                                          \
        {   .string   = sid_text " SID base address",                                                                                                       \
            .type     = MENU_ENTRY_TEXT,                                                                                                                    \
            .callback = show_Sid##sid_nr##AddressStart_callback,                                                                                            \
        },                                                                                                                                                  \
        {   .string   = "$D4x0",                                                                                                                            \
            .type     = MENU_ENTRY_SUBMENU,                                                                                                                 \
            .callback = submenu_callback,                                                                                                                   \
            .data     = (ui_callback_data_t)sid##sid_nr##_d4x0_menu                                                                                         \
        },                                                                                                                                                  \
        {   .string   = "$D5x0",                                                                                                                            \
            .type     = MENU_ENTRY_SUBMENU,                                                                                                                 \
            .callback = submenu_callback,                                                                                                                   \
            .data     = (ui_callback_data_t)sid##sid_nr##_d5x0_menu                                                                                         \
        },                                                                                                                                                  \
        {   .string   = "$D6x0",                                                                                                                            \
            .type     = MENU_ENTRY_SUBMENU,                                                                                                                 \
            .callback = submenu_callback,                                                                                                                   \
            .data     = (ui_callback_data_t)sid##sid_nr##_d6x0_menu                                                                                         \
        },                                                                                                                                                  \
        {   .string   = "$D7x0",                                                                                                                            \
            .type     = MENU_ENTRY_SUBMENU,                                                                                                                 \
            .callback = submenu_callback,                                                                                                                   \
            .data     = (ui_callback_data_t)sid##sid_nr##_d7x0_menu                                                                                         \
        },                                                                                                                                                  \
        {   .string   = "$DEx0",                                                                                                                            \
            .type     = MENU_ENTRY_SUBMENU,                                                                                                                 \
            .callback = submenu_callback,                                                                                                                   \
            .data     = (ui_callback_data_t)sid##sid_nr##_dex0_menu                                                                                         \
        },                                                                                                                                                  \
        {   .string   = "$DFx0",                                                                                                                            \
            .type     = MENU_ENTRY_SUBMENU,                                                                                                                 \
            .callback = submenu_callback,                                                                                                                   \
            .data     = (ui_callback_data_t)sid##sid_nr##_dfx0_menu                                                                                         \
        },                                                                                                                                                  \
        SDL_MENU_LIST_END                                                                                                                                   \
    };

SID_EXTRA_MENU(2, "Second")
SID_EXTRA_MENU(3, "Third")
SID_EXTRA_MENU(4, "Fourth")
SID_EXTRA_MENU(5, "Fifth")
SID_EXTRA_MENU(6, "Sixth")
SID_EXTRA_MENU(7, "Seventh")
SID_EXTRA_MENU(8, "Eight")

static UI_MENU_CALLBACK(show_SidStereo_callback)
{
    int value;

    resources_get_int("SidStereo", &value);
    switch (value) {
        case 1:
            return "One";
        case 2:
            return "Two";
        case 3:
            return "Three";
        case 4:
            return "Four";
        case 5:
            return "Five";
        case 6:
            return "Six";
        case 7:
            return "Seven";
    }
    return "None";
}

static const ui_menu_entry_t c64_stereo_sid_menu[] = {
    {   .string   = "None",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidStereo_callback,
        .data     = (ui_callback_data_t)0
    },
    {   .string   = "One",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidStereo_callback,
        .data     = (ui_callback_data_t)1
    },
    {   .string   = "Two",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidStereo_callback,
        .data     = (ui_callback_data_t)2
    },
    {   .string   = "Three",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidStereo_callback,
        .data     = (ui_callback_data_t)3
    },
    {   .string   = "Four",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidStereo_callback,
        .data     = (ui_callback_data_t)4
    },
    {   .string   = "Five",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidStereo_callback,
        .data     = (ui_callback_data_t)5
    },
    {   .string   = "Six",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidStereo_callback,
        .data     = (ui_callback_data_t)6
    },
    {   .string   = "Seven",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidStereo_callback,
        .data     = (ui_callback_data_t)7
    },
    SDL_MENU_LIST_END
};

ui_menu_entry_t sid_c64_menu[] = {
    /* CAUTION: position is hardcoded below */
    {   .string   = "SID Model",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
    },
    {   .string   = "Extra SIDs",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_SidStereo_callback,
        .data     = (ui_callback_data_t)c64_stereo_sid_menu
    },
    {   .string   = "Second SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid2AddressStart_callback,
        .data     = (ui_callback_data_t)c64_sid2_base_menu
    },
    {   .string   = "Third SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid3AddressStart_callback,
        .data     = (ui_callback_data_t)c64_sid3_base_menu
    },
    {   .string   = "Fourth SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid4AddressStart_callback,
        .data     = (ui_callback_data_t)c64_sid4_base_menu
    },
    {   .string   = "Fifth SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid5AddressStart_callback,
        .data     = (ui_callback_data_t)c64_sid5_base_menu
    },
    {   .string   = "Sixth SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid6AddressStart_callback,
        .data     = (ui_callback_data_t)c64_sid6_base_menu
    },
    {   .string   = "Seventh SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid7AddressStart_callback,
        .data     = (ui_callback_data_t)c64_sid7_base_menu
    },
    {   .string   = "Eight SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid8AddressStart_callback,
        .data     = (ui_callback_data_t)c64_sid8_base_menu
    },
    {   .string   = "Emulate filters",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_SidFilters_callback,
    },
#ifdef HAVE_RESID
    VICE_SDL_RESID_OPTIONS
#endif
#ifdef HAVE_RESIDFP
    VICE_SDL_RESIDFP_OPTIONS
#endif
#ifdef HAVE_USBSID
    VICE_SDL_USBSID_OPTIONS
#endif
    SDL_MENU_LIST_END
};

ui_menu_entry_t sid_c128_menu[] = {
    /* CAUTION: position is hardcoded below */
    {   .string   = "SID Model",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback
    },
    {   .string   = "Extra SIDs",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_SidStereo_callback,
        .data     = (ui_callback_data_t)c64_stereo_sid_menu
    },
    {   .string   = "Second SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid2AddressStart_callback,
        .data     = (ui_callback_data_t)c128_sid2_base_menu
    },
    {   .string   = "Third SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid3AddressStart_callback,
        .data     = (ui_callback_data_t)c128_sid3_base_menu
    },
    {   .string   = "Fourth SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid4AddressStart_callback,
        .data     = (ui_callback_data_t)c128_sid4_base_menu
    },
    {   .string   = "Fift SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid5AddressStart_callback,
        .data     = (ui_callback_data_t)c128_sid5_base_menu
    },
    {   .string   = "Sixth SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid6AddressStart_callback,
        .data     = (ui_callback_data_t)c128_sid6_base_menu
    },
    {   .string   = "Seventh SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid7AddressStart_callback,
        .data     = (ui_callback_data_t)c128_sid7_base_menu
    },
    {   .string   = "Eighth SID base address",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = show_Sid8AddressStart_callback,
        .data     = (ui_callback_data_t)c128_sid8_base_menu
    },
    {   .string   = "Emulate filters",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_SidFilters_callback
    },
#ifdef HAVE_RESID
    VICE_SDL_RESID_OPTIONS
#endif
#ifdef HAVE_RESIDFP
    VICE_SDL_RESIDFP_OPTIONS
#endif
#ifdef HAVE_USBSID
    VICE_SDL_USBSID_OPTIONS
#endif
    SDL_MENU_LIST_END
};

ui_menu_entry_t sid_cbm2_menu[] = {
    /* CAUTION: position is hardcoded below */
    {   .string   = "SID Model",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback
    },
    {   .string   = "Emulate filters",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_SidFilters_callback
    },
#ifdef HAVE_RESID
    VICE_SDL_RESID_OPTIONS
#endif
#ifdef HAVE_RESIDFP
    VICE_SDL_RESIDFP_OPTIONS
#endif
#ifdef HAVE_USBSID
    VICE_SDL_USBSID_OPTIONS
#endif
    SDL_MENU_LIST_END
};

ui_menu_entry_t sid_dtv_menu[] = {
    /* CAUTION: position is hardcoded below */
    {   .string   = "SID Model",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback
    },
    {   .string   = "Emulate filters",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_SidFilters_callback
    },
#ifdef HAVE_RESID
    VICE_SDL_RESID_OPTIONS
#endif
#ifdef HAVE_RESIDFP
    VICE_SDL_RESIDFP_OPTIONS
#endif
#ifdef HAVE_USBSID
    VICE_SDL_USBSID_OPTIONS
#endif
    SDL_MENU_LIST_END
};

UI_MENU_DEFINE_TOGGLE(SidCart)
UI_MENU_DEFINE_RADIO(SidAddress)
UI_MENU_DEFINE_RADIO(SidClock)

ui_menu_entry_t sid_vic_menu[] = {
    {   .string   = "Enable SID cartridge emulation",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_SidCart_callback
    },
    /* CAUTION: position is hardcoded below */
    {   .string   = "SID Model",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback
    },
    {   .string   = "Emulate filters",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_SidFilters_callback
    },
#ifdef HAVE_RESID
    VICE_SDL_RESID_OPTIONS
#endif
#ifdef HAVE_RESIDFP
    VICE_SDL_RESIDFP_OPTIONS
#endif
#ifdef HAVE_USBSID
    VICE_SDL_USBSID_OPTIONS
#endif
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID address"),
    {   .string   = "$9800",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidAddress_callback,
        .data     = (ui_callback_data_t)0x9800
    },
    {   .string   = "$9C00",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidAddress_callback,
        .data     = (ui_callback_data_t)0x9c00
    },
    SDL_MENU_ITEM_SEPARATOR,

    SDL_MENU_ITEM_TITLE("SID clock"),
    {   .string   = "C64",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidClock_callback,
        .data     = (ui_callback_data_t)SIDCART_CLOCK_C64
    },
    {   .string   = "VIC20",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidClock_callback,
        .data     = (ui_callback_data_t)SIDCART_CLOCK_NATIVE
    },
    SDL_MENU_LIST_END
};

ui_menu_entry_t sid_pet_menu[] = {
    {   .string   = "Enable SID cartridge emulation",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_SidCart_callback
    },
    /* CAUTION: position is hardcoded below */
    {   .string   = "SID Model",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback
    },
    {   .string   = "Emulate filters",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_SidFilters_callback
    },
#ifdef HAVE_RESID
    VICE_SDL_RESID_OPTIONS
#endif
#ifdef HAVE_RESIDFP
    VICE_SDL_RESIDFP_OPTIONS
#endif
#ifdef HAVE_USBSID
    VICE_SDL_USBSID_OPTIONS
#endif
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("SID address"),
    {   .string   = "$8F00",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidAddress_callback,
        .data     = (ui_callback_data_t)0x8f00
    },
    {   .string   = "$E900",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidAddress_callback,
        .data     = (ui_callback_data_t)0xe900
    },
    SDL_MENU_ITEM_SEPARATOR,

    SDL_MENU_ITEM_TITLE("SID clock"),
    {   .string   = "C64",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidClock_callback,
        .data     = (ui_callback_data_t)SIDCART_CLOCK_C64
    },
    {   .string   = "PET",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidClock_callback,
        .data     = (ui_callback_data_t)SIDCART_CLOCK_NATIVE
    },
    SDL_MENU_LIST_END
};

UI_MENU_DEFINE_TOGGLE(DIGIBLASTER)

ui_menu_entry_t sid_plus4_menu[] = {
    {   .string   = "Enable SID cartridge emulation",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_SidCart_callback
    },
    /* CAUTION: position is hardcoded below */
    {   .string   = "SID Model",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback
    },
    {   .string   = "Emulate filters",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_SidFilters_callback
    },
#ifdef HAVE_RESID
    VICE_SDL_RESID_OPTIONS
#endif
#ifdef HAVE_RESIDFP
    VICE_SDL_RESIDFP_OPTIONS
#endif
#ifdef HAVE_USBSID
    VICE_SDL_USBSID_OPTIONS
#endif
    SDL_MENU_ITEM_SEPARATOR,

    SDL_MENU_ITEM_TITLE("SID address"),
    {   .string   = "$FD40",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidAddress_callback,
        .data     = (ui_callback_data_t)0xfd40
    },
    {   .string   = "$FE80",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidAddress_callback,
        .data     = (ui_callback_data_t)0xfe80
    },
    SDL_MENU_ITEM_SEPARATOR,

    SDL_MENU_ITEM_TITLE("SID clock"),
    {   .string   = "C64",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidClock_callback,
        .data     = (ui_callback_data_t)SIDCART_CLOCK_C64
    },
    {   .string   = "PLUS4",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_SidClock_callback,
        .data     = (ui_callback_data_t)SIDCART_CLOCK_NATIVE
    },
    SDL_MENU_ITEM_SEPARATOR,

    {   .string   = "Enable SID cartridge digiblaster add-on",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_DIGIBLASTER_callback
    },
    SDL_MENU_LIST_END
};


void uisid_menu_create(void)
{
    sid_engine_model_t **list = sid_get_engine_model_list();
    int i;

    /* create "Sid Model" menu */
    for (i = 0; list[i]; ++i) {}

    sid_model_menu = lib_malloc((i + 1) * sizeof(ui_menu_entry_t));

    for (i = 0; list[i]; ++i) {
        sid_model_menu[i].action   = ACTION_NONE;
        sid_model_menu[i].string   = (char*)list[i]->name;
        sid_model_menu[i].type     = MENU_ENTRY_RESOURCE_RADIO;
        sid_model_menu[i].callback = custom_SidModel_callback;
        sid_model_menu[i].data     = (ui_callback_data_t)vice_int_to_ptr(list[i]->value);
    }
    sid_model_menu[i].string = NULL;

    sid_c64_menu[0].data   = (ui_callback_data_t)sid_model_menu;
    sid_c128_menu[0].data  = (ui_callback_data_t)sid_model_menu;
    sid_cbm2_menu[0].data  = (ui_callback_data_t)sid_model_menu;
    sid_dtv_menu[0].data   = (ui_callback_data_t)sid_model_menu;
    sid_vic_menu[1].data   = (ui_callback_data_t)sid_model_menu;
    sid_pet_menu[1].data   = (ui_callback_data_t)sid_model_menu;
    sid_plus4_menu[1].data = (ui_callback_data_t)sid_model_menu;

#ifdef HAVE_RESIDFP
    /* create "SID Profile" Menu */
    for (i = 0; filterRangeMap[i].name; ++i) {}

    sid_profile_menu = lib_malloc((i + 1) * sizeof(ui_menu_entry_t));

    for (i = 0; filterRangeMap[i].name; ++i) {
        sid_profile_menu[i].action   = ACTION_NONE;
        sid_profile_menu[i].string   = (char*)filterRangeMap[i].name;
        sid_profile_menu[i].type     = MENU_ENTRY_RESOURCE_RADIO;
        sid_profile_menu[i].callback = custom_SidProfile_callback;
        sid_profile_menu[i].data     = (ui_callback_data_t)vice_int_to_ptr(i);
    }
    sid_profile_menu[i].string = NULL;

    for (i = 0; (sid_c64_menu[i].data != (ui_callback_data_t)0xdeadc0de); ++i) {}
    sid_c64_menu[i].data   = (ui_callback_data_t)sid_profile_menu;
    for (i = 0; (sid_c128_menu[i].data != (ui_callback_data_t)0xdeadc0de); ++i) {}
    sid_c128_menu[i].data  = (ui_callback_data_t)sid_profile_menu;
    for (i = 0; (sid_cbm2_menu[i].data != (ui_callback_data_t)0xdeadc0de); ++i) {}
    sid_cbm2_menu[i].data  = (ui_callback_data_t)sid_profile_menu;
    for (i = 0; (sid_dtv_menu[i].data != (ui_callback_data_t)0xdeadc0de); ++i) {}
    sid_dtv_menu[i].data   = (ui_callback_data_t)sid_profile_menu;
    for (i = 0; (sid_vic_menu[i].data != (ui_callback_data_t)0xdeadc0de); ++i) {}
    sid_vic_menu[i].data   = (ui_callback_data_t)sid_profile_menu;
    for (i = 0; (sid_pet_menu[i].data != (ui_callback_data_t)0xdeadc0de); ++i) {}
    sid_pet_menu[i].data   = (ui_callback_data_t)sid_profile_menu;
    for (i = 0; (sid_plus4_menu[i].data != (ui_callback_data_t)0xdeadc0de); ++i) {}
    sid_plus4_menu[i].data = (ui_callback_data_t)sid_profile_menu;
#endif
}

/** \brief  Clean up memory used by the SID model menu
 */
void uisid_menu_shutdown(void)
{
    if (sid_model_menu != NULL) {
        lib_free(sid_model_menu);
    }
    if (sid_profile_menu != NULL) {
        lib_free(sid_profile_menu);
    }
}
