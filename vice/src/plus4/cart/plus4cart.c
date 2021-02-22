/*
 * plus4cart.c -- Plus4 cartridge handling.
 *
 * Written by
 *  Tibor Biczo <crown@axelero.hu>
 *  Groepaz <groepaz@gmx.net>
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

#define DEBUGCART

#include "vice.h"

#include <string.h>

#include "archdep.h"
#include "cartio.h"
#include "cartridge.h"
#include "cmdline.h"
#include "plus4cart.h"
#include "plus4mem.h"
#include "lib.h"
#include "log.h"
#include "util.h"
#include "machine.h"
#include "monitor.h"
#include "resources.h"
#include "sysfile.h"

#include "debugcart.h"
#include "jacint1mb.h"
#include "magiccart.h"
#include "multicart.h"
#include "plus4-generic.h"

#ifdef DEBUGCART
#define DBG(x)  log_debug x
#else
#define DBG(x)
#endif

/*
basic   internal                $8000-$bfff basic
kernal  internal                $c000-$ffff kernal
c0lo    internal                $8000-$bfff function rom low (only Plus4)
c0hi    internal                $c000-$ffff function rom high (only Plus4)
c1lo    exp. port               $8000-$bfff cartridge rom low
c1hi    exp. port               $c000-$ffff cartridge rom high
c2lo    exp. port (or internal) $8000-$bfff reserved / v364 speech software low
c2hi    exp. port (or internal) $c000-$ffff reserved / v364 speech software high

TED controls all banking. The cs0 and cs1 are active when the CPU is
accessing $8000-$bfff and $c000-$ffff respectively, but can be “overridden”
by writing anything to TED registers $3e and $3f. Writing anything to $FF3E
will page in the currently configured ROMs to the upper memory area
($8000..$FFFF), and writing anything to $FF3F will page in RAM to the same
area.

ROM Banking is also explained on page 73 of
http://www.zimmers.net/anonftp/pub/cbm/schematics/computers/plus4/264_Hardware_Spec.pdf
*/

/* global options for the cart system */
static int plus4cartridge_reset; /* (resource) hardreset system after cart was attached/detached */

/* defaults */
static char *cartridge_file = NULL; /* (resource) file name */
static int cartridge_type = CARTRIDGE_NONE; /* (resource) is == CARTRIDGE_CRT (0) if CRT file */

/* actually in use */
static char *cartfile = NULL; /* file name */
static int plus4cart_type = CARTRIDGE_NONE; /* is == CARTRIDGE_CRT (0) if CRT file */
static int crttype = CARTRIDGE_NONE; /* contains CRT ID if plus4cart_type == 0 */

static int mem_cartridge_type = CARTRIDGE_NONE;  /* Type of the cartridge attached. */

/* ---------------------------------------------------------------------*/

static cartridge_info_t cartlist[] = {
    /* standard cartridges with CRT ID = 0 */
    /* { CARTRIDGE_NAME_GENERIC_8KB,         CARTRIDGE_GENERIC_8KB,         CARTRIDGE_GROUP_GENERIC }, */

    /* all cartridges with a CRT ID > 0, alphabetically sorted */
    { CARTRIDGE_PLUS4_NAME_JACINT1MB,         CARTRIDGE_PLUS4_JACINT1MB,         CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_PLUS4_NAME_MAGIC,             CARTRIDGE_PLUS4_MAGIC,             CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_PLUS4_NAME_MULTI,             CARTRIDGE_PLUS4_MULTI,             CARTRIDGE_GROUP_UTIL },

    { NULL, 0, 0 }
};

cartridge_info_t *cartridge_get_info_list(void)
{
    return &cartlist[0];
}

/* ---------------------------------------------------------------------*/

static int cart_attach_cmdline(const char *param, void *extra_param)
{
    int type = vice_ptr_to_int(extra_param);

    /* NULL param is used for +cart */
    if (!param) {
        cartridge_detach_image(-1);
        return 0;
    }
    return cartridge_attach_image(type, param);
}

