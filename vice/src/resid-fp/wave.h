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

#ifndef VICE__WAVE_H__
#define VICE__WAVE_H__

#include "siddefs-fp.h"

extern float dac[12];
extern float wftable[11][4096];

// ----------------------------------------------------------------------------
// A 24 bit accumulator is the basis for waveform generation. FREQ is added to
// the lower 16 bits of the accumulator each cycle.
// The accumulator is set to zero when TEST is set, and starts counting
// when TEST is cleared.
// The noise waveform is taken from intermediate bits of a 23 bit shift
// register. This register is clocked by bit 19 of the accumulator.
// ----------------------------------------------------------------------------
class WaveformGeneratorFP
{
public:
  WaveformGeneratorFP();

  void set_sync_source(WaveformGeneratorFP*);
  void set_chip_model(chip_model model);

  RESID_INLINE void clock();
  RESID_INLINE void synchronize();
  void reset();

  void writeFREQ_LO(reg8);
  void writeFREQ_HI(reg8);
  void writePW_LO(reg8);
  void writePW_HI(reg8);
  void writeCONTROL_REG(reg8);
  reg8 readOSC();

  RESID_INLINE float output();

protected:
  RESID_INLINE void clock_noise(const bool clock);
  void set_nonlinearity(float nl);
  void rebuild_wftable();
  void calculate_waveform_sample(float *o);

  const WaveformGeneratorFP* sync_source;
  WaveformGeneratorFP* sync_dest;

  chip_model model;

  // Tell whether the accumulator MSB was set high on this cycle.
  bool msb_rising;

  reg24 accumulator;
  reg24 shift_register;
  reg12 noise_output_cached;
  reg8 previous;
  int noise_overwrite_delay;

  // Fout  = (Fn*Fclk/16777216)Hz
  reg16 freq;
  // PWout = (PWn/40.95)%
  reg12 pw;

  // The control register right-shifted 4 bits; used for output function
  // table lookup.
  reg8 waveform;

  // The remaining control register bits.
  bool test, ring_mod, sync;
  // The gate bit is handled by the EnvelopeGenerator.

  float previous_dac, noise_output_cached_dac;

  // 4 possible combinations of waveforms.
  // These do generate the waveforms, but ignore ring modulation and test bit.
  RESID_INLINE reg12 output___T();
  RESID_INLINE reg12 output__S_();
  RESID_INLINE reg12 output_P__();
  RESID_INLINE reg12 outputN___();

friend class VoiceFP;
friend class SIDFP;
};

// ----------------------------------------------------------------------------
// SID clocking - 1 cycle.
// ----------------------------------------------------------------------------
RESID_INLINE
void WaveformGeneratorFP::clock()
{
  /* no digital operation if test bit is set. Only emulate analog fade. */
  if (test) {
    if (noise_overwrite_delay != 0) {
        if (-- noise_overwrite_delay == 0) {
            shift_register |= 0x7ffffc;
            noise_output_cached = outputN___();
            noise_output_cached_dac = 0;
            for (int i = 0; i < 12; i ++) {
                if (noise_output_cached & (1 << i)) {
                    noise_output_cached_dac += dac[i];
                }
            }
        }
    }
    return;
  }

  reg24 accumulator_prev = accumulator;

  // Calculate new accumulator value;
  accumulator += freq;
  accumulator &= 0xffffff;

  // Check whether the MSB became set high. This is used for synchronization.
  msb_rising = !(accumulator_prev & 0x800000) && (accumulator & 0x800000);

  // Shift noise register once for each time accumulator bit 19 is set high.
  if (!(accumulator_prev & 0x080000) && (accumulator & 0x080000)) {
    clock_noise(true);
  }
}

RESID_INLINE
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
  noise_output_cached_dac = 0;
  for (int i = 0; i < 12; i ++) {
    if (noise_output_cached & (1 << i)) {
      noise_output_cached_dac += dac[i];
    }
  }
}

