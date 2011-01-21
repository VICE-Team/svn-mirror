//  ---------------------------------------------------------------------------
//  This file is part of reSID, a MOS6581 SID emulator engine.
//  Copyright (C) 2010  Dag Lem <resid@nimrod.no>
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

#ifndef RESID_FILTER_H
#define RESID_FILTER_H

#include "siddefs.h"
#include <math.h>

#ifndef HAVE_SQRTF_PROTOTYPE
extern float sqrtf(float val);
#endif
 
#ifndef HAVE_SQRTF
static float sqrtf(float val)
{
    return (float)sqrt((double)val);
}
#endif

namespace reSID
{

// ----------------------------------------------------------------------------
// The SID filter is modeled with a two-integrator-loop biquadratic filter,
// which has been confirmed by Bob Yannes to be the actual circuit used in
// the SID chip.
//
// Measurements show that excellent emulation of the SID filter is achieved,
// except when high resonance is combined with high sustain levels.
// In this case the SID op-amps are performing less than ideally and are
// causing some peculiar behavior of the SID filter. This however seems to
// have more effect on the overall amplitude than on the color of the sound.
//
// The theory for the filter circuit can be found in "Microelectric Circuits"
// by Adel S. Sedra and Kenneth C. Smith.
// The circuit is modeled based on the explanation found there except that
// an additional inverter is used in the feedback from the bandpass output,
// allowing the summer op-amp to operate in single-ended mode. This yields
// filter outputs with levels independent of Q, which corresponds with the
// results obtained from a real SID.
//
// We have been able to model the summer and the two integrators of the circuit
// to form components of an IIR filter.
// Vhp is the output of the summer, Vbp is the output of the first integrator,
// and Vlp is the output of the second integrator in the filter circuit.
//
// According to Bob Yannes, the active stages of the SID filter are not really
// op-amps. Rather, simple NMOS inverters are used. By biasing an inverter
// into its region of quasi-linear operation using a feedback resistor from
// input to output, a MOS inverter can be made to act like an op-amp for
// small signals centered around the switching threshold.
//
// In 2008, Michael Huth facilitated closer investigation of the SID 6581
// filter circuit by publishing high quality microscope photographs of the die.
// Tommi Lempinen has done an impressive work on re-vectorizing and annotating
// the die photographs, substantially simplifying further analysis of the
// filter circuit.
// 
// The filter schematics below are reverse engineered from these re-vectorized
// and annotated die photographs. While the filter first depicted in reSID 0.9
// is a correct model of the basic filter, the schematics are now completed
// with the audio mixer and output stage, including details on intended
// relative resistor values. Also included are schematics for the NMOS FET
// voltage controlled resistors (VCRs) used to control cutoff frequency, the
// DAC which controls the VCRs, the NMOS op-amps, and the output buffer.
//
//
// SID filter / mixer / output
// ---------------------------
// 
//                ---------------------------------------------------
//               |                                                   |
//               |                         --1R1-- \--  D7           |
//               |              ---R1--   |           |              |
//               |             |       |  |--2R1-- \--| D6           |
//               |    ------------<A]-----|           |     $17      |
//               |   |                    |--4R1-- \--| D5  1=open   | (3.5R1)
//               |   |                    |           |              |
//               |   |                     --8R1-- \--| D4           | (7.0R1)
//               |   |                                |              |
// $17           |   |                    (CAP2B)     |  (CAP1B)     |
// 0=to mixer    |    --R8--    ---R8--        ---C---|       ---C---| 
// 1=to filter   |          |  |       |      |       |      |       |
//                ------R8--|-----[A>--|--Rw-----[A>--|--Rw-----[A>--|
//     ve (EXT IN)          |          |              |              |
// D3  \ ---------------R8--|          |              | (CAP2A)      | (CAP1A)
//     |   v3               |          | vhp          | vbp          | vlp
// D2  |   \ -----------R8--|     -----               |              |
//     |   |   v2           |    |                    |              |
// D1  |   |   \ -------R8--|    |    ----------------               |
//     |   |   |   v1       |    |   |                               |
// D0  |   |   |   \ ---R8--     |   |    ---------------------------
//     |   |   |   |             |   |   |
//     R6  R6  R6  R6            R6  R6  R6
//     |   |   |   | $18         |   |   |  $18
//     |    \  |   | D7: 1=open   \   \   \ D6 - D4: 0=open
//     |   |   |   |             |   |   |
//      ---------------------------------                          12V
//                 |
//                 |               D3  --/ --1R2--                  |
//                 |    ---R8--       |           |   ---R2--       |
//                 |   |       |   D2 |--/ --2R2--|  |       |  ||--
//                  ------[A>---------|           |-----[A>-----||
//                                 D1 |--/ --4R2--| (4.25R2)    ||--
//                        $18         |           |                 |
//                        0=open   D0  --/ --8R2--  (8.75R2)        |
//
//                                                                  vo (AUDIO
//                                                                      OUT)
//
//
// v1  - voice 1
// v2  - voice 2
// v3  - voice 3
// ve  - ext in
// vhp - highpass output
// vbp - bandpass output
// vlp - lowpass output
// vo  - audio out
// [A> - single ended inverting op-amp (self-biased NMOS inverter)
// Rn  - "resistors", implemented with custom NMOS FETs
// Rw  - cutoff frequency resistor (VCR)
// C   - capacitor
//
// Notes:
//
// R2  ~  2.0*R1
// R6  ~  6.0*R1
// R8  ~  8.0*R1
// R24 ~ 24.0*R1
//
// The Rn "resistors" in the circuit are implemented with custom NMOS FETs,
// probably because of space constraints on the SID die. The silicon substrate
// is laid out in a narrow strip or "snake", with a strip length proportional
// to the intended resistance. The polysilicon gate electrode covers the entire
// silicon substrate and is fixed at 12V in order for the NMOS FET to operate
// in triode mode (a.k.a. linear mode or ohmic mode).
//
// Even in "linear mode", an NMOS FET is only an approximation of a resistor,
// as the apparant resistance increases with increasing drain-to-source
// voltage. If the drain-to-source voltage should approach the gate voltage
// of 12V, the NMOS FET will enter saturation mode (a.k.a. active mode), and
// the NMOS FET will not operate anywhere like a resistor.
//
// 
// 
// NMOS FET voltage controlled resistor (VCR)
// ------------------------------------------
//
//                Vw
//
//                |
//                |
//                R1
//                |
//          --R1--|
//         |    __|__
//         |    -----
//         |    |   |
// vi ----------     -------- vo
//         |           |
//          ----R24----
//
//
// vi  - input
// vo  - output
// Rn  - "resistors", implemented with custom NMOS FETs
// Vw  - voltage from 11-bit DAC (frequency cutoff control)
// 
// Notes:
//
// An approximate value for R24 can be found by using the formula for the
// filter cutoff frequency:
//
// FCmin = 1/(2*pi*Rmax*C)
//
// Assuming that a the setting for minimum cutoff frequency in combination with
// a low level input signal ensures that only negligible current will flow
// through the transistor in the schematics above, values for FCmin and C can
// be substituted in this formula to find Rmax.
// Using C = 470pF and FCmin = 220Hz (measured value), we get:
//
// FCmin = 1/(2*pi*Rmax*C)
// Rmax = 1/(2*pi*FCmin*C) = 1/(2*pi*220*470e-12) ~ 1.5MOhm
//
// From this it follows that:
// R24 =  Rmax   ~ 1.5MOhm
// R1  ~  R24/24 ~  64kOhm
// R2  ~  2.0*R1 ~ 128kOhm
// R6  ~  6.0*R1 ~ 384kOhm
// R8  ~  8.0*R1 ~ 512kOhm
//
// Note that these are only approximate values for one particular SID chip,
// due to process variations the values can be substantially different in
// other chips.
// 
// 
// 
// Filter frequency cutoff DAC
// ---------------------------
//
//
//        12V  10   9   8   7   6   5   4   3   2   1   0   VGND
//          |   |   |   |   |   |   |   |   |   |   |   |     |   Missing
//         2R  2R  2R  2R  2R  2R  2R  2R  2R  2R  2R  2R    2R   termination
//          |   |   |   |   |   |   |   |   |   |   |   |     |
//     Vw ----R---R---R---R---R---R---R---R---R---R---R--   ---
//
// Bit on:  12V
// Bit off:  5V (VGND)
//
// As is the case with all MOS 6581 DACs, the termination to (virtual) ground
// at bit 0 is missing.
//
// Furthermore, the control of the two VCRs imposes a load on the DAC output
// which varies with the input signals to the VCRs. This can be seen from the
// VCR figure above.
//
// 
// 
// "Op-amp" (self-biased NMOS inverter)
// ------------------------------------
//                  
//                  
//                        12V
//
//                         |
//              -----------|
//             |           |
//             |     ------|
//             |    |      |
//             |    |  ||--
//             |     --||
//             |       ||--
//         ||--            |
// vi -----||              |--------- vo
//         ||--            |   |
//             |       ||--    |
//             |-------||      |
//             |       ||--    |
//         ||--            |   |
//       --||              |   |
//      |  ||--            |   |
//      |      |           |   |
//      |       -----------|   |
//      |                  |   |
//      |                      |
//      |                 GND  |
//      |                      |
//       ----------------------
//
//
// vi  - input
// vo  - output
//
// Notes:
//
// The schematics above are laid out to show that the "op-amp" logically
// consists of two building blocks; a saturated load NMOS inverter (on the
// right hand side of the schematics) with a buffer / bias input stage
// consisting of a variable saturated load NMOS inverter (on the left hand
// side of the schematics).
//
// Provided a reasonably high input impedance and a reasonably low output
// impedance, the "op-amp" can be modeled as a voltage transfer function
// mapping input voltage to output voltage.
//
//
//
// Output buffer (NMOS voltage follower)
// -------------------------------------
//
//
//            12V
//
//             |
//             |
//         ||--
// vi -----||
//         ||--
//             |
//             |------ vo
//             |     (AUDIO
//            Rext    OUT)
//             |
//             |
//
//            GND
//
// vi   - input
// vo   - output
// Rext - external resistor, 1kOhm
//
// Notes:
//
// The external resistor Rext is needed to complete the NMOS voltage follower,
// this resistor has a recommended value of 1kOhm.
//
// Die photographs show that actually, two NMOS transistors are used in the
// voltage follower. However the two transistors are coupled in parallel (all
// terminals are pairwise common), which implies that we can model the two
// transistors as one.
//
// ----------------------------------------------------------------------------

// Compile-time computation of op-amp summer and mixer table offsets.

// The highpass summer has 2 - 6 inputs (bandpass, lowpass, and 0 - 4 voices).
template<int i>
struct summer_offset
{
  enum { value = summer_offset<i - 1>::value + ((2 + i - 1) << 16) };
};

template<>
struct summer_offset<0>
{
  enum { value = 0 };
};

// The mixer has 0 - 7 inputs (0 - 4 voices and 0 - 3 filter outputs).
template<int i>
struct mixer_offset
{
  enum { value = mixer_offset<i - 1>::value + ((i - 1) << 16) };
};
	      
template<>
struct mixer_offset<1>
{
  enum { value = 1 };
};

template<>
struct mixer_offset<0>
{
  enum { value = 0 };
};


class Filter
{
public:
  Filter();

