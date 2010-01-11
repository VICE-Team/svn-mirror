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

extern int rtc_get_second(int time_val, int bcd);         /* 0 - 61 (leap seconds would be 60 and 61) */
extern int rtc_get_minute(int time_val, int bcd);         /* 0 - 59 */
extern int rtc_get_hour(int time_val, int bcd);           /* 0 - 23 */
extern int rtc_get_hour_am_pm(int time_val, int bcd);     /* 1 - 12 + AM/PM in bit 5 (0 = AM, 1 = PM) */
extern int rtc_get_day_of_month(int time_val, int bcd);   /* 1 - 31 */
extern int rtc_get_month(int time_val, int bcd);          /* 0 - 11 (0 = January, 1 = Febuary ...etc) */
extern int rtc_get_year(int time_val, int bcd);           /* 0 - 99 */
extern int rtc_get_century(int time_val, int bcd);        /* 19 - 20 */
extern int rtc_get_weekday(int time_val);                 /* 0 - 6 (sunday 0, monday 1 ...etc) */
extern int rtc_get_day_of_year(int time_val);             /* 0 - 365 */
extern int rtc_get_dst(int time_val);                     /* 0 - >0 (0 no dst, >0 dst) */
extern int rtc_get_latch(int offset);

/* these functions all return a new offset based on what is changed and the old offset */

extern int rtc_set_second(int seconds, int offset, int bcd);     /* 0 - 61 (leap seconds would be 60 and 61) */
extern int rtc_set_minute(int minutes, int offset, int bcd);     /* 0 - 59 */
extern int rtc_set_hour(int hours, int offset, int bcd);         /* 0 - 23 */
extern int rtc_set_hour_am_pm(int hours, int offset, int bcd);   /* 1 - 12 + AM/PM in bit 5 (0 = AM, 1 = PM) */
extern int rtc_set_day_of_month(int day, int offset, int bcd);   /* 1 - 31 */
extern int rtc_set_month(int month, int offset, int bcd);        /* 0 - 11 */
extern int rtc_set_year(int year, int offset, int bcd);          /* 0 - 99 */
extern int rtc_set_century(int year, int offset, int bcd);       /* 19 - 20 */
extern int rtc_set_weekday(int day, int offset);                 /* 0 - 6 (sunday 0, monday 1 ...etc) */
extern int rtc_set_day_of_year(int day, int offset);             /* 0 - 365 */

/* these functions all return a new latch based on what is changed and the old latch */
extern int rtc_set_latched_second(int seconds, int latch, int bcd);     /* 0 - 61 (leap seconds would be 60 and 61) */
extern int rtc_set_latched_minute(int minutes, int latch, int bcd);     /* 0 - 59 */
extern int rtc_set_latched_hour(int hours, int latch, int bcd);         /* 0 - 23 */
extern int rtc_set_latched_hour_am_pm(int hours, int latch, int bcd);   /* 1 - 12 + AM/PM in bit 5 (0 = AM, 1 = PM) */
extern int rtc_set_latched_day_of_month(int day, int latch, int bcd);   /* 1 - 31 */
extern int rtc_set_latched_month(int month, int latch, int bcd);        /* 0 - 11 */
extern int rtc_set_latched_year(int year, int latch, int bcd);          /* 0 - 99 */
extern int rtc_set_latched_century(int year, int latch, int bcd);       /* 19 - 20 */
extern int rtc_set_latched_weekday(int day, int latch);                 /* 0 - 6 (sunday 0, monday 1 ...etc) */
extern int rtc_set_latched_day_of_year(int day, int latch);             /* 0 - 365 */

#endif