static const cmdline_option_t cmdline_options[] =
{
    /* hardreset on cartridge change */
    { "-cartreset", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "CartridgeReset", (void *)1,
      NULL, "Reset machine if a cartridge is attached or detached" },
    { "+cartreset", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "CartridgeReset", (void *)0,
      NULL, "Do not reset machine if a cartridge is attached or detached" },
    /* smart attach */
    { "-cart", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void*)CARTRIDGE_PLUS4_DETECT, NULL, NULL,
      "<Name>", "Smart-attach cartridge image" },
    /* seperate cartridge types */
    { "-cartjacint", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void*)CARTRIDGE_PLUS4_JACINT1MB, NULL, NULL,
      "<Name>", "Attach 1MiB " CARTRIDGE_PLUS4_NAME_JACINT1MB " image" },
    { "-cartmagic", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void*)CARTRIDGE_PLUS4_MAGIC, NULL, NULL,
      "<Name>", "Attach 512kiB/1MiB/2MiB " CARTRIDGE_PLUS4_NAME_MAGIC " image" },
    { "-cartmulti", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void*)CARTRIDGE_PLUS4_MULTI, NULL, NULL,
      "<Name>", "Attach 1MiB/2MiB " CARTRIDGE_PLUS4_NAME_MULTI " image" },
    /* no cartridge */
    { "+cart", CALL_FUNCTION, CMDLINE_ATTRIB_NONE,
      cart_attach_cmdline, NULL, NULL, NULL,
      NULL, "Disable default cartridge" },
    CMDLINE_LIST_END
};

