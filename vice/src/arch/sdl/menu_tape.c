/** \file   menu_tape.c
 * \brief   Tape menu for SDL UI
 *
 * \author  Hannu Nuotio <hannu.nuotio@tut.fi>
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include "types.h"

#include "archdep_serial_control.h"
#include "attach.h"
#include "cascom.h"
#include "cbmimage.h"
#include "datasette.h"
#include "diskimage.h"
#include "tapecart.h"
#include "lib.h"
#include "menu_common.h"
#include "menu_tape.h"
#include "tape.h"
#include "tapeport.h"
#include "ui.h"
#include "uiactions.h"
#include "uifilereq.h"
#include "uimenu.h"
#include "uimsgbox.h"
#include "util.h"

UI_MENU_DEFINE_RADIO(TapePort1Device)
UI_MENU_DEFINE_RADIO(TapePort2Device)

static ui_menu_entry_t tapeport_dyn_menu[TAPEPORT_MAX_PORTS][TAPEPORT_MAX_DEVICES + 1];

static int tapeport_dyn_menu_init[TAPEPORT_MAX_PORTS] = { 0 };

static void sdl_menu_tapeport_free(int port)
{
    int i;

    for (i = 0; tapeport_dyn_menu[port][i].string != NULL; i++) {
        lib_free(tapeport_dyn_menu[port][i].string);
    }
}

static UI_MENU_CALLBACK(TapePort1Device_dynmenu_callback)
{
    tapeport_desc_t *devices = tapeport_get_valid_devices(TAPEPORT_PORT_1, 1);
    int i;

    /* rebuild menu if it already exists. */
    if (tapeport_dyn_menu_init[TAPEPORT_PORT_1] != 0) {
        sdl_menu_tapeport_free(TAPEPORT_PORT_1);
    } else {
        tapeport_dyn_menu_init[TAPEPORT_PORT_1] = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        tapeport_dyn_menu[TAPEPORT_PORT_1][i].action   = ACTION_NONE;
        tapeport_dyn_menu[TAPEPORT_PORT_1][i].string   = lib_strdup(devices[i].name);
        tapeport_dyn_menu[TAPEPORT_PORT_1][i].type     = MENU_ENTRY_RESOURCE_RADIO;
        tapeport_dyn_menu[TAPEPORT_PORT_1][i].callback = radio_TapePort1Device_callback;
        tapeport_dyn_menu[TAPEPORT_PORT_1][i].data     = (ui_callback_data_t)vice_int_to_ptr(devices[i].id);
    }
    tapeport_dyn_menu[TAPEPORT_PORT_1][i].string = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(TapePort2Device_dynmenu_callback)
{
    tapeport_desc_t *devices = tapeport_get_valid_devices(TAPEPORT_PORT_2, 1);
    int i;

    /* rebuild menu if it already exists. */
    if (tapeport_dyn_menu_init[TAPEPORT_PORT_2] != 0) {
        sdl_menu_tapeport_free(TAPEPORT_PORT_2);
    } else {
        tapeport_dyn_menu_init[TAPEPORT_PORT_2] = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        tapeport_dyn_menu[TAPEPORT_PORT_2][i].action   = ACTION_NONE;
        tapeport_dyn_menu[TAPEPORT_PORT_2][i].string   = (char *)lib_strdup(devices[i].name);
        tapeport_dyn_menu[TAPEPORT_PORT_2][i].type     = MENU_ENTRY_RESOURCE_RADIO;
        tapeport_dyn_menu[TAPEPORT_PORT_2][i].callback = radio_TapePort2Device_callback;
        tapeport_dyn_menu[TAPEPORT_PORT_2][i].data     = (ui_callback_data_t)vice_int_to_ptr(devices[i].id);
    }
    tapeport_dyn_menu[TAPEPORT_PORT_2][i].string   = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(attach_tape1_callback)
{
    char *name;

    if (activated) {
        name = sdl_ui_file_selection_dialog("Select tape image", FILEREQ_MODE_CHOOSE_FILE);
        if (name != NULL) {
            if (tape_image_attach(TAPEPORT_PORT_1 + 1, name) < 0) {
                ui_error("Cannot attach tape image.");
            }
            lib_free(name);
        }
        ui_action_finish(ACTION_TAPE_ATTACH_1);
    }
    return NULL;
}

static UI_MENU_CALLBACK(attach_tape2_callback)
{
    char *name;

    if (activated) {
        name = sdl_ui_file_selection_dialog("Select tape image", FILEREQ_MODE_CHOOSE_FILE);
        if (name != NULL) {
            if (tape_image_attach(TAPEPORT_PORT_2 + 1, name) < 0) {
                ui_error("Cannot attach tape image.");
            }
            lib_free(name);
        }
        ui_action_finish(ACTION_TAPE_ATTACH_2);
    }
    return NULL;
}

static UI_MENU_CALLBACK(create_tape_image_callback)
{
    char *name = NULL;
    int overwrite = 1;

    if (activated) {
        name = sdl_ui_file_selection_dialog("Select tape image name", FILEREQ_MODE_SAVE_FILE);
        if (name != NULL) {
            if (util_file_exists(name)) {
                if (message_box("VICE QUESTION", "File exists, do you want to overwrite?", MESSAGE_YESNO) != 1) {
                    overwrite = 0;
                }
            }
            if (overwrite == 1) {
                if (cbmimage_create_image(name, DISK_IMAGE_TYPE_TAP)) {
                    ui_error("Cannot create tape image");
                }
            }
            lib_free(name);
        }
        ui_action_finish(ACTION_TAPE_CREATE_1);
    }
    return NULL;
}

UI_MENU_DEFINE_INT(DatasetteSpeedTuning)
UI_MENU_DEFINE_INT(DatasetteZeroGapDelay)
UI_MENU_DEFINE_TOGGLE(DatasetteResetWithCPU)
UI_MENU_DEFINE_INT(DatasetteTapeWobbleFrequency)
UI_MENU_DEFINE_INT(DatasetteTapeWobbleAmplitude)
UI_MENU_DEFINE_INT(DatasetteTapeAzimuthError)
UI_MENU_DEFINE_TOGGLE(DatasetteSound)
UI_MENU_DEFINE_TOGGLE(TrapDevice1)

const ui_menu_entry_t tape_pet_menu[] = {
    {   .action   = ACTION_TAPE_ATTACH_1,
        .string   = "Attach tape image to Datasette 1",
        .type     = MENU_ENTRY_DIALOG,
        .callback = attach_tape1_callback
    },
    {   .action   = ACTION_TAPE_ATTACH_2,
        .string   = "Attach tape image to Datasette 2",
        .type     = MENU_ENTRY_DIALOG,
        .callback = attach_tape2_callback
    },
    {   .action   = ACTION_TAPE_DETACH_1,
        .string   = "Detach tape image from Datasette 1",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_DETACH_2,
        .string   = "Detach tape image from Datasette 2",
        .type     = MENU_ENTRY_OTHER
    },
    /* unlike the Gtk3 UI, the SDL UI appears to only have a single "Create tape"
     * menu item/action */
    {   .action   = ACTION_TAPE_CREATE_1,
        .string   = "Create new tape image",
        .type     = MENU_ENTRY_DIALOG,
        .callback = create_tape_image_callback
    },
    SDL_MENU_ITEM_SEPARATOR,

    SDL_MENU_ITEM_TITLE("Datasette 1 control"),
    {   .action   = ACTION_TAPE_STOP_1,
        .string   = "Stop",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_PLAY_1,
        .string   = "Play",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_FFWD_1,
        .string   = "Forward",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_REWIND_1,
        .string   = "Rewind",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_RECORD_1,
        .string   = "Record",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_RESET_1,
        .string   = "Reset",
        .type     = MENU_ENTRY_OTHER
    },
    SDL_MENU_ITEM_SEPARATOR,

    SDL_MENU_ITEM_TITLE("Datasette 2 control"),
    {   .action   = ACTION_TAPE_STOP_2,
        .string   = "Stop",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_PLAY_2,
        .string   = "Play",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_FFWD_2,
        .string   = "Forward",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_REWIND_2,
        .string   = "Rewind",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_RECORD_2,
        .string   = "Record",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_RESET_2,
        .string   = "Reset",
        .type     = MENU_ENTRY_OTHER
    },
    SDL_MENU_ITEM_SEPARATOR,

    {   .string   = "Datasette speed tuning",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_DatasetteSpeedTuning_callback,
        .data     = (ui_callback_data_t)"Set datasette speed tuning"
    },
    {   .string   = "Datasette zero gap delay",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_DatasetteZeroGapDelay_callback,
        .data     = (ui_callback_data_t)"Set datasette zero gap delay"
    },
    {   .string   = "Datasette tape wobble frequency",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_DatasetteTapeWobbleFrequency_callback,
        .data     = (ui_callback_data_t)"Set datasette tape wobble frequency"
    },
    {   .string   = "Datasette tape wobble amplitude",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_DatasetteTapeWobbleAmplitude_callback,
        .data     = (ui_callback_data_t)"Set datasette tape wobble amplitude"
    },
    {   .string   = "Datasette tape alignment",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_DatasetteTapeAzimuthError_callback,
        .data     = (ui_callback_data_t)"Set datasette alignment error"
    },
    {   .string   = "Reset Datasette on CPU Reset",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_DatasetteResetWithCPU_callback
    },
    {   .string   = "Enable Datasette sound",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_DatasetteSound_callback
    },
    {   .string   = "Enable virtual device (for t64)",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_TrapDevice1_callback
    },
    SDL_MENU_LIST_END
};

ui_menu_entry_t tape_menu_no_tapeport[] = {
    SDL_MENU_ITEM_TITLE("No tapeport on current model"),
    SDL_MENU_LIST_END
};

const ui_menu_entry_t tape_menu_template[] = {
    {   .action   = ACTION_TAPE_ATTACH_1,
        .string   = "Attach tape image",
        .type     = MENU_ENTRY_DIALOG,
        .callback = attach_tape1_callback
    },
    {   .action   = ACTION_TAPE_DETACH_1,
        .string   = "Detach tape image",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_CREATE_1,
        .string   = "Create new tape image",
        .type     = MENU_ENTRY_DIALOG,
        .callback = create_tape_image_callback
    },
    SDL_MENU_ITEM_SEPARATOR,

    SDL_MENU_ITEM_TITLE("Datasette control"),
    {   .action   = ACTION_TAPE_STOP_1,
        .string   = "Stop",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_PLAY_1,
        .string   = "Play",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_FFWD_1,
        .string   = "Forward",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_REWIND_1,
        .string   = "Rewind",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_RECORD_1,
        .string   = "Record",
        .type     = MENU_ENTRY_OTHER
    },
    {   .action   = ACTION_TAPE_RESET_1,
        .string   = "Reset",
        .type     = MENU_ENTRY_OTHER
    },
    SDL_MENU_ITEM_SEPARATOR,

    {   .string   = "Datasette speed tuning",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_DatasetteSpeedTuning_callback,
        .data     = (ui_callback_data_t)"Set datasette speed tuning"
    },
    {   .string   = "Datasette zero gap delay",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_DatasetteZeroGapDelay_callback,
        .data     = (ui_callback_data_t)"Set datasette zero gap delay"
    },
    {   .string   = "Datasette tape wobble frequency",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_DatasetteTapeWobbleFrequency_callback,
        .data     = (ui_callback_data_t)"Set datasette tape wobble frequency"
    },
    {   .string   = "Datasette tape wobble amplitude",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_DatasetteTapeWobbleAmplitude_callback,
        .data     = (ui_callback_data_t)"Set datasette tape wobble amplitude"
    },
    {   .string   = "Datasette tape alignment",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_DatasetteTapeAzimuthError_callback,
        .data     = (ui_callback_data_t)"Set datasette alignment error"
    },
    {   .string   = "Reset Datasette on CPU Reset",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_DatasetteResetWithCPU_callback,
    },
    {   .string   = "Enable Datasette sound",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_DatasetteSound_callback
    },
    {   .string   = "Enable virtual device (for t64)",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_TrapDevice1_callback
    },
    SDL_MENU_LIST_END
};

ui_menu_entry_t tape_menu[sizeof(tape_menu_template) / sizeof(ui_menu_entry_t)];

UI_MENU_DEFINE_TOGGLE(CPClockF83Save)

const ui_menu_entry_t cpclockf83_device_menu[] = {
    {   .string   = "Save CP Clock F83 RTC data when changed",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_CPClockF83Save_callback
    },
    SDL_MENU_LIST_END
};


/*
 * tapecart support (see https://github.com/ikorb/tapecart/)
 */
UI_MENU_DEFINE_TOGGLE(TapecartUpdateTCRT)
UI_MENU_DEFINE_TOGGLE(TapecartOptimizeTCRT)
UI_MENU_DEFINE_INT(TapecartLoglevel)
UI_MENU_DEFINE_FILE_STRING(TapecartTCRTFilename)


/** \brief  Flush tapecart image to disk
 */
static UI_MENU_CALLBACK(tapecart_flush_callback)
{
    if (activated) {
        if (tapecart_flush_tcrt() != 0) {
            /* report error */
            ui_error("Failed to flush tapecart image");
        } else {
            ui_message("Flushed tapecart image to disk");
        }
    }
    return NULL;
}

const ui_menu_entry_t tapecart_submenu[] = {
    {   .string   = "Save tapecart data when changed",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_TapecartUpdateTCRT_callback
    },
    {   .string   = "Optimize tapecart data when changed",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_TapecartOptimizeTCRT_callback
    },
    {   .string   = "tapecart Log level",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_TapecartLoglevel_callback,
        .data     = (ui_callback_data_t)"Set tapecart log level"
    },
    {   .string   = "TCRT filename",
        .type     = MENU_ENTRY_DIALOG,
        .callback = file_string_TapecartTCRTFilename_callback,
        .data     = (ui_callback_data_t)"Select TCRT file"
    },
    {   .string   = "Flush current image",
        .type     = MENU_ENTRY_OTHER,
        .callback = tapecart_flush_callback
    },
    SDL_MENU_LIST_END
};


/*
 * cascom (BayCom / TCM3105 packet-radio modem) settings
 *
 * Mirrors the Gtk3 tape-port page: a mode selector plus per-mode option groups
 * (audio + PTT for Mode=Audio, network for Mode=Net). The TX/RX audio devices
 * and the PTT serial device are dynamic radio submenus built from the same
 * cascom_*_device_list() enumerators the Gtk3 combos use. Groups whose
 * backend is not compiled into this build are left out entirely.
 */
UI_MENU_DEFINE_RADIO(CascomMode)
#ifdef USE_PORTAUDIO
UI_MENU_DEFINE_RADIO(CascomTxRate)
UI_MENU_DEFINE_RADIO(CascomRxRate)
UI_MENU_DEFINE_RADIO(CascomTxAudioDev)
UI_MENU_DEFINE_RADIO(CascomRxAudioDev)
UI_MENU_DEFINE_INT(CascomTxGain)
UI_MENU_DEFINE_TOGGLE(CascomTxGate)
#endif
#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
UI_MENU_DEFINE_RADIO(CascomPttLine)
UI_MENU_DEFINE_RADIO(CascomPttDevice)
UI_MENU_DEFINE_TOGGLE(CascomPttInvert)
#endif
#ifdef HAVE_NETWORK
UI_MENU_DEFINE_INT(CascomNetPort)
UI_MENU_DEFINE_INT(CascomNetListenPort)
UI_MENU_DEFINE_STRING(CascomNetPeer)
UI_MENU_DEFINE_STRING(CascomNetRxFilter)
#endif

#if defined(USE_PORTAUDIO) || defined(ARCHDEP_HAVE_SERIAL_CONTROL)

/* upper bound on host devices shown in a picker (PortAudio/serial enumeration) */
#define CASCOM_MENU_MAX_DEVICES 64

static void cascom_dyn_menu_free(ui_menu_entry_t *menu)
{
    int i;

    for (i = 0; menu[i].string != NULL; i++) {
        lib_free(menu[i].string);
        lib_free(menu[i].data);
    }
}

#endif /* USE_PORTAUDIO || ARCHDEP_HAVE_SERIAL_CONTROL */

#ifdef USE_PORTAUDIO

/* dynamically-built device pickers on string resources (cf. SoundDeviceName):
   list entry [0] is always "" = system default / none */
static ui_menu_entry_t cascom_txaudio_dyn_menu[CASCOM_MENU_MAX_DEVICES + 1];
static ui_menu_entry_t cascom_rxaudio_dyn_menu[CASCOM_MENU_MAX_DEVICES + 1];
static int cascom_txaudio_dyn_menu_init = 0;
static int cascom_rxaudio_dyn_menu_init = 0;

static UI_MENU_CALLBACK(cascom_txaudio_dynmenu_callback)
{
    char **devices = cascom_audio_device_list(0);   /* playback */
    int i;

    if (cascom_txaudio_dyn_menu_init != 0) {
        cascom_dyn_menu_free(cascom_txaudio_dyn_menu);
    } else {
        cascom_txaudio_dyn_menu_init = 1;
    }

    for (i = 0; devices[i] != NULL && i < CASCOM_MENU_MAX_DEVICES; i++) {
        cascom_txaudio_dyn_menu[i].action   = ACTION_NONE;
        cascom_txaudio_dyn_menu[i].string   = lib_strdup(devices[i][0] != '\0' ? devices[i] : "(system default)");
        cascom_txaudio_dyn_menu[i].type     = MENU_ENTRY_RESOURCE_RADIO;
        cascom_txaudio_dyn_menu[i].callback = radio_CascomTxAudioDev_callback;
        cascom_txaudio_dyn_menu[i].data     = (ui_callback_data_t)lib_strdup(devices[i]);
    }
    cascom_txaudio_dyn_menu[i].string = NULL;

    cascom_device_list_free(devices);
    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(cascom_rxaudio_dynmenu_callback)
{
    char **devices = cascom_audio_device_list(1);   /* capture */
    int i;

    if (cascom_rxaudio_dyn_menu_init != 0) {
        cascom_dyn_menu_free(cascom_rxaudio_dyn_menu);
    } else {
        cascom_rxaudio_dyn_menu_init = 1;
    }

    for (i = 0; devices[i] != NULL && i < CASCOM_MENU_MAX_DEVICES; i++) {
        cascom_rxaudio_dyn_menu[i].action   = ACTION_NONE;
        cascom_rxaudio_dyn_menu[i].string   = lib_strdup(devices[i][0] != '\0' ? devices[i] : "(system default)");
        cascom_rxaudio_dyn_menu[i].type     = MENU_ENTRY_RESOURCE_RADIO;
        cascom_rxaudio_dyn_menu[i].callback = radio_CascomRxAudioDev_callback;
        cascom_rxaudio_dyn_menu[i].data     = (ui_callback_data_t)lib_strdup(devices[i]);
    }
    cascom_rxaudio_dyn_menu[i].string = NULL;

    cascom_device_list_free(devices);
    return MENU_SUBMENU_STRING;
}

#endif /* USE_PORTAUDIO */

#ifdef ARCHDEP_HAVE_SERIAL_CONTROL

static ui_menu_entry_t cascom_pttdev_dyn_menu[CASCOM_MENU_MAX_DEVICES + 1];
static int cascom_pttdev_dyn_menu_init  = 0;

static UI_MENU_CALLBACK(cascom_pttdev_dynmenu_callback)
{
    char **devices = cascom_serial_device_list();
    int i;

    if (cascom_pttdev_dyn_menu_init != 0) {
        cascom_dyn_menu_free(cascom_pttdev_dyn_menu);
    } else {
        cascom_pttdev_dyn_menu_init = 1;
    }

    for (i = 0; devices[i] != NULL && i < CASCOM_MENU_MAX_DEVICES; i++) {
        cascom_pttdev_dyn_menu[i].action   = ACTION_NONE;
        cascom_pttdev_dyn_menu[i].string   = lib_strdup(devices[i][0] != '\0' ? devices[i] : "(none)");
        cascom_pttdev_dyn_menu[i].type     = MENU_ENTRY_RESOURCE_RADIO;
        cascom_pttdev_dyn_menu[i].callback = radio_CascomPttDevice_callback;
        cascom_pttdev_dyn_menu[i].data     = (ui_callback_data_t)lib_strdup(devices[i]);
    }
    cascom_pttdev_dyn_menu[i].string = NULL;

    cascom_device_list_free(devices);
    return MENU_SUBMENU_STRING;
}

#endif /* ARCHDEP_HAVE_SERIAL_CONTROL */

static const ui_menu_entry_t cascom_mode_menu[] = {
    {   .string   = "Loopback (self-test)",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_CascomMode_callback,
        .data     = (ui_callback_data_t)CASCOM_MODE_LOOPBACK
    },
#ifdef USE_PORTAUDIO
    {   .string   = "Audio (soundcard AFSK)",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_CascomMode_callback,
        .data     = (ui_callback_data_t)CASCOM_MODE_AUDIO
    },
#endif
#ifdef HAVE_NETWORK
    {   .string   = "Network (AXUDP)",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_CascomMode_callback,
        .data     = (ui_callback_data_t)CASCOM_MODE_NET
    },
#endif
    SDL_MENU_LIST_END
};

#ifdef USE_PORTAUDIO

#define CASCOM_RATE_MENU(cb)                                                \
    {   .string = "8000",  .type = MENU_ENTRY_RESOURCE_RADIO,               \
        .callback = cb, .data = (ui_callback_data_t)8000  },                \
    {   .string = "11025", .type = MENU_ENTRY_RESOURCE_RADIO,               \
        .callback = cb, .data = (ui_callback_data_t)11025 },                \
    {   .string = "16000", .type = MENU_ENTRY_RESOURCE_RADIO,               \
        .callback = cb, .data = (ui_callback_data_t)16000 },                \
    {   .string = "22050", .type = MENU_ENTRY_RESOURCE_RADIO,               \
        .callback = cb, .data = (ui_callback_data_t)22050 },                \
    {   .string = "24000", .type = MENU_ENTRY_RESOURCE_RADIO,               \
        .callback = cb, .data = (ui_callback_data_t)24000 },                \
    {   .string = "32000", .type = MENU_ENTRY_RESOURCE_RADIO,               \
        .callback = cb, .data = (ui_callback_data_t)32000 },                \
    {   .string = "44100", .type = MENU_ENTRY_RESOURCE_RADIO,               \
        .callback = cb, .data = (ui_callback_data_t)44100 },                \
    {   .string = "48000", .type = MENU_ENTRY_RESOURCE_RADIO,               \
        .callback = cb, .data = (ui_callback_data_t)48000 },                \
    {   .string = "96000", .type = MENU_ENTRY_RESOURCE_RADIO,               \
        .callback = cb, .data = (ui_callback_data_t)96000 },                \
    SDL_MENU_LIST_END

static const ui_menu_entry_t cascom_txrate_menu[] = {
    CASCOM_RATE_MENU(radio_CascomTxRate_callback)
};

static const ui_menu_entry_t cascom_rxrate_menu[] = {
    CASCOM_RATE_MENU(radio_CascomRxRate_callback)
};

#undef CASCOM_RATE_MENU

static const ui_menu_entry_t cascom_audio_menu[] = {
    SDL_MENU_ITEM_TITLE("Transmit (TX)"),
    {   .string   = "TX audio device",
        .type     = MENU_ENTRY_DYNAMIC_SUBMENU,
        .callback = cascom_txaudio_dynmenu_callback,
        .data     = (ui_callback_data_t)cascom_txaudio_dyn_menu
    },
    {   .string   = "TX sample rate",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)cascom_txrate_menu
    },
    {   .string   = "TX gain (0-100)",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_CascomTxGain_callback,
        .data     = (ui_callback_data_t)"Set TX gain (0-100)"
    },
    {   .string   = "Gate TX audio on PTT",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_CascomTxGate_callback
    },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Receive (RX)"),
    {   .string   = "RX audio device",
        .type     = MENU_ENTRY_DYNAMIC_SUBMENU,
        .callback = cascom_rxaudio_dynmenu_callback,
        .data     = (ui_callback_data_t)cascom_rxaudio_dyn_menu
    },
    {   .string   = "RX sample rate",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)cascom_rxrate_menu
    },
    SDL_MENU_LIST_END
};

