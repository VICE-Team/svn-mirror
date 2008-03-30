/*
 * soundwav.c - Implementation of the RIFF/WAV dump sound device
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Dag Lem <resid@nimrod.no>
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

#ifdef STDC_HEADERS
#include <stdio.h>
#endif

#include "sound.h"
#include "utils.h"

static FILE *wav_fd = NULL;
static int samples = 0;

static int wav_init(warn_t *w, const char *param, int *speed,
		   int *fragsize, int *fragnr, double bufsize)
{
    /* RIFF/WAV header. */
    char header[45] =
      "RIFFxxxxWAVEfmt \020\0\0\0\001\0\001\0xxxxxxxx\002\0\020\0dataxxxx";
    char rate[5];

    if (!param)
	param = "vicesnd.wav";
    wav_fd = fopen(param, "w");
    if (!wav_fd)
	return 1;

    samples = 0;

    /* Sampling rate. */
    sprintf(rate, "%c%c%c%c",
	    *speed, *speed >> 8, *speed >> 16, *speed >> 24);
    memcpy(header + 24, rate, 4);

    /* Bytes per second. */
    sprintf(rate, "%c%c%c%c",
	    *speed*2, *speed*2 >> 8, *speed*2 >> 16, *speed*2 >> 24);
    memcpy(header + 28, rate, 4);

    if (fwrite(header, 1, 44, wav_fd) != 44)
	return 1;

    return 0;
}

static int wav_write(warn_t *w, SWORD *pbuf, int nr)
{
    int	i;

    /* Swap bytes on big endian machines. */
#ifdef WORDS_BIGENDIAN
    for (i = 0; i < nr; i++) {
      pbuf[i] = (((WORD)pbuf[i] & 0xff) << 8) | ((WORD)pbuf[i] >> 8);
    }
#endif

    i = fwrite(pbuf, sizeof(SWORD), nr, wav_fd);
    if (i != nr)
	return 1;

    /* Swap the bytes back just in case. */
#ifdef WORDS_BIGENDIAN
    for (i = 0; i < nr; i++) {
      pbuf[i] = (((WORD)pbuf[i] & 0xff) << 8) | ((WORD)pbuf[i] >> 8);
    }
#endif

    /* Accumulate number of samples. */
    samples += nr;

    return 0;
}

static void wav_close(warn_t *w)
{
    char len[5];
    int bytes;

    /* RIFF header. */
    bytes = samples*2 + 36;
    sprintf(len, "%c%c%c%c",
	    bytes, bytes >> 8, bytes >> 16, bytes >> 24);
    fseek(wav_fd, 4, SEEK_SET);
    fwrite(len, 1, 4, wav_fd);

    /* WAVE data chunk. */
    bytes = samples*2;
    sprintf(len, "%c%c%c%c",
	    bytes, bytes >> 8, bytes >> 16, bytes >> 24);
    fseek(wav_fd, 32, SEEK_CUR);
    fwrite(len, 1, 4, wav_fd);

    fclose(wav_fd);
    wav_fd = NULL;
}

static sound_device_t wav_device =
{
    "wav",
    wav_init,
    wav_write,
    NULL,
    NULL,
    NULL,
    wav_close,
    NULL,
    NULL
};

int sound_init_wav_device(void)
{
    return sound_register_device(&wav_device);
}
