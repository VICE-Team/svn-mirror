/*
 * pcf8583.h - PCF8583 RTC emulation.
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

#ifndef VICE_PCF8583_H
#define VICE_PCF8583_H

#include "types.h"

typedef struct rtc_pcf8583_s rtc_pcf8583_t;

extern rtc_pcf8583_t *pcf8583_init(char *device);
extern void pcf8583_destroy(rtc_pcf8583_t *context, int save);

extern void pcf8583_set_clk_line(rtc_pcf8583_t *context, BYTE data);
extern void pcf8583_set_data_line(rtc_pcf8583_t *context, BYTE data);

extern BYTE pcf8583_read_data_line(rtc_pcf8583_t *context);

#endif