#endif /* USE_PORTAUDIO */

#ifdef ARCHDEP_HAVE_SERIAL_CONTROL

static const ui_menu_entry_t cascom_pttline_menu[] = {
    {   .string   = "RTS",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_CascomPttLine_callback,
        .data     = (ui_callback_data_t)0
    },
    {   .string   = "DTR",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_CascomPttLine_callback,
        .data     = (ui_callback_data_t)1
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t cascom_ptt_menu[] = {
    {   .string   = "Serial device",
        .type     = MENU_ENTRY_DYNAMIC_SUBMENU,
        .callback = cascom_pttdev_dynmenu_callback,
        .data     = (ui_callback_data_t)cascom_pttdev_dyn_menu
    },
    {   .string   = "Control line",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)cascom_pttline_menu
    },
    {   .string   = "Invert PTT polarity",
        .type     = MENU_ENTRY_RESOURCE_TOGGLE,
        .callback = toggle_CascomPttInvert_callback
    },
    SDL_MENU_LIST_END
};

#endif /* ARCHDEP_HAVE_SERIAL_CONTROL */

#ifdef HAVE_NETWORK

static const ui_menu_entry_t cascom_net_menu[] = {
    {   .string   = "Peer host/IP",
        .type     = MENU_ENTRY_RESOURCE_STRING,
        .callback = string_CascomNetPeer_callback,
        .data     = (ui_callback_data_t)"Set network peer host/IP"
    },
    {   .string   = "Peer UDP port",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_CascomNetPort_callback,
        .data     = (ui_callback_data_t)"Set peer UDP port (1-65535)"
    },
    {   .string   = "Listen UDP port",
        .type     = MENU_ENTRY_RESOURCE_INT,
        .callback = int_CascomNetListenPort_callback,
        .data     = (ui_callback_data_t)"Set listen UDP port (1-65535)"
    },
    {   .string   = "RX callsign filter",
        .type     = MENU_ENTRY_RESOURCE_STRING,
        .callback = string_CascomNetRxFilter_callback,
        .data     = (ui_callback_data_t)"Set RX callsign accept-list"
    },
    SDL_MENU_LIST_END
};

#endif /* HAVE_NETWORK */

const ui_menu_entry_t cascom_device_menu[] = {
    {   .string   = "Mode",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)cascom_mode_menu
    },
#if defined(USE_PORTAUDIO) || defined(ARCHDEP_HAVE_SERIAL_CONTROL)
    SDL_MENU_ITEM_SEPARATOR,

    SDL_MENU_ITEM_TITLE("Audio (soundcard AFSK) mode"),
#ifdef USE_PORTAUDIO
    {   .string   = "Audio modem settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)cascom_audio_menu
    },
#endif
#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
    {   .string   = "PTT (radio keying)",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)cascom_ptt_menu
    },
