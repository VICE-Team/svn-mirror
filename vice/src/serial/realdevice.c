/*
 * realdevice.c - Real device access.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Michael Klein <nip@c64.org>
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

#ifdef HAVE_REALDEVICE

#include <stdio.h>

#include "opencbmlib.h"

#include "log.h"
#include "realdevice.h"
#include "serial.h"
#include "types.h"
#include "vsync.h"

#define DEBUG_RD


static log_t realdevice_log = LOG_DEFAULT;

static unsigned int realdevice_available = 0;

static unsigned int realdevice_enabled = 0;

static CBM_FILE realdevice_fd;

static opencbmlib_t opencbmlib;


void realdevice_open(unsigned int device, uint8_t secondary, void (*st_func)(uint8_t))
{
    vsync_suspend_speed_eval();

#ifdef DEBUG_RD
    log_debug("realdevice: OPEN DEVICE %u SECONDARY %i", device & 0x0f, secondary & 0x0f);
#endif
    (*opencbmlib.p_cbm_open)(realdevice_fd, (uint8_t)(device & 0x0f),
                             (uint8_t)(secondary & 0x0f),
                             NULL, 0);
}

void realdevice_close(unsigned int device, uint8_t secondary, void (*st_func)(uint8_t))
{
    vsync_suspend_speed_eval();

#ifdef DEBUG_RD
    log_debug("realdevice: CLOSE DEVICE %u SECONDARY %i", device & 0x0f, secondary & 0x0f);
#endif
    (*opencbmlib.p_cbm_close)(realdevice_fd, (uint8_t)(device & 0x0f),
                              (uint8_t)(secondary & 0x0f));
}

void realdevice_listen(unsigned int device, uint8_t secondary, void (*st_func)(uint8_t))
{
    vsync_suspend_speed_eval();

#ifdef DEBUG_RD
    log_debug("realdevice: LISTEN DEVICE %u SECONDARY %i", device & 0x0f,
              secondary & 0x0f);
#endif
    (*opencbmlib.p_cbm_listen)(realdevice_fd, (uint8_t)(device & 0x0f),
                               (uint8_t)(secondary & 0x0f));
}

void realdevice_talk(unsigned int device, uint8_t secondary, void (*st_func)(uint8_t))
{
    vsync_suspend_speed_eval();

#ifdef DEBUG_RD
    log_debug("realdevice: TALK DEVICE %u SECONDARY %i", device & 0x0f,
              secondary & 0x0f);
#endif
    (*opencbmlib.p_cbm_talk)(realdevice_fd, (uint8_t)(device & 0x0f),
                             (uint8_t)(secondary & 0x0f));
}

void realdevice_unlisten(void (*st_func)(uint8_t))
{
    vsync_suspend_speed_eval();

#ifdef DEBUG_RD
    log_debug("realdevice: UNLISTEN");
#endif
    (*opencbmlib.p_cbm_unlisten)(realdevice_fd);
}

void realdevice_untalk(void (*st_func)(uint8_t))
{
    vsync_suspend_speed_eval();

#ifdef DEBUG_RD
    log_debug("realdevice: UNTALK");
#endif
    (*opencbmlib.p_cbm_untalk)(realdevice_fd);
}

void realdevice_write(uint8_t data, void (*st_func)(uint8_t))
{
    uint8_t st;

#ifdef DEBUG_RD
    uint8_t mydata = data;
#endif

    vsync_suspend_speed_eval();

#ifdef DEBUG_RD
    log_debug("realdevice: WRITE DATA %02x", mydata);
#endif
    st = ((*opencbmlib.p_cbm_raw_write)(realdevice_fd, &data, 1) == 1)
         ? 0 : 0x83;

#ifdef DEBUG_RD
    log_debug("realdevice: ST %02x", st);
#endif

    st_func(st);
}

uint8_t realdevice_read(void (*st_func)(uint8_t))
{
    uint8_t st, data;

    vsync_suspend_speed_eval();

#ifdef DEBUG_RD
    log_debug("realdevice: READ %02x", data);
#endif
    st = ((*opencbmlib.p_cbm_raw_read)(realdevice_fd, &data, 1) == 1) ? 0 : 2;

#ifdef DEBUG_RD
    log_debug("realdevice: ST %02x", st);
#endif

    if ((*opencbmlib.p_cbm_get_eoi)(realdevice_fd)) {
        st |= 0x40;
    }

#ifdef DEBUG_RD
    log_debug("realdevice: READ NEWST %02x", st);
#endif

    st_func(st);

    return data;
}

void realdevice_init(void)
{
    realdevice_log = log_open("Real Device");
#ifdef DEBUG_RD
    log_debug("realdevice: realdevice_init()");
#endif

    if (opencbmlib_open(&opencbmlib) >= 0) {
        realdevice_available = 1;
    }
#ifdef DEBUG_RD
    log_debug("realdevice: realdevice_available: %u", realdevice_available);
#endif
}

void realdevice_reset(void)
{
#ifdef DEBUG_RD
    log_debug("realdevice: realdevice_reset()");
#endif
    if (realdevice_enabled) {
        (*opencbmlib.p_cbm_reset)(realdevice_fd);
    }
}

int realdevice_enable(void)
{
#ifdef DEBUG_RD
    log_debug("realdevice: realdevice_enable() realdevice_available: %u", realdevice_available);
#endif
    if (realdevice_available == 0 &&
            opencbmlib_open(&opencbmlib) >= 0) {
        realdevice_available = 1;
    }

    if (realdevice_available == 0) {
        log_message(realdevice_log, "Real device access is not available!");
        return -1;
    }

    if (realdevice_enabled == 0) {
#ifdef DEBUG_RD
        log_debug("realdevice: realdevice_enable: calling cbm_driver_open");
#endif
        if ((*opencbmlib.p_cbm_driver_open)(&realdevice_fd, 0) != 0) {
            log_message(realdevice_log,
                        "Cannot open %s, realdevice not available!",
                        (*opencbmlib.p_cbm_get_driver_name)(0));
            return -1;
        }

        log_message(realdevice_log, "%s opened.",
                    (*opencbmlib.p_cbm_get_driver_name)(0));
    }

#ifdef DEBUG_RD
    log_debug("realdevice: realdevice_enable: realdevice_enabled++");
#endif
    realdevice_enabled++;

    return 0;
}

void realdevice_disable(void)
{
#ifdef DEBUG_RD
    log_debug("realdevice: realdevice_disable() realdevice_enabled: %u", realdevice_enabled);
#endif
    if (realdevice_enabled > 0) {
        realdevice_enabled--;

        if (realdevice_enabled == 0) {
            (*opencbmlib.p_cbm_driver_close)(realdevice_fd);

            log_message(realdevice_log, "%s closed.",
                        (*opencbmlib.p_cbm_get_driver_name)(0));

            opencbmlib_close();
            realdevice_available = 0;
        }
    }
}
#endif
