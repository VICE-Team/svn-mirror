/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 *  Copyright (C) 2026 Leandro Nini
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "utpp/utpp.h"

#include "../src/FilterModelConfig6581.cpp"
#include "../src/FilterModelConfig.cpp"
#include "../src/Dac.cpp"
#include "../src/Spline.cpp"
#include "../src/OpAmp.cpp"

using namespace UnitTest;
using namespace reSIDfp;

SUITE(FilterModelConfig6581)
{


TEST(TestFilterCurve)
{
    // Check that values in the filter curve range do not
    // trigger assertions
    unsigned short* dac;

    dac = FilterModelConfig6581::getInstance()->getDAC(0.0);
    delete [] dac;

    dac = FilterModelConfig6581::getInstance()->getDAC(1.0);
    delete [] dac;
}

TEST(TestFilterRange)
{
    FilterModelConfig6581::getInstance()->setFilterRange(0.0);

    FilterModelConfig6581::getInstance()->setFilterRange(1.0);
}

}
