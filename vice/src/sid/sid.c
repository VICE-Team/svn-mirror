/*
 * sid.c - MOS6581 (SID) emulation, hooks to actual implementation.
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

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "fastsid.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "sid-resources.h"
#include "sid.h"
#include "types.h"

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
BYTE siddata[SOUND_CHANNELS_MAX][32];

BYTE REGPARM2 sid_read_chip(ADDRESS addr, int chipno)
{
    int	val;
    addr &= 0x1f;
    machine_handle_pending_alarms(0);
#ifdef HAVE_MOUSE
    if (addr == 0x19 && _mouse_enabled)
        val = mouse_get_x();
    else if (addr == 0x1a && _mouse_enabled)
        val = mouse_get_y();
    else
#endif
    val = sound_read(addr, chipno);

    /* Fallback when sound is switched off. */
    if (val < 0) {
        if (addr == 0x19 || addr == 0x1a)
	    val = 0xff;
	else {
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
void REGPARM3 sid_store_chip(ADDRESS addr, BYTE byte, int chipno)
{
    addr &= 0x1f;
    siddata[chipno][addr] = byte;

    machine_handle_pending_alarms(maincpu_rmw_flag + 1);
    if (maincpu_rmw_flag) {
	maincpu_clk--;
	sound_store(addr, lastsidread, chipno);
	maincpu_clk++;
    }
    sound_store(addr, byte, chipno);
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 sid_read(ADDRESS addr)
{
    if (sid_stereo
        && addr >= sid_stereo_address_start
        && addr < sid_stereo_address_end)
        sid_read_chip(addr, 1);

    return sid_read_chip(addr, 0);
}

BYTE REGPARM1 sid2_read(ADDRESS addr)
{
    return sid_read_chip(addr, 1);
}

void REGPARM2 sid_store(ADDRESS addr, BYTE byte)
{
    if (sid_stereo
        && addr >= sid_stereo_address_start
        && addr < sid_stereo_address_end) {
        sid_store_chip(addr, byte, 1);
        return;
    }

    sid_store_chip(addr, byte, 0);
}

void REGPARM2 sid2_store(ADDRESS addr, BYTE byte)
{
    sid_store_chip(addr, byte, 1);
}

/* ------------------------------------------------------------------------- */

void sid_reset(void)
{
    sound_reset();

    memset(siddata, 0, sizeof(siddata));
}


static int useresid;

sound_t *sound_machine_open(int chipno)
{
#ifdef HAVE_RESID
    useresid = 0;
    if (resources_get_value("SidUseResid", (resource_value_t *)&useresid) < 0)
        return NULL;

    if (useresid)
	sid_engine = resid_hooks;
    else
#endif
        sid_engine = fastsid_hooks;
    return sid_engine.open(siddata[chipno]);
}

int sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    return sid_engine.init(psid, speed, cycles_per_sec);
}

void sound_machine_close(sound_t *psid)
{
    sid_engine.close(psid);
}

BYTE sound_machine_read(sound_t *psid, ADDRESS addr)
{
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
				    int interleave, int *delta_t)
{
    return sid_engine.calculate_samples(psid, pbuf, nr, interleave, delta_t);
}

void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
    sid_engine.prevent_clk_overflow(psid, sub);
}

char *sound_machine_dump_state(sound_t *psid)
{
    return sid_engine.dump_state(psid);
}

int sound_machine_cycle_based(void)
{
    return useresid;
}

int sound_machine_channels(void)
{
    int stereo = 0;
    resources_get_value("SidStereo", (resource_value_t*)&stereo);
    return stereo ? 2 : 1;
}

