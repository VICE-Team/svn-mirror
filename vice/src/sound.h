/*
 * sound.h - General code for the sound interface
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

#ifndef _SOUND_H
#define _SOUND_H

#include "vice.h"

#include <stdio.h>

#include "types.h"


/* Sound defaults.  */
#define SOUND_SAMPLE_RATE 22050
#define SOUND_CHANNELS_MAX 2
#define SOUND_BUFSIZE 32768

#ifdef __MSDOS__
# define SOUND_SAMPLE_BUFFER_SIZE       100     /* ms */
#endif
#ifdef __OS2__
# define SOUND_SAMPLE_BUFFER_SIZE       400
#endif
#ifndef SOUND_SAMPLE_BUFFER_SIZE
# define SOUND_SAMPLE_BUFFER_SIZE       350
#endif

/* I need this to serialize close_sound and enablesound/sound_open in
   the OS/2 Multithreaded environment                              */
extern int sound_state_changed;
extern int sid_state_changed;

/* device structure */
typedef struct sound_device_s
{
    /* name of the device */
    const char *name;
    /* init -routine to be called at device initialization. Should use
       suggested values if possible or return new values if they cannot be
       used */
    int (*init)(const char *param,
        int *speed,
        int *fragsize, int *fragnr,
        int *channels);
    /* send number of bytes to the soundcard. it is assumed to block if kernel
       buffer is full */
    int (*write)(SWORD *pbuf, size_t nr);
    /* dump-routine to be called for every write to SID */
    int (*dump)(WORD addr, BYTE byte, CLOCK clks);
    /* flush-routine to be called every frame */
    int (*flush)(char *state);
    /* return number of samples currently available in the kernel buffer */
    int (*bufferspace)(void);
    /* close and cleanup device */
    void (*close)(void);
    /* suspend device */
    int (*suspend)(void);
    /* resume device */
    int (*resume)(void);
    /* is attenuation needed on suspend or not */
    int need_attenuation;
} sound_device_t;

static inline SWORD sound_audio_mix(int ch1, int ch2)
{
  if (ch1 == 0)
    return ch2;

  if (ch2 == 0)
    return ch1;

  if ((ch1 > 0 && ch2 < 0) || (ch1 < 0 && ch2 >0))
    return ch1+ch2;

  if (ch1 > 0)
    return (SWORD)((SDWORD)(ch1 + ch2) - (SDWORD)(ch1 * ch2 / 32768));

  return (SWORD)-((SDWORD)(-(ch1) + -(ch2)) - (SDWORD)(-(ch1) * -(ch2) / 32768));
}

/* Sound adjustment types.  */
#define SOUND_ADJUST_FLEXIBLE   0
#define SOUND_ADJUST_ADJUSTING  1
#define SOUND_ADJUST_EXACT      2

/* external functions for vice */
extern void sound_init(unsigned int clock_rate, unsigned int ticks_per_frame);
extern void sound_reset(void);
#if defined(__MSDOS__) || defined(__riscos)
extern int sound_flush(int relative_speed);
#else
extern double sound_flush(int relative_speed);
#endif
extern void sound_suspend(void);
extern void sound_resume(void);
extern int sound_open(void);
extern void sound_close(void);
extern void sound_set_relative_speed(int value);
extern void sound_set_warp_mode(int value);
extern void sound_set_machine_parameter(long clock_rate, long ticks_per_frame);
extern void sound_snapshot_prepare(void);
extern void sound_snapshot_finish(void);

extern int sound_resources_init(void);
extern void sound_resources_shutdown(void);
extern int sound_cmdline_options_init(void);


/* device initialization prototypes */
extern int sound_init_aix_device(void);
extern int sound_init_allegro_device(void);
extern int sound_init_alsa_device(void);
extern int sound_init_sb_device(void);
extern int sound_init_dummy_device(void);
extern int sound_init_dump_device(void);
extern int sound_init_fs_device(void);
extern int sound_init_wav_device(void);
extern int sound_init_hpux_device(void);
extern int sound_init_midas_device(void);
extern int sound_init_sdl_device(void);
extern int sound_init_sgi_device(void);
extern int sound_init_speed_device(void);
extern int sound_init_sun_device(void);
extern int sound_init_test_device(void); /* XXX: missing */
extern int sound_init_uss_device(void);
extern int sound_init_dx_device(void);
extern int sound_init_ce_device(void);
extern int sound_init_vidc_device(void);
extern int sound_init_mmos2_device(void);
extern int sound_init_dart_device(void);
extern int sound_init_dart2_device(void);
extern int sound_init_beos_device(void);
extern int sound_init_arts_device(void);
extern int sound_init_esd_device(void);
extern int sound_init_wmm_device(void);
extern int sound_init_ffmpegaudio_device(void);
extern int sound_init_coreaudio_device(void);
extern int sound_init_ahi_device(void);
extern int sound_init_voc_device(void);
extern int sound_init_iff_device(void);
extern int sound_init_aiff_device(void);
extern int sound_init_mp3_device(void);

/* internal function for sound device registration */
extern int sound_register_device(sound_device_t *pdevice);

/* other internal functions used around sound -code */
extern int sound_read(WORD addr, int chipno);
extern void sound_store(WORD addr, BYTE val, int chipno);
extern long sound_sample_position(void);

/* functions and structs implemented by each machine */
typedef struct sound_s sound_t;
extern sound_t *sound_machine_open(int chipno);
extern int sound_machine_init(sound_t *psid, int speed, int cycles_per_sec);
extern void sound_machine_close(sound_t *psid);
extern int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr,
					   int interleave, int *delta_t);
extern void sound_machine_store(sound_t *psid, WORD addr, BYTE val);
extern BYTE sound_machine_read(sound_t *psid, WORD addr);
extern char *sound_machine_dump_state(sound_t *psid);
extern void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub);
extern void sound_machine_reset(sound_t *psid, CLOCK cpu_clk);
extern int sound_machine_cycle_based(void);
extern int sound_machine_channels(void);
extern void sound_machine_enable(int enable);

extern unsigned int sound_device_num(void);
extern const char *sound_device_name(unsigned int num);

#ifdef __riscos
extern int SoundPollEvery;
extern int SoundMachineReady;
extern int SoundThreadActive;
extern void sound_poll(void);
extern void sound_synthesize(SWORD *buffer, int length);
#endif

extern sound_t *sound_get_psid(unsigned int channel);

#endif

