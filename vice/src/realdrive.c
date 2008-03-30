/*
 * realdrive.c - cbm4linux device implementation, based on serial.c.
 *
 * Written by
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

#include <fcntl.h>
#include <unistd.h>

#include "log.h"
#include "maincpu.h"
#include "mem.h"
#include "mos6510.h"
#include "realdrive.h"
#include "traps.h"
#include "vsync.h"

#ifdef HAVE_CBM4LINUX
# include <cbm4linux.h>
#else
# include <opencbm.h>
#endif

/* Warning: these are only valid for the VIC20, C64 and C128, but *not* for
   the PET.  (FIXME?)  */
#define SET_ST(b) mem_store(0x90, (mem_read(0x90) | b))
#define BSOUR 0x95 /* Buffered Character for IEEE Bus */
#define TMP_IN 0xA4

/* Flag: Have traps been installed?  */
static int traps_installed = 0;

/* Pointer to list of traps we are using.  */
static const trap_t *realdrive_traps;

/* Logging goes here.  */
static log_t realdrive_log = LOG_ERR;

#ifdef HAVE_CBM4LINUX
static int realdrive_fd = -1;
#else
static CBM_FILE realdrive_fd;
#endif

static BYTE TrapDevice = 0;

void realdrive_receive(void)
{    
    BYTE st;
    BYTE data;

    vsync_suspend_speed_eval();

    st = (read(realdrive_fd, &data, 1) == 1) ? 0 : 2;

    if(cbm_get_eoi(realdrive_fd))
        st |= 0x40;

    if(st)
        SET_ST(st);

    mem_store(TMP_IN, data);

    MOS6510_REGS_SET_A(&maincpu_regs, data);
    MOS6510_REGS_SET_SIGN(&maincpu_regs, (data & 0x80) ? 1 : 0);
    MOS6510_REGS_SET_ZERO(&maincpu_regs, data ? 0 : 1);
    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}

void realdrive_attention(void)
{
    BYTE b, st = 0;
#ifdef HAVE_CBM4LINUX
    int (*f)(int, BYTE, BYTE) = NULL;
#else
    int (*f)(CBM_FILE, BYTE, BYTE) = NULL;
#endif

    b = mem_read(BSOUR);

    vsync_suspend_speed_eval();
    
    if(b == 0x3f) {
        cbm_unlisten(realdrive_fd);
    } else if(b == 0x5f) {
        cbm_untalk(realdrive_fd);
    } else {
        switch(b & 0xf0) {
            case 0x20:
            case 0x40:
                TrapDevice = b;
                break;

            case 0x60:
                switch(TrapDevice & 0xf0) {
                    case 0x20:
                        f = cbm_listen;
                        break;

                    case 0x40:
                        f = cbm_talk;
                        break;
                }
                break;

            case 0xe0:
                f = cbm_close;
                break;

            case 0xf0:
#ifdef HAVE_CBM4LINUX
                f = cbm_open;
#else
                st = cbm_open(realdrive_fd, TrapDevice & 0x0f, b & 0x0f, NULL, 0);
#endif
                break;

            default:
        }
        if(f)
            st = f(realdrive_fd, TrapDevice & 0x0f, b & 0x0f);
    }

    if(st)
        SET_ST(0x83);

    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}

void realdrive_send(void)
{
    BYTE data;

    data = mem_read(BSOUR);

    vsync_suspend_speed_eval();

    SET_ST((write(realdrive_fd, &data, 1) == 1) ? 0 : 0x83);
    
    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}

/* ------------------------------------------------------------------------- */

int realdrive_init(const trap_t * trap_list)
{
    if (realdrive_log == LOG_ERR)
        realdrive_log = log_open("Realdrive");

    realdrive_traps = trap_list;

    return 0;
}

int realdrive_install_traps(void)
{
#ifdef HAVE_CBM4LINUX
    if(realdrive_fd >= 0)
        close(realdrive_fd);

    realdrive_fd = open(cbm_dev, O_RDWR);
    if(realdrive_fd < 0) {
        log_warning(realdrive_log,
               "Cannot open '%s', realdrive not available", cbm_dev);
        return 1;
    }
    log_message(realdrive_log, "%s opened", cbm_dev);
#else
    if(cbm_driver_open(&realdrive_fd, 0) < 0)
    {
        log_warning(realdrive_log,
               "Cannot open %s, realdrive not available",
               cbm_get_driver_name(0));
        return 1;
    }
    log_message(realdrive_log, "%s opened", cbm_get_driver_name(0));
#endif

    if (!traps_installed && realdrive_traps != NULL) {
	const trap_t *p;

	for (p = realdrive_traps; p->func != NULL; p++)
	    traps_add(p);
	traps_installed = 1;
    }
    return 0;
}

int realdrive_remove_traps(void)
{
#ifdef HAVE_CBM4LINUX
    if(realdrive_fd >= 0) {
        close(realdrive_fd);
        log_message(realdrive_log, "%s closed", cbm_dev);
    }
#else
    cbm_driver_close(realdrive_fd);
    log_message(realdrive_log, "%s closed", cbm_get_driver_name(0));
#endif

    if (traps_installed && realdrive_traps != NULL) {
	const trap_t *p;

	for (p = realdrive_traps; p->func != NULL; p++)
	    traps_remove(p);
	traps_installed = 0;
    }
    return 0;
}

void realdrive_reset(void)
{
    cbm_reset(realdrive_fd);
}
