/*
 * c64carthooks.c - C64 cartridge emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alarm.h"
#include "archdep.h"
#include "actionreplay2.h"
#include "actionreplay3.h"
#include "actionreplay4.h"
#include "actionreplay.h"
#include "atomicpower.h"
#include "c64.h"
#include "c64acia.h"
#include "c64cart.h"
#include "c64cartsystem.h"
#include "c64export.h"
#include "c64mem.h"
#include "c64-midi.h"
#include "c64tpi.h"
#include "c64cartmem.h"
#include "c64io.h"
#include "cartridge.h"
#include "cmdline.h"
#include "comal80.h"
#include "capture.h"
#include "crt.h"
#include "delaep256.h"
#include "delaep64.h"
#include "delaep7x8.h"
#include "digimax.h"
#include "dinamic.h"
#include "dqbb.h"
#include "easyflash.h"
#include "epyxfastload.h"
#include "exos.h"
#include "expert.h"
#include "final.h"
#include "finalplus.h"
#include "final3.h"
#include "freezeframe.h"
#include "freezemachine.h"
#include "funplay.h"
#include "gamekiller.h"
#include "generic.h"
#include "georam.h"
#include "gs.h"
#include "ide64.h"
#include "interrupt.h"
#include "isepic.h"
#include "kcs.h"
#include "lib.h"
#include "log.h"
#include "mach5.h"
#include "machine.h"
#include "maincpu.h"
#include "magicdesk.h"
#include "magicformel.h"
#include "magicvoice.h"
#include "mem.h"
#include "mikroass.h"
#include "mmc64.h"
#include "mmcreplay.h"
#include "monitor.h"
#include "sfx_soundexpander.h"
#include "sfx_soundsampler.h"
#include "ocean.h"
#include "prophet64.h"
#include "ramcart.h"
#include "resources.h"
#include "retroreplay.h"
#include "reu.h"
#include "rexep256.h"
#include "rexutility.h"
#include "ramcart.h"
#include "ross.h"
#include "simonsbasic.h"
#include "snapshot64.h"
#include "stardos.h"
#include "stb.h"
#include "supergames.h"
#include "superexplode5.h"
#include "supersnapshot.h"
#include "supersnapshot4.h"
#ifdef HAVE_TFE
#include "tfe.h"
#endif
#include "translate.h"
#include "tpi.h"
#include "types.h"
#include "util.h"
#include "warpspeed.h"
#include "westermann.h"
#include "zaxxon.h"

/* #define DEBUGCART */

#ifdef DEBUGCART
#define DBG(x)  printf x
#else
#define DBG(x)
#endif

/*
    this file is supposed to include ONLY the implementations of all non-memory
    related (which go into c64cartmem.c) hooks that wrap to the individual cart
    implementations.
*/