#endif
#endif /* USE_PORTAUDIO || ARCHDEP_HAVE_SERIAL_CONTROL */
#ifdef HAVE_NETWORK
    SDL_MENU_ITEM_SEPARATOR,

    SDL_MENU_ITEM_TITLE("Network (AXUDP) mode"),
    {   .string   = "Network gateway settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)cascom_net_menu
    },
#endif
    SDL_MENU_LIST_END
};

const ui_menu_entry_t tapeport_devices_menu[] = {
    {   .string   = "Tapeport devices",
        .type     = MENU_ENTRY_DYNAMIC_SUBMENU,
        .callback = TapePort1Device_dynmenu_callback,
        .data     = (ui_callback_data_t)tapeport_dyn_menu[TAPEPORT_PORT_1]
    },
    {   .string   = "CP Clock F83 device settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)cpclockf83_device_menu
    },
    {   .string   = "tapecart device settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)tapecart_submenu
    },
    {   .string   = "CASCOM modem settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)cascom_device_menu
    },
    SDL_MENU_LIST_END
};

const ui_menu_entry_t tapeport_pet_devices_menu[] = {
    {   .string   = "Tapeport 1 devices",
        .type     = MENU_ENTRY_DYNAMIC_SUBMENU,
        .callback = TapePort1Device_dynmenu_callback,
        .data     = (ui_callback_data_t)tapeport_dyn_menu[TAPEPORT_PORT_1]
    },
    {   .string   = "Tapeport 2 devices",
        .type     = MENU_ENTRY_DYNAMIC_SUBMENU,
        .callback = TapePort2Device_dynmenu_callback,
        .data     = (ui_callback_data_t)tapeport_dyn_menu[TAPEPORT_PORT_2]
    },
    {   .string   = "CP Clock F83 device settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)cpclockf83_device_menu
    },
    SDL_MENU_LIST_END
};

