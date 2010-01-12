/*
 * rtc.c - Time get/set routines for RTC emulation.
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

#include "vice.h"

#include <time.h>

#include "rtc.h"

inline static int int_to_bcd(int dec)
{
    return ((dec / 10) << 4) + (dec % 10);
}

inline static int bcd_to_int(int bcd)
{
    return ((bcd >> 4) * 10) + bcd % 16;
}

/* ---------------------------------------------------------------------- */

/* get seconds from time value
   0 - 61 (leap seconds would be 60 and 61) */
int rtc_get_second(int time_val, int bcd)
{
    time_t now = time_val;
    struct tm *local = localtime(&now);

    return (bcd) ? int_to_bcd(local->tm_sec) : local->tm_sec;
}

/* get minutes from time value
   0 - 59 */
int rtc_get_minute(int time_val, int bcd)
{
    time_t now = time_val;
    struct tm *local = localtime(&now);

    return (bcd) ? int_to_bcd(local->tm_min) : local->tm_min;
}

/* get hours from time value
   0 - 23 */
int rtc_get_hour(int time_val, int bcd)
{
    time_t now = time_val;
    struct tm *local = localtime(&now);

    return (bcd) ? int_to_bcd(local->tm_hour) : local->tm_hour;
}

/* get hours from time_value
   1 - 12 + AM/PM flag in bit 5 (0 = PM, 1 = AM) */
int rtc_get_hour_am_pm(int time_val, int bcd)
{
    int hour;
    int pm = 0;
    time_t now = time_val;
    struct tm *local = localtime(&now);

    hour = local->tm_hour;

    if (hour == 0) {
        hour = 12;
    } else if (hour == 12) {
        pm = 1;
    } else if (hour > 12) {
        hour -= 12;
        pm = 1;
    }
    hour = (bcd) ? int_to_bcd(hour) : hour;
    hour |= (pm << 5);
    return hour;
}

/* get day of month from time value
   1 - 31 */
int rtc_get_day_of_month(int time_val, int bcd)
{
    time_t now = time_val;
    struct tm *local = localtime(&now);

    return (bcd) ? int_to_bcd(local->tm_mday) : local->tm_mday;
}

/* get month from time value
   0 - 11 */
int rtc_get_month(int time_val, int bcd)
{
    time_t now = time_val;
    struct tm *local = localtime(&now);

    return (bcd) ? int_to_bcd(local->tm_mon) : local->tm_mon;
}

/* get year of the century from time value
   0 - 99 */
int rtc_get_year(int time_val, int bcd)
{
    time_t now = time_val;
    struct tm *local = localtime(&now);

    return (bcd) ? int_to_bcd(local->tm_year % 100) : local->tm_year & 100;
}

/* get the century from time value
   19 - 20 */
int rtc_get_century(int time_val, int bcd)
{
    time_t now = time_val;
    struct tm *local = localtime(&now);

    return (bcd) ? int_to_bcd((int)(local->tm_year / 100) + 19) : (int)(local->tm_year / 100) + 19;
}

/* get the day of the week from time value
   0 - 6 (sunday 0, monday 1 ...etc) */
int rtc_get_weekday(int time_val)
{
    time_t now = time_val;
    struct tm *local = localtime(&now);

    return local->tm_wday;
}

/* get the day of the year from time value
   0 - 365 */
int rtc_get_day_of_year(int time_val)
{
    time_t now = time_val;
    struct tm *local = localtime(&now);

    return local->tm_yday;
}

/* get the DST from time value
   0 - >0 (0 no dst, >0 dst) */
int rtc_get_dst(int time_val)
{
    time_t now = time_val;
    struct tm *local = localtime(&now);

    return local->tm_isdst;
}

/* get the current clock based on time + offset so the value can be latched */
int rtc_get_latch(int offset)
{
    return time(NULL) + offset;
}

/* ---------------------------------------------------------------------- */

/* set seconds and returns new offset
   0 - 59 */
int rtc_set_second(int seconds, int offset, int bcd)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_seconds = (bcd) ? bcd_to_int(seconds) : seconds;

    /* sanity check and disregard setting of leap seconds */
    if (real_seconds < 0 || real_seconds > 59) {
        return offset;
    }
    local->tm_sec = real_seconds;
    offset_now = mktime(local);
    
    return offset + (offset_now - now);
}

/* set minutes and returns new offset
   0 - 59 */
int rtc_set_minute(int minutes, int offset, int bcd)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_minutes = (bcd) ? bcd_to_int(minutes) : minutes;

    /* sanity check */
    if (real_minutes < 0 || real_minutes > 59) {
        return offset;
    }
    local->tm_min = real_minutes;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set hours and returns new offset
   0 - 23 */