int cartridge_cmdline_options_init(void)
{
    mon_cart_cmd.cartridge_attach_image = cartridge_attach_image;
    mon_cart_cmd.cartridge_detach_image = cartridge_detach_image;
#if 0
    mon_cart_cmd.cartridge_trigger_freeze = cartridge_trigger_freeze;
    mon_cart_cmd.cartridge_trigger_freeze_nmi_only = cartridge_trigger_freeze_nmi_only;
    mon_cart_cmd.export_dump = plus4export_dump;
#endif
    if (generic_cmdline_options_init() < 0) {
        return -1;
    }

    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

/*
    we have 3 resources for the main cart that may be changed in arbitrary order:

    - cartridge type
    - cartridge file name
    - cartridge change reset behaviour

    the following functions try to deal with this in a hopefully sane way... however,
    do _NOT_ change the used resources from the (G)UI directly. (used the set_default
    function instead)
*/

static int try_cartridge_attach(int type, const char *filename)
{
    if (filename) {
        if (util_file_exists(filename)) {
#if 0
            if (crt_getid(filename) > 0) {
                cartridge_type = CARTRIDGE_CRT; /* resource value modified */
                return cartridge_attach_image(CARTRIDGE_CRT, filename);
            } else
#endif
            if ((type != CARTRIDGE_NONE) && (type != CARTRIDGE_CRT)) {
                cartridge_type = type; /* resource value modified */
                return cartridge_attach_image(type, filename);
            }
        } else {
            DBG(("cartridge_file does not exist: '%s'\n", filename));
        }
    }

    return 0;
}

static int set_cartridge_type(int val, void *param)
{
    switch (val) {
        case CARTRIDGE_NONE:
        /* case CARTRIDGE_CRT: */

        case CARTRIDGE_PLUS4_JACINT1MB:
        case CARTRIDGE_PLUS4_MAGIC:
        case CARTRIDGE_PLUS4_MULTI:
            break;
        default:
            return -1;
    }

/*    DBG(("cartridge_type: %d\n", val)); */
    if (cartridge_type != val) {
        DBG(("cartridge_type changed: %d\n", val));
        cartridge_type = val;
        return try_cartridge_attach(cartridge_type, cartridge_file);
    }

    return 0;
}

/*
*/
static int set_cartridge_file(const char *name, void *param)
{
/*    DBG(("cartridge_file: '%s'\n", name)); */
    if (cartridge_file == NULL) {
        util_string_set(&cartridge_file, ""); /* resource value modified */
    }

    if (!strcmp(cartridge_file, name)) {
        return 0;
    }

    if (name == NULL || !strlen(name)) {
        cartridge_detach_image(-1);
        return 0;
    }

    DBG(("cartridge_file changed: '%s'\n", name));

    if (util_file_exists(name)) {
        util_string_set(&cartridge_file, name); /* resource value modified */
        return try_cartridge_attach(cartridge_type, cartridge_file);
    } else {
        DBG(("cartridge_file does not exist: '%s'\n", name));
        cartridge_type = CARTRIDGE_NONE; /* resource value modified */
        util_string_set(&cartridge_file, ""); /* resource value modified */
    }

    return 0;
}

static int set_cartridge_reset(int value, void *param)
{
    int val = value ? 1 : 0;

/*    DBG(("plus4cartridge_reset: %d", val)); */
    if (plus4cartridge_reset != val) {
        DBG(("plus4cartridge_reset changed: %d", val));
        plus4cartridge_reset = val; /* resource value modified */
    }
    return 0;
}

static const resource_int_t resources_int[] = {
    { "CartridgeReset", 1, RES_EVENT_NO, NULL,
      &plus4cartridge_reset, set_cartridge_reset, NULL },
    { "CartridgeType", CARTRIDGE_NONE,
      RES_EVENT_STRICT, (resource_value_t)CARTRIDGE_NONE,
      &cartridge_type, set_cartridge_type, NULL },
    RESOURCE_INT_LIST_END
};

static const resource_string_t resources_string[] = {
    { "CartridgeFile", "", RES_EVENT_NO, NULL,
      &cartridge_file, set_cartridge_file, NULL },
    RESOURCE_STRING_LIST_END
};

int cartridge_resources_init(void)
{
    /* first the general int resource, so we get the "Cartridge Reset" one */
    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    if (generic_resources_init() < 0) {
        return -1;
    }

    return resources_register_string(resources_string);
}

void cartridge_resources_shutdown(void)
{
    generic_resources_shutdown();

    lib_free(cartridge_file);
    lib_free(cartfile);
}

/* ---------------------------------------------------------------------*/
/* expansion port memory read/write hooks */
uint8_t plus4cart_c1lo_read(uint16_t addr)
{
    switch (mem_cartridge_type) {
        case CARTRIDGE_PLUS4_JACINT1MB:
            return jacint1mb_c1lo_read(addr);
        case CARTRIDGE_PLUS4_MAGIC:
            return magiccart_c1lo_read(addr);
        case CARTRIDGE_PLUS4_MULTI:
            return multicart_c1lo_read(addr);
    }
    /* FIXME: when no cartridge is attached, we will probably read open i/o */
    return generic_c1lo_read(addr);
}

uint8_t plus4cart_c1hi_read(uint16_t addr)
{
    switch (mem_cartridge_type) {
        case CARTRIDGE_PLUS4_MULTI:
            return multicart_c1hi_read(addr);
    }
    /* FIXME: when no cartridge is attached, we will probably read open i/o */
    return generic_c1hi_read(addr);
}

void cartridge_mmu_translate(unsigned int addr, uint8_t **base, int *start, int *limit)
{
    *base = NULL;
    *start = 0;
    *limit = 0;
}

/* ---------------------------------------------------------------------*/
/*
    called by plus4.c:machine_specific_reset (calls XYZ_reset)
*/
void cartridge_reset(void)
{
    /* cart_unset_alarms(); */
    /* cart_reset_memptr(); */
    switch (mem_cartridge_type) {
        case CARTRIDGE_PLUS4_JACINT1MB:
            return jacint1mb_reset();
        case CARTRIDGE_PLUS4_MAGIC:
            return magiccart_reset();
        case CARTRIDGE_PLUS4_MULTI:
            return multicart_reset();
    }
}

static void cart_power_off(void)
{
    if (plus4cartridge_reset) {
        /* "Turn off machine before removing cartridge" */
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }
}
/* ---------------------------------------------------------------------*/

/* FIXME: get rid of this ugly hack */
extern int plus4_rom_loaded;

int plus4cart_load_func_lo(const char *rom_name)
{
    if (!plus4_rom_loaded) {
        return 0;
    }

    /* Load 3plus1 low ROM.  */
    if (*rom_name != 0) {
        if (sysfile_load(rom_name, machine_name, extromlo1, PLUS4_CART16K_SIZE, PLUS4_CART16K_SIZE) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load 3plus1 low ROM `%s'.",
                      rom_name);
            return -1;
        }
    } else {
        memset(extromlo1, 0, PLUS4_CART16K_SIZE);
    }
    return 0;
}

int plus4cart_load_func_hi(const char *rom_name)
{
    if (!plus4_rom_loaded) {
        return 0;
    }

    /* Load 3plus1 high ROM.  */
    if (*rom_name != 0) {
        if (sysfile_load(rom_name, machine_name, extromhi1, PLUS4_CART16K_SIZE, PLUS4_CART16K_SIZE) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load 3plus1 high ROM `%s'.",
                      rom_name);
            return -1;
        }
    } else {
        memset(extromhi1, 0, PLUS4_CART16K_SIZE);
    }
    return 0;
}

