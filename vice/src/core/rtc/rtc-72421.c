/*
 * rtc-72421.c - RTC-72421 RTC emulation.
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

#include "rtc-72421.h"
#include "lib.h"
#include "rtc.h"
#include "snapshot.h"

#include <string.h>

/* The RTC-72421 is a 4bit address/data line RTC module,
 * The RTC has the following features:
 * - Real-Time Clock Counts seconds, minutes, hours, date of the month,
 *   months, and years
 * - All registers are decimal
 */

/* The RTC-72421 has the following clock registers:
 *
 * register 0 : bits 3-0 seconds
 *
 * register 1 : bit  3   0
 *              bits 2-0 10 seconds
 *
 * register 2 : bits 3-0 minutes
 *
 * register 3 : bit  3   0
 *              bits 2-0 10 minutes
 *
 * register 4 : bits 3-0 hours
 *
 * register 5 : bit  3   24/12 hour selection (0 = 12 hour, 1 = 24 hour)
 *              bit  2   AM/PM indication bit (1 when in 12 hour mode, otherwise 0)
 *              bits 1-0 10 hours
 *
 * register 6 : bits 3-0 day of the month
 *
 * register 7 : bits 3-2 leapyear indicator
 *              bits 1-0 10 day of the month
 *
 * register 8 : bits 3-0 months
 *
 * register 9 : bits 3-1 0
 *              bit  0   10 months
 *
 * register A : bits 3-0 years
 *
 * register B : bits 3-0 10 years
 *
 * register C : bit  3   0
 *              bits 2-0 weekdays
 *
 * register D : bits 3-0 0
 *
 * register E : bits 3-0 0
 *
 * register F : bit  3   0
 *              bit  2   AM/PM selector
 *              bit  1   Clock Halt
 *              bit  0   0
 */

/* This module is currently used in the following emulated hardware:
 * CMDHD
 * RAMLINK
 */

/* ---------------------------------------------------------------------------------------------------- */

rtc_72421_t *rtc72421_init(char *device)
{
    rtc_72421_t *retval = lib_calloc(1, sizeof(rtc_72421_t));
    int loaded = rtc_load_context(device, 0, 0);

    if (loaded) {
        retval->offset = rtc_get_loaded_offset();
        retval->day_offset = rtc_get_loaded_day_offset();
    } else {
        retval->offset = 0;
        retval->day_offset = 0;
    }
    retval->old_offset = retval->offset;
    retval->day = ( 7 + rtc_get_weekday(rtc_get_latch(retval->offset)) + retval->day_offset ) % 7;

    retval->hour24 = 0;
    retval->device = lib_strdup(device);

    return retval;
}

void rtc72421_destroy(rtc_72421_t *context, int save)
{
    if (save) {
        if (context->old_offset != context->offset) {
            rtc_save_context(NULL, 0, NULL, 0, context->device, context->offset,
                             context->day_offset);
        }
    }
    lib_free(context->device);
    lib_free(context);
}

/* ---------------------------------------------------------------------------------------------------- */

uint8_t rtc72421_read(rtc_72421_t *context, uint8_t address)
{
    uint8_t retval = 0;
    time_t latch = (context->stop) ? context->latch : rtc_get_latch(context->offset);

    switch (address & 0xf) {
        case RTC72421_REGISTER_SECONDS:
            retval = rtc_get_second(latch, 0);
            retval %= 10;
            break;
        case RTC72421_REGISTER_10SECONDS:
            retval = rtc_get_second(latch, 0);
            retval /= 10;
            break;
        case RTC72421_REGISTER_MINUTES:
            retval = rtc_get_minute(latch, 0);
            retval %= 10;
            break;
        case RTC72421_REGISTER_10MINUTES:
            retval = rtc_get_minute(latch, 0);
            retval /= 10;
            break;
        case RTC72421_REGISTER_HOURS:
            if (context->hour24) {
                retval = rtc_get_hour(latch, 0);
            } else {
                retval = rtc_get_hour_am_pm(latch, 0);
                retval &= 0x0f;
            }
            retval %= 10;
            break;
        case RTC72421_REGISTER_10HOURS:
            if (context->hour24) {
                retval = rtc_get_hour(latch, 0);
                retval /= 10;
            } else {
                retval = rtc_get_hour_am_pm(latch, 0);
                if (retval >= 32) {
                    retval = (retval & 31) / 10;
                    retval |= 4;
                } else {
                    retval /= 10;
                }
            }
            break;
        case RTC72421_REGISTER_WEEKDAYS:
            retval = rtc_get_weekday(latch);
            /* apply day offset */
            if (context->stop) {
                retval = ( 7 + retval + context->day_latch ) % 7;
            } else {
                retval = ( 7 + retval + context->day_offset ) % 7;
            }
            break;
        case RTC72421_REGISTER_MONTHDAYS:
            retval = rtc_get_day_of_month(latch, 0);
            retval %= 10;
            break;
        case RTC72421_REGISTER_10MONTHDAYS:
            retval = rtc_get_day_of_month(latch, 0);
            retval /= 10;
            break;
        case RTC72421_REGISTER_MONTHS:
            retval = rtc_get_month(latch, 0);
            retval %= 10;
            break;
        case RTC72421_REGISTER_10MONTHS:
            retval = rtc_get_month(latch, 0);
            retval /= 10;
            break;
        case RTC72421_REGISTER_YEARS:
            retval = rtc_get_year(latch, 0);
            retval %= 10;
            break;
        case RTC72421_REGISTER_10YEARS:
            retval = rtc_get_year(latch, 0);
            retval /= 10;
            break;
        case RTC72421_REGISTER_CTRL1:
            /* RAMLINK writes/reads data to this register to detect the
               presence of the rtc */
            retval = context->control[1];
            break;
        case RTC72421_REGISTER_CTRL2:
            retval = context->hour24 ? 2 : 0;
            retval |= context->stop ? 1 : 0;
            break;
    }
    return retval;
}


