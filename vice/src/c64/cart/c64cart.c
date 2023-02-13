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

#ifndef RAND_MAX
#include <limits.h>
#define RAND_MAX INT_MAX
#endif

#include "alarm.h"
#include "archdep.h"
#include "c64.h"
#include "c64cart.h"
#define CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64cartsystem.h"
#undef CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "cartridge.h"
#include "cmdline.h"
#include "crt.h"
#include "export.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "monitor.h"
#include "resources.h"
#include "util.h"

#define CARTRIDGE_INCLUDE_PRIVATE_API
#include "actionreplay.h"
#include "actionreplay2.h"
#include "actionreplay3.h"
#include "actionreplay4.h"
#include "atomicpower.h"
#include "bisplus.h"
#include "blackbox3.h"
#include "blackbox4.h"
#include "blackbox8.h"
#include "blackbox9.h"
#include "c64-generic.h"
#include "c64tpi.h"
#include "comal80.h"
#include "capture.h"
#include "delaep256.h"
#include "delaep64.h"
#include "delaep7x8.h"
#include "diashowmaker.h"
#include "dinamic.h"
#include "easycalc.h"
#include "easyflash.h"
#include "epyxfastload.h"
#include "exos.h"
#include "expert.h"
#include "final.h"
#include "finalplus.h"
#include "final3.h"
#include "formel64.h"
#include "freezeframe.h"
#include "freezeframe2.h"
#include "freezemachine.h"
#include "funplay.h"
#include "gamekiller.h"
#include "gmod2.h"
#include "gmod3.h"
#include "gs.h"
#include "drean.h"
#include "ide64.h"
#include "ieeeflash64.h"
#include "isepic.h"
#include "kcs.h"
#include "kingsoft.h"
#include "ltkernal.h"
#include "mach5.h"
#include "magicdesk.h"
#include "magicformel.h"
#include "magicvoice.h"
#include "maxbasic.h"
#include "mikroass.h"
#include "mmc64.h"
#include "mmcreplay.h"
#include "multimax.h"
#include "ocean.h"
#include "pagefox.h"
#include "partner64.h"
#include "prophet64.h"
#include "ramlink.h"
#include "retroreplay.h"
#include "rexep256.h"
#include "rexramfloppy.h"
#include "rexutility.h"
#include "rgcd.h"
#include "rrnetmk3.h"
#include "ross.h"
#include "sdbox.h"
#include "silverrock128.h"
#include "simonsbasic.h"
#include "stardos.h"
#include "stb.h"
#include "snapshot64.h"
#include "supergames.h"
#include "supersnapshot4.h"
#include "supersnapshot.h"
#include "superexplode5.h"
#include "turtlegraphics.h"
#include "warpspeed.h"
#include "westermann.h"
#include "zaxxon.h"
#include "zippcode48.h"
#undef CARTRIDGE_INCLUDE_PRIVATE_API

#define DEBUGCART

#ifdef DEBUGCART
#define DBG(x)  printf x; fflush(stdout);
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
    ieee488
    ramlink
    (scpu, ...)

    "Slot 1"
    - other ROM/RAM carts that can be enabled individually
    - only ONE of the carts in the "Slot 1" can be active at a time

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

/* pointer to access c128 specific functions in x128 */
c128cartridge_interface_t *c128cartridge = NULL;

/* global options for the cart system */
static int c64cartridge_reset; /* (resource) hardreset system after cart was attached/detached */

/* defaults for the "Main Slot" */
static char *cartridge_file = NULL; /* (resource) file name */
static int cartridge_type = CARTRIDGE_NONE; /* (resource) is == CARTRIDGE_CRT (0) if CRT file */
/* actually in use for the "Main Slot" */
static char *cartfile = NULL; /* file name */
static int c64cart_type = CARTRIDGE_NONE; /* is == CARTRIDGE_CRT (0) if CRT file */
static int crttype = CARTRIDGE_NONE; /* contains CRT ID if c64cart_type == 0 */

static alarm_t *cartridge_nmi_alarm = NULL; /* cartridge nmi alarm context */
static alarm_t *cartridge_freeze_alarm = NULL; /* cartridge freeze button alarm context */
static unsigned int cartridge_int_num; /* irq number for cart */

CLOCK cart_nmi_alarm_time = CLOCK_MAX; /* cartridge NMI alarm time */
CLOCK cart_freeze_alarm_time = CLOCK_MAX; /* cartridge freeze button alarm time */

/* Type of the cartridge attached. ("Main Slot") */
int mem_cartridge_type = CARTRIDGE_NONE;

