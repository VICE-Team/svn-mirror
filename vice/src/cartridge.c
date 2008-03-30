/*
 * cartridge.c - Cartridge emulation.
 *
 * Written by
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
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
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <assert.h>
#include <errno.h>

#include "resources.h"
#include "file.h"
#include "mem.h"
#include "utils.h"
#include "cartridge.h"

static int cartridge_type;
static char *cartridge_file;
static int cartridge_default_enabled;

static int set_cartridge_type(resource_value_t v)
{
    cartridge_type = (int) v;
    return 0;
}

static int set_cartridge_file(resource_value_t v)
{
    const char *name = (const char *) v;

    if (cartridge_file != NULL && name != NULL
        && strcmp(name, cartridge_file) == 0)
        return 0;

    string_set(&cartridge_file, name);
    return 0;
}

static int set_cartridge_default_enabled(resource_value_t v)
{
    cartridge_default_enabled = (int) v;
    if (cartridge_default_enabled) {
	if (cartridge_attach_image(cartridge_type, cartridge_file) < 0)
	    cartridge_default_enabled = 0;
    }
    return 0;
}

static resource_t resources[] = {
    { "CartridgeType", RES_INTEGER, (resource_value_t) CARTRIDGE_NONE,
      (resource_value_t *) &cartridge_type, set_cartridge_type },
    { "CartridgeFile", RES_STRING, (resource_value_t) "/",
      (resource_value_t *) &cartridge_file, set_cartridge_file },
    { "CartridgeDefault", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &cartridge_default_enabled,
      set_cartridge_default_enabled },
    { NULL }
};

int cartridge_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

int cartridge_attach_image(int type, char *filename)
{
    BYTE rawcart[0x10000];
    FILE *fd;
    BYTE header[0x40], chipheader[0x10];
    int crttype, i;

    switch(type) {
      case CARTRIDGE_GENERIC_8KB:
	fd = fopen(filename, READ);
	if (!fd)
	    return -1;
	if (fread(rawcart, 0x2000, 1, fd) < 1) {
	    fclose(fd);
	    return -1;
	}
	fclose(fd);
	break;
      case CARTRIDGE_GENERIC_16KB:
	fd = fopen(filename, READ);
	if (!fd)
	    return -1;
	if (fread(rawcart, 0x4000, 1, fd) < 1) {
	    fclose(fd);
	    return -1;
	}
	fclose(fd);
	break;
      case CARTRIDGE_ACTION_REPLAY:
	fd = fopen(filename, READ);
	if (!fd)
	    return -1;
	if (fread(rawcart, 0x8000, 1, fd) < 1) {
	    fclose(fd);
	    return -1;
	}
	fclose(fd);
	break;
      case CARTRIDGE_CRT:
	fd = fopen(filename, READ);
	if (!fd)
	    return -1;
	if (fread(header, 0x40, 1, fd) < 1) {
	    fclose(fd);
	    return -1;
	}
	if (strncmp(header, "C64 CARTRIDGE   ", 16)) {
	    fclose(fd);
	    return -1;
	}
	crttype = header[17] + header[16] * 256;
	switch (crttype) {
	  case 0:
	    if (fread(chipheader, 0x10, 1, fd) < 1) {
		fclose(fd);
		return -1;
	    }
	    if (chipheader[0xd] == 0x80) {
		if (fread(rawcart, 0x2000, 1, fd) < 1) {
		    fclose(fd);
		    return -1;
		}
	    }
	    fclose(fd);
	    type = CARTRIDGE_GENERIC_8KB;
	    break;
	  case 1:
	    for (i = 0; i <= 3; i++) {
		if (fread(chipheader, 0x10, 1, fd) < 1) {
		    fclose(fd);
		    return -1;
		}
		if (chipheader[0xb] > 3) {
		    fclose(fd);
		    return -1;
		}
		if (fread(&rawcart[chipheader[0xb] >> 13], 0x2000, 1, fd) < 1) {
		    fclose(fd);
		    return -1;
		}
	    }
	    fclose(fd);
	    type = CARTRIDGE_ACTION_REPLAY;
	    break;
	  default:
	    fclose(fd);
	    return -1;
	}
	break;
      default:
	return -1;
    }

    set_cartridge_type((resource_value_t) type);
    set_cartridge_file((resource_value_t) filename);
    mem_attach_cartridge(type, rawcart);
    return 0;
}

void cartridge_detach_image(void)
{
    if (cartridge_type != CARTRIDGE_NONE) {
	mem_detach_cartridge(cartridge_type);
	set_cartridge_type((resource_value_t) CARTRIDGE_NONE);
    }
}

