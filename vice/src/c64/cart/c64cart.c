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
#include "supersnapshot4.h"
#include "supersnapshot.h"
#include "translate.h"
#include "util.h"

static int cartridge_type;
static char *cartridge_file = NULL;
static int c64cartridge_reset;

int c64cart_type = CARTRIDGE_NONE;

static int cartres = 0;
static char *cartfile = NULL;

static alarm_t *cartridge_alarm = NULL;

static unsigned int cartridge_int_num;

static int trying_cart = 0;

int try_cartridge_init(int c)
{
    cartres ^= c;

    if (cartres) {
        return 0;
    }

    trying_cart = 1;
    return cartridge_attach_image(c64cart_type, cartfile);
}

static int set_cartridge_type(int val, void *param)
{
    cartridge_type = val;
    c64cart_type = cartridge_type;

    return try_cartridge_init(1);
}

static int set_cartridge_file(const char *name, void *param)
{
    util_string_set(&cartridge_file, name);
    util_string_set(&cartfile, name);

    return try_cartridge_init(2);
}

/* FIXME: the previous idea was bollocks :) this hook can happily die and
          related resources set up via the generic interface
          remove this after some testing */
#if 0
/* FIXME: hook up user operated toggle (on/off) switches from other carts here */
static int set_cartridge_mode(int val, void *param)
{
    const int type = ((c64cart_type == CARTRIDGE_CRT) ? crttype : c64cart_type);

    /*
     * Set cartridge mode.
     */
    cartridge_mode = val;

    switch (type) {
        case CARTRIDGE_EXPERT:
            expert_mode_changed(cartridge_mode);
            /* Manually force the cartres bit low and avoid the init.
               This is needed to have the cart work when saved as default
               and to not reattach the cart on runtime changes to the mode. */
            cartres &= ~4;
            return 0;
    }

    return try_cartridge_init(4);
}
#endif

static int set_cartridge_reset(int val, void *param)
{
    c64cartridge_reset = val;

    return try_cartridge_init(8);
}

static const resource_string_t resources_string[] = {
    { "CartridgeFile", "", RES_EVENT_NO, NULL,
      &cartridge_file, set_cartridge_file, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "CartridgeType", CARTRIDGE_NONE,
      RES_EVENT_STRICT, (resource_value_t)CARTRIDGE_NONE,
      &cartridge_type, set_cartridge_type, NULL },
    { "CartridgeReset", 1, RES_EVENT_NO, NULL,
      &c64cartridge_reset, set_cartridge_reset, NULL },
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
        || digimax_resources_init() < 0
        || sfx_soundexpander_resources_init() < 0
        || sfx_soundsampler_resources_init() < 0
        || easyflash_resources_init() < 0
        || ide64_resources_init() < 0
        || mmcreplay_resources_init() < 0
        || expert_resources_init() < 0
        ) {
        return -1;
    }

    if (resources_register_string(resources_string) < 0) {
        return -1;
    }

    return resources_register_int(resources_int);
}

void cartridge_resources_shutdown(void)
{
    lib_free(cartridge_file);
    lib_free(cartfile);
    ide64_resources_shutdown();
    expert_resources_shutdown();
    reu_resources_shutdown();
    georam_resources_shutdown();
    ramcart_resources_shutdown();
    mmc64_resources_shutdown();
    dqbb_resources_shutdown();
}

static int attach_cartridge_cmdline(const char *param, void *extra_param)
{
    int type = vice_ptr_to_int(extra_param);

    /* NULL param is used for +cart, but Expert cart doesn't have a filename */
    if (!param && (type != CARTRIDGE_EXPERT)) {
        cartridge_detach_image();
        return 0;
    }
    return cartridge_attach_image(type, param);
}