/* from c64cart.c */
extern int mem_cartridge_type; /* Type of the cartridge attached. ("Main Slot") */

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
    /* generic cartridges */
    { "-cart8", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_GENERIC_8KB, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_GENERIC_8KB_CART,
      NULL, NULL },
    { "-cart16", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_GENERIC_16KB, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_GENERIC_16KB_CART,
      NULL, NULL },
    { "-cartultimax", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_ULTIMAX, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach generic 16kB Ultimax Cartridge image") },
    /* smart-insert CRT */
    { "-cartcrt", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_CRT, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_CRT_CART,
      NULL, NULL },
    /* binary images: */
    { "-cartar2", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_ACTION_REPLAY2, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 16kB Action Replay 2 image") },
    { "-cartar3", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_ACTION_REPLAY3, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ACTION_REPLAY3_CART,
      NULL, NULL },
    { "-cartar4", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_ACTION_REPLAY4, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ACTION_REPLAY4_CART,
      NULL, NULL },
    { "-cartar5", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_ACTION_REPLAY, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_ACTION_REPLAY_CART },
    { "-cartap", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_ATOMIC_POWER, NULL, NULL,
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
      cart_attach_cmdline, (void *)CARTRIDGE_EPYX_FASTLOAD, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_EPYX_FASTLOAD_CART,
      NULL, NULL },
    { "-cartexos", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_EXOS, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 8kB Exos cartridge image") },
    { "-cartexpert", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_EXPERT, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 8kB Expert cartridge image") },
    { "-cartfc", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_FINAL_I, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 16kB Final Cartridge image") },
    { "-cartfc3", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_FINAL_III, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 64kB Final Cartridge 3 image") },
    { "-cartfcplus", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_FINAL_PLUS, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 32kB Final Cartridge Plus image") },
    { "-cartff", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_FREEZE_FRAME, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 8kB Freeze Frame image") },
    { "-cartfm", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_FREEZE_MACHINE, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 32kB Freeze Machine image") },
    /* TODO: CARTRIDGE_FUNPLAY */
    { "-cartgamekiller", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_GAME_KILLER, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_GAME_KILLER_CART,
      NULL, NULL },
    /* TODO: CARTRIDGE_GEORAM */
    /* TODO: CARTRIDGE_GS */
    { "-cartide", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_IDE64, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_IDE64_CART,
      NULL, NULL },
    { "-cartieee488", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_IEEE488, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_CBM_IEEE488_CART,
      NULL, NULL },
    { "-cartisepic", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_ISEPIC, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 2kB ISEPIC image") },
    /* TODO: CARTRIDGE_KCS_POWER */
    /* TODO: CARTRIDGE_MAGIC_DESK */
    /* TODO: CARTRIDGE_MAGIC_FORMEL */
    /* TODO: CARTRIDGE_MIKRO_ASSEMBLER */
    /* TODO: CARTRIDGE_MMC64 */
    { "-cartmach5", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_MACH5, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 8kB Mach 5 cartridge image") },
    { "-cartmmcr", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_MMC_REPLAY, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 512kB MMC Replay cartridge image") },
    { "-cartmv", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_MAGIC_VOICE, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 16kB Magic Voice image") },
    /* TODO: CARTRIDGE_OCEAN */
    { "-cartp64", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_P64, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_P64_CART,
      NULL, NULL },
    /* TODO: CARTRIDGE_RAMCART */
    /* TODO: CARTRIDGE_REU */
    /* TODO: CARTRIDGE_REX */
    /* TODO: CARTRIDGE_REX_EP256 */
    /* TODO: CARTRIDGE_ROSS */
    { "-cartrr", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_RETRO_REPLAY, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_RETRO_REPLAY_CART,
      NULL, NULL },
    { "-cartsb", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_SIMONS_BASIC, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 16kB Simons Basic image") },
    /* TODO: CARTRIDGE_SUPER_GAMES */
    { "-carts64", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_SNAPSHOT64, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 4kB Snapshot 64 image") },
    { "-cartstardos", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_STARDOS, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_STARDOS_CART,
      NULL, NULL },
    { "-cartstb", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_STRUCTURED_BASIC, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_STB_CART,
      NULL, NULL },
    { "-cartse5", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_SUPER_EXPLODE_V5, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 16kB Super Explode V5 image") },
    { "-cartss4", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_SUPER_SNAPSHOT, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_SS4_CART,
      NULL, NULL },
    { "-cartss5", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_SUPER_SNAPSHOT_V5, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_SS5_CART,
      NULL, NULL },
    { "-cartws", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_WARPSPEED, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 8kB Warpspeed image") },
    { "-cartwestermann", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_WESTERMANN, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_ATTACH_RAW_WESTERMANN_CART,
      NULL, NULL },
    { "-cartzaxxon", CALL_FUNCTION, 1,
      cart_attach_cmdline, (void *)CARTRIDGE_ZAXXON, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_STRING,
      IDCLS_P_NAME, IDCLS_UNUSED,
      NULL, T_("Attach raw 16kB Zaxxon image") },
    { NULL }
};

int cart_cmdline_options_init(void)
{
        /* "Slot 1" */
    if (mmc64_cmdline_options_init() < 0
        /* "Slot 0" */
        || dqbb_cmdline_options_init() < 0
        || expert_cmdline_options_init() < 0
        || isepic_cmdline_options_init() < 0
        || ramcart_cmdline_options_init() < 0
        /* "I/O Slot" */
#ifdef HAVE_MIDI
        || c64_midi_cmdline_options_init() < 0
#endif
        || aciacart_cmdline_options_init() < 0
        || digimax_cmdline_options_init() < 0
        || georam_cmdline_options_init() < 0
        || reu_cmdline_options_init() < 0
        || sfx_soundexpander_cmdline_options_init() < 0
        || sfx_soundsampler_cmdline_options_init() < 0
#ifdef HAVE_TFE
        || tfe_cmdline_options_init() < 0
#endif
        /* "Main Slot" */
        || easyflash_cmdline_options_init() < 0
        || ide64_cmdline_options_init() < 0
        || mmcreplay_cmdline_options_init() < 0
        || retroreplay_cmdline_options_init() < 0
        ) {
        return -1;
    }
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int cart_resources_init(void)
{
        /* "Slot 0" */
    if (mmc64_resources_init() < 0
        || magicvoice_resources_init() < 0
        /* "Slot 1" */
        || expert_resources_init() < 0
        || dqbb_resources_init() < 0
        || isepic_resources_init() < 0
        || ramcart_resources_init() < 0
        /* "IO Slot" */
#ifdef HAVE_MIDI
        || c64_midi_resources_init() < 0
#endif
        || aciacart_resources_init() < 0
        || digimax_resources_init() < 0
        || georam_resources_init() < 0
        || reu_resources_init() < 0
        || sfx_soundexpander_resources_init() < 0
        || sfx_soundsampler_resources_init() < 0
#ifdef HAVE_TFE
        || tfe_resources_init() < 0
#endif
        || tpi_resources_init() < 0
        /* "Main Slot" */
        || easyflash_resources_init() < 0
        || ide64_resources_init() < 0
        || mmcreplay_resources_init() < 0
        || retroreplay_resources_init() < 0
        ) {
        return -1;
    }
    return 0;
}

void cart_resources_shutdown(void)
{
    /* "IO Slot" */
    aciacart_resources_shutdown();
    georam_resources_shutdown();
#ifdef HAVE_MIDI
    midi_resources_shutdown();
#endif
    reu_resources_shutdown();
#ifdef HAVE_TFE
    tfe_resources_shutdown();
#endif
    tpi_resources_shutdown();
    /* "Main Slot" */
    ide64_resources_shutdown();
    mmcreplay_resources_shutdown();
    /* "Slot 1" */
    expert_resources_shutdown();
    dqbb_resources_shutdown();
    ramcart_resources_shutdown();
    /* "Slot 0" */
    mmc64_resources_shutdown();
    magicvoice_resources_shutdown();
}

/* ------------------------------------------------------------------------- */

/*
    returns 1 if given cart type is in "Main Slot"
*/
int cart_is_slotmain(int type)
{
   switch (type) {
        /* slot 0 */
        case CARTRIDGE_MMC64:
        case CARTRIDGE_MAGIC_VOICE:
        case CARTRIDGE_IEEE488:
        /* slot 1 */
        case CARTRIDGE_DQBB:
        case CARTRIDGE_EXPERT:
        case CARTRIDGE_ISEPIC:
        case CARTRIDGE_RAMCART:
        /* io slot */
        case CARTRIDGE_DIGIMAX:
        case CARTRIDGE_GEORAM:
        case CARTRIDGE_MIDI_PASSPORT:
        case CARTRIDGE_MIDI_DATEL:
        case CARTRIDGE_MIDI_SEQUENTIAL:
        case CARTRIDGE_MIDI_NAMESOFT:
        case CARTRIDGE_MIDI_MAPLIN:
        case CARTRIDGE_REU:
        case CARTRIDGE_SFX_SOUND_EXPANDER:
        case CARTRIDGE_SFX_SOUND_SAMPLER:
        case CARTRIDGE_TFE:
        case CARTRIDGE_TURBO232:
            return 0;
        default:
            return 1;
    }
}

/* ------------------------------------------------------------------------- */

/*
    returns 1 if the cartridge of the given type is enabled

    FIXME: incomplete, but currently only used by c64iec.c:iec_available_busses
*/
int cart_type_enabled(int type)
{
    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            return tpi_cart_enabled();
        case CARTRIDGE_MAGIC_VOICE:
            return magicvoice_cart_enabled();
        case CARTRIDGE_MMC64:
            return mmc64_cart_enabled();
        /* "Slot 1" */
        case CARTRIDGE_DQBB:
            return dqbb_cart_enabled();
        case CARTRIDGE_EXPERT:
            return expert_cart_enabled();
        case CARTRIDGE_ISEPIC:
            return isepic_cart_enabled();
        case CARTRIDGE_RAMCART:
            return ramcart_cart_enabled();
        /* "I/O Slot" */
        /* Main Slot handled in c64cart.c */
    }
    return 0;
}

/*
    get filename of cart with given type

    FIXME: only works for carts in "Main Slot" (handled in c64cart.c)
*/
const char *cart_get_file_name(int type)
{
    /* FIXME */
    return "";
}

/* ------------------------------------------------------------------------- */

/* FIXME: shutdown missing */

/* called once by machine_setup_context */
void cartridge_setup_context(machine_context_t *machine_context)
{
    tpi_setup_context(machine_context);
    magicvoice_setup_context(machine_context);
}

/* ------------------------------------------------------------------------- */

int cart_bin_attach(int type, const char *filename, BYTE *rawcart)
{
    switch(type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            return tpi_bin_attach(filename, rawcart);
        case CARTRIDGE_MAGIC_VOICE:
            return magicvoice_bin_attach(filename, rawcart);
        /* FIXME: MMC64 missing */
        /* "Slot 1" */
        /* FIXME: dqbb missing */
        case CARTRIDGE_EXPERT:
            return expert_bin_attach(filename, rawcart);
        case CARTRIDGE_ISEPIC:
            return isepic_bin_attach(filename, rawcart);
        /* FIXME: ramcart missing */
        /* "I/O Slot" */
        /* "Main Slot" */
        case CARTRIDGE_ACTION_REPLAY:
            return actionreplay_bin_attach(filename, rawcart);
        case CARTRIDGE_ACTION_REPLAY2:
            return actionreplay2_bin_attach(filename, rawcart);
        case CARTRIDGE_ACTION_REPLAY3:
            return actionreplay3_bin_attach(filename, rawcart);
        case CARTRIDGE_ACTION_REPLAY4:
            return actionreplay4_bin_attach(filename, rawcart);
        case CARTRIDGE_ATOMIC_POWER:
            return atomicpower_bin_attach(filename, rawcart);
        case CARTRIDGE_EPYX_FASTLOAD:
            return epyxfastload_bin_attach(filename, rawcart);
        case CARTRIDGE_EXOS:
            return exos_bin_attach(filename, rawcart);
        case CARTRIDGE_FINAL_I:
            return final_v1_bin_attach(filename, rawcart);
        case CARTRIDGE_FINAL_III:
            return final_v3_bin_attach(filename, rawcart);
        case CARTRIDGE_FINAL_PLUS:
            return final_plus_bin_attach(filename, rawcart);
        case CARTRIDGE_FREEZE_FRAME:
            return freezeframe_bin_attach(filename, rawcart);
        case CARTRIDGE_FREEZE_MACHINE:
            return freezemachine_bin_attach(filename, rawcart);
        case CARTRIDGE_GAME_KILLER:
            return gamekiller_bin_attach(filename, rawcart);
        case CARTRIDGE_GENERIC_8KB:
            return generic_8kb_bin_attach(filename, rawcart);
        case CARTRIDGE_GENERIC_16KB:
            return generic_16kb_bin_attach(filename, rawcart);
        case CARTRIDGE_IDE64:
/* FIXME: test if it works and then delete this */
#if 0
            if (c64cart_type==CARTRIDGE_IDE64) {
                ide64_detach(); /* detach IDE64 if reattaching */
            }
            if (ide64_bin_attach(filename, rawcart) < 0) {
                return -1;
            }
#endif
            return ide64_bin_attach(filename, rawcart);
        case CARTRIDGE_MACH5:
            return mach5_bin_attach(filename, rawcart);
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_bin_attach(filename, rawcart);
        case CARTRIDGE_P64:
            return p64_bin_attach(filename, rawcart);
        case CARTRIDGE_RETRO_REPLAY:
            return retroreplay_bin_attach(filename, rawcart);
        case CARTRIDGE_SIMONS_BASIC:
            return simon_bin_attach(filename, rawcart);
        case CARTRIDGE_SNAPSHOT64:
            return snapshot64_bin_attach(filename, rawcart);
        case CARTRIDGE_STARDOS:
            return stardos_bin_attach(filename, rawcart);
        case CARTRIDGE_STRUCTURED_BASIC:
            return stb_bin_attach(filename, rawcart);
        case CARTRIDGE_SUPER_EXPLODE_V5:
            return se5_bin_attach(filename, rawcart);
        case CARTRIDGE_SUPER_SNAPSHOT:
            return supersnapshot_v4_bin_attach(filename, rawcart);
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            return supersnapshot_v5_bin_attach(filename, rawcart);
        case CARTRIDGE_ULTIMAX:
            return generic_ultimax_bin_attach(filename, rawcart);
        case CARTRIDGE_WARPSPEED:
            return warpspeed_bin_attach(filename, rawcart);
        case CARTRIDGE_WESTERMANN:
            return westermann_bin_attach(filename, rawcart);
        case CARTRIDGE_ZAXXON:
            return zaxxon_bin_attach(filename, rawcart);
    }
    return -1;
}

/*
    called by cartridge_attach_image after cart_crt/bin_attach
    XYZ_config_setup should copy the raw cart image into the
    individual implementations array.
*/
void cart_attach(int type, BYTE *rawcart)
{
    cart_detach_conflicting(type);
    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            tpi_config_setup(rawcart);
            break;
        case CARTRIDGE_MAGIC_VOICE:
            magicvoice_config_setup(rawcart);
            break;
        case CARTRIDGE_MMC64:
            mmc64_config_setup(rawcart);
            break;
        /* "Slot 1" */
        case CARTRIDGE_EXPERT:
            expert_config_setup(rawcart);
            break;
        /* FIXME: ISEPIC missing here ? */
        /* "IO Slot" */
        /* "Main Slot" */
        case CARTRIDGE_ACTION_REPLAY:
            actionreplay_config_setup(rawcart);
            break;
        case CARTRIDGE_ACTION_REPLAY2:
            actionreplay2_config_setup(rawcart);
            break;
        case CARTRIDGE_ACTION_REPLAY3:
            actionreplay3_config_setup(rawcart);
            break;
        case CARTRIDGE_ACTION_REPLAY4:
            actionreplay4_config_setup(rawcart);
            break;
        case CARTRIDGE_ATOMIC_POWER:
            atomicpower_config_setup(rawcart);
            break;
        case CARTRIDGE_CAPTURE:
            capture_config_setup(rawcart);
            break;
        case CARTRIDGE_COMAL80:
            comal80_config_setup(rawcart);
            break;
        case CARTRIDGE_DELA_EP256:
            delaep256_config_setup(rawcart);
            break;
        case CARTRIDGE_DELA_EP64:
            delaep64_config_setup(rawcart);
            break;
        case CARTRIDGE_DELA_EP7x8:
            delaep7x8_config_setup(rawcart);
            break;
        case CARTRIDGE_DINAMIC:
            dinamic_config_setup(rawcart);
            break;
        case CARTRIDGE_EASYFLASH:
            easyflash_config_setup(rawcart);
            break;
        case CARTRIDGE_EPYX_FASTLOAD:
            epyxfastload_config_setup(rawcart);
            break;
        case CARTRIDGE_EXOS:
            exos_config_setup(rawcart);
            break;
        case CARTRIDGE_FINAL_I:
            final_v1_config_setup(rawcart);
            break;
        case CARTRIDGE_FINAL_III:
            final_v3_config_setup(rawcart);
            break;
        case CARTRIDGE_FINAL_PLUS:
            final_plus_config_setup(rawcart);
            break;
        case CARTRIDGE_FREEZE_FRAME:
            freezeframe_config_setup(rawcart);
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            freezemachine_config_setup(rawcart);
            break;
        case CARTRIDGE_FUNPLAY:
            funplay_config_setup(rawcart);
            break;
        case CARTRIDGE_GAME_KILLER:
            gamekiller_config_setup(rawcart);
            break;
        case CARTRIDGE_GENERIC_8KB:
            generic_8kb_config_setup(rawcart);
            break;
        case CARTRIDGE_GENERIC_16KB:
            generic_16kb_config_setup(rawcart);
            break;
        case CARTRIDGE_GS:
            gs_config_setup(rawcart);
            break;
        case CARTRIDGE_IDE64:
            ide64_config_setup(rawcart);
            break;
        case CARTRIDGE_KCS_POWER:
            kcs_config_setup(rawcart);
            break;
        case CARTRIDGE_MACH5:
            mach5_config_setup(rawcart);
            break;
        case CARTRIDGE_MAGIC_DESK:
            magicdesk_config_setup(rawcart);
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_config_setup(rawcart);
            break;
        case CARTRIDGE_MIKRO_ASSEMBLER:
            mikroass_config_setup(rawcart);
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_config_setup(rawcart);
            break;
        case CARTRIDGE_OCEAN:
            ocean_config_setup(rawcart);
            break;
        case CARTRIDGE_P64:
            p64_config_setup(rawcart);
            break;
        case CARTRIDGE_RETRO_REPLAY:
            retroreplay_config_setup(rawcart);
            break;
        case CARTRIDGE_REX:
            rex_config_setup(rawcart);
            break;
        case CARTRIDGE_REX_EP256:
            rexep256_config_setup(rawcart);
            break;
        case CARTRIDGE_ROSS:
            ross_config_setup(rawcart);
            break;
        case CARTRIDGE_SIMONS_BASIC:
            simon_config_setup(rawcart);
            break;
        case CARTRIDGE_SNAPSHOT64:
            snapshot64_config_setup(rawcart);
            break;
        case CARTRIDGE_STARDOS:
            stardos_config_setup(rawcart);
            break;
        case CARTRIDGE_STRUCTURED_BASIC:
            stb_config_setup(rawcart);
            break;
        case CARTRIDGE_SUPER_EXPLODE_V5:
            se5_config_setup(rawcart);
            break;
        case CARTRIDGE_SUPER_GAMES:
            supergames_config_setup(rawcart);
            break;
        case CARTRIDGE_SUPER_SNAPSHOT:
            supersnapshot_v4_config_setup(rawcart);
            break;
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            supersnapshot_v5_config_setup(rawcart);
            break;
        case CARTRIDGE_ULTIMAX:
            generic_ultimax_config_setup(rawcart);
            break;
        case CARTRIDGE_WARPSPEED:
            warpspeed_config_setup(rawcart);
            break;
        case CARTRIDGE_WESTERMANN:
            westermann_config_setup(rawcart);
            break;
        case CARTRIDGE_ZAXXON:
            zaxxon_config_setup(rawcart);
            break;
        default:
            DBG(("CART: no attach hook %d\n", type));
            break;
    }
}

/* only one of the "Slot 0" carts can be enabled at a time */
static int slot0conflicts[]=
{
    CARTRIDGE_IEEE488,
    CARTRIDGE_MAGIC_VOICE,
    CARTRIDGE_MMC64,
    0
};

void cart_detach_conflicts0(int *list, int type)
{
    int *l = list;
    /* find in list */
    while (*l != 0) {
        if (*l == type) {
            /* if in list, remove all others */
            while (*list != 0) {
                if (*list != type) {
                    if (cartridge_type_enabled(*list)) {
                        DBG(("CART: detach conflicting cart: %d (only one Slot 0 cart can be active)\n", *list));
                        cartridge_detach_image(*list);
                    }
                }
                list++;
            }
            return;
        }
        l++;
    }
}

void cart_detach_conflicting(int type)
{
    DBG(("CART: detach conflicting for type: %d ...\n", type));
    cart_detach_conflicts0(slot0conflicts, type);
}

/* FIXME: many still missing */
int cartridge_enable(int type)
{
    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            tpi_enable();
            break;
        case CARTRIDGE_MAGIC_VOICE:
            magicvoice_enable();
            break;
        /* FIXME: MMC64 */
        /* "Slot 1" */
        case CARTRIDGE_DQBB:
            dqbb_enable();
            break;
        case CARTRIDGE_EXPERT:
            expert_enable();
            break;
        /* FIXME: ISEPIC */
        case CARTRIDGE_RAMCART:
            ramcart_enable();
            break;
        /* "I/O Slot" */
        /* FIXME: missing: georam, tfe, digimax, midi, sfx expander, sfx sampler */
        case CARTRIDGE_REU:
            reu_enable();
            break;
        case CARTRIDGE_TURBO232:
            aciacart_enable();
            break;
        /* "Main Slot" */
        default:
            DBG(("CART: no enable hook %d\n", type));
            break;
    }
    cart_detach_conflicting(type);
    return -1;
}

/*
    detach a cartridge.
    - carts that are not "main" cartridges can be disabled individually

    - carts not in "Main Slot" must make sure their _detach hook does not
      fail when it is called and the cart is not actually attached.
*/
void cart_detach_all(void)
{
    DBG(("CART: detach all\n"));
    /* detach all cartridges */
    /* "slot 0" */
    tpi_detach();
    magicvoice_detach();
    mmc64_detach();
    /* "Slot 1" */
    dqbb_detach();
    expert_detach();
    isepic_detach();
    ramcart_detach();
    /* "io Slot" */
    /* FIXME: missing: digimax, midi, sfx expander, sfx sampler */
    aciacart_detach();
    georam_detach();
    reu_detach();
#ifdef HAVE_TFE
    tfe_shutdown(); /* FIXME: review and rename */
#endif
    /* "Main Slot" */
    cart_detach_main();
    return ;
}

void cart_detach(int type)
{
    DBG(("CART: cart_detach ID: %d\n", type));

    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            tpi_detach();
            break;
        case CARTRIDGE_MAGIC_VOICE:
            magicvoice_detach();
            break;
        case CARTRIDGE_MMC64:
            mmc64_detach();
            break;
        /* "Slot 1" */
        case CARTRIDGE_DQBB:
            dqbb_detach();
            break;
        case CARTRIDGE_EXPERT:
            expert_detach();
            break;
        case CARTRIDGE_ISEPIC:
            isepic_detach();
            break;
        case CARTRIDGE_RAMCART:
            ramcart_detach();
            break;
        /* "IO Slot" */
        /* FIXME: missing: acia, digimax, midi, sfx expander, sfx sampler */
        case CARTRIDGE_GEORAM:
            georam_detach();
            break;
        case CARTRIDGE_REU:
            reu_detach();
            break;
#ifdef HAVE_TFE
        case CARTRIDGE_TFE:
            tfe_shutdown(); /* FIXME: review and rename */
            break;
#endif
        case CARTRIDGE_TURBO232:
            aciacart_detach();
            break;
        /* "Main Slot" */
        case CARTRIDGE_ACTION_REPLAY:
            actionreplay_detach();
            break;
        case CARTRIDGE_ACTION_REPLAY2:
            actionreplay2_detach();
            break;
        case CARTRIDGE_ACTION_REPLAY3:
            actionreplay3_detach();
            break;
        case CARTRIDGE_ACTION_REPLAY4:
            actionreplay4_detach();
            break;
        case CARTRIDGE_ATOMIC_POWER:
            atomicpower_detach();
            break;
        case CARTRIDGE_CAPTURE:
            capture_detach();
            break;
        case CARTRIDGE_COMAL80:
            comal80_detach();
            break;
        case CARTRIDGE_DELA_EP64:
            delaep64_detach();
            break;
        case CARTRIDGE_DELA_EP7x8:
            delaep7x8_detach();
            break;
        case CARTRIDGE_DELA_EP256:
            delaep256_detach();
            break;
        case CARTRIDGE_DINAMIC:
            dinamic_detach();
            break;
        case CARTRIDGE_EASYFLASH:
            easyflash_detach();
            break;
        case CARTRIDGE_EPYX_FASTLOAD:
            epyxfastload_detach();
            break;
        case CARTRIDGE_EXOS:
            exos_detach();
            break;
        case CARTRIDGE_FINAL_I:
            final_v1_detach();
            break;
        case CARTRIDGE_FINAL_III:
            final_v3_detach();
            break;
        case CARTRIDGE_FINAL_PLUS:
            final_plus_detach();
            break;
        case CARTRIDGE_FREEZE_FRAME:
            freezeframe_detach();
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            freezemachine_detach();
            break;
        case CARTRIDGE_FUNPLAY:
            funplay_detach();
            break;
        case CARTRIDGE_GENERIC_16KB:
            generic_16kb_detach();
            break;
        case CARTRIDGE_GENERIC_8KB:
            generic_8kb_detach();
            break;
        case CARTRIDGE_GS:
            gs_detach();
            break;
        case CARTRIDGE_IDE64:
            ide64_detach();
            break;
        case CARTRIDGE_KCS_POWER:
            kcs_detach();
            break;
        case CARTRIDGE_MACH5:
            mach5_detach();
            break;
        case CARTRIDGE_MAGIC_DESK:
            magicdesk_detach();
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_detach();
            break;
        case CARTRIDGE_MIKRO_ASSEMBLER:
            mikroass_detach();
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_detach();
            break;
        case CARTRIDGE_OCEAN:
            ocean_detach();
            break;
        case CARTRIDGE_RETRO_REPLAY:
            retroreplay_detach();
            break;
        case CARTRIDGE_REX:
            rex_detach();
            break;
        case CARTRIDGE_REX_EP256:
            rexep256_detach();
            break;
        case CARTRIDGE_ROSS:
            ross_detach();
            break;
        case CARTRIDGE_SIMONS_BASIC:
            simon_detach();
            break;
        case CARTRIDGE_SNAPSHOT64:
            snapshot64_detach();
            break;
        case CARTRIDGE_STARDOS:
            stardos_detach();
            break;
        case CARTRIDGE_STRUCTURED_BASIC:
            stb_detach();
            break;
        case CARTRIDGE_SUPER_EXPLODE_V5:
            se5_detach();
            break;
        case CARTRIDGE_SUPER_GAMES:
            supergames_detach();
            break;
        case CARTRIDGE_SUPER_SNAPSHOT:
            supersnapshot_v4_detach();
            break;
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            supersnapshot_v5_detach();
            break;
        case CARTRIDGE_ULTIMAX:
            generic_ultimax_detach();
            break;
        case CARTRIDGE_WARPSPEED:
            warpspeed_detach();
            break;
        case CARTRIDGE_WESTERMANN:
            westermann_detach();
            break;
        case CARTRIDGE_ZAXXON:
            zaxxon_detach();
            break;
        default:
            DBG(("CART: no detach hook ID: %d\n", type));
            break;
    }
}

/* called once by cartridge_init at machine init */
void cart_init(void)
{
    /* "Slot 0" */
    mmc64_init(); /* Initialize the MMC64.  */
    magicvoice_init();
    tpi_init();

    /* "Slot 1" */
    ramcart_init(); /* Initialize the RAMCART.  */
    /* "IO Slot" */
    aciacart_init();
    georam_init(); /* Initialize the GEORAM.  */
#ifdef HAVE_MIDI
    midi_init();
#endif
    reu_init(); /* Initialize the REU.  */
#ifdef HAVE_TFE
    tfe_init(); /* Initialize the TFE.  */
#endif
}

/* called once by cartridge_init at machine shutdown */
void cartridge_shutdown(void)
{
    /* "Slot 0" */
    tpi_shutdown();
    magicvoice_shutdown();
}

/*
    called from c64mem.c:mem_initialize_memory (calls XYZ_config_init)
*/
void cartridge_init_config(void)
{
    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_STARDOS:
            stardos_config_init();
            break;
        case CARTRIDGE_ACTION_REPLAY:
            actionreplay_config_init();
            break;
        case CARTRIDGE_ACTION_REPLAY2:
            actionreplay2_config_init();
            break;
        case CARTRIDGE_ACTION_REPLAY3:
            actionreplay3_config_init();
            break;
        case CARTRIDGE_ACTION_REPLAY4:
            actionreplay4_config_init();
            break;
        case CARTRIDGE_ATOMIC_POWER:
            atomicpower_config_init();
            break;
        case CARTRIDGE_RETRO_REPLAY:
            retroreplay_config_init();
            break;
        case CARTRIDGE_MACH5:
            mach5_config_init();
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_config_init();
            break;
        case CARTRIDGE_IDE64:
            ide64_config_init();
            break;
        case CARTRIDGE_KCS_POWER:
            kcs_config_init();
            break;
        case CARTRIDGE_SUPER_GAMES:
            supergames_config_init();
            break;
        case CARTRIDGE_COMAL80:
            comal80_config_init();
            break;
        case CARTRIDGE_GENERIC_8KB:
        case CARTRIDGE_REX:
            generic_8kb_config_init();
            break;
        case CARTRIDGE_EPYX_FASTLOAD:
            epyxfastload_config_init();
            break;
        case CARTRIDGE_MIKRO_ASSEMBLER:
            mikroass_config_init();
            break;
        case CARTRIDGE_FINAL_I:
            final_v1_config_init();
            break;
        case CARTRIDGE_FINAL_PLUS:
            final_plus_config_init();
            break;
        case CARTRIDGE_FINAL_III:
            final_v3_config_init();
            break;
        case CARTRIDGE_SIMONS_BASIC:
        case CARTRIDGE_GENERIC_16KB:
        case CARTRIDGE_WESTERMANN:
        case CARTRIDGE_WARPSPEED:
            generic_16kb_config_init();
            break;
        case CARTRIDGE_ZAXXON:
            zaxxon_config_init();
            break;
        case CARTRIDGE_ULTIMAX:
            generic_ultimax_config_init();
            break;
        case CARTRIDGE_SNAPSHOT64:
            snapshot64_config_init();
            break;
        case CARTRIDGE_SUPER_EXPLODE_V5:
            se5_config_init();
            break;
        case CARTRIDGE_SUPER_SNAPSHOT:
            supersnapshot_v4_config_init();
            break;
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            supersnapshot_v5_config_init();
            break;
        case CARTRIDGE_OCEAN:
            ocean_config_init();
            break;
        case CARTRIDGE_FUNPLAY:
            funplay_config_init();
            break;
        case CARTRIDGE_EASYFLASH:
            easyflash_config_init();
            break;
        case CARTRIDGE_GS:
            gs_config_init();
            break;
        case CARTRIDGE_DINAMIC:
            dinamic_config_init();
            break;
        case CARTRIDGE_MAGIC_DESK:
            magicdesk_config_init();
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_config_init();
            break;
        case CARTRIDGE_CAPTURE:
            capture_config_init();
            break;
        case CARTRIDGE_ROSS:
            ross_config_init();
            break;
        case CARTRIDGE_STRUCTURED_BASIC:
            stb_config_init();
            break;
        case CARTRIDGE_DELA_EP64:
            delaep64_config_init();
            break;
        case CARTRIDGE_DELA_EP7x8:
            delaep7x8_config_init();
            break;
        case CARTRIDGE_DELA_EP256:
            delaep256_config_init();
            break;
        case CARTRIDGE_REX_EP256:
            rexep256_config_init();
            break;
        case CARTRIDGE_P64:
            p64_config_init();
            break;
        case CARTRIDGE_GAME_KILLER:
            gamekiller_config_init();
            break;
        case CARTRIDGE_EXOS:
            exos_config_init();
            break;
        case CARTRIDGE_FREEZE_FRAME:
            freezeframe_config_init();
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            freezemachine_config_init();
            break;
        /* HACK: add all missing ones instead of the default */
        case CARTRIDGE_NONE:
            break;
        default:
            DBG(("CART: no init hook ID: %d\n", mem_cartridge_type));
            cartridge_config_changed(CMODE_RAM, CMODE_RAM, CMODE_READ);
            break;
    }

    /* "Slot 1" */
    if (ramcart_cart_enabled()) {
        ramcart_init_config();
    }
    if (dqbb_cart_enabled()) {
        dqbb_init_config();
    }
    if (expert_cart_enabled()) {
        expert_config_init();
    }

    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        magicvoice_config_init();
    }
    if (mmc64_cart_enabled()) {
        mmc64_init_card_config();
    }
    if (tpi_cart_enabled()) {
        tpi_config_init();
    }

}

/*
    called by c64.c:machine_specific_reset (calls XYZ_reset)

    the reset signal goes to all active carts. we call the hooks
    in "back to front" order, so carts closer to the "front" will
    win with whatever they do.
*/
void cartridge_reset(void)
{
    cart_unset_alarms();

    /* "IO Slot" */
    if (aciacart_cart_enabled()) {
        aciacart_reset();
    }
    if (georam_cart_enabled()) {
        georam_reset();
    }
#ifdef HAVE_MIDI
    midi_reset();
#endif
    if (reu_cart_enabled()) {
        reu_reset();
    }
    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_ACTION_REPLAY:
            actionreplay_reset();
            break;
        case CARTRIDGE_ACTION_REPLAY2:
            actionreplay2_reset();
            break;
        case CARTRIDGE_ACTION_REPLAY3:
            actionreplay3_reset();
            break;
        case CARTRIDGE_ACTION_REPLAY4:
            actionreplay4_reset();
            break;
        case CARTRIDGE_ATOMIC_POWER:
            atomicpower_reset();
            break;
        case CARTRIDGE_CAPTURE:
            capture_reset();
            break;
        case CARTRIDGE_EPYX_FASTLOAD:
            epyxfastload_reset();
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            freezemachine_reset();
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_reset();
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_reset();
            break;
        case CARTRIDGE_RETRO_REPLAY:
            retroreplay_reset();
            break;
    }
    /* "Slot 1" */
    if (dqbb_cart_enabled()) {
        dqbb_reset();
    }
    if (expert_cart_enabled()) {
        expert_reset();
    }
    if (ramcart_cart_enabled()) {
        ramcart_reset();
    }
    /* "Slot 0" */
    if (tpi_cart_enabled()) {
        tpi_reset();
    }
    if (magicvoice_cart_enabled()) {
        magicvoice_reset();
    }
    if (mmc64_cart_enabled()) {
        mmc64_reset();
    }
}

/* ------------------------------------------------------------------------- */

/* called by cart_nmi_alarm_triggered, after an alarm occured */
void cart_freeze(int type)
{
    DBG(("CART: freeze\n"));
    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_MAGIC_VOICE:
            /* cartridge_release_freeze(); */
            break;
        /* "Slot 1" */
        case CARTRIDGE_EXPERT:
            expert_freeze();
            break;
        case CARTRIDGE_ISEPIC:
            /* FIXME: do nothing ? */
            break;
        /* "IO Slot" */
        /* "Main Slot" */
        case CARTRIDGE_SNAPSHOT64:
            snapshot64_freeze();
            break;
        case CARTRIDGE_SUPER_SNAPSHOT:
            supersnapshot_v4_freeze();
            break;
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            supersnapshot_v5_freeze();
            break;
        case CARTRIDGE_ACTION_REPLAY4:
            actionreplay4_freeze();
            break;
        case CARTRIDGE_ACTION_REPLAY3:
            actionreplay3_freeze();
            break;
        case CARTRIDGE_ACTION_REPLAY2:
            actionreplay2_freeze();
            break;
        case CARTRIDGE_ACTION_REPLAY:
            actionreplay_freeze();
            break;
        case CARTRIDGE_ATOMIC_POWER:
            atomicpower_freeze();
            break;
        case CARTRIDGE_RETRO_REPLAY:
            retroreplay_freeze();
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_freeze();
            break;
        case CARTRIDGE_KCS_POWER:
            kcs_freeze();
            break;
        case CARTRIDGE_FINAL_I:
            final_v1_freeze();
            break;
        case CARTRIDGE_FINAL_PLUS:
            final_plus_freeze();
            break;
        case CARTRIDGE_FINAL_III:
            final_v3_freeze();
            break;
        case CARTRIDGE_CAPTURE:
            capture_freeze();
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_freeze();
            break;
        case CARTRIDGE_GAME_KILLER:
            gamekiller_freeze();
            break;
        case CARTRIDGE_FREEZE_FRAME:
            freezeframe_freeze();
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            freezemachine_freeze();
            break;
    }
}

/* called by cart_nmi_alarm_triggered */
void cart_nmi_alarm(CLOCK offset, void *data)
{
    /* "Slot 0" */
    /* "Slot 1" */
    if (expert_freeze_allowed()) {
        cart_freeze(CARTRIDGE_EXPERT);
    }
    if (isepic_freeze_allowed()) {
        cart_freeze(CARTRIDGE_ISEPIC);
    }
    /* "Main Slot" */
    cart_freeze(cart_getid_slotmain());
}

/* called by the UI when the freeze button is pressed */
int cart_freeze_allowed(void)
{
    int maintype = cart_getid_slotmain();
    /* "Slot 0" */
    /* "Slot 1" */
    if (expert_freeze_allowed()) {
        return 1;
    }
    if (isepic_freeze_allowed()) {
        return 1;
    }

    /* "Main Slot" */
    switch (maintype) {
        case CARTRIDGE_ACTION_REPLAY4:
        case CARTRIDGE_ACTION_REPLAY3:
        case CARTRIDGE_ACTION_REPLAY2:
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
            return 1;
        case CARTRIDGE_RETRO_REPLAY:
            if (retroreplay_freeze_allowed()) {
                return 1;
            }
            break;
        case CARTRIDGE_MMC_REPLAY:
            if (mmcreplay_freeze_allowed()) {
                return 1;
            }
            break;
    }
    return 0;
}


/* ------------------------------------------------------------------------- */

/*
    FIXME: missing here (at least)
    - rr
    - easyflash
    - mmc64
    - mmcr
    - dqbb
    - ramcart
    - reu
    - georam
*/
int cartridge_bin_save(int type, const char *filename)
{
    switch (type) {
        case CARTRIDGE_EXPERT:
            return expert_bin_save(filename);
        case CARTRIDGE_ISEPIC:
            return isepic_bin_save(filename);
    }
    return -1;
}

int cartridge_crt_save(int type, const char *filename)
{
    switch (type) {
        case CARTRIDGE_EXPERT:
            return expert_crt_save(filename);
        case CARTRIDGE_ISEPIC:
            return isepic_crt_save(filename);
    }
    return -1;
}

/* ------------------------------------------------------------------------- */

/*
    Snapshot reading and writing
*/
int cartridge_snapshot_write_modules(struct snapshot_s *s)
{
    /* "I/O Slot" */
    if (reu_cart_enabled()) {
        if (reu_write_snapshot_module(s) < 0) {
            return -1;
        }
    }
    if (georam_cart_enabled()) {
        if (georam_write_snapshot_module(s) < 0) {
            return -1;
        }
    }
#ifdef HAVE_RS232
    if (aciacart_cart_enabled()) {
        if(aciacart_snapshot_write_module(s) < 0) {
            return -1;
        }
    }
#endif
    if (tpi_cart_enabled()) {
        if(tpi_snapshot_write_module(s) < 0) {
            return -1;
        }
    }
    return 0;
}

int cartridge_snapshot_read_modules(struct snapshot_s *s)
{
    /* "I/O Slot" */
    if (reu_read_snapshot_module(s) < 0) {
        /* REU disabled  */
    }
    if (georam_read_snapshot_module(s) < 0) {
        /* georam disabled  */
    }
#ifdef HAVE_RS232
    if (aciacart_snapshot_read_module(s) < 0) {
        /* acia disabled */
    }
#endif
    if (tpi_snapshot_read_module(s) < 0) {
        /* IEEE488 module disabled  */
    }
    return 0;
}

