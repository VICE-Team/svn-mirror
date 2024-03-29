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

#ifdef HAVE_TIME_T_IN_TIME_H
#include <time.h>
#endif

#ifdef HAVE_TIME_T_IN_TYPES_H
#include <sys/types.h>
#endif

enum {
    RTC_MONTH_JAN = 0,
    RTC_MONTH_FEB,
    RTC_MONTH_MAR,
    RTC_MONTH_APR,
    RTC_MONTH_MAY,
    RTC_MONTH_JUN,
    RTC_MONTH_JUL,
    RTC_MONTH_AUG,
    RTC_MONTH_SEP,
    RTC_MONTH_OCT,
    RTC_MONTH_NOV,
    RTC_MONTH_DEC
};

/* max amount of RTC's in use at the same time */
#define RTC_MAX 20

uint8_t rtc_get_centisecond(int bcd);

uint8_t rtc_get_second(time_t time_val, int bcd);         /* 0 - 61 (leap seconds would be 60 and 61) */
uint8_t rtc_get_minute(time_t time_val, int bcd);         /* 0 - 59 */
uint8_t rtc_get_hour(time_t time_val, int bcd);           /* 0 - 23 */
uint8_t rtc_get_hour_am_pm(time_t time_val, int bcd);     /* 1 - 12 + AM/PM in bit 5 (0 = AM, 1 = PM) */
uint8_t rtc_get_day_of_month(time_t time_val, int bcd);   /* 1 - 31 */
uint8_t rtc_get_month(time_t time_val, int bcd);          /* 1 - 12 (1 = January, 2 = Febuary ...etc) */
uint8_t rtc_get_year(time_t time_val, int bcd);           /* 0 - 99 */
uint8_t rtc_get_century(time_t time_val, int bcd);        /* 19 - 20 */
uint8_t rtc_get_weekday(time_t time_val);                 /* 0 - 6 (sunday 0, monday 1 ...etc) */
uint16_t rtc_get_day_of_year(time_t time_val);            /* 0 - 365 */
int rtc_get_dst(time_t time_val);                         /* 0 - >0 (0 no dst, >0 dst) */
time_t rtc_get_latch(time_t offset);

/* these functions all return a new offset based on what is changed and the old offset */

time_t rtc_set_second(int seconds, time_t offset, int bcd);     /* 0 - 61 (leap seconds would be 60 and 61) */
time_t rtc_set_minute(int minutes, time_t offset, int bcd);     /* 0 - 59 */
time_t rtc_set_hour(int hours, time_t offset, int bcd);         /* 0 - 23 */
time_t rtc_set_hour_am_pm(int hours, time_t offset, int bcd);   /* 1 - 12 + AM/PM in bit 5 (0 = AM, 1 = PM) */
time_t rtc_set_day_of_month(int day, time_t offset, int bcd);   /* 1 - 31 */
time_t rtc_set_month(int month, time_t offset, int bcd);        /* 1 - 12 */
time_t rtc_set_year(int year, time_t offset, int bcd);          /* 0 - 99 */
time_t rtc_set_century(int year, time_t offset, int bcd);       /* 19 - 20 */
time_t rtc_set_weekday(int day, time_t offset);                 /* 0 - 6 (sunday 0, monday 1 ...etc) */
time_t rtc_set_day_of_year(int day, time_t offset);             /* 0 - 365 */

/* these functions all return a new latch based on what is changed and the old latch */
time_t rtc_set_latched_second(int seconds, time_t latch, int bcd);     /* 0 - 61 (leap seconds would be 60 and 61) */
time_t rtc_set_latched_minute(int minutes, time_t latch, int bcd);     /* 0 - 59 */
time_t rtc_set_latched_hour(int hours, time_t latch, int bcd);         /* 0 - 23 */
time_t rtc_set_latched_hour_am_pm(int hours, time_t latch, int bcd);   /* 1 - 12 + AM/PM in bit 5 (0 = AM, 1 = PM) */
time_t rtc_set_latched_day_of_month(int day, time_t latch, int bcd);   /* 1 - 31 */
time_t rtc_set_latched_month(int month, time_t latch, int bcd);        /* 1 - 12 */
time_t rtc_set_latched_year(int year, time_t latch, int bcd);          /* 0 - 99 */
time_t rtc_set_latched_century(int year, time_t latch, int bcd);       /* 19 - 20 */
time_t rtc_set_latched_weekday(int day, time_t latch);                 /* 0 - 6 (sunday 0, monday 1 ...etc) */
time_t rtc_set_latched_day_of_year(int day, time_t latch);             /* 0 - 365 */

void rtc_save_context(uint8_t *ram, int ram_size, uint8_t *regs, int reg_size, char *device, time_t offset, int day_offset);
int rtc_load_context(char *device, int ram_size, int reg_size);
uint8_t *rtc_get_loaded_ram(void);
time_t rtc_get_loaded_offset(void);
int rtc_get_loaded_day_offset(void);
uint8_t *rtc_get_loaded_clockregs(void);

#endif
