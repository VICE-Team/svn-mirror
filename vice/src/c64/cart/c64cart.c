/*
 * c64cart.c - C64 cartridge emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <string.h>

#include "actionreplay3.h"
#include "actionreplay4.h"
#include "actionreplay.h"
#include "atomicpower.h"
#include "alarm.h"
#include "archdep.h"
#include "c64.h"
#include "c64-midi.h"
#include "c64acia.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64cartsystem.h"
#include "c64export.h"
#include "c64tpi.h"
#include "cartridge.h"
#include "cmdline.h"
#include "crt.h"
#include "digimax.h"
#include "dqbb.h"
#include "easyflash.h"
#include "exos.h"
#include "expert.h"
#include "final.h"
#include "finalplus.h"
#include "final3.h"
#include "freezeframe.h"
#include "freezemachine.h"
#include "gamekiller.h"
#include "generic.h"
#include "georam.h"
#include "ide64.h"
#include "interrupt.h"
#include "isepic.h"
#include "lib.h"
#include "machine.h"
#include "maincpu.h"
#include "magicvoice.h"
#include "mem.h"
#include "mmc64.h"
#include "monitor.h"
#include "prophet64.h"
#include "ramcart.h"
#include "resources.h"
#include "retroreplay.h"
#include "reu.h"
#include "mmcreplay.h"
#include "sfx_soundexpander.h"
#include "sfx_soundsampler.h"
#include "snapshot64.h"
#include "stardos.h"
#include "stb.h"
#include "superexplode5.h"
#include "supersnapshot4.h"
#include "supersnapshot.h"
#ifdef HAVE_TFE
#include "tfe.h"
#endif
#include "translate.h"
#include "tpi.h"
#include "util.h"

/* #define DEBUGCART */

#ifdef DEBUGCART
#define DBG(x)  printf x ; fflush(stdout);
#else
#define DBG(x)
#endif

/*
    as a first step to a completely generic cart system, everything should
    get reorganised based on the following assumptions:

    - it is pointless to attach/use several cartridges of the same type at
      once. by not supporting this, we can use the cartridge id as a unique
      id for a given cart, regardless of the underlaying "slot logic"
    - moreover it is also pointless to support a lot of combinations of
      cartridges, simply because they won't work anyway.

    "Slot 0"
    - carts that have a passthrough port go here
    - the passthrough of individual "Slot 0" carts must be handled on a
      per case basis.
    - if any "Slot 0" cart is active, then all following slots are assumed
      to be attached to the respective "Slot 0" passthrough port.
    - only ONE of the carts in the "Slot 0" can be active at a time

    mmc64
    Magic Voice
    ramlink, scpu, ...

    "Slot 1"
    - other ROM/RAM carts that can be enabled individually
    - any number of "Slot 1" carts can be, in theory, active at a time

    isepic
    expert
    dqbb
    ramcart

    "Main Slot"
    - the vast majority of carts go here, since only one of them
      can be used at a time anyway.
    - only ONE of the carts in the "Main Slot" can be active at a time

    this pretty much resembles the remains of the old cart system. ultimativly
    all carts should go into one of the other slots (to be completely generic),
    but it doesnt make a lot of sense to rewrite them all before passthrough-
    and mapping is handled correctly.

    "IO Slot"
    - all carts that do not use the game/exrom lines, and which do only
      map into io1/io2 go here
    - any number of "IO Slot" carts can be, in theory, active at a time

    reu
    georam
    ethernet
    acia (rs232)
    midi

    - all cards *except* those in the "Main Slot" should:
      - maintain a resource (preferably XYZCartridgeEnabled) that tells
        wether said cart is "inserted" into our virtual "expansion port expander".
      - maintain their own arrays to store rom/ram content.
      - as a consequence, changing said resource equals attaching/detaching the
        cartridge.
*/

/* global options for the cart system */
static int c64cartridge_reset; /* (resource) hardreset system after cart was attached/detached */

/* defaults for the "Main Slot" */
static char *cartridge_file = NULL; /* (resource) file name */
static int cartridge_type = CARTRIDGE_NONE; /* (resource) is == CARTRIDGE_CRT (0) if CRT file */
/* actually in use for the "Main Slot" */
static char *cartfile = NULL; /* file name */
static int c64cart_type = CARTRIDGE_NONE; /* is == CARTRIDGE_CRT (0) if CRT file */
static int crttype = CARTRIDGE_NONE; /* contains CRT ID if c64cart_type == 0 */

