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

#ifndef __FILTER_H__
#define __FILTER_H__

#include "siddefs.h"

class Filter
{
public:
  Filter();

  void enable_filter(bool enable);

  void clock(sound_sample voice1, sound_sample voice2, sound_sample voice3);
  void clock(cycle_count delta_t,
  	     sound_sample voice1, sound_sample voice2, sound_sample voice3);
  void reset();

  // Write registers.
  void writeFC_LO(reg8);
  void writeFC_HI(reg8);
  void writeRES_FILT(reg8);
  void writeMODE_VOL(reg8);

  // SID audio output.
  // Approximate range [-2048*255*3*15 * 2, 2047*255*3*15 * 2]
  sound_sample output();

protected:
  void set_w0();
  void set_Q();

  // Filter enabled.
  bool enabled;

  // Filter cutoff frequency.
  reg12 fc;

  // Filter resonance.
  reg8 res;

  // External audio input routed through filter.
  // NB! Not modeled.
  bool filtex;

  // Voices routed through filter.
  bool filt3, filt2, filt1;

  // Switch voice 3 off.
  bool voice3off;

  // Highpass, bandpass, and lowpass filter modes.
  bool hp, bp, lp;
  sound_sample hp_bp_lp;

  // Output master volume.
  reg4 vol;

  // State of filter.
  sound_sample Vhp; // higpass
  sound_sample Vbp; // bandpass
  sound_sample Vlp; // lowpass
  sound_sample Vnf; // not filtered

  // Cutoff frequency, resonance.
  sound_sample _2_pi_w0;
  sound_sample _1024_div_Q;

  static double bsd_copysign(double x, double y);

friend class SID;
};


// ----------------------------------------------------------------------------
// Inline functions.
// The following functions are defined inline because they are called every
// time a sample is calculated.
// ----------------------------------------------------------------------------

#if RESID_INLINE || defined(__FILTER_CC__)

// ----------------------------------------------------------------------------
// SID clocking.
//
// The SID filter is modeled with a two-integrator-loop biquadratic filter,
// which is almost certainly the actual circuit used in the SID chip.
//
// Measurements show that excellent emulation of the SID filter is achieved,
// except for when high resonance is combined with high sustain levels.
// In this case op-amp clipping is suspected to be the cause of some peculiar
// behavior of the SID filter. This however seems to have more effect on the
// overall amplitude than on the color of the sound.
//
// The theory for the filter circuit can be found in "Microelectric Circuits"
// by Adel S. Sedra and Kenneth C. Smith.
// The circuit is modeled based on the explanation found there except for that
// n2 is set to 1 instead of 2 - 1/Q. Supposedly Vi is divided by n2
// before entering the filter circuit in the SID chip.
//
// We have been able to model the summer and the two integrators of the circuit
// to form components of an IIR filter.
// Vhp is the output of the summer, Vbp is the output of the first integrator,
// and Vlp is the output of the second integrator in the filter circuit.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// SID clocking - 1 cycle.
// ----------------------------------------------------------------------------
#if RESID_INLINE
inline
#endif
void Filter::clock(sound_sample voice1,
		   sound_sample voice2,
		   sound_sample voice3)
{
  // This is handy for testing.
  if (!enabled) {
    Vnf = voice1 + voice2 + (voice3off ? 0 : voice3);
    Vhp = Vbp = Vlp = 0;
    return;
  }

  // Route voices into or around filter.
  sound_sample Vi = 0;
  Vnf = 0;

  (filt1 ? Vi : Vnf) += voice1;
  (filt2 ? Vi : Vnf) += voice2;
  (filt3 ? Vi : Vnf) += (voice3off ? 0 : voice3);

  // delta_t is converted to seconds given a 1MHz clock by dividing
  // with 1 000 000. This is done in two operations to avoid integer
  // multiplication overflow.

  // Calculate filter outputs.
  sound_sample Vhp_next = (Vbp*_1024_div_Q >> 10) - Vlp + Vi;
  sound_sample Vbp_next = Vbp - (_2_pi_w0*(Vhp >> 7) >> 13);
  sound_sample Vlp_next = Vlp - (_2_pi_w0*(Vbp >> 7) >> 13);

  Vhp = Vhp_next;
  Vbp = Vbp_next;
  Vlp = Vlp_next;
}

// ----------------------------------------------------------------------------
// SID clocking - delta_t cycles.
// ----------------------------------------------------------------------------
#if RESID_INLINE
inline
#endif
void Filter::clock(cycle_count delta_t,
		   sound_sample voice1,
		   sound_sample voice2,
		   sound_sample voice3)
{
  // This is handy for testing.
  if (!enabled) {
    Vnf = voice1 + voice2 + (voice3off ? 0 : voice3);
    Vhp = Vbp = Vlp = 0;
    return;
  }

  // Route voices into or around filter.
  sound_sample Vi = 0;
  Vnf = 0;

  (filt1 ? Vi : Vnf) += voice1;
  (filt2 ? Vi : Vnf) += voice2;
  (filt3 ? Vi : Vnf) += (voice3off ? 0 : voice3);

  // delta_t is converted to seconds given a 1MHz clock by dividing
  // with 1 000 000. This is done in three operations to avoid integer
  // multiplication overflow.

  // Calculate filter outputs.
  sound_sample _2_pi_w0_delta_t = _2_pi_w0*delta_t >> 6;

  sound_sample Vhp_next = (Vbp*_1024_div_Q >> 10) - Vlp + Vi;
  sound_sample Vbp_next = Vbp - (_2_pi_w0_delta_t*(Vhp >> 7) >> 7);
  sound_sample Vlp_next = Vlp - (_2_pi_w0_delta_t*(Vbp >> 7) >> 7);

  Vhp = Vhp_next;
  Vbp = Vbp_next;
  Vlp = Vlp_next;
}


// ----------------------------------------------------------------------------
// SID audio output.
// Approximate range [-2048*255*3*15 * 2, 2047*255*3*15 * 2]
// ----------------------------------------------------------------------------
#if RESID_INLINE
inline
#endif
sound_sample Filter::output()
{
  const sound_sample Vmax = 2048*255*3;

  // Mix highpass, bandpass, and lowpass outputs.
  sound_sample Vf = 0;
  if (hp_bp_lp) {
    if (hp) Vf += Vhp;
    if (bp) Vf += Vbp;
    if (lp) Vf += Vlp;
    Vf /= hp_bp_lp;
  }

  // Sum non-filtered output, filtered output, and signal offset.
  // Multiply this sum with volume and normalize.
  return (Vnf + Vf + Vmax)*vol - Vmax*0x0f;
}

#endif // RESID_INLINE || defined(__FILTER_CC__)

#endif // not __FILTER_H__
