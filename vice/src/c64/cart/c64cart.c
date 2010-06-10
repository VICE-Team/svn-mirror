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
#include "c64cart.h"
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
#include "translate.h"
#include "util.h"

/* #define DEBUGCART */

#ifdef DEBUGCART
#define DBG(x)  printf x
#else
#define DBG(x)
#endif

/* global options for the cart system */
static int c64cartridge_reset = 1; /* (resource) hardreset system after cart was attached/detached */
/* defaults for the "Main Slot" */
static char *cartridge_file = NULL; /* (resource) file name */
static int cartridge_type = CARTRIDGE_NONE; /* (resource) is == CARTRIDGE_CRT (0) if CRT file */
/* actually in use for the "Main Slot" */
static char *cartfile = NULL; /* file name */
static int c64cart_type = CARTRIDGE_NONE; /* is == CARTRIDGE_CRT (0) if CRT file */
static int crttype = CARTRIDGE_NONE; /* contains CRT ID if c64cart_type == 0 */

static alarm_t *cartridge_alarm = NULL; /* cartridge alarm context */
static unsigned int cartridge_int_num; /* irq number for cart */

/*
    we have 3 resources for the main cart that may be changed in arbitrary order:

    - cartridge type
    - cartridge file name
    - cartridge change reset behaviour

    the following functions try to deal with this in a hopefully sane way... however,
    do _NOT_ change the used resources from the (G)UI directly. (used the set_default
    function instead)
*/

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

