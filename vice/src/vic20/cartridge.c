/*
 * cartridge.c - Cartridge emulation.
 *
 * Written by
 *  Andre Fachat (fachat@physik.tu-chemnitz.de)
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
#include <fcntl.h>
#include <memory.h>
#include <assert.h>
#include <errno.h>

#include "resources.h"
#include "cmdline.h"
#include "file.h"
#include "mem.h"
#include "vmachine.h"
#include "interrupt.h"
#include "utils.h"
#include "cartridge.h"

/* Hm, if this gets more, I should introduce an array :-) */
static char *cartridge_file_2;
static char *cartridge_file_6;
static char *cartridge_file_A;
static char *cartridge_file_B;

static char *cartfile2;
static char *cartfile6;
static char *cartfileA;
static char *cartfileB;

static int set_cartridge_file_2(resource_value_t v)
{
    const char *name = (const char *) v;

    if (cartridge_file_2 != NULL && name != NULL
        && strcmp(name, cartridge_file_2) == 0)
        return 0;

    string_set(&cartridge_file_2, name);
    string_set(&cartfile2, name);
    return cartridge_attach_image(CARTRIDGE_VIC20_8KB_2000, cartfile2);
}

static int set_cartridge_file_6(resource_value_t v)
{
    const char *name = (const char *) v;

    if (cartridge_file_6 != NULL && name != NULL
        && strcmp(name, cartridge_file_6) == 0)
        return 0;

    string_set(&cartridge_file_6, name);
    string_set(&cartfile6, name);
    return cartridge_attach_image(CARTRIDGE_VIC20_8KB_6000, cartfile6);
}

static int set_cartridge_file_A(resource_value_t v)
{
    const char *name = (const char *) v;

    if (cartridge_file_A != NULL && name != NULL
        && strcmp(name, cartridge_file_A) == 0)
        return 0;

    string_set(&cartridge_file_A, name);
    string_set(&cartfileA, name);
    return cartridge_attach_image(CARTRIDGE_VIC20_8KB_A000, cartfileA);
}

static int set_cartridge_file_B(resource_value_t v)
{
    const char *name = (const char *) v;

    if (cartridge_file_B != NULL && name != NULL
        && strcmp(name, cartridge_file_B) == 0)
        return 0;

    string_set(&cartridge_file_B, name);
    string_set(&cartfileB, name);
    return cartridge_attach_image(CARTRIDGE_VIC20_4KB_B000, cartfileB);
}

static resource_t resources[] = {
    { "CartridgeFile2000", RES_STRING, (resource_value_t) "",
      (resource_value_t *) &cartridge_file_2, set_cartridge_file_2 },
    { "CartridgeFile6000", RES_STRING, (resource_value_t) "",
      (resource_value_t *) &cartridge_file_6, set_cartridge_file_6 },
    { "CartridgeFileA000", RES_STRING, (resource_value_t) "",
      (resource_value_t *) &cartridge_file_A, set_cartridge_file_A },
    { "CartridgeFileB000", RES_STRING, (resource_value_t) "",
      (resource_value_t *) &cartridge_file_B, set_cartridge_file_B },
    { NULL }
};

int cartridge_init_resources(void)
{
    return resources_register(resources);
}

static int attach_cartB(const char *param, void *extra_param)
{
    return cartridge_attach_image(CARTRIDGE_VIC20_4KB_B000, param);
}

static int attach_cartA(const char *param, void *extra_param)
{
    return cartridge_attach_image(CARTRIDGE_VIC20_8KB_A000, param);
}

static int attach_cart6(const char *param, void *extra_param)
{
    return cartridge_attach_image(CARTRIDGE_VIC20_8KB_6000, param);
}

static int attach_cart2(const char *param, void *extra_param)
{
    return cartridge_attach_image(CARTRIDGE_VIC20_8KB_2000, param);
}

static cmdline_option_t cmdline_options[] = {
    { "-cart2", CALL_FUNCTION, 1, attach_cart2, NULL, NULL, NULL,
      "<name>", "Specify 4/8K extension ROM name at $2***" },
    { "-cart6", CALL_FUNCTION, 1, attach_cart6, NULL, NULL, NULL,
      "<name>", "Specify 4/8K extension ROM name at $6***" },
    { "-cartA", CALL_FUNCTION, 1, attach_cartA, NULL, NULL, NULL,
      "<name>", "Specify 4/8K extension ROM name at $A***" },
    { "-cartB", CALL_FUNCTION, 1, attach_cartB, NULL, NULL, NULL,
      "<name>", "Specify 4K extension ROM name at $B***" },
    { NULL }
};

