/*
 * vsyncarch.c - Native GTK3 UI vsync stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdio.h>

#include "not_implemented.h"

#include "vsyncapi.h"


void vsyncarch_display_speed(double speed, double frame_rate, int warp_enabled)
{
    NOT_IMPLEMENTED();
}

signed long vsyncarch_frequency(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

unsigned long vsyncarch_gettime(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

void vsyncarch_init(void)
{
    NOT_IMPLEMENTED();
}

void vsyncarch_postsync(void)
{
    NOT_IMPLEMENTED();
}

void vsyncarch_presync(void)
{
    NOT_IMPLEMENTED();
}

void vsyncarch_sleep(signed long delay)
{
    NOT_IMPLEMENTED();
}