static int try_cartridge_attach(int type, const char *filename)
{
    int crtid;

    if (filename) {
        if (util_file_exists(filename)) {
            if((crtid=crt_getid(filename)) > 0) {
                cartridge_type = CARTRIDGE_CRT; /* resource value modified */
                trying_cart = 1; /* FIXME */
                return cartridge_attach_image(CARTRIDGE_CRT, filename);
            } else if ((type != CARTRIDGE_NONE) && (type != CARTRIDGE_CRT)) {
                cartridge_type = type; /* resource value modified */
                trying_cart = 1; /* FIXME */
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
    if (   reu_resources_init() < 0
        || georam_resources_init() < 0
        || ramcart_resources_init() < 0
        || isepic_resources_init() < 0
        || dqbb_resources_init() < 0
        || mmc64_resources_init() < 0
        || magicvoice_resources_init() < 0
        || digimax_resources_init() < 0
        || sfx_soundexpander_resources_init() < 0
        || sfx_soundsampler_resources_init() < 0
        || easyflash_resources_init() < 0
        || ide64_resources_init() < 0
        || mmcreplay_resources_init() < 0
        || expert_resources_init() < 0
        || retroreplay_resources_init() < 0
        ) {
        return -1;
    }

    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    return resources_register_string(resources_string);
}

void cartridge_resources_shutdown(void)
{
    /* "IO Slot" */
    reu_resources_shutdown();
    georam_resources_shutdown();
    /* "Main Slot" */
    lib_free(cartridge_file);
    lib_free(cartfile);
    ide64_resources_shutdown();
    /* "Slot 1" */
    expert_resources_shutdown();
    dqbb_resources_shutdown();
    ramcart_resources_shutdown();
    /* "Slot 0" */
    mmc64_resources_shutdown();
    magicvoice_resources_shutdown();
}
/* ---------------------------------------------------------------------*/
static int attach_cartridge_cmdline(const char *param, void *extra_param)
{
    int type = vice_ptr_to_int(extra_param);

    /* NULL param is used for +cart */
    if (!param) {
        cartridge_detach_image(-1);
        return 0;
    }
    return cartridge_attach_image(type, param);
}

/*
    TODO: add commandline options for the missing carts
    TODO: keep in sync with cartridge.h (currently highest: CARTRIDGE_MAGIC_VOICE)

    the following carts, which do not have any rom or ram, are NOT in the list below,
    for obvious reasons:

        CARTRIDGE_DIGIMAX
        CARTRIDGE_SFX_SOUND_EXPANDER
        CARTRIDGE_SFX_SOUND_SAMPLER
        CARTRIDGE_MIDI_PASSPORT
        CARTRIDGE_MIDI_DATEL
        CARTRIDGE_MIDI_SEQUENTIAL
        CARTRIDGE_MIDI_NAMESOFT
        CARTRIDGE_MIDI_MAPLIN
        CARTRIDGE_TFE
        CARTRIDGE_TURBO232

    all other carts should get a commandline option here like this:

    -cartXYZ <name>     attach a ram/rom image for cartridgeXYZ
*/
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
      attach_cartridge_cmdline, NULL, NULL, NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_CART,
      NULL, NULL },
    /* generic cartridges */
    { "-cart8", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_GENERIC_8KB, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_GENERIC_8KB_CART,
      NULL, NULL },
    { "-cart16", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_GENERIC_16KB, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_GENERIC_16KB_CART,
      NULL, NULL },
    { "-cartultimax", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_ULTIMAX, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach generic 16kB Ultimax Cartridge image") },
    /* smart-insert CRT */
    { "-cartcrt", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_CRT, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_CRT_CART,
      NULL, NULL },
    /* binary images: */
    { "-cartar3", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_ACTION_REPLAY3, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART,
      NULL, NULL },
    { "-cartar4", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_ACTION_REPLAY4, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART,
      NULL, NULL },
    { "-cartar", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_ACTION_REPLAY, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ACTION_REPLAY_CART },
    { "-cartap", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_ATOMIC_POWER, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ATOMIC_POWER_CART,
      NULL, NULL },
    /* TODO: CARTRIDGE_CAPTURE */
    /* TODO: CARTRIDGE_COMAL80 */
    /* TODO: CARTRIDGE_DELA_EP64 */
    /* TODO: CARTRIDGE_DELA_EP7x8 */
    /* TODO: CARTRIDGE_DELA_EP256 */
    /* TODO: CARTRIDGE_DINAMIC */
    /* TODO: CARTRIDGE_DQBB */
    /* TODO: CARTRIDGE_EASYFLASH */
    /* TODO: CARTRIDGE_EASYFLASH_XBANK */
    { "-cartepyx", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_EPYX_FASTLOAD, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART,
      NULL, NULL },
    { "-cartexos", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_EXOS, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 8kB Exos cartridge image") },
    { "-cartexpert", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_EXPERT, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 8kB Expert cartridge image") },
    { "-cartfc", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_FINAL_I, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 16kB Final Cartridge image") },
    { "-cartfc3", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_FINAL_III, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 64kB Final Cartridge 3 image") },
    { "-cartfcplus", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_FINAL_PLUS, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 32kB Final Cartridge Plus image") },
    { "-cartff", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_FREEZE_FRAME, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 8kB Freeze Frame image") },
    { "-cartfm", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_FREEZE_MACHINE, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 32kB Freeze Machine image") },
    /* TODO: CARTRIDGE_FUNPLAY */
    { "-cartgamekiller", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_GAME_KILLER, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_GAME_KILLER_CART,
      NULL, NULL },
    /* TODO: CARTRIDGE_GEORAM */
    /* TODO: CARTRIDGE_GS */
    { "-cartide", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_IDE64, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_IDE64_CART,
      NULL, NULL },
    { "-cartieee488", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_IEEE488, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_CBM_IEEE488_CART,
      NULL, NULL },
    { "-cartisepic", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_ISEPIC, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 2kB Isepic image") },
    /* TODO: CARTRIDGE_KCS_POWER */
    /* TODO: CARTRIDGE_MAGIC_DESK */
    /* TODO: CARTRIDGE_MAGIC_FORMEL */
    /* TODO: CARTRIDGE_MIKRO_ASSEMBLER */
    /* TODO: CARTRIDGE_MMC64 */
    { "-cartmmcr", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_MMC_REPLAY, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 512kB MMC Replay cartridge image") },
    { "-cartmv", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_MAGIC_VOICE, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 16kB Magic Voice image") },
    /* TODO: CARTRIDGE_OCEAN */
    { "-cartp64", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_P64, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_P64_CART,
      NULL, NULL },
    /* TODO: CARTRIDGE_RAMCART */
    /* TODO: CARTRIDGE_REU */
    /* TODO: CARTRIDGE_REX */
    /* TODO: CARTRIDGE_REX_EP256 */
    /* TODO: CARTRIDGE_ROSS */
    { "-cartrr", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_RETRO_REPLAY, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_RETRO_REPLAY_CART,
      NULL, NULL },
    /* TODO: CARTRIDGE_SIMONS_BASIC */
    /* TODO: CARTRIDGE_SUPER_GAMES */
    { "-carts64", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_SNAPSHOT64, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 4kB Snapshot 64 image") },
    { "-cartstardos", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_STARDOS, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_STARDOS_CART,
      NULL, NULL },
    { "-cartstb", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_STRUCTURED_BASIC, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_STB_CART,
      NULL, NULL },
    { "-cartse5", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_SUPER_EXPLODE_V5, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 16kB Super Explode V5 image") },
    { "-cartss4", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_SUPER_SNAPSHOT, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_SS4_CART,
      NULL, NULL },
    { "-cartss5", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_SUPER_SNAPSHOT_V5, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_SS5_CART,
      NULL, NULL },
    { "-cartws", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_WARPSPEED, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 8kB Warpspeed image") },
    { "-cartwestermann", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_WESTERMANN, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_WESTERMANN_CART,
      NULL, NULL },
    /* TODO: CARTRIDGE_ZAXXON */
    { NULL }
};

int cartridge_cmdline_options_init(void)
{
    mon_cart_cmd.cartridge_attach_image = cartridge_attach_image;
    mon_cart_cmd.cartridge_detach_image = cartridge_detach_image;
    mon_cart_cmd.cartridge_trigger_freeze = cartridge_trigger_freeze;
    mon_cart_cmd.cartridge_trigger_freeze_nmi_only = cartridge_trigger_freeze_nmi_only;

    if (reu_cmdline_options_init() < 0
        || georam_cmdline_options_init() < 0
        || ramcart_cmdline_options_init() < 0
        || isepic_cmdline_options_init() < 0
        || expert_cmdline_options_init() < 0
        || dqbb_cmdline_options_init() < 0
        || mmc64_cmdline_options_init() < 0
        || digimax_cmdline_options_init() < 0
        || sfx_soundexpander_cmdline_options_init() < 0
        || sfx_soundsampler_cmdline_options_init() < 0
        || ide64_cmdline_options_init() < 0
        || mmcreplay_cmdline_options_init() < 0
        || retroreplay_cmdline_options_init() < 0
        || easyflash_cmdline_options_init() < 0) {
        return -1;
    }

    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static int cartridge_bin_attach(int type, const char *filename, BYTE *rawcart)
{
    switch(type) {
        case CARTRIDGE_MAGIC_VOICE:
            if (magicvoice_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;

        case CARTRIDGE_GENERIC_8KB:
        case CARTRIDGE_EPYX_FASTLOAD:
            if (generic_8kb_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_GENERIC_16KB:
        case CARTRIDGE_WESTERMANN:
        case CARTRIDGE_WARPSPEED:
            if (generic_16kb_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_ULTIMAX:
            if (generic_ultimax_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_ACTION_REPLAY3:
            if (actionreplay3_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_ACTION_REPLAY4:
            if (actionreplay4_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_ACTION_REPLAY:
            if (actionreplay_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_ATOMIC_POWER:
            if (atomicpower_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_RETRO_REPLAY:
            if (retroreplay_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_MMC_REPLAY:
            if (mmcreplay_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_IDE64:
            if (c64cart_type==CARTRIDGE_IDE64) {
                ide64_detach(); /* detach IDE64 if reattaching */
            }
            if (ide64_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_SNAPSHOT64:
            if (snapshot64_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_SUPER_EXPLODE_V5:
            if (se5_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_SUPER_SNAPSHOT:
            if (supersnapshot_v4_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            if (supersnapshot_v5_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_IEEE488:
            if (tpi_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_STRUCTURED_BASIC:
            if (stb_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_STARDOS:
            if (stardos_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_P64:
            if (p64_bin_attach(filename, rawcart) < 0) {
               return -1;
            }
            break;
        case CARTRIDGE_GAME_KILLER:
            if (gamekiller_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_FINAL_I:
            if (final_v1_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_FINAL_PLUS:
            if (final_plus_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_FINAL_III:
            if (final_v3_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_EXOS:
            if (exos_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_FREEZE_FRAME:
            if (freezeframe_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            if (freezemachine_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_ISEPIC:
            if (isepic_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        case CARTRIDGE_EXPERT:
            if (expert_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
            break;
        default:
            return -1;
    }

    return 0;
}

/*
    returns 1 if given cart type is in "Main Slot"
*/
int cartridge_is_slotmain(int type)
{
   switch (type) {
        /* slot 0 */
        case CARTRIDGE_MMC64:
        case CARTRIDGE_MAGIC_VOICE:
        /* slot 1 */
        case CARTRIDGE_DQBB:
        case CARTRIDGE_EXPERT:
        case CARTRIDGE_ISEPIC:
        case CARTRIDGE_RAMCART:
        /* io slot */
        case CARTRIDGE_GEORAM:
        case CARTRIDGE_REU:
            return 0;
        default:
            return 1;
    }
}

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
    return 0;
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

    DBG(("CART: attach RAW ID: %d\n", cartid));
    cartridge_attach(cartid, rawcart);

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

/*
    detach cartridge from "Main Slot"
*/
static void cartridge_detach_main(void)
{
    if (c64cart_type != CARTRIDGE_NONE) {
        DBG(("CART: detach main type: %d id: %d\n", c64cart_type, crttype));
        cartridge_detach(cartridge_getid_slotmain());
        /* reset "Main Slot" */
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
    }
    if (type == -1) {
        DBG(("CART: detach all\n"));
        /* detach all cartridges */
        /* "slot 0" */
        mmc64_shutdown();
        magicvoice_detach();
        /* "Slot 1" */
        dqbb_shutdown();
        expert_detach();
        isepic_detach();
        ramcart_shutdown();
        /* "io Slot" */
        georam_shutdown();
        reu_shutdown();
        /* "Main Slot" */
        cartridge_detach_main();
    } else {
        DBG(("CART: detach ID: %d\n", type));
        /* detach only given type */
        if (cartridge_is_slotmain(type)) {
            cartridge_detach_main();
        } else {
            cartridge_detach(type);
        }
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
    get filename of cart in "Main Slot"
*/
const char *cartridge_get_file_name(WORD addr_ignored)
{
    return cartfile;
}

static void cartridge_change_mapping(CLOCK offset, void *data)
{
    alarm_unset(cartridge_alarm);

    /* "Slot 0" */
    /* "Slot 1" */
    if (expert_freeze_allowed()) {
        cartridge_freeze(CARTRIDGE_EXPERT);
    }
    if (isepic_freeze_allowed()) {
        cartridge_freeze(CARTRIDGE_ISEPIC);
    }
    /* "Main Slot" */
    cartridge_freeze(cartridge_getid_slotmain());
}

/* called by c64.c:machine_specific_init */
void cartridge_init(void)
{
    /* "Slot 0" */
    mmc64_init(); /* Initialize the MMC64.  */
    magicvoice_init();
    /* "Slot 1" */
    ramcart_init(); /* Initialize the RAMCART.  */
    /* "IO Slot" */
    reu_init(); /* Initialize the REU.  */
    georam_init(); /* Initialize the GEORAM.  */

    cartridge_alarm = alarm_new(maincpu_alarm_context, "Cartridge", cartridge_change_mapping, NULL);
    cartridge_int_num = interrupt_cpu_status_int_new(maincpu_int_status, "Cartridge");
}

void cartridge_trigger_freeze_nmi_only(void)
{
    maincpu_set_nmi(cartridge_int_num, IK_NMI);
}

void cartridge_trigger_nmi(void)
{
    maincpu_set_nmi(cartridge_int_num, IK_NMI);
    alarm_set(cartridge_alarm, maincpu_clk + 3);
}

void cartridge_release_freeze(void)
{
    maincpu_set_nmi(cartridge_int_num, 0);
}

void cartridge_trigger_freeze(void)
{
    int maintype = cartridge_getid_slotmain();
    /* "Slot 0" */
    /* "Slot 1" */
    if (expert_freeze_allowed()) {
        maincpu_set_nmi(cartridge_int_num, IK_NMI);
        alarm_set(cartridge_alarm, maincpu_clk + 3);
    }
    if (isepic_freeze_allowed()) {
        maincpu_set_nmi(cartridge_int_num, IK_NMI);
        alarm_set(cartridge_alarm, maincpu_clk + 3);
    }

    /* "Main Slot" */
    switch (maintype) {
        case CARTRIDGE_ACTION_REPLAY4:
        case CARTRIDGE_ACTION_REPLAY3:
        case CARTRIDGE_ACTION_REPLAY:
        case CARTRIDGE_KCS_POWER:
        case CARTRIDGE_FINAL_III:
        case CARTRIDGE_SNAPSHOT64:
        case CARTRIDGE_SUPER_SNAPSHOT:
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
        case CARTRIDGE_ATOMIC_POWER:
        case CARTRIDGE_FINAL_I:
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_MAGIC_FORMEL:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_FREEZE_FRAME:
        case CARTRIDGE_FREEZE_MACHINE:
            maincpu_set_nmi(cartridge_int_num, IK_NMI);
            alarm_set(cartridge_alarm, maincpu_clk + 3);
            break;
        case CARTRIDGE_RETRO_REPLAY:
            if (retroreplay_freeze_allowed()) {
                maincpu_set_nmi(cartridge_int_num, IK_NMI);
                alarm_set(cartridge_alarm, maincpu_clk + 3);
            }
            break;
        case CARTRIDGE_MMC_REPLAY:
            if (mmcreplay_freeze_allowed()) {
                maincpu_set_nmi(cartridge_int_num, IK_NMI);
                alarm_set(cartridge_alarm, maincpu_clk + 3);
            }
            break;
    }
}

