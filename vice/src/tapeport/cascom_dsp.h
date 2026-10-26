/*
 * cascom_dsp.h - Bell-202 AFSK modulator/demodulator for the cascom modem.
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

/* Pure, dependency-free DSP for a 1200-baud Bell-202 AFSK modem - fixed-point,
 * no libm, no floating-point trig (a static quarter-wave sine LUT is shared by
 * both the modulator's and the demodulator's NCOs):
 *
 *   - cascom_mod_t   : continuous-phase FSK modulator (TXD level -> tone samples)
 *   - cascom_demod_t : non-coherent quadrature matched-filter FSK demodulator
 *                      (tone samples -> mark/space bit)
 *
 * The state lives in caller-owned structs, so the modulator and demodulator are
 * trivially testable in isolation.
 */

#ifndef VICE_CASCOM_DSP_H
#define VICE_CASCOM_DSP_H

#include <stdint.h>

/* max samples-per-bit window the matched filter supports
   (e.g. 48000/1200 = 40; 9600/1200 = 8) */
#define CASCOM_DEMOD_MAXWIN 128

/* continuous-phase FSK modulator, implemented as a fixed-point NCO: a phase
   accumulator (a full turn = 2^32) indexes a 256-byte quarter-wave sine LUT, so
   the per-sample hot path uses no libm / float trig. FSK is a phase-continuous
   increment switch, exactly like the TCM3105. */
typedef struct cascom_mod_s {
    uint32_t phase;     /* NCO phase accumulator */
    uint32_t inc_mark;  /* per-sample phase increment for mark  (1200 Hz) */
    uint32_t inc_space; /* per-sample phase increment for space (2200 Hz) */
    int32_t  gain;      /* output amplitude, Q15 (0 .. 32768) */
} cascom_mod_t;

/* non-coherent quadrature matched-filter FSK demodulator (fixed-point: NCO
   reference oscillators sharing the modulator's sine LUT; integer correlator) */
typedef struct cascom_demod_s {
    int win;                          /* samples per bit (matched-filter length) */
    int idx;                          /* circular-buffer write index */

    uint32_t pm;                      /* mark reference NCO phase */
    uint32_t dm;                      /* mark reference NCO increment */
    uint32_t ps;                      /* space reference NCO phase */
    uint32_t ds;                      /* space reference NCO increment */

    /* circular buffers of the last 'win' quadrature products, plus running sums */
    int32_t mc[CASCOM_DEMOD_MAXWIN];
    int32_t ms[CASCOM_DEMOD_MAXWIN];
    int32_t sc[CASCOM_DEMOD_MAXWIN];
    int32_t ss[CASCOM_DEMOD_MAXWIN];
    int64_t sum_mc;
    int64_t sum_ms;
    int64_t sum_sc;
    int64_t sum_ss;
} cascom_demod_t;

/* initialise a modulator for the given sample rate / tone frequencies */
void cascom_mod_init(cascom_mod_t *m, double sample_rate,
                     double mark_hz, double space_hz, double amplitude);

/* produce the next 16-bit output sample for the given TXD level (1 = mark,
   0 = space); phase is continuous across bit changes (CPFSK, like the TCM3105) */
int16_t cascom_mod_next(cascom_mod_t *m, int mark);

/* initialise a demodulator; 'win' is the matched-filter length in samples
   (typically sample_rate / baud) */
void cascom_demod_init(cascom_demod_t *d, double sample_rate,
                       double mark_hz, double space_hz, int win);

/* feed one 16-bit input sample; returns the current decided bit (1 = mark,
   0 = space). Carrier detect is not the demodulator's job: the receiving
   software (e.g. DIGICOM's DPLL) derives DCD from the data-line transitions. */
int cascom_demod_next(cascom_demod_t *d, int16_t x);

#endif
