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

#include "sound.h"
#include "uimenu.h"


UI_MENU_DEFINE_TOGGLE(Sound)
UI_MENU_DEFINE_RADIO(SoundSpeedAdjustment)
UI_MENU_DEFINE_RADIO(SoundSampleRate)
UI_MENU_DEFINE_RADIO(SoundBufferSize)
UI_MENU_DEFINE_RADIO(SoundSuspendTime)
UI_MENU_DEFINE_RADIO(SoundOversample)

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
      (ui_callback_data_t)300, NULL },
    { N_("*0.25 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)250, NULL },
    { N_("*0.20 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)200, NULL },
    { N_("*0.15 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)150, NULL },
    { N_("*0.10 sec"), (ui_callback_t)radio_SoundBufferSize,
      (ui_callback_data_t)100, NULL },
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

ui_menu_entry_t set_sound_oversample_submenu [] = {
    { "*1x",
      (ui_callback_t)radio_SoundOversample, (ui_callback_data_t)0, NULL },
    { "*2x",
      (ui_callback_t)radio_SoundOversample, (ui_callback_data_t)1, NULL },
    { "*4x",
      (ui_callback_t)radio_SoundOversample, (ui_callback_data_t)2, NULL },
    { "*8x",
      (ui_callback_t)radio_SoundOversample, (ui_callback_data_t)3, NULL },
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
    { N_("Sound synchronization"),
      NULL, NULL, set_sound_adjustment_submenu },
    { "--" },
    { N_("Sample rate"),
      NULL, NULL, set_sound_sample_rate_submenu },
    { N_("Buffer size"),
      NULL, NULL, set_sound_buffer_size_submenu },
    { N_("Suspend time"),
      NULL, NULL, set_sound_suspend_time_submenu },
    { N_("Oversample"),
      NULL, NULL, set_sound_oversample_submenu },
    { NULL },
};

ui_menu_entry_t ui_sound_settings_menu[] = {
    { N_("Sound settings"),
      NULL, NULL, sound_settings_submenu },
    { NULL }
};

