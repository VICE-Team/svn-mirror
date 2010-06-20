/*
 * c64cartsystem.h -- C64 cartridge emulation, internal stuff
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

#ifndef VICE_C64CARTSYSTEM_H
#define VICE_C64CARTSYSTEM_H

/*
    these are the functions which are only shared internally by the cartridge
    system, meaning c64cart.c, c64cartmem.c, c64carthooks, c64export and the
    individual cartridge implementations
*/

#include "types.h"

/* from c64cart.c */
extern int cart_attach_cmdline(const char *param, void *extra_param);

extern void cart_trigger_freeze(void);
extern void cart_trigger_nmi(void);
extern void cart_detach_main(void);

extern int cart_getid_slotmain(void); /* returns ID of cart in "Main Slot" */

/* from c64carthooks.c */
extern void cart_nmi_alarm(CLOCK offset, void *data);

extern void cart_init(void);
extern int cart_resources_init(void);
extern void cart_resources_shutdown(void);
extern int cart_cmdline_options_init(void);

extern const char *cart_get_file_name(int type);
extern int cart_is_slotmain(int type); /* returns 1 if cart of given type is in "Main Slot" */
extern int cart_type_enabled(int type);

extern void cart_attach(int type, BYTE *rawcart);
extern int cart_bin_attach(int type, const char *filename, BYTE *rawcart);
extern void cart_detach(int type);
extern void cart_detach_all(void);

extern void cart_detach_conflicting(int type);

#endif