  void enable_filter(bool enable);
  void set_chip_model(chip_model model);
  void set_voice_mask(reg4 mask);

  RESID_INLINE
  void clock(int voice1, int voice2, int voice3);
  RESID_INLINE
  void clock(cycle_count delta_t, int voice1, int voice2, int voice3);
  void reset();

  // Write registers.
  void writeFC_LO(reg8);
  void writeFC_HI(reg8);
  void writeRES_FILT(reg8);
  void writeMODE_VOL(reg8);

  // SID audio input (16 bits).
  void input(short sample);

  // SID audio output (16 bits).
  short output();

protected:
  void set_sum_mix();
  void set_w0();
  void set_Q();

  RESID_INLINE
  static int isqrt(int x);

  // Filter enabled.
  bool enabled;

  // Filter cutoff frequency.
  reg12 fc;

  // Filter resonance.
  reg8 res;

  // Selects which voices to route through the filter.
  reg8 filt;

  // Selects which filter outputs to route into the mixer.
  reg4 mode;

  // Output master volume.
  reg4 vol;

  // Used to mask out EXT IN if not connected, and for test purposes
  // (voice muting).
  reg8 voice_mask;

  // Select which inputs to route into the summer / mixer.
  // These are derived from filt, mode, and voice_mask.
  reg8 sum;
  reg8 mix;

