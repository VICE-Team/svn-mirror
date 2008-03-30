/*
 * resid.cc - reSID interface code
 *
 * Written by
 *  Teemu Rantanen (tvr@cs.hut.fi)
 *
 * Original reSID integration for VICE 0.14.2 by
 *  Dag Lem (resid@nimrod.no)
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

#include "resid/sid.h"

extern "C" {

#include <stdio.h>
#include <math.h>

#include "vice.h"
#include "sid.h"
#include "warn.h"

/* clockcycles for each dropping bit when write-only register read is done */
static DWORD sidreadclocks[9];

struct sound_s
{
    /* resid sid implementation */
    SID			sid;
    /* time of last sid.clock() */
    CLOCK		sidclk;
    /* clock */
    double		clk;
    /* clock step / sample */
    double		clkstep;
    /* constants needed to implement write-only register reads */
    BYTE		 laststore;
    BYTE		 laststorebit;
    CLOCK		 laststoreclk;
};

/* warnings */
static warn_t *pwarn;

sound_t *resid_sound_machine_open(int speed, int cycles_per_sec,
				  int filters_enabled, BYTE *siddata,
				  CLOCK clk)
{
    sound_t			*psid;
    int				 i;

    psid = new sound_t;
    psid->sid.filter.bypass = !filters_enabled;
    psid->clk = 0.0;
    psid->sidclk = 0;
    psid->clkstep = (double)cycles_per_sec / speed;
    psid->laststore = 0;
    psid->laststorebit = 0;
    psid->laststoreclk = clk;
    psid->sid.voice1.wave.writeFREQ_LO(siddata[0x00]);
    psid->sid.voice1.wave.writeFREQ_HI(siddata[0x01]);
    psid->sid.voice1.wave.writePW_LO(siddata[0x02]);
    psid->sid.voice1.wave.writePW_HI(siddata[0x03]);
    psid->sid.voice1.writeCONTROL_REG(siddata[0x04]);
    psid->sid.voice1.envelope.writeATTACK_DECAY(siddata[0x05]);
    psid->sid.voice1.envelope.writeSUSTAIN_RELEASE(siddata[0x06]);
    psid->sid.voice2.wave.writeFREQ_LO(siddata[0x07]);
    psid->sid.voice2.wave.writeFREQ_HI(siddata[0x08]);
    psid->sid.voice2.wave.writePW_LO(siddata[0x09]);
    psid->sid.voice2.wave.writePW_HI(siddata[0x0a]);
    psid->sid.voice2.writeCONTROL_REG(siddata[0x0b]);
    psid->sid.voice2.envelope.writeATTACK_DECAY(siddata[0x0c]);
    psid->sid.voice2.envelope.writeSUSTAIN_RELEASE(siddata[0x0d]);
    psid->sid.voice3.wave.writeFREQ_LO(siddata[0x0e]);
    psid->sid.voice3.wave.writeFREQ_HI(siddata[0x0f]);
    psid->sid.voice3.wave.writePW_LO(siddata[0x10]);
    psid->sid.voice3.wave.writePW_HI(siddata[0x11]);
    psid->sid.voice3.writeCONTROL_REG(siddata[0x12]);
    psid->sid.voice3.envelope.writeATTACK_DECAY(siddata[0x13]);
    psid->sid.voice3.envelope.writeSUSTAIN_RELEASE(siddata[0x14]);
    psid->sid.filter.writeFC_LO(siddata[0x15]);
    psid->sid.filter.writeFC_HI(siddata[0x16]);
    psid->sid.filter.writeRES_FILT(siddata[0x17]);
    psid->sid.filter.writeMODE_VOL(siddata[0x18]);
    for (i = 0; i < 9; i++)
	sidreadclocks[i] = 13;
    warn(pwarn, -1, "using reSID MOS6581 emulation");
    return psid;
}

void resid_sound_machine_close(sound_t *psid)
{
    delete psid;
}

