/*
 * constants.h - BeOS constans for menus, messages, ...
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

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// Messages for window interaction with application

const uint32 WINDOW_CLOSED				= 'WRcl';

// Messages for menu commands

const uint32 MENU_ATTACH_DISK8			= 'MFa8';
const uint32 MENU_ATTACH_DISK9			= 'MFa9';
const uint32 MENU_ATTACH_TAPE			= 'MFta';
const uint32 MENU_RESET_SOFT			= 'MFrs';
const uint32 MENU_RESET_HARD			= 'MFrh';
const uint32 MENU_ABOUT					= 'MFab';



#endif