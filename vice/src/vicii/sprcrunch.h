/*
 * sprcrunch.h - Table for emulation of the "spritecrunch" effect.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _SPRCRUNCH_H
#define _SPRCRUNCH_H

static const int sprite_crunch_table[64] = {  1,   4,   3, /* 0 */
                                              4,   1,   0, /* 3 */
                                              -1,   0,   1, /* 6 */
                                              4,   3,   4, /* 9 */
                                              1,   8,   7, /* 12 */
                                              8,   1,   4, /* 15 */
                                              3,   4,   1, /* 18 */
                                              0,  -1,   0, /* 21 */
                                              1,   4,   3, /* 24 */
                                              4,   1,  -8, /* 27 */
                                              -9,  -8,   1, /* 30 */
                                              4,   3,   4, /* 33 */
                                              1,   0,  -1, /* 36 */
                                              0,   1,   4, /* 39 */
                                              3,   4,   1, /* 42 */
                                              8,   7,   8, /* 45 */
                                              1,   4,   3, /* 48 */
                                              4,   1,   0, /* 51 */
                                              -1,   0,   1, /* 54 */
                                              4,   3,   4, /* 57 */
                                              1, -40, -41, /* 60 */
                                              0 };

#endif /* _SPRCRUNCH_H */