int rtc_set_hour(int hours, int offset, int bcd)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_hours = (bcd) ? bcd_to_int(hours) : hours;

    /* sanity check */
    if (real_hours < 0 || real_hours > 23) {
        return offset;
    }
    local->tm_hour = real_hours;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set hours and returns new offset
   1 - 12 and AM/PM indicator */
int rtc_set_hour_am_pm(int hours, int offset, int bcd)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_hours = (bcd) ? bcd_to_int(hours & 0x1f) : hours & 0x1f;
    int pm = (hours & 0x20) >> 5;

    if (real_hours == 12 && !pm) {
        real_hours = 0;
    } else if (real_hours == 12 && pm) {
    } else {
        real_hours += 12;
    }

    /* sanity check */
    if (real_hours < 0 || real_hours > 23) {
        return offset;
    }
    local->tm_hour = real_hours;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set day of month and returns new offset
   0 - 31 */
int rtc_set_day_of_month(int day, int offset, int bcd)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int is_leap_year = 0;
    int year = local->tm_year + 1900;
    int real_day = (bcd) ? bcd_to_int(day) : day;

    /* sanity check */
    if (((year % 4) == 0) && ((year % 100) != 0)) {
        is_leap_year = 1;
    }
    if (((year % 4) == 0) & ((year % 100) == 0) && ((year % 400) != 0)) {
        is_leap_year = 1;
    }
    switch (local->tm_mon) {
        case 0:
        case 2:
        case 4:
        case 6:
        case 7:
        case 9:
        case 11:
            if (real_day < 0 || real_day > 31) {
                return offset;
            }
        case 3:
        case 5:
        case 8:
        case 10:
            if (real_day < 0 || real_day > 30) {
                return offset;
            }
        case 1:
            if (real_day < 0 || real_day > (28 + is_leap_year)) {
                return offset;
            }
    }
    local->tm_mday = real_day;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set month and returns new offset
   0 - 11 */
int rtc_set_month(int month, int offset, int bcd)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_month = (bcd) ? bcd_to_int(month) : month;

    /* sanity check */
    if (real_month < 0 || real_month > 11) {
        return offset;
    }
    local->tm_mon = real_month;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set years and returns new offset
   0 - 99 */
int rtc_set_year(int year, int offset, int bcd)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_year = (bcd) ? bcd_to_int(year) : year;

    /* sanity check */
    if (real_year < 0 || real_year > 99) {
        return offset;
    }
    local->tm_year = (local->tm_year / 100) * 100;
    local->tm_year += real_year;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set century and returns new offset
   19 - 20 */
int rtc_set_century(int century, int offset, int bcd)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_century = (bcd) ? bcd_to_int(century) : century;

    /* sanity check */
    if (real_century != 19 && real_century != 20) {
        return offset;
    }
    local->tm_year %= 100;
    local->tm_year += ((real_century - 19) * 100);
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set weekday and returns new offset
   0 - 6 */
int rtc_set_weekday(int day, int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    /* sanity check */
    if (day < 0 || day > 6) {
        return offset;
    }
    return offset + ((day - local->tm_wday) * 24 * 60 * 60);
}

/* set day of the year and returns new offset
   0 - 365 */
int rtc_set_day_of_year(int day, int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    int is_leap_year = 0;
    int year = local->tm_year + 1900;

    /* sanity check */
    if (((year % 4) == 0) && ((year % 100) != 0)) {
        is_leap_year = 1;
    }
    if (((year % 4) == 0) & ((year % 100) == 0) && ((year % 400) != 0)) {
        is_leap_year = 1;
    }

    /* sanity check */
    if (day < 0 || day > (364 + is_leap_year)) {
        return offset;
    }
    return offset + ((day - local->tm_yday) * 24 * 60 * 60);
}

/* ---------------------------------------------------------------------- */

/* set seconds and returns new latched value
   0 - 59 */
int rtc_set_latched_second(int seconds, int latch, int bcd)
{
    time_t now = latch;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_seconds = (bcd) ? bcd_to_int(seconds) : seconds;

    /* sanity check and disregard setting of leap seconds */
    if (real_seconds < 0 || real_seconds > 59) {
        return latch;
    }
    local->tm_sec = real_seconds;
    offset_now = mktime(local);
    
    return offset_now;
}

/* set minutes and returns new latched value
   0 - 59 */
int rtc_set_latched_minute(int minutes, int latch, int bcd)
{
    time_t now = latch;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_minutes = (bcd) ? bcd_to_int(minutes) : minutes;

    /* sanity check */
    if (real_minutes < 0 || real_minutes > 59) {
        return latch;
    }
    local->tm_min = real_minutes;
    offset_now = mktime(local);

    return offset_now;
}