/* FIXME: c2lo/hi can be external or internal */
int plus4cart_load_c2lo(const char *rom_name)
{
    if (!plus4_rom_loaded) {
        return 0;
    }

    /* Load c2 low ROM.  */
    if (*rom_name != 0) {
        if (sysfile_load(rom_name, machine_name, extromlo3, PLUS4_CART16K_SIZE, PLUS4_CART16K_SIZE) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load cartridge 2 low ROM `%s'.",
                      rom_name);
            return -1;
        }
    } else {
        memset(extromlo3, 0, PLUS4_CART16K_SIZE);
    }
    return 0;
}

/* FIXME: c2lo/hi can be external or internal */
int plus4cart_load_c2hi(const char *rom_name)
{
    if (!plus4_rom_loaded) {
        return 0;
    }

    /* Load c2 high ROM.  */
    if (*rom_name != 0) {
        if (sysfile_load(rom_name, machine_name, extromhi3, PLUS4_CART16K_SIZE, PLUS4_CART16K_SIZE) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load cartridge 2 high ROM `%s'.",
                      rom_name);
            return -1;
        }
    } else {
        memset(extromhi3, 0, PLUS4_CART16K_SIZE);
    }
    return 0;
}

void plus4cart_detach_cartridges(void)
{
    resources_set_string("c2loName", "");
    resources_set_string("c2hiName", "");
    memset(extromlo3, 0, PLUS4_CART16K_SIZE);
    memset(extromhi3, 0, PLUS4_CART16K_SIZE);
#if 0
    generic_detach();
    jacint1mb_detach();
    magiccart_detach();
    multicart_detach();
#endif
    mem_cartridge_type = CARTRIDGE_NONE;

    cart_power_off();
}

void cartridge_detach_image(int type)
{
    if (type < 0) {
        plus4cart_detach_cartridges();
    } else {
        switch (type) {
            /* FIXME: we probably shouldnt be able to detach these individually */
            case CARTRIDGE_PLUS4_16KB_C1LO:
                resources_set_string("c1loName", "");
                break;
            case CARTRIDGE_PLUS4_16KB_C1HI:
                resources_set_string("c1hiName", "");
                break;
            case CARTRIDGE_PLUS4_16KB_C2LO:
                resources_set_string("c2loName", "");
                break;
            case CARTRIDGE_PLUS4_16KB_C2HI:
                resources_set_string("c2hiName", "");
                break;

            case CARTRIDGE_PLUS4_JACINT1MB:
                jacint1mb_detach();
                break;
            case CARTRIDGE_PLUS4_MAGIC:
                magiccart_detach();
                break;
            case CARTRIDGE_PLUS4_MULTI:
                multicart_detach();
                break;
        }
        cart_power_off();
    }
}

