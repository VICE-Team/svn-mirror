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
#include "cartridge.h"
#include "cmdline.h"
#include "crt.h"
#include "expert.h"
#include "generic.h"
#include "ide64.h"
#include "interrupt.h"
#include "lib.h"
#include "maincpu.h"
#include "mem.h"
#include "monitor.h"
#include "resources.h"
#include "retroreplay.h"
#include "stardos.h"
#include "stb.h"
#include "supersnapshot.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "util.h"


static int cartridge_type;
static char *cartridge_file = NULL;
static int cartridge_mode;
static int c64cartridge_reset;

int c64cart_type = CARTRIDGE_NONE;

int cartmode = CARTRIDGE_MODE_OFF;

int cartres = 0;
static char *cartfile = NULL;

static alarm_t *cartridge_alarm = NULL;

static unsigned int cartridge_int_num;

static int trying_cart = 0;

int try_cartridge_init(int c)
{
    cartres ^= c;

    if (cartres)
        return 0;

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

static int set_cartridge_mode(int val, void *param)
{
    const int type = ((c64cart_type == CARTRIDGE_CRT) ? crttype : c64cart_type);

    /*
     * Set cartridge mode.
     */
    cartridge_mode = val;
    cartmode = cartridge_mode;

    switch (type) {
      case (CARTRIDGE_EXPERT):
        expert_mode_changed(cartridge_mode);
        break;
    }

    return try_cartridge_init(4);
}

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
    { "CartridgeMode", CARTRIDGE_MODE_OFF, RES_EVENT_NO, NULL,
      &cartridge_mode, set_cartridge_mode, NULL },
    { "CartridgeReset", 1, RES_EVENT_NO, NULL,
      &c64cartridge_reset, set_cartridge_reset, NULL },
    { NULL }
};

int cartridge_resources_init(void)
{
    if (ide64_resources_init() < 0)
        return -1;

    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void cartridge_resources_shutdown(void)
{
    lib_free(cartridge_file);
    lib_free(cartfile);
    lib_free(ide64_image_file);
}

static int attach_cartridge_cmdline(const char *param, void *extra_param)
{
    return cartridge_attach_image((int)extra_param, param);
}

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] =
{
    { "-cartreset", SET_RESOURCE, 0, NULL, NULL, "CartridgeReset",
      (void *)1, 0,
      IDCLS_CART_ATTACH_DETACH_RESET },
    { "+cartreset", SET_RESOURCE, 0, NULL, NULL, "CartridgeReset",
      (void *)0, 0,
      IDCLS_CART_ATTACH_DETACH_NO_RESET },
    { "-cartcrt", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_CRT, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_CRT_CART },
    { "-cart8", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_GENERIC_8KB, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_GENERIC_8KB_CART },
    { "-cart16", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_GENERIC_16KB, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_GENERIC_16KB_CART },
    { "-cartar", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_ACTION_REPLAY, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ACTION_REPLAY_CART },
    { "-cartar3", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_ACTION_REPLAY3, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART },
    { "-cartar4", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_ACTION_REPLAY4, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART },
    { "-cartrr", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_RETRO_REPLAY, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_RETRO_REPLAY_CART },
    { "-cartide", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_IDE64, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_IDE64_CART },
    { "-cartap", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_ATOMIC_POWER, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ATOMIC_POWER_CART },
    { "-cartepyx", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_EPYX_FASTLOAD, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART },
    { "-cartss4", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_SUPER_SNAPSHOT, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_SS4_CART },
    { "-cartss5", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_SUPER_SNAPSHOT_V5, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_SS5_CART },
    { "-cartieee488", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_IEEE488, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_CBM_IEEE488_CART },
    { "-cartwestermann", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_WESTERMANN, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_WESTERMANN_CART },
    { "-cartstb", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_STRUCTURED_BASIC, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_STB_CART },
    { "-cartstardos", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_STARDOS, NULL, NULL,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_STARDOS_CART },
    { "-cartexpert", CALL_FUNCTION, 0, attach_cartridge_cmdline,
      (void *)CARTRIDGE_EXPERT, NULL, NULL,
      0, IDCLS_ENABLE_EXPERT_CART },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] =
{
    { "-cartreset", SET_RESOURCE, 0, NULL, NULL, "CartridgeReset",
      (void *)1, NULL,
      N_("Reset machine if a cartridge is attached or detached") },
    { "+cartreset", SET_RESOURCE, 0, NULL, NULL, "CartridgeReset",
      (void *)0, NULL,
      N_("Do not reset machine if a cartridge is attached or detached") },
    { "-cartcrt", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_CRT, NULL, NULL,
      N_("<name>"), N_("Attach CRT cartridge image") },
    { "-cart8", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_GENERIC_8KB, NULL, NULL,
      N_("<name>"), N_("Attach generic 8KB cartridge image") },
    { "-cart16", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_GENERIC_16KB, NULL, NULL,
      N_("<name>"), N_("Attach generic 16KB cartridge image") },
    { "-cartar", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_ACTION_REPLAY, NULL, NULL,
      N_("<name>"), N_("Attach raw 32KB Action Replay cartridge image") },
    { "-cartar3", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_ACTION_REPLAY3, NULL, NULL,
      N_("<name>"), N_("Attach raw 16KB Action Replay III cartridge image") },
    { "-cartar4", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_ACTION_REPLAY4, NULL, NULL,
      N_("<name>"), N_("Attach raw 32KB Action Replay 4 cartridge image") },
    { "-cartrr", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_RETRO_REPLAY, NULL, NULL,
      N_("<name>"), N_("Attach raw 64KB Retro Replay cartridge image") },
    { "-cartide", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_IDE64, NULL, NULL,
      N_("<name>"), N_("Attach raw 64KB IDE64 cartridge image") },
    { "-cartap", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_ATOMIC_POWER, NULL, NULL,
      N_("<name>"), N_("Attach raw 32KB Atomic Power cartridge image") },
    { "-cartepyx", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_EPYX_FASTLOAD, NULL, NULL,
      N_("<name>"), N_("Attach raw 8KB Epyx fastload cartridge image") },
    { "-cartss4", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_SUPER_SNAPSHOT, NULL, NULL,
      N_("<name>"), N_("Attach raw 32KB Super Snapshot cartridge image") },
    { "-cartss5", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_SUPER_SNAPSHOT_V5, NULL, NULL,
      N_("<name>"), N_("Attach raw 64KB Super Snapshot cartridge image") },
    { "-cartieee488", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_IEEE488, NULL, NULL,
      N_("<name>"), N_("Attach CBM IEEE488 cartridge image") },
    { "-cartwestermann", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_WESTERMANN, NULL, NULL,
      N_("<name>"), N_("Attach raw 16KB Westermann learning cartridge image") },
    { "-cartstb", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_STRUCTURED_BASIC, NULL, NULL,
      N_("<name>"), N_("Attach raw Structured Basic cartridge image") },
    { "-cartstardos", CALL_FUNCTION, 1, attach_cartridge_cmdline,
      (void *)CARTRIDGE_STARDOS, NULL, NULL,
      N_("<name>"), N_("Attach raw StarDOS image") },
    { "-cartexpert", CALL_FUNCTION, 0, attach_cartridge_cmdline,
      (void *)CARTRIDGE_EXPERT, NULL, NULL,
      NULL, N_("Enable expert cartridge") },
    { NULL }
};
#endif

