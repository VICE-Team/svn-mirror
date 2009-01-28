/*
 * ui_siddtv.h - DTV SID settings
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#ifndef VICE__UI_SIDDTV_H__
#define VICE__UI_SIDDTV_H__

const uint32 MESSAGE_SIDDTV_MODEL				= 'MDS1';
const uint32 MESSAGE_SIDDTV_RESID				= 'MDS2';
const uint32 MESSAGE_SIDDTV_FILTERS			= 'MDS3';
const uint32 MESSAGE_SIDDTV_RESIDSAMPLING		= 'MDS4';
const uint32 MESSAGE_SIDDTV_RESIDPASSBAND		= 'MDS5';

extern void ui_siddtv(void);

#endif
