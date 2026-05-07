/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2026 Leandro Nini <drfiemost@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef LIMITER_H
#define LIMITER_H

#include <cmath>
#include <cassert>

#include "siddefs-fp.h"

class Limiter
{
private:
    static constexpr int threshold = 28000;

    template<int m>
    static inline int clipper(int x)
    {
        static_assert(m > 0);
        assert(x >= 0);
        if (likely(x < threshold))
            return x;

        constexpr double max_val = static_cast<double>(m);
        constexpr double t = threshold / max_val;
        constexpr double a = 1. - t;
        constexpr double b = 1. / a;

        double value = static_cast<double>(x - threshold) / max_val;
        value = a * std::tanh(b * value);
        return static_cast<int>(threshold + (value * max_val));
    }

    /*
     * Soft Clipping implementation, splitted for test.
     */
    static inline int softClipImpl(int x)
    {
        return x < 0 ? -clipper<32768>(-x) : clipper<32767>(x);
    }

public:
    /*
     * Soft Clipping into 16 bit range [-32768,32767]
     */
    static inline short softClip(int x) { return static_cast<short>(softClipImpl(x)); }

};

#endif