/*
    set currently active cart as default
*/
void cartridge_set_default(void)
{
    int type = CARTRIDGE_NONE;

    if (cartfile != NULL) {
        if (util_file_exists(cartfile)) {
#if 0
            if (crt_getid(cartfile) > 0) {
                type = CARTRIDGE_CRT;
            } else 
#endif
            {
                type = plus4cart_type;
            }
        } else {
            DBG(("cartridge_set_default: file does not exist: '%s'\n", cartfile ? cartfile : "NULL"));
        }
    } else {
        DBG(("cartridge_set_default: no filename\n"));
    }
    DBG(("cartridge_set_default: id %d '%s'\n", type, cartfile ? cartfile : "NULL"));

    if (type == CARTRIDGE_NONE) {
        util_string_set(&cartridge_file, ""); /* resource value modified */
    } else {
        util_string_set(&cartridge_file, cartfile); /* resource value modified */
    }
    cartridge_type = type; /* resource value modified */
}

/** \brief  Wipe "default cartidge"
 */
void cartridge_unset_default(void)
{
    util_string_set(&cartridge_file, "");
    cartridge_type = CARTRIDGE_NONE;
}

/* FIXME: this is kinda broken and wrong, remove */
int cartridge_detect(const char *filename)
{
    int type = CARTRIDGE_NONE;
    FILE *fd;
    size_t len;
    unsigned char b[0x100];

    fd = fopen(filename, "rb");
    if (fd == NULL) {
        return CARTRIDGE_NONE;
    }
    len = util_file_length(fd);

    /* there are so few carts that this little thing actually works :) */
    if (len == 8192) {
        type = CARTRIDGE_PLUS4_16KB_C1LO;
    } else if (len == 16384) {
        type = CARTRIDGE_PLUS4_16KB_C1LO;
        fseek(fd, 10, SEEK_SET);
        if (fread(b, 1, 0x100, fd) < 0x100) {
            fclose(fd);
            return CARTRIDGE_NONE;
        }
        /* Octasoft Basic v7 */
        if (!strncmp("SYS1546: BASIC V7.0 ON KEY F2", (const char*)b, 29)) {
            type = CARTRIDGE_PLUS4_16KB_C2LO;
        }
    } else if (len == 32768) {
        type = CARTRIDGE_PLUS4_32KB_C1;
    } else if (len == 49152) {
        type = CARTRIDGE_PLUS4_NEWROM;
    }

    fclose (fd);

    DBG(("detected cartridge type: %04x", (unsigned int)type));

    return type;
}

int cart_bin_attach(int type, const char *filename, uint8_t *rawcart)
{
    if ((type & 0xff00) == CARTRIDGE_PLUS4_DETECT) {
        return generic_bin_attach(type, filename);
    }

    switch (type) {
        case CARTRIDGE_PLUS4_JACINT1MB:
            return jacint1mb_bin_attach(filename, rawcart);
        case CARTRIDGE_PLUS4_MAGIC:
            return magiccart_bin_attach(filename, rawcart);
        case CARTRIDGE_PLUS4_MULTI:
            return multicart_bin_attach(filename, rawcart);
    }
    log_error(LOG_DEFAULT,
              "cartridge_bin_attach: unsupported type (%04x)", (unsigned int)type);
    return -1;
}

