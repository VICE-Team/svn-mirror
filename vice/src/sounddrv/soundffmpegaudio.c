/*
 * soundffmpegaudio.c - Implementation of the audio stream for movie encoding
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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
#include <string.h>

#include "log.h"
#include "../gfxoutputdrv/ffmpegdrv.h"
#include "sound.h"
#include "types.h"
#include "archdep.h"

static ffmpegdrv_audio_in_t *ffmpeg_audio_in;


static int ffmpegaudio_init(const char *param, int *speed,
		   int *fragsize, int *fragnr, int *channels)
{
    ffmpegdrv_init_audio(*speed, *channels, &ffmpeg_audio_in);

    return 0;
}


static int ffmpegaudio_write(SWORD *pbuf, size_t nr)
{
    size_t copied = 0;
    int samples_to_copy;

    if (ffmpeg_audio_in == NULL || ffmpeg_audio_in->buffersamples == 0)
        return 0;

    while (copied < nr) {
        samples_to_copy = ffmpeg_audio_in->buffersamples 
                            - ffmpeg_audio_in->used;
        if (samples_to_copy > (int)(nr - copied))
            samples_to_copy = nr - copied;
        memcpy(ffmpeg_audio_in->buffer + ffmpeg_audio_in->used, pbuf + copied,
                samples_to_copy * sizeof(SWORD));
        ffmpeg_audio_in->used += samples_to_copy;
        copied += samples_to_copy;
        if (ffmpeg_audio_in->used == ffmpeg_audio_in->buffersamples) {
            ffmpegdrv_encode_audio(ffmpeg_audio_in);
        }
    }

    return 0;
}


static void ffmpegaudio_close(void)
{
    ffmpeg_audio_in = NULL;
    /* movie recording may still run */
    screenshot_stop_recording();
}

static sound_device_t ffmpegaudio_device =
{
    "ffmpegaudio",
    ffmpegaudio_init,
    ffmpegaudio_write,
    NULL,
    NULL,
    NULL,
    ffmpegaudio_close,
    NULL,
    NULL,
    0
};

int sound_init_ffmpegaudio_device(void)
{
    return sound_register_device(&ffmpegaudio_device);
}
