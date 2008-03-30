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

#ifndef __FILTER_H__
#define __FILTER_H__

#include "siddefs.h"
#include "spline.h"

// ----------------------------------------------------------------------------
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

  // SID audio output (16 bits).
  sound_sample output();

  // Spline functions.
  void fc_default(const fc_point*& points, int& count);
  PointPlotter<sound_sample> fc_plotter();

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
  reg8 filt3_filt2_filt1;

  // Switch voice 3 off.
  bool voice3off;

  // Highpass, bandpass, and lowpass filter modes.
  reg8 hp_bp_lp;

  // Output master volume.
  reg4 vol;

  // State of filter.
  sound_sample Vhp; // highpass
  sound_sample Vbp; // bandpass
  sound_sample Vlp; // lowpass
  sound_sample Vnf; // not filtered

  // Cutoff frequency, resonance.
  sound_sample w0;
  sound_sample _1024_div_Q;

  // Cutoff frequency table.
  // FC is an 11 bit register.
  sound_sample f0[2048];
  static fc_point f0_points[];

friend class SID;
};


// ----------------------------------------------------------------------------
// Inline functions.
// The following functions are defined inline because they are called every
// time a sample is calculated.
// ----------------------------------------------------------------------------

#if RESID_INLINE || defined(__FILTER_CC__)

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
  // Scale each voice down from 20 to 13 bits.
  voice1 >>= 7;
  voice2 >>= 7;
  if (voice3off) {
    voice3 = 0;
  }
  else {
    voice3 >>= 7;
  }

  // This is handy for testing.
  if (!enabled) {
    Vnf = voice1 + voice2 + voice3;
    Vhp = Vbp = Vlp = 0;
    return;
  }

  // Route voices into or around filter.
  // The code below is expanded to a switch for faster execution.
  // (filt1 ? Vi : Vnf) += voice1;
  // (filt2 ? Vi : Vnf) += voice2;
  // (filt3 ? Vi : Vnf) += voice3;

  sound_sample Vi;

  switch (filt3_filt2_filt1) {
  default:
  case 0x0:
    Vi = 0;
    Vnf = voice1 + voice2 + voice3;
    break;
  case 0x1:
    Vi = voice1;
    Vnf = voice2 + voice3;
    break;
  case 0x2:
    Vi = voice2;
    Vnf = voice1 + voice3;
    break;
  case 0x3:
    Vi = voice1 + voice2;
    Vnf = voice3;
    break;
  case 0x4:
    Vi = voice3;
    Vnf = voice1 + voice2;
    break;
  case 0x5:
    Vi = voice1 + voice3;
    Vnf = voice2;
    break;
  case 0x6:
    Vi = voice2 + voice3;
    Vnf = voice1;
    break;
  case 0x7:
    Vi = voice1 + voice2 + voice3;
    Vnf = 0;
    break;
  }
    
  // delta_t = 1 is converted to seconds given a 1MHz clock by dividing
  // with 1 000 000.

  // Calculate filter outputs.
  // Vhp = Vbp/Q - Vlp + Vi;
  // dVbp = -w0*Vhp*dt;
  // dVlp = -w0*Vbp*dt;

  sound_sample dVbp = (w0*Vhp >> 20);
  sound_sample dVlp = (w0*Vbp >> 20);
  Vbp -= dVbp;
  Vlp -= dVlp;
  Vhp = (Vbp*_1024_div_Q >> 10) - Vlp + Vi;
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
  // Scale each voice down from 20 to 13 bits.
  voice1 >>= 7;
  voice2 >>= 7;
  if (voice3off) {
    voice3 = 0;
  }
  else {
    voice3 >>= 7;
  }

  // This is handy for testing.
  if (!enabled) {
    Vnf = voice1 + voice2 + voice3;
    Vhp = Vbp = Vlp = 0;
    return;
  }

  // Route voices into or around filter.
  // The code below is expanded to a switch for faster execution.
  // (filt1 ? Vi : Vnf) += voice1;
  // (filt2 ? Vi : Vnf) += voice2;
  // (filt3 ? Vi : Vnf) += voice3;

  sound_sample Vi;

  switch (filt3_filt2_filt1) {
  default:
  case 0x0:
    Vi = 0;
    Vnf = voice1 + voice2 + voice3;
    break;
  case 0x1:
    Vi = voice1;
    Vnf = voice2 + voice3;
    break;
  case 0x2:
    Vi = voice2;
    Vnf = voice1 + voice3;
    break;
  case 0x3:
    Vi = voice1 + voice2;
    Vnf = voice3;
    break;
  case 0x4:
    Vi = voice3;
    Vnf = voice1 + voice2;
    break;
  case 0x5:
    Vi = voice1 + voice3;
    Vnf = voice2;
    break;
  case 0x6:
    Vi = voice2 + voice3;
    Vnf = voice1;
    break;
  case 0x7:
    Vi = voice1 + voice2 + voice3;
    Vnf = 0;
    break;
  }

  // delta_t is converted to seconds given a 1MHz clock by dividing
  // with 1 000 000. This is done in two operations to avoid integer
  // multiplication overflow.

  // Calculate filter outputs.
  // Vhp = Vbp/Q - Vlp + Vi;
  // dVbp = -w0*Vhp*dt;
  // dVlp = -w0*Vbp*dt;
  sound_sample w0_delta_t = w0*delta_t >> 6;

  sound_sample dVbp = (w0_delta_t*Vhp >> 14);
  sound_sample dVlp = (w0_delta_t*Vbp >> 14);
  Vbp -= dVbp;
  Vlp -= dVlp;
  Vhp = (Vbp*_1024_div_Q >> 10) + Vi - Vlp;
}


// ----------------------------------------------------------------------------
// SID audio output (20 bits).
// ----------------------------------------------------------------------------
#if RESID_INLINE
inline
#endif
sound_sample Filter::output()
{
  const sound_sample Vmax = 4096*3;

  // This is handy for testing.
  if (!enabled) {
    return -((Vnf + Vmax)*vol);
  }

  // Mix highpass, bandpass, and lowpass outputs.
  // The code below is expanded to a switch for faster execution.
  // if (hp_bp_lp) {
  //   if (hp) Vf += Vhp;
  //   if (bp) Vf += Vbp;
  //   if (lp) Vf += Vlp;
  //   Vf /= hp_bp_lp;
  // }

  sound_sample Vf;

  switch (hp_bp_lp) {
  default:
  case 0x0:
    Vf = 0;
    break;
  case 0x1:
    Vf = Vlp;
    break;
  case 0x2:
    Vf = Vbp;
    break;
  case 0x3:
    Vf = (Vlp + Vbp) >> 1;
    break;
  case 0x4:
    Vf = Vhp;
    break;
  case 0x5:
    Vf = (Vlp + Vhp) >> 1;
    break;
  case 0x6:
    Vf = (Vbp + Vhp) >> 1;
    break;
  case 0x7:
    Vf = (Vlp + Vbp + Vhp)/3;
    break;
  }

  // Sum non-filtered output, filtered output, and signal offset.
  // The filter output is inverted compared to the non-filtered output.
  // Multiply the sum with volume.
  // The output is inverted. This should not make any audible difference,
  // but is included for correctness.
  return -((Vnf - Vf + Vmax)*vol);
}

#endif // RESID_INLINE || defined(__FILTER_CC__)

#endif // not __FILTER_H__