/* CAUTION: keep in sync with the list in c128/c128cart.c */
static cartridge_info_t cartlist[] = {
    /* standard cartridges with CRT ID = 0 */
    { CARTRIDGE_NAME_GENERIC_8KB,         CARTRIDGE_GENERIC_8KB,         CARTRIDGE_GROUP_GENERIC },
    { CARTRIDGE_NAME_GENERIC_16KB,        CARTRIDGE_GENERIC_16KB,        CARTRIDGE_GROUP_GENERIC },
    { CARTRIDGE_NAME_ULTIMAX,             CARTRIDGE_ULTIMAX,             CARTRIDGE_GROUP_GENERIC },

    /* all cartridges with a CRT ID > 0, alphabetically sorted */
    { CARTRIDGE_NAME_ACTION_REPLAY,       CARTRIDGE_ACTION_REPLAY,       CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_ACTION_REPLAY2,      CARTRIDGE_ACTION_REPLAY2,      CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_ACTION_REPLAY3,      CARTRIDGE_ACTION_REPLAY3,      CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_ACTION_REPLAY4,      CARTRIDGE_ACTION_REPLAY4,      CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_ATOMIC_POWER,        CARTRIDGE_ATOMIC_POWER,        CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_BISPLUS,             CARTRIDGE_BISPLUS,             CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_BLACKBOX3,           CARTRIDGE_BLACKBOX3,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_BLACKBOX4,           CARTRIDGE_BLACKBOX4,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_BLACKBOX8,           CARTRIDGE_BLACKBOX8,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_BLACKBOX9,           CARTRIDGE_BLACKBOX9,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_CAPTURE,             CARTRIDGE_CAPTURE,             CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_COMAL80,             CARTRIDGE_COMAL80,             CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_DELA_EP256,          CARTRIDGE_DELA_EP256,          CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_DELA_EP64,           CARTRIDGE_DELA_EP64,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_DELA_EP7x8,          CARTRIDGE_DELA_EP7x8,          CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_DIASHOW_MAKER,       CARTRIDGE_DIASHOW_MAKER,       CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_DINAMIC,             CARTRIDGE_DINAMIC,             CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_EASYCALC,            CARTRIDGE_EASYCALC,            CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_EASYFLASH,           CARTRIDGE_EASYFLASH,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_EPYX_FASTLOAD,       CARTRIDGE_EPYX_FASTLOAD,       CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_EXOS,                CARTRIDGE_EXOS,                CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_EXPERT,              CARTRIDGE_EXPERT,              CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_FINAL_I,             CARTRIDGE_FINAL_I,             CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_FINAL_III,           CARTRIDGE_FINAL_III,           CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_FINAL_PLUS,          CARTRIDGE_FINAL_PLUS,          CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_FORMEL64,            CARTRIDGE_FORMEL64,            CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_FREEZE_FRAME,        CARTRIDGE_FREEZE_FRAME,        CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_FREEZE_FRAME_MK2,    CARTRIDGE_FREEZE_FRAME_MK2,    CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_FREEZE_MACHINE,      CARTRIDGE_FREEZE_MACHINE,      CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_FUNPLAY,             CARTRIDGE_FUNPLAY,             CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_GAME_KILLER,         CARTRIDGE_GAME_KILLER,         CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_GMOD2,               CARTRIDGE_GMOD2,               CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_GMOD3,               CARTRIDGE_GMOD3,               CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_GS,                  CARTRIDGE_GS,                  CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_DREAN,               CARTRIDGE_DREAN,               CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_IDE64,               CARTRIDGE_IDE64,               CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_IEEE488,             CARTRIDGE_IEEE488,             CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_IEEEFLASH64,         CARTRIDGE_IEEEFLASH64,         CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_KCS_POWER,           CARTRIDGE_KCS_POWER,           CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_KINGSOFT,            CARTRIDGE_KINGSOFT,            CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_LT_KERNAL,           CARTRIDGE_LT_KERNAL,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_MACH5,               CARTRIDGE_MACH5,               CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_MAGIC_DESK,          CARTRIDGE_MAGIC_DESK,          CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_MAGIC_FORMEL,        CARTRIDGE_MAGIC_FORMEL,        CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_MAGIC_VOICE,         CARTRIDGE_MAGIC_VOICE,         CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_MAX_BASIC,           CARTRIDGE_MAX_BASIC,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_MIKRO_ASSEMBLER,     CARTRIDGE_MIKRO_ASSEMBLER,     CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_MMC64,               CARTRIDGE_MMC64,               CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_MMC_REPLAY,          CARTRIDGE_MMC_REPLAY,          CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_MULTIMAX,            CARTRIDGE_MULTIMAX,            CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_OCEAN,               CARTRIDGE_OCEAN,               CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_P64,                 CARTRIDGE_P64,                 CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_PAGEFOX,             CARTRIDGE_PAGEFOX,             CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_PARTNER64,           CARTRIDGE_PARTNER64,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_RAMLINK,             CARTRIDGE_RAMLINK,             CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_RETRO_REPLAY,        CARTRIDGE_RETRO_REPLAY,        CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_REX,                 CARTRIDGE_REX,                 CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_REX_EP256,           CARTRIDGE_REX_EP256,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_REX_RAMFLOPPY,       CARTRIDGE_REX_RAMFLOPPY,       CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_RGCD,                CARTRIDGE_RGCD,                CARTRIDGE_GROUP_GAME },
#ifdef HAVE_RAWNET
    { CARTRIDGE_NAME_RRNETMK3,            CARTRIDGE_RRNETMK3,            CARTRIDGE_GROUP_UTIL },
#endif
    { CARTRIDGE_NAME_ROSS,                CARTRIDGE_ROSS,                CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_SDBOX,               CARTRIDGE_SDBOX,               CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_SILVERROCK_128,      CARTRIDGE_SILVERROCK_128,      CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_SIMONS_BASIC,        CARTRIDGE_SIMONS_BASIC,        CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_SNAPSHOT64,          CARTRIDGE_SNAPSHOT64,          CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_STARDOS,             CARTRIDGE_STARDOS,             CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_STRUCTURED_BASIC,    CARTRIDGE_STRUCTURED_BASIC,    CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_SUPER_EXPLODE_V5,    CARTRIDGE_SUPER_EXPLODE_V5,    CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_SUPER_GAMES,         CARTRIDGE_SUPER_GAMES,         CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_SUPER_SNAPSHOT,      CARTRIDGE_SUPER_SNAPSHOT,      CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_SUPER_SNAPSHOT_V5,   CARTRIDGE_SUPER_SNAPSHOT_V5,   CARTRIDGE_GROUP_FREEZER },
    { CARTRIDGE_NAME_TURTLE_GRAPHICS_II,  CARTRIDGE_TURTLE_GRAPHICS_II,  CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_WARPSPEED,           CARTRIDGE_WARPSPEED,           CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_WESTERMANN,          CARTRIDGE_WESTERMANN,          CARTRIDGE_GROUP_UTIL },
    { CARTRIDGE_NAME_ZAXXON,              CARTRIDGE_ZAXXON,              CARTRIDGE_GROUP_GAME },
    { CARTRIDGE_NAME_ZIPPCODE48,          CARTRIDGE_ZIPPCODE48,          CARTRIDGE_GROUP_UTIL },

    /* cartridges that have only RAM, these do not have a CRT ID */
    { CARTRIDGE_NAME_DQBB,                CARTRIDGE_DQBB,                CARTRIDGE_GROUP_RAMEX },
    { CARTRIDGE_NAME_GEORAM,              CARTRIDGE_GEORAM,              CARTRIDGE_GROUP_RAMEX },
    { CARTRIDGE_NAME_ISEPIC,              CARTRIDGE_ISEPIC,              CARTRIDGE_GROUP_RAMEX },
    { CARTRIDGE_NAME_RAMCART,             CARTRIDGE_RAMCART,             CARTRIDGE_GROUP_RAMEX },
    { CARTRIDGE_NAME_REU,                 CARTRIDGE_REU,                 CARTRIDGE_GROUP_RAMEX },

    { NULL, 0, 0 }
};

