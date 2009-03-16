/*
 * uisound.c - Implementation of sound UI settings.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <stdio.h>

#include "lib.h"
#include "sound.h"
#include "uilib.h"
#include "uimenu.h"
#include "uisound.h"


UI_MENU_DEFINE_TOGGLE(Sound)
UI_MENU_DEFINE_STRING_RADIO(SoundDeviceName)
UI_MENU_DEFINE_RADIO(SoundSpeedAdjustment)
UI_MENU_DEFINE_RADIO(SoundSampleRate)
UI_MENU_DEFINE_RADIO(SoundBufferSize)
UI_MENU_DEFINE_RADIO(SoundSuspendTime)
UI_MENU_DEFINE_RADIO(SoundOversample)

UI_CALLBACK(set_sound_device_arg)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("Sound driver argument"),
                        _("Argument:"));
}

ui_menu_entry_t set_sound_sample_rate_submenu[] = {
    { "*8000Hz", (ui_callback_t)radio_SoundSampleRate,
      (ui_callback_data_t)8000, NULL },
    { "*11025Hz", (ui_callback_t)radio_SoundSampleRate,
      (ui_callback_data_t)11025, NULL },
    { "*22050Hz", (ui_callback_t)radio_SoundSampleRate,
      (ui_callback_data_t)22050, NULL },
    { "*44100Hz", (ui_callback_t)radio_SoundSampleRate,
      (ui_callback_data_t)44100, NULL },
    { "*48000Hz", (ui_callback_t)radio_SoundSampleRate,
      (ui_callback_data_t)48000, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_buffer_size_submenu[] = {
    { N_("*1.00 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)1000, NULL },
    { N_("*0.75 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)750, NULL },
    { N_("*0.50 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)500, NULL },
    { N_("*0.35 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)350, NULL },
    { N_("*0.30 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)250, NULL },
    { N_("*0.20 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)150, NULL },
    { N_("*0.10 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)100, NULL },
    { N_("*0.09 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)80, NULL },
    { N_("*0.07 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)60, NULL },
    { N_("*0.05 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)50, NULL },
    { N_("*0.04 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)40, NULL },
    { N_("*0.03 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)30, NULL },
    { N_("*0.025 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)25, NULL },
    { N_("*0.02 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)20, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_suspend_time_submenu[] = {
    { N_("*Keep going"), (ui_callback_t)radio_SoundSuspendTime,
      (ui_callback_data_t)0, NULL },
    { N_("*1 sec suspend"), (ui_callback_t)radio_SoundSuspendTime,
      (ui_callback_data_t)1, NULL },
    { N_("*2 sec suspend"), (ui_callback_t)radio_SoundSuspendTime,
      (ui_callback_data_t)2, NULL },
    { N_("*5 sec suspend"), (ui_callback_t)radio_SoundSuspendTime,
      (ui_callback_data_t)5, NULL },
    { N_("*10 sec suspend"), (ui_callback_t)radio_SoundSuspendTime,
      (ui_callback_data_t)10, NULL },
    { NULL }
};

static ui_menu_entry_t set_sound_adjustment_submenu [] = {
    { N_("*Flexible"),
      (ui_callback_t)radio_SoundSpeedAdjustment,
      (ui_callback_data_t)SOUND_ADJUST_FLEXIBLE, NULL },
    { N_("*Adjusting"),
      (ui_callback_t)radio_SoundSpeedAdjustment,
      (ui_callback_data_t)SOUND_ADJUST_ADJUSTING, NULL },
    { N_("*Exact"),
      (ui_callback_t)radio_SoundSpeedAdjustment,
      (ui_callback_data_t)SOUND_ADJUST_EXACT, NULL },
    { NULL }
};

ui_menu_entry_t sound_settings_submenu[] = {
    { N_("*Enable sound playback"),
      (ui_callback_t)toggle_Sound, NULL, NULL },
    { "--" },
      /* Do not change position as position 2 is hard coded. */
    { N_("Sound device name"),
      NULL, NULL, NULL },
    { N_("Sound driver argument..."),
      (ui_callback_t)set_sound_device_arg,
      (ui_callback_data_t)"SoundDeviceArg", NULL },
    { "--" },
    { N_("Sound synchronization"),
      NULL, NULL, set_sound_adjustment_submenu },
    { "--" },
    { N_("Sample rate"),
      NULL, NULL, set_sound_sample_rate_submenu },
    { N_("Buffer size"),
      NULL, NULL, set_sound_buffer_size_submenu },
    { N_("Suspend time"),
      NULL, NULL, set_sound_suspend_time_submenu },
    { NULL },
};

ui_menu_entry_t ui_sound_settings_menu[] = {
    { N_("Sound settings"),
      NULL, NULL, sound_settings_submenu },
    { NULL }
};

void uisound_menu_create(void)
{
    unsigned int i, num;
    ui_menu_entry_t *devices_submenu;

    num = sound_device_num();

    if (num == 0)
        return;

    devices_submenu = (ui_menu_entry_t *)lib_calloc((size_t)(num + 1),
                      sizeof(ui_menu_entry_t));

    for (i = 0; i < num ; i++) {
        devices_submenu[i].string =
            (ui_callback_data_t)lib_msprintf("*%s", sound_device_name(i));
        devices_submenu[i].callback = (ui_callback_t)radio_SoundDeviceName;
        devices_submenu[i].callback_data
            = (ui_callback_data_t)lib_stralloc(sound_device_name(i));
    }

    sound_settings_submenu[2].sub_menu = devices_submenu;
}

void uisound_menu_shutdown(void)
{
    unsigned int i;
    ui_menu_entry_t *devices_submenu = NULL;

    devices_submenu = sound_settings_submenu[2].sub_menu;

    if (devices_submenu == NULL)
        return;

    sound_settings_submenu[2].sub_menu = NULL;

    i = 0;

    while (devices_submenu[i].string != NULL) {
        lib_free(devices_submenu[i].string);
        lib_free(devices_submenu[i].callback_data);
        i++;
    }

    lib_free(devices_submenu);
}