static alarm_t *cartridge_alarm = NULL; /* cartridge alarm context */
static unsigned int cartridge_int_num; /* irq number for cart */

/* Type of the cartridge attached. ("Main Slot") */
int mem_cartridge_type = CARTRIDGE_NONE;

/* FIXME: remove this trying... stuff and do better sanity checks
          - this function is only used from within ide64.c, this should be changed
            so it can be removed
*/
static int cartres = 0;
static int trying_cart = 0; /* FIXME: remove :) */
int try_cartridge_init(int c)
{
    cartres ^= c;

    if (cartres) {
        return 0;
    }

    trying_cart = 1;
    return cartridge_attach_image(c64cart_type, cartfile);
}

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
    int crtid;

    if (filename) {
        if (util_file_exists(filename)) {
            if((crtid=crt_getid(filename)) > 0) {
                cartridge_type = CARTRIDGE_CRT; /* resource value modified */
                return cartridge_attach_image(CARTRIDGE_CRT, filename);
            } else if ((type != CARTRIDGE_NONE) && (type != CARTRIDGE_CRT)) {
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

static int set_cartridge_reset(int val, void *param)
{
/*    DBG(("c64cartridge_reset: %d\n", val)); */
    if (c64cartridge_reset != val) {
        DBG(("c64cartridge_reset changed: %d\n", val));
        c64cartridge_reset = val; /* resource value modified */
        return try_cartridge_attach(cartridge_type, cartridge_file);
    }
    return 0;
}

/* warning: generally the order of these resources does not matter,
            however by putting them into an "ideal" order here we
            can avoid some unnecessary reinitialization at init time
*/

static const resource_int_t resources_int[] = {
    { "CartridgeReset", 1, RES_EVENT_NO, NULL,
      &c64cartridge_reset, set_cartridge_reset, NULL },
    { "CartridgeType", CARTRIDGE_NONE,
      RES_EVENT_STRICT, (resource_value_t)CARTRIDGE_NONE,
      &cartridge_type, set_cartridge_type, NULL },
    { NULL }
};

static const resource_string_t resources_string[] = {
    { "CartridgeFile", "", RES_EVENT_NO, NULL,
      &cartridge_file, set_cartridge_file, NULL },
    { NULL }
};

int cartridge_resources_init(void)
{
    /* first the general int resource, so we get the "Cartridge Reset" one */
    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    if (cart_resources_init() < 0) {
        return -1;
    }

    return resources_register_string(resources_string);
}

void cartridge_resources_shutdown(void)
{
    cart_resources_shutdown();
    /* "Main Slot" */
    lib_free(cartridge_file);
    lib_free(cartfile);
}

/* ---------------------------------------------------------------------*/
int cart_attach_cmdline(const char *param, void *extra_param)
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
    { "-cartreset", SET_RESOURCE, 0,
      NULL, NULL, "CartridgeReset", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_CART_ATTACH_DETACH_RESET,
      NULL, NULL },
    { "+cartreset", SET_RESOURCE, 0,
      NULL, NULL, "CartridgeReset", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_CART_ATTACH_DETACH_NO_RESET,
      NULL, NULL },
    /* no cartridge */
    { "+cart", CALL_FUNCTION, 0,
      cart_attach_cmdline, NULL, NULL, NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_CART,
      NULL, NULL },
    { NULL }
};

int cartridge_cmdline_options_init(void)
{
    mon_cart_cmd.cartridge_attach_image = cartridge_attach_image;
    mon_cart_cmd.cartridge_detach_image = cartridge_detach_image;
    mon_cart_cmd.cartridge_trigger_freeze = cartridge_trigger_freeze;
    mon_cart_cmd.cartridge_trigger_freeze_nmi_only = cartridge_trigger_freeze_nmi_only;
    mon_cart_cmd.export_dump = c64export_dump;

    if (cart_cmdline_options_init() < 0) {
        return -1;
    }

    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/*
    returns ID of cart in "Main Slot"
*/
int cartridge_getid_slotmain(void)
{
    if (c64cart_type == CARTRIDGE_CRT) {
        return crttype;
    }
    return c64cart_type;
}

/* ---------------------------------------------------------------------*/

/*
    get filename of cart with given type

    FIXME: only works for carts in "Main Slot"
*/
const char *cartridge_get_file_name(int type)
{
    if (cartridge_getid_slotmain() == type) {
        return cartfile;
    }
    return cart_get_file_name(type);
}

/*
    returns 1 if the cartridge of the given type is enabled

    FIXME: incomplete, currently only used by c64iec.c:iec_available_busses
    FIXME: only works for carts in "Main Slot"
*/
int cartridge_type_enabled(int type)
{
    if (cartridge_getid_slotmain() == type) {
        return 1;
    }
    return cart_type_enabled(type);
}

/*
    attach cartridge image

    type == -1  NONE
    type ==  0  CRT format

    returns -1 on error, 0 on success
*/
int cartridge_attach_image(int type, const char *filename)
{
    BYTE *rawcart;
    int carttype = CARTRIDGE_NONE;
    int cartid = CARTRIDGE_NONE;
    int oldmain = CARTRIDGE_NONE;
    int slotmain = 0;

    if (filename == NULL) {
        return -1;
    }

    /* Attaching no cartridge always works. */
    if (type == CARTRIDGE_NONE || *filename == '\0') {
        return 0;
    }

    if (type == CARTRIDGE_CRT) {
        carttype = crt_getid(filename);
    } else {
        carttype = type;
    }
    DBG(("CART: cartridge_attach_image type: %d ID: %d\n", type, carttype));

    /* allocate temporary array */
    rawcart = lib_malloc(C64CART_IMAGE_LIMIT);

/*  cart should always be detached. there is no reason for doing fancy checks
    here, and it will cause problems incase a cart MUST be detached before
    attaching another, or even itself. (eg for initialization reasons)

    most obvious reason: attaching a different ROM (software) for the same
    cartridge (hardware) */

    slotmain = cartridge_is_slotmain(carttype);
    if (slotmain) {
        /* if the cart to be attached is in the "Main Slot", detach whatever
           cart currently is in the "Main Slot" */
        oldmain = cartridge_getid_slotmain();
        if (oldmain != CARTRIDGE_NONE) {
            DBG(("CART: detach slot main ID: %d\n", oldmain));
            cartridge_detach_image(oldmain);
        }
    }
    if (oldmain != carttype) {
        DBG(("CART: detach %s ID: %d\n", slotmain ? "slot main" : "other slot", carttype));
        cartridge_detach_image(carttype);
    }

    if (type == CARTRIDGE_CRT) {
        DBG(("CART: attach CRT ID: %d '%s'\n", carttype, filename));
        cartid = crt_attach(filename, rawcart);
        if (cartid == CARTRIDGE_NONE) {
            goto exiterror;
        }
        if (type < 0) {
            DBG(("CART: attach generic CRT ID: %d\n", type));
        }
    } else {
        DBG(("CART: attach BIN ID: %d '%s'\n", carttype, filename));
        cartid = carttype;
        if (cartridge_bin_attach(carttype, filename, rawcart) < 0) {
            goto exiterror;
        }
    }

    if (cartridge_is_slotmain(cartid)) {
        DBG(("cartridge_attach MAIN ID: %d\n", cartid));
        mem_cartridge_type = cartid;
        cartridge_romhbank_set(0);
        cartridge_romlbank_set(0);
    } else {
        DBG(("cartridge_attach (other) ID: %d\n", cartid));
    }

    DBG(("CART: attach RAW ID: %d\n", cartid));
    cart_attach(cartid, rawcart);

    if (c64cartridge_reset) {
        /* "Turn off machine before inserting cartridge" */
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }

    if (cartridge_is_slotmain(cartid)) {
        /* "Main Slot" */
        DBG(("CART: set main slot ID: %d type: %d\n", carttype, type));
        c64cart_type = type;
        if (type == CARTRIDGE_CRT) {
            crttype = carttype;
        }
        util_string_set(&cartfile, filename);
    }

    lib_free(rawcart);
    return 0;

exiterror:
    DBG(("CART: error\n"));
    lib_free(rawcart);
    return -1;
}

int cartridge_enable_type(int type)
{
    /* FIXME */
    return -1;
}

/*
    detach cartridge from "Main Slot"
*/
void cartridge_detach_main(void)
{
    int type = cartridge_getid_slotmain();
    DBG(("CART: detach main %d: type: %d id: %d\n", type, c64cart_type, crttype));
    if (type != CARTRIDGE_NONE) {
        cart_detach(type);

        DBG(("CART: unset cart config\n"));
        cartridge_config_changed(CMODE_RAM, CMODE_RAM, CMODE_READ | CMODE_PHI2_RAM);

        if (c64cartridge_reset) {
            /* "Turn off machine before removing cartridge" */
            machine_trigger_reset(MACHINE_RESET_MODE_HARD);
        }

        /* reset "Main Slot" */
        mem_cartridge_type = CARTRIDGE_NONE;
        c64cart_type = CARTRIDGE_NONE;
        crttype = CARTRIDGE_NONE;
        if (cartfile) {
            lib_free(cartfile);
            cartfile = NULL;
        }
    }
}

/*
    detach a cartridge.
    - carts that are not "main" cartridges can be disabled individually
    - if type is -1, then all carts will get detached

    - carts not in "Main Slot" must make sure their detach hook does not
      fail when it is called and the cart is not actually attached.
*/
void cartridge_detach_image(int type)
{
    if (type == 0) {
        DBG(("CART: detach MAIN ID: %d\n", type));
        cartridge_detach_main();
    } else if (type == -1) {
        cartridge_detach_all();
    } else {
        DBG(("CART: detach ID: %d\n", type));
        /* detach only given type */
        if (cartridge_is_slotmain(type)) {
            cartridge_detach_main();
        } else {
            cart_detach(type);
        }
    }

    DBG(("CART: unset cart config\n"));
    cartridge_config_changed(CMODE_RAM, CMODE_RAM, CMODE_READ | CMODE_PHI2_RAM);

    if (c64cartridge_reset) {
        /* "Turn off machine before removing cartridge" */
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }
}

/*
    set currently active cart in "Main Slot" as default
*/
void cartridge_set_default(void)
{
    int type = CARTRIDGE_NONE;

    if (util_file_exists(cartfile)) {
        if (crt_getid(cartfile) > 0) {
            type = CARTRIDGE_CRT;
        } else {
            type = c64cart_type;
        }
    } else {
        DBG(("cartridge_set_default: file does not exist: '%s'\n", cartfile));
    }
    DBG(("cartridge_set_default: id %d '%s'\n", type, cartfile));

    if (type == CARTRIDGE_NONE) {
        util_string_set(&cartridge_file, ""); /* resource value modified */
    } else {
        util_string_set(&cartridge_file, cartfile); /* resource value modified */
    }
    cartridge_type = type; /* resource value modified */
}

/*
    FIXME: we should auto-detect from the filename wether to save .bin or .crt
*/
int cartridge_save_image(int type, const char *filename)
{
    /* return cartridge_bin_save(type, filename); */
    return cartridge_crt_save(type, filename);
}

/* trigger a freeze, but don't trigger the cartridge logic (which might release it). used by monitor */
void cartridge_trigger_freeze_nmi_only(void)
{
    maincpu_set_nmi(cartridge_int_num, IK_NMI);
}

/* called by individual carts */
void cartridge_release_freeze(void)
{
    maincpu_set_nmi(cartridge_int_num, 0);
}

/* called from individual carts */
void cart_trigger_nmi(void)
{
    maincpu_set_nmi(cartridge_int_num, IK_NMI);
    alarm_set(cartridge_alarm, maincpu_clk + 3);
}

/* called by c64carthooks.c:cartridge_trigger_freeze */
void cart_trigger_freeze(void)
{
    maincpu_set_nmi(cartridge_int_num, IK_NMI);
    alarm_set(cartridge_alarm, maincpu_clk + 3);
}

/* called by the NMI alarm */
static void cart_nmi_alarm_triggered(CLOCK offset, void *data)
{
    alarm_unset(cartridge_alarm);
    cart_nmi_alarm(offset, data);
}

/* called by c64.c:machine_specific_init */
void cartridge_init(void)
{
    cart_init();
    cartridge_alarm = alarm_new(maincpu_alarm_context, "Cartridge", cart_nmi_alarm_triggered, NULL);
    cartridge_int_num = interrupt_cpu_status_int_new(maincpu_int_status, "Cartridge");
}