#define LIMIT_9(x) (x > 9) ? 9 : x

void rtc72421_write(rtc_72421_t *context, uint8_t address, uint8_t data)
{
    time_t latch = (context->stop) ? context->latch : rtc_get_latch(context->offset);
    uint8_t real_data = data & 0xf;
    uint8_t new_data;
    int8_t day;

    switch (address & 0xf) {
        case RTC72421_REGISTER_SECONDS:
            new_data = rtc_get_second(latch, 0);
            new_data /= 10;
            new_data *= 10;
            new_data += LIMIT_9(real_data);
            if (context->stop) {
                context->latch = rtc_set_latched_second(new_data, latch, 0);
            } else {
                context->offset = rtc_set_second(new_data, context->offset, 0);
            }
            break;
        case RTC72421_REGISTER_10SECONDS:
            new_data = rtc_get_second(latch, 0);
            new_data %= 10;
            new_data += ((real_data & 7) * 10);
            if (context->stop) {
                context->latch = rtc_set_latched_second(new_data, latch, 0);
            } else {
                context->offset = rtc_set_second(new_data, context->offset, 0);
            }
            break;
        case RTC72421_REGISTER_MINUTES:
            new_data = rtc_get_minute(latch, 0);
            new_data /= 10;
            new_data *= 10;
            new_data += LIMIT_9(real_data);
            if (context->stop) {
                context->latch = rtc_set_latched_minute(new_data, latch, 0);
            } else {
                context->offset = rtc_set_minute(new_data, context->offset, 0);
            }
            break;
        case RTC72421_REGISTER_10MINUTES:
            new_data = rtc_get_minute(latch, 0);
            new_data %= 10;
            new_data += ((real_data & 7) * 10);
            if (context->stop) {
                context->latch = rtc_set_latched_minute(new_data, latch, 0);
            } else {
                context->offset = rtc_set_minute(new_data, context->offset, 0);
            }
            break;
        case RTC72421_REGISTER_HOURS:
            if (context->hour24) {
                new_data = rtc_get_hour(latch, 0);
                new_data /= 10;
                new_data *= 10;
                new_data += LIMIT_9(real_data);
                if (context->stop) {
                    context->latch = rtc_set_latched_hour(new_data, latch, 0);
                } else {
                    context->offset = rtc_set_hour(new_data, context->offset, 0);
                }
            } else {
                new_data = rtc_get_hour_am_pm(latch, 0);
                if (new_data >= 32) {
                    new_data -= 32;
                    new_data /= 10;
                    new_data *= 10;
                    new_data += (LIMIT_9(real_data) + 32);
                } else {
                    new_data /= 10;
                    new_data *= 10;
                    new_data += LIMIT_9(real_data);
                }
                if (context->stop) {
                    context->latch = rtc_set_latched_hour_am_pm(new_data, latch, 0);
                } else {
                    context->offset = rtc_set_hour_am_pm(new_data, context->offset, 0);
                }
            }
            break;
        case RTC72421_REGISTER_10HOURS:
            /* FIXME: I don't think this is correct */
            if (real_data & 8) {
                new_data = rtc_get_hour(latch, 0);
                new_data %= 10;
                new_data += ((real_data & 3) * 10);
                context->hour24 = 1;
                if (context->stop) {
                    context->latch = rtc_set_latched_hour(new_data, latch, 0);
                } else {
                    context->offset = rtc_set_hour(new_data, context->offset, 0);
                }
            } else {
                real_data &= 7;
                new_data = rtc_get_hour_am_pm(latch, 0);
                if (new_data >= 32) {
                    new_data -= 32;
                }
                new_data %= 10;
                new_data += ((real_data & 3) * 10);
                if (real_data & 4) {
                    new_data += 32;
                }
                context->hour24 = 0;
                if (context->stop) {
                    context->latch = rtc_set_latched_hour_am_pm(new_data, latch, 0);
                } else {
                    context->offset = rtc_set_hour_am_pm(new_data, context->offset, 0);
                }
            }
            break;
        case RTC72421_REGISTER_WEEKDAYS:
            /* We can't figure out the offset until the full date is set, and
                we don't know how the date will be filled, so we keep a short
                term register "day" to remember and adjust the day_offset on
                every other register write. Eventually, it will be set correctly. */
            if (real_data > 6) {
                real_data = 6;
            }
            context->day = real_data;
            break;
        case RTC72421_REGISTER_MONTHDAYS:
            new_data = rtc_get_day_of_month(latch, 0);
            new_data /= 10;
            new_data *= 10;
            new_data += LIMIT_9(real_data);
            if (context->stop) {
                context->latch = rtc_set_latched_day_of_month(new_data, latch, 0);
            } else {
                context->offset = rtc_set_day_of_month(new_data, context->offset, 0);
            }
            break;
        case RTC72421_REGISTER_10MONTHDAYS:
            new_data = rtc_get_day_of_month(latch, 0);
            new_data %= 10;
            new_data += ((real_data & 3) * 10);
            if (context->stop) {
                context->latch = rtc_set_latched_day_of_month(new_data, latch, 0);
            } else {
                context->offset = rtc_set_day_of_month(new_data, context->offset, 0);
            }
            break;
        case RTC72421_REGISTER_MONTHS:
            new_data = rtc_get_month(latch, 0);
            new_data /= 10;
            new_data *= 10;
            new_data += LIMIT_9(real_data);
            if (context->stop) {
                context->latch = rtc_set_latched_month(new_data, latch, 0);
            } else {
                context->offset = rtc_set_month(new_data, context->offset, 0);
            }
            break;
        case RTC72421_REGISTER_10MONTHS:
            new_data = rtc_get_month(latch, 0);
            new_data %= 10;
            new_data += ((real_data & 1) * 10);
            if (context->stop) {
                context->latch = rtc_set_latched_month(new_data, latch, 0);
            } else {
                context->offset = rtc_set_month(new_data, context->offset, 0);
            }
            break;
        case RTC72421_REGISTER_YEARS:
            new_data = rtc_get_year(latch, 0);
            new_data /= 10;
            new_data *= 10;
            new_data += LIMIT_9(real_data);
            if (context->stop) {
                context->latch = rtc_set_latched_year(new_data, latch, 0);
            } else {
                context->offset = rtc_set_year(new_data, context->offset, 0);
            }
            break;
        case RTC72421_REGISTER_10YEARS:
            new_data = rtc_get_year(latch, 0);
            new_data %= 10;
            new_data += (LIMIT_9(real_data) * 10);
            if (context->stop) {
                context->latch = rtc_set_latched_year(new_data, latch, 0);
            } else {
                context->offset = rtc_set_year(new_data, context->offset, 0);
            }
            break;
        case RTC72421_REGISTER_CTRL0:
            context->control[0] = real_data;
            break;
        case RTC72421_REGISTER_CTRL1:
            /* RAMLINK writes/reads data to this register to detect the
               presence of the rtc */
            context->control[1] = real_data;
            break;
        case RTC72421_REGISTER_CTRL2:
            context->control[2] = real_data;
            context->hour24 = (real_data & 4) ? 1: 0;
            if (real_data & 2) {
                context->stop = 1;
                context->latch = rtc_get_latch(context->offset);
            } else {
                /* problem here, we have to do this only if we were previously
                    stopped, otherwise we mess up the counter */
                if (context->stop) {
                    context->stop = 0;
                    context->offset = context->offset -
                        (rtc_get_latch(0) - (context->latch - context->offset));
                }
            }
            break;
    }
    /* after every register write, recalculate the day_offset */
    day = rtc_get_weekday(latch);
    if (context->stop) {
        context->day_latch = context->day - day;
    } else {
        context->day_offset = context->day - day;
    }
}