cartridge_info_t *cartridge_get_info_list(void)
{
    if (machine_class == VICE_MACHINE_C128) {
        return &c128cartridge->get_info_list()[0];
    }
    return &cartlist[0];
}

/* FIXME: this only works in slot 0 right now */
int cartridge_get_id(int slot)
{
    int type = cart_getid_slotmain();
    /*DBG(("cartridge_get_id(slot:%d): type:%d\n", slot, type));*/
    return type;
}

/* FIXME: terrible name, we already have cartridge_get_file_name */
char *cartridge_get_filename(int slot)
{
    DBG(("cartridge_get_filename(slot:%d)\n", slot));
/*    return cart_get_file_name(mem_cartridge_type); */
    int type = cart_getid_slotmain();
    if (cart_getid_slotmain() == type && !cart_can_get_file_name(type)) {
        return cartfile;
    }
    return (char*)cart_get_file_name(type);
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
    if (filename) {
        if (util_file_exists(filename)) {
            if (crt_getid(filename) >= 0) {
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
    DBG(("set_cartridge_type: %d\n", val));
    switch (val) {
        case CARTRIDGE_ULTIMAX:
        case CARTRIDGE_GENERIC_8KB:
        case CARTRIDGE_GENERIC_16KB:
        case CARTRIDGE_NONE:
        case CARTRIDGE_CRT:

        case CARTRIDGE_ACTION_REPLAY:
        case CARTRIDGE_ACTION_REPLAY2:
        case CARTRIDGE_ACTION_REPLAY3:
        case CARTRIDGE_ACTION_REPLAY4:
        case CARTRIDGE_ATOMIC_POWER:
        case CARTRIDGE_BISPLUS:
        case CARTRIDGE_BLACKBOX3:
        case CARTRIDGE_BLACKBOX4:
        case CARTRIDGE_BLACKBOX8:
        case CARTRIDGE_BLACKBOX9:
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_COMAL80:
        case CARTRIDGE_DELA_EP64:
        case CARTRIDGE_DELA_EP7x8:
        case CARTRIDGE_DELA_EP256:
        case CARTRIDGE_DIASHOW_MAKER:
        case CARTRIDGE_DINAMIC:
        case CARTRIDGE_EASYCALC:
        case CARTRIDGE_EASYFLASH:
        case CARTRIDGE_EASYFLASH_XBANK:
        case CARTRIDGE_EPYX_FASTLOAD:
        case CARTRIDGE_EXPERT:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_FINAL_I:
        case CARTRIDGE_FINAL_III:
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_FORMEL64:
        case CARTRIDGE_FREEZE_FRAME:
        case CARTRIDGE_FREEZE_FRAME_MK2:
        case CARTRIDGE_FREEZE_MACHINE:
        case CARTRIDGE_FUNPLAY:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_GMOD2:
        case CARTRIDGE_GMOD3:
        case CARTRIDGE_GS:
        case CARTRIDGE_DREAN:
        case CARTRIDGE_IEEE488:
        case CARTRIDGE_IEEEFLASH64:
        case CARTRIDGE_IDE64:
        case CARTRIDGE_KINGSOFT:
        case CARTRIDGE_KCS_POWER:
        case CARTRIDGE_MACH5:
        case CARTRIDGE_MAGIC_DESK:
        case CARTRIDGE_MAGIC_DESK_16:
        case CARTRIDGE_MAGIC_FORMEL:
        case CARTRIDGE_MAGIC_VOICE:
        case CARTRIDGE_MAX_BASIC:
        case CARTRIDGE_MIKRO_ASSEMBLER:
        case CARTRIDGE_MMC64:
        case CARTRIDGE_MMC_REPLAY:
        case CARTRIDGE_MULTIMAX:
        case CARTRIDGE_OCEAN:
        case CARTRIDGE_P64:
        case CARTRIDGE_PAGEFOX:
        case CARTRIDGE_PARTNER64:
        case CARTRIDGE_RAMLINK:
        case CARTRIDGE_RETRO_REPLAY:
        case CARTRIDGE_REX:
        case CARTRIDGE_REX_EP256:
        case CARTRIDGE_REX_RAMFLOPPY:
        case CARTRIDGE_RGCD:
        case CARTRIDGE_RRNETMK3:
        case CARTRIDGE_ROSS:
        case CARTRIDGE_SDBOX:
        case CARTRIDGE_SNAPSHOT64:
        case CARTRIDGE_SIMONS_BASIC:
        case CARTRIDGE_SILVERROCK_128:
        case CARTRIDGE_STARDOS:
        case CARTRIDGE_STRUCTURED_BASIC:
        case CARTRIDGE_SUPER_EXPLODE_V5:
        case CARTRIDGE_SUPER_GAMES:
        case CARTRIDGE_SUPER_SNAPSHOT:
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
        case CARTRIDGE_TURTLE_GRAPHICS_II:
        case CARTRIDGE_WARPSPEED:
        case CARTRIDGE_WESTERMANN:
        case CARTRIDGE_ZAXXON:
            break;
        default:
            /* FIXME: perhaps make this a call via the c128cartridge interface */
            if ((machine_class == VICE_MACHINE_C128) && (
                (val == CARTRIDGE_C128_MAKEID(CARTRIDGE_C128_GENERIC)) ||
                (val == CARTRIDGE_C128_MAKEID(CARTRIDGE_C128_WARPSPEED128)) ||
                (val == CARTRIDGE_C128_MAKEID(CARTRIDGE_C128_PARTNER128)) ||
                (val == CARTRIDGE_C128_MAKEID(CARTRIDGE_C128_COMAL80))
                )) {
                break;
            } else {
                return -1;
            }
    }

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
/*    DBG(("c64cartridge_reset: %d\n", val)); */
    int val = value ? 1 : 0;

    if (c64cartridge_reset != val) {
        DBG(("c64cartridge_reset changed: %d\n", val));
        c64cartridge_reset = val; /* resource value modified */
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
    { "-cartreset", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "CartridgeReset", (void *)1,
      NULL, "Reset machine if a cartridge is attached or detached" },
    { "+cartreset", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "CartridgeReset", (void *)0,
      NULL, "Do not reset machine if a cartridge is attached or detached" },
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
    mon_cart_cmd.cartridge_trigger_freeze = cartridge_trigger_freeze;
    mon_cart_cmd.cartridge_trigger_freeze_nmi_only = cartridge_trigger_freeze_nmi_only;
    mon_cart_cmd.export_dump = export_dump;

    if (cart_cmdline_options_init() < 0) {
        return -1;
    }

    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/*
    returns ID of cart in "Main Slot"
*/
int cart_getid_slotmain(void)
{
#if 0
    DBG(("CART: cart_getid_slotmain c64cart_type: %d crttype: %d\n", c64cart_type, crttype));
    if (c64cart_type == CARTRIDGE_CRT) {
        return crttype;
    }
    return c64cart_type;
#else
    /* DBG(("CART: cart_getid_slotmain mem_cartridge_type: %d \n", mem_cartridge_type)); */
    return mem_cartridge_type;
#endif
}

/* ---------------------------------------------------------------------*/

/*
    get filename of cart with given type
*/
const char *cartridge_get_file_name(int type)
{
    if (cart_getid_slotmain() == type && !cart_can_get_file_name(type)) {
        return cartfile;
    }
    return cart_get_file_name(type);
}

/*
    returns 1 if the cartridge of the given type is enabled

    - used by c64iec.c:iec_available_busses
*/
int cartridge_type_enabled(int type)
{
    if (cart_getid_slotmain() == type) {
        return 1;
    }
    return cart_type_enabled(type);
}

/*
    returns -1 on error, else a positive CRT ID

    FIXME: to simplify this function a little bit, all subfunctions should
           also return the respective CRT ID on success
*/
static int crt_attach(const char *filename, uint8_t *rawcart)
{
    crt_header_t header;
    int rc = -1, new_crttype;
    FILE *fd;

    DBG(("crt_attach: %s\n", filename));

    fd = crt_open(filename, &header);
    if (fd == NULL) {
        return -1;
    }

    new_crttype = header.type;

    /* if we have loaded a C128 cartridge, convert the C128 crt id to something
       else (that can coexist with C64 crt ids) */
    if (header.machine == VICE_MACHINE_C128) {
        new_crttype = CARTRIDGE_C128_MAKEID(new_crttype);
    }

    /* handle our negative test IDs */
    if (new_crttype & 0x8000) {
        new_crttype -= 0x10000;
    }
    DBG(("crt_attach ID: %d\n", new_crttype));

/*  cart should always be detached. there is no reason for doing fancy checks
    here, and it will cause problems incase a cart MUST be detached before
    attaching another, or even itself. (eg for initialization reasons)

    most obvious reason: attaching a different ROM (software) for the same
    cartridge (hardware) */

    cartridge_detach_image(new_crttype);

    /* now attach a crt image. note that for carts not in the main slot, the image
       name is usually kept in a resource, and the cartridge is enabled via another
       resource - the function called from here must also do this */
    if ((machine_class == VICE_MACHINE_C128) && (header.machine == VICE_MACHINE_C128)) {
        DBG(("attaching as C128 cartridge id: %d\n", new_crttype));
        rc = c128cartridge->attach_crt(new_crttype, fd, filename, rawcart);
    } else {

        switch (new_crttype) {
            case CARTRIDGE_CRT:
                rc = generic_crt_attach(fd, rawcart);
                if (rc != CARTRIDGE_NONE) {
                    new_crttype = rc;
                }
                break;
            case CARTRIDGE_ACTION_REPLAY:
                rc = actionreplay_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_ACTION_REPLAY2:
                rc = actionreplay2_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_ACTION_REPLAY3:
                rc = actionreplay3_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_ACTION_REPLAY4:
                rc = actionreplay4_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_ATOMIC_POWER:
                rc = atomicpower_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_BISPLUS:
                rc = bisplus_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_BLACKBOX3:
                rc = blackbox3_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_BLACKBOX4:
                rc = blackbox4_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_BLACKBOX8:
                rc = blackbox8_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_BLACKBOX9:
                rc = blackbox9_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_CAPTURE:
                rc = capture_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_COMAL80:
                rc = comal80_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_DELA_EP256:
                rc = delaep256_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_DELA_EP64:
                rc = delaep64_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_DELA_EP7x8:
                rc = delaep7x8_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_DIASHOW_MAKER:
                rc = dsm_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_DINAMIC:
                rc = dinamic_crt_attach(fd, rawcart);
                break;
#if 0
            case CARTRIDGE_DQBB: /* slot 1 */
                rc = dqbb_crt_attach(fd, rawcart, filename);
                break;
#endif
            case CARTRIDGE_EASYCALC:
                rc = easycalc_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_EASYFLASH:
                rc = easyflash_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_EPYX_FASTLOAD:
                rc = epyxfastload_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_EXOS:
                rc = exos_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_EXPERT: /* slot 1 */
                rc = expert_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_FINAL_I:
                rc = final_v1_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_FINAL_III:
                rc = final_v3_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_FINAL_PLUS:
                rc = final_plus_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_FORMEL64:
                rc = formel64_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_FREEZE_FRAME:
                rc = freezeframe_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_FREEZE_FRAME_MK2:
                rc = freezeframe2_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_FREEZE_MACHINE:
                rc = freezemachine_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_FUNPLAY:
                rc = funplay_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_GAME_KILLER:
                rc = gamekiller_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_GMOD2:
                rc = gmod2_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_GMOD3:
                rc = gmod3_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_GS:
                rc = gs_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_DREAN:
                rc = drean_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_IDE64:
                rc = ide64_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_IEEE488: /* slot 0 */
                rc = tpi_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_IEEEFLASH64: /* slot 0 */
                rc = ieeeflash64_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_ISEPIC: /* slot 1 */
                rc = isepic_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_KCS_POWER:
                rc = kcs_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_KINGSOFT:
                rc = kingsoft_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_LT_KERNAL:
                rc = ltkernal_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_MACH5:
                rc = mach5_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_MAGIC_DESK:
                rc = magicdesk_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_MAGIC_DESK_16:
                rc = magicdesk16_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_MAGIC_FORMEL:
                rc = magicformel_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_MAGIC_VOICE: /* slot 0 */
                rc = magicvoice_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_MAX_BASIC:
                rc = maxbasic_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_MIKRO_ASSEMBLER:
                rc = mikroass_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_MMC64: /* slot 0 */
                rc = mmc64_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_MMC_REPLAY:
                rc = mmcreplay_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_MULTIMAX:
                rc = multimax_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_OCEAN:
                rc = ocean_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_P64:
                rc = p64_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_PAGEFOX:
                rc = pagefox_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_PARTNER64:
                rc = partner64_crt_attach(fd, rawcart);
                break;
#if 0
            case CARTRIDGE_RAMCART: /* slot 1 */
                rc = ramcart_crt_attach(fd, rawcart, filename);
                break;
#endif
            case CARTRIDGE_RAMLINK: /* slot 0 */
                rc = ramlink_crt_attach(fd, rawcart, filename);
                break;
            case CARTRIDGE_RETRO_REPLAY:
                rc = retroreplay_crt_attach(fd, rawcart, filename, header.subtype);
                break;
            case CARTRIDGE_REX_EP256:
                rc = rexep256_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_REX:
                rc = rex_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_REX_RAMFLOPPY:
                rc = rexramfloppy_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_RGCD:
                rc = rgcd_crt_attach(fd, rawcart, header.subtype);
                break;
#ifdef HAVE_RAWNET
            case CARTRIDGE_RRNETMK3:
                rc = rrnetmk3_crt_attach(fd, rawcart, filename);
                break;
#endif
            case CARTRIDGE_ROSS:
                rc = ross_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_SDBOX:
                rc = sdbox_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_SILVERROCK_128:
                rc = silverrock128_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_SIMONS_BASIC:
                rc = simon_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_STARDOS:
                rc = stardos_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_SNAPSHOT64:
                rc = snapshot64_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_STRUCTURED_BASIC:
                rc = stb_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_SUPER_GAMES:
                rc = supergames_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_SUPER_SNAPSHOT:
                rc = supersnapshot_v4_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_SUPER_SNAPSHOT_V5:
                rc = supersnapshot_v5_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_SUPER_EXPLODE_V5:
                rc = se5_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_TURTLE_GRAPHICS_II:
                rc = turtlegraphics_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_WARPSPEED:
                rc = warpspeed_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_WESTERMANN:
                rc = westermann_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_ZAXXON:
                rc = zaxxon_crt_attach(fd, rawcart);
                break;
            case CARTRIDGE_ZIPPCODE48:
                rc = zippcode48_crt_attach(fd, rawcart);
                break;
            default:
                archdep_startup_log_error("unknown CRT ID: %d\n", new_crttype);
                rc = -1;
                break;
        }
    }

    fclose(fd);

    if (rc == -1) {
        DBG(("crt_attach error (%d)\n", rc));
        return -1;
    }
    DBG(("crt_attach return ID: %d\n", new_crttype));
    return new_crttype;
}

/*
    attach cartridge image

    type == -1  NONE
    type ==  0  CRT format

    returns -1 on error, 0 on success
*/
int cartridge_attach_image(int type, const char *filename)
{
    uint8_t *rawcart;
    char *abs_filename;
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

    if (archdep_path_is_relative(filename)) {
        archdep_expand_path(&abs_filename, filename);
    } else {
        abs_filename = lib_strdup(filename);
    }

    if (type == CARTRIDGE_CRT) {
        carttype = crt_getid(abs_filename);
        if (carttype == -1) {
            log_message(LOG_DEFAULT, "CART: '%s' is not a valid CRT file.", abs_filename);
            lib_free(abs_filename);
            return -1;
        }
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

    slotmain = cart_is_slotmain(carttype);
    if (slotmain) {
        /* if the cart to be attached is in the "Main Slot", detach whatever
           cart currently is in the "Main Slot" */
        oldmain = cart_getid_slotmain();
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
        cartid = crt_attach(abs_filename, rawcart);
        if (cartid == CARTRIDGE_NONE) {
            goto exiterror;
        }
        if (type < 0) {
            DBG(("CART: attach generic CRT ID: %d\n", type));
        }
    } else {
        DBG(("CART: attach BIN ID: %d '%s'\n", carttype, filename));
        cartid = carttype;
        /* if this is x128 and the ID is a C128-only cart, use c128 specific function */
        DBG(("%d %d %d\n",cartid,(machine_class == VICE_MACHINE_C128), (CARTRIDGE_C128_ISID(cartid)) ));
        if ((machine_class == VICE_MACHINE_C128) && (CARTRIDGE_C128_ISID(cartid))) {
            DBG(("trying C128 exclusive function\n"));
            if (c128cartridge->bin_attach(carttype, abs_filename, rawcart) < 0) {
                DBG(("C128 exclusive function FAILED\n"));
                goto exiterror;
            }
            DBG(("C128 exclusive function OK\n"));
        } else if (cart_bin_attach(carttype, abs_filename, rawcart) < 0) {
            goto exiterror;
        }
    }

    if (cart_is_slotmain(cartid)) {
        DBG(("cartridge_attach MAIN ID: %d\n", cartid));
        mem_cartridge_type = cartid;
        cart_romhbank_set_slotmain(0);
        cart_romlbank_set_slotmain(0);
    } else {
        DBG(("cartridge_attach (other) ID: %d\n", cartid));
    }

    DBG(("CART: attach RAW ID: %d\n", cartid));
    cart_attach(cartid, rawcart);

    cart_power_off();

    if (cart_is_slotmain(cartid)) {
        /* "Main Slot" */
        DBG(("CART: set main slot ID: %d type: %d\n", carttype, type));
        c64cart_type = type;
        if (type == CARTRIDGE_CRT) {
            crttype = carttype;
        }
        util_string_set(&cartfile, abs_filename);
    }

    DBG(("CART: cartridge_attach_image type: %d ID: %d done.\n", type, carttype));
    lib_free(rawcart);
    log_message(LOG_DEFAULT, "CART: attached '%s' as ID %d.", abs_filename, carttype);
    lib_free(abs_filename);
    return 0;

exiterror:
    DBG(("CART: error\n"));
    lib_free(rawcart);
    log_message(LOG_DEFAULT, "CART: could not attach '%s'.", abs_filename);
    lib_free(abs_filename);
    return -1;
}

void cart_power_off(void)
{
    if (c64cartridge_reset) {
        /* "Turn off machine before removing cartridge" */
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }
}

/*
    Attach cartridge from snapshot

    Sets static variables related to the "Main Slot".
    This is needed for cart_getid_slotmain to return a proper
    value for cart_freeze and such.
*/
void cart_attach_from_snapshot(int type)
{
    if (cart_is_slotmain(type)) {
        c64cart_type = type;
    }
}

/*
    detach cartridge from "Main Slot"
*/
void cart_detach_slotmain(void)
{
    int type = cart_getid_slotmain();
    DBG(("CART: detach main %d: type: %d id: %d\n", type, c64cart_type, crttype));
    if (type != CARTRIDGE_NONE) {
        cart_detach(type);

        DBG(("CART: unset cart config\n"));
        cart_config_changed_slotmain(CMODE_RAM, CMODE_RAM, CMODE_READ);

        cart_power_off();

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
    - if type is 0, then cart in main slot will get detached

    - carts not in "Main Slot" must make sure their detach hook does not
      fail when it is called and the cart is not actually attached.
*/
void cartridge_detach_image(int type)
{
    if (type == 0) {
        DBG(("CART: detach MAIN ID: %d\n", type));
        cart_detach_slotmain();
    } else if (type == -1) {
        cart_detach_all();
    } else {
        DBG(("CART: detach ID: %d\n", type));
        /* detach only given type */
        if (cart_is_slotmain(type)) {
            cart_detach_slotmain();
        } else {
            cart_detach(type);
        }
    }

    if (machine_class == VICE_MACHINE_C128) {
        c128cartridge->detach_image(type);
    }

    /* FIXME: cart_detach should take care of it */
    DBG(("CART: unset cart config\n"));
    cart_config_changed_slotmain(CMODE_RAM, CMODE_RAM, CMODE_READ);

    cart_power_off();
}

/*
    set currently active cart in "Main Slot" as default
*/
void cartridge_set_default(void)
{
    int type = CARTRIDGE_NONE;

    if (cartfile != NULL) {
        if (util_file_exists(cartfile)) {
            if (crt_getid(cartfile) > 0) {
                type = CARTRIDGE_CRT;
            } else {
                type = c64cart_type;
            }
        } else {
            DBG(("cartridge_set_default: file does not exist: '%s'\n", cartfile));
        }
    } else {
        DBG(("cartridge_set_default: no filename\n"));
    }
    DBG(("cartridge_set_default: id %d '%s'\n", type, cartfile));

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


int cartridge_save_image(int type, const char *filename)
{
    char *ext = util_get_extension(filename);
    if (ext != NULL && !strcmp(ext, "crt")) {
        return cartridge_crt_save(type, filename);
    }
    return cartridge_bin_save(type, filename);
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
    cart_nmi_alarm_time = maincpu_clk + 3;
    alarm_set(cartridge_nmi_alarm, cart_nmi_alarm_time);
}

/* called by the NMI alarm */
static void cart_nmi_alarm_triggered(CLOCK offset, void *data)
{
    alarm_unset(cartridge_nmi_alarm);
    cart_nmi_alarm_time = CLOCK_MAX;
    cart_nmi_alarm(offset, data); /* c64carthooks.c */
}

/* called by the Freeze Button alarm */
static void cart_freeze_alarm_triggered(CLOCK offset, void *data)
{
    DBG(("cart_freeze_alarm_triggered\n"));
    alarm_unset(cartridge_freeze_alarm);
    cart_freeze_alarm_time = CLOCK_MAX;

    if (cart_freeze_allowed()) {  /* c64carthooks.c */
        DBG(("cart_trigger_freeze delay 3 cycles\n"));
        maincpu_set_nmi(cartridge_int_num, IK_NMI);
        cart_nmi_alarm_time = maincpu_clk + 3;
        alarm_set(cartridge_nmi_alarm, cart_nmi_alarm_time);
    }
}

/*
   called by the UI when the freeze button is pressed

   sets cartridge_freeze_alarm to delay button press up to one frame, then

   - cart_freeze_alarm_triggered
     - cart_freeze_allowed (c64carthooks.c)
       checks wether freeze is allowed for currently active cart(s)
     if yes, sets up cartridge_nmi_alarm to delay NMI 3 cycles

   - cart_nmi_alarm_triggered
     - cart_nmi_alarm (c64carthooks.c)

*/
void cartridge_trigger_freeze(void)
{
    int delay = lib_unsigned_rand(1, (unsigned int)machine_get_cycles_per_frame());

    cart_freeze_alarm_time = maincpu_clk + delay;
    alarm_set(cartridge_freeze_alarm, cart_freeze_alarm_time);
    DBG(("cartridge_trigger_freeze delay %d cycles\n", delay));
}

void cart_unset_alarms(void)
{
    alarm_unset(cartridge_freeze_alarm);
    alarm_unset(cartridge_nmi_alarm);
    cart_freeze_alarm_time = CLOCK_MAX;
    cart_nmi_alarm_time = CLOCK_MAX;
}

void cart_undump_alarms(void)
{
    if (cart_freeze_alarm_time < CLOCK_MAX) {
        alarm_set(cartridge_freeze_alarm, cart_freeze_alarm_time);
    }
    if (cart_nmi_alarm_time < CLOCK_MAX) {
        alarm_set(cartridge_nmi_alarm, cart_nmi_alarm_time);
    }
}

/* called by c64.c:machine_specific_init */
void cartridge_init(void)
{
    cart_init();
    cartridge_nmi_alarm = alarm_new(maincpu_alarm_context, "Cartridge", cart_nmi_alarm_triggered, NULL);
    cartridge_freeze_alarm = alarm_new(maincpu_alarm_context, "Cartridge", cart_freeze_alarm_triggered, NULL);
    cartridge_int_num = interrupt_cpu_status_int_new(maincpu_int_status, "Cartridge");
}

/* returns 1 when cartridge (ROM) image can be flushed */
int cartridge_can_flush_image(int crtid)
{
    const char *p;
    if (!cartridge_type_enabled(crtid)) {
        return 0;
    }
    p = cartridge_get_file_name(crtid);
    if ((p == NULL) || (*p == '\x0')) {
        return 0;
    }
    return 1;
}

/* returns 1 when cartridge (ROM) image can be saved */
int cartridge_can_save_image(int crtid)
{
    if (!cartridge_type_enabled(crtid)) {
        return 0;
    }
    return 1;
}