  // Voice scale and DC offset.
  // FIXME:
 public:
  // State of filter.
  int Vhp; // highpass
  int Vbp; // bandpass
  int Vbp_x, Vbp_vc;
  int Vlp; // lowpass
  int Vlp_x, Vlp_vc;
  // Filter / mixer inputs.
  int ve;
  int v3;
  int v2;
  int v1;

  // Cutoff frequency DAC voltage, resonance.
  int Vw, Vw_term;
  int _8_div_Q;
  // FIXME: Temporarily used for MOS 8580 emulation.
  int w0;
  int _1024_div_Q;

  chip_model sid_model;

  typedef struct {
    int vo_N19;  // Fixed point scaling for 19 bit op-amp output.
    int vo_T19;  // Fixed point scaled translation for 19 bit op-amp output.
    int Vth;     // Transistor threshold voltage.
    int Vddt;    // Vdd - Vth
    int n_vcr;
    int n_snake;
    int voice_scale_s14;
    int voice_DC;
    int vc_min;
    int vc_max;

    // Op-amp transfer function.
    int opamp[1 << 19];
    // Lookup tables for gain and summer op-amps in output stage / filter.
    unsigned short summer[summer_offset<5>::value];
    unsigned short gain[16][1 << 16];
    unsigned short mixer[mixer_offset<8>::value];
    // Cutoff frequency DAC output voltage table. FC is an 11 bit register.
    unsigned int f0_dac[1 << 11];
  } model_filter_t;

  int solve_gain(int n, int vi_t, int& x, model_filter_t& mf);
  int solve_integrate(int dt, int vi_t, int& x, int& vc, model_filter_t& mf);

