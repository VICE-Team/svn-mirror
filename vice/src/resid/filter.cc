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

#define __FILTER_CC__
#include "filter.h"

fc_point Filter::f0_points[] =
{
  {    0,  230 },
  {  384,  278 },
  {  640,  500 },
  {  768,  900 },
  {  896, 1950 },
  { 1024, 3200 },
  { 1152, 3750 },
  { 1280, 3980 },
  { 1536, 4080 },
  { 2047, 4100 }
};


// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
Filter::Filter()
{
  int i_max = sizeof(f0_points)/sizeof(*f0_points) - 1;
  interpolate(f0_points, f0_points, f0_points + i_max, f0_points + i_max,
	      PointPlotter<sound_sample>(f0), 1.0);

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

  filtex = false;

  filt3_filt2_filt1 = 0;

  voice3off = false;

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
  filt3_filt2_filt1 = res_filt & 0x07;
}

void Filter::writeMODE_VOL(reg8 mode_vol)
{
  voice3off = mode_vol & 0x80;

  hp_bp_lp = (mode_vol >> 4) & 0x07;

  vol = mode_vol & 0x0f;
}

// Set filter cutoff frequency.
void Filter::set_w0()
{
  // Maximum cutoff frequency is specified as
  // FCmax = 2.6e-5/C = 2.6e-5/2200e-12 = 11818.
  // Measurements indicate a much lower maximum frequency cutoff.
  // Measurements of bandpass output from a Commodore 64 shows that the
  // function mapping fc to cutoff frequency has the shape of the tanh
  // function. The mapping function is specified with spline interpolation
  // points and the function values are retrieved via table lookup.
  // Minimum cutoff frequency is approximately 230Hz, maximum cutoff
  // frequency is approximately 4KHz.
  // NB! Cutoff frequency characteristics varies a lot, we have modeled
  // one particular Commodore 64.
  const double pi = 3.1415926535897932385;

  // Multiply with 1.048576 to facilitate division by 1 000 000 by right-
  // shifting 20 times (2 ^ 20 = 1048576).
  w0 = sound_sample(2*pi*f0[fc]*1.048576);
}

// Set filter resonance.
void Filter::set_Q()
{
  // Q is controlled linearly by res. Q has approximate range [0.707, 1.7].
  // n2 should have been calculated as 2 - 1/Q, however it seems like Vi
  // is divided by n2 before entering the filter circuit.
  // As resonance is increased, the filter must be clocked more often to keep
  // stable.

  // The coefficient 1024 is dispensed of later by right-shifting 10 times
  // (2 ^ 10 = 1024).
  _1024_div_Q = sound_sample(1024.0/(0.707 + 1.0*res/0x0f));
}

// ----------------------------------------------------------------------------
// Spline functions.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Return the array of spline interpolation points used to map the FC register
// to filter cutoff frequency.
// ----------------------------------------------------------------------------
void Filter::fc_default(const fc_point*& points, int& count)
{
  points = f0_points;
  count = sizeof(f0_points)/sizeof(*f0_points);
}

// ----------------------------------------------------------------------------
// Given an array of interpolation points p with n points, the following
// statement will specify a new FC mapping:
//   interpolate(p, p, p + n - 1, p + n - 1, filter.fc_plotter());
// Note that the x range of the interpolation points *must* be [0, 2047].
// ----------------------------------------------------------------------------
PointPlotter<sound_sample> Filter::fc_plotter()
{
  return PointPlotter<sound_sample>(f0);
}