void uitapeport_menu_shutdown(void)
{
    int i;

    for (i = 0; i < TAPEPORT_MAX_PORTS; i++) {
        if (tapeport_dyn_menu_init[i]) {
            sdl_menu_tapeport_free(i);
        }
    }

#ifdef USE_PORTAUDIO
    if (cascom_txaudio_dyn_menu_init) {
        cascom_dyn_menu_free(cascom_txaudio_dyn_menu);
    }
    if (cascom_rxaudio_dyn_menu_init) {
        cascom_dyn_menu_free(cascom_rxaudio_dyn_menu);
    }
#endif
#ifdef ARCHDEP_HAVE_SERIAL_CONTROL
    if (cascom_pttdev_dyn_menu_init) {
        cascom_dyn_menu_free(cascom_pttdev_dyn_menu);
    }
#endif
}

void uitape_menu_create(int has_tapeport)
{
    int i;

    if (!has_tapeport) {
        /* copy over 'no tapeport' menu if there is no tapeport */
        tape_menu[0] = tape_menu_no_tapeport[0];
        tape_menu[1] = tape_menu_no_tapeport[1];
    } else {
        for (i = 0; tape_menu_template[i].string != NULL; i++) {
            tape_menu[i] = tape_menu_template[i];
        }
        /* terminate list */
        tape_menu[i].string = NULL;
    }
}
