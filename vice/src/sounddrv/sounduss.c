/*
 * sounduss.c - Implementation of the Linux/FreeBSD sound device
 *
 * Written by
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "vice.h"
#include "sound.h"

#if defined(HAVE_LINUX_SOUNDCARD_H)
#include <linux/soundcard.h>
#endif
#if defined(HAVE_MACHINE_SOUNDCARD_H)
#include <machine/soundcard.h>
#endif

static int uss_fd = -1;
static int uss_8bit = 0;
static int uss_bufsize = 0;
static int uss_fragsize = 0;

static int uss_bufferstatus(warn_t *s, int first);

static int uss_init(warn_t *w, const char *param, int *speed,
		    int *fragsize, int *fragnr, double bufsize)
{
    int			 st, tmp, orig;
    if (!param)
	param = "/dev/dsp";
    uss_fd = open(param, O_WRONLY, 0777);
    if (uss_fd < 0)
    {
	warn(w, -1, "cannot open '%s' for writing", param);
	return 1;
    }
    /* samplesize 16 bits */
#ifdef WORDS_BIGENDIAN
    orig = tmp = AFMT_S16_BE;
#else
    orig = tmp = AFMT_S16_LE;
#endif
    st = ioctl(uss_fd, SNDCTL_DSP_SETFMT, &tmp);
    if (st < 0 || orig != tmp || getenv("USS8BIT"))
    {
	/* samplesize 8 bits */
	orig = tmp = AFMT_U8;
	st = ioctl(uss_fd, SNDCTL_DSP_SETFMT, &tmp);
	if (st < 0 || orig != tmp)
	{
	    warn(w, -1, "SNDCTL_DSP_SETFMT failed");
	    goto fail;
	}
	warn(w, -1, "playing 8bit sample");
	uss_8bit = 1;
    }
    /* no stereo */
    tmp = 0;
    st = ioctl(uss_fd, SNDCTL_DSP_STEREO, &tmp);
    if (st < 0 || tmp != 0)
    {
	warn(w, -1, "SNDCTL_DSP_STEREO failed");
	goto fail;
    }
    /* speed */
    tmp = *speed;
    st = ioctl(uss_fd, SNDCTL_DSP_SPEED, &tmp);
    if (st < 0 || tmp <= 0)
    {
	warn(w, -1, "SNDCTL_DSP_SPEED failed");
	goto fail;
    }
    *speed = tmp;
    /* fragments */
    for (tmp = 1; 1 << tmp < *fragsize; tmp++);
    orig = tmp = tmp + (*fragnr << 16) + !uss_8bit;
    st = ioctl(uss_fd, SNDCTL_DSP_SETFRAGMENT, &tmp);
    if (st < 0 || (tmp^orig)&0xffff)
    {
	warn(w, -1, "SNDCTL_DSP_SETFRAGMENT failed");
	goto fail;
    }
    if (tmp != orig)
    {
	if (tmp >> 16 > *fragnr)
	{
	    warn(w, -1, "SNDCTL_DSP_SETFRAGMENT: too many fragments");
	    goto fail;
	}
	*fragnr = tmp >> 16;
	if (*fragnr < 3)
	{
	    warn(w, -1, "SNDCTL_DSP_SETFRAGMENT: too few fragments");
	    goto fail;
	}
    }
    uss_bufsize = (*fragsize)*(*fragnr);
    uss_fragsize = *fragsize;
    return 0;
fail:
    close(uss_fd);
    uss_fd = -1;
    uss_8bit = 0;
    uss_bufsize = 0;
    uss_fragsize = 0;
    return 1;
}

static int uss_write(warn_t *w, SWORD *pbuf, int nr)
{
    int			total, i, now;
    if (uss_8bit)
    {
	/* XXX: ugly to change contents of the buffer */
	for (i = 0; i < nr; i++)
	    ((char *)pbuf)[i] = pbuf[i]/256 + 128;
	total = nr;
    }
    else
	total = nr*sizeof(SWORD);
    for (i = 0; i < total; i += now)
    {
	now = write(uss_fd, (char *)pbuf + i, total - i);
	if (now <= 0)
	{
	    if (now < 0)
		perror("uss_write");
	    return 1;
	}
    }
    return 0;
}

static int uss_bufferstatus(warn_t *w, int first)
{
    audio_buf_info		info;
    int				st, ret;

    st = ioctl(uss_fd, SNDCTL_DSP_GETOSPACE, &info);
    if (st < 0)
    {
	warn(w, -1, "SNDCTL_DSP_GETOSPACE failed");
	return -1;
    }
    ret = info.fragments*info.fragsize;
    if (ret != info.bytes)
    {
	warn(w, 11, "GETOSPACE: ret(%d)!=bytes(%d)", ret, info.bytes);
	ret = info.bytes;
    }
    if (ret < 0)
    {
        warn(w, 12, "GETOSPACE: bytes < 0");
	ret = 0;
    }
    if (!uss_8bit)
	ret /= sizeof(SWORD);
    if (ret > uss_bufsize)
    {
	warn(w, 13, "GETOSPACE: bytes > bufsize");
	ret = uss_bufsize;
    }
#if defined(linux)
    /*
     * GETOSPACE before first write returns random value (or actually the
     * value on which the device was when it was closed last time). I hope
     * this has been fixed after 'Sound Driver:3.5-beta2-960210'
     */
    if (first && !ret)
    {
	ret = 1;
	warn(w, -1, "SNDCTL_DSP_GETOSPACE not reliable after open()");
    }
#endif
    return ret;
}

static void uss_close(warn_t *w)
{
    close(uss_fd);
    uss_fd = -1;
    uss_8bit = 0;
    uss_bufsize = 0;
    uss_fragsize = 0;
}

static int uss_suspend(warn_t *w)
{
    int			 st;
    st = ioctl(uss_fd, SNDCTL_DSP_POST, NULL);
    if (st < 0)
    {
	warn(w, -1, "SNDCTL_DSP_POST failed");
	return 1;
    }
    return 0;
}

static sound_device_t uss_device =
{
    "uss",
    uss_init,
    uss_write,
    NULL,
    NULL,
    uss_bufferstatus,
    uss_close,
    uss_suspend,
    NULL
};

int sound_init_uss_device(void)
{
    return sound_register_device(&uss_device);
}