/* ---------------------------------------------------------------------------------------------------- */

/* RTC_72421 snapshot module format:

   type   | name          | description
   --------------------------------
   BYTE   | stop          | stop flag
   BYTE   | 24 hours      | 24 hours flag
   BYTE   | day offset    | offset for day of week
   BYTE   | day latch     | latch for day of week
   BYTE   | day           | day of week (needed for above 2)
   DWORD  | latch hi      | high DWORD of latch offset
   DWORD  | latch lo      | low DWORD of latch offset
   DWORD  | offset hi     | high DWORD of RTC offset
   DWORD  | offset lo     | low DWORD of RTC offset
   DWORD  | old offset hi | high DWORD of old RTC offset
   DWORD  | old offset lo | low DWORD of old RTC offset
   STRING | device        | device name string
 */

static const char snap_module_name[] = "RTC_72421";
#define SNAP_MAJOR   0
#define SNAP_MINOR   0

int rtc72421_write_snapshot(rtc_72421_t *context, snapshot_t *s)
{
    uint32_t latch_lo = 0;
    uint32_t latch_hi = 0;
    uint32_t offset_lo = 0;
    uint32_t offset_hi = 0;
    uint32_t old_offset_lo = 0;
    uint32_t old_offset_hi = 0;
    snapshot_module_t *m;

    /* time_t can be either 32bit or 64bit, so we save as 64bit */
#if (SIZE_OF_TIME_T == 8)
    latch_hi = (uint32_t)(context->latch >> 32);
    latch_lo = (uint32_t)(context->latch & 0xffffffff);
    offset_hi = (uint32_t)(context->offset >> 32);
    offset_lo = (uint32_t)(context->offset & 0xffffffff);
    old_offset_hi = (uint32_t)(context->old_offset >> 32);
    old_offset_lo = (uint32_t)(context->old_offset & 0xffffffff);
#else
    latch_lo = (uint32_t)context->latch;
    offset_lo = (uint32_t)context->offset;
    old_offset_lo = (uint32_t)context->old_offset;
#endif

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_B(m, (uint8_t)context->stop) < 0
        || SMW_B(m, (uint8_t)context->hour24) < 0
        || SMW_B(m, (uint8_t)context->day_offset) < 0
        || SMW_B(m, (uint8_t)context->day_latch) < 0
        || SMW_B(m, (uint8_t)context->day) < 0
        || SMW_DW(m, latch_hi) < 0
        || SMW_DW(m, latch_lo) < 0
        || SMW_DW(m, offset_hi) < 0
        || SMW_DW(m, offset_lo) < 0
        || SMW_DW(m, old_offset_hi) < 0
        || SMW_DW(m, old_offset_lo) < 0
        || SMW_STR(m, context->device) < 0) {
        snapshot_module_close(m);
        return -1;
    }
    return snapshot_module_close(m);
}

