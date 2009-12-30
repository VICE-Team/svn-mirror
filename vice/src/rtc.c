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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "archdep.h"
#include "lib.h"
#include "log.h"
#include "rtc.h"
#include "sysfile.h"
#include "util.h"

static rtc_devices_t rtc_devices[] = {
    { "RTC_DEVICE_IDE64", 0 },
    { "RTC_DEVICE_SMARTMOUSE", 0 },
    { "RTC_DEVICE_BBRTC", 0 },
    { "RTC_DEVICE_RTC64", 0 },
    { "RTC_DEVICE_YTM", 0 },
    { "RTC_DEVICE_FE3", 0 },
    { "RTC_DEVICE_C64_CASSETTE", 0 },
    { "RTC_DEVICE_PET_OPTION_ROM", 0 },
    { "RTC_DEVICE_VIC20_CART_PORT", 0 },
    { NULL, 0 }
};

static void set_offsets(char *buffer)
{
    int counter = 0;
    int length = strlen(buffer);
    int i, found;

    while (counter < length) {
        if (buffer[counter] == '#') {
            /* comment line, skip it */
            while (buffer[counter] != '\n') {
                counter++;
            }
        }
        if (buffer[counter] == '\n') {
            /* empty line, skip it */
            counter++;
        }
        found = 0;
        for (i = 0; rtc_devices[i].name != NULL && found == 0; i++) {
            if (!strncasecmp(buffer + counter, rtc_devices[i].name, strlen(rtc_devices[i].name))) {
                rtc_devices[i].offset = atoi(buffer + counter);
                found = -1;
            }
        }
        /* skip rest of line */
        while (buffer[counter] != '\n') {
            counter++;
        }
    }
}

/* load the rtc offset file if available */
int rtc_load_offsets(void)
{
    FILE *offsets_file;
    char *complete_path;
    char *buffer = NULL;
    int filelen;

    offsets_file = sysfile_open("offsets.rtc", &complete_path, MODE_READ_TEXT);
    if (offsets_file == NULL) {
        log_error(LOG_DEFAULT, "Warning. Cannot open RTC offsets file offsets.rtc.");
        lib_free(complete_path);
        return -1;
    }
    filelen = util_file_length(offsets_file);
    lib_free(complete_path);
    buffer = lib_malloc(filelen);
    if (fread(buffer, 1, filelen, offsets_file) != (unsigned int)filelen) {
        fclose(offsets_file);
        lib_free(buffer);
        return -1;
    }
    fclose(offsets_file);
    set_offsets(buffer);
    lib_free(buffer);
    return 0;
}

static int rtc_save_offsets(void)
{
    FILE *offsets_file;
    char *complete_path;
    int i;

    offsets_file = sysfile_open("offsets.rtc", &complete_path, MODE_WRITE_TEXT);
    lib_free(complete_path);
    if (offsets_file == NULL) {
        log_error(LOG_DEFAULT, "Warning. Cannot open RTC offsets file offsets.rtc for writing.");
        return -1;
    }
    for (i = 0; rtc_devices[i].name != NULL; i++) {
        fprintf(offsets_file, "# RTC offsets file, do not edit!\n\n");
        fprintf(offsets_file, "%s %d\n", rtc_devices[i].name, rtc_devices[i].offset);
    }
    fclose(offsets_file);
    return 0;
}

/* get seconds from current time + offset
   0 - 61 (leap seconds would be 60 and 61) */
int rtc_get_second(int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    return local->tm_sec;
}

/* get minutes from current time + offset
   0 - 59 */
int rtc_get_minute(int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    return local->tm_min;
}

/* get hours from current time + offset
   0 - 23 */
int rtc_get_hour(int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    return local->tm_hour;
}

/* get day of month from current time + offset
   1 - 31 */
int rtc_get_day_of_month(int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    return local->tm_mday;
}

/* get month from current time + offset
   0 - 11 */
int rtc_get_month(int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    return local->tm_mon;
}

/* get year of the century from current time + offset
   0 - 99 */
int rtc_get_year(int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    return local->tm_year % 100;
}