  static int sqrt_table[1 << 16];
  static model_filter_t model_filter[2];

friend class SID;
};


// ----------------------------------------------------------------------------
// Inline functions.
// The following functions are defined inline because they are called every
// time a sample is calculated.
// ----------------------------------------------------------------------------

#if RESID_INLINING || defined(RESID_FILTER_CC)

// ----------------------------------------------------------------------------
// SID clocking - 1 cycle.
// ----------------------------------------------------------------------------
RESID_INLINE
void Filter::clock(int voice1, int voice2, int voice3)
{
  model_filter_t& f = model_filter[sid_model];

  v1 = (voice1*f.voice_scale_s14 >> 14) + f.voice_DC;
  v2 = (voice2*f.voice_scale_s14 >> 14) + f.voice_DC;
  v3 = (voice3*f.voice_scale_s14 >> 14) + f.voice_DC;

  // This is handy for testing.
  if (unlikely(!enabled)) {
    return;
  }

  // Sum inputs routed into the filter.
  int Vi;
  int offset;

  switch (sum) {
  default:
  case 0x0:
    Vi = 0;
    offset = summer_offset<0>::value;
    break;
  case 0x1:
    Vi = v1;
    offset = summer_offset<1>::value;
    break;
  case 0x2:
    Vi = v2;
    offset = summer_offset<1>::value;
    break;
  case 0x3:
    Vi = v2 + v1;
    offset = summer_offset<2>::value;
    break;
  case 0x4:
    Vi = v3;
    offset = summer_offset<1>::value;
    break;
  case 0x5:
    Vi = v3 + v1;
    offset = summer_offset<2>::value;
    break;
  case 0x6:
    Vi = v3 + v2;
    offset = summer_offset<2>::value;
    break;
  case 0x7:
    Vi = v3 + v2 + v1;
    offset = summer_offset<3>::value;
    break;
  case 0x8:
    Vi = ve;
    offset = summer_offset<1>::value;
    break;
  case 0x9:
    Vi = ve + v1;
    offset = summer_offset<2>::value;
    break;
  case 0xa:
    Vi = ve + v2;
    offset = summer_offset<2>::value;
    break;
  case 0xb:
    Vi = ve + v2 + v1;
    offset = summer_offset<3>::value;
    break;
  case 0xc:
    Vi = ve + v3;
    offset = summer_offset<2>::value;
    break;
  case 0xd:
    Vi = ve + v3 + v1;
    offset = summer_offset<3>::value;
    break;
  case 0xe:
    Vi = ve + v3 + v2;
    offset = summer_offset<3>::value;
    break;
  case 0xf:
    Vi = ve + v3 + v2 + v1;
    offset = summer_offset<4>::value;
    break;
  }

  // Calculate filter outputs.
  if (sid_model == 0) {
    // MOS 6581.
    Vlp = solve_integrate(1, Vbp, Vlp_x, Vlp_vc, f);
    Vbp = solve_integrate(1, Vhp, Vbp_x, Vbp_vc, f);
    Vhp = f.summer[offset + f.gain[_8_div_Q][Vbp >> 3] + ((Vlp + Vi) >> 3)] << 3;
  }
  else {
    // MOS 8580. FIXME: Not yet using op-amp model.

    // delta_t = 1 is converted to seconds given a 1MHz clock by dividing
    // with 1 000 000.

    int dVbp = w0*(Vhp >> 7) >> 13;
    int dVlp = w0*(Vbp >> 7) >> 13;
    Vbp -= dVbp;
    Vlp -= dVlp;
    Vhp = (Vbp*_1024_div_Q >> 10) - Vlp - Vi;
  }
}

// ----------------------------------------------------------------------------
// SID clocking - delta_t cycles.
// ----------------------------------------------------------------------------
RESID_INLINE
void Filter::clock(cycle_count delta_t, int voice1, int voice2, int voice3)
{
  model_filter_t& f = model_filter[sid_model];

  v1 = (voice1*f.voice_scale_s14 >> 14) + f.voice_DC;
  v2 = (voice2*f.voice_scale_s14 >> 14) + f.voice_DC;
  v3 = (voice3*f.voice_scale_s14 >> 14) + f.voice_DC;

  // Enable filter on/off.
  // This is not really part of SID, but is useful for testing.
  // On slow CPUs it may be necessary to bypass the filter to lower the CPU
  // load.
  if (unlikely(!enabled)) {
    return;
  }

  // Sum inputs routed into the filter.
  int Vi;
  int offset;

  switch (sum) {
  default:
  case 0x0:
    Vi = 0;
    offset = summer_offset<0>::value;
    break;
  case 0x1:
    Vi = v1;
    offset = summer_offset<1>::value;
    break;
  case 0x2:
    Vi = v2;
    offset = summer_offset<1>::value;
    break;
  case 0x3:
    Vi = v2 + v1;
    offset = summer_offset<2>::value;
    break;
  case 0x4:
    Vi = v3;
    offset = summer_offset<1>::value;
    break;
  case 0x5:
    Vi = v3 + v1;
    offset = summer_offset<2>::value;
    break;
  case 0x6:
    Vi = v3 + v2;
    offset = summer_offset<2>::value;
    break;
  case 0x7:
    Vi = v3 + v2 + v1;
    offset = summer_offset<3>::value;
    break;
  case 0x8:
    Vi = ve;
    offset = summer_offset<1>::value;
    break;
  case 0x9:
    Vi = ve + v1;
    offset = summer_offset<2>::value;
    break;
  case 0xa:
    Vi = ve + v2;
    offset = summer_offset<2>::value;
    break;
  case 0xb:
    Vi = ve + v2 + v1;
    offset = summer_offset<3>::value;
    break;
  case 0xc:
    Vi = ve + v3;
    offset = summer_offset<2>::value;
    break;
  case 0xd:
    Vi = ve + v3 + v1;
    offset = summer_offset<3>::value;
    break;
  case 0xe:
    Vi = ve + v3 + v2;
    offset = summer_offset<3>::value;
    break;
  case 0xf:
    Vi = ve + v3 + v2 + v1;
    offset = summer_offset<4>::value;
    break;
  }

  // Maximum delta cycles for filter fixpoint iteration to converge
  // is approximately 3.
  cycle_count delta_t_flt = 3;

  if (sid_model == 0) {
    // MOS 6581.
    while (delta_t) {
      if (unlikely(delta_t < delta_t_flt)) {
	delta_t_flt = delta_t;
      }

      // Calculate filter outputs.
      Vlp = solve_integrate(delta_t_flt, Vbp, Vlp_x, Vlp_vc, f);
      Vbp = solve_integrate(delta_t_flt, Vhp, Vbp_x, Vbp_vc, f);
      Vhp = f.summer[offset + f.gain[_8_div_Q][Vbp >> 3] + ((Vlp + Vi) >> 3)] << 3;

      delta_t -= delta_t_flt;
    }
  }
  else {
    // MOS 8580. FIXME: Not yet using op-amp model.
    while (delta_t) {
      if (delta_t < delta_t_flt) {
	delta_t_flt = delta_t;
      }

      // delta_t is converted to seconds given a 1MHz clock by dividing
      // with 1 000 000. This is done in two operations to avoid integer
      // multiplication overflow.

      // Calculate filter outputs.
      int w0_delta_t = w0*delta_t_flt >> 2;

      int dVbp = w0_delta_t*(Vhp >> 7) >> 11;
      int dVlp = w0_delta_t*(Vbp >> 7) >> 11;
      Vbp -= dVbp;
      Vlp -= dVlp;
      Vhp = (Vbp*_1024_div_Q >> 10) - Vlp - Vi;

      delta_t -= delta_t_flt;
    }
  }
}


// ----------------------------------------------------------------------------
// SID audio input (16 bits).
// ----------------------------------------------------------------------------
RESID_INLINE
void Filter::input(short sample)
{
  // Scale to three times the peak-to-peak for one voice and add the op-amp
  // "zero" DC level.
  // NB! Adding the op-amp "zero" DC level is a (wildly inaccurate)
  // approximation of feeding the input through an AC coupling capacitor.
  // This could be implemented as a separate filter circuit, however the
  // primary use of the emulator is not to process external signals.
  // The upside is that the MOS8580 "digi boost" works without a separate (DC)
  // input interface.
  // Note that the input is 16 bits, compared to the 20 bit voice output;
  // we thus right shift by 10 instead of 14.
  model_filter_t& f = model_filter[sid_model];
  ve = (sample*f.voice_scale_s14*3 >> 10) + (f.mixer[0] << 3);
}


// ----------------------------------------------------------------------------
// SID audio output (16 bits).
// ----------------------------------------------------------------------------
RESID_INLINE
short Filter::output()
{
  model_filter_t& f = model_filter[sid_model];

  // Writing the switch below manually would be tedious and error-prone;
  // it is rather generated by the following Perl program:

  /*
my @i = qw(v1 v2 v3 ve Vlp Vbp Vhp);
for my $mix (0..2**@i-1) {
    print sprintf("  case 0x%02x:\n", $mix);
    my @sum;
    for (@i) {
	unshift(@sum, $_) if $mix & 0x01;
	$mix >>= 1;
    }
    my $sum = join(" + ", @sum) || "0";
    print "    Vi = $sum;\n";
    print "    offset = mixer_offset<" . @sum . ">::value;\n";
    print "    break;\n";
}
  */

  // Sum inputs routed into the mixer.
  int Vi;
  int offset;

  switch (mix) {
  default:
  case 0x00:
    Vi = 0;
    offset = mixer_offset<0>::value;
    break;
  case 0x01:
    Vi = v1;
    offset = mixer_offset<1>::value;
    break;
  case 0x02:
    Vi = v2;
    offset = mixer_offset<1>::value;
    break;
  case 0x03:
    Vi = v2 + v1;
    offset = mixer_offset<2>::value;
    break;
  case 0x04:
    Vi = v3;
    offset = mixer_offset<1>::value;
    break;
  case 0x05:
    Vi = v3 + v1;
    offset = mixer_offset<2>::value;
    break;
  case 0x06:
    Vi = v3 + v2;
    offset = mixer_offset<2>::value;
    break;
  case 0x07:
    Vi = v3 + v2 + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x08:
    Vi = ve;
    offset = mixer_offset<1>::value;
    break;
  case 0x09:
    Vi = ve + v1;
    offset = mixer_offset<2>::value;
    break;
  case 0x0a:
    Vi = ve + v2;
    offset = mixer_offset<2>::value;
    break;
  case 0x0b:
    Vi = ve + v2 + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x0c:
    Vi = ve + v3;
    offset = mixer_offset<2>::value;
    break;
  case 0x0d:
    Vi = ve + v3 + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x0e:
    Vi = ve + v3 + v2;
    offset = mixer_offset<3>::value;
    break;
  case 0x0f:
    Vi = ve + v3 + v2 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x10:
    Vi = Vlp;
    offset = mixer_offset<1>::value;
    break;
  case 0x11:
    Vi = Vlp + v1;
    offset = mixer_offset<2>::value;
    break;
  case 0x12:
    Vi = Vlp + v2;
    offset = mixer_offset<2>::value;
    break;
  case 0x13:
    Vi = Vlp + v2 + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x14:
    Vi = Vlp + v3;
    offset = mixer_offset<2>::value;
    break;
  case 0x15:
    Vi = Vlp + v3 + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x16:
    Vi = Vlp + v3 + v2;
    offset = mixer_offset<3>::value;
    break;
  case 0x17:
    Vi = Vlp + v3 + v2 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x18:
    Vi = Vlp + ve;
    offset = mixer_offset<2>::value;
    break;
  case 0x19:
    Vi = Vlp + ve + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x1a:
    Vi = Vlp + ve + v2;
    offset = mixer_offset<3>::value;
    break;
  case 0x1b:
    Vi = Vlp + ve + v2 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x1c:
    Vi = Vlp + ve + v3;
    offset = mixer_offset<3>::value;
    break;
  case 0x1d:
    Vi = Vlp + ve + v3 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x1e:
    Vi = Vlp + ve + v3 + v2;
    offset = mixer_offset<4>::value;
    break;
  case 0x1f:
    Vi = Vlp + ve + v3 + v2 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x20:
    Vi = Vbp;
    offset = mixer_offset<1>::value;
    break;
  case 0x21:
    Vi = Vbp + v1;
    offset = mixer_offset<2>::value;
    break;
  case 0x22:
    Vi = Vbp + v2;
    offset = mixer_offset<2>::value;
    break;
  case 0x23:
    Vi = Vbp + v2 + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x24:
    Vi = Vbp + v3;
    offset = mixer_offset<2>::value;
    break;
  case 0x25:
    Vi = Vbp + v3 + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x26:
    Vi = Vbp + v3 + v2;
    offset = mixer_offset<3>::value;
    break;
  case 0x27:
    Vi = Vbp + v3 + v2 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x28:
    Vi = Vbp + ve;
    offset = mixer_offset<2>::value;
    break;
  case 0x29:
    Vi = Vbp + ve + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x2a:
    Vi = Vbp + ve + v2;
    offset = mixer_offset<3>::value;
    break;
  case 0x2b:
    Vi = Vbp + ve + v2 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x2c:
    Vi = Vbp + ve + v3;
    offset = mixer_offset<3>::value;
    break;
  case 0x2d:
    Vi = Vbp + ve + v3 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x2e:
    Vi = Vbp + ve + v3 + v2;
    offset = mixer_offset<4>::value;
    break;
  case 0x2f:
    Vi = Vbp + ve + v3 + v2 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x30:
    Vi = Vbp + Vlp;
    offset = mixer_offset<2>::value;
    break;
  case 0x31:
    Vi = Vbp + Vlp + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x32:
    Vi = Vbp + Vlp + v2;
    offset = mixer_offset<3>::value;
    break;
  case 0x33:
    Vi = Vbp + Vlp + v2 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x34:
    Vi = Vbp + Vlp + v3;
    offset = mixer_offset<3>::value;
    break;
  case 0x35:
    Vi = Vbp + Vlp + v3 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x36:
    Vi = Vbp + Vlp + v3 + v2;
    offset = mixer_offset<4>::value;
    break;
  case 0x37:
    Vi = Vbp + Vlp + v3 + v2 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x38:
    Vi = Vbp + Vlp + ve;
    offset = mixer_offset<3>::value;
    break;
  case 0x39:
    Vi = Vbp + Vlp + ve + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x3a:
    Vi = Vbp + Vlp + ve + v2;
    offset = mixer_offset<4>::value;
    break;
  case 0x3b:
    Vi = Vbp + Vlp + ve + v2 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x3c:
    Vi = Vbp + Vlp + ve + v3;
    offset = mixer_offset<4>::value;
    break;
  case 0x3d:
    Vi = Vbp + Vlp + ve + v3 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x3e:
    Vi = Vbp + Vlp + ve + v3 + v2;
    offset = mixer_offset<5>::value;
    break;
  case 0x3f:
    Vi = Vbp + Vlp + ve + v3 + v2 + v1;
    offset = mixer_offset<6>::value;
    break;
  case 0x40:
    Vi = Vhp;
    offset = mixer_offset<1>::value;
    break;
  case 0x41:
    Vi = Vhp + v1;
    offset = mixer_offset<2>::value;
    break;
  case 0x42:
    Vi = Vhp + v2;
    offset = mixer_offset<2>::value;
    break;
  case 0x43:
    Vi = Vhp + v2 + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x44:
    Vi = Vhp + v3;
    offset = mixer_offset<2>::value;
    break;
  case 0x45:
    Vi = Vhp + v3 + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x46:
    Vi = Vhp + v3 + v2;
    offset = mixer_offset<3>::value;
    break;
  case 0x47:
    Vi = Vhp + v3 + v2 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x48:
    Vi = Vhp + ve;
    offset = mixer_offset<2>::value;
    break;
  case 0x49:
    Vi = Vhp + ve + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x4a:
    Vi = Vhp + ve + v2;
    offset = mixer_offset<3>::value;
    break;
  case 0x4b:
    Vi = Vhp + ve + v2 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x4c:
    Vi = Vhp + ve + v3;
    offset = mixer_offset<3>::value;
    break;
  case 0x4d:
    Vi = Vhp + ve + v3 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x4e:
    Vi = Vhp + ve + v3 + v2;
    offset = mixer_offset<4>::value;
    break;
  case 0x4f:
    Vi = Vhp + ve + v3 + v2 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x50:
    Vi = Vhp + Vlp;
    offset = mixer_offset<2>::value;
    break;
  case 0x51:
    Vi = Vhp + Vlp + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x52:
    Vi = Vhp + Vlp + v2;
    offset = mixer_offset<3>::value;
    break;
  case 0x53:
    Vi = Vhp + Vlp + v2 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x54:
    Vi = Vhp + Vlp + v3;
    offset = mixer_offset<3>::value;
    break;
  case 0x55:
    Vi = Vhp + Vlp + v3 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x56:
    Vi = Vhp + Vlp + v3 + v2;
    offset = mixer_offset<4>::value;
    break;
  case 0x57:
    Vi = Vhp + Vlp + v3 + v2 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x58:
    Vi = Vhp + Vlp + ve;
    offset = mixer_offset<3>::value;
    break;
  case 0x59:
    Vi = Vhp + Vlp + ve + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x5a:
    Vi = Vhp + Vlp + ve + v2;
    offset = mixer_offset<4>::value;
    break;
  case 0x5b:
    Vi = Vhp + Vlp + ve + v2 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x5c:
    Vi = Vhp + Vlp + ve + v3;
    offset = mixer_offset<4>::value;
    break;
  case 0x5d:
    Vi = Vhp + Vlp + ve + v3 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x5e:
    Vi = Vhp + Vlp + ve + v3 + v2;
    offset = mixer_offset<5>::value;
    break;
  case 0x5f:
    Vi = Vhp + Vlp + ve + v3 + v2 + v1;
    offset = mixer_offset<6>::value;
    break;
  case 0x60:
    Vi = Vhp + Vbp;
    offset = mixer_offset<2>::value;
    break;
  case 0x61:
    Vi = Vhp + Vbp + v1;
    offset = mixer_offset<3>::value;
    break;
  case 0x62:
    Vi = Vhp + Vbp + v2;
    offset = mixer_offset<3>::value;
    break;
  case 0x63:
    Vi = Vhp + Vbp + v2 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x64:
    Vi = Vhp + Vbp + v3;
    offset = mixer_offset<3>::value;
    break;
  case 0x65:
    Vi = Vhp + Vbp + v3 + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x66:
    Vi = Vhp + Vbp + v3 + v2;
    offset = mixer_offset<4>::value;
    break;
  case 0x67:
    Vi = Vhp + Vbp + v3 + v2 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x68:
    Vi = Vhp + Vbp + ve;
    offset = mixer_offset<3>::value;
    break;
  case 0x69:
    Vi = Vhp + Vbp + ve + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x6a:
    Vi = Vhp + Vbp + ve + v2;
    offset = mixer_offset<4>::value;
    break;
  case 0x6b:
    Vi = Vhp + Vbp + ve + v2 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x6c:
    Vi = Vhp + Vbp + ve + v3;
    offset = mixer_offset<4>::value;
    break;
  case 0x6d:
    Vi = Vhp + Vbp + ve + v3 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x6e:
    Vi = Vhp + Vbp + ve + v3 + v2;
    offset = mixer_offset<5>::value;
    break;
  case 0x6f:
    Vi = Vhp + Vbp + ve + v3 + v2 + v1;
    offset = mixer_offset<6>::value;
    break;
  case 0x70:
    Vi = Vhp + Vbp + Vlp;
    offset = mixer_offset<3>::value;
    break;
  case 0x71:
    Vi = Vhp + Vbp + Vlp + v1;
    offset = mixer_offset<4>::value;
    break;
  case 0x72:
    Vi = Vhp + Vbp + Vlp + v2;
    offset = mixer_offset<4>::value;
    break;
  case 0x73:
    Vi = Vhp + Vbp + Vlp + v2 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x74:
    Vi = Vhp + Vbp + Vlp + v3;
    offset = mixer_offset<4>::value;
    break;
  case 0x75:
    Vi = Vhp + Vbp + Vlp + v3 + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x76:
    Vi = Vhp + Vbp + Vlp + v3 + v2;
    offset = mixer_offset<5>::value;
    break;
  case 0x77:
    Vi = Vhp + Vbp + Vlp + v3 + v2 + v1;
    offset = mixer_offset<6>::value;
    break;
  case 0x78:
    Vi = Vhp + Vbp + Vlp + ve;
    offset = mixer_offset<4>::value;
    break;
  case 0x79:
    Vi = Vhp + Vbp + Vlp + ve + v1;
    offset = mixer_offset<5>::value;
    break;
  case 0x7a:
    Vi = Vhp + Vbp + Vlp + ve + v2;
    offset = mixer_offset<5>::value;
    break;
  case 0x7b:
    Vi = Vhp + Vbp + Vlp + ve + v2 + v1;
    offset = mixer_offset<6>::value;
    break;
  case 0x7c:
    Vi = Vhp + Vbp + Vlp + ve + v3;
    offset = mixer_offset<5>::value;
    break;
  case 0x7d:
    Vi = Vhp + Vbp + Vlp + ve + v3 + v1;
    offset = mixer_offset<6>::value;
    break;
  case 0x7e:
    Vi = Vhp + Vbp + Vlp + ve + v3 + v2;
    offset = mixer_offset<6>::value;
    break;
  case 0x7f:
    Vi = Vhp + Vbp + Vlp + ve + v3 + v2 + v1;
    offset = mixer_offset<7>::value;
    break;
  }

  // Sum the inputs in the mixer and run the mixer output through the gain.
  if (sid_model == 0) {
    return (short)(f.gain[vol][f.mixer[offset + (Vi >> 3)]] - (1 << 15));
  }
  else {
    // FIXME: Temporary code for MOS 8580, should use code above.
    return Vi*vol >> 7;
  }
}


/*
Find output voltage in inverting gain and inverting summer SID op-amp
circuits, using a combination of Newton-Raphson and bisection.

             ---R2--
            |       |
  vi ---R1-----[A>----- vo
            vx

From Kirchoff's current law it follows that

  IR1 - IR2 = 0

Substituting the standard Id-Vds transistor model (triode mode) for the
currents, we get one equation for each direction of the current.

Eq. 1: n*(2*(Vddt-vx)-(vi-vx))*(vi-vx) - (2*(Vddt-vo)-(vx-vo))*(vx-vo) = 0
Eq. 2: n*(2*(Vddt-vi)-(vx-vi))*(vx-vi) - (2*(Vddt-vx)-(vo-vx))*(vo-vx) = 0

It turns out that these equations are equal (multiply Eq. 2 by -1).
We factorize Eq. 1 in order to reduce the number of arithmetic operations,
and get our root function f:

f = (2*Vddt - vo)*vo - (n + 1)*(2*Vddt - vx)*vx + n*(2*Vddt - vi)*vi = 0

Using substitution constants

a = n + 1
b = Vddt
c = n*(2*b - vi)*vi

we get the following equations for the root function and its derivative:

f = (2*b - vo)*vo - a*(2*b - vx)*vx + c
df = 2*((b - vo)*dvo - a*(b - vx))
*/
RESID_INLINE
int Filter::solve_gain(int n, int vi_n, int& x, model_filter_t& mf)
{
  // Translate normalized vi.
  int vi = vi_n + mf.vo_T19;

  // Start off with an estimate of x and a root bracket [ak, bk].
  // f is decreasing, so that f(ak) > 0 and f(bk) < 0.
  int ak = mf.vo_T19, bk = mf.vo_T19 + (1 << 19) - 1;

  int a = n + (1 << 7); // Scaled by 2^7
  int b = mf.Vddt;      // Scaled by m*2^19
  unsigned int _2b = b << 1;  // Scaled by m*2^19, unsigned to use all bits.
  int c = n*(((_2b - vi) >> 4)*(vi >> 3) >> 11); // Scaled by m^2*2^27.

  for (;;) {
    int xk = x;

    // Calculate f and df.
    int vo_dvo = mf.opamp[x - mf.vo_T19];
    int vo = (vo_dvo & 0x7ffff) + mf.vo_T19;  // Scaled by m*2^19
    int dvo = vo_dvo >> 19;                   // Scaled by 2^8

    // f = (2*b - vo)*vo - a*(2*b - vx)*vx + c
    // df = 2*((b - vo)*dvo - a*(b - vx))
    //
    // The dividend is scaled by m^2*2^27.
    int f = int(((_2b - vo) >> 4)*(vo >> 3) >> 4) - int(a*(((_2b - x) >> 4)*(x >> 3) >> 11)) + c;
    // The divisor is scaled by m*2^8.
    int df = (((b - vo)*dvo >> 1) - a*(b - x)) >> 17;
    // The resulting quotient is thus scaled by m*2^19.

    // Newton-Raphson step: xk1 = xk - f(xk)/f'(xk)
    x -= f/df;
    if (unlikely(x == xk)) {
      // No further root improvement possible.
      return vo - mf.vo_T19;
    }

    // Narrow down root bracket.
    if (f < 0) {
      // f(xk) < 0
      bk = xk;
    }
    else {
      // f(xk) > 0
      ak = xk;
    }

    if (unlikely(x <= ak) || unlikely(x >= bk)) {
      // Bisection step (ala Dekker's method).
      x = (ak + bk) >> 1;
      if (unlikely(x == ak)) {
	// No further bisection possible.
	return vo - mf.vo_T19;
      }
    }
  }
}


/*
Find output voltage in inverting integrator SID op-amp circuits, using a
single fixpoint iteration step.

                 ---C---
                |       |
  vi -----Rw-------[A>----- vo
       |      | vx
        --Rs--

From Kirchoff's current law it follows that

  IRw + IRs - IC = 0

Using the formula for current through a capacitor, i = C*dv/dt, we get

  IRw + IRs - C*(vc - vc0)/dt = 0
  dt/C*(IRw + IRs) - vc + vc0 = 0

  vc = n*(IRw(vi,vx) + IRs(vi,vx)) + vc0
  vc = n*(IRw(vi,g(vc)) + IRs(vi,g(vc))) + vc0

*/
RESID_INLINE
int Filter::solve_integrate(int dt, int vi_n, int& x, int& vc,
			    model_filter_t& mf)
{
  // Translate normalized vi.
  int vi = vi_n + mf.vo_T19; // Scaled by m*2^19

  int Vddt = mf.Vddt;        // Scaled by m*2^19
  int n_vcr = mf.n_vcr;      // Scaled by (1/m)*2^9  (fits in 12 bits)
  int n_snake = mf.n_snake;  // Scaled by (1/m)*2^19 (fits in 12 bits)

  // VCR gate voltage.
  // Vddt - sqrt(Vddt*(Vddt - Vw - Vi) + (Vw*Vw + Vi*Vi)/2)
  // Vth could be included in the table lookup by using different tables
  // for the 6581 and the 8580.
  int Vg = Vddt - sqrt_table[(Vw_term + (vi >> 4)*(((vi >> 1) - Vddt) >> 4)) >> 14];
  int Vgt = Vg - mf.Vth;     // Scaled by m*2^19

  // Determine the direction of the current flowing through the VCR and
  // the "snake" transistor.
  if (vi < x) {
    // Negative current.
    int Vds = x - vi;
    int Vov_vcr = Vgt - vi;
    int Vov_snake = Vddt - vi;

    // Start with the current through the "snake" (triode mode).
    // n_I = n_snake*(2*Vov_snake - Vds)*Vds
    //
    // Scaled by (1/m)*2^19*m*2^19*m*2^19*2^-4*2^-4*2^-12*2^-18 = m*2^19
    int n_I = n_snake*((((Vov_snake << 1) - Vds) >> 4)*(Vds >> 4) >> 12) >> 18;

    if (Vov_vcr > 0) {
      // Add the current through the VCR.
      if (Vds < Vov_vcr) {
	// Vov >= 0, Vds < Vov: Triode mode (linear region).
	// n_I += n_vcr*(2*Vov_vcr - Vds)*Vds
	//
	// Scaled by (1/m)*2^9*m*2^19*m*2^19*2^-4*2^-4*2^-12*2^8 = m*2^19
	n_I += n_vcr*((((Vov_vcr << 1) - Vds) >> 4)*(Vds >> 4) >> 12) >> 8;
      }
      else {
	// Vov >= 0, Vds >= Vov: Saturation mode (pinch-off).
	// The linear dependence of Ids on Vds is negligible, and is not
	// modeled.
	// n_I += n_vcr*Vov_vcr*Vov_vcr
	//
	// Scaled by (1/m)*2^9*m*2^19*m*2^19*2^-4*2^-3*2^-12*2^8 = m*2^19
	n_I += n_vcr*((Vov_vcr >> 4)*(Vov_vcr >> 4) >> 12) >> 8;
      }
    }
    // The subthreshold leakage current is negligible, and is not modeled.

    // Change in capacitor charge.
    vc -= n_I*dt;
    if (vc < mf.vc_min) {
      vc = mf.vc_min;
    }
  }
  else {
    // Positive current.
    int Vds = vi - x;
    int Vov_vcr = Vgt - x;
    int Vov_snake = Vddt - x;

    int n_I = n_snake*((((Vov_snake << 1) - Vds) >> 4)*(Vds >> 4) >> 12) >> 18;

    if (Vov_vcr > 0) {
      if (Vds < Vov_vcr) {
	n_I += n_vcr*((((Vov_vcr << 1) - Vds) >> 4)*(Vds >> 4) >> 12) >> 8;
      }
      else {
	n_I += n_vcr*((Vov_vcr >> 4)*(Vov_vcr >> 4) >> 12) >> 8;
      }
    }

    vc += n_I*dt;
    if (vc > mf.vc_max) {
      vc = mf.vc_max;
    }
  }

  // vx = g(vc)
  x = mf.opamp[(vc + (1 << 19)) >> 1];

  // Return vo.
  return (x - vc) - mf.vo_T19;
}

#endif // RESID_INLINING || defined(RESID_FILTER_CC)

} // namespace reSID

#endif // not RESID_FILTER_H
