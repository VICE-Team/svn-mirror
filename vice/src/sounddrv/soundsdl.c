/*
 * soundsdl.c - Implementation of the Simple Directmedia Layer sound device
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
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

/* XXX: includes */

#include <SDL/SDL_audio.h>
#include <unistd.h>

#include "vice.h"

#include "lib.h"
#include "sound.h"


static SWORD *sdl_buf = NULL;
static SDL_AudioSpec sdl_spec;
static volatile int sdl_inptr = 0;
static volatile int sdl_outptr = 0;
static volatile int sdl_len = 0;

static void sdl_callback(void *userdata, Uint8 *stream, int len)
{
    int			amount, total;
    total = 0;
    while (total < len/sizeof(SWORD))
    {
	amount = sdl_inptr - sdl_outptr;
	if (amount < 0)
	    amount = sdl_len - sdl_outptr;
	if (amount + total > len/sizeof(SWORD))
	    amount = len/sizeof(SWORD) - total;
	if (!amount)
	{
	    memset(stream + total*sizeof(SWORD), 0, len - total*sizeof(SWORD));
		return;
	    }
	memcpy(stream + total*sizeof(SWORD), sdl_buf + sdl_outptr,
	       amount*sizeof(SWORD));
	total += amount;
	sdl_outptr += amount;
	if (sdl_outptr == sdl_len)
	    sdl_outptr = 0;
    }
}

static int sdl_init(const char *param, int *speed,
		    int *fragsize, int *fragnr, int *channels)
{
    SDL_AudioSpec		spec;

    /* No stereo capability. */
    *channels = 1;

    memset(&spec, 0, sizeof(spec));
    spec.freq = *speed;
    spec.format = AUDIO_S16;
    spec.channels = 1;
    spec.samples = *fragsize;
    spec.callback = sdl_callback;
    if(SDL_OpenAudio(&spec, &sdl_spec))
    {
	return 1;
    }
    if (sdl_spec.format != AUDIO_S16 || sdl_spec.channels != 1)
    {
	SDL_CloseAudio();
	return 1;
    }
    sdl_len = (*fragsize)*(*fragnr) + 1;
    sdl_inptr = sdl_outptr = 0;
    sdl_buf = lib_malloc(sizeof(SWORD)*sdl_len);
    if (!sdl_buf)
    {
	SDL_CloseAudio();
	return 1;
    }
    *speed = sdl_spec.freq;
    SDL_PauseAudio(0);
    return 0;
}

static int sdl_write(SWORD *pbuf, size_t nr)
{
    int			total, amount;
    total = 0;
    while (total < nr)
    {
	amount = sdl_outptr - sdl_inptr;
	if (amount <= 0)
	    amount = sdl_len - sdl_inptr;
	if ((sdl_inptr + amount)%sdl_len == sdl_outptr)
	    amount--;
	if (amount <= 0)
	{
/*	    Sleep(5); */
            usleep(5000);
	    continue;
	}
	if (total + amount > nr)
	    amount = nr - total;
	memcpy(sdl_buf + sdl_inptr, pbuf + total, amount*sizeof(SWORD));
	sdl_inptr += amount;
	total += amount;
	if (sdl_inptr == sdl_len)
	    sdl_inptr = 0;
    }
    return 0;
}

static int sdl_bufferspace(void)
{
    int		amount;
    amount = sdl_inptr - sdl_outptr;
    if (amount < 0)
	amount += sdl_len;
    return sdl_len - amount;
}

static void sdl_close(void)
{
    SDL_CloseAudio();
    free(sdl_buf);
    sdl_buf = NULL;
    sdl_inptr = sdl_outptr = sdl_len = 0;
}


static sound_device_t sdl_device =
{
    "sdl",
    sdl_init,
    sdl_write,
    NULL,
    NULL,
    sdl_bufferspace,
    sdl_close,
    NULL,
    NULL,
    1
};

int sound_init_sdl_device(void)
{
    return sound_register_device(&sdl_device);
}