BYTE resid_sound_machine_read(sound_t *psid, ADDRESS addr, CLOCK clk)
{
    CLOCK				tmp;
    int					delta;

    delta = psid->clk + sound_sample_position()*psid->clkstep - psid->sidclk;
    if (delta > 0)
    {
	psid->sid.clock(delta);
	psid->sidclk += delta;
    }

    switch (addr)
    {
    case 0x19:
	return psid->sid.potx.readPOT();
    case 0x1a:
	return psid->sid.poty.readPOT();
    case 0x1b:
	return psid->sid.voice3.wave.readOSC();
    case 0x1c:
	return psid->sid.voice3.envelope.readENV();
    }
    while ((tmp = psid->laststorebit) &&
	   (tmp = psid->laststoreclk + sidreadclocks[tmp]) < clk)
    {
	psid->laststoreclk = tmp;
	psid->laststore &= 0xfeff >> psid->laststorebit--;
    }
    return psid->laststore;
}

void resid_sound_machine_store(sound_t *psid, ADDRESS addr, BYTE byte,
			       CLOCK clk)
{
    int					delta;

    delta = psid->clk + sound_sample_position()*psid->clkstep - psid->sidclk;
    if (delta > 0)
    {
	psid->sid.clock(delta);
	psid->sidclk += delta;
    }

    switch (addr)
    {
    case 0x00:
	psid->sid.voice1.wave.writeFREQ_LO(byte);
	break;
    case 0x01:
	psid->sid.voice1.wave.writeFREQ_HI(byte);
	break;
    case 0x02:
	psid->sid.voice1.wave.writePW_LO(byte);
	break;
    case 0x03:
	psid->sid.voice1.wave.writePW_HI(byte);
	break;
    case 0x04:
	psid->sid.voice1.writeCONTROL_REG(byte);
	break;
    case 0x05:
	psid->sid.voice1.envelope.writeATTACK_DECAY(byte);
	break;
    case 0x06:
	psid->sid.voice1.envelope.writeSUSTAIN_RELEASE(byte);
	break;
    case 0x07:
	psid->sid.voice2.wave.writeFREQ_LO(byte);
	break;
    case 0x08:
	psid->sid.voice2.wave.writeFREQ_HI(byte);
	break;
    case 0x09:
	psid->sid.voice2.wave.writePW_LO(byte);
	break;
    case 0x0a:
	psid->sid.voice2.wave.writePW_HI(byte);
	break;
    case 0x0b:
	psid->sid.voice2.writeCONTROL_REG(byte);
	break;
    case 0x0c:
	psid->sid.voice2.envelope.writeATTACK_DECAY(byte);
	break;
    case 0x0d:
	psid->sid.voice2.envelope.writeSUSTAIN_RELEASE(byte);
	break;
    case 0x0e:
	psid->sid.voice3.wave.writeFREQ_LO(byte);
	break;
    case 0x0f:
	psid->sid.voice3.wave.writeFREQ_HI(byte);
	break;
    case 0x10:
	psid->sid.voice3.wave.writePW_LO(byte);
	break;
    case 0x11:
	psid->sid.voice3.wave.writePW_HI(byte);
	break;
    case 0x12:
	psid->sid.voice3.writeCONTROL_REG(byte);
	break;
    case 0x13:
	psid->sid.voice3.envelope.writeATTACK_DECAY(byte);
	break;
    case 0x14:
	psid->sid.voice3.envelope.writeSUSTAIN_RELEASE(byte);
	break;
    case 0x15:
	psid->sid.filter.writeFC_LO(byte);
	break;
    case 0x16:
	psid->sid.filter.writeFC_HI(byte);
	break;
    case 0x17:
	psid->sid.filter.writeRES_FILT(byte);
	break;
    case 0x18:
	psid->sid.filter.writeMODE_VOL(byte);
	break;
    }
    psid->laststore = byte;
    psid->laststorebit = 8;
    psid->laststoreclk = clk;
}

int resid_sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr)
{
    int					i, delta;
    psid->clk += psid->clkstep;
    delta = psid->clk - psid->sidclk;
    if (delta > 0)
    {
	psid->sid.clock(delta);
	psid->sidclk += delta;
    }
    pbuf[0] = psid->sid.output();
    for (i = 1; i < nr; i++)
    {
	psid->clk += psid->clkstep;
	delta = psid->clk - psid->sidclk;
	psid->sid.clock(delta);
	psid->sidclk += delta;
	pbuf[i] = psid->sid.output();
    }
}

void resid_sound_machine_init(void)
{
    pwarn = warn_init("RESID", 128);
}

void resid_sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
    psid->laststoreclk -= sub;
}

} // extern "C"
