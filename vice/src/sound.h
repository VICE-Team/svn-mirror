/*
 * sound.h - General code for the sound interface
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

#ifndef _SOUND_H
#define _SOUND_H

#include "types.h"
#include "warn.h"

/* device structure */
typedef struct
{
    /* name of the device */
    char			 *name;
    /* init -routine to be called at device initialization. Should use
       suggested values if possible or return new values if they cannot be
       used */
    int				(*init)(warn_t *w, char *device, int *speed,
					int *fragsize, int *fragnr,
					double bufsize);
    /* send number of bytes to the soundcard. it is assumed to block if kernel
       buffer is full */
    int				(*write)(warn_t *w, SWORD *pbuf, int nr);
    /* dump-routine to be called for every write to SID */
    int				(*dump)(warn_t *w, ADDRESS addr, BYTE byte,
					CLOCK clks);
    /* flush-routine to be called every frame */
    int				(*flush)(warn_t *s, char *state);
    /* return number of samples unplayed in the kernel buffer at the moment */
    int				(*bufferstatus)(warn_t *s, int first);
    /* close and cleanup device */
    void			(*close)(warn_t *w);
    /* suspend device */
    int				(*suspend)(warn_t *s);
    /* resume device */
    int				(*resume)(warn_t *s);
} sound_device_t;

/* Sound adjustment types.  */
#define SOUND_ADJUST_FLEXIBLE   0
#define SOUND_ADJUST_ADJUSTING  1
#define SOUND_ADJUST_EXACT      2

/* external functions for vice */
extern void sound_init(unsigned int clock_rate, unsigned int ticks_per_frame);
extern int  sound_flush(int relative_speed);
extern void sound_suspend(void);
extern void sound_resume(void);
extern void sound_close(void);
extern void sound_set_warp_mode(int value);

extern int  sound_init_resources(void);
extern int  sound_init_cmdline_options(void);

extern void sound_prevent_clk_overflow(CLOCK sub);


/* device initialization prototypes */
extern int  sound_init_aix_device(void);
extern int  sound_init_allegro_device(void);
extern int  sound_init_sb_device(void);
extern int  sound_init_dummy_device(void);
extern int  sound_init_dump_device(void);
extern int  sound_init_fs_device(void);
extern int  sound_init_hpux_device(void);
extern int  sound_init_midas_device(void);
extern int  sound_init_sdl_device(void);
extern int  sound_init_sgi_device(void);
extern int  sound_init_speed_device(void);
extern int  sound_init_sun_device(void);
extern int  sound_init_test_device(void);	/* XXX: missing */
extern int  sound_init_uss_device(void);


/* internal function for sound device registration */
extern int  sound_register_device(sound_device_t *pdevice);

/* other internal functions used around sound -code */
extern int  sound_read(ADDRESS addr);
extern void sound_store(ADDRESS addr, BYTE val);
extern double sound_sample_position(void);

/* functions and structs implemented by each machine */
extern void sound_machine_init(void);
typedef struct sound_s sound_t;
extern sound_t *sound_machine_open(int speed, int cycles_per_sec);
extern void sound_machine_close(sound_t *psid);
extern int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr);
extern void sound_machine_store(sound_t *psid, ADDRESS addr, BYTE val);
extern BYTE sound_machine_read(sound_t *psid, ADDRESS addr);
extern char *sound_machine_dump_state(sound_t *psid);
extern void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub);

#endif /* !defined (_SOUND_H) */
