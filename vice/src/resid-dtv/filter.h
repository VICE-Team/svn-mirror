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

#ifndef __FILTER_H__
#define __FILTER_H__

#include "siddefs.h"

class Filter
{
public:
  Filter();

  RESID_INLINE
  void clock(sound_sample voice1, sound_sample voice2, sound_sample voice3);
  void reset();

  // Write registers.
  void writeFC_LO(reg8);
  void writeFC_HI(reg8);
  void writeRES_FILT(reg8);
  void writeMODE_VOL(reg8);

  // SID audio output (16 bits).
  sound_sample output();

protected:
  // Filter cutoff frequency.
  reg12 fc;

  // Filter resonance.
  reg8 res;

  // Selects which inputs to route through filter.
  reg8 filt;

  // Switch voice 3 off.
  reg8 voice3off;

  // Highpass, bandpass, and lowpass filter modes.
  reg8 hp_bp_lp;

  // Output master volume.
  reg4 vol;

  // Temporary state of filter.
  sound_sample Vnf;

friend class SID;
};


// ----------------------------------------------------------------------------
// Inline functions.
// The following functions are defined inline because they are called every
// time a sample is calculated.
// ----------------------------------------------------------------------------

#if RESID_INLINING || defined(__FILTER_CC__)

// ----------------------------------------------------------------------------
// SID clocking - 1 cycle.
// ----------------------------------------------------------------------------
RESID_INLINE
void Filter::clock(sound_sample voice1,
		   sound_sample voice2,
		   sound_sample voice3)
{
  // NB! Voice 3 is not silenced by voice3off if it is routed through
  // the filter.
  if (voice3off && !(filt & 0x04)) {
    voice3 = 0;
  }
  
  Vnf = (voice1 + voice2 + voice3) << 9;
}

// ----------------------------------------------------------------------------
// SID audio output (20 bits).
// ----------------------------------------------------------------------------
RESID_INLINE
sound_sample Filter::output()
{
  return Vnf + (static_cast<sound_sample>(vol) << 10);
}

#endif // RESID_INLINING || defined(__FILTER_CC__)

#endif // not __FILTER_H__
