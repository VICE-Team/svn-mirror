/*
 * vic20sound.c - Implementation of VIC20 sound code.
 *
 * Written by
 *  Rami Räsänen <raipsu@users.sf.net>
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
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "maincpu.h"
#include "sid.h"
#include "sid-resources.h"
#include "sound.h"
#include "types.h"
#include "vic20sound.h"
#include "vic20click.h"
#include "vic20.h"

static BYTE vic20_sound_data[16];

/* dummy function for now */
int machine_sid2_check_range(unsigned int sid2_adr)
{
    return 0;
}

struct sound_vic20_s
{
  unsigned char div;
  unsigned char bassreg;
  unsigned char basscntr;
  unsigned char bassshift;
  unsigned char altoreg;
  unsigned char altocntr;
  unsigned char altoshift;
  unsigned char soprreg;
  unsigned char soprcntr;
  unsigned char soprshift;
  unsigned char noisereg;
  unsigned char noisecntr;
  unsigned short noiseshift;
  unsigned short noisefoo;
  unsigned char vol;
  int cycles_per_sample;
  int speed;
};
typedef struct sound_vic20_s sound_vic20_t;

static struct sound_vic20_s snd;

int vic_sound_run(int cycles);

static int sample_counter = -1;
static int sample_volume = 0;
static int current_volume = -1;

static int vic_sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr,
                                    int interleave, int *delta_t)
{
    int i;

    if (current_volume == -1)
        current_volume = snd.vol;

    if (current_volume != snd.vol)
    {
        sample_volume = snd.vol - current_volume;
        sample_counter = 0;
        current_volume = snd.vol;
    }

    for (i = 0; i < nr; i++)
    {
        SWORD vicbuf;
        int real_sample_counter;

        vicbuf = (((vic_sound_run(snd.cycles_per_sample) * snd.vol) / (snd.cycles_per_sample))<<9) - (snd.vol*0x800);
        pbuf[i * interleave] = sound_audio_mix(pbuf[i * interleave], vicbuf);

        if (vicbuf != 0 && sample_counter != -1)
            sample_counter = -1;

        if (sample_counter != -1)
        {
            real_sample_counter = sample_counter*44100/snd.speed;
            if (real_sample_counter < VIC20CLICK_LEN)
            {
                pbuf[i * interleave] = sound_audio_mix(pbuf[i * interleave], (SWORD)vic20click[real_sample_counter]*sample_volume);
                sample_counter++;
            }
            else
            {
                sample_counter = -1;
            }
        }
    }
    return 0;
}

void vic_sound_reset(void)
{
    WORD i;

    sound_reset();
    for (i = 10; i < 15; i++)
        vic_sound_store(i, 0);
}

void vic_sound_store(WORD addr, BYTE value)
{
    addr &= 0x0f;
    vic20_sound_data[addr] = value;
    sound_store((WORD)(addr+0x20), value, 0);
}

int vic_sound_run(int cycles) {
  int i;
  int ret = cycles*4;  /* Initial offset */

  for (i=0;i<cycles;i++) {
    snd.div++;
    if (!(snd.div&0x3)) {
      snd.soprcntr++; snd.soprcntr &= 0x7F;
      if (snd.soprcntr == 0x7F) {
        unsigned char tmp = snd.soprshift;
        snd.soprcntr = snd.soprreg & ~0x80;
        snd.soprshift <<= 1;
        if (snd.soprreg&0x80) {
          snd.soprshift |= ((tmp^128) & 128) >> 7;
        }
      }
      snd.noisecntr++; snd.noisecntr &= 0x7F;
      if (snd.noisecntr == 0x7F) {
        unsigned short tmp = snd.noiseshift;
        snd.noisecntr = snd.noisereg & ~0x80;
        snd.noiseshift <<= 1;
        if (snd.noisereg&0x80) {
          snd.noiseshift |= ( ( (tmp&0x4000) ^ ((tmp&0x400)<<4) ^ (0x4000) ) >> (6+8) );
          if (!(snd.noiseshift & 1) && (tmp & 1))
            snd.noisefoo++;
        } else
          snd.noisefoo = 4;
      }
    }
    if (!(snd.div&0x7)) {
      snd.altocntr++; snd.basscntr &= 0x7F;
      if (snd.altocntr == 0x7F) {
        unsigned char tmp = snd.altoshift;
        snd.altocntr = snd.altoreg & ~0x80;
        snd.altoshift <<= 1;
        if (snd.altoreg&0x80) {
          snd.altoshift |= ((tmp^128) & 128) >> 7;
        }
      }
    }
    if (!(snd.div&0xF)) {
      snd.basscntr++; snd.basscntr &= 0x7F;
      if (snd.basscntr == 0x7F) {
        unsigned char tmp = snd.bassshift;
        snd.basscntr = snd.bassreg & ~0x80;
        snd.bassshift <<= 1;
        if (snd.bassreg&0x80) {
          snd.bassshift |= ((tmp^128) & 128) >> 7;
        }
      }
    }
    ret += (snd.soprshift & 1) + (snd.altoshift & 1) + (snd.bassshift & 1) + (!(snd.noisefoo & 4));
  }
  return ret;
}

