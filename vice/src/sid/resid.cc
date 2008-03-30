/*
 * resid.cc - reSID interface code
 *
 * Written by
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#include <stdio.h>
#include <math.h>

extern "C" {

#include "vice.h"
#include "sid.h"
#include "warn.h"
#include "utils.h"

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
};

/* warnings */
static warn_t *pwarn;

sound_t *resid_sound_machine_open(int speed, int cycles_per_sec,
				  int filters_enabled, BYTE *siddata,
				  int model, CLOCK clk)
{
    sound_t			*psid;
    int				 i;

    psid = new sound_t;
    psid->sid.enable_filter(filters_enabled);
    psid->sid.enable_external_filter(filters_enabled);
    psid->clk = 0.0;
    psid->sidclk = 0;
    psid->clkstep = (double)cycles_per_sec / speed;
    if (model == 0)
    {
	psid->sid.set_chip_model(MOS6581);
	warn(pwarn, -1, "using reSID MOS6581 emulation");
    }
    else
    {
	psid->sid.set_chip_model(MOS8580);
	warn(pwarn, -1, "using reSID MOS8580 emulation");
    }
    for (i = 0x00; i <= 0x18; i++) {
	psid->sid.write(i, siddata[i]);
    }
    return psid;
}

void resid_sound_machine_close(sound_t *psid)
{
    delete psid;
}

BYTE resid_sound_machine_read(sound_t *psid, ADDRESS addr, CLOCK clk)
{
    int					delta;

    delta = (int) BIG_FLOAT_TO_INT(psid->clk + sound_sample_position()*psid->clkstep
                                 - psid->sidclk);
    if (delta > 0)
    {
	psid->sid.clock(delta);
	psid->sidclk += delta;
    }

    return psid->sid.read(addr);
}

void resid_sound_machine_store(sound_t *psid, ADDRESS addr, BYTE byte,
			       CLOCK clk)
{
    int					delta;

    delta = (int) BIG_FLOAT_TO_INT(psid->clk + sound_sample_position()*psid->clkstep
                                 - psid->sidclk);
    if (delta > 0)
    {
	psid->sid.clock(delta);
	psid->sidclk += delta;
    }

    psid->sid.write(addr, byte);
}

int resid_sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr)
{
    int					i, delta;
    psid->clk += psid->clkstep;
    delta = (int) (psid->clk - psid->sidclk);
    if (delta > 0)
    {
	psid->sid.clock(delta);
	psid->sidclk += delta;
    }
    pbuf[0] = psid->sid.output();
    for (i = 1; i < nr; i++)
    {
	psid->clk += psid->clkstep;
	delta = (int) (psid->clk - psid->sidclk);
	psid->sid.clock(delta);
	psid->sidclk += delta;
	pbuf[i] = psid->sid.output();
    }
    if (psid->sidclk > 0x70000000)
    {
	psid->sidclk -= 0x70000000 - 0x1000;
	psid->clk -= 0x70000000 - 0x1000;
    }

    return 0;
}

void resid_sound_machine_init(void)
{
    pwarn = warn_init("RESID", 128);
}

void resid_sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
}

void resid_sound_machine_reset(sound_t *psid, CLOCK clk)
{
}

char *resid_sound_machine_dump_state(sound_t *psid)
{
    return stralloc("");
}

} // extern "C"