int cartridge_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int cartridge_attach_image(int type, const char *filename)
{
    BYTE rawcart[0x4000];
    FILE *fd;
    int addr;
    size_t n;

    /* Attaching no cartridge always works.  */
    if (type == CARTRIDGE_NONE || *filename == '\0')
	return 0;

    printf("Cartridge: Attach type %d, file=%s\n",type, filename);

    fd = fopen(filename, READ);
    if (!fd)
        return -1;

    addr = fgetc(fd);
    addr = (addr & 0xff) | ((fgetc(fd) << 8) & 0xff00);

    if (type==CARTRIDGE_VIC20_DETECT) {
	if (addr == 0x6000) {
	     type = CARTRIDGE_VIC20_8KB_6000;
	} else
	if (addr == 0xA000) {
	     type = CARTRIDGE_VIC20_8KB_A000;
	} else
	if (addr == 0x2000) {
	     type = CARTRIDGE_VIC20_8KB_2000;
	} else
	if (addr == 0xB000) {
	     type = CARTRIDGE_VIC20_4KB_B000;
	}
    }

    memset(rawcart, 0xff, 0x4000);

    switch(type) {
      case CARTRIDGE_VIC20_8KB_2000:
	if ( (n=fread(rawcart, 0x1000, 2, fd)) < 1) {
	    fclose(fd);
	    return -1;
	}
	if(n<2) {
	    /* type = CARTRIDGE_VIC20_4KB_2000; */
	    memcpy(rawcart + 0x1000, rawcart, 0x1000);
	}
	string_set(&cartfile2, filename);
	break;
      case CARTRIDGE_VIC20_8KB_6000:
	if ( (n=fread(rawcart, 0x1000, 2, fd)) < 1) {
	    fclose(fd);
	    return -1;
	}
	if(n<2) {
	    /* type = CARTRIDGE_VIC20_4KB_6000; */
	    memcpy(rawcart + 0x1000, rawcart, 0x1000);
	}
	string_set(&cartfile6, filename);
	break;
      case CARTRIDGE_VIC20_8KB_A000:
	if ( (n=fread(rawcart, 0x1000, 2, fd)) < 1) {
	    fclose(fd);
	    return -1;
	}
	if(n<2) {
	    if(cartfileB) {
	        type = CARTRIDGE_VIC20_4KB_A000;
	    } else {
	        memcpy(rawcart + 0x1000, rawcart, 0x1000);
	    }
	}
	string_set(&cartfileA, filename);
	break;
      case CARTRIDGE_VIC20_4KB_B000:
	if ( (n=fread(rawcart, 0x1000, 1, fd)) < 1) {
	    fclose(fd);
	    return -1;
	}
	string_set(&cartfileB, filename);
	break;
      default:
        fclose(fd);
	return -1;
    }

    fclose(fd);

    mem_attach_cartridge(type, rawcart);

    return 0;
}

void cartridge_detach_image(void)
{
    mem_detach_cartridge(CARTRIDGE_VIC20_8KB_2000);
    mem_detach_cartridge(CARTRIDGE_VIC20_8KB_6000);
    mem_detach_cartridge(CARTRIDGE_VIC20_8KB_A000);
    string_set(&cartfile2, NULL);
    string_set(&cartfile6, NULL);
    string_set(&cartfileA, NULL);
    string_set(&cartfileB, NULL);
}

void cartridge_set_default(void)
{
    set_cartridge_file_2((resource_value_t) cartfile2);
    set_cartridge_file_6((resource_value_t) cartfile6);
    set_cartridge_file_A((resource_value_t) cartfileA);
    set_cartridge_file_B((resource_value_t) cartfileB);
}

const char *cartridge_get_file_name(ADDRESS addr)
{
    switch (addr) {
      case 0x2000:
        return cartfile2;
      case 0x6000:
        return cartfile6;
      case 0xa000:
        return cartfileA;
      case 0xb000:
        return cartfileB;
      default:
        return NULL;
    }
}