int cartridge_cmdline_options_init(void)
{
    mon_cart_cmd.cartridge_attach_image = cartridge_attach_image;
    mon_cart_cmd.cartridge_detach_image = cartridge_detach_image;
    mon_cart_cmd.cartridge_trigger_freeze = cartridge_trigger_freeze;
    mon_cart_cmd.cartridge_trigger_freeze_nmi_only = cartridge_trigger_freeze_nmi_only;

    if (ide64_cmdline_options_init() < 0)
        return -1;

    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int cartridge_attach_image(int type, const char *filename)
{
    BYTE *rawcart;
    FILE *fd;

    /* The expert cartridge does not have a filename.
     * It should only be enabled without loading an image.
     */
    if (type != CARTRIDGE_EXPERT) {
        /* Attaching no cartridge always works. */
        if (type == CARTRIDGE_NONE || *filename == '\0')
            return 0;
    }

    /* allocate temporary array */
    rawcart = lib_malloc(0x88000);

/*  cart should always be detached. there is no reason for doing fancy checks
    here, and it will cause problems incase a cart MUST be detached before
    attaching another, or even itself. (eg for initialization reasons)
    
    most obvious reason: attaching a different ROM (software) for the same
    cartridge (hardware) */

    if (trying_cart == 0)
      cartridge_detach_image();
    else
      trying_cart = 0;

    switch(type) {
      case CARTRIDGE_GENERIC_8KB:
      case CARTRIDGE_EPYX_FASTLOAD:
        if (generic_8kb_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_GENERIC_16KB:
      case CARTRIDGE_WESTERMANN:
      case CARTRIDGE_WARPSPEED:
        if (generic_16kb_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_ACTION_REPLAY3:
        if (actionreplay3_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_ACTION_REPLAY4:
        if (actionreplay4_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_ACTION_REPLAY:
        if (actionreplay_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_ATOMIC_POWER:
        if (atomicpower_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_RETRO_REPLAY:
        if (retroreplay_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_IDE64:
        if (c64cart_type==CARTRIDGE_IDE64)
            ide64_detach(); /* detach IDE64 if reattaching */
        if (ide64_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        if (supersnapshot_v4_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        if (supersnapshot_v5_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_EXPERT:
        if (expert_bin_attach(filename, rawcart) < 0)
            goto done;
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
        if (crt_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_STRUCTURED_BASIC:
        if (stb_bin_attach(filename, rawcart) < 0)
            goto done;
        break;
      case CARTRIDGE_STARDOS:
        if (stardos_bin_attach(filename, rawcart) < 0)
            goto done;
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
        cartridge_detach((c64cart_type == CARTRIDGE_CRT)
                         ? crttype : c64cart_type);
        c64cart_type = CARTRIDGE_NONE;
        crttype = CARTRIDGE_NONE;
        cartridge_type = CARTRIDGE_NONE;        /* Resource value updated! */
        if (cartfile != NULL)
            lib_free(cartfile), cartfile = NULL;
    }
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
    cartridge_alarm = alarm_new(maincpu_alarm_context, "Cartridge",
                                cartridge_change_mapping, NULL);
    cartridge_int_num = interrupt_cpu_status_int_new(maincpu_int_status,
                                                     "Cartridge");
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
      case CARTRIDGE_SUPER_SNAPSHOT:
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
      case CARTRIDGE_ATOMIC_POWER:
      case CARTRIDGE_FINAL_I:
        maincpu_set_nmi(cartridge_int_num, IK_NMI);
        alarm_set(cartridge_alarm, maincpu_clk + 3);
        break;
      case CARTRIDGE_RETRO_REPLAY:
        if (retroreplay_freeze_allowed()) {
            maincpu_set_nmi(cartridge_int_num, IK_NMI);
            alarm_set(cartridge_alarm, maincpu_clk + 3);
        }
        break;
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