int rtc72421_read_snapshot(rtc_72421_t *context, snapshot_t *s)
{
    uint32_t latch_lo = 0;
    uint32_t latch_hi = 0;
    uint32_t offset_lo = 0;
    uint32_t offset_hi = 0;
    uint32_t old_offset_lo = 0;
    uint32_t old_offset_hi = 0;
    uint8_t vmajor, vminor;
    snapshot_module_t *m;

    m = snapshot_module_open(s, snap_module_name, &vmajor, &vminor);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(vmajor, vminor, SNAP_MAJOR, SNAP_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (0
        || SMR_B_INT(m, &context->stop) < 0
        || SMR_B_INT(m, &context->hour24) < 0
        || SMR_B_INT(m, &context->day_offset) < 0
        || SMR_B_INT(m, &context->day_latch) < 0
        || SMR_B(m, &context->day) < 0
        || SMR_DW(m, &latch_hi) < 0
        || SMR_DW(m, &latch_lo) < 0
        || SMR_DW(m, &offset_hi) < 0
        || SMR_DW(m, &offset_lo) < 0
        || SMR_DW(m, &old_offset_hi) < 0
        || SMR_DW(m, &old_offset_lo) < 0
        || SMR_STR(m, &context->device) < 0) {
        goto fail;
    }

#if (SIZE_OF_TIME_T == 8)
    context->latch = (time_t)(latch_hi) << 32;
    context->latch |= latch_lo;
    context->offset = (time_t)(offset_hi) << 32;
    context->offset |= offset_lo;
    context->old_offset = (time_t)(old_offset_hi) << 32;
    context->old_offset |= old_offset_lo;
#else
    context->latch = latch_lo;
    context->offset = offset_lo;
    context->old_offset = old_offset_lo;
#endif

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}
