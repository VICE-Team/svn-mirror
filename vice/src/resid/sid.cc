//  ---------------------------------------------------------------------------
//  This file is part of reSID, a MOS6581 SID emulator engine.
//  Copyright (C) 1998  Dag Lem <resid@nimrod.no>
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

#include "sid.h"

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
SID::SID()
  : voice1(&voice3), voice2(&voice1), voice3(&voice2)
{
  voice[0] = &voice1;
  voice[1] = &voice2;
  voice[2] = &voice3;
}


// ----------------------------------------------------------------------------
// Set chip model.
// ----------------------------------------------------------------------------
void SID::set_chip_model(chip_model model)
{
  for (int i = 0; i < 3; i++) {
    voice[i]->wave.set_chip_model(model);
  }
}


// ----------------------------------------------------------------------------
// SID reset.
// ----------------------------------------------------------------------------
void SID::reset()
{
  voice1.reset();
  voice2.reset();
  voice3.reset();
  filter.reset();

  bus_value = 0;
}


// ----------------------------------------------------------------------------
// Read sample of audio output.
// Both 16-bit and n-bit output is provided.
// The output is inverted just like on a Commodore 64. This should not really
// make any audible difference.
// ----------------------------------------------------------------------------
int SID::output()
{
  return -filter.output()/(4095*255*3*15*2/65536);
}

int SID::output(int bits)
{
  return -filter.output()/(4095*255*3*15*2/(1 << bits));
}


// ----------------------------------------------------------------------------
// Read registers.
//
// Reading a write only register returns the last byte written to any SID
// register. The individual bits in this value start to fade down towards
// zero after a few cycles. All bits reach zero within approximately
// $2000 - $4000 cycles.
// NB! This is not modeled.
// It has been claimed that this fading happens in an orderly fashion, however
// sampling of write only registers reveals that this is not the case.
// The actual use of write only registers has largely been made in the belief
// that all SID registers are readable. To support this belief the read
// would have to be done immediately after a write to the same register
// (remember that an intermediate write to another register would yield that
// value instead). With this in mind we return the last value written to
// any SID register without modeling the bit fading.
// ----------------------------------------------------------------------------
reg8 SID::read(reg8 offset)
{
  switch (offset) {
  case 0x19:
    return potx.readPOT();
  case 0x1a:
    return poty.readPOT();
  case 0x1b:
    return voice3.wave.readOSC();
  case 0x1c:
    return voice3.envelope.readENV();
  default:
    return bus_value;
  }
}


// ----------------------------------------------------------------------------
// Write registers.
// ----------------------------------------------------------------------------
void SID::write(reg8 offset, reg8 value)
{
  bus_value = value;

  switch (offset) {
  case 0x00:
    voice1.wave.writeFREQ_LO(value);
    break;
  case 0x01:
    voice1.wave.writeFREQ_HI(value);
    break;
  case 0x02:
    voice1.wave.writePW_LO(value);
    break;
  case 0x03:
    voice1.wave.writePW_HI(value);
    break;
  case 0x04:
    voice1.writeCONTROL_REG(value);
    break;
  case 0x05:
    voice1.envelope.writeATTACK_DECAY(value);
    break;
  case 0x06:
    voice1.envelope.writeSUSTAIN_RELEASE(value);
    break;
  case 0x07:
    voice2.wave.writeFREQ_LO(value);
    break;
  case 0x08:
    voice2.wave.writeFREQ_HI(value);
    break;
  case 0x09:
    voice2.wave.writePW_LO(value);
    break;
  case 0x0a:
    voice2.wave.writePW_HI(value);
    break;
  case 0x0b:
    voice2.writeCONTROL_REG(value);
    break;
  case 0x0c:
    voice2.envelope.writeATTACK_DECAY(value);
    break;
  case 0x0d:
    voice2.envelope.writeSUSTAIN_RELEASE(value);
    break;
  case 0x0e:
    voice3.wave.writeFREQ_LO(value);
    break;
  case 0x0f:
    voice3.wave.writeFREQ_HI(value);
    break;
  case 0x10:
    voice3.wave.writePW_LO(value);
    break;
  case 0x11:
    voice3.wave.writePW_HI(value);
    break;
  case 0x12:
    voice3.writeCONTROL_REG(value);
    break;
  case 0x13:
    voice3.envelope.writeATTACK_DECAY(value);
    break;
  case 0x14:
    voice3.envelope.writeSUSTAIN_RELEASE(value);
    break;
  case 0x15:
    filter.writeFC_LO(value);
    break;
  case 0x16:
    filter.writeFC_HI(value);
    break;
  case 0x17:
    filter.writeRES_FILT(value);
    break;
  case 0x18:
    filter.writeMODE_VOL(value);
    break;
  default:
    break;
  }
}


