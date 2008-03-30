/*
 * driver-select.h - Select a printer driver.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#ifndef _DRIVER_SELECT_H
#define _DRIVER_SELECT_H

#include "types.h"

struct driver_select_s {
    const char *drv_name;
    int (*drv_open)(int device);
    void (*drv_close)(int fi);
    int (*drv_putc)(int fi, BYTE b);
    int (*drv_getc)(int fi, BYTE *b);
    int (*drv_flush)(int fi);
};
typedef struct driver_select_s driver_select_t;

extern void driver_select_init(void);
extern int driver_select_init_resources(void);
extern int driver_select_init_cmdline_options(void);

extern void driver_select_register(driver_select_t *driver_select);

extern int driver_select_open(int device);
extern void driver_select_close(int fi);
extern int driver_select_putc(int fi, BYTE b);
extern int driver_select_getc(int fi, BYTE *b);
extern int driver_select_flush(int fi);

#endif

