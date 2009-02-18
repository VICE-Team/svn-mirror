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

    float wave_zero = (float) (model == MOS6581FP ? 0x380 : 0x800);

    for (waveform = 1; waveform < 8; waveform ++) {
        for (accumulator = 0; accumulator < (1<<24); accumulator += (1<<12)) {
            /* generate pulse-low variants. Also,
             * when waveform < 4, pw doesn't matter. */
            pw = 0xfff; /* pulse low except for accumulator = 0xfff???. */
            calculate_waveform_sample(o);
            wftable[waveform - 1][accumulator >> 12] = make_sample(o) - wave_zero;
            /* Add pulse-high variants after pulse-low state variants */
            if (waveform >= 4) {
                pw = 0x000; /* pulse always high */
                calculate_waveform_sample(o);
                wftable[waveform + 3][accumulator >> 12] = make_sample(o) - wave_zero;
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
  float old;

  /* waveforms left: 1 .. 7 */
  switch (waveform & 3) {
  case 1: /* T, PT */
    populate(output___T(), o);
    break;

  case 2: /* S, PS */
    populate(output__S_(), o);
    break;

  case 3: /* ST, PST */
    populate(output__S_(), o);
    /* calculate ST */
    float factor;

    /* For some reason, 6581 ST always has high bit low.
     * 8580 ST reaches 255 according to OSC3 samplings.
     * The XOR circuit doesn't seem to work on 8580 either, though. */
    if (model == MOS6581FP) {
        o[11] = 0;
        factor = 0.5;
    } else {
        factor = 0.75;
    }
    for (i = 11; i > 0; i --) {
      o[i] = (o[i - 1] + o[i]) * 0.5f;
    }
    o[0] = 1;

    old = 0;
    for (i = 0; i < 12; i ++) {
      old = old * (1.f - factor) + o[i] * factor;
      o[i] = old;
    }
    break;

  default: /* P */
    populate(output_P__(), o);
    break;
  }

  /* P* waveform? */
  if (waveform > 4) {
    /* attenuation rate of adjacent bits */
    float distance;
    /* tunable to set combined waveform loudness */
    float bias;
    if (model == MOS6581FP) {
        /* This controls the high-frequency content of the waveform by
         * adjusting the FIR length. Low value means a long FIR, and lots
         * of noise as the lowest bits toggle often. */
        distance = 0.05f;
        /* Various chips are reproduced by settings from 0.1 to 0.35.
         * the lower this value, the quieter and edgier the combined waves
         * become. There is a threshold around 0.25f where the PS waveform
         * (a jagged rising sawtooth) becomes two half-intensity sawtooths.
         * only kevtris chips C, D, E and H have a complete ramp in PS. */
        bias = 0.25f;

        if (waveform == 6) {
            if (bias < 0.25f) {
                o[11] = 0;
            }
            /* PS is significantly louder than PT */
            bias *= 1.33f;
        }
        if (waveform == 7) {
            /* PST is quiet compared to PT. */
            bias *= 0.6f;
        }
    } else {
        distance = 0.2f;
        bias = 0.25f;
    }

    float distancetable[12 * 2 + 1];
    for (i = 0; i <= 12; i ++) {
        distancetable[12+i] = distancetable[12-i] = 1.f / (1.f + i * i * distance);
    }

    float pulse = (accumulator >> 12) >= pw ? 1.0f : 0.0f;
    float tmp[12];
    for (i = 0; i < 12; i ++) {
        float avg = 0;
        float n = 0;
        for (int j = 0; j <= 12; j ++) {
            if (i == j) {
                continue;
            }
            float weight = distancetable[i - j + 12];
            if (j == 12) {
                avg += pulse * weight;
            } else {
                avg += o[j] * weight;
            }
            n += weight;
        }

        tmp[i] = (o[i] + avg / n + bias) * 0.5f;
        if (tmp[i] > 1.f) {
            tmp[i] = 1.f;
        }
    }

    for (i = 0; i < 12; i ++) {
        o[i] = tmp[i];
    }
  }

  /* x^8 seems to approximate the DAC output well enough */
  for (i = 0; i < 12; i ++) {
    o[i] = o[i] * o[i];
    o[i] = o[i] * o[i];
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
