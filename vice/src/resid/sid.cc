//  ---------------------------------------------------------------------------
//  This file is part of reSID, a MOS6581 SID emulator engine.
//  Copyright (C) 1999  Dag Lem <resid@nimrod.no>
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
  extfilt.reset();

  bus_value = 0;
  bus_value_ttl = 0;
}


// ----------------------------------------------------------------------------
// Read sample of audio output.
// Both 16-bit and n-bit output is provided.
// ----------------------------------------------------------------------------
int SID::output()
{
  return extfilt.output()/(8192*3*15*2/65536);
}

int SID::output(int bits)
{
  return extfilt.output()/(8192*3*15*2/(1 << bits));
}


// ----------------------------------------------------------------------------
// Read registers.
//
// Reading a write only register returns the last byte written to any SID
// register. The individual bits in this value start to fade down towards
// zero after a few cycles. All bits reach zero within approximately
// $2000 - $4000 cycles.
// It has been claimed that this fading happens in an orderly fashion, however
// sampling of write only registers reveals that this is not the case.
// NB! This is not correctly modeled.
// The actual use of write only registers has largely been made in the belief
// that all SID registers are readable. To support this belief the read
// would have to be done immediately after a write to the same register
// (remember that an intermediate write to another register would yield that
// value instead). With this in mind we return the last value written to
// any SID register for $2000 cycles without modeling the bit fading.
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
  bus_value_ttl = 0x2000;

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
// Constructor.
// ----------------------------------------------------------------------------
SID::State::State()
{
  int i;

  for (i = 0; i < 0x20; i++) {
    sid_register[i] = 0;
  }

  bus_value = 0;
  bus_value_ttl = 0;

  for (i = 0; i < 3; i++) {
    accumulator[i] = 0;
    shift_register[i] = 0;
    rate_counter[i] = 0;
    exponential_counter[i] = 0;
    envelope_counter[i] = 0;
    hold_zero[i] = 0;
  }

  Vhp = 0;
  Vbp = 0;
  Vlp = 0;

  extVhp = 0;
  extVlp = 0;
}


// ----------------------------------------------------------------------------
// Read state.
// ----------------------------------------------------------------------------
SID::State SID::read_state()
{
  State state;
  int i, j;

  for (i = 0, j = 0; i < 3; i++, j += 7) {
    WaveformGenerator& wave = voice[i]->wave;
    EnvelopeGenerator& envelope = voice[i]->envelope;
    state.sid_register[j + 0] = wave.freq & 0xff;
    state.sid_register[j + 1] = wave.freq >> 8;
    state.sid_register[j + 2] = wave.pw & 0xff;
    state.sid_register[j + 3] = wave.pw >> 8;
    state.sid_register[j + 4] =
      (wave.waveform << 4)
      | (wave.test ? 0x08 : 0)
      | (wave.ring_mod ? 0x04 : 0)
      | (wave.sync ? 0x02 : 0)
      | (envelope.gate ? 0x01 : 0);
    state.sid_register[j + 5] = (envelope.attack << 4) | envelope.decay;
    state.sid_register[j + 6] = (envelope.decay << 4) | envelope.release;
  }

  state.sid_register[j++] = filter.fc & 0x007;
  state.sid_register[j++] = filter.fc >> 3;
  state.sid_register[j++] =
    (filter.res << 4)
    | (filter.filtex ? 0x08 : 0)
    | filter.filt3_filt2_filt1;
  state.sid_register[j++] =
    (filter.voice3off ? 0x80 : 0)
    | (filter.hp_bp_lp << 4)
    | filter.vol;

  // These registers are superfluous, but included for completeness.
  for (; j < 0x1d; j++) {
    state.sid_register[j] = read(j);
  }
  for (; j < 0x20; j++) {
    state.sid_register[j] = 0;
  }

  state.bus_value = bus_value;
  state.bus_value_ttl = bus_value_ttl;

  for (i = 0; i < 3; i++) {
    state.accumulator[i] = voice[i]->wave.accumulator;
    state.shift_register[i] = voice[i]->wave.shift_register;
    state.rate_counter[i] = voice[i]->envelope.rate_counter;
    state.exponential_counter[i] = voice[i]->envelope.exponential_counter;
    state.envelope_counter[i] = voice[i]->envelope.envelope_counter;
    state.hold_zero[i] = voice[i]->envelope.hold_zero;
  }

  state.Vhp = filter.Vhp;
  state.Vbp = filter.Vbp;
  state.Vlp = filter.Vlp;

  state.extVhp = extfilt.Vhp;
  state.extVlp = extfilt.Vlp;

  return state;
}


