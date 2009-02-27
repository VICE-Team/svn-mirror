//  ---------------------------------------------------------------------------
//  This file is part of reSID, a MOS6581 SID emulator engine.
//  Copyright (C) 2004  Dag Lem <resid@nimrod.no>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  ---------------------------------------------------------------------------

#define __WAVE_CC__
#include "wave.h"
#include "sid.h"

typedef struct {
    float bias;
    float distance;
} waveformconfig_t;

const float sharpness = 24.f;
const waveformconfig_t wfconfig[2][4] = {
  { /* kevtris chip D */
    { 0.9157283f, 2.21507f,   }, /* 3 */
    { 0.7928209f, 0.08109789f }, /* 5 */
    { 0.7992666f, 0.04497359f }, /* 6 */
    { 0.9184855f, 0.04872148f }, /* 7 */
  },
  { /* kevtris chip V */
    { 0.9113708f, 1.898943f   }, /* 3 */
    { 0.8055819f, 0.2157787f  }, /* 5 */
    { 0.827376f,  0.2223487f  }, /* 6 */
    { 0.9409516f, 1.793725f   }, /* 7 */
  }
};

/* render output from bitstate */
static float make_sample(float *o) {
  float out = 0;
  for (int i = 0; i < 12; i ++) {
    out += o[i] * dac[i];
  }
  return out;
}

/* generate tables for waveforms 1 .. 7 */
void WaveformGeneratorFP::rebuild_wftable() {
    float o[12];
    reg8 oldwf = waveform;
    reg24 oldacc = accumulator;
    reg12 oldpw = pw;

    for (waveform = 1; waveform < 8; waveform ++) {
        for (accumulator = 0; accumulator < (1<<24); accumulator += (1<<12)) {
            /* generate pulse-low variants. Also,
             * when waveform < 4, pw doesn't matter. */
            pw = 0x1000; /* pulse always low */
            calculate_waveform_sample(o);
            wftable[waveform - 1][accumulator >> 12] = make_sample(o) + wave_zero;
            /* Add pulse-high variants after pulse-low state variants */
            if (waveform >= 4) {
                pw = 0x000; /* pulse always high */
                calculate_waveform_sample(o);
                wftable[waveform + 3][accumulator >> 12] = make_sample(o) + wave_zero;
            }
        }
    }

    waveform = oldwf;
    accumulator = oldacc;
    pw = oldpw;
}

/* explode reg12 to a floating point bit array */
static void populate(reg12 v, float *o)
{
    int j = 1;
    for (int i = 0; i < 12; i ++) {
        o[i] = v & j ? 1 : 0;
        j <<= 1;
    }
}

/* waveform values valid are 1 .. 7 */
void WaveformGeneratorFP::calculate_waveform_sample(float *o)
{
  int i;

  /* P */
  if (waveform == 4) {
    populate((accumulator >> 12) >= pw ? 0xfff : 0x000, o);
    return;
  }

  const waveformconfig_t config = wfconfig[
    model == MOS6581FP ? 0 : 1
  ][
    waveform == 3 ? 0 :
    waveform == 5 ? 1 :
    waveform == 6 ? 2 :
                    3
  ];
  /* table #3 is valid also for waveforms 1, 2 -- anything goes as long as
   * 0 < bias < 1. */

  /* S with strong top bit for 6581 */
  populate(accumulator >> 12, o);
  if (model == MOS6581FP) {
    o[11] *= 1.5f;
  }

  /* convert to T */
  if ((waveform & 3) == 1) {
    float top = o[11];
    for (i = 11; i > 0; i --) {
      if (top == 0.0f) {
        o[i] = o[i-1];
      } else {
        o[i] = 1.0f - o[i-1];
      }
    }
    o[0] = 0;
  }

  /* convert to ST */
  if ((waveform & 3) == 3) {
    for (i = 11; i > 0; i --) {
      o[i] = (o[i - 1] + o[i]) * 0.5f;
    }
    /* bottom bit is grounded via T waveform selector */
    o[0] = o[0] * 0.5f;
    /* For some reason, 6581 ST always has high bit low. Fitting against model
     * of ST indicates a 50 % strength for the top bit. 8580 ST reaches 255
     * according to OSC3 samplings. XOR circuit is disabled on both, because
     * sawtooth is generated. */
    if (model == MOS6581FP) {
        o[11] *= 0.5;
    }
  }

  /* ST, P* waveform? */
  if (waveform == 3 || waveform > 4) {
    float distancetable[12 * 2 + 1];
    for (i = 0; i <= 12; i ++) {
        distancetable[12+i] = distancetable[12-i] = 1.f / (1.f + i * i * config.distance);
    }

    /* Is the pulse control high or low? The low state appears to suppress
     * more powerfully than mere 0-bit would, so I'm actually supplying a
     * negative value for this. Tel's song called 'Digi-Piece for Telecomsoft'
     * uses the pulse-low suppression. */
    float pulse = (accumulator >> 12) >= pw ? 1.0f : -2.0f;
    float tmp[12];
    for (i = 0; i < 12; i ++) {
        float avg = 0;
        float n = 0;
        for (int j = 0; j < 12; j ++) {
            if (i == j) {
                continue;
            }
            float weight = distancetable[i - j + 12];
            avg += o[j] * weight;
            n += weight;
        }
        /* pulse control bit is included for waveform > 4 */
        if (waveform > 4) {
            float weight = distancetable[i - 12 + 12];
            avg += pulse * weight;
            n += weight;
        }

        tmp[i] = (o[i] + avg / n) * 0.5f;
    }

    for (i = 0; i < 12; i ++) {
        o[i] = tmp[i];
    }
  }

  /* use the environment around 0.95 to set/clear dac bit */
  for (i = 0; i < 12; i ++) {
    o[i] = (o[i] - config.bias) * sharpness;

    o[i] += 0.707107f;
    if (o[i] > 1.f) {
       o[i] = 1.f;
    }
    if (o[i] < 0.f) {
        o[i] = 0.;
    }
    o[i] = o[i] * o[i];
  }
}

