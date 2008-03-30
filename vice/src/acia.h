/*
 * acia.h - ACIA 6551 register number declarations.
 *
 * Written by
 *  Andre Fachat <a.fachat@physik.tu-chemnitz.de>
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

#ifndef _ACIA_H
#define _ACIA_H

#define   ACIA_DR    0           /* Data register */
#define   ACIA_SR    1           /* R: status register W: programmed Reset */
#define   ACIA_CMD   2           /* Command register */
#define   ACIA_CTRL  3           /* Control register */
#define   T232_NDEF1 4           /* Undefined register 1, turbo232 only */
#define   T232_NDEF2 5           /* Undefined register 2, turbo232 only */
#define   T232_NDEF3 6           /* Undefined register 3, turbo232 only */
#define   T232_ECTRL 7           /* Enhanced control register, turbo232 only */

#define   ACIA_MODE_NORMAL    0  /* Normal ACIA emulation */
#define   ACIA_MODE_SWIFTLINK 1  /* Swiftlink ACIA emulation, baud rates are doubled */
#define   ACIA_MODE_TURBO232  2  /* Turbo232 ACIA emulation, baud rates are doubled,
                                    and enhanced baud rate register */

#endif
