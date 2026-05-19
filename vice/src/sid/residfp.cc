/*
 * residfp.cc - reSIDfp interface code.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Dag Lem <resid@nimrod.no>
 *  Andreas Boose <viceteam@t-online.de>
 *  groepaz <groepaz@gmx.net>
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

#ifdef _M_ARM
#undef _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE
#define _ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE 1
#endif

extern "C" {

#include <string.h>

#include "sid/sid.h" /* sid_engine_t */
#include "lib.h"
#include "log.h"
#include "residfp.h"
#include "resources.h"
#include "sid-snapshot.h"
#include "types.h"

extern log_t sound_log;

} // extern "C"

#include "src/SID.h"

using namespace reSIDfp;

extern "C" {

struct sound_s
{
    /* speed factor */
    int factor;

    /* libresidfp does not have a public interface to the internal state of the
     * emulated SID, so we keep a mirror of the written register values here */
    int sid_register[0x20];

    /* resid sid implementation */
    reSIDfp::SID *sid;
};

typedef struct sound_s sound_t;

/* manage temporary buffers. if the requested size is smaller or equal to the
 * size of the already allocated buffer, reuse it.  */
static short *buf = NULL;

static int blen = 0;

static short *getbuf(int len)
{
    if ((buf == NULL) || (blen < len)) {
        if (buf) {
            lib_free(buf);
        }
        blen = len;
        buf = (short *)lib_calloc(len, 1);
    }
    return buf;
}

static sound_t *residfp_open(uint8_t *sidstate)
{
    sound_t *psid;
    int i;

    psid = new sound_t;
    psid->sid = new reSIDfp::SID;

    for (i = 0x00; i <= 0x18; i++) {
        psid->sid->write(i, sidstate[i]);
    }

    return psid;
}

static int residfp_init(sound_t *psid, int speed, int cycles_per_sec, int factor)
{
    SamplingMethod method;
    char model_text[100];
    char method_text[100];
    double curve, range = 0.5f;

    int filters_enabled, model, sampling;
    int curve_6581_int = RESIDFP_6581_FILTER_CURVE_DEFAULT;
    int range_6581_int = RESIDFP_6581_FILTER_RANGE_DEFAULT;
    int curve_8580_int = RESIDFP_8580_FILTER_CURVE_DEFAULT;
    int combined_strength_int = RESIDFP_COMBINED_WAVEFORM_STRENGTH_DEFAULT;

    CombinedWaveforms combined_table[3] = { WEAK, AVERAGE, STRONG };

    if (resources_get_int("SidFilters", &filters_enabled) < 0) {
        return 0;
    }

    if (resources_get_int("SidModel", &model) < 0) {
        return 0;
    }
    /*printf("residfp_init SidFilters:%d SidModel:%d\n", filters_enabled, model);*/

    /*
     * Don't even think about changing this to fast during warp :)
     * the resampled result is visible to the emulator.
     */
    if (resources_get_int("SidResidSampling", &sampling) < 0) {
        return 0;
    }

    if (resources_get_int("SidResidCombinedWaveformStrength", &combined_strength_int) < 0) {
        return 0;
    }

    if ((model == 1) || (model == 2)) {
        /* 8580 */
        if (resources_get_int("SidResid8580FilterCurve", &curve_8580_int) < 0) {
            return 0;
        }
        curve = ((double)curve_8580_int) / RESIDFP_8580_FILTER_CURVE_ONE;
    } else {
        /* 6581 */
        if (resources_get_int("SidResid6581FilterCurve", &curve_6581_int) < 0) {
            return 0;
        }

        if (resources_get_int("SidResid6581FilterRange", &range_6581_int) < 0) {
            return 0;
        }
        curve = ((double)curve_6581_int) / RESIDFP_6581_FILTER_CURVE_ONE;
        range = ((double)range_6581_int) / RESIDFP_6581_FILTER_RANGE_ONE;
    }
    /*printf("residfp_init range int %d curve int:%d %d curve:%f range:%f\n",
           range_6581_int, curve_6581_int, curve_8580_int, curve, range);*/

    psid->factor = factor;

    switch (model) {
        default:
        case 0:
            psid->sid->setChipModel(MOS6581);
            psid->sid->input(0);
            psid->sid->setFilter6581Curve(curve);
            psid->sid->setFilter6581Range(range);
            strcpy(model_text, "MOS6581");
            break;
        case 1:
            psid->sid->setChipModel(CSG8580);
            psid->sid->input(0);
            psid->sid->setFilter6581Curve(curve);
            strcpy(model_text, "MOS8580");
            break;
        case 2:
            psid->sid->setChipModel(CSG8580);
            psid->sid->input(-32768);
            psid->sid->setFilter8580Curve(curve);
            strcpy(model_text, "MOS8580 + digi boost");
            break;
    }
    psid->sid->enableFilter(filters_enabled ? true : false);
    psid->sid->setCombinedWaveforms(combined_table[combined_strength_int]);

    switch (sampling) {
        default:
        case 0: /* "fast" */
            method = NONE;
            strcpy(method_text, "raw 1MHz output");
            break;
        case 1: /* "interpolating" */
            method = DECIMATE;
            strcpy(method_text, "linear interpolating");
            break;
        case 2: /* "resampling" */
        case 3: /* "fast resample" */
            method = RESAMPLE;
            sprintf(method_text, "SINC resampling");
            break;
    }

    psid->sid->setSamplingParameters(cycles_per_sec, method, speed);

    /* psid->sid->enable_raw_debug_output(rawoutput); */

    log_message(sound_log, "reSIDfp: %s, filter %s, sampling rate %dHz - %s",
                model_text,
                filters_enabled ? "on" : "off",
                speed, method_text);

    return 1;
}

static void residfp_close(sound_t *psid)
{
    delete psid->sid;
    delete psid;

    if (buf) {
        lib_free(buf);
        buf = NULL;
    }
}

static uint8_t residfp_read(sound_t *psid, uint16_t addr)
{
    return psid->sid->read(addr);
}

static void residfp_store(sound_t *psid, uint16_t addr, uint8_t byte)
{
    psid->sid_register[addr & 0x1f] = byte;
    psid->sid->write(addr, byte);
}

static void residfp_reset(sound_t *psid, CLOCK cpu_clk)
{
    psid->sid->reset();
}

#ifdef SOUND_SYSTEM_FLOAT
/* FIXME */
static int residfp_calculate_samples(sound_t *psid, float *pbuf, int nr, CLOCK *delta_t)
{
    short *tmp_buf;
    int retval;
    int int_delta_t_original = (int)*delta_t;
    int int_delta_t = (int)*delta_t;
    int i;

    /* Tried not to mess with resid during 64-bit conversion. clock(...) wants to modify *delta_t ... */

    if (psid->factor == 1000) {
        tmp_buf = getbuf(2 * nr);
        retval = psid->sid->clock(int_delta_t, tmp_buf, nr, 0);
        (*delta_t) += int_delta_t - int_delta_t_original;
        for (i = 0; i < nr; i++) {
            pbuf[i] = tmp_buf[i] / 32767.0;
        }
        return retval;
    }

    tmp_buf = getbuf(2 * nr * psid->factor / 1000);
    retval = psid->sid->clock(int_delta_t, tmp_buf, nr * psid->factor / 1000, 0) * 1000 / psid->factor;
    (*delta_t) += int_delta_t - int_delta_t_original;
    for (i = 0; i < nr; i++) {
        pbuf[i] = tmp_buf[i] / 32767.0;
    }

    return retval;
}
#else
static int residfp_calculate_samples(sound_t *psid, short *pbuf, int nr, int interleave, CLOCK *delta_t)
{
    short *tmp_buf;
    int retval;
    int int_delta_t = (int)*delta_t;

    /* Tried not to mess with resid during 64-bit conversion. clock(...) wants to modify *delta_t ... */

    if (psid->factor == 1000) {
        tmp_buf = getbuf(2 * nr);
        /* CAUTION: unlike ReSID; this does NOT return the number of cycles "left to do" in int_delta_t */
        retval = psid->sid->clock(int_delta_t, tmp_buf);
        {
            int n, p = 0;
            for (n = 0; n < retval; n++) {
                pbuf[p] = tmp_buf[n];
                p += interleave;
            }
        }

        (*delta_t) = 0;
        return retval;
    }

    /* Used when SID does not run at system clock ("SID card") */
    tmp_buf = getbuf(2 * nr * psid->factor / 1000);
    retval = psid->sid->clock(int_delta_t, tmp_buf);
    {
        int n, p = 0;
        for (n = 0; n < retval; n++) {
            pbuf[p] = tmp_buf[n];
            p += interleave;
        }
    }

    (*delta_t) = 0;
    return retval;
}
#endif

static char *residfp_dump_state(sound_t *psid)
{
    char strbuf[0x400];
    /* when sound is disabled *psid is NULL */
    if (psid && psid->sid) {
        /*state = psid->sid->read_state();*/
        psid->sid_register[25] = psid->sid->read(0x19);
        psid->sid_register[26] = psid->sid->read(0x1a);
        psid->sid_register[27] = psid->sid->read(0x1b);
        psid->sid_register[28] = psid->sid->read(0x1c);
    } else {
        return lib_strdup("no state available when sound is disabled.");
    }
    sprintf(strbuf,
            "FREQ:   %04x %04x %04x\n"
            "PULSE:  %04x %04x %04x\n"
            "CTRL:     %02x   %02x   %02x\n"
            "ADSR:   %04x %04x %04x\n"
            "FILTER: %04x RES: %02x MODE/VOL: %02x\n"
            "ADC: %02x %02x\n"
            "OSC3: %02x ENV3: %02x\n",
            ((psid->sid_register[(0 * 7) + 1] << 8) | psid->sid_register[(0 * 7) + 0]) & 0xffff,
            ((psid->sid_register[(1 * 7) + 1] << 8) | psid->sid_register[(1 * 7) + 0]) & 0xffff,
            ((psid->sid_register[(2 * 7) + 1] << 8) | psid->sid_register[(2 * 7) + 0]) & 0xffff,
            ((psid->sid_register[(0 * 7) + 3] << 8) | psid->sid_register[(0 * 7) + 2]) & 0xffff,
            ((psid->sid_register[(1 * 7) + 3] << 8) | psid->sid_register[(1 * 7) + 2]) & 0xffff,
            ((psid->sid_register[(2 * 7) + 3] << 8) | psid->sid_register[(2 * 7) + 2]) & 0xffff,
            (psid->sid_register[(0 * 7) + 4]) & 0xff,
            (psid->sid_register[(1 * 7) + 4]) & 0xff,
            (psid->sid_register[(2 * 7) + 4]) & 0xff,
            ((psid->sid_register[(0 * 7) + 5] << 8) | psid->sid_register[(0 * 7) + 6]) & 0xffff,
            ((psid->sid_register[(1 * 7) + 5] << 8) | psid->sid_register[(1 * 7) + 6]) & 0xffff,
            ((psid->sid_register[(2 * 7) + 5] << 8) | psid->sid_register[(2 * 7) + 6]) & 0xffff,
            ((psid->sid_register[22] << 8) | psid->sid_register[21]) & 0xffff,
            (psid->sid_register[23]) & 0xff,
            (psid->sid_register[24]) & 0xff,
            (psid->sid_register[25]) & 0xff,
            (psid->sid_register[26]) & 0xff,
            (psid->sid_register[27]) & 0xff,
            (psid->sid_register[28]) & 0xff
            );
    return lib_strdup(strbuf);
}

static void residfp_state_read(sound_t *psid, sid_snapshot_state_t *sid_state)
{
    unsigned int i;

    /* when sound is disabled *psid is NULL */
    if (psid) {
        /*state = psid->sid->read_state();*/
        psid->sid_register[25] = psid->sid->read(0x19);
        psid->sid_register[26] = psid->sid->read(0x1a);
        psid->sid_register[27] = psid->sid->read(0x1b);
        psid->sid_register[28] = psid->sid->read(0x1c);
    }

    for (i = 0; i < 0x20; i++) {
        sid_state->sid_register[i] = (uint8_t)psid->sid_register[i];
    }

#if 0
    sid_state->bus_value = (uint8_t)state.bus_value;
    sid_state->bus_value_ttl = (uint32_t)state.bus_value_ttl;
    for (i = 0; i < 3; i++) {
        sid_state->accumulator[i] = (uint32_t)state.accumulator[i];
        sid_state->shift_register[i] = (uint32_t)state.shift_register[i];
        sid_state->rate_counter[i] = (uint16_t)state.rate_counter[i];
        sid_state->rate_counter_period[i] = (uint16_t)state.rate_counter_period[i];
        sid_state->exponential_counter[i] = (uint16_t)state.exponential_counter[i];
        sid_state->exponential_counter_period[i] = (uint16_t)state.exponential_counter_period[i];
        sid_state->envelope_counter[i] = (uint8_t)state.envelope_counter[i];
        sid_state->envelope_state[i] = (uint8_t)state.envelope_state[i];
        sid_state->hold_zero[i] = (uint8_t)state.hold_zero[i];
        sid_state->envelope_pipeline[i] = (uint8_t)state.envelope_pipeline[i];
        sid_state->shift_pipeline[i] = (uint8_t)state.shift_pipeline[i];
        sid_state->shift_register_reset[i] = (uint32_t)state.shift_register_reset[i];
        sid_state->floating_output_ttl[i] = (uint32_t)state.floating_output_ttl[i];
        sid_state->pulse_output[i] = (uint16_t)state.pulse_output[i];
    }
    sid_state->write_pipeline = (uint8_t)state.write_pipeline;
    sid_state->write_address = (uint8_t)state.write_address;
    sid_state->voice_mask = (uint8_t)state.voice_mask;
#endif
}

static void residfp_state_write(sound_t *psid, sid_snapshot_state_t *sid_state)
{
    unsigned int i;

    for (i = 0; i < 0x20; i++) {
        psid->sid_register[i] = (char)sid_state->sid_register[i];
    }

#if 0
    state.bus_value = (reg8)sid_state->bus_value;
    state.bus_value_ttl = (cycle_count)sid_state->bus_value_ttl;
    for (i = 0; i < 3; i++) {
        state.accumulator[i] = (reg24)sid_state->accumulator[i];
        state.shift_register[i] = (reg24)sid_state->shift_register[i];
        state.rate_counter[i] = (reg16)sid_state->rate_counter[i];
        if (sid_state->rate_counter_period[i]) {
            state.rate_counter_period[i] = (reg16)sid_state->rate_counter_period[i];
        }
        state.exponential_counter[i] = (reg16)sid_state->exponential_counter[i];
        if (sid_state->exponential_counter_period[i]) {
            state.exponential_counter_period[i] = (reg16)sid_state->exponential_counter_period[i];
        }
        state.envelope_counter[i] = (reg8)sid_state->envelope_counter[i];
        state.envelope_state[i] = (EnvelopeGenerator::State)sid_state->envelope_state[i];
        state.hold_zero[i] = (sid_state->hold_zero[i] != 0);
        state.envelope_pipeline[i] = (cycle_count)sid_state->envelope_pipeline[i];
        state.shift_pipeline[i] = (cycle_count)sid_state->shift_pipeline[i];
        state.shift_register_reset[i] = (cycle_count)sid_state->shift_register_reset[i];
        state.floating_output_ttl[i] = (cycle_count)sid_state->floating_output_ttl[i];
        state.pulse_output[i] = (reg16)sid_state->pulse_output[i];
    }
    state.write_pipeline = (cycle_count)sid_state->write_pipeline;
    state.write_address = (reg8)sid_state->write_address;
    state.voice_mask = (reg4)sid_state->voice_mask;

    psid->sid->write_state((const reSID::SID::State)state);
#endif
}

sid_engine_t residfp_hooks =
{
    residfp_open,
    residfp_init,
    residfp_close,
    residfp_read,
    residfp_store,
    residfp_reset,
    residfp_calculate_samples,
    residfp_dump_state,
    residfp_state_read,
    residfp_state_write
};

} // extern "C"
