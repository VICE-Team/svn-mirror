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

#ifndef __VOICE_H__
#define __VOICE_H__

#include "siddefs.h"
#include "wave.h"
#include "envelope.h"

class Voice
{
public:
  // Constructor parameter is sync_source.
  Voice(Voice*);

  void reset();

  void writeCONTROL_REG(reg8);

  // Amplitude modulated waveform output.
  // Range [-2048*255, 2047*255].
  sound_sample output();

protected:
  WaveformGenerator wave;
  EnvelopeGenerator envelope;

friend class SID;
};


// ----------------------------------------------------------------------------
// Inline functions.
// The following function is defined inline because it is called every
// time a sample is calculated.
// ----------------------------------------------------------------------------

#if RESID_INLINE || defined(__VOICE_CC__)

// ----------------------------------------------------------------------------
// Amplitude modulated waveform output.
// Range [-2048*255, 2047*255].
// ----------------------------------------------------------------------------
#if RESID_INLINE
inline
#endif
sound_sample Voice::output()
{
  // Multiply oscillator output with envelope output.
  return (wave.output() - 2048)*envelope.output();
}

#endif // RESID_INLINE || defined(__VOICE_CC__)

#endif // not __VOICE_H__
