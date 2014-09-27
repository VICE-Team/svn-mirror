/*
 * hardsid.c
 *
 * Written by
 *  Simon White <sidplay2@yahoo.com>
 *
 * This file is part of VICE, modified from the sidplay2 sources.  It is
 * a one for all driver with real timing support via real time kernel
 * extensions or through the hardware buffering.  It supports the hardsid
 * isa/pci single/quattro and also the catweasel MK3/4.
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

#ifdef HAVE_HARDSID

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/hardsid.h>

#include "alarm.h"
#include "hardsid.h"
#include "log.h"
#include "maincpu.h"
#include "sid-resources.h"
#include "types.h"

// Approx 3 PAL screen updates
#define HARDSID_DELAY_CYCLES 50000

static int hsid_fd = -1;
static CLOCK hsid_main_clk;
static CLOCK hsid_alarm_clk;
static alarm_t *hsid_alarm = 0;

static void hardsid_alarm_handler(CLOCK offset, void *data);

static int hardsid_init(void)
{
    /* Already open */
    if (hsid_fd >= 0) {
        return -1;
    }

    /* Open device */
    hsid_fd = open("/dev/sid", O_RDWR);
    if (hsid_fd < 0) {
        log_error(LOG_DEFAULT, "Could not open sid device /dev/sid.");
        return -1;
    }

    /* Make sure we have atleast sid */
    if (ioctl(hsid_fd, HSID_IOCTL_ALLOCATED, 0) == 0) {
        log_error(LOG_DEFAULT, "Could not find any SID installed.");
        close(hsid_fd);
        hsid_fd = -1;
        return -1;
    }
    return 0;
}

void hardsid_reset(void)
{
    hsid_main_clk  = maincpu_clk;
    hsid_alarm_clk = HARDSID_DELAY_CYCLES;
    alarm_set(hsid_alarm, HARDSID_DELAY_CYCLES);
}

int hardsid_open(void)
{
    if (hardsid_init() < 0) {
        return -1;
    }
    hsid_alarm = alarm_new(maincpu_alarm_context, "hardsid", hardsid_alarm_handler, 0);
    hardsid_reset();
    return 0;
}

int hardsid_close(void)
{
    /* Driver cleans up after itself */
    if (hsid_fd >= 0) {
        close(hsid_fd);
    }
    alarm_destroy(hsid_alarm);
    hsid_alarm = 0;
    return 0;
}

int hardsid_read(WORD addr, int chipno)
{
    if (hsid_fd >= 0) {
        CLOCK cycles = maincpu_clk - hsid_main_clk - 1;
        hsid_main_clk = maincpu_clk;

        while (cycles > 0xffff) {
            /* delay */
            ioctl(hsid_fd, HSID_IOCTL_DELAY, 0xffff);
            cycles -= 0xffff;
        }

        {
            uint packet = ((cycles & 0xffff) << 16) | ((addr & 0x1f) << 8);
            ioctl(hsid_fd, HSID_IOCTL_READ, &packet);
            return (int)packet;
        }
    }
    return 0;
}

void hardsid_store(WORD addr, BYTE val, int chipno)
{
    if (hsid_fd >= 0)
    {
        CLOCK cycles = maincpu_clk - hsid_main_clk - 1;
        hsid_main_clk = maincpu_clk;

        while (cycles > 0xffff) {
            /* delay */
            ioctl(hsid_fd, HSID_IOCTL_DELAY, 0xffff);
            cycles -= 0xffff;
        }

        uint packet = ((cycles & 0xffff) << 16) | ((addr & 0x1f) << 8) | val;
        write(hsid_fd, &packet, sizeof (packet));
    }
}

void hardsid_set_machine_parameter(long cycles_per_sec)
{
}

unsigned int hardsid_available(void)
{
    if (hardsid_init() < 0) {
        return 0;
    }

    /* Say one for now */
    return 1;
}

void hardsid_alarm_handler (CLOCK offset, void *data)
{
    CLOCK cycles = (hsid_alarm_clk + offset) - hsid_main_clk;

    if (cycles < HARDSID_DELAY_CYCLES) {
        hsid_alarm_clk = hsid_main_clk + HARDSID_DELAY_CYCLES;
    } else {
        uint delay = (uint) cycles;
        ioctl(hsid_fd, HSID_IOCTL_DELAY, delay);
        hsid_main_clk   = maincpu_clk - offset;
        hsid_alarm_clk  = hsid_main_clk + HARDSID_DELAY_CYCLES;
    }
    alarm_set(hsid_alarm, hsid_alarm_clk);
}

void hardsid_set_device(unsigned int chipno, unsigned int device)
{
}

#endif
