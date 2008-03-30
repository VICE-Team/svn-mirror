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

#ifndef __EXTFILT_H__
#define __EXTFILT_H__

#include "siddefs.h"

// ----------------------------------------------------------------------------
// The audio output stage in a Commodore 64 consists of two STC networks,
// a low-pass filter with 3-dB frequency 16kHz followed by a high-pass
// filter with 3-dB frequency 16Hz (the latter provided an audio equipment
// input impedance of 1kOhm).
// The STC networks are connected with a BJT supposedly meant to act as
// a unity gain buffer, which is not really how it works. A more elaborate
// model would include the BJT, however DC circuit analysis yields BJT
// base-emitter and emitter-base impedances sufficiently low to produce
// additional low-pass and high-pass 3dB-frequencies in the order of hundreds
// of kHz. This calls for a sampling frequency of several MHz, which is far
// too high for practical use.
// ----------------------------------------------------------------------------
class ExternalFilter
{
public:
  ExternalFilter();

  void enable_filter(bool enable);

  void clock(sound_sample Vi);
  void clock(cycle_count delta_t, sound_sample Vi);
  void reset();

  // Audio output (20 bits).
  sound_sample output();

protected:
  // Filter enabled.
  bool enabled;

  // State of filters.
  sound_sample Vlp; // lowpass
  sound_sample Vhp; // highpass
  sound_sample Vo;

  // Cutoff frequencies.
  sound_sample w0lp;
  sound_sample w0hp;

friend class SID;
};


// ----------------------------------------------------------------------------
// Inline functions.
// The following functions are defined inline because they are called every
// time a sample is calculated.
// ----------------------------------------------------------------------------

#if RESID_INLINE || defined(__EXTFILT_CC__)

// ----------------------------------------------------------------------------
// SID clocking - 1 cycle.
// ----------------------------------------------------------------------------
#if RESID_INLINE
inline
#endif
void ExternalFilter::clock(sound_sample Vi)
{
  // This is handy for testing.
  if (!enabled) {
    // Normalize output since there is no filter to do it.
    const sound_sample Vmax = 4096*3*0x0f;
    Vlp = Vhp = 0;
    Vo = Vi + Vmax;
    return;
  }

  // delta_t is converted to seconds given a 1MHz clock by dividing
  // with 1 000 000.

  // Calculate filter outputs.
  // Vo  = Vlp - Vhp;
  // Vlp = Vlp + w0lp*(Vi - Vlp)*delta_t;
  // Vhp = Vhp + w0hp*(Vlp - Vhp)*delta_t;

  sound_sample dVlp = (w0lp >> 8)*(Vi - Vlp) >> 12;
  sound_sample dVhp = w0hp*(Vlp - Vhp) >> 20;
  Vo = Vlp - Vhp;
  Vlp += dVlp;
  Vhp += dVhp;
}

// ----------------------------------------------------------------------------
// SID clocking - delta_t cycles.
// ----------------------------------------------------------------------------
#if RESID_INLINE
inline
#endif
void ExternalFilter::clock(cycle_count delta_t,
			   sound_sample Vi)
{
  // This is handy for testing.
  if (!enabled) {
    // Normalize output since there is no filter to do it.
    const sound_sample Vmax = 4096*3*0x0f;
    Vlp = Vhp = 0;
    Vo = Vi + Vmax;
    return;
  }

  // delta_t is converted to seconds given a 1MHz clock by dividing
  // with 1 000 000.

  // Calculate filter outputs.
  // Vo  = Vlp - Vhp;
  // Vlp = Vlp + w0lp*(Vi - Vlp)*delta_t;
  // Vhp = Vhp + w0hp*(Vlp - Vhp)*delta_t;

  sound_sample dVlp = (w0lp*delta_t >> 8)*(Vi - Vlp) >> 12;
  sound_sample dVhp = w0hp*delta_t*(Vlp - Vhp) >> 20;
  Vo = Vlp - Vhp;
  Vlp += dVlp;
  Vhp += dVhp;
}


// ----------------------------------------------------------------------------
// Audio output (19.5 bits).
// ----------------------------------------------------------------------------
#if RESID_INLINE
inline
#endif
sound_sample ExternalFilter::output()
{
  return Vo;
}

#endif // RESID_INLINE || defined(__EXTFILT_CC__)

#endif // not __EXTFILT_H__