static const cmdline_option_t cmdline_options[] =
{
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
    { "-cartcrt", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_CRT, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_CRT_CART,
      NULL, NULL },
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
    { "-cartar", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_ACTION_REPLAY, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ACTION_REPLAY_CART },
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
    { "-cartrr", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_RETRO_REPLAY, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_RETRO_REPLAY_CART,
      NULL, NULL },
    { "-cartmmcr", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_MMC_REPLAY, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 512kB MMC Replay cartridge image") },
    { "-cartfc", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_FINAL_I, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 16kB Final Cartridge image") },
    { "-cartfcplus", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_FINAL_PLUS, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 32kB Final Cartridge Plus image") },
    { "-cartfc3", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_FINAL_III, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 64kB Final Cartridge 3 image") },
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
    { "-cartide", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_IDE64, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_IDE64_CART,
      NULL, NULL },
    { "-cartap", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_ATOMIC_POWER, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ATOMIC_POWER_CART,
      NULL, NULL },
    { "-cartepyx", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_EPYX_FASTLOAD, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART,
      NULL, NULL },
    { "-cartss64", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_SNAPSHOT64, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 4kB Snapshot 64 image") },
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
    { "-cartieee488", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_IEEE488, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_CBM_IEEE488_CART,
      NULL, NULL },
    { "-cartwestermann", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_WESTERMANN, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_WESTERMANN_CART,
      NULL, NULL },
    { "-cartstb", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_STRUCTURED_BASIC, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_STB_CART,
      NULL, NULL },
    { "-cartstardos", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_STARDOS, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_STARDOS_CART,
      NULL, NULL },
    { "-cartexos", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_EXOS, NULL, NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 8kB Exos cartridge image") },
    { "-cartexpert", CALL_FUNCTION, 0,
      attach_cartridge_cmdline, (void *)CARTRIDGE_EXPERT, NULL, NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_EXPERT_CART,
      NULL, NULL },
    { "-cartp64", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_P64, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_P64_CART,
      NULL, NULL },
    { "-cartgamekiller", CALL_FUNCTION, 1,
      attach_cartridge_cmdline, (void *)CARTRIDGE_GAME_KILLER, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_GAME_KILLER_CART,
      NULL, NULL },
    { "+cart", CALL_FUNCTION, 0,
      attach_cartridge_cmdline, NULL, NULL, NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_CART,
      NULL, NULL }, /* patch: iAN CooG */
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
        || dqbb_cmdline_options_init() < 0
        || mmc64_cmdline_options_init() < 0
        || digimax_cmdline_options_init() < 0
        || sfx_soundexpander_cmdline_options_init() < 0
        || sfx_soundsampler_cmdline_options_init() < 0
        || ide64_cmdline_options_init() < 0
        || mmcreplay_cmdline_options_init() < 0
        || easyflash_cmdline_options_init() < 0) {
        return -1;
    }

    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int cartridge_attach_image(int type, const char *filename)
{
    BYTE *rawcart;

    /* The expert cartridge does not have a filename.
     * It should only be enabled without loading an image.
     */
    if (type != CARTRIDGE_EXPERT) {
        /* Attaching no cartridge always works. */
        if (type == CARTRIDGE_NONE || *filename == '\0') {
            return 0;
        }
    }

    /* allocate temporary array */
    rawcart = lib_malloc(0x100000);

/*  cart should always be detached. there is no reason for doing fancy checks
    here, and it will cause problems incase a cart MUST be detached before
    attaching another, or even itself. (eg for initialization reasons)

    most obvious reason: attaching a different ROM (software) for the same
    cartridge (hardware) */

    if (trying_cart == 0) {
        cartridge_detach_image();
    } else {
        trying_cart = 0;
    }

    switch(type) {
        case CARTRIDGE_GENERIC_8KB:
        case CARTRIDGE_EPYX_FASTLOAD:
            if (generic_8kb_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_GENERIC_16KB:
        case CARTRIDGE_WESTERMANN:
        case CARTRIDGE_WARPSPEED:
            if (generic_16kb_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_ACTION_REPLAY3:
            if (actionreplay3_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_ACTION_REPLAY4:
            if (actionreplay4_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_ACTION_REPLAY:
            if (actionreplay_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_ATOMIC_POWER:
            if (atomicpower_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_RETRO_REPLAY:
            if (retroreplay_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_MMC_REPLAY:
            if (mmcreplay_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_IDE64:
            if (c64cart_type==CARTRIDGE_IDE64) {
                ide64_detach(); /* detach IDE64 if reattaching */
            }
            if (ide64_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_SNAPSHOT64:
            if (snapshot64_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_SUPER_SNAPSHOT:
            if (supersnapshot_v4_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            if (supersnapshot_v5_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_EXPERT:
            if (expert_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_IEEE488:
            if (tpi_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_CRT:
            if (crt_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_STRUCTURED_BASIC:
            if (stb_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_STARDOS:
            if (stardos_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_P64:
            if (p64_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_GAME_KILLER:
            if (gamekiller_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_FINAL_I:
            if (final_v1_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_FINAL_PLUS:
            if (final_plus_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_FINAL_III:
            if (final_v3_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_EXOS:
            if (exos_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_FREEZE_FRAME:
            if (freezeframe_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            if (freezemachine_bin_attach(filename, rawcart) < 0) {
                goto done;
            }
            break;
        default:
            goto done;
    }

    cartridge_type = c64cart_type = type;       /* Resource value updated! */
    util_string_set(&cartfile, filename);
    cartridge_attach((type == CARTRIDGE_CRT) ? crttype : type, rawcart);
    lib_free(rawcart);
    return 0;

done:
    lib_free(rawcart);
    return -1;
}

void cartridge_detach_image(void)
{
    if (c64cart_type != CARTRIDGE_NONE) {
        cartridge_detach((c64cart_type == CARTRIDGE_CRT) ? crttype : c64cart_type);
        c64cart_type = CARTRIDGE_NONE;
        crttype = CARTRIDGE_NONE;
        cartridge_type = CARTRIDGE_NONE;        /* Resource value updated! */
        lib_free(cartfile);
        cartfile = NULL;
    }

    reu_shutdown();
    georam_shutdown();
    ramcart_shutdown();
    dqbb_shutdown();
    mmc64_shutdown();
}

void cartridge_set_default(void)
{
    set_cartridge_type(c64cart_type, NULL);
    set_cartridge_file((c64cart_type == CARTRIDGE_NONE) ? "" : cartfile, NULL);
}

static void cartridge_change_mapping(CLOCK offset, void *data)
{
    alarm_unset(cartridge_alarm);

    cartridge_freeze((c64cart_type == CARTRIDGE_CRT) ? crttype : c64cart_type);
}

void cartridge_init(void)
{
    /* Initialize the REU.  */
    reu_init();

    /* Initialize the GEORAM.  */
    georam_init();

    /* Initialize the RAMCART.  */
    ramcart_init();

    /* Initialize the MMC64.  */
    mmc64_init();

    cartridge_alarm = alarm_new(maincpu_alarm_context, "Cartridge", cartridge_change_mapping, NULL);
    cartridge_int_num = interrupt_cpu_status_int_new(maincpu_int_status, "Cartridge");
}

void cartridge_trigger_freeze(void)
{
    int type = ((c64cart_type == CARTRIDGE_CRT) ? crttype : c64cart_type);

    switch (type) {
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
        case CARTRIDGE_EXPERT:
            if (expert_freeze_allowed()) {
                maincpu_set_nmi(cartridge_int_num, IK_NMI);
                alarm_set(cartridge_alarm, maincpu_clk + 3);
            }
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
    if (isepic_freeze_allowed()) {
        maincpu_set_nmi(cartridge_int_num, IK_NMI);
        alarm_set(cartridge_alarm, maincpu_clk + 3);
    }
}

void cartridge_trigger_freeze_nmi_only(void)
{
    maincpu_set_nmi(cartridge_int_num, IK_NMI);
}

void cartridge_release_freeze(void)
{
    maincpu_set_nmi(cartridge_int_num, 0);
}

const char *cartridge_get_file_name(WORD addr_ignored)
{
    return cartfile;
}
