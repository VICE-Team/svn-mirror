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

#include "../src/residfp/residfp.h"

#include <algorithm>

using namespace UnitTest;
using namespace reSIDfp;

SUITE(SID)
{

#define BUF_SIZE 481
#define CYCLES 10000
#define CANARY 0x7fff

struct TestFixture
{
    // Test setup
    TestFixture()
    {
        std::fill_n(buf, BUF_SIZE+1, CANARY);
        s.setSamplingParameters(1000000., DECIMATE, 48000.);
    }

    residfp s;

    short buf[BUF_SIZE+1];
};

TEST_FIXTURE(TestFixture, TestCycles)
{
    int c = s.clock(buf, BUF_SIZE);
    CHECK(c == CYCLES);
    CHECK(buf[BUF_SIZE-1] != CANARY);
    CHECK(buf[BUF_SIZE] == CANARY);
}

TEST_FIXTURE(TestFixture, TestBufsize)
{
    int b = s.clock(CYCLES, buf);
    CHECK(b == BUF_SIZE);
    CHECK(buf[BUF_SIZE-1] != CANARY);
    CHECK(buf[BUF_SIZE] == CANARY);
}

}
