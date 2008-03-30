/*
 * resid.cc - reSID interface code.
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

#include "resid/sid.h"

extern "C" {

#include <string.h>
#include "log.h"
#include "utils.h"
#include "sound.h"
#include "fixpoint.h"

struct sound_s
{
    /* resid sid implementation */
    SID			sid;
};

sound_t *resid_sound_machine_open(int speed, int cycles_per_sec,
				  int filters_enabled, int model,
				  int sampling, int passband_percentage,
				  BYTE *sidstate)
{
    sound_t			*psid;
    int				 i;
    sampling_method method;
    char method_text[100];
    double passband = speed*passband_percentage/200.0;

    psid = new sound_t;

    psid->sid.set_chip_model(model == 0 ? MOS6581 : MOS8580);
    psid->sid.enable_filter(filters_enabled);
    psid->sid.enable_external_filter(filters_enabled);

    switch (sampling) {
    default:
    case 0:
        method = SAMPLE_FAST;
	strcpy(method_text, "fast");
	break;
    case 1:
        method = SAMPLE_INTERPOLATE;
	strcpy(method_text, "interpolating");
	break;
    case 2:
        method = SAMPLE_RESAMPLE;
	sprintf(method_text, "resampling, pass to %dHz", (int)passband);
	break;
    }

    if (!psid->sid.set_sampling_parameters(cycles_per_sec, method,
					   speed, passband))
    {
        log_warning(LOG_DEFAULT, "reSID: Out of spec, increase sampling rate or decrease maximum speed");
	delete psid;
	return NULL;
    }

    log_message(LOG_DEFAULT, "reSID: %s, filter %s, sampling rate %dHz - %s",
		model == 0 ? "MOS6581" : "MOS8580",
		filters_enabled ? "on" : "off",
		speed, method_text);

    for (i = 0x00; i <= 0x18; i++) {
	psid->sid.write(i, sidstate[i]);
    }

    return psid;
}

void resid_sound_machine_close(sound_t *psid)
{
    delete psid;
}

BYTE resid_sound_machine_read(sound_t *psid, ADDRESS addr)
{
    return psid->sid.read(addr);
}

void resid_sound_machine_store(sound_t *psid, ADDRESS addr, BYTE byte)
{
    psid->sid.write(addr, byte);
}

int resid_sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr,
					  int *delta_t)
{
    return psid->sid.clock(*delta_t, pbuf, nr);
}

void resid_sound_machine_init(void)
{
}

void resid_sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
}

void resid_sound_machine_reset(sound_t *psid)
{
}

char *resid_sound_machine_dump_state(sound_t *psid)
{
    return stralloc("");
}

} // extern "C"
