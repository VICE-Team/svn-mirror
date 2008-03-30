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

#include <stdio.h>

#include "sound.h"
#include "types.h"
#include "utils.h"

static FILE *wav_fd = NULL;
static int samples = 0;

static int wav_init(const char *param, int *speed,
		   int *fragsize, int *fragnr, double bufsize)
{
    /* RIFF/WAV header. */
    BYTE header[45] =
        "RIFFxxxxWAVEfmt \020\0\0\0\001\0\001\0xxxxxxxx\002\0\020\0dataxxxx";
    DWORD samples_per_sec = *speed;
    DWORD bytes_per_sec = *speed*2;
    int i;

    wav_fd = fopen(param?param:"vicesnd.wav", "w");
    if (!wav_fd)
	return 1;

    /* Reset number of samples. */
    samples = 0;

    /* Sampling rate and bytes per second stored as little endian numbers. */
    for (i = 0; i < 4; i++) {
        header[24 + i] = (BYTE)(samples_per_sec & 0xff);
        header[28 + i] = (BYTE)(bytes_per_sec & 0xff);
        samples_per_sec >>= 8;
        bytes_per_sec >>= 8;
    }

    return (fwrite(header, 1, 44, wav_fd) != 44);
}

static int wav_write(SWORD *pbuf, size_t nr)
{
#ifdef WORDS_BIGENDIAN
    int	i;

    /* Swap bytes on big endian machines. */
    for (i = 0; i < nr; i++) {
        pbuf[i] = (((WORD)pbuf[i] & 0xff) << 8) | ((WORD)pbuf[i] >> 8);
    }
#endif

    if (nr != fwrite(pbuf, sizeof(SWORD), nr, wav_fd))
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

static void wav_close(void)
{
    BYTE rlen[4];
    BYTE dlen[4];
    DWORD rifflen = samples*2 + 36;
    DWORD datalen = samples*2;
    int i;

    /* RIFF length and WAVE data length stored as little endian numbers. */
    for (i = 0; i < 4; i++) {
        rlen[i] = (BYTE)(rifflen & 0xff);
        dlen[i] = (BYTE)(datalen & 0xff);
        rifflen >>= 8;
        datalen >>= 8;
    }

    fseek(wav_fd, 4, SEEK_SET);
    fwrite(rlen, 1, 4, wav_fd);

    fseek(wav_fd, 32, SEEK_CUR);
    fwrite(dlen, 1, 4, wav_fd);

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