/* get the century from current time + offset
   19 - 20 */
int rtc_get_century(int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    return (int)(local->tm_year / 100) + 19;
}

/* get the day of the week from current time + offset
   0 - 6 (sunday 0, monday 1 ...etc) */
int rtc_get_weekday(int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    return local->tm_wday;
}

/* get the day of the year from current time + offset
   0 - 365 */
int rtc_get_day_of_year(int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    return local->tm_yday;
}

/* get the DST from current time + offset
   0 - >0 (0 no dst, >0 dst) */
int rtc_get_dst(int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    return local->tm_isdst;
}

/* set seconds and sets new offset
   0 - 59 */
int rtc_set_second(int seconds, int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check and disregard setting of leap seconds */
    if (seconds < 0 || seconds > 59) {
        return -1;
    }
    local->tm_sec = seconds;
    offset_now = mktime(local);
    
    rtc_devices[device].offset += (offset_now - now);
    rtc_save_offsets();
    return 0;
}

/* set minutes and sets new offset
   0 - 59 */
int rtc_set_minute(int minutes, int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check */
    if (minutes < 0 || minutes > 59) {
        return -1;
    }
    local->tm_min = minutes;
    offset_now = mktime(local);

    rtc_devices[device].offset += (offset_now - now);
    rtc_save_offsets();
    return 0;
}

/* set hours and sets new offset
   0 - 23 */
int rtc_set_hour(int hours, int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check */
    if (hours < 0 || hours > 23) {
        return -1;
    }
    local->tm_hour = hours;
    offset_now = mktime(local);

    rtc_devices[device].offset += (offset_now - now);
    rtc_save_offsets();
    return 0;
}

/* set day of month and sets new offset
   0 - 31 */
int rtc_set_day_of_month(int day, int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
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
                return -1;
            }
        case 3:
        case 5:
        case 8:
        case 10:
            if (day < 0 || day > 30) {
                return -1;
            }
        case 1:
            if (day < 0 || day > 28 + is_leap_year) {
                return -1;
            }
    }
    local->tm_mday = day;
    offset_now = mktime(local);

    rtc_devices[device].offset += (offset_now - now);
    rtc_save_offsets();
    return 0;
}

/* set month and sets new offset
   0 - 11 */
int rtc_set_month(int month, int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check */
    if (month < 0 || month > 11) {
        return -1;
    }
    local->tm_mon = month;
    offset_now = mktime(local);

    rtc_devices[device].offset += (offset_now - now);
    rtc_save_offsets();
    return 0;
}

/* set years and sets new offset
   0 - 99 */
int rtc_set_year(int year, int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check */
    if (year < 0 || year > 99) {
        return -1;
    }
    local->tm_year = (year / 100) * 100;
    local->tm_year += year;
    offset_now = mktime(local);

    rtc_devices[device].offset += (offset_now - now);
    rtc_save_offsets();
    return 0;
}

/* set century and sets new offset
   0 - 99 */
int rtc_set_century(int century, int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);
    time_t offset_now;

    /* sanity check */
    if (century != 19 && century != 20) {
        return -1;
    }
    local->tm_year %= 100;
    local->tm_year += ((century - 19) * 100);
    offset_now = mktime(local);

    rtc_devices[device].offset += (offset_now - now);
    rtc_save_offsets();
    return 0;
}

/* set weekday and sets new offset
   0 - 6 */
int rtc_set_weekday(int day, int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
    struct tm *local = localtime(&now);

    /* sanity check */
    if (day < 0 || day > 6) {
        return -1;
    }
    rtc_devices[device].offset += ((day - local->tm_wday) * 24 * 60 * 60);
    rtc_save_offsets();
    return 0;
}

/* set day of the year and return new offset
   0 - 365 */
int rtc_set_day_of_year(int day, int device)
{
    time_t now = time(NULL) + rtc_devices[device].offset;
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
        return -1;
    }
    rtc_devices[device].offset += ((day - local->tm_yday) * 24 * 60 * 60);
    rtc_save_offsets();
    return 0;
}