static void vic_sound_machine_store(sound_t *psid, WORD addr, BYTE value)
{
    switch (addr) {
      case 0xA:
        snd.bassreg = value;
        break;
      case 0xB:
        snd.altoreg = value;
        break;
      case 0xC:
        snd.soprreg = value;
        break;
      case 0xD:
        snd.noisereg = value;
        break;
      case 0xE:
        snd.vol = value & 0x0f;
        break;
    }
}

static int vic_sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    DWORD i;

    memset((unsigned char*)&snd, 0, sizeof(snd));

    snd.cycles_per_sample = cycles_per_sec / speed;

    snd.speed = speed;
    sample_counter = -1;

    for (i = 0; i < 16; i++)
        vic_sound_machine_store(psid, (WORD)i, vic20_sound_data[i]);

    return 1;
}

static BYTE vic_sound_machine_read(sound_t *psid, WORD addr)
{
  return 0;
}

sound_t *sound_machine_open(int chipno)
{
    return sid_sound_machine_open(chipno);
}

int sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    vic_sound_machine_init(psid, speed, cycles_per_sec);

    if (!sidcart_clock && cycles_per_sec==VIC20_PAL_CYCLES_PER_SEC)
    {
        return sid_sound_machine_init(psid, (int)(speed*1.125), cycles_per_sec);
    }
    else
    {
        return sid_sound_machine_init(psid, speed, cycles_per_sec);
    }
}

void sound_machine_close(sound_t *psid)
{
    sid_sound_machine_close(psid);
}

/* for read/store 0x00 <= addr <= 0x1f is the sid
 *                0x20 <= addr <= 0x3f is the vic
 *
 * future sound devices will be able to use 0x40 and up
 */

BYTE sound_machine_read(sound_t *psid, WORD addr)
{
    if (addr>=0x20 && addr<=0x3f)
        return vic_sound_machine_read(psid, (WORD)(addr-0x20));
    else
        return sid_sound_machine_read(psid, addr);
}

void sound_machine_store(sound_t *psid, WORD addr, BYTE byte)
{
    if (addr>=0x20 && addr<=0x3f)
        vic_sound_machine_store(psid, (WORD)(addr-0x20), byte);
    else
        sid_sound_machine_store(psid, addr, byte);
}

void sound_machine_reset(sound_t *psid, CLOCK cpu_clk)
{
    sid_sound_machine_reset(psid, cpu_clk);
}

int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr,
                                    int interleave, int *delta_t)
{
    int temp;

    temp=sid_sound_machine_calculate_samples(psid, pbuf, nr, interleave, delta_t);
    vic_sound_machine_calculate_samples(psid, pbuf, nr, interleave, delta_t);
    return temp;
}

void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
    sid_sound_machine_prevent_clk_overflow(psid, sub);
}

char *sound_machine_dump_state(sound_t *psid)
{
    return sid_sound_machine_dump_state(psid);
}

int sound_machine_cycle_based(void)
{
    return 0;
}

int sound_machine_channels(void)
{
    return sid_sound_machine_channels();
}

void sound_machine_enable(int enable)
{
    sid_sound_machine_enable(enable);
}
