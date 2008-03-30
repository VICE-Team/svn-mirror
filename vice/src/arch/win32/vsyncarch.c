/*
 * vsyncarch.c - End-of-frame handling for Win32
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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


#include <windows.h>

#include "vice.h"

#include "vsync.h"
#include "kbdbuf.h"
#include "machine.h"
#include "ui.h"
#include "mouse.h"

#ifdef HAS_JOYSTICK
#include "joystick.h"
#endif

#include "vsyncarch.h"
// -------------------------------------------------------------------------

static unsigned long frequency = 0;

signed long vsyncarch_frequency()
{
	LARGE_INTEGER li;
	if (!frequency) {
		if (!QueryPerformanceFrequency(&li)) {
			ui_error("Can't get frequency of performance counter");
			return -1;
		}
#ifdef HAS_LONGLONG_INTEGER
		frequency = (signed long)li.QuadPart;
#else
		frequency = (signed long)li.LowPart;
#endif
	}
	return frequency;
}

unsigned long vsyncarch_gettime()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceCounter(&li)) {
		ui_error("Can't get performance counter");
		return 0;
	}
	
#ifdef HAS_LONGLONG_INTEGER
	return (unsigned long)li.QuadPart;
#else
	return (unsigned long)li.LowPart;
#endif
		
}

void vsyncarch_init()
{
}

// -------------------------------------------------------------------------

// Display speed (percentage) and frame rate (frames per second).
void vsyncarch_display_speed(double speed, double frame_rate, int warp_enabled)
{
	ui_display_speed((float) speed, (float)frame_rate, warp_enabled);

}

void vsyncarch_sleep(signed long delay)
{
	LARGE_INTEGER start, now;
	
	if (delay <= 0)
		return;
	QueryPerformanceCounter(&start);
	do {
		Sleep(10);
		QueryPerformanceCounter(&now);
#ifdef HAS_LONGLONG_INTEGER
	} while ((now.QuadPart - start.QuadPart) < delay);
#else
	} while ((now.LowPart - start.LowPart) < delay);
#endif

}

void vsyncarch_presync()
{
    /* Dispatch all the pending UI events.  */
    ui_dispatch_events();

    /* Update mouse */
    mouse_update_mouse();
}

void vsyncarch_postsync()
{
    /* Flush keypresses emulated through the keyboard buffer.  */
    kbd_buf_flush();
    joystick_update();

}
