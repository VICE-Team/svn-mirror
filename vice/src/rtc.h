/*
 * rtc.h - Time get/set routines for RTC emulation.
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

#ifndef VICE_RTC_H
#define VICE_RTC_H

#include "types.h"

#define RTC_DEVICE_IDE64		0
#define RTC_DEVICE_SMARTMOUSE		1
#define RTC_DEVICE_BBRTC		2
#define RTC_DEVICE_RTC64		3
#define RTC_DEVICE_YTM			4
#define RTC_DEVICE_FE3			5
#define RTC_DEVICE_C64_CASSETTE	6
#define RTC_DEVICE_PET_OPTION_ROM	7
#define RTC_DEVICE_VIC20_CART_PORT	8

typedef struct rtc_devices_s {
    char *name;
    int offset;
} rtc_devices_t;

extern int rtc_load_offsets(void);

extern int rtc_get_second(int device);			/* 0 - 61 (leap seconds would be 60 and 61) */
extern int rtc_get_minute(int device);			/* 0 - 59 */
extern int rtc_get_hour(int device);			/* 0 - 23 */
extern int rtc_get_day_of_month(int device);		/* 1 - 31 */
extern int rtc_get_month(int device);			/* 0 - 11 (0 = January, 1 = Febuary ...etc) */
extern int rtc_get_year(int device);			/* 0 - 99 */
extern int rtc_get_century(int device);			/* 19 - 20 */
extern int rtc_get_weekday(int device);			/* 0 - 6 (sunday 0, monday 1 ...etc) */
extern int rtc_get_day_of_year(int device);		/* 0 - 365 */
extern int rtc_get_dst(int device);				/* 0 - >0 (0 no dst, >0 dst) */

/* these functions all set a new offset based on what is changed and the old offset
   they return -1 for failure and 0 for success. */

extern int rtc_set_second(int seconds, int device);			/* 0 - 61 (leap seconds would be 60 and 61) */
extern int rtc_set_minute(int minutes, int device);			/* 0 - 59 */
extern int rtc_set_hour(int hours, int device);				/* 0 - 23 */
extern int rtc_set_day_of_month(int day, int device);			/* 1 - 31 */
extern int rtc_set_month(int month, int device);			/* 0 - 11 */
extern int rtc_set_year(int year, int device);				/* 0 - 99 */
extern int rtc_set_century(int year, int device);			/* 0 - 99 */
extern int rtc_set_weekday(int day, int device);			/* 0 - 6 (sunday 0, monday 1 ...etc) */
extern int rtc_set_day_of_year(int day, int device);			/* 0 - 365 */

#endif
