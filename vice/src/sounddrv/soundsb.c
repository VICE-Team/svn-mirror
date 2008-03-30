/*
 * soundsb.c - Implementation of the Sound Blaster sound device.
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

/* Warning: this could be optimized a lot.  */

#include "vice.h"

#include <go32.h>
#include <dpmi.h>
#include <dos.h>
#include <sys/farptr.h>

#include <allegro.h>            /* Must come after everything else.  */

#include "sound.h"

#include "utils.h"
#include "vicesb.h"

/* ------------------------------------------------------------------------- */

/* Circular buffer for streaming.  */
#define MAX_AUDIO_BUFFER_SIZE 0x20000
static BYTE audio_buffer[MAX_AUDIO_BUFFER_SIZE];

/* Size of the audio buffer (bytes).  */
static unsigned int audio_buffer_size;

/* Number of bytes pending in the streaming buffer.  */
static volatile unsigned int num_bytes_in_buffer;

/* Pointer to the first sample waiting to be played in the buffer.  */
static volatile unsigned int first_sample;

/* Pointer to the place where the next streamed sample must to be put.  */
static unsigned int next_sample;

/* Fragment size (bytes).  */
static unsigned int fragment_size;

/* Flag: have we already detected the soundcard?  */
static int detect_done;

/* Flag: are we running in 16bit mode?  */
static int is_16bit;

/* Last sample played by the SB interrupt.  */
static WORD last_played_sample;

/* ------------------------------------------------------------------------- */

static void interrupt_function(unsigned long buf)
{
    if (num_bytes_in_buffer < fragment_size) {
        int i;

        /* Underflowing: repeat the last sample.  */

        _farsetsel(_dos_ds);

        if (is_16bit) {
            for (i = 0; i < fragment_size / 2; i++) {
                _farnspokew(buf, last_played_sample);
                buf += 2;
            }
        } else {
            for (i = 0; i < fragment_size; i++) {
                _farnspokeb(buf, (BYTE) last_played_sample);
                buf++;
            }
        }

    } else {

        /* Play one fragment.  */

        movedata(_my_ds(), (unsigned long) (audio_buffer + first_sample),
                 _dos_ds, buf, fragment_size);

        if (is_16bit)
            last_played_sample = *((WORD *)(audio_buffer + first_sample +
                                            fragment_size) - 1);
        else
            last_played_sample = *((BYTE *)(audio_buffer + first_sample +
                                            fragment_size) - 1);

        num_bytes_in_buffer -= fragment_size;

        first_sample += fragment_size;
        if (first_sample >= audio_buffer_size)
            first_sample = 0;
    }
}

static void interrupt_function_end(void) {}

static void lock_mem(void)
{
    _go32_dpmi_lock_code((void *) interrupt_function,
                         ((unsigned long) interrupt_function_end
                          - (unsigned long) interrupt_function));
    _go32_dpmi_lock_data((void *) audio_buffer, MAX_AUDIO_BUFFER_SIZE);
    _go32_dpmi_lock_data((void *) &audio_buffer_size, sizeof(audio_buffer_size));
    _go32_dpmi_lock_data((void *) &num_bytes_in_buffer, sizeof(num_bytes_in_buffer));
    _go32_dpmi_lock_data((void *) &first_sample, sizeof(first_sample));
    _go32_dpmi_lock_data((void *) &next_sample, sizeof(next_sample));
    _go32_dpmi_lock_data((void *) &fragment_size, sizeof(fragment_size));
    _go32_dpmi_lock_data((void *) &detect_done, sizeof(detect_done));
    _go32_dpmi_lock_data((void *) &is_16bit, sizeof(is_16bit));
}

static int sb_init(warn_t *w, const char *param, int *speed,
                   int *fragsize, int *fragnr, double bufsize)
{
    int tmp_fragsize = *fragsize;

    if (!detect_done) {
        if (!vicesb_detect(&is_16bit))
            return -1;
        detect_done = 1;
    }

    if (is_16bit)
        tmp_fragsize *= 2;

    /* XXX: We assume we are given a power of 2 as the `fragsize'.  */
    if (!vicesb_init(speed, &tmp_fragsize, interrupt_function))
        return -1;

    vicesb_set_volume(255);

#if 0                           /* (not sure this is a good thing) */
    /* If the fragment size is larger than requested, reduce the number of
       fragments accordingly.  */
    if (tmp_fragsize > *fragsize) {
        int new_fragnr = (int) ((double) *fragnr * ((double) *fragsize
                                                    / (double) *tmp_fragsize));

        if (new_fragnr > 0)
            *fragnr = new_fragnr;
    }
#endif

    if (is_16bit)
        *fragsize = tmp_fragsize / 2;
    else
        *fragsize = tmp_fragsize;

    fragment_size = tmp_fragsize;
    audio_buffer_size = *fragnr * fragment_size;

    /* FIXME: Check for audio buffer size.  */

    num_bytes_in_buffer = 0;
    first_sample = 0;
    next_sample = 0;

    last_played_sample = is_16bit ? 0x0 : 0x80;

    return 0;
}

static int sb_write(warn_t *w, SWORD *pbuf, int nr)
{
    int total;

    if (audio_buffer_size == 0)
        return -1;
    if (nr == 0)
        return 0;

    if (is_16bit)
        total = nr * 2;
    else
        total = nr;

    /* XXX: We only allow writing full fragments here.  */
    while (total >= fragment_size) {

        /* Block until there is space in the buffer.  The interrupt routine
           will decrement `num_bytes_in_buffer' as soon as a new fragment is
           played.  */
        while (num_bytes_in_buffer == audio_buffer_size)
            ;

        /* Disable interrupts to make sure we don't interfere with the
           playback routine in ugly ways.  */
        asm volatile ("cli");

        /* Write one fragment.  */

        if (!is_16bit) {
            BYTE *p = audio_buffer + next_sample;
            int i;

            for (i = 0; i < fragment_size; i++)
                *(p++) = (*(pbuf++) >> 8) + 0x80;
        } else {
            SWORD *p = (SWORD *) (audio_buffer + next_sample);
            int i;

            for (i = 0; i < fragment_size / 2; i++)
                *(p++) = *(pbuf++);
        }
        num_bytes_in_buffer += fragment_size;

        /* Done writing the fragment.  Enable interrupts again.  */
        asm volatile("sti");

        next_sample += fragment_size;
        if (next_sample >= audio_buffer_size)
            next_sample = 0;

        total -= fragment_size;
    }

    if (total != 0)
        printf("%s: Argh, we have a problem!  total = %d\n",
               __FUNCTION__, total);

    return 0;
}

static int sb_bufferstatus(warn_t *s, int first)
{
    int ret;

    if (first)
        return 0;

    ret = num_bytes_in_buffer;

    if (is_16bit)
        ret /= 2;

    return ret;
}

static void sb_close(warn_t *w)
{
    vicesb_close();

    audio_buffer_size = 0;
    num_bytes_in_buffer = 0;
    first_sample = 0;
    next_sample = 0;
    fragment_size = 0;
}

#if 0

static int sb_suspend(warn_t *w)
{
    return 0;
}

static int sb_resume(warn_t *w)
{
    return 0;
}

#endif

static sound_device_t sb_device =
{
    "sb",
    sb_init,
    sb_write,
    NULL,
    NULL,
    sb_bufferstatus,
    sb_close,
    NULL,
    NULL
};

int sound_init_sb_device(void)
{
    return sound_register_device(&sb_device);
}
