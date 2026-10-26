/*
 * cascom_dsp.c - Bell-202 AFSK modulator/demodulator for the cascom modem.
 *
 * Written by
 *  Felix Erckenbrecht <code@eligs.de>
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

#include <string.h>

#include "cascom_dsp.h"

/* ------------------------------------------------------------------ */
/* Continuous-phase FSK modulator (fixed-point NCO)                   */
/* ------------------------------------------------------------------ */

/* Sine via a quarter-wave lookup table. The full circle has 2^SINE_BITS points;
   by symmetry we store only the first quadrant (SINE_QUART = 2^(SINE_BITS-2)
   entries) as signed Q15. At SINE_BITS = 9 that's 128 int16 = 256 bytes, small
   enough to stay resident in L1 regardless of the emulator's memory churn. The
   table is a compile-time constant (entry k = round(32767*sin(pi/2 * k/128))),
   so the DSP pulls in no libm at all. */
#define CASCOM_SINE_BITS  9
#define CASCOM_SINE_QUART (1 << (CASCOM_SINE_BITS - 2))     /* 128 */
#define CASCOM_QUARTER    0x40000000u                       /* +pi/2 in phase units */
/* right-shift applied to each correlator sum before squaring, so the sum-of-
   squares energy of a full window (<=128 Q15xQ15 products, ~2^37) stays inside
   int64. Both tones use the same shift, so the mark/space ratio is preserved. */
#define CASCOM_MAG_SHIFT  8

static const int16_t cascom_sine_quarter[CASCOM_SINE_QUART] = {   /* sin(0..pi/2), Q15 */
         0,    402,    804,   1206,   1608,   2009,   2410,   2811,
      3212,   3612,   4011,   4410,   4808,   5205,   5602,   5998,
      6393,   6786,   7179,   7571,   7962,   8351,   8739,   9126,
      9512,   9896,  10278,  10659,  11039,  11417,  11793,  12167,
     12539,  12910,  13279,  13645,  14010,  14372,  14732,  15090,
     15446,  15800,  16151,  16499,  16846,  17189,  17530,  17869,
     18204,  18537,  18868,  19195,  19519,  19841,  20159,  20475,
     20787,  21096,  21403,  21705,  22005,  22301,  22594,  22884,
     23170,  23452,  23731,  24007,  24279,  24547,  24811,  25072,
     25329,  25582,  25832,  26077,  26319,  26556,  26790,  27019,
     27245,  27466,  27683,  27896,  28105,  28310,  28510,  28706,
     28898,  29085,  29268,  29447,  29621,  29791,  29956,  30117,
     30273,  30424,  30571,  30714,  30852,  30985,  31113,  31237,
     31356,  31470,  31580,  31685,  31785,  31880,  31971,  32057,
     32137,  32213,  32285,  32351,  32412,  32469,  32521,  32567,
     32609,  32646,  32678,  32705,  32728,  32745,  32757,  32765,
};

/* full-circle sine (Q15) from the top CASCOM_SINE_BITS of the phase accumulator */
static int cascom_sine(uint32_t phase)
{
    unsigned idx  = phase >> (32 - CASCOM_SINE_BITS);       /* 0 .. 2^BITS-1 */
    unsigned quad = (idx >> (CASCOM_SINE_BITS - 2)) & 3u;   /* which quadrant */
    unsigned pos  = idx & (CASCOM_SINE_QUART - 1);          /* offset in quadrant */

    switch (quad) {
        case 0:  return  cascom_sine_quarter[pos];                       /* 0    .. pi/2 */
        case 1:  return  cascom_sine_quarter[CASCOM_SINE_QUART - 1 - pos];/* pi/2 .. pi   */
        case 2:  return -cascom_sine_quarter[pos];                       /* pi   .. 3pi/2 */
        default: return -cascom_sine_quarter[CASCOM_SINE_QUART - 1 - pos];/* 3pi/2.. 2pi  */
    }
}

