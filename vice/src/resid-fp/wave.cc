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

#include "wave.h"
#include "sid.h"

typedef struct {
    float bias;
    float pulsestrength;
    float topbit;
    float distance;
    float stmix;
} waveformconfig_t;

const float sharpness = 16.f;
const waveformconfig_t wfconfig[3][5] = {
  { /* kevtris chip D (6581r2/r3) */
    {0.9347278f, 0.0f, 0.0f, 1.017948f, 0.5655234f},
    {0.8931507f, 2.483499f, 0.0f, 0.03339716f, 0.0f},
    {0.8869214f, 2.440879f, 1.680824f, 0.02267573f, 0.0f},
    {0.9266459f, 0.7393153f, 0.0f, 0.0598464f, 0.1851717f},
    { 0.5f, 0.0f, 1.0f, 0.0f, 0.0f },
  },
  { /* kevtris chip G (6581r2/r3) */
    {0.949378f, 0.0f, 0.0f, 1.552844f, 0.6995564f},
    {0.895557f, 2.094213f, 0.0f, 0.02890657f, 0.0f},
    {0.8856111f, 2.299693f, 1.163996f, 0.01210956f, 0.0f},
    {0.9322616f, 0.6173857f, 0.0f, 0.06722359f, 0.2427691f},
    { 0.5f, 0.0f, 1.0f, 0.0f, 0.0f },
  },
  { /* kevtris chip V (8580) */
    {0.9737178f, 0.0f, 0.9935237f, 2.540223f, 0.9620218f},
    {0.9236207f, 2.19129f, 0.0f, 0.1108298f, 0.0f},
    {0.9247459f, 2.231508f, 0.9495742f, 0.1312631f, 0.0f},
    {0.9826007f, 1.296801f, 0.9701104f, 1.576534f, 0.9534868f},
    { 0.5f, 0.0f, 1.0f, 0.0f, 0.0f },
  },
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
static void populate(reg12 v, float o[12])
{
    int j = 1;
    for (int i = 0; i < 12; i ++) {
        o[i] = v & j ? 1.f : 0.f;
        j <<= 1;
    }
}

/* waveform values valid are 1 .. 7 */
void WaveformGeneratorFP::calculate_waveform_sample(float o[12])
{
  int i;

  /* P */
  if (waveform == 4) {
    populate((accumulator >> 12) >= pw ? 0xfff : 0x000, o);
    return;
  }

  const waveformconfig_t config = wfconfig[
    model == MOS6581FP ? 1 : 2
  ][
    waveform == 3 ? 0 :
    waveform == 5 ? 1 :
    waveform == 6 ? 2 :
    waveform == 7 ? 3 :
                    4
  ];

  /* S with strong top bit for 6581 */
  populate(accumulator >> 12, o);
  o[11] *= config.topbit;

  /* convert to T */
  if ((waveform & 3) == 1) {
    bool top = (accumulator & 0x800000) != 0;
    for (i = 11; i > 0; i --) {
      if (top) {
        o[i] = 1.0f - o[i-1];
      } else {
        o[i] = o[i-1];
      }
    }
    o[0] = 0;
  }

  /* convert to ST */
  if ((waveform & 3) == 3) {
    for (i = 11; i > 0; i --) {
      o[i] = o[i - 1] * (1.f - config.stmix) + o[i] * config.stmix;
    }
    /* bottom bit is grounded via T waveform selector */
    o[0] *= config.stmix;
  }

  /* ST, P* waveform? */
  if (waveform == 3 || waveform > 4) {
    float distancetable[12 * 2 + 1];
    for (i = 0; i <= 12; i ++) {
        distancetable[12+i] = distancetable[12-i] = 1.f / (1.f + i * i * config.distance);
    }

    float pulse = (accumulator >> 12) >= pw ? config.pulsestrength : 0.f;

    float tmp[12];
    for (i = 0; i < 12; i ++) {
        float avg = 0;
        float n = 0;
        for (int j = 0; j < 12; j ++) {
            float weight = distancetable[i - j + 12];
            avg += o[j] * weight;
            n += weight;
        }
        /* pulse control bit */
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

  /* use the environment around bias value to set/clear dac bit. The
   * relationship is nonlinear because that seems to sound a bit better. */
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
  set_chip_model(MOS6581FP);
  reset();
}

// ----------------------------------------------------------------------------
// Set chip model.
// ----------------------------------------------------------------------------
void WaveformGeneratorFP::set_chip_model(chip_model model)
{
  this->model = model;
  wave_zero = static_cast<float>(model == MOS6581FP ? -0x380 : -0x800);
}


// ----------------------------------------------------------------------------
// Register functions.
// ----------------------------------------------------------------------------
void WaveformGeneratorFP::writeFREQ_LO(reg8 freq_lo)
{
  freq = (freq & 0xff0000) | ((freq_lo << 8) & 0xff00);
}

void WaveformGeneratorFP::writeFREQ_HI(reg8 freq_hi)
{
  freq = ((freq_hi << 16) & 0xff0000) | (freq & 0xff00);
}

void WaveformGeneratorFP::writePW_LO(reg8 pw_lo)
{
  pw = (pw & 0xf00) | (pw_lo & 0x0ff);
}

void WaveformGeneratorFP::writePW_HI(reg8 pw_hi)
{
  pw = ((pw_hi << 8) & 0xf00) | (pw & 0x0ff);
}

void WaveformGeneratorFP::writeCONTROL_REG(WaveformGeneratorFP& source, reg8 control)
{
  /* when selecting the 0 waveform, the previous output is held for
   * a time in the DAC MOSFET gates. We keep on holding forever, though... */
  if (waveform != 0 && (control & 0x10) == 0) {
    previous = readOSC(source);
    previous_dac = output(source);
  }

  waveform = (control >> 4) & 0x0f;
  ring_mod = (control & 0x04) != 0;
  sync = (control & 0x02) != 0;
  bool test_next = (control & 0x08) != 0;

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

reg8 WaveformGeneratorFP::readOSC(WaveformGeneratorFP& source)
{
  float o[12];

  if (waveform == 0) {
    return previous;
  }
  if (waveform == 8) {
    return (reg8) (noise_output_cached >> 4);
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
  accumulator ^= (waveform & 3) == 1 && ring_mod && (source.accumulator & 0x800000) ? 0x800000 : 0;
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

void WaveformGeneratorFP::clock_noise(const bool clock)
{
  if (clock) {
    reg24 bit0 = ((shift_register >> 22) ^ (shift_register >> 17)) & 0x1;
    shift_register <<= 1;
    shift_register |= bit0;
  }

  // clear output bits of shift register if noise and other waveforms
  // are selected simultaneously
  if (waveform > 8) {
    shift_register &= 0x7fffff^(1<<22)^(1<<20)^(1<<16)^(1<<13)^(1<<11)^(1<<7)^(1<<4)^(1<<2);
  }

  noise_output_cached = outputN___();
  noise_output_cached_dac = wave_zero;
  for (int i = 4; i < 12; i ++) {
    if (noise_output_cached & (1 << i)) {
      noise_output_cached_dac += dac[i];
    }
  }
}

reg12 WaveformGeneratorFP::outputN___()
{
  return
    ((shift_register & 0x400000) >> 11) |
    ((shift_register & 0x100000) >> 10) |
    ((shift_register & 0x010000) >> 7) |
    ((shift_register & 0x002000) >> 5) |
    ((shift_register & 0x000800) >> 4) |
    ((shift_register & 0x000080) >> 1) |
    ((shift_register & 0x000010) << 1) |
    ((shift_register & 0x000004) << 2);
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
  waveform = 0;
  writeCONTROL_REG(*this, 0);
  msb_rising = false;
}
