/*
 * ui_drive.h - Drive settings
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

#ifndef __UI_DRIVE_H__
#define __UI_DRIVE_H__

#define DRIVE_EXPANSION_2000(type) (        \
        ((type) == DRIVE_TYPE_1541)         \
        || ((type) == DRIVE_TYPE_1541II)    \
        )

#define DRIVE_EXPANSION_4000(type) (        \
        ((type) == DRIVE_TYPE_1541)         \
        || ((type) == DRIVE_TYPE_1541II)    \
        )

#define DRIVE_EXPANSION_6000(type) (        \
        ((type) == DRIVE_TYPE_1541)         \
        || ((type) == DRIVE_TYPE_1541II)    \
        )

#define DRIVE_EXPANSION_8000(type) (        \
        ((type) == DRIVE_TYPE_1541)         \
        || ((type) == DRIVE_TYPE_1541II)    \
        )

#define DRIVE_EXPANSION_A000(type) (        \
        ((type) == DRIVE_TYPE_1541)         \
        || ((type) == DRIVE_TYPE_1541II)    \
        )

const uint32 MESSAGE_DRIVE_TYPE				= 'MD01';
const uint32 MESSAGE_DRIVE_EXTENDIMAGEPOLICY= 'MD02';
const uint32 MESSAGE_DRIVE_IDLEMETHOD		= 'MD03';
const uint32 MESSAGE_DRIVE_EXPANSION		= 'MD04';
const uint32 MESSAGE_DRIVE_PARALLELCABLE	= 'MD05';

extern void ui_drive(void);

#endif