// ----------------------------------------------------------------------------
// Write state.
// ----------------------------------------------------------------------------
void SID::write_state(const State& state)
{
  int i;

  for (i = 0; i < 0x18; i++) {
    write(i, state.sid_register[i]);
  }

  bus_value = state.bus_value;
  bus_value_ttl = state.bus_value_ttl;

  for (i = 0; i < 3; i++) {
    voice[i]->wave.accumulator = state.accumulator[i];
    voice[i]->wave.shift_register = state.shift_register[i];
    voice[i]->envelope.rate_counter = state.rate_counter[i];
    voice[i]->envelope.exponential_counter = state.exponential_counter[i];
    voice[i]->envelope.envelope_counter = state.envelope_counter[i];
    voice[i]->envelope.hold_zero = state.hold_zero[i];
  }

  filter.Vhp = state.Vhp;
  filter.Vbp = state.Vbp;
  filter.Vlp = state.Vlp;

  extfilt.Vhp = state.extVhp;
  extfilt.Vlp = state.extVlp;
}


// ----------------------------------------------------------------------------
// Enable filter.
// ----------------------------------------------------------------------------
void SID::enable_filter(bool enable)
{
  filter.enable_filter(enable);
}


// ----------------------------------------------------------------------------
// Enable external filter.
// ----------------------------------------------------------------------------
void SID::enable_external_filter(bool enable)
{
  extfilt.enable_filter(enable);
}


// ----------------------------------------------------------------------------
// Return array of default spline interpolation points to map FC to
// filter cutoff frequency.
// ----------------------------------------------------------------------------
void SID::fc_default(const fc_point*& points, int& count)
{
  filter.fc_default(points, count);
}


// ----------------------------------------------------------------------------
// Return FC spline plotter object.
// ----------------------------------------------------------------------------
PointPlotter<sound_sample> SID::fc_plotter()
{
  return filter.fc_plotter();
}


// ----------------------------------------------------------------------------
// SID clocking - 1 cycle.
// ----------------------------------------------------------------------------
void SID::clock()
{
  int i;

  // Age bus value.
  if (--bus_value_ttl <= 0) {
    bus_value = 0;
    bus_value_ttl = 0;
  }

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

  // Clock external filter.
  extfilt.clock(filter.output());
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

  // Age bus value.
  bus_value_ttl -= delta_t;
  if (bus_value_ttl <= 0) {
    bus_value = 0;
    bus_value_ttl = 0;
  }

  // Clock filter.

  // Enable filter on/off.
  // This is not really part of SID, but is useful for testing.
  // On slow CPU's it may be necessary to bypass the filter to lower the CPU
  // load.
  // Maximum delta cycles for filters to work satisfactorily under current
  // cutoff frequency and resonance constraints is approximately 8.
  cycle_count delta_t_flt = (filter.enabled || extfilt.enabled) ? 8 : delta_t;

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
      // We have to clock on each MSB on / MSB off for hard sync to operate
      // correctly.
      for (i = 0; i < 3; i++) {
	WaveformGenerator& wave = voice[i]->wave;

	// It is only necessary to clock on the MSB of an oscillator that is
	// a sync source and has freq != 0.
	if (!(wave.sync_dest->sync && wave.freq)) {
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

    // Clock external filter.
    extfilt.clock(delta_t_flt, filter.output());

    delta_t -= delta_t_flt;
  }
}
