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

#define __FILTER_CC__
#include "filter.h"
#include <math.h>

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
Filter::Filter()
{
  reset();
  enabled = true;
}


// ----------------------------------------------------------------------------
// Enable filter.
// ----------------------------------------------------------------------------
void Filter::enable_filter(bool enable)
{
  enabled = enable;
}


// ----------------------------------------------------------------------------
// SID reset.
// ----------------------------------------------------------------------------
void Filter::reset()
{
  fc = 0;

  res = 0;

  filtex = 0;

  filt3 = filt2 = filt1 = false;

  voice3off = false;

  hp = bp = lp = false;
  hp_bp_lp = 0;

  vol = 0;

  // State of filter.
  Vhp = 0;
  Vbp = 0;
  Vlp = 0;
  Vnf = 0;

  set_w0();
  set_Q();
}


// ----------------------------------------------------------------------------
// Register functions.
// ----------------------------------------------------------------------------
void Filter::writeFC_LO(reg8 fc_lo)
{
  fc = fc & 0x7f8 | fc_lo & 0x007;
  set_w0();
}

void Filter::writeFC_HI(reg8 fc_hi)
{
  fc = (fc_hi << 3) & 0x7f8 | fc & 0x007;
  set_w0();
}

void Filter::writeRES_FILT(reg8 res_filt)
{
  res = (res_filt >> 4) & 0x0f;
  set_Q();

  filtex = res_filt & 0x08;

  filt3 = res_filt & 0x04;
  filt2 = res_filt & 0x02;
  filt1 = res_filt & 0x01;
}

void Filter::writeMODE_VOL(reg8 mode_vol)
{
  voice3off = mode_vol & 0x80;

  hp = mode_vol & 0x40;
  bp = mode_vol & 0x20;
  lp = mode_vol & 0x10;
  hp_bp_lp = hp + bp + lp;

  vol = mode_vol & 0x0f;
}


// Non-POSIX copysign function.
inline double Filter::bsd_copysign(double x, double y)
{
  return x < 0 ? (y < 0 ? x : -x) : (y >= 0 ? x : -x);
}

// Set filter cutoff frequency.
void Filter::set_w0()
{
  // Maximum cutoff frequency is specified as
  // FCmax = 2.6e-5/C = 2.6e-5/2200e-12 = 11818.
  // Measurements indicate a much lower maximum frequency cutoff.
  // Based on measurements of bandpass output from a Commodore 64,
  // we use the tanh function to map fc to cutoff frequency.
  // Minimum cutoff frequency is approximately 230Hz, maximum cutoff
  // frequency is approximately 4KHz.
  // NB! Cutoff frequency characteristics varies a lot, we have modeled
  // one particular Commodore 64.
  static const double pi = 3.1415926535897932385;

  double x = fc - 920.0;
  double w0 =
    228 + 3900/2*(1 + tanh(bsd_copysign(pow(fabs(x), 0.85)/95, x)));

  _2_pi_w0 = sound_sample(2*pi*w0*1.048576);
}

// Set filter resonance.
void Filter::set_Q()
{
  // Q is controlled linearly by res. Q has approximate range [0.707, 1.7].
  // n2 should have been calculated as 2 - 1/Q, however it seems like Vi
  // is divided by n2 before entering the filter circuit.
  // As resonance is increased, the filter must be clocked more often to keep
  // stable.
  // NB! To avoid even higher demands on the CPU, Q is limited to 1.3.
  // Q = 0.707 + 1.0*res/0x0f;
  _1024_div_Q = sound_sample(1024.0/(0.707 + 0.6*res/0x0f));
}
