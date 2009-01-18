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
// C64 DTV modifications written by
//   Daniel Kahlin <daniel@kahlin.net>
// Copyright (C) 2007  Daniel Kahlin <daniel@kahlin.net>
//   Antti S. Lankila <alankila@bel.fi>
// Copyright (C) 2009  Antti S. Lankila <alankila@bel.fi>

#define __WAVE_CC__
#include "wave.h"

int WaveformGenerator::wave_train_lut[256][8];

void WaveformGenerator::init_train_lut() {
    for (int level = 0; level < 256; level ++) {
        for (int phase1 = 0; phase1 < 8; phase1 ++) {
            unsigned int counter = phase1 * 32;
            unsigned int train = 0;

            /* calculate wave train */
            for (int phase2 = 0; phase2 < 32; phase2 ++) {
                counter += level;
                train <<= 1;
                train |= counter >> 8;
                counter &= 0xff;
            }

            wave_train_lut[level][phase1] = train;
        }
    }
}


// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
WaveformGenerator::WaveformGenerator()
{
  static bool tableinit = false;
  if (! tableinit) {
    init_train_lut();
    tableinit = true;
  }

  sync_source = this;
  reset();
}


// ----------------------------------------------------------------------------
// Set sync source.
// ----------------------------------------------------------------------------
void WaveformGenerator::set_sync_source(WaveformGenerator* source)
{
  sync_source = source;
  source->sync_dest = this;
}

// ----------------------------------------------------------------------------
// Register functions.
// ----------------------------------------------------------------------------
void WaveformGenerator::writeFREQ_LO(reg8 freq_lo)
{
  freq = (freq & 0xff00) | (freq_lo & 0x00ff);
}

void WaveformGenerator::writeFREQ_HI(reg8 freq_hi)
{
  freq = ((freq_hi << 8) & 0xff00) | (freq & 0x00ff);
}

void WaveformGenerator::writePW_LO(reg8 pw_lo)
{
  pw = (pw & 0xf00) | (pw_lo & 0x0ff);
}

void WaveformGenerator::writePW_HI(reg8 pw_hi)
{
  pw = ((pw_hi << 8) & 0xf00) | (pw & 0x0ff);
}

void WaveformGenerator::writeCONTROL_REG(reg8 control)
{
  waveform = (control >> 4) & 0x0f;
  ring_mod = control & 0x04;
  sync = control & 0x02;
  test = control & 0x08;

  // Test bit set. Accumulator is cleared.
  if (test) {
    accumulator = 0;
  }

  // The gate bit is handled by the EnvelopeGenerator.
}

reg8 WaveformGenerator::readOSC()
{
  return outputN___();
}

/* LFSR is clocked if the bit 19 is incremented */
void WaveformGenerator::writeACC_HI(reg8 value)
{
  reg24 accumulator_prev = accumulator;
  accumulator = (value << 16) | (accumulator & 0xffff);
  if (!(accumulator_prev & 0x080000) && (accumulator & 0x080000)) {
    /* This has been measured to happen also with test bit on. */
    clock_noise();
  }
}

// ----------------------------------------------------------------------------
// SID reset.
// ----------------------------------------------------------------------------
void WaveformGenerator::reset()
{
  accumulator = 0;
  counter = 0;
  shift_register = 0x7ffffc;
  noise = 0xff;
  freq = 0;
  pw = 0;

  test = 0;
  ring_mod = 0;
  sync = 0;

  msb_rising = false;
}
