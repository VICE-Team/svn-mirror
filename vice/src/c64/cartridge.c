/*
 * cartridge.c - Cartridge emulation.
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

#include "vice.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __riscos
#include <memory.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include "alarm.h"
#include "archdep.h"
#include "cartridge.h"
#include "cmdline.h"
#include "interrupt.h"
#include "maincpu.h"
#include "mem.h"
#include "resources.h"
#include "utils.h"

static int cartridge_type;
static char *cartridge_file;

int carttype = CARTRIDGE_NONE;
static int crttype = 0;
static char *cartfile;

static alarm_t cartridge_alarm;

static int set_cartridge_type(resource_value_t v)
{
    cartridge_type = (int) v;
    carttype = cartridge_type;

    return cartridge_attach_image(carttype, cartfile);
}

static int set_cartridge_file(resource_value_t v)
{
    const char *name = (const char *) v;

    if (cartridge_file != NULL && name != NULL
        && strcmp(name, cartridge_file) == 0)
        return 0;

    string_set(&cartridge_file, name);
    string_set(&cartfile, name);
    return cartridge_attach_image(carttype, cartfile);
}

static resource_t resources[] = {
    { "CartridgeType", RES_INTEGER, (resource_value_t) CARTRIDGE_NONE,
      (resource_value_t *) &cartridge_type, set_cartridge_type },
    { "CartridgeFile", RES_STRING, (resource_value_t) "",
      (resource_value_t *) &cartridge_file, set_cartridge_file },
    { NULL }
};

int cartridge_init_resources(void)
{
    return resources_register(resources);
}

static int attach_cartridge_cmdline(const char *param, void *extra_param)
{
    return cartridge_attach_image((int)extra_param, param);
}

static cmdline_option_t cmdline_options[] =
{
    {"-cartcrt", CALL_FUNCTION, 1, attach_cartridge_cmdline,
     (void *)CARTRIDGE_CRT, NULL, NULL,
     "<name>", "Attach CRT cartridge image"},
    {"-cart8", CALL_FUNCTION, 1, attach_cartridge_cmdline,
     (void *)CARTRIDGE_GENERIC_8KB, NULL, NULL,
     "<name>", "Attach generic 8KB cartridge image"},
    {"-cart16", CALL_FUNCTION, 1, attach_cartridge_cmdline,
     (void *)CARTRIDGE_GENERIC_16KB, NULL, NULL,
     "<name>", "Attach generic 16KB cartridge image"},
    {"-cartar", CALL_FUNCTION, 1, attach_cartridge_cmdline,
     (void *)CARTRIDGE_ACTION_REPLAY, NULL, NULL,
     "<name>", "Attach raw 32KB Action Replay cartridge image"},
    {"-cartap", CALL_FUNCTION, 1, attach_cartridge_cmdline,
     (void *)CARTRIDGE_ATOMIC_POWER, NULL, NULL,
     "<name>", "Attach raw 32KB Atomic Power cartridge image"},
    {"-cartepyx", CALL_FUNCTION, 1, attach_cartridge_cmdline,
     (void *)CARTRIDGE_EPYX_FASTLOAD, NULL, NULL,
     "<name>", "Attach raw 8KB Epyx fastload cartridge image"},
    {"-cartss4", CALL_FUNCTION, 1, attach_cartridge_cmdline,
     (void *)CARTRIDGE_SUPER_SNAPSHOT, NULL, NULL,
     "<name>", "Attach raw 32KB Super Snapshot cartridge image"},
    {"-cartss5", CALL_FUNCTION, 1, attach_cartridge_cmdline,
     (void *)CARTRIDGE_SUPER_SNAPSHOT_V5, NULL, NULL,
     "<name>", "Attach raw 64KB Super Snapshot cartridge image"},
    {"-cartieee488", CALL_FUNCTION, 1, attach_cartridge_cmdline,
     (void *)CARTRIDGE_IEEE488, NULL, NULL,
     "<name>", "Attach CBM IEEE488 cartridge image"},
    {"-cartwestermann", CALL_FUNCTION, 1, attach_cartridge_cmdline,
     (void *)CARTRIDGE_WESTERMANN, NULL, NULL,
     "<name>", "Attach raw 16KB Westermann learning cartridge image"},
    {NULL}
};

int cartridge_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int cartridge_attach_image(int type, const char *filename)
{
    BYTE *rawcart;
    FILE *fd;
    BYTE header[0x40], chipheader[0x10];
    int i;

    /* Attaching no cartridge always works.  */
    if (type == CARTRIDGE_NONE || *filename == '\0')
        return 0;

    /* allocate temporary array */
    rawcart = xmalloc(0x44000);

    /* Do not detach cartridge when attaching the same cart type again.  */
    if (type != carttype)
        cartridge_detach_image();

    switch(type) {
      case CARTRIDGE_GENERIC_8KB:
      case CARTRIDGE_EPYX_FASTLOAD:
        fd = fopen(filename, MODE_READ);
        if (!fd)
            goto done;
        if (file_length(fd) == 0x2002)
            fread(rawcart, 2, 1, fd);
        if (fread(rawcart, 0x2000, 1, fd) < 1) {
            fclose(fd);
            goto done;
        }
        fclose(fd);
        break;
      case CARTRIDGE_GENERIC_16KB:
      case CARTRIDGE_WESTERMANN:
        fd = fopen(filename, MODE_READ);
        if (!fd)
            goto done;
        if (file_length(fd) == 0x4002)
            fread(rawcart, 2, 1, fd);
        if (fread(rawcart, 0x4000, 1, fd) < 1) {
            fclose(fd);
            goto done;
        }
        fclose(fd);
        break;
      case CARTRIDGE_ACTION_REPLAY:
      case CARTRIDGE_ATOMIC_POWER:
        fd = fopen(filename, MODE_READ);
        if (!fd)
            goto done;
        if (fread(rawcart, 0x8000, 1, fd) < 1) {
            fclose(fd);
            goto done;
        }
        fclose(fd);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        fd = fopen(filename, MODE_READ);
        if (!fd)
            goto done;
        if (fread(rawcart, 0x8000, 1, fd) < 1) {
            fclose(fd);
            goto done;
        }
        fclose(fd);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        fd = fopen(filename, MODE_READ);
        if (!fd)
            goto done;
        if (fread(rawcart, 0x10000, 1, fd) < 1) {
            fclose(fd);
            goto done;
        }
        fclose(fd);
        break;
      case CARTRIDGE_IEEE488:
	/* FIXME: ROM removed? */
        fd = fopen(filename, MODE_READ);
        if (!fd)
            goto done;
        if (fread(rawcart, 0x1000, 1, fd) < 1) {
            fclose(fd);
            goto done;
        }
        fclose(fd);
        break;
      case CARTRIDGE_CRT:
        fd = fopen(filename, MODE_READ);
        if (!fd)
            goto done;
        if (fread(header, 0x40, 1, fd) < 1) {
            fclose(fd);
            goto done;
        }
        if (strncmp((char*)header, "C64 CARTRIDGE   ", 16)) {
            fclose(fd);
            goto done;
        }
        crttype = header[0x17] + header[0x16] * 256;
        switch (crttype) {
          case 0:
          case 11:
            if (fread(chipheader, 0x10, 1, fd) < 1) {
                fclose(fd);
                goto done;
            }
            if (chipheader[0xc] == 0x80 && chipheader[0xe] != 0
                && chipheader[0xe] <= 0x40) {
                if (fread(rawcart, chipheader[0xe] << 8, 1, fd) < 1) {
                    fclose(fd);
                    goto done;
                }
                if (crttype != 11)
                    crttype = (chipheader[0xe] <= 0x20) ? CARTRIDGE_GENERIC_8KB
                              : CARTRIDGE_GENERIC_16KB;
                fclose(fd);
                break;
            }
            if (chipheader[0xc] >= 0xe0 && chipheader[0xe] != 0
                && (chipheader[0xe] + chipheader[0xc]) == 0x100) {
                if (fread(rawcart + ((chipheader[0xc] << 8) & 0x1fff),
                    chipheader[0xe] << 8, 1, fd) < 1) {
                    fclose(fd);
                    goto done;
                }
                crttype = CARTRIDGE_ULTIMAX;
                fclose(fd);
                break;
            }
            fclose(fd);
            goto done;
          case 1:
          case 9:
            for (i = 0; i <= 3; i++) {
                if (fread(chipheader, 0x10, 1, fd) < 1) {
                    fclose(fd);
                    goto done;
                }
                if (chipheader[0xb] > 3) {
                    fclose(fd);
                    goto done;
                }
                if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1) {                    fclose(fd);
                    goto done;
                }
            }
            fclose(fd);
            break;
          case 2:
          case 4:
            for (i = 0; i <= 1; i++) {
                if (fread(chipheader, 0x10, 1, fd) < 1) {
                    fclose(fd);
                    goto done;
                }
                if (chipheader[0xc] != 0x80 && chipheader[0xc] != 0xa0) {
                    fclose(fd);
                    goto done;
                }
                if (fread(&rawcart[(chipheader[0xc] << 8) - 0x8000], 0x2000,
                          1, fd) < 1) {
                    fclose(fd);
                    goto done;
                }
            }
            fclose(fd);
            break;
          case 3:
            for (i = 0; i <= 3; i++) {
                if (fread(chipheader, 0x10, 1, fd) < 1) {
                    fclose(fd);
                    goto done;
                }
                if (chipheader[0xb] > 3) {
                    fclose(fd);
                    goto done;
                }
                if (fread(&rawcart[chipheader[0xb] << 14], 0x4000, 1, fd) < 1) {                    fclose(fd);
                    goto done;
                }
            }
            fclose(fd);
            break;
          case 5:
            while (1) {
                if (fread(chipheader, 0x10, 1, fd) < 1) {
                    fclose(fd);
                    break;
                }
                if (chipheader[0xb] >= 32 || (chipheader[0xc] != 0x80
                    && chipheader[0xc] != 0xa0)) {
                    fclose(fd);
                    goto done;
                }
                if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1) {                    fclose(fd);
                    goto done;
                }
            }
            break;
          case 7:
            while (1) {
                if (fread(chipheader, 0x10, 1, fd) < 1) {
                    fclose(fd);
                    break;
                }
                if (chipheader[0xc] != 0x80 && chipheader[0xc] != 0xa0) {
                    fclose(fd);
                    goto done;
                }
                if (fread(&rawcart[(((chipheader[0xb] >> 2) |
                    (chipheader[0xb] & 1)) & 15) << 13], 0x2000, 1, fd) < 1) {
                    fclose(fd);
                    goto done;
                }
            }
            break;
          case 8:
            while (1) {
                if (fread(chipheader, 0x10, 1, fd) < 1) {
                    fclose(fd);
                    break;
                }
                if (chipheader[0xc] != 0x80 && chipheader[0xe] != 0x40
                    && chipheader[0xb] > 3) {
                    fclose(fd);
                    goto done;
                }
                if (fread(&rawcart[chipheader[0xb] << 14], 0x4000, 1, fd) < 1) {                    fclose(fd);
                    goto done;
                }
            }
            break;
          case 10:
            if (fread(chipheader, 0x10, 1, fd) < 1) {
                fclose(fd);
                goto done;
            }
            if (fread(rawcart, 0x2000, 1, fd) < 1) {
                fclose(fd);
                goto done;
            }
            break;
          default:
            fclose(fd);
            goto done;
        }
        break;
      default:
        goto done;
    }

    carttype = type;
    string_set(&cartfile, filename);
    cartridge_attach((type == CARTRIDGE_CRT) ? crttype : type, rawcart);
    free(rawcart);
    return 0;

  done:
    free(rawcart);
    return -1;
}