// ----------------------------------------------------------------------------
// Synchronize oscillators.
// This must be done after all the oscillators have been clock()'ed since the
// oscillators operate in parallel.
// Note that the oscillators must be clocked exactly on the cycle when the
// MSB is set high for hard sync to operate correctly. See SID::clock().
// ----------------------------------------------------------------------------
RESID_INLINE
void WaveformGeneratorFP::synchronize()
{
  // A special case occurs when a sync source is synced itself on the same
  // cycle as when its MSB is set high. In this case the destination will
  // not be synced. This has been verified by sampling OSC3.
  if (msb_rising && sync_dest->sync && !(sync && sync_source->msb_rising)) {
    sync_dest->accumulator = 0;
  }
}


// ----------------------------------------------------------------------------
// Output functions.
// NB! The output from SID 8580 is delayed one cycle compared to SID 6581,
// this is not modeled.
// ----------------------------------------------------------------------------

// Triangle:
// The upper 12 bits of the accumulator are used.
// The MSB is used to create the falling edge of the triangle by inverting
// the lower 11 bits. The MSB is thrown away and the lower 11 bits are
// left-shifted (half the resolution, full amplitude).
// Ring modulation substitutes the MSB with MSB EOR sync_source MSB.
//
RESID_INLINE
reg12 WaveformGeneratorFP::output___T()
{
  return ((accumulator & 0x800000 ? ~accumulator : accumulator) >> 11) & 0xfff;
}

// Sawtooth:
// The output is identical to the upper 12 bits of the accumulator.
//
RESID_INLINE
reg12 WaveformGeneratorFP::output__S_()
{
  return accumulator >> 12;
}

// Pulse:
// The upper 12 bits of the accumulator are used.
// These bits are compared to the pulse width register by a 12 bit digital
// comparator; output is either all one or all zero bits.
// NB! The output is actually delayed one cycle after the compare.
// This is not modeled.
//
// The test bit, when set to one, holds the pulse waveform output at 0xfff
// regardless of the pulse width setting.
//
RESID_INLINE
reg12 WaveformGeneratorFP::output_P__()
{
  return ((accumulator >> 12) >= pw) ? 0xfff : 0x000;
}

// Noise:
// The noise output is taken from intermediate bits of a 23-bit shift register
// which is clocked by bit 19 of the accumulator.
// NB! The output is actually delayed 2 cycles after bit 19 is set high.
// This is not modeled.
//
// Operation: Calculate EOR result, shift register, set bit 0 = result.
//
//                        ----------------------->---------------------
//                        |                                            |
//                   ----EOR----                                       |
//                   |         |                                       |
//                   2 2 2 1 1 1 1 1 1 1 1 1 1                         |
// Register bits:    2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 <---
//                   |   |       |     |   |       |     |   |
// OSC3 bits  :      7   6       5     4   3       2     1   0
//
// Since waveform output is 12 bits the output is left-shifted 4 times.
//
RESID_INLINE
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
// Select one of 16 possible combinations of waveforms.
// ----------------------------------------------------------------------------
RESID_INLINE
float WaveformGeneratorFP::output()
{
  if (waveform == 0) {
    return previous_dac;
  }
  if (waveform == 8) {
    return noise_output_cached_dac;
  }
  if (waveform > 8) {
    return 0;
  }
  /* waveforms 1 .. 7 left */

  /* Phase for all waveforms */
  reg12 phase = accumulator >> 12;
  /* pulse on/off generates 4 more variants after the main pulse types */
  int variant = waveform >= 4 && (test || phase >= pw) ? 3 : -1;

  /* triangle waveform XOR circuit. Since the table already makes a triangle
   * wave internally, we only need to account for the sync source here.
   * Flipping the top bit suffices to reproduce the original SID ringmod */
  if ((waveform & 3) == 1 && ring_mod && (sync_source->accumulator & 0x800000)) {
    phase ^= 0x800;
  }
  return wftable[waveform + variant][phase];
}

#endif // not VICE__WAVE_H__
