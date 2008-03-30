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

#ifndef __SID_H__
#define __SID_H__

#include "siddefs.h"
#include "voice.h"
#include "filter.h"
#include "extfilt.h"
#include "pot.h"

class SID
{
public:
  SID();

  void set_chip_model(chip_model model);
  void enable_filter(bool enable);
  void enable_external_filter(bool enable);

  void fc_default(const fc_point*& points, int& count);
  PointPlotter<sound_sample> fc_plotter();

  void clock();
  void clock(cycle_count delta_t);
  void reset();
  
  // Read/write registers.
  reg8 read(reg8 offset);
  void write(reg8 offset, reg8 value);

  // Read/write state.
  class State
  {
  public:
    State();

    char sid_register[0x20];

    reg8 bus_value;
    cycle_count bus_value_ttl;

    reg24 accumulator[3];
    reg24 shift_register[3];
    reg16 rate_counter[3];
    reg16 exponential_counter[3];
    reg8 envelope_counter[3];
    bool hold_zero[3];
    
    sound_sample Vhp;
    sound_sample Vbp;
    sound_sample Vlp;

    sound_sample extVhp;
    sound_sample extVlp;
  };
    
  State read_state();
  void write_state(const State& state);

  // 16-bit output.
  int output();
  // n-bit output.
  int output(int bits);

protected:
  Voice voice1;
  Voice voice2;
  Voice voice3;
  Filter filter;
  ExternalFilter extfilt;
  Potentiometer potx;
  Potentiometer poty;

  reg8 bus_value;
  cycle_count bus_value_ttl;
  Voice* voice[3];
};

#endif // not __SID_H__