/* set hours and returns new latched value
   0 - 23 */
int rtc_set_latched_hour(int hours, int latch, int bcd)
{
    time_t now = latch;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_hours = (bcd) ? bcd_to_int(hours) : hours;

    /* sanity check */
    if (real_hours < 0 || real_hours > 23) {
        return latch;
    }
    local->tm_hour = real_hours;
    offset_now = mktime(local);

    return offset_now;
}

/* set hours and returns new offset
   0 - 23 */
int rtc_set_latched_hour_am_pm(int hours, int latch, int bcd)
{
    time_t now = latch;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_hours = (bcd) ? bcd_to_int(hours & 0x1f) : hours & 0x1f;
    int pm = (hours & 0x20) >> 5;

    if (real_hours == 12 && !pm) {
        real_hours = 0;
    } else if (real_hours == 12 && pm) {
    } else {
        real_hours += 12;
    }

    /* sanity check */
    if (real_hours < 0 || real_hours > 23) {
        return latch;
    }
    local->tm_hour = real_hours;
    offset_now = mktime(local);

    return offset_now;
}

/* set day of month and returns new latched value
   0 - 31 */
int rtc_set_latched_day_of_month(int day, int latch, int bcd)
{
    time_t now = latch;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int is_leap_year = 0;
    int year = local->tm_year + 1900;
    int real_day = (bcd) ? bcd_to_int(day) : day;

    /* sanity check */
    if (((year % 4) == 0) && ((year % 100) != 0)) {
        is_leap_year = 1;
    }
    if (((year % 4) == 0) & ((year % 100) == 0) && ((year % 400) != 0)) {
        is_leap_year = 1;
    }
    switch (local->tm_mon) {
        case 0:
        case 2:
        case 4:
        case 6:
        case 7:
        case 9:
        case 11:
            if (real_day < 0 || real_day > 31) {
                return latch;
            }
        case 3:
        case 5:
        case 8:
        case 10:
            if (real_day < 0 || real_day > 30) {
                return latch;
            }
        case 1:
            if (real_day < 0 || real_day > (28 + is_leap_year)) {
                return latch;
            }
    }
    local->tm_mday = real_day;
    offset_now = mktime(local);

    return offset_now;
}

/* set month and returns new latched value
   0 - 11 */
int rtc_set_latched_month(int month, int latch, int bcd)
{
    time_t now = latch;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_month = (bcd) ? bcd_to_int(month) : month;

    /* sanity check */
    if (real_month < 0 || real_month > 11) {
        return latch;
    }
    local->tm_mon = real_month;
    offset_now = mktime(local);

    return offset_now;
}

/* set years and returns new latched value
   0 - 99 */
int rtc_set_latched_year(int year, int latch, int bcd)
{
    time_t now = latch;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_year = (bcd) ? bcd_to_int(year) : year;

    /* sanity check */
    if (real_year < 0 || real_year > 99) {
        return latch;
    }
    local->tm_year = (local->tm_year / 100) * 100;
    local->tm_year += real_year;
    offset_now = mktime(local);

    return offset_now;
}

/* set century and returns new latched value
   19 - 20 */
int rtc_set_latched_century(int century, int latch, int bcd)
{
    time_t now = latch;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int real_century = (bcd) ? bcd_to_int(century) : century;

    /* sanity check */
    if (real_century != 19 && real_century != 20) {
        return latch;
    }
    local->tm_year %= 100;
    local->tm_year += ((real_century - 19) * 100);
    offset_now = mktime(local);

    return offset_now;
}

/* set weekday and returns new latched value
   0 - 6 */
int rtc_set_latched_weekday(int day, int latch)
{
    time_t now = latch;
    struct tm *local = localtime(&now);

    /* sanity check */
    if (day < 0 || day > 6) {
        return latch;
    }
    return latch + ((day - local->tm_wday) * 24 * 60 * 60);
}

/* set day of the year and returns new latched value
   0 - 365 */
int rtc_set_latched_day_of_year(int day, int latch)
{
    time_t now = latch;
    struct tm *local = localtime(&now);
    int is_leap_year = 0;
    int year = local->tm_year + 1900;

    /* sanity check */
    if (((year % 4) == 0) && ((year % 100) != 0)) {
        is_leap_year = 1;
    }
    if (((year % 4) == 0) & ((year % 100) == 0) && ((year % 400) != 0)) {
        is_leap_year = 1;
    }

    /* sanity check */
    if (day < 0 || day > (364 + is_leap_year)) {
        return latch;
    }
    return latch + ((day - local->tm_yday) * 24 * 60 * 60);
}
