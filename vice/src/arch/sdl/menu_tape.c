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

#include "attach.h"
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
#include "uifilereq.h"
#include "uimenu.h"
#include "uimsgbox.h"
#include "util.h"

UI_MENU_DEFINE_RADIO(TapePort1Device)

static ui_menu_entry_t tapeport_dyn_menu[TAPEPORT_MAX_DEVICES + 1];

static int tapeport_dyn_menu_init = 0;

static void sdl_menu_tapeport_free(void)
{
    int i;

    for (i = 0; tapeport_dyn_menu[i].string != NULL; i++) {
        lib_free(tapeport_dyn_menu[i].string);
    }
}

static UI_MENU_CALLBACK(TapePort1Device_dynmenu_callback)
{
    tapeport_desc_t *devices = tapeport_get_valid_devices(TAPEPORT_PORT_1, 1);
    int i;

    /* rebuild menu if it already exists. */
    if (tapeport_dyn_menu_init != 0) {
        sdl_menu_tapeport_free();
    } else {
        tapeport_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        tapeport_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        tapeport_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        tapeport_dyn_menu[i].callback = radio_TapePort1Device_callback;
        tapeport_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    tapeport_dyn_menu[i].string = NULL;
    tapeport_dyn_menu[i].type = 0;
    tapeport_dyn_menu[i].callback = NULL;
    tapeport_dyn_menu[i].data = NULL;

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
    }
    return NULL;
}

static UI_MENU_CALLBACK(detach_tape1_callback)
{
    if (activated) {
        tape_image_detach(TAPEPORT_PORT_1 + 1);
    }
    return NULL;
}

static UI_MENU_CALLBACK(detach_tape2_callback)
{
    if (activated) {
        tape_image_detach(TAPEPORT_PORT_2 + 1);
    }
    return NULL;
}

static UI_MENU_CALLBACK(custom_datasette_control1_callback)
{
    if (activated) {
        datasette_control(TAPEPORT_PORT_1, vice_ptr_to_int(param));
    }
    return NULL;
}