void cartridge_detach_image(void)
{
    if (carttype != CARTRIDGE_NONE) {
	cartridge_detach((carttype == CARTRIDGE_CRT) ? crttype : carttype);
	carttype = CARTRIDGE_NONE;
	crttype = CARTRIDGE_NONE;
        if (cartfile != NULL)
            free(cartfile), cartfile = NULL;
    }
}

void cartridge_set_default(void)
{
    set_cartridge_type((resource_value_t) carttype);
    set_cartridge_file((resource_value_t) ((carttype == CARTRIDGE_NONE) ?
                       "" : cartfile));
}

static int cartridge_change_mapping(CLOCK offset)
{
    alarm_unset(&cartridge_alarm);

    cartridge_freeze((carttype == CARTRIDGE_CRT) ? crttype : carttype);

    return 0;
}

void cartridge_init(void)
{
    alarm_init(&cartridge_alarm, &maincpu_alarm_context,
               "Cartrigde", cartridge_change_mapping);
}

void cartridge_trigger_freeze(void)
{
    if (crttype != CARTRIDGE_ACTION_REPLAY
        && carttype != CARTRIDGE_ACTION_REPLAY
        && crttype != CARTRIDGE_KCS_POWER
        && crttype != CARTRIDGE_FINAL_III
        && carttype != CARTRIDGE_SUPER_SNAPSHOT
        && carttype != CARTRIDGE_SUPER_SNAPSHOT_V5
        && carttype != CARTRIDGE_ATOMIC_POWER)
        return;

    maincpu_set_nmi(I_FREEZE, IK_NMI);

    alarm_set(&cartridge_alarm, clk + 3);    
}

void cartridge_release_freeze(void)
{
    maincpu_set_nmi(I_FREEZE, 0);
}

const char *cartridge_get_file_name(ADDRESS addr_ignored)
{
    return cartfile;
}

