/*
 * uisound.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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
#ifdef AMIGA_M68K
#define _INLINE_MUIMASTER_H
#endif
#include "mui.h"

#include "sound.h"
#include "uisound.h"

static const char *ui_sound_freq[] = {
  "8000 Hz",
  "11025 Hz",
  "22050 Hz",
  "44100 Hz",
  NULL
};

static const int ui_sound_freq_values[] = {
  8000,
  11025,
  22050,
  44100,
  -1
};

static const char *ui_sound_buffer[] = {
  "100 msec",
  "150 msec",
  "200 msec",
  "250 msec",
  "300 msec",
  "350 msec",
  NULL
};

static const int ui_sound_buffer_values[] = {
  100,
  150,
  200,
  250,
  300,
  350,
  -1
};

static const char *ui_sound_oversample[] = {
  "None",
  "2x",
  "4x",
  "8x",
  NULL
};

static const int ui_sound_oversample_values[] = {
  0,
  1,
  2,
  3,
  -1
};

static const char *ui_sound_adjusting[] = {
  "Flexible",
  "Adjusting",
  "Exact",
  NULL
};

static const int ui_sound_adjusting_values[] = {
  SOUND_ADJUST_FLEXIBLE,
  SOUND_ADJUST_ADJUSTING,
  SOUND_ADJUST_EXACT,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "SoundSampleRate", ui_sound_freq, ui_sound_freq_values },
  { NULL, MUI_TYPE_CYCLE, "SoundBufferSize", ui_sound_buffer, ui_sound_buffer_values },
  { NULL, MUI_TYPE_CYCLE, "SoundOversample", ui_sound_oversample, ui_sound_oversample_values },
  { NULL, MUI_TYPE_CYCLE, "SoundSpeedAdjustment", ui_sound_adjusting, ui_sound_adjusting_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, "Sample Rate", ui_sound_freq)
    CYCLE(ui_to_from[1].object, "Buffer Size", ui_sound_buffer)
    CYCLE(ui_to_from[2].object, "Oversample", ui_sound_oversample)
    CYCLE(ui_to_from[3].object, "Speed Adjustment", ui_sound_adjusting)
  End;
}

void ui_sound_settings_dialog(void)
{
  mui_show_dialog(build_gui(), "Sound Settings", ui_to_from);
}
