/*
 * soundallegro.c - Implementation of the Allegro sound device.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

/* FIXME: This code does not work yet.  */

#include "vice.h"

#include <stdio.h>
#include <allegro.h>            /* Must come after <stdio.h>.  */
#include <dpmi.h>

#include "sound.h"

#include "utils.h"

/* ------------------------------------------------------------------------- */

/* Flag: have we already initialized Allegro?  */
static int allegro_startup_done;

/* Audio buffer.  */
static SAMPLE *buffer;

/* Buffer size (bytes).  */
static unsigned int buffer_len;

/* Voice playing the buffer.  */
static int voice;

/* Size of fragment (bytes).  */
static unsigned int fragment_size;

/* Write position in the buffer.  */
static unsigned int buffer_offset;

/* ------------------------------------------------------------------------- */

static int allegro_startup(void)
{
    if (allegro_startup_done)
        return 0;

    printf("Starting up Allegro sound...  ");

    /* In any case, we will not try another time.  */
    allegro_startup_done = 1;

    detect_digi_driver(DIGI_AUTODETECT);
    reserve_voices(1, 0);

    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) != 0) {
        printf("Failed: %s\n", allegro_error);
        return -1;
    }

    set_volume(255, 0);

    printf("OK.\n");
    return 0;
}

/* (`allegro_init()' is already defined by Allegro itself.)  */
static int allegro_init_sound(warn_t *w, char *param, int *speed,
                              int *fragsize, int *fragnr, double bufsize)
{
    int i;

    if (allegro_startup() < 0)
        return 1;

    fragment_size = *fragsize * sizeof(SWORD);

    buffer_len = fragment_size * *fragnr;
    buffer = (SAMPLE *) xmalloc(sizeof(SAMPLE));
    _go32_dpmi_lock_data(buffer, sizeof(SAMPLE));

    buffer->bits = 16;
    buffer->freq = *speed;
    buffer->priority = 255;
    buffer->len = buffer_len / sizeof(SWORD);
    buffer->loop_start = 0;
    buffer->loop_end = buffer->len;
    buffer->param = -1;

    buffer->data = xmalloc(buffer_len);
    _go32_dpmi_lock_data(buffer->data, buffer_len);

    for (i = 0; i < buffer_len / 2; i++)
        *((WORD *)buffer->data + i) = 0x8000;

    voice = allocate_voice(buffer);
    if (voice < 0) {
        fprintf(stderr, "Cannot allocate Allegro voice!\n");
        _unlock_dpmi_data(buffer->data, buffer_len * 2);
        _unlock_dpmi_data(buffer, sizeof(SAMPLE));
        return 1;
    }

    buffer_offset = 0;

    voice_set_playmode(voice, PLAYMODE_LOOP);
    voice_set_volume(voice, 255);
    voice_set_pan(voice, 128);
    voice_start(voice);

    return 0;
}

static int allegro_write(warn_t *w, SWORD *pbuf, int nr)
{
    unsigned int i, count;
    unsigned int write_size;

    /* XXX: Assumes `nr' is multiple of `fragment_size'.  This is always the
       case with the current implementation.  */
    count = nr / (fragment_size / sizeof(SWORD));

    /* Write one fragment at a time.  FIXME: This could be faster.  */
    for (i = 0; i < count; i++, pbuf += fragment_size / sizeof(SWORD)) {
        unsigned int write_end;

        /* XXX: We do not use module here because we assume we always write
           full fragments.  */
        write_end = buffer_offset + fragment_size - 1;

        /* Block if we are at the position the soundcard is playing.  Notice
           that we also assume that the part of the buffer we are going to
           lock is small enough to fit in the safe space.  */
        while (1) {
            int pos = sizeof(SWORD) * voice_get_position(voice);
            int pos2 = pos + fragment_size;

            if (pos2 < buffer_len) {
                if (buffer_offset >= pos2 || write_end < pos)
                    break;
            } else {
                pos2 -= buffer_len;
                if (write_end < pos && buffer_offset >= pos2)
                    break;
            }
        }


        /* Write fragment.  */
	{
	    int j;
	    WORD *p = (WORD *) (buffer->data + buffer_offset);

            /* XXX: Maybe the SID engine could already produce samples in
               unsigned format as we need them here?  */
	    for (j = 0; j < fragment_size / sizeof(SWORD); j++)
	        p[j] = pbuf[j] + 0x8000;
	}

	buffer_offset += fragment_size;
        if (buffer_offset >= buffer_len)
            buffer_offset = 0;
    }

    return 0;
}

static int allegro_bufferstatus(warn_t *s, int first)
{
    int pos, ret;

    if (first)
        return 0;

    pos = voice_get_position(voice) * sizeof(SWORD);
    ret = buffer_offset - pos;
    if (ret < 0)
        ret += buffer_len;

    return ret / sizeof(SWORD);
}

static void allegro_close(warn_t *w)
{
    voice_stop(voice);
    deallocate_voice(voice);
    destroy_sample(buffer);
}

static sound_device_t allegro_device =
{
    "allegro",
    allegro_init_sound,
    allegro_write,
    NULL,
    NULL,
    allegro_bufferstatus,
    allegro_close,
    NULL,
    NULL
};

int sound_init_allegro_device(void)
{
    printf("Initializing Allegro sound device.\n");
    return sound_register_device(&allegro_device);
}
