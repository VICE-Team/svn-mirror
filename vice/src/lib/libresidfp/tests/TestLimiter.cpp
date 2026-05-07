/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 *  Copyright (C) 2024-2025 Leandro Nini
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

#define private public

#include "../src/resample/Limiter.h"

#include <limits>
#include <cstdint>

using namespace UnitTest;

SUITE(Limiter)
{

TEST(TestSoftClip)
{
    // We assume values stay below this peak
    constexpr int peak = Limiter::threshold + Limiter::threshold/2;

    // Values within threshold should pass unchanged
    for (int i=-Limiter::threshold; i<=Limiter::threshold; i++)
        CHECK(Limiter::softClipImpl(i) == i);

    // Values above threshold should be compressed
    for (int i=Limiter::threshold; i<=peak; i++)
    {
        auto x = Limiter::softClipImpl(i);
        CHECK((x <= i) && (x <= INT16_MAX));
    }

    for (int i=-Limiter::threshold; i<=-peak; i--)
    {
        auto x = Limiter::softClipImpl(i);
        CHECK((x >= i) && (x >= INT16_MIN));
    }

    // Check the extremes too
    CHECK(Limiter::softClipImpl(std::numeric_limits<int>::max()) <= INT16_MAX);
    CHECK(Limiter::softClipImpl(std::numeric_limits<int>::min()+1) >= INT16_MIN);
}

}
