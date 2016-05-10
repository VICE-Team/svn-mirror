/*
 * sid-snapshot.c - SID snapshot.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include "log.h"
#include "resources.h"
#include "screenshot.h"
#include "sid-snapshot.h"
#include "sid.h"
#include "sound.h"
#include "snapshot.h"
#include "types.h"

/* TODO:
   - Add snapshot support for fastsid
   - Add snapshot support for cw3
   - Add snapshot support for hardsid
   - Add snapshot support for parsid
 */

static const char snap_module_name_simple1[] = "SID";
static const char snap_module_name_simple2[] = "SID2";
static const char snap_module_name_simple3[] = "SID3";

#define SNAP_MAJOR_SIMPLE 1
#define SNAP_MINOR_SIMPLE 2

static int sid_snapshot_write_module_simple(snapshot_t *s, int sidnr)
{
    int sound, sid_engine, sids;
    snapshot_module_t *m;
    const char *snap_module_name_simple = NULL;
    int sid_address;

    switch (sidnr) {
        default:
        case 0:
            snap_module_name_simple = snap_module_name_simple1;
            break;
        case 1:
            snap_module_name_simple = snap_module_name_simple2;
            break;
        case 2:
            snap_module_name_simple = snap_module_name_simple3;
            break;
    }

    m = snapshot_module_create(s, snap_module_name_simple, SNAP_MAJOR_SIMPLE, SNAP_MINOR_SIMPLE);

    if (m == NULL) {
        return -1;
    }

    resources_get_int("Sound", &sound);
    resources_get_int("SidEngine", &sid_engine);
    resources_get_int("SidStereo", &sids);

    /* Added in 1.2, for the 1st SID module the amount of SIDs is saved 1st */
    if (!sidnr) {
        if (SMW_B(m, (BYTE)sids) < 0) {
            goto fail;
        }
    }

    /* Added in 1.2, for the 2nd SID moduke the address is saved */
    if (sidnr == 1) {
        resources_get_int("SidStereoAddressStart", &sid_address);
        if (SMW_W(m, (WORD)sid_address) < 0) {
            goto fail;
        }
    }

    /* Added in 1.2, for the 3rd SID moduke the address is saved */
    if (sidnr == 2) {
        resources_get_int("SidTripleAddressStart", &sid_address);
        if (SMW_W(m, (WORD)sid_address) < 0) {
            goto fail;
        }
    }

    /* Changed in 1.2, all data is saved whether sound is on or off */
    if (0
        || SMW_B(m, (BYTE)sound) < 0
        || SMW_B(m, (BYTE)sid_engine) < 0
        || SMW_BA(m, sid_get_siddata(sidnr), 32) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

static int sid_snapshot_read_module_simple(snapshot_t *s, int sidnr)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;
    BYTE tmp[34];
    const char *snap_module_name_simple = NULL;
    int sids = 0;
    int sid_address;

    switch (sidnr) {
        default:
        case 0:
            snap_module_name_simple = snap_module_name_simple1;
            break;
        case 1:
            snap_module_name_simple = snap_module_name_simple2;
            break;
        case 2:
            snap_module_name_simple = snap_module_name_simple3;
            break;
    }

    m = snapshot_module_open(s, snap_module_name_simple, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (major_version > SNAP_MAJOR_SIMPLE || minor_version > SNAP_MINOR_SIMPLE) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    /* Handle 1.2 snapshots differently */
    if (SNAPVAL(major_version, minor_version, 1, 2)) {
        if (!sidnr) {
            if (SMR_B_INT(m, &sids) < 0) {
                goto fail;
            }
            resources_set_int("SidStereo", sids);
        } else {
            if (SMR_W_INT(m, &sid_address) < 0) {
                goto fail;
            }
        }
        if (sidnr == 1) {
            resources_set_int("SidStereoAddressStart", sid_address);
        }
        if (sidnr == 2) {
            resources_set_int("SidTripleAddressStart", sid_address);
        }
        if (0
            || SMR_B(m, &tmp[0]) < 0
            || SMR_B(m, &tmp[1]) < 0
            || SMR_BA(m, tmp + 2, 32) < 0) {
            goto fail;
        }
        screenshot_prepare_reopen();
        sound_close();
        screenshot_try_reopen();
        resources_set_int("Sound", (int)tmp[0]);
        resources_set_int("SidEngine", (int)tmp[1]);
        memcpy(sid_get_siddata(sidnr), &tmp[2], 32);
        sound_open();
        return snapshot_module_close(m);
    }

    /* If more than 32 bytes are present then the resource "Sound" and
       "SidEngine" come first! If there is only one byte present, then
       sound is disabled. */
    if (SMR_BA(m, tmp, 34) < 0) {
        if (SMR_BA(m, tmp, 32) < 0) {
            if (SMR_BA(m, tmp, 1) < 0) {
                snapshot_module_close(m);
                goto fail;
            } else {
                sound_close();
            }
        } else {
            memcpy(sid_get_siddata(0), &tmp[0], 32);
        }
    } else {
        int res_sound = (int)(tmp[0]);
        int res_engine = (int)(tmp[1]);

        screenshot_prepare_reopen();
        sound_close();
        screenshot_try_reopen();
        resources_set_int("Sound", res_sound);
        if (res_sound) {
            resources_set_int("SidEngine", res_engine);
            /* FIXME: Only data for first SID read. */
            memcpy(sid_get_siddata(0), &tmp[2], 32);
            sound_open();
        }
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

static const char snap_module_name_extended1[] = "SIDEXTENDED";
static const char snap_module_name_extended2[] = "SIDEXTENDED2";
static const char snap_module_name_extended3[] = "SIDEXTENDED3";
#define SNAP_MAJOR_EXTENDED 1
#define SNAP_MINOR_EXTENDED 1

static int sid_snapshot_write_module_extended(snapshot_t *s, int sidnr)
{
    snapshot_module_t *m;
    sid_snapshot_state_t sid_state;
    int sound, sid_engine;
    const char *snap_module_name_extended = NULL;

    switch (sidnr) {
        default:
        case 0:
            snap_module_name_extended = snap_module_name_extended1;
            break;
        case 1:
            snap_module_name_extended = snap_module_name_extended2;
            break;
        case 2:
            snap_module_name_extended = snap_module_name_extended3;
            break;
    }

    resources_get_int("Sound", &sound);
    resources_get_int("SidEngine", &sid_engine);

    if (sid_engine != SID_ENGINE_FASTSID
#ifdef HAVE_RESID
        && sid_engine != SID_ENGINE_RESID
#endif
        ) {
        return 0;
    }

    sid_state_read(sidnr, &sid_state);

    m = snapshot_module_create(s, snap_module_name_extended, SNAP_MAJOR_EXTENDED, SNAP_MINOR_EXTENDED);

    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_BA(m, sid_state.sid_register, 32) < 0
        || SMW_B(m, sid_state.bus_value) < 0
        || SMW_DW(m, sid_state.bus_value_ttl) < 0
        || SMW_DWA(m, sid_state.accumulator, 3) < 0
        || SMW_DWA(m, sid_state.shift_register, 3) < 0
        || SMW_WA(m, sid_state.rate_counter, 3) < 0
        || SMW_WA(m, sid_state.exponential_counter, 3) < 0
        || SMW_BA(m, sid_state.envelope_counter, 3) < 0
        || SMW_BA(m, sid_state.envelope_state, 3) < 0
        || SMW_BA(m, sid_state.hold_zero, 3) < 0) {
        goto fail;
    }

    if (0
        || SMW_WA(m, sid_state.rate_counter_period, 3) < 0
        || SMW_WA(m, sid_state.exponential_counter_period, 3) < 0) {
        goto fail;
    }

    if (0
        || SMW_BA(m, sid_state.envelope_pipeline, 3) < 0
        || SMW_BA(m, sid_state.shift_pipeline, 3) < 0
        || SMW_DWA(m, sid_state.shift_register_reset, 3) < 0
        || SMW_DWA(m, sid_state.floating_output_ttl, 3) < 0
        || SMW_WA(m, sid_state.pulse_output, 3) < 0
        || SMW_B(m, sid_state.write_pipeline) < 0
        || SMW_B(m, sid_state.write_address) < 0
        || SMW_B(m, sid_state.voice_mask) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

static int sid_snapshot_read_module_extended(snapshot_t *s, int sidnr)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;
    sid_snapshot_state_t sid_state;
    int sid_engine;
    const char *snap_module_name_extended = NULL;

    memset(&sid_state, 0, sizeof(sid_state));

    resources_get_int("SidEngine", &sid_engine);

    if (sid_engine != SID_ENGINE_FASTSID
#ifdef HAVE_RESID
        && sid_engine != SID_ENGINE_RESID
#endif
        ) {
        return 0;
    }

    switch (sidnr) {
        default:
        case 0:
            snap_module_name_extended = snap_module_name_extended1;
            break;
        case 1:
            snap_module_name_extended = snap_module_name_extended2;
            break;
        case 2:
            snap_module_name_extended = snap_module_name_extended3;
            break;
    }

    m = snapshot_module_open(s, snap_module_name_extended, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (major_version > SNAP_MAJOR_EXTENDED || minor_version > SNAP_MINOR_EXTENDED) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (0
        || SMR_BA(m, sid_state.sid_register, 32) < 0
        || SMR_B(m, &(sid_state.bus_value)) < 0
        || SMR_DW(m, &(sid_state.bus_value_ttl)) < 0
        || SMR_DWA(m, sid_state.accumulator, 3) < 0
        || SMR_DWA(m, sid_state.shift_register, 3) < 0
        || SMR_WA(m, sid_state.rate_counter, 3) < 0
        || SMR_WA(m, sid_state.exponential_counter, 3) < 0
        || SMR_BA(m, sid_state.envelope_counter, 3) < 0
        || SMR_BA(m, sid_state.envelope_state, 3) < 0
        || SMR_BA(m, sid_state.hold_zero, 3) < 0) {
        goto fail;
    }

    if (0
        || SMR_WA(m, sid_state.rate_counter_period, 3) < 0
        || SMR_WA(m, sid_state.exponential_counter_period, 3) < 0) {
        goto fail;
    }

    if (0
        || SMR_BA(m, sid_state.envelope_pipeline, 3) < 0
        || SMR_BA(m, sid_state.shift_pipeline, 3) < 0
        || SMR_DWA(m, sid_state.shift_register_reset, 3) < 0
        || SMR_DWA(m, sid_state.floating_output_ttl, 3) < 0
        || SMR_WA(m, sid_state.pulse_output, 3) < 0
        || SMR_B(m, &(sid_state.write_pipeline)) < 0
        || SMR_B(m, &(sid_state.write_address)) < 0
        || SMR_B(m, &(sid_state.voice_mask)) < 0) {
        goto fail;
    }

    sid_state_write(0, &sid_state);

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

int sid_snapshot_write_module(snapshot_t *s)
{
    int sids = 0;
    int i;

    resources_get_int("SidStereo", &sids);

    ++sids;

    for (i = 0; i < sids; ++i) {
        if (sid_snapshot_write_module_simple(s, i) < 0) {
           return -1;
        }

        if (sid_snapshot_write_module_extended(s, i) < 0) {
            return -1;
        }
    }

    return 0;
}

int sid_snapshot_read_module(snapshot_t *s)
{
    int sids = 0;
    int i;

    if (sid_snapshot_read_module_simple(s, 0) < 0) {
        return -1;
    }

    if (sid_snapshot_read_module_extended(s, 0) < 0) {
        return -1;
    }

    resources_get_int("SidStereo", &sids);
    ++sids;

    for (i = 1; i < sids; ++i) {
        if (0
            || sid_snapshot_read_module_simple(s, i) < 0
            || sid_snapshot_read_module_extended(s, i) < 0) {
            return -1;
        }
    }
    return 0;
}