static UI_MENU_CALLBACK(custom_datasette_control2_callback)
{
    if (activated) {
        datasette_control(TAPEPORT_PORT_2, vice_ptr_to_int(param));
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
UI_MENU_DEFINE_TOGGLE(VirtualDevice1)

const ui_menu_entry_t tape_pet_menu[] = {
    { "Attach tape image to Datasette 1",
      MENU_ENTRY_DIALOG,
      attach_tape1_callback,
      NULL },
    { "Attach tape image to Datasette 2",
      MENU_ENTRY_DIALOG,
      attach_tape2_callback,
      NULL },
    { "Detach tape image from Datasette 1",
      MENU_ENTRY_OTHER,
      detach_tape1_callback,
      NULL },
    { "Detach tape image from Datasette 2",
      MENU_ENTRY_OTHER,
      detach_tape2_callback,
      NULL },
    { "Create new tape image",
      MENU_ENTRY_DIALOG,
      create_tape_image_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Datasette 1 control"),
    { "Stop",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_STOP },
    { "Play",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_START },
    { "Forward",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_FORWARD },
    { "Rewind",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_REWIND },
    { "Record",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_RECORD },
    { "Reset",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_RESET },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Datasette 2 control"),
    { "Stop",
      MENU_ENTRY_OTHER,
      custom_datasette_control2_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_STOP },
    { "Play",
      MENU_ENTRY_OTHER,
      custom_datasette_control2_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_START },
    { "Forward",
      MENU_ENTRY_OTHER,
      custom_datasette_control2_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_FORWARD },
    { "Rewind",
      MENU_ENTRY_OTHER,
      custom_datasette_control2_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_REWIND },
    { "Record",
      MENU_ENTRY_OTHER,
      custom_datasette_control2_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_RECORD },
    { "Reset",
      MENU_ENTRY_OTHER,
      custom_datasette_control2_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_RESET },
    SDL_MENU_ITEM_SEPARATOR,
    { "Datasette speed tuning",
      MENU_ENTRY_RESOURCE_INT,
      int_DatasetteSpeedTuning_callback,
      (ui_callback_data_t)"Set datasette speed tuning" },
    { "Datasette zero gap delay",
      MENU_ENTRY_RESOURCE_INT,
      int_DatasetteZeroGapDelay_callback,
      (ui_callback_data_t)"Set datasette zero gap delay" },
    { "Datasette tape wobble frequency",
      MENU_ENTRY_RESOURCE_INT,
      int_DatasetteTapeWobbleFrequency_callback,
      (ui_callback_data_t)"Set datasette tape wobble frequency" },
    { "Datasette tape wobble amplitude",
      MENU_ENTRY_RESOURCE_INT,
      int_DatasetteTapeWobbleAmplitude_callback,
      (ui_callback_data_t)"Set datasette tape wobble amplitude" },
    { "Datasette tape alignment",
      MENU_ENTRY_RESOURCE_INT,
      int_DatasetteTapeAzimuthError_callback,
      (ui_callback_data_t)"Set datasette alignment error" },
    { "Reset Datasette on CPU Reset",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_DatasetteResetWithCPU_callback,
      NULL },
    { "Enable Datasette sound",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_DatasetteSound_callback,
      NULL },
    { "Enable virtual device (for t64)",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VirtualDevice1_callback,
      NULL },
    SDL_MENU_LIST_END
};

const ui_menu_entry_t tape_menu[] = {
    { "Attach tape image",
      MENU_ENTRY_DIALOG,
      attach_tape1_callback,
      NULL },
    { "Detach tape image",
      MENU_ENTRY_OTHER,
      detach_tape1_callback,
      NULL },
    { "Create new tape image",
      MENU_ENTRY_DIALOG,
      create_tape_image_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Datasette control"),
    { "Stop",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_STOP },
    { "Play",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_START },
    { "Forward",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_FORWARD },
    { "Rewind",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_REWIND },
    { "Record",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_RECORD },
    { "Reset",
      MENU_ENTRY_OTHER,
      custom_datasette_control1_callback,
      (ui_callback_data_t)DATASETTE_CONTROL_RESET },
    SDL_MENU_ITEM_SEPARATOR,
    { "Datasette speed tuning",
      MENU_ENTRY_RESOURCE_INT,
      int_DatasetteSpeedTuning_callback,
      (ui_callback_data_t)"Set datasette speed tuning" },
    { "Datasette zero gap delay",
      MENU_ENTRY_RESOURCE_INT,
      int_DatasetteZeroGapDelay_callback,
      (ui_callback_data_t)"Set datasette zero gap delay" },
    { "Datasette tape wobble frequency",
      MENU_ENTRY_RESOURCE_INT,
      int_DatasetteTapeWobbleFrequency_callback,
      (ui_callback_data_t)"Set datasette tape wobble frequency" },
    { "Datasette tape wobble amplitude",
      MENU_ENTRY_RESOURCE_INT,
      int_DatasetteTapeWobbleAmplitude_callback,
      (ui_callback_data_t)"Set datasette tape wobble amplitude" },
    { "Datasette tape alignment",
      MENU_ENTRY_RESOURCE_INT,
      int_DatasetteTapeAzimuthError_callback,
      (ui_callback_data_t)"Set datasette alignment error" },
    { "Reset Datasette on CPU Reset",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_DatasetteResetWithCPU_callback,
      NULL },
    { "Enable Datasette sound",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_DatasetteSound_callback,
      NULL },
    { "Enable virtual device (for t64)",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VirtualDevice1_callback,
      NULL },
    SDL_MENU_LIST_END
};

UI_MENU_DEFINE_TOGGLE(CPClockF83Save)

const ui_menu_entry_t cpclockf83_device_menu[] = {
    { "Save CP Clock F83 RTC data when changed",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CPClockF83Save_callback,
      NULL },
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
    { "Save tapecart data when changed",
        MENU_ENTRY_RESOURCE_TOGGLE,
        toggle_TapecartUpdateTCRT_callback,
        NULL },
    { "Optimize tapecart data when changed",
        MENU_ENTRY_RESOURCE_TOGGLE,
        toggle_TapecartOptimizeTCRT_callback,
        NULL },
    { "tapecart Log level",
        MENU_ENTRY_RESOURCE_INT,
        int_TapecartLoglevel_callback,
        (ui_callback_data_t)"Set tapecart log level" },
    { "TCRT filename",
        MENU_ENTRY_DIALOG,
        file_string_TapecartTCRTFilename_callback,
        (ui_callback_data_t)"Select TCRT file" },
    { "Flush current image",
        MENU_ENTRY_OTHER,
        tapecart_flush_callback,
        NULL },
    SDL_MENU_LIST_END
};

const ui_menu_entry_t tapeport_devices_menu[] = {
    { "Tapeport devices",
      MENU_ENTRY_DYNAMIC_SUBMENU,
      TapePort1Device_dynmenu_callback,
      (ui_callback_data_t)tapeport_dyn_menu },
    { "CP Clock F83 device settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)cpclockf83_device_menu },
    { "tapecart device settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)tapecart_submenu },
    SDL_MENU_LIST_END
};

void uitapeport_menu_shutdown(void)
{
    if (tapeport_dyn_menu_init) {
        sdl_menu_tapeport_free();
    }
}
