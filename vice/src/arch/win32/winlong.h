/*
 * winlong.h - GetWindowLongPtr/SetWindowLongPtr definitions.
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

#ifndef VICE_WINLONG_H
#define VICE_WINLONG_H

#include <winuser.h>

#if !defined(_WIN64) && !defined(GetWindowLongPtr)
#define GetWindowLongPtr GetWindowLong
#endif

#if !defined(_WIN64) && !defined(SetWindowLongPtr)
#define SetWindowLongPtr SetWindowLong
#endif

#ifndef GWLP_HWNDPARENT
#define GWLP_HWNDPARENT GWL_HWNDPARENT
#endif

#ifndef GWLP_USERDATA
#define GWLP_USERDATA GWL_USERDATA
#endif

#ifndef DWLP_MSGRESULT
#define DWLP_MSGRESULT DWL_MSGRESULT
#endif

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC GWL_WNDPROC
#endif

#endif
