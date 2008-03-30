/*
 * sid-engine.c - MOS6581 (SID) emulation, hooks to actual implementation.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Michael Schwendt <sidplay@geocities.com>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include <string.h>

#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "sid-engine.h"
#include "sid.h"

#ifdef HAVE_MOUSE
#include "mouse.h"
#endif

#ifdef HAVE_RESID
#include "resid.h"
#endif

/* SID engine hooks. */
static sid_engine_t sid_engine;

/* read register value from sid */
static BYTE lastsidread;
/* register data */
BYTE siddata[32];

BYTE REGPARM1 sid_read(ADDRESS addr)
{
    int	val;
    machine_handle_pending_alarms(0);
    addr = addr & 0x1f;
#ifdef HAVE_MOUSE
    if (addr == 0x19 && _mouse_enabled)
        val = mouse_get_x();
    else if (addr == 0x1a && _mouse_enabled)
        val = mouse_get_y();
    else
#endif
    val = sound_read(addr);

    /* Fallback when sound is switched off. */
    if (val < 0)
    {
        if (addr == 0x19 || addr == 0x1a)
	    val = 0xff;
	else
	{
	    if (addr == 0x1b || addr == 0x1c)
		val = rand();
	    else
		val = 0;
	}
    }

    lastsidread = val;
    return val;
}

/* write register value to sid */
void REGPARM2 sid_store(ADDRESS addr, BYTE byte)
{
    addr &= 0x1f;
    siddata[addr] = byte;

    machine_handle_pending_alarms(rmw_flag + 1);
    if (rmw_flag)
    {
	clk--;
	sound_store(addr, lastsidread);
	clk++;
    }
    sound_store(addr, byte);
}

void sid_reset(void)
{
    ADDRESS i;

    sound_reset();

    memset(siddata, 0, 32);
    for (i = 0; i < 32; i++)
        sound_store(i, 0);
}



void sound_machine_init(void)
{
}

sound_t *sound_machine_open(int speed, int cycles_per_sec)
{
#ifdef HAVE_RESID
    int useresid;
    if (resources_get_value("SidUseResid", (resource_value_t *)&useresid) < 0)
        return NULL;

    if (useresid)
	sid_engine = resid_hooks;
    else
#endif
        sid_engine = fastsid_hooks;
    return sid_engine.open(speed, cycles_per_sec, siddata);
}

void sound_machine_close(sound_t *psid)
{
    sid_engine.close(psid);
}

BYTE sound_machine_read(sound_t *psid, ADDRESS addr)
{
#ifdef HAVE_MOUSE
    if (addr == 0x19 && _mouse_enabled)
        return mouse_get_x();
    else if (addr == 0x1a && _mouse_enabled)
        return mouse_get_y();
    else
#endif
	return sid_engine.read(psid, addr);
}

void sound_machine_store(sound_t *psid, ADDRESS addr, BYTE byte)
{
    sid_engine.store(psid, addr, byte);
}

void sound_machine_reset(sound_t *psid, CLOCK cpu_clk)
{
    sid_engine.reset(psid, cpu_clk);
}

int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr,
				    int *delta_t)
{
    return sid_engine.calculate_samples(psid, pbuf, nr, delta_t);
}

void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
    sid_engine.prevent_clk_overflow(psid, sub);
}

char *sound_machine_dump_state(sound_t *psid)
{
    return sid_engine.dump_state(psid);
}
