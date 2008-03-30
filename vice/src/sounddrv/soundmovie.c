/*
 * soundmovie.c - Implementation of the audio stream for movie encoding
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

#include "avformat.h"
#include "log.h"
#include "../gfxoutputdrv/mpegdrv.h"
#include "sound.h"
#include "types.h"
#include "archdep.h"

static mpegdrv_audio_in_t *mpeg_audio_in;


static int movie_init(const char *param, int *speed,
		   int *fragsize, int *fragnr, int *channels)
{
    mpegdrv_init_audio(*speed, *channels, &mpeg_audio_in);

    return 0;
}


static int movie_write(SWORD *pbuf, size_t nr)
{
    size_t copied = 0;
    int samples_to_copy;

    if (mpeg_audio_in == NULL || mpeg_audio_in->buffersamples == 0)
        return 0;

    while (copied < nr) {
        samples_to_copy = mpeg_audio_in->buffersamples - mpeg_audio_in->used;
        if (samples_to_copy > nr - copied)
            samples_to_copy = nr - copied;
        memcpy(mpeg_audio_in->buffer + mpeg_audio_in->used, pbuf + copied,
                samples_to_copy * sizeof(SWORD));
        mpeg_audio_in->used += samples_to_copy;
        copied += samples_to_copy;
        if (mpeg_audio_in->used == mpeg_audio_in->buffersamples) {
            mpegdrv_encode_audio(mpeg_audio_in);
        }
    }

    return 0;
}


static void movie_close(void)
{
    mpeg_audio_in = NULL;
}

static sound_device_t movie_device =
{
    "movie",
    movie_init,
    movie_write,
    NULL,
    NULL,
    NULL,
    movie_close,
    NULL,
    NULL,
    0
};

int sound_init_movie_device(void)
{
    return sound_register_device(&movie_device);
}