/*
    called by cartridge_attach_image after cart_crt/bin_attach
    XYZ_config_setup should copy the raw cart image into the
    individual implementations array.
*/
void cart_attach(int type, uint8_t *rawcart)
{
    /* cart_detach_conflicting(type); */
    switch (type) {
        case CARTRIDGE_PLUS4_JACINT1MB:
            jacint1mb_config_setup(rawcart);
            break;
        case CARTRIDGE_PLUS4_MAGIC:
            magiccart_config_setup(rawcart);
            break;
        case CARTRIDGE_PLUS4_MULTI:
            multicart_config_setup(rawcart);
            break;
    }
}

/*
    attach cartridge image

    type == -1  NONE
    type ==  0  CRT format

    returns -1 on error, 0 on success
*/

int cartridge_attach_image(int type, const char *filename)
{
    unsigned char *rawcartdata;  /* raw cartridge data while loading/attaching */
    char *abs_filename;
    int cartid = type; /* FIXME: this will get the crtid */
    int carttype = CARTRIDGE_NONE;
    /* FIXME: we should convert the intermediate type to generic type 0 */

    if (filename == NULL) {
        return -1;
    }

    /* Attaching no cartridge always works. */
    if (type == CARTRIDGE_NONE || *filename == '\0') {
        return 0;
    }

    if (archdep_path_is_relative(filename)) {
        archdep_expand_path(&abs_filename, filename);
    } else {
        abs_filename = lib_strdup(filename);
    }

    DBG(("CART: detach slot main ID: %d\n", mem_cartridge_type));
    cartridge_detach_image(mem_cartridge_type);

    if (type == CARTRIDGE_PLUS4_DETECT) {
        type = cartridge_detect(filename);
    }
#if 0 /* FIXME */
    if (type == CARTRIDGE_CRT) {
        carttype = crt_getid(abs_filename);
        if (carttype == -1) {
            log_message(LOG_DEFAULT, "CART: '%s' is not a valid CRT file.", abs_filename);
            lib_free(abs_filename);
            return -1;
        }
    } else 
#endif
    {
        carttype = type;
    }
    DBG(("CART: cartridge_attach_image type: %d ID: %d\n", type, cartid));

    /* allocate temporary array */
    rawcartdata = lib_malloc(PLUS4CART_IMAGE_LIMIT);

#if 0
    if (type == CARTRIDGE_CRT) {
        DBG(("CART: attach CRT ID: %d '%s'\n", carttype, filename));
        cartid = crt_attach(abs_filename, rawcart);
        if (cartid == CARTRIDGE_NONE) {
            goto exiterror;
        }
        if (type < 0) {
            DBG(("CART: attach generic CRT ID: %d\n", type));
        }
    } else 
#endif
    {
       DBG(("CART: attach BIN ID: %d '%s'\n", carttype, filename));
        cartid = carttype;
        if (cart_bin_attach(cartid, abs_filename, rawcartdata) < 0) {
            goto exiterror;
        }
    }

    mem_cartridge_type = cartid;

    cart_attach(type, rawcartdata);
    cart_power_off();

    DBG(("CART: set ID: %d type: %d\n", carttype, type));
    plus4cart_type = type;
    if (type == CARTRIDGE_CRT) {
        crttype = carttype;
    }
    util_string_set(&cartfile, abs_filename);

    DBG(("CART: cartridge_attach_image type: %d ID: %d done.\n", type, cartid));
    lib_free(rawcartdata);
    log_message(LOG_DEFAULT, "CART: attached '%s' as ID %d.", filename, cartid);
    return 0;

exiterror:
    DBG(("CART: error\n"));
    lib_free(rawcartdata);
    log_message(LOG_DEFAULT, "CART: could not attach '%s'.", filename);
    return -1;
}

/* FIXME: todo */
void cartridge_trigger_freeze(void)
{
    int delay = lib_unsigned_rand(1, (unsigned int)machine_get_cycles_per_frame());
#if 0
    cart_freeze_alarm_time = maincpu_clk + delay;
    alarm_set(cartridge_freeze_alarm, cart_freeze_alarm_time);
#endif
    DBG(("cartridge_trigger_freeze delay %d cycles\n", delay));
}