// ----------------------------------------------------------------------------
// Enable filter.
// ----------------------------------------------------------------------------
void SID::enable_filter(bool enable)
{
  filter.enable_filter(enable);
}


// ----------------------------------------------------------------------------
// SID clocking - 1 cycle.
// ----------------------------------------------------------------------------
void SID::clock()
{
  int i;

  // Clock amplitude modulators.
  for (i = 0; i < 3; i++) {
    voice[i]->envelope.clock();
  }

  // Clock oscillators.
  for (i = 0; i < 3; i++) {
    voice[i]->wave.clock();
  }

  // Synchronize oscillators.
  for (i = 0; i < 3; i++) {
    voice[i]->wave.synchronize();
  }

  // Clock filter.
  filter.clock(voice1.output(), voice2.output(), voice3.output());
}


// ----------------------------------------------------------------------------
// SID clocking - delta_t cycles.
// ----------------------------------------------------------------------------
void SID::clock(cycle_count delta_t)
{
  int i;

  if (delta_t <= 0) {
    return;
  }

  // Clock filter.

  // Enable filter on/off.
  // This is not really part of SID, but is useful for testing.
  // On slow CPU's it may be necessary to bypass the filter to lower the CPU
  // load.
  // Maximum delta cycles for filter to work satisfactorily under current
  // cutoff frequency and resonance constraints is approximately 8.
  cycle_count delta_t_flt = filter.enabled ? 8 : delta_t;

  while (delta_t) {
    if (delta_t < delta_t_flt) {
      delta_t_flt = delta_t;
    }

    // Clock amplitude modulators.
    for (i = 0; i < 3; i++) {
      voice[i]->envelope.clock(delta_t_flt);
    }

    // Clock and synchronize oscillators.
    // Loop until we reach the current cycle.
    cycle_count delta_t_osc = delta_t_flt;
    while (delta_t_osc) {
      cycle_count delta_t_min = delta_t_osc;

      // Find minimum number of cycles to an oscillator accumulator MSB toggle.
      // We have to clock on each MSB on / MSB off for hard sync and ring
      // modulation to operate correctly.
      for (i = 0; i < 3; i++) {
	WaveformGenerator& wave = voice[i]->wave;

	// It is only necessary to clock on the MSB of an oscillator that has
	// freq != 0 and is a sync source.
	if (!(wave.freq && (wave.sync_dest->sync || wave.sync_dest->ring_mod)))
	{
	  continue;
	}

	reg16 freq = wave.freq;
	reg24 accumulator = wave.accumulator;

	// Clock on MSB off if MSB is on, clock on MSB on if MSB is off.
	reg24 delta_accumulator =
	  (accumulator & 0x800000 ? 0x1000000 : 0x800000) - accumulator;

	cycle_count delta_t_next = delta_accumulator/freq;
	if (delta_accumulator%freq) {
	  ++delta_t_next;
	}

	if (delta_t_next < delta_t_min) {
	  delta_t_min = delta_t_next;
	}
      }

      // Clock oscillators.
      for (i = 0; i < 3; i++) {
	voice[i]->wave.clock(delta_t_min);
      }

      // Synchronize oscillators.
      for (i = 0; i < 3; i++) {
	voice[i]->wave.synchronize();
      }

      delta_t_osc -= delta_t_min;
    }

    // Clock filter.
    filter.clock(delta_t_flt,
		 voice1.output(), voice2.output(), voice3.output());

    delta_t -= delta_t_flt;
  }
}
