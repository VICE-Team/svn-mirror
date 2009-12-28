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

/* get seconds from current time + offset
   0 - 61 (leap seconds would be 60 and 61) */
int rtc_get_second(int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    return local->tm_sec;
}

/* get minutes from current time + offset
   0 - 59 */
int rtc_get_minute(int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    return local->tm_min;
}

/* get hours from current time + offset
   0 - 23 */
int rtc_get_hour(int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    return local->tm_hour;
}

/* get day of month from current time + offset
   1 - 31 */
int rtc_get_day_of_month(int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    return local->tm_mday;
}

/* get month from current time + offset
   0 - 11 */
int rtc_get_month(int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    return local->tm_mon;
}

/* get year of the century from current time + offset
   0 - 99 */
int rtc_get_year(int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    return local->tm_year % 100;
}

/* get the century from current time + offset
   19 - 20 */
int rtc_get_century(int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    return (int)(local->tm_year / 100) + 19;
}

/* get the day of the week from current time + offset
   0 - 6 (sunday 0, monday 1 ...etc) */
int rtc_get_weekday(int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    return local->tm_wday;
}

/* get the day of the year from current time + offset
   0 - 365 */
int rtc_get_day_of_year(int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    return local->tm_yday;
}

/* get the DST from current time + offset
   0 - >0 (0 no dst, >0 dst) */
int rtc_get_dst(int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);

    return local->tm_isdst;
}

/* these functions all return a new offset based on what is changed and the old offset */

/* set seconds and return new offset
   0 - 59 */
int rtc_set_second(int seconds, int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check and disregard setting of leap seconds */
    if (seconds < 0 || seconds > 59) {
        return offset;
    }
    local->tm_sec = seconds;
    offset_now = mktime(local);
    
    return offset + (offset_now - now);
}

/* set minutes and return new offset
   0 - 59 */
int rtc_set_minute(int minutes, int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check */
    if (minutes < 0 || minutes > 59) {
        return offset;
    }
    local->tm_min = minutes;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set hours and return new offset
   0 - 23 */
int rtc_set_hour(int hours, int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check */
    if (hours < 0 || hours > 23) {
        return offset;
    }
    local->tm_hour = hours;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set day of month and return new offset
   0 - 31 */
int rtc_set_day_of_month(int day, int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;
    int is_leap_year = 0;
    int year = local->tm_year + 1900;

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
            if (day < 0 || day > 31) {
                return offset;
            }
        case 3:
        case 5:
        case 8:
        case 10:
            if (day < 0 || day > 30) {
                return offset;
            }
        case 1:
            if (day < 0 || day > 28 + is_leap_year) {
                return offset;
            }
    }
    local->tm_mday = day;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set month and return new offset
   0 - 11 */
int rtc_set_month(int month, int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check */
    if (month < 0 || month > 11) {
        return offset;
    }
    local->tm_mon = month;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set years and return new offset
   0 - 99 */
int rtc_set_year(int year, int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check */
    if (year < 0 || year > 99) {
        return offset;
    }
    local->tm_year = (year / 100) * 100;
    local->tm_year += year;
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set century and return new offset
   0 - 99 */
int rtc_set_century(int century, int offset)
{
    time_t now = time(NULL) + offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check */
    if (century != 19 && century != 20) {
        return offset;
    }
    local->tm_year %= 100;
    local->tm_year += ((century - 19) * 100);
    offset_now = mktime(local);

    return offset + (offset_now - now);
}

/* set weekday and return new offset
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

/* set day of the year and return new offset
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