void cascom_mod_init(cascom_mod_t *m, double sample_rate,
                     double mark_hz, double space_hz, double amplitude)
{
    m->phase     = 0;
    /* phase increment = f / fs of a full 2^32 turn (round via +0.5, no libm) */
    m->inc_mark  = (uint32_t)(4294967296.0 * mark_hz  / sample_rate + 0.5);
    m->inc_space = (uint32_t)(4294967296.0 * space_hz / sample_rate + 0.5);
    m->gain      = (int32_t)(amplitude * 32768.0 + 0.5);   /* Q15 */
    if (m->gain > 32768) {
        m->gain = 32768;
    } else if (m->gain < 0) {
        m->gain = 0;
    }
}

int16_t cascom_mod_next(cascom_mod_t *m, int mark)
{
    int32_t s = cascom_sine(m->phase);               /* Q15 sine */

    m->phase += mark ? m->inc_mark : m->inc_space;   /* wraps at 2^32 */
    return (int16_t)((s * m->gain) >> 15);           /* apply Q15 gain */
}

/* ------------------------------------------------------------------ */
/* Non-coherent quadrature matched-filter FSK demodulator             */
/* ------------------------------------------------------------------ */

void cascom_demod_init(cascom_demod_t *d, double sample_rate,
                       double mark_hz, double space_hz, int win)
{
    memset(d, 0, sizeof(*d));
    if (win < 1) {
        win = 1;
    }
    if (win > CASCOM_DEMOD_MAXWIN) {
        win = CASCOM_DEMOD_MAXWIN;
    }
    d->win = win;
    /* reference NCO increments = f / fs of a full 2^32 turn (round, no libm) */
    d->dm = (uint32_t)(4294967296.0 * mark_hz  / sample_rate + 0.5);
    d->ds = (uint32_t)(4294967296.0 * space_hz / sample_rate + 0.5);
}

int cascom_demod_next(cascom_demod_t *d, int16_t x)
{
    int32_t xi = x;                       /* 16-bit sample is already the Q15 input */
    int32_t c_m;
    int32_t s_m;
    int32_t c_s;
    int32_t s_s;
    int64_t mag_m;
    int64_t mag_s;
    int64_t a;
    int64_t b;
    int i;

    /* quadrature reference tones from the shared sine LUT (cos = sin(+pi/2)) */
    c_m = cascom_sine(d->pm + CASCOM_QUARTER);
    s_m = cascom_sine(d->pm);
    c_s = cascom_sine(d->ps + CASCOM_QUARTER);
    s_s = cascom_sine(d->ps);
    d->pm += d->dm;                   /* wrap at 2^32 */
    d->ps += d->ds;

    /* sliding-window sums of the four Q15xQ15 products (each fits int32) */
    i = d->idx;
    {
        int32_t xmc = xi * c_m;
        int32_t xms = xi * s_m;
        int32_t xsc = xi * c_s;
        int32_t xss = xi * s_s;
        d->sum_mc += xmc - d->mc[i];
        d->mc[i] = xmc;
        d->sum_ms += xms - d->ms[i];
        d->ms[i] = xms;
        d->sum_sc += xsc - d->sc[i];
        d->sc[i] = xsc;
        d->sum_ss += xss - d->ss[i];
        d->ss[i] = xss;
    }
    d->idx = (i + 1 == d->win) ? 0 : i + 1;

    a = d->sum_mc >> CASCOM_MAG_SHIFT;
    b = d->sum_ms >> CASCOM_MAG_SHIFT;
    mag_m = a * a + b * b;
    a = d->sum_sc >> CASCOM_MAG_SHIFT;
    b = d->sum_ss >> CASCOM_MAG_SHIFT;
    mag_s = a * a + b * b;

    /* the stronger tone wins; an all-zero window decides mark (= line idle) */
    return (mag_m >= mag_s) ? 1 : 0;
}