void WaveformGeneratorFP::set_nonlinearity(float nl)
{
  for (int i = 0; i < 12; i ++) {
    dac[i] = SIDFP::kinked_dac((1 << i), nl, 12);
  }
}

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
WaveformGeneratorFP::WaveformGeneratorFP()
{
  sync_source = this;

  set_chip_model(MOS6581FP);

  reset();
}


// ----------------------------------------------------------------------------
// Set sync source.
// ----------------------------------------------------------------------------
void WaveformGeneratorFP::set_sync_source(WaveformGeneratorFP* source)
{
  sync_source = source;
  source->sync_dest = this;
}


// ----------------------------------------------------------------------------
// Set chip model.
// ----------------------------------------------------------------------------
void WaveformGeneratorFP::set_chip_model(chip_model model)
{
  this->model = model;
  wave_zero = (float) (model == MOS6581FP ? -0x380 : -0x800);
}


// ----------------------------------------------------------------------------
// Register functions.
// ----------------------------------------------------------------------------
void WaveformGeneratorFP::writeFREQ_LO(reg8 freq_lo)
{
  freq = (freq & 0xff00) | (freq_lo & 0x00ff);
}

void WaveformGeneratorFP::writeFREQ_HI(reg8 freq_hi)
{
  freq = ((freq_hi << 8) & 0xff00) | (freq & 0x00ff);
}

/* The original form was (acc >> 12) >= pw, where truth value is not affected
 * by the contents of the low 12 bits. Therefore the lowest bits must be zero
 * in the new formulation acc >= (pw << 12). */
void WaveformGeneratorFP::writePW_LO(reg8 pw_lo)
{
  pw = (pw & 0xf00) | (pw_lo & 0x0ff);
}

void WaveformGeneratorFP::writePW_HI(reg8 pw_hi)
{
  pw = ((pw_hi << 8) & 0xf00) | (pw & 0x0ff);
}

void WaveformGeneratorFP::writeCONTROL_REG(reg8 control)
{
  /* when selecting the 0 waveform, the previous output is held for
   * a time in the DAC MOSFET gates. We keep on holding forever, though... */
  if (waveform != 0 && (control & 0x10) == 0) {
    previous = readOSC();
    previous_dac = output();
  }

  waveform = (control >> 4) & 0x0f;
  ring_mod = control & 0x04;
  sync = control & 0x02;
  reg8 test_next = control & 0x08;

  // Test bit rising? Invert bit 19 and write it to bit 1.
  if (test_next && !test) {
    accumulator = 0;
    reg24 bit19 = (shift_register >> 18) & 2;
    shift_register = (shift_register & 0x7ffffd) | (bit19^2);
    noise_overwrite_delay = 200000; /* 200 ms, probably too generous? */
  }

  // Test bit falling? clock noise once,
  // otherwise just emulate noise's combined waveforms.
  clock_noise(!test_next && test);
 
  test = test_next;
}

reg8 WaveformGeneratorFP::readOSC()
{
  float o[12];

  if (waveform == 0) {
    return previous;
  }
  if (waveform == 8) {
    return noise_output_cached;
  }
  if (waveform > 8) {
    return 0;
  }

  /* Include effects of the test bit & ring mod */
  reg12 oldpw = pw;
  if (test) {
    pw = 0;
  }
  reg24 oldaccumulator = accumulator;
  if ((waveform & 3) == 1 && ring_mod && (sync_source->accumulator & 0x800000) != 0) {
    accumulator ^= 0x800000;
  }
  calculate_waveform_sample(o);
  pw = oldpw;
  accumulator = oldaccumulator;

  reg8 out = 0;
  reg8 bit = 1;
  for (int i = 4; i < 12; i ++) {
    if (o[i] > 0.5f) {
        out |= bit;
    }
    bit <<= 1;
  }
  return out;
}

// ----------------------------------------------------------------------------
// SID reset.
// ----------------------------------------------------------------------------
void WaveformGeneratorFP::reset()
{
  accumulator = 0;
  previous = 0;
  previous_dac = 0;
  noise_output_cached = 0;
  noise_output_cached_dac = 0;
  shift_register = 0x7ffffc;
  freq = 0;
  pw = 0;
  test = 0;
  writeCONTROL_REG(0);
  msb_rising = false;
}
