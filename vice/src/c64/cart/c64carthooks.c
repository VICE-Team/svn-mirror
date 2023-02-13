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
#include "c64.h"
#include "c64cart.h"
#define CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64cartsystem.h"
#undef CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64mem.h"
#include "cartio.h"
#include "cartridge.h"
#include "cmdline.h"
#include "cpmcart.h"
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
#include "snapshot.h"
#include "types.h"
#include "util.h"

#define CARTRIDGE_INCLUDE_PRIVATE_API
#include "actionreplay2.h"
#include "actionreplay3.h"
#include "actionreplay4.h"
#include "actionreplay.h"
#include "atomicpower.h"
#include "bisplus.h"
#include "blackbox3.h"
#include "blackbox4.h"
#include "blackbox8.h"
#include "blackbox9.h"
#include "c64acia.h"
#include "c64-generic.h"
#include "c64-midi.h"
#include "c64tpi.h"
#include "comal80.h"
#include "capture.h"
#include "debugcart.h"
#include "delaep256.h"
#include "delaep64.h"
#include "delaep7x8.h"
#include "diashowmaker.h"
#include "digimax.h"
#include "dinamic.h"
#include "dqbb.h"
#include "ds12c887rtc.h"
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
#include "georam.h"
#include "gs.h"
#include "gmod2.h"
#include "gmod3.h"
#include "drean.h"
#include "ide64.h"
#include "ieeeflash64.h"
#include "isepic.h"
#include "kcs.h"
#include "kingsoft.h"
#include "ltkernal.h"
#include "mach5.h"
#include "machine.h"
#include "magicdesk.h"
#include "magicformel.h"
#include "magicvoice.h"
#include "maxbasic.h"
#include "mikroass.h"
#include "mmc64.h"
#include "mmcreplay.h"
#include "multimax.h"
#include "sfx_soundexpander.h"
#include "sfx_soundsampler.h"
#include "ocean.h"
#include "pagefox.h"
#include "partner64.h"
#include "prophet64.h"
#include "ramcart.h"
#include "ramlink.h"
#include "retroreplay.h"
#include "reu.h"
#include "rexep256.h"
#include "rexramfloppy.h"
#include "rexutility.h"
#include "rgcd.h"
#include "rrnetmk3.h"
#include "ross.h"
#include "sdbox.h"
#include "shortbus_digimax.h"
#include "silverrock128.h"
#include "simonsbasic.h"
#include "snapshot64.h"
#include "stardos.h"
#include "stb.h"
#include "supergames.h"
#include "superexplode5.h"
#include "supersnapshot.h"
#include "supersnapshot4.h"
#include "turtlegraphics.h"
#ifdef HAVE_RAWNET
#include "ethernetcart.h"
#endif
#include "warpspeed.h"
#include "westermann.h"
#include "zaxxon.h"
#include "zippcode48.h"
#undef CARTRIDGE_INCLUDE_PRIVATE_API

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

/* from c64cartmem.c */
extern export_t export_slot1;
extern export_t export_slotmain;
extern export_t export_passthrough; /* slot1 and main combined, goes into slot0 passthrough or c64 export */

/*
    TODO: keep in sync with cartridge.h (currently highest: CARTRIDGE_DIASHOW_MAKER)
    TODO: keep -cartXYZ options in sync with cartconv -t option

    the following carts, which do not have any rom or ram, are NOT in the list below,
    for obvious reasons:

        CARTRIDGE_DIGIMAX
        CARTRIDGE_DS12C887RTC
        CARTRIDGE_SFX_SOUND_EXPANDER
        CARTRIDGE_SFX_SOUND_SAMPLER
        CARTRIDGE_MIDI_PASSPORT
        CARTRIDGE_MIDI_DATEL
        CARTRIDGE_MIDI_SEQUENTIAL
        CARTRIDGE_MIDI_NAMESOFT
        CARTRIDGE_MIDI_MAPLIN
        CARTRIDGE_TFE
        CARTRIDGE_TURBO232
        CARTRIDGE_CPM

    all other carts should get a commandline option here like this:

    -cartXYZ <name>     attach a ram/rom image for cartridgeXYZ
*/
static const cmdline_option_t cmdline_options[] =
{
    /* generic cartridges */
    { "-cart8", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_GENERIC_8KB, NULL, NULL,
      "<Name>", "Attach raw 8KiB cartridge image" },
    { "-cart16", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_GENERIC_16KB, NULL, NULL,
      "<Name>", "Attach raw 16KiB cartridge image" },
    { "-cartultimax", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_ULTIMAX, NULL, NULL,
      "<Name>", "Attach generic 16KiB Ultimax cartridge image" },
    /* smart-insert CRT */
    { "-cartcrt", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_CRT, NULL, NULL,
      "<Name>", "Attach CRT cartridge image" },
    /* binary images: */
    { "-cartap", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_ATOMIC_POWER, NULL, NULL,
      "<Name>", "Attach raw 32KiB Atomic Power cartridge image" },
    { "-cartar2", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_ACTION_REPLAY2, NULL, NULL,
      "<Name>", "Attach raw 16KiB Action Replay MK2 cartridge image" },
    { "-cartar3", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_ACTION_REPLAY3, NULL, NULL,
      "<Name>", "Attach raw 16KiB Action Replay MK3 cartridge image" },
    { "-cartar4", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_ACTION_REPLAY4, NULL, NULL,
      "<Name>", "Attach raw 32KiB Action Replay MK4 cartridge image" },
    { "-cartar5", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_ACTION_REPLAY, NULL, NULL,
      "<Name>", "Attach raw 32KiB Action Replay cartridge image" },
    { "-cartbb3", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_BLACKBOX3, NULL, NULL,
      "<Name>", "Attach raw 8KiB " CARTRIDGE_NAME_BLACKBOX3 " cartridge image" },
    { "-cartbb4", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_BLACKBOX4, NULL, NULL,
      "<Name>", "Attach raw 16KiB " CARTRIDGE_NAME_BLACKBOX4 " cartridge image" },
    { "-cartbb8", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_BLACKBOX8, NULL, NULL,
      "<Name>", "Attach raw 32/64KiB " CARTRIDGE_NAME_BLACKBOX8 " cartridge image" },
    { "-cartbb9", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_BLACKBOX9, NULL, NULL,
      "<Name>", "Attach raw 32KiB " CARTRIDGE_NAME_BLACKBOX9 " cartridge image" },
    { "-cartbis", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_BISPLUS, NULL, NULL,
      "<Name>", "Attach raw 2/4/8KiB " CARTRIDGE_NAME_BISPLUS " cartridge image" },
    { "-cartcap", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_CAPTURE, NULL, NULL,
      "<Name>", "Attach raw 8KiB Capture cartridge image" },
    { "-cartcomal", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_COMAL80, NULL, NULL,
      "<Name>", "Attach raw 64KiB Comal 80 cartridge image" },
    { "-cartdep256", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_DELA_EP256, NULL, NULL,
      "<Name>", "Attach raw Dela EP256 cartridge image" },
    { "-cartdep64", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_DELA_EP64, NULL, NULL,
      "<Name>", "Attach raw Dela EP64 cartridge image" },
    { "-cartdep7x8", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_DELA_EP7x8, NULL, NULL,
      "<Name>", "Attach raw Dela EP7x8 cartridge image" },
    { "-cartdin", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_DINAMIC, NULL, NULL,
      "<Name>", "Attach raw 128KiB Dinamic cartridge image" },
    { "-cartdsm", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_DIASHOW_MAKER, NULL, NULL,
      "<Name>", "Attach raw 8KiB Diashow-Maker cartridge image" },
    { "-cartdqbb", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_DQBB, NULL, NULL,
      "<Name>", "Attach raw 16KiB Double Quick Brown Box cartridge image" },
    { "-carteasy", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_EASYFLASH, NULL, NULL,
      "<Name>", "Attach raw EasyFlash cartridge image" },
    { "-carteasycalc", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_EASYCALC, NULL, NULL,
      "<Name>", "Attach raw 24KiB Easy Calc Result cartridge image" },
    /* omitted: CARTRIDGE_EASYFLASH_XBANK (NO CART EXISTS!) */
    { "-cartepyx", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_EPYX_FASTLOAD, NULL, NULL,
      "<Name>", "Attach raw 8KiB Epyx FastLoad cartridge image" },
    { "-cartexos", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_EXOS, NULL, NULL,
      "<Name>", "Attach raw 8KiB EXOS cartridge image" },
    { "-cartexpert", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_EXPERT, NULL, NULL,
      "<Name>", "Attach raw 8KiB Expert Cartridge image" },
    { "-cartf64", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_FORMEL64, NULL, NULL,
      "<Name>", "Attach raw 32KiB Formel 64 image" },
    { "-cartfc1", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_FINAL_I, NULL, NULL,
      "<Name>", "Attach raw 16KiB Final Cartridge image" },
    { "-cartfc3", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_FINAL_III, NULL, NULL,
      "<Name>", "Attach raw 64KiB Final Cartridge III image" },
    { "-cartfcplus", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_FINAL_PLUS, NULL, NULL,
      "<Name>", "Attach raw 32KiB Final Cartridge Plus image" },
    { "-cartff", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_FREEZE_FRAME, NULL, NULL,
      "<Name>", "Attach raw 8KiB Freeze Frame image" },
    { "-cartff2", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_FREEZE_FRAME_MK2, NULL, NULL,
      "<Name>", "Attach raw 16KiB Freeze Frame MK2/MK3 image" },
    { "-cartfm", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_FREEZE_MACHINE, NULL, NULL,
      "<Name>", "Attach raw 32KiB Freeze Machine image" },
    { "-cartfp", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_FUNPLAY, NULL, NULL,
      "<Name>", "Attach raw 128KiB Fun Play/Power Play cartridge image" },
    { "-cartgmod2", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_GMOD2, NULL, NULL,
      "<Name>", "Attach raw GMod2 cartridge image" },
    { "-cartgmod3", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_GMOD3, NULL, NULL,
      "<Name>", "Attach raw GMod2 cartridge image" },
    { "-cartgk", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_GAME_KILLER, NULL, NULL,
      "<Name>", "Attach raw 8KiB Game Killer cartridge image" },
    { "-cartgeoram", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_GEORAM, NULL, NULL,
      "<Name>", "Attach raw GEO-RAM cartridge image" },
    { "-cartgs", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_GS, NULL, NULL,
      "<Name>", "Attach raw 512KiB Game System cartridge image" },
    { "-cartdrean", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_DREAN, NULL, NULL,
      "<Name>", "Attach raw 32KiB Hero cartridge image" },
    { "-cartide64", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_IDE64, NULL, NULL,
      "<Name>", "Attach raw 64KiB IDE64 cartridge image" },
    { "-cartieee", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_IEEE488, NULL, NULL,
      "<Name>", "Attach IEEE-488 Interface cartridge image" },
    { "-cartieeeflash64", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_IEEEFLASH64, NULL, NULL,
      "<Name>", "Attach raw 8KiB IEEE Flash! 64 cartridge image" },
    { "-cartisepic", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_ISEPIC, NULL, NULL,
      "<Name>", "Attach raw 2KiB ISEPIC cartridge image" },
    { "-cartkcs", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_KCS_POWER, NULL, NULL,
      "<Name>", "Attach raw 16KiB KCS Power cartridge image" },
    { "-cartks", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_KINGSOFT, NULL, NULL,
      "<Name>", "Attach raw 24KiB Kingsoft cartridge image" },
    { "-cartltk", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_LT_KERNAL, NULL, NULL,
      "<Name>", "Attach raw 8kB Lt. Kernal boot image" },
    { "-cartmach5", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_MACH5, NULL, NULL,
      "<Name>", "Attach raw 8KiB MACH 5 cartridge image" },
    { "-cartmd", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_MAGIC_DESK, NULL, NULL,
      "<Name>", "Attach raw 32/64/128KiB Magic Desk cartridge image" },
    { "-cartmd16", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_MAGIC_DESK_16, NULL, NULL,
      "<Name>", "Attach raw up to 2048KiB Magic Desk 16K cartridge image" },
    { "-cartmf", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_MAGIC_FORMEL, NULL, NULL,
      "<Name>", "Attach raw Magic Formel cartridge image" },
    { "-cartmax", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_MAX_BASIC, NULL, NULL,
      "<Name>", "Attach raw MAX Basic cartridge image" },
    { "-cartmikro", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_MIKRO_ASSEMBLER, NULL, NULL,
      "<Name>", "Attach raw 8KiB Mikro Assembler cartridge image" },
    { "-cartmmc64", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_MMC64, NULL, NULL,
      "<Name>", "Attach raw 8KiB MMC64 cartridge image" },
    { "-cartmmcr", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_MMC_REPLAY, NULL, NULL,
      "<Name>", "Attach raw 512KiB MMC Replay cartridge image" },
    { "-cartmv", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_MAGIC_VOICE, NULL, NULL,
      "<Name>", "Attach raw 16KiB Magic Voice cartridge image" },
    { "-cartmm", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_MULTIMAX, NULL, NULL,
      "<Name>", "Attach raw 1MiB " CARTRIDGE_NAME_MULTIMAX " cartridge image" },
    { "-cartocean", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_OCEAN, NULL, NULL,
      "<Name>", "Attach raw Ocean cartridge image" },
    { "-cartpf", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_PAGEFOX, NULL, NULL,
      "<Name>", "Attach raw 64KiB Pagefox cartridge image" },
    { "-cartp64", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_P64, NULL, NULL,
      "<Name>", "Attach raw 256KiB Prophet 64 cartridge image" },
    { "-cartpartner64", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_PARTNER64, NULL, NULL,
      "<Name>", "Attach raw 16KiB " CARTRIDGE_NAME_PARTNER64 " cartridge image" },
    { "-cartramcart", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_RAMCART, NULL, NULL,
      "<Name>", "Attach raw RamCart cartridge image" },
    { "-cartramlink", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_RAMLINK, NULL, NULL,
      "<Name>", "Attach raw 64KiB RAMLink ROM image" },
    { "-cartreu", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_REU, NULL, NULL,
      "<Name>", "Attach raw REU cartridge image" },
    { "-cartrep256", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_REX_EP256, NULL, NULL,
      "<Name>", "Attach raw REX EP256 cartridge image" },
    { "-cartrrf", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_REX_RAMFLOPPY, NULL, NULL,
      "<Name>", "Attach raw " CARTRIDGE_NAME_REX_RAMFLOPPY " cartridge image" },
    { "-cartrgcd", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_RGCD, NULL, NULL,
      "<Name>", "Attach raw 64KiB RGCD cartridge image" },
#ifdef HAVE_RAWNET
    { "-cartrrnet", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_RRNETMK3, NULL, NULL,
      "<Name>", "Attach raw 8KiB RR-Net MK3 cartridge image" },
#endif
    { "-cartross", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_ROSS, NULL, NULL,
      "<Name>", "Attach raw 16/32KiB ROSS cartridge image" },
    { "-cartrr", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_RETRO_REPLAY, NULL, NULL,
      "<Name>", "Attach raw 64KiB Retro Replay cartridge image" },
    { "-cartru", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_REX, NULL, NULL,
      "<Name>", "Attach raw 8KiB REX Utility cartridge image" },
    { "-cartsdbox", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_SDBOX, NULL, NULL,
      "<Name>", "Attach raw 128KiB " CARTRIDGE_NAME_SDBOX " cartridge image" },
    { "-carts64", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_SNAPSHOT64, NULL, NULL,
      "<Name>", "Attach raw 4KiB Snapshot 64 cartridge image" },
    { "-cartsb", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_STRUCTURED_BASIC, NULL, NULL,
      "<Name>", "Attach raw Structured Basic cartridge image" },
    { "-cartse5", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_SUPER_EXPLODE_V5, NULL, NULL,
      "<Name>", "Attach raw 16KiB Super Explode V5 cartridge image" },
    { "-cartsg", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_SUPER_GAMES, NULL, NULL,
      "<Name>", "Attach raw 64KiB Super Games cartridge image" },
    { "-cartsilver", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_SILVERROCK_128, NULL, NULL,
      "<Name>", "Attach raw Silverrock 128 cartridge image" },
    { "-cartsimon", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_SIMONS_BASIC, NULL, NULL,
      "<Name>", "Attach raw 16KiB Simons Basic cartridge image" },
    { "-cartss4", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_SUPER_SNAPSHOT, NULL, NULL,
      "<Name>", "Attach raw 32KiB Super Snapshot V4 cartridge image" },
    { "-cartss5", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_SUPER_SNAPSHOT_V5, NULL, NULL,
      "<Name>", "Attach raw 64KiB or 128KiB Super Snapshot V5 cartridge image" },
    { "-cartstar", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_STARDOS, NULL, NULL,
      "<Name>", "Attach raw 16KiB Stardos cartridge image" },
    { "-cartturtle", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_TURTLE_GRAPHICS_II, NULL, NULL,
      "<Name>", "Attach raw 16KiB " CARTRIDGE_NAME_TURTLE_GRAPHICS_II " cartridge image" },
    { "-cartwl", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_WESTERMANN, NULL, NULL,
      "<Name>", "Attach raw 16KiB Westermann Learning cartridge image" },
    { "-cartws", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_WARPSPEED, NULL, NULL,
      "<Name>", "Attach raw 16KiB " CARTRIDGE_NAME_WARPSPEED " cartridge image" },
    { "-cartzaxxon", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_ZAXXON, NULL, NULL,
      "<Name>", "Attach raw 16KiB Zaxxon cartridge image" },
    { "-cartzipp", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      cart_attach_cmdline, (void *)CARTRIDGE_ZIPPCODE48, NULL, NULL,
      "<Name>", "Attach raw 8KiB " CARTRIDGE_NAME_ZIPPCODE48 " cartridge image" },
    CMDLINE_LIST_END
};

int cart_cmdline_options_init(void)
{
    /* "Slot 0" */
    if (mmc64_cmdline_options_init() < 0
        || magicvoice_cmdline_options_init() < 0
        || tpi_cmdline_options_init() < 0
        || ramlink_cmdline_options_init() < 0
        /* "Slot 1" */
        || dqbb_cmdline_options_init() < 0
        || expert_cmdline_options_init() < 0
        || isepic_cmdline_options_init() < 0
        || ramcart_cmdline_options_init() < 0
        /* "I/O Slot" */
#ifdef HAVE_MIDI
        || c64_midi_cmdline_options_init() < 0
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
        || aciacart_cmdline_options_init() < 0
#endif
        || digimax_cmdline_options_init() < 0
        || ds12c887rtc_cmdline_options_init() < 0
        || georam_cmdline_options_init() < 0
        || reu_cmdline_options_init() < 0
        || sfx_soundexpander_cmdline_options_init() < 0
        || sfx_soundsampler_cmdline_options_init() < 0
#ifdef HAVE_RAWNET
        || ethernetcart_cmdline_options_init() < 0
#endif
        /* "Main Slot" */
        || easyflash_cmdline_options_init() < 0
        || gmod2_cmdline_options_init() < 0
        || gmod3_cmdline_options_init() < 0
        || ide64_cmdline_options_init() < 0
        || ieeeflash64_cmdline_options_init() < 0
        || ltkernal_cmdline_options_init() < 0
        || mmcreplay_cmdline_options_init() < 0
        || retroreplay_cmdline_options_init() < 0
        || rexramfloppy_cmdline_options_init() < 0
        || rgcd_cmdline_options_init() < 0
#ifdef HAVE_RAWNET
        || rrnetmk3_cmdline_options_init() < 0
#endif
        || supersnapshot_v5_cmdline_options_init() < 0
        ) {
        return -1;
    }

    if (debugcart_cmdline_options_init() < 0) {
        return -1;
    }

    if (machine_class == VICE_MACHINE_C64 || machine_class == VICE_MACHINE_C64SC) {
        if (cpmcart_cmdline_options_init() < 0) {
            return -1;
        }
    }

    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */
/*
    resource init and de-init
    - every cart that has an _init hook should also have a _shutdown hook!
*/
int cart_resources_init(void)
{
    /* "Slot 0" */
    if (mmc64_resources_init() < 0
        || magicvoice_resources_init() < 0
        || tpi_resources_init() < 0
        || ramlink_resources_init() < 0
        || ieeeflash64_resources_init() < 0
        /* "Slot 1" */
        || expert_resources_init() < 0
        || dqbb_resources_init() < 0
        || isepic_resources_init() < 0
        || ramcart_resources_init() < 0
        /* "IO Slot" */
        || digimax_resources_init() < 0
        || ds12c887rtc_resources_init() < 0
        || georam_resources_init() < 0
#ifdef HAVE_MIDI
        || c64_midi_resources_init() < 0
#endif
        || reu_resources_init() < 0
        || sfx_soundexpander_resources_init() < 0
        || sfx_soundsampler_resources_init() < 0
#ifdef HAVE_RAWNET
        || ethernetcart_resources_init() < 0
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
        || aciacart_resources_init() < 0
#endif
        /* "Main Slot" */
        || easyflash_resources_init() < 0
        || gmod2_resources_init() < 0
        || gmod3_resources_init() < 0
        || ide64_resources_init() < 0
        || ltkernal_resources_init() < 0
        || mmcreplay_resources_init() < 0
        || retroreplay_resources_init() < 0
        || rexramfloppy_resources_init() < 0
        || rgcd_resources_init() < 0
#ifdef HAVE_RAWNET
        || rrnetmk3_resources_init() < 0
#endif
        || supersnapshot_v5_resources_init() < 0
        ) {
        return -1;
    }

    if (debugcart_resources_init() < 0) {
        return -1;
    }

    if (machine_class == VICE_MACHINE_C64 || machine_class == VICE_MACHINE_C64SC) {
        if (cpmcart_resources_init() < 0) {
            return -1;
        }
    }

    return 0;
}

void cart_resources_shutdown(void)
{
    /* "IO Slot" */
    digimax_resources_shutdown();
    ds12c887rtc_resources_shutdown();
    georam_resources_shutdown();
#ifdef HAVE_MIDI
    midi_resources_shutdown();
#endif
    reu_resources_shutdown();
    sfx_soundexpander_resources_shutdown();
    sfx_soundsampler_resources_shutdown();
#ifdef HAVE_RAWNET
    ethernetcart_resources_shutdown();
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
    aciacart_resources_shutdown();
#endif

    /* "Main Slot" */
    easyflash_resources_shutdown();
    gmod2_resources_shutdown();
    gmod3_resources_shutdown();
    ide64_resources_shutdown();
    ltkernal_resources_shutdown();
    mmcreplay_resources_shutdown();
    retroreplay_resources_shutdown();
    rexramfloppy_resources_shutdown();
    rgcd_resources_shutdown();
#ifdef HAVE_RAWNET
    rrnetmk3_resources_shutdown();
#endif
    supersnapshot_v5_resources_shutdown();

    /* "Slot 1" */
    expert_resources_shutdown();
    dqbb_resources_shutdown();
    ramcart_resources_shutdown();
    isepic_resources_shutdown();

    /* "Slot 0" */
    mmc64_resources_shutdown();
    magicvoice_resources_shutdown();
    tpi_resources_shutdown();
    ramlink_resources_shutdown();
    ieeeflash64_resources_shutdown();
}

/* ------------------------------------------------------------------------- */

/*
    returns 1 if given cart type is in "Main Slot"
*/
int cart_is_slotmain(int type)
{
    switch (type) {
        /* slot 0 */
        case CARTRIDGE_IEEE488:
        case CARTRIDGE_IEEEFLASH64:
        case CARTRIDGE_MAGIC_VOICE:
        case CARTRIDGE_MMC64:
        case CARTRIDGE_RAMLINK:
        /* slot 1 */
        case CARTRIDGE_DQBB:
        case CARTRIDGE_EXPERT:
        case CARTRIDGE_ISEPIC:
        case CARTRIDGE_RAMCART:
        /* io slot */
        case CARTRIDGE_DIGIMAX:
        case CARTRIDGE_DS12C887RTC:
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

int cart_getid_slot0(void)
{
    if (mmc64_cart_enabled()) {
        return CARTRIDGE_MMC64;
    }
    if (magicvoice_cart_enabled()) {
        return CARTRIDGE_MAGIC_VOICE;
    }
    if (tpi_cart_enabled()) {
        return CARTRIDGE_IEEE488;
    }
    if (ieeeflash64_cart_enabled()) {
        return CARTRIDGE_IEEEFLASH64;
    }
    if (ramlink_cart_enabled()) {
        return CARTRIDGE_RAMLINK;
    }
    return CARTRIDGE_NONE;
}

int cart_getid_slot1(void)
{
    if (isepic_cart_active()) {
        return CARTRIDGE_ISEPIC;
    }
    if (expert_cart_enabled()) {
        return CARTRIDGE_EXPERT;
    }
    if (ramcart_cart_enabled()) {
        return CARTRIDGE_RAMCART;
    }
    if (dqbb_cart_enabled()) {
        return CARTRIDGE_DQBB;
    }
    return CARTRIDGE_NONE;
}

/* ------------------------------------------------------------------------- */

/*
    returns 1 if the cartridge of the given type is enabled

    -  used by c64iec.c:iec_available_busses (checks for CARTRIDGE_IEEE488)
*/
int cart_type_enabled(int type)
{
    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            return tpi_cart_enabled();
        case CARTRIDGE_RAMLINK:
            return ramlink_cart_enabled();
        case CARTRIDGE_IEEEFLASH64:
            return ieeeflash64_cart_enabled();
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
        case CARTRIDGE_DIGIMAX:
            return digimax_cart_enabled();
        case CARTRIDGE_DS12C887RTC:
            return ds12c887rtc_cart_enabled();
        case CARTRIDGE_GEORAM:
            return georam_cart_enabled();
#ifdef HAVE_MIDI
        case CARTRIDGE_MIDI_PASSPORT:
            return c64_midi_pp_cart_enabled();
        case CARTRIDGE_MIDI_DATEL:
            return c64_midi_datel_cart_enabled();
        case CARTRIDGE_MIDI_SEQUENTIAL:
            return c64_midi_seq_cart_enabled();
        case CARTRIDGE_MIDI_NAMESOFT:
            return c64_midi_nsoft_cart_enabled();
        case CARTRIDGE_MIDI_MAPLIN:
            return c64_midi_maplin_cart_enabled();
#endif
        case CARTRIDGE_REU:
            return reu_cart_enabled();
        case CARTRIDGE_SFX_SOUND_EXPANDER:
            return sfx_soundexpander_cart_enabled();
        case CARTRIDGE_SFX_SOUND_SAMPLER:
            return sfx_soundsampler_cart_enabled();
#ifdef HAVE_RAWNET
        case CARTRIDGE_TFE:
            return ethernetcart_cart_enabled();
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
        case CARTRIDGE_TURBO232:
            return aciacart_cart_enabled();
#endif
            /* Main Slot handled in c64cart.c:cartridge_type_enabled */
    }
    return 0;
}

/*
    can the "main slot" cart handle get filename
*/
int cart_can_get_file_name(int type)
{
    switch (type) {
        /* "Main Slot" */
        case CARTRIDGE_RAMLINK:
            return 1;
    }
    return 0;
}

/*
    get filename of cart with given type
*/
const char *cart_get_file_name(int type)
{
    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            return tpi_get_file_name();
        case CARTRIDGE_RAMLINK:
            return ramlink_get_file_name();
        case CARTRIDGE_IEEEFLASH64:
            return ieeeflash64_get_file_name();
        case CARTRIDGE_MAGIC_VOICE:
            return magicvoice_get_file_name();
        case CARTRIDGE_MMC64:
            return mmc64_get_file_name();
        /* "Slot 1" */
        case CARTRIDGE_DQBB:
            return dqbb_get_file_name();
        case CARTRIDGE_EXPERT:
            return expert_get_file_name();
        case CARTRIDGE_ISEPIC:
            return isepic_get_file_name();
        case CARTRIDGE_RAMCART:
            return ramcart_get_file_name();
        /* "Main Slot" */
        /* "I/O Slot" */
        case CARTRIDGE_GEORAM:
            return georam_get_file_name();
        case CARTRIDGE_REU:
            return reu_get_file_name();
        /* the following have no associated file */
        case CARTRIDGE_DIGIMAX:
        case CARTRIDGE_DS12C887RTC:
#ifdef HAVE_MIDI
        case CARTRIDGE_MIDI_PASSPORT:
        case CARTRIDGE_MIDI_DATEL:
        case CARTRIDGE_MIDI_SEQUENTIAL:
        case CARTRIDGE_MIDI_NAMESOFT:
        case CARTRIDGE_MIDI_MAPLIN:
#endif
        case CARTRIDGE_SFX_SOUND_EXPANDER:
        case CARTRIDGE_SFX_SOUND_SAMPLER:
#ifdef HAVE_RAWNET
        case CARTRIDGE_TFE:
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
        case CARTRIDGE_TURBO232:
#endif
            break;

            /* Main Slot handled in c64cart.c:cartridge_get_file_name */
    }
    return ""; /* FIXME: NULL or empty string? */
}

/* ------------------------------------------------------------------------- */

/* FIXME: shutdown missing */

/* called once by machine_setup_context */
void cartridge_setup_context(machine_context_t *context)
{
    /* "Slot 0" */
    tpi_setup_context(context);
    magicvoice_setup_context(context);
    /* mmc64 */

    /* "Slot 1" */
    /* "Main Slot" */
    /* "I/O Slot" */
}

/* ------------------------------------------------------------------------- */

/* attach a binary image. note that for carts not in the main slot, the image
   name is usually kept in a resource, and the cartridge is enabled via another
   resource - the function called from here must also do this */
int cart_bin_attach(int type, const char *filename, uint8_t *rawcart)
{
    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            return tpi_bin_attach(filename, rawcart);
        case CARTRIDGE_RAMLINK:
            return ramlink_bin_attach(filename, rawcart);
        case CARTRIDGE_IEEEFLASH64:
            return ieeeflash64_bin_attach(filename, rawcart);
        case CARTRIDGE_MAGIC_VOICE:
            return magicvoice_bin_attach(filename, rawcart);
        case CARTRIDGE_MMC64:
            return mmc64_bin_attach(filename, rawcart);
        /* "Slot 1" */
        case CARTRIDGE_DQBB:
            return dqbb_bin_attach(filename, rawcart);
        case CARTRIDGE_EXPERT:
            return expert_bin_attach(filename, rawcart);
        case CARTRIDGE_ISEPIC:
            return isepic_bin_attach(filename, rawcart);
        case CARTRIDGE_RAMCART:
            return ramcart_bin_attach(filename, rawcart);
        /* "I/O Slot" */
        case CARTRIDGE_GEORAM:
            return georam_bin_attach(filename, rawcart);
        case CARTRIDGE_REU:
            return reu_bin_attach(filename, rawcart);
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
        case CARTRIDGE_BISPLUS:
            return bisplus_bin_attach(filename, rawcart);
        case CARTRIDGE_BLACKBOX3:
            return blackbox3_bin_attach(filename, rawcart);
        case CARTRIDGE_BLACKBOX4:
            return blackbox4_bin_attach(filename, rawcart);
        case CARTRIDGE_BLACKBOX8:
            return blackbox8_bin_attach(filename, rawcart);
        case CARTRIDGE_BLACKBOX9:
            return blackbox9_bin_attach(filename, rawcart);
        case CARTRIDGE_CAPTURE:
            return capture_bin_attach(filename, rawcart);
        case CARTRIDGE_COMAL80:
            return comal80_bin_attach(filename, rawcart);
        case CARTRIDGE_DELA_EP64:
            return delaep64_bin_attach(filename, rawcart);
        case CARTRIDGE_DELA_EP7x8:
            return delaep7x8_bin_attach(filename, rawcart);
        case CARTRIDGE_DELA_EP256:
            return delaep256_bin_attach(filename, rawcart);
        case CARTRIDGE_DIASHOW_MAKER:
            return dsm_bin_attach(filename, rawcart);
        case CARTRIDGE_DINAMIC:
            return dinamic_bin_attach(filename, rawcart);
        case CARTRIDGE_EASYCALC:
            return easycalc_bin_attach(filename, rawcart);
        case CARTRIDGE_EASYFLASH:
            return easyflash_bin_attach(filename, rawcart);
        case CARTRIDGE_EASYFLASH_XBANK:
            return -1; /* NO CART EXISTS */
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
        case CARTRIDGE_FORMEL64:
            return formel64_bin_attach(filename, rawcart);
        case CARTRIDGE_FREEZE_FRAME:
            return freezeframe_bin_attach(filename, rawcart);
        case CARTRIDGE_FREEZE_FRAME_MK2:
            return freezeframe2_bin_attach(filename, rawcart);
        case CARTRIDGE_FREEZE_MACHINE:
            return freezemachine_bin_attach(filename, rawcart);
        case CARTRIDGE_FUNPLAY:
            return funplay_bin_attach(filename, rawcart);
        case CARTRIDGE_GAME_KILLER:
            return gamekiller_bin_attach(filename, rawcart);
        case CARTRIDGE_GENERIC_8KB:
            return generic_8kb_bin_attach(filename, rawcart);
        case CARTRIDGE_GENERIC_16KB:
            return generic_16kb_bin_attach(filename, rawcart);
        case CARTRIDGE_GMOD2:
            return gmod2_bin_attach(filename, rawcart);
        case CARTRIDGE_GMOD3:
            return gmod3_bin_attach(filename, rawcart);
        case CARTRIDGE_GS:
            return gs_bin_attach(filename, rawcart);
        case CARTRIDGE_DREAN:
            return drean_bin_attach(filename, rawcart);
        case CARTRIDGE_IDE64:
            return ide64_bin_attach(filename, rawcart);
        case CARTRIDGE_KCS_POWER:
            return kcs_bin_attach(filename, rawcart);
        case CARTRIDGE_KINGSOFT:
            return kingsoft_bin_attach(filename, rawcart);
        case CARTRIDGE_LT_KERNAL:
            return ltkernal_bin_attach(filename, rawcart);
        case CARTRIDGE_MACH5:
            return mach5_bin_attach(filename, rawcart);
        case CARTRIDGE_MAGIC_DESK:
            return magicdesk_bin_attach(filename, rawcart);
        case CARTRIDGE_MAGIC_DESK_16:
            return magicdesk16_bin_attach(filename, rawcart);
        case CARTRIDGE_MAGIC_FORMEL:
            return magicformel_bin_attach(filename, rawcart);
        case CARTRIDGE_MAX_BASIC:
            return maxbasic_bin_attach(filename, rawcart);
        case CARTRIDGE_MIKRO_ASSEMBLER:
            return mikroass_bin_attach(filename, rawcart);
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_bin_attach(filename, rawcart);
        case CARTRIDGE_MULTIMAX:
            return multimax_bin_attach(filename, rawcart);
        case CARTRIDGE_OCEAN:
            return ocean_bin_attach(filename, rawcart);
        case CARTRIDGE_P64:
            return p64_bin_attach(filename, rawcart);
        case CARTRIDGE_PAGEFOX:
            return pagefox_bin_attach(filename, rawcart);
        case CARTRIDGE_PARTNER64:
            return partner64_bin_attach(filename, rawcart);
        case CARTRIDGE_RETRO_REPLAY:
            return retroreplay_bin_attach(filename, rawcart);
        case CARTRIDGE_REX:
            return rex_bin_attach(filename, rawcart);
        case CARTRIDGE_REX_EP256:
            return rexep256_bin_attach(filename, rawcart);
        case CARTRIDGE_REX_RAMFLOPPY:
            return rexramfloppy_bin_attach(filename, rawcart);
        case CARTRIDGE_RGCD:
            return rgcd_bin_attach(filename, rawcart);
#ifdef HAVE_RAWNET
        case CARTRIDGE_RRNETMK3:
            return rrnetmk3_bin_attach(filename, rawcart);
#endif
        case CARTRIDGE_ROSS:
            return ross_bin_attach(filename, rawcart);
        case CARTRIDGE_SDBOX:
            return sdbox_bin_attach(filename, rawcart);
        case CARTRIDGE_SILVERROCK_128:
            return silverrock128_bin_attach(filename, rawcart);
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
        case CARTRIDGE_SUPER_GAMES:
            return supergames_bin_attach(filename, rawcart);
        case CARTRIDGE_SUPER_SNAPSHOT:
            return supersnapshot_v4_bin_attach(filename, rawcart);
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            return supersnapshot_v5_bin_attach(filename, rawcart);
        case CARTRIDGE_TURTLE_GRAPHICS_II:
            return turtlegraphics_bin_attach(filename, rawcart);
        case CARTRIDGE_ULTIMAX:
            return generic_ultimax_bin_attach(filename, rawcart);
        case CARTRIDGE_WARPSPEED:
            return warpspeed_bin_attach(filename, rawcart);
        case CARTRIDGE_WESTERMANN:
            return westermann_bin_attach(filename, rawcart);
        case CARTRIDGE_ZAXXON:
            return zaxxon_bin_attach(filename, rawcart);
        case CARTRIDGE_ZIPPCODE48:
            return zippcode48_bin_attach(filename, rawcart);
    }
    return -1;
}

/*
    called by cartridge_attach_image after cart_crt/bin_attach
    XYZ_config_setup should copy the raw cart image into the
    individual implementations array.
*/
void cart_attach(int type, uint8_t *rawcart)
{
    cart_detach_conflicting(type);

    if ((machine_class == VICE_MACHINE_C128) && CARTRIDGE_C128_ISID(type)) {
        c128cartridge->config_setup(type, rawcart);
        return;
    }

    /* FIXME: cartridges that work in both c64 and c128 mode must explicitly
              call c128cartridge->config_setup() below */

    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            tpi_config_setup(rawcart);
            break;
        case CARTRIDGE_RAMLINK:
            ramlink_config_setup(rawcart);
            break;
        case CARTRIDGE_IEEEFLASH64:
            ieeeflash64_config_setup(rawcart);
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
        case CARTRIDGE_ISEPIC:
            isepic_config_setup(rawcart);
            break;
        case CARTRIDGE_DQBB:
            dqbb_config_setup(rawcart);
            break;
        case CARTRIDGE_RAMCART:
            ramcart_config_setup(rawcart);
            break;
        /* "IO Slot" */
        case CARTRIDGE_GEORAM:
            georam_config_setup(rawcart);
            break;
        case CARTRIDGE_REU:
            reu_config_setup(rawcart);
            break;
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
        case CARTRIDGE_BISPLUS:
            bisplus_config_setup(rawcart);
            break;
        case CARTRIDGE_BLACKBOX3:
            blackbox3_config_setup(rawcart);
            break;
        case CARTRIDGE_BLACKBOX4:
            blackbox4_config_setup(rawcart);
            break;
        case CARTRIDGE_BLACKBOX8:
            blackbox8_config_setup(rawcart);
            break;
        case CARTRIDGE_BLACKBOX9:
            blackbox9_config_setup(rawcart);
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
        case CARTRIDGE_DIASHOW_MAKER:
            dsm_config_setup(rawcart);
            break;
        case CARTRIDGE_DINAMIC:
            dinamic_config_setup(rawcart);
            break;
        case CARTRIDGE_EASYCALC:
            easycalc_config_setup(rawcart);
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
        case CARTRIDGE_FORMEL64:
            formel64_config_setup(rawcart);
            break;
        case CARTRIDGE_FREEZE_FRAME:
            freezeframe_config_setup(rawcart);
            break;
        case CARTRIDGE_FREEZE_FRAME_MK2:
            freezeframe2_config_setup(rawcart);
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
        case CARTRIDGE_GMOD2:
            gmod2_config_setup(rawcart);
            break;
        case CARTRIDGE_GMOD3:
            gmod3_config_setup(rawcart);
            break;
        case CARTRIDGE_GS:
            gs_config_setup(rawcart);
            break;
        case CARTRIDGE_DREAN:
            drean_config_setup(rawcart);
            break;
        case CARTRIDGE_IDE64:
            ide64_config_setup(rawcart);
            break;
        case CARTRIDGE_KCS_POWER:
            kcs_config_setup(rawcart);
            break;
        case CARTRIDGE_KINGSOFT:
            kingsoft_config_setup(rawcart);
            break;
        case CARTRIDGE_LT_KERNAL:
            ltkernal_config_setup(rawcart);
            break;
        case CARTRIDGE_MACH5:
            mach5_config_setup(rawcart);
            break;
        case CARTRIDGE_MAGIC_DESK:
            magicdesk_config_setup(rawcart);
            break;
        case CARTRIDGE_MAGIC_DESK_16:
            magicdesk16_config_setup(rawcart);
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_config_setup(rawcart);
            break;
        case CARTRIDGE_MAX_BASIC:
            maxbasic_config_setup(rawcart);
            break;
        case CARTRIDGE_MIKRO_ASSEMBLER:
            mikroass_config_setup(rawcart);
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_config_setup(rawcart);
            break;
        case CARTRIDGE_MULTIMAX:
            multimax_config_setup(rawcart);
            break;
        case CARTRIDGE_OCEAN:
            ocean_config_setup(rawcart);
            break;
        case CARTRIDGE_P64:
            p64_config_setup(rawcart);
            break;
        case CARTRIDGE_PAGEFOX:
            pagefox_config_setup(rawcart);
            break;
        case CARTRIDGE_PARTNER64:
            partner64_config_setup(rawcart);
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
        case CARTRIDGE_REX_RAMFLOPPY:
            rexramfloppy_config_setup(rawcart);
            break;
        case CARTRIDGE_RGCD:
            rgcd_config_setup(rawcart);
            break;
        case CARTRIDGE_ROSS:
            ross_config_setup(rawcart);
            break;
#ifdef HAVE_RAWNET
        case CARTRIDGE_RRNETMK3:
            rrnetmk3_config_setup(rawcart);
            break;
#endif
        case CARTRIDGE_SDBOX:
            sdbox_config_setup(rawcart);
            break;
        case CARTRIDGE_SILVERROCK_128:
            silverrock128_config_setup(rawcart);
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
        case CARTRIDGE_TURTLE_GRAPHICS_II:
            turtlegraphics_config_setup(rawcart);
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
        case CARTRIDGE_ZIPPCODE48:
            zippcode48_config_setup(rawcart);
            break;
        default:
            DBG(("CART: no attach hook %d\n", type));
            break;
    }
}

/* only one of the "Slot 0" carts can be enabled at a time */
static int slot0conflicts[] =
{
    CARTRIDGE_IEEE488,
    CARTRIDGE_IEEEFLASH64,
    CARTRIDGE_MAGIC_VOICE,
    CARTRIDGE_MMC64,
    0
};

/* only one of the "Slot 1" carts can be enabled at a time */
static int slot1conflicts[] =
{
    CARTRIDGE_EXPERT,
    CARTRIDGE_ISEPIC,
    CARTRIDGE_DQBB,
    CARTRIDGE_RAMCART,
    0
};

static void cart_detach_conflicts0(int *list, int type)
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
    cart_detach_conflicts0(slot1conflicts, type);
}

/*
    attach a cartridge without setting an image name
*/
int cartridge_enable(int type)
{
    DBG(("CART: enable type: %d\n", type));
    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            tpi_enable();
            break;
        case CARTRIDGE_IEEEFLASH64:
            ieeeflash64_enable();
            break;
        case CARTRIDGE_MAGIC_VOICE:
            magicvoice_enable();
            break;
        case CARTRIDGE_MMC64:
            mmc64_enable();
            break;
        /* "Slot 1" */
        case CARTRIDGE_DQBB:
            dqbb_enable();
            break;
        case CARTRIDGE_EXPERT:
            expert_enable();
            break;
        case CARTRIDGE_ISEPIC:
            isepic_enable();
            break;
        case CARTRIDGE_RAMCART:
            ramcart_enable();
            break;
        case CARTRIDGE_RAMLINK:
            ramlink_enable();
            break;
        /* "I/O Slot" */
        case CARTRIDGE_DIGIMAX:
            digimax_enable();
            break;
        case CARTRIDGE_DS12C887RTC:
            ds12c887rtc_enable();
            break;
        case CARTRIDGE_GEORAM:
            georam_enable();
            break;
#ifdef HAVE_MIDI
        case CARTRIDGE_MIDI_PASSPORT:
        case CARTRIDGE_MIDI_DATEL:
        case CARTRIDGE_MIDI_SEQUENTIAL:
        case CARTRIDGE_MIDI_NAMESOFT:
        case CARTRIDGE_MIDI_MAPLIN:
            c64_midi_enable();
            break;
#endif
        case CARTRIDGE_REU:
            reu_enable();
            break;
        case CARTRIDGE_SFX_SOUND_EXPANDER:
            sfx_soundexpander_enable();
            break;
        case CARTRIDGE_SFX_SOUND_SAMPLER:
            sfx_soundsampler_enable();
            break;
#ifdef HAVE_RAWNET
        case CARTRIDGE_TFE:
            ethernetcart_enable();
            break;
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
        case CARTRIDGE_TURBO232:
            aciacart_enable();
            break;
#endif
        /* "Main Slot" */
        default:
            DBG(("CART: no enable hook %d\n", type));
            break;
    }
    cart_detach_conflicting(type);

    if (cart_type_enabled(type)) {
        return 0;
    }
    log_error(LOG_ERR, "Failed to enable cartridge with ID %d.\n", type);
    return -1;
}


/** \brief  Disable cartridge by \a type
 *
 * \return  0 on success, -1 on failure
 *
 * \todo    More or less copy cartridge_enable() while replacing
 *          ${cart}_enable() with ${cart_disable(). The various disable
 *          functions still need to be written at the moment.
 */
int cartridge_disable(int type)
{
    /*
    fprintf(stderr, "%s:%d: %s() isn't implemented yet, continuing\n",
            __FILE__, __LINE__, __func__);
    */
    DBG(("CART: enable type: %d\n", type));
    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            tpi_disable();
            break;
        case CARTRIDGE_IEEEFLASH64:
            ieeeflash64_disable();
            break;
        case CARTRIDGE_MAGIC_VOICE:
            magicvoice_disable();
            break;
        case CARTRIDGE_MMC64:
            mmc64_disable();
            break;
        /* "Slot 1" */
        case CARTRIDGE_DQBB:
            dqbb_disable();
            break;
        case CARTRIDGE_EXPERT:
            expert_disable();
            break;
        case CARTRIDGE_ISEPIC:
            isepic_disable();
            break;
        case CARTRIDGE_RAMCART:
            ramcart_disable();
            break;
        case CARTRIDGE_RAMLINK:
            ramlink_disable();
            break;
        /* "I/O Slot" */
        case CARTRIDGE_DIGIMAX:
            digimax_disable();
            break;
        case CARTRIDGE_DS12C887RTC:
            ds12c887rtc_disable();
            break;
        case CARTRIDGE_GEORAM:
            georam_disable();
            break;
#ifdef HAVE_MIDI
        case CARTRIDGE_MIDI_PASSPORT:
        case CARTRIDGE_MIDI_DATEL:
        case CARTRIDGE_MIDI_SEQUENTIAL:
        case CARTRIDGE_MIDI_NAMESOFT:
        case CARTRIDGE_MIDI_MAPLIN:
            c64_midi_disable();
            break;
#endif
        case CARTRIDGE_REU:
            reu_disable();
            break;
        case CARTRIDGE_SFX_SOUND_EXPANDER:
            sfx_soundexpander_disable();
            break;
        case CARTRIDGE_SFX_SOUND_SAMPLER:
            sfx_soundsampler_disable();
            break;
#ifdef HAVE_RAWNET
        case CARTRIDGE_TFE:
            ethernetcart_disable();
            break;
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
        case CARTRIDGE_TURBO232:
            aciacart_disable();
            break;
#endif
        /* "Main Slot" */
        default:
            DBG(("CART: no disable hook %d\n", type));
            break;
    }
#if 0
    cart_detach_conflicting(type);
#endif
    /* make sure the cart has been disabled */
    if (!cart_type_enabled(type)) {
        return 0;
    }
    log_error(LOG_ERR, "Failed to disable cartridge with ID %d.\n", type);
    return -1;
}


/*
    detach all cartridges

    - carts not in "Main Slot" must make sure their _detach hook does not
      fail when it is called and the cart is not actually attached.
*/
void cart_detach_all(void)
{
    DBG(("CART: detach all\n"));
    debugcart_detach();
    /* "slot 0" */
    tpi_detach();
    magicvoice_detach();
    mmc64_detach();
    ieeeflash64_detach();
    ramlink_detach();
    /* "Slot 1" */
    dqbb_detach();
    expert_detach();
    isepic_detach();
    ramcart_detach();
    /* "io Slot" */
    digimax_detach();
    ds12c887rtc_detach();
    georam_detach();
#ifdef HAVE_MIDI
    c64_midi_detach();
#endif
    reu_detach();
    sfx_soundexpander_detach();
    sfx_soundsampler_detach();
#ifdef HAVE_RAWNET
    ethernetcart_detach();
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
    aciacart_detach();
#endif
    /* "Main Slot" */
    cart_detach_slotmain();
}

/*
    detach a cartridge

    - carts that are not "main" cartridges can be disabled individually

    - carts not in "Main Slot" must make sure their _detach hook does not
      fail when it is called and the cart is not actually attached.
*/
void cart_detach(int type)
{
    DBG(("CART: cart_detach ID: %d\n", type));

    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_IEEE488:
            tpi_detach();
            break;
        case CARTRIDGE_RAMLINK:
            ramlink_detach();
            break;
        case CARTRIDGE_IEEEFLASH64:
            ieeeflash64_detach();
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
        case CARTRIDGE_DIGIMAX:
            digimax_detach();
            break;
        case CARTRIDGE_DS12C887RTC:
            ds12c887rtc_detach();
            break;
        case CARTRIDGE_GEORAM:
            georam_detach();
            break;
#ifdef HAVE_MIDI
        case CARTRIDGE_MIDI_PASSPORT:
        case CARTRIDGE_MIDI_DATEL:
        case CARTRIDGE_MIDI_SEQUENTIAL:
        case CARTRIDGE_MIDI_NAMESOFT:
        case CARTRIDGE_MIDI_MAPLIN:
            c64_midi_detach();
            break;
#endif
        case CARTRIDGE_REU:
            reu_detach();
            break;
        case CARTRIDGE_SFX_SOUND_EXPANDER:
            sfx_soundexpander_detach();
            break;
        case CARTRIDGE_SFX_SOUND_SAMPLER:
            sfx_soundsampler_detach();
            break;
#ifdef HAVE_RAWNET
        case CARTRIDGE_TFE:
            ethernetcart_detach();
            break;
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
        case CARTRIDGE_TURBO232:
            aciacart_detach();
            break;
#endif
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
        case CARTRIDGE_BISPLUS:
            bisplus_detach();
            break;
        case CARTRIDGE_BLACKBOX3:
            blackbox3_detach();
            break;
        case CARTRIDGE_BLACKBOX4:
            blackbox4_detach();
            break;
        case CARTRIDGE_BLACKBOX8:
            blackbox8_detach();
            break;
        case CARTRIDGE_BLACKBOX9:
            blackbox9_detach();
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
        case CARTRIDGE_DIASHOW_MAKER:
            dsm_detach();
            break;
        case CARTRIDGE_DINAMIC:
            dinamic_detach();
            break;
        case CARTRIDGE_EASYCALC:
            easycalc_detach();
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
        case CARTRIDGE_FORMEL64:
            formel64_detach();
            break;
        case CARTRIDGE_FREEZE_FRAME:
            freezeframe_detach();
            break;
        case CARTRIDGE_FREEZE_FRAME_MK2:
            freezeframe2_detach();
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            freezemachine_detach();
            break;
        case CARTRIDGE_FUNPLAY:
            funplay_detach();
            break;
        case CARTRIDGE_GAME_KILLER:
            gamekiller_detach();
            break;
        case CARTRIDGE_GENERIC_16KB:
            generic_16kb_detach();
            break;
        case CARTRIDGE_GENERIC_8KB:
            generic_8kb_detach();
            break;
        case CARTRIDGE_GMOD2:
            gmod2_detach();
            break;
        case CARTRIDGE_GMOD3:
            gmod3_detach();
            break;
        case CARTRIDGE_GS:
            gs_detach();
            break;
        case CARTRIDGE_DREAN:
            drean_detach();
            break;
        case CARTRIDGE_IDE64:
            ide64_detach();
            break;
        case CARTRIDGE_KCS_POWER:
            kcs_detach();
            break;
        case CARTRIDGE_KINGSOFT:
            kingsoft_detach();
            break;
        case CARTRIDGE_LT_KERNAL:
            ltkernal_detach();
            break;
        case CARTRIDGE_MACH5:
            mach5_detach();
            break;
        case CARTRIDGE_MAGIC_DESK:
            magicdesk_detach();
            break;
        case CARTRIDGE_MAGIC_DESK_16:
            magicdesk16_detach();
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_detach();
            break;
        case CARTRIDGE_MAX_BASIC:
            maxbasic_detach();
            break;
        case CARTRIDGE_MIKRO_ASSEMBLER:
            mikroass_detach();
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_detach();
            break;
        case CARTRIDGE_MULTIMAX:
            multimax_detach();
            break;
        case CARTRIDGE_OCEAN:
            ocean_detach();
            break;
        case CARTRIDGE_P64:
            p64_detach();
            break;
        case CARTRIDGE_PAGEFOX:
            pagefox_detach();
            break;
        case CARTRIDGE_PARTNER64:
            partner64_detach();
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
        case CARTRIDGE_REX_RAMFLOPPY:
            rexramfloppy_detach();
            break;
        case CARTRIDGE_RGCD:
            rgcd_detach();
            break;
#ifdef HAVE_RAWNET
        case CARTRIDGE_RRNETMK3:
            rrnetmk3_detach();
            break;
#endif
        case CARTRIDGE_ROSS:
            ross_detach();
            break;
        case CARTRIDGE_SDBOX:
            sdbox_detach();
            break;
        case CARTRIDGE_SILVERROCK_128:
            silverrock128_detach();
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
        case CARTRIDGE_TURTLE_GRAPHICS_II:
            turtlegraphics_detach();
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
        case CARTRIDGE_ZIPPCODE48:
            zippcode48_detach();
            break;
        default:
            DBG(("CART: no detach hook ID: %d\n", type));
            break;
    }
}

/* called once by cartridge_init at machine init */
void cart_init(void)
{
    DBG(("CART: cart_init\n"));

    /* "Slot 0" */
    mmc64_init();
    magicvoice_init();
    tpi_init();
    /* ieeeflash64_init(); */

    /* "Slot 1" */
    ramcart_init();
    /* isepic_init(); */
    /* expert_init(); */
    /* dqbb_init(); */

    /* "Main Slot" */

    /* "IO Slot" */
    /* digimax */
    /* ds12c887rtc */
    georam_init();
#ifdef HAVE_MIDI
    midi_init();
#endif
    reu_init();
    /* sfx sound expander */
    /* sfx sound sampler */
#ifdef HAVE_RAWNET
    ethernetcart_init();
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
    aciacart_init();
#endif
}

/* called once by c64.c:machine_specific_shutdown at machine shutdown */
void cartridge_shutdown(void)
{
    /* "Slot 0" */
    tpi_shutdown();
    magicvoice_shutdown();
    /* mmc64_shutdown(); */
    /* ieeeflash64_shutdown(); */

    /* "Main Slot" */
    /* "Slot 1" */
    /* "IO Slot" */
}

/*
    called from c64mem.c:mem_initialize_memory (calls XYZ_config_init)
*/
void cartridge_init_config(void)
{
    if ((machine_class == VICE_MACHINE_C128) && CARTRIDGE_C128_ISID(mem_cartridge_type)) {
        c128cartridge->config_init(mem_cartridge_type);
    } else {
        /* "Main Slot" */
        switch (mem_cartridge_type) {
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
            case CARTRIDGE_BISPLUS:
                bisplus_config_init();
                break;
            case CARTRIDGE_BLACKBOX3:
                blackbox3_config_init();
                break;
            case CARTRIDGE_BLACKBOX4:
                blackbox4_config_init();
                break;
            case CARTRIDGE_BLACKBOX8:
                blackbox8_config_init();
                break;
            case CARTRIDGE_BLACKBOX9:
                blackbox9_config_init();
                break;
            case CARTRIDGE_CAPTURE:
                capture_config_init();
                break;
            case CARTRIDGE_COMAL80:
                comal80_config_init();
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
            case CARTRIDGE_DIASHOW_MAKER:
                dsm_config_init();
                break;
            case CARTRIDGE_DINAMIC:
                dinamic_config_init();
                break;
            case CARTRIDGE_EASYCALC:
                easycalc_config_init();
                break;
            case CARTRIDGE_EASYFLASH:
                easyflash_config_init();
                break;
            case CARTRIDGE_EPYX_FASTLOAD:
                epyxfastload_config_init();
                break;
            case CARTRIDGE_EXOS:
                exos_config_init();
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
            case CARTRIDGE_FORMEL64:
                formel64_config_init();
                break;
            case CARTRIDGE_FREEZE_FRAME:
                freezeframe_config_init();
                break;
            case CARTRIDGE_FREEZE_FRAME_MK2:
                freezeframe2_config_init();
                break;
            case CARTRIDGE_FREEZE_MACHINE:
                freezemachine_config_init();
                break;
            case CARTRIDGE_FUNPLAY:
                funplay_config_init();
                break;
            case CARTRIDGE_GAME_KILLER:
                gamekiller_config_init();
                break;
            case CARTRIDGE_GENERIC_8KB:
                generic_8kb_config_init();
                break;
            case CARTRIDGE_GENERIC_16KB:
                generic_16kb_config_init();
                break;
            case CARTRIDGE_GMOD2:
                gmod2_config_init();
                break;
            case CARTRIDGE_GMOD3:
                gmod3_config_init();
                break;
            case CARTRIDGE_GS:
                gs_config_init();
                break;
            case CARTRIDGE_DREAN:
                drean_config_init();
                break;
            case CARTRIDGE_IDE64:
                ide64_config_init();
                break;
            case CARTRIDGE_KCS_POWER:
                kcs_config_init();
                break;
            case CARTRIDGE_KINGSOFT:
                kingsoft_config_init();
                break;
            case CARTRIDGE_LT_KERNAL:
                ltkernal_config_init();
                break;
            case CARTRIDGE_MACH5:
                mach5_config_init();
                break;
            case CARTRIDGE_MAGIC_DESK:
                magicdesk_config_init();
                break;
            case CARTRIDGE_MAGIC_DESK_16:
                magicdesk16_config_init();
                break;
            case CARTRIDGE_MAGIC_FORMEL:
                magicformel_config_init();
                break;
            case CARTRIDGE_MAX_BASIC:
                maxbasic_config_init();
                break;
            case CARTRIDGE_MIKRO_ASSEMBLER:
                mikroass_config_init();
                break;
            case CARTRIDGE_MMC_REPLAY:
                mmcreplay_config_init();
                break;
            case CARTRIDGE_MULTIMAX:
                multimax_config_init();
                break;
            case CARTRIDGE_OCEAN:
                ocean_config_init();
                break;
            case CARTRIDGE_P64:
                p64_config_init();
                break;
            case CARTRIDGE_PAGEFOX:
                pagefox_config_init();
                break;
            case CARTRIDGE_PARTNER64:
                partner64_config_init();
                break;
            case CARTRIDGE_RETRO_REPLAY:
                retroreplay_config_init();
                break;
            case CARTRIDGE_REX:
                rex_config_init();
                break;
            case CARTRIDGE_REX_EP256:
                rexep256_config_init();
                break;
            case CARTRIDGE_REX_RAMFLOPPY:
                rexramfloppy_config_init();
                break;
            case CARTRIDGE_RGCD:
                rgcd_config_init();
                break;
#ifdef HAVE_RAWNET
            case CARTRIDGE_RRNETMK3:
                rrnetmk3_config_init();
                break;
#endif
            case CARTRIDGE_ROSS:
                ross_config_init();
                break;
            case CARTRIDGE_SDBOX:
                sdbox_config_init();
                break;
            case CARTRIDGE_SILVERROCK_128:
                silverrock128_config_init();
                break;
            case CARTRIDGE_SIMONS_BASIC:
                simon_config_init();
                break;
            case CARTRIDGE_SNAPSHOT64:
                snapshot64_config_init();
                break;
            case CARTRIDGE_STARDOS:
                stardos_config_init();
                break;
            case CARTRIDGE_STRUCTURED_BASIC:
                stb_config_init();
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
            case CARTRIDGE_SUPER_GAMES:
                supergames_config_init();
                break;
            case CARTRIDGE_TURTLE_GRAPHICS_II:
                turtlegraphics_config_init();
                break;
            case CARTRIDGE_ULTIMAX:
                generic_ultimax_config_init();
                break;
            case CARTRIDGE_WARPSPEED:
                warpspeed_config_init();
                break;
            case CARTRIDGE_WESTERMANN:
                westermann_config_init();
                break;
            case CARTRIDGE_ZAXXON:
                zaxxon_config_init();
                break;
            case CARTRIDGE_ZIPPCODE48:
                zippcode48_config_init();
                break;
            /* FIXME: add all missing ones instead of using the default */
            case CARTRIDGE_NONE:
                break;
            default:
                DBG(("CART: no init hook ID: %d\n", mem_cartridge_type));
                cart_config_changed_slotmain(CMODE_RAM, CMODE_RAM, CMODE_READ);
                break;
        }
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
    if (isepic_cart_enabled()) {
        isepic_config_init();
    }

    cart_passthrough_changed();

    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        magicvoice_config_init(&export_passthrough);
    } else if (mmc64_cart_enabled()) {
        mmc64_config_init(&export_passthrough);
    } else if (tpi_cart_enabled()) {
        tpi_config_init(&export_passthrough);
    } else if (ramlink_cart_enabled()) {
        ramlink_config_init(&export_passthrough);
    } else if (ieeeflash64_cart_enabled()) {
        ieeeflash64_config_init(&export_passthrough);
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

    cart_reset_memptr();

    /* "IO Slot" */
    if (digimax_cart_enabled()) {
        digimax_reset();
    }
    if (ds12c887rtc_cart_enabled()) {
        ds12c887rtc_reset();
    }
    if (georam_cart_enabled()) {
        georam_reset();
    }
#ifdef HAVE_MIDI
    if (c64_midi_cart_enabled()) {
        midi_reset();
    }
#endif
    if (reu_cart_enabled()) {
        reu_reset();
    }
    if (sfx_soundexpander_cart_enabled()) {
        sfx_soundexpander_reset();
    }
    if (sfx_soundsampler_cart_enabled()) {
        sfx_soundsampler_reset();
    }
#ifdef HAVE_RAWNET
    if (ethernetcart_cart_enabled()) {
        ethernetcart_reset();
    }
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
    if (aciacart_cart_enabled()) {
        aciacart_reset();
    }
#endif
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
        case CARTRIDGE_FORMEL64:
            formel64_reset();
            break;
        case CARTRIDGE_FREEZE_FRAME_MK2:
            freezeframe2_reset();
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            freezemachine_reset();
            break;
        case CARTRIDGE_GMOD2:
            gmod2_reset();
            break;
        case CARTRIDGE_GMOD3:
            gmod3_reset();
            break;
        case CARTRIDGE_IDE64:
            ide64_reset();
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_reset();
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_reset();
            break;
        case CARTRIDGE_PARTNER64:
            partner64_reset();
            break;
        case CARTRIDGE_REX_RAMFLOPPY:
            rexramfloppy_reset();
            break;
#ifdef HAVE_RAWNET
        case CARTRIDGE_RRNETMK3:
            rrnetmk3_reset();
            break;
#endif
        case CARTRIDGE_RGCD:
            rgcd_reset();
            break;
        case CARTRIDGE_RETRO_REPLAY:
            retroreplay_reset();
            break;
        case CARTRIDGE_SUPER_EXPLODE_V5:
            se5_reset();
            break;
        case CARTRIDGE_WARPSPEED:
            zippcode48_reset();
            break;
        case CARTRIDGE_ZIPPCODE48:
            warpspeed_reset();
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
    if (isepic_cart_enabled()) {
        isepic_reset();
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
    if (ieeeflash64_cart_enabled()) {
        ieeeflash64_reset();
    }
    if (cpmcart_cart_enabled()) {
        cpmcart_reset();
    }

    if (machine_class == VICE_MACHINE_C128) {
        c128cartridge->reset();
    }
}

/*
    called by c64.c:machine_specific_powerup (calls XYZ_powerup)

    we call the hooks in "back to front" order, so carts closer
    to the "front" will win with whatever they do.
*/
void cartridge_powerup(void)
{
    /* "IO Slot" */
    if (georam_cart_enabled()) {
        georam_powerup();
    }
    if (reu_cart_enabled()) {
        reu_powerup();
    }

    if (machine_class == VICE_MACHINE_C128) {
        c128cartridge->powerup();
    }

    /* "Main Slot" */
    memset(export_ram0, 0xff, C64CART_RAM_LIMIT);

    switch (mem_cartridge_type) {
        case CARTRIDGE_ACTION_REPLAY:
            actionreplay_powerup();
            break;
        case CARTRIDGE_ATOMIC_POWER:
            atomicpower_powerup();
            break;
        case CARTRIDGE_CAPTURE:
            capture_powerup();
            break;
        case CARTRIDGE_EASYFLASH:
            easyflash_powerup();
            break;
        case CARTRIDGE_KCS_POWER:
            kcs_powerup();
            break;
        case CARTRIDGE_LT_KERNAL:
            ltkernal_powerup();
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_powerup();
            break;
        case CARTRIDGE_MAX_BASIC:
            maxbasic_powerup();
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_powerup();
            break;
        case CARTRIDGE_MULTIMAX:
            multimax_powerup();
            break;
        case CARTRIDGE_PAGEFOX:
            pagefox_powerup();
            break;
        case CARTRIDGE_PARTNER64:
            partner64_powerup();
            break;
        case CARTRIDGE_RETRO_REPLAY:
            retroreplay_powerup();
            break;
        case CARTRIDGE_SDBOX:
            sdbox_powerup();
            break;
        case CARTRIDGE_SUPER_SNAPSHOT:
            supersnapshot_v4_powerup();
            break;
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            supersnapshot_v5_powerup();
            break;
    }

    /* "Slot 1" */
    if (dqbb_cart_enabled()) {
        dqbb_powerup();
    }
    if (expert_cart_enabled()) {
        expert_powerup();
    }
    if (isepic_cart_enabled()) {
        isepic_powerup();
    }
}

/* ------------------------------------------------------------------------- */

/* called by cart_nmi_alarm_triggered, after an alarm occured */
static void cart_freeze(int type)
{
    DBG(("CART: freeze (type:%d)\n", type));
    switch (type) {
        /* "Slot 0" (no freezer carts) */
        /* "Slot 1" */
        case CARTRIDGE_EXPERT:
            expert_freeze();
            break;
        case CARTRIDGE_ISEPIC:
            isepic_freeze();
            break;
        /* "IO Slot" (no freezer carts) */
        /* "Main Slot" */
        case CARTRIDGE_ACTION_REPLAY:
            actionreplay_freeze();
            break;
        case CARTRIDGE_ACTION_REPLAY2:
            actionreplay2_freeze();
            break;
        case CARTRIDGE_ACTION_REPLAY3:
            actionreplay3_freeze();
            break;
        case CARTRIDGE_ACTION_REPLAY4:
            actionreplay4_freeze();
            break;
        case CARTRIDGE_ATOMIC_POWER:
            atomicpower_freeze();
            break;
        case CARTRIDGE_CAPTURE:
            capture_freeze();
            break;
        case CARTRIDGE_DIASHOW_MAKER:
            dsm_freeze();
            break;
        case CARTRIDGE_FINAL_I:
            final_v1_freeze();
            break;
        case CARTRIDGE_FINAL_III:
            final_v3_freeze();
            break;
        case CARTRIDGE_FINAL_PLUS:
            final_plus_freeze();
            break;
        case CARTRIDGE_FREEZE_FRAME:
            freezeframe_freeze();
            break;
        case CARTRIDGE_FREEZE_FRAME_MK2:
            freezeframe2_freeze();
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            freezemachine_freeze();
            break;
        case CARTRIDGE_GAME_KILLER:
            gamekiller_freeze();
            break;
        case CARTRIDGE_KCS_POWER:
            kcs_freeze();
            break;
        case CARTRIDGE_LT_KERNAL:
            ltkernal_freeze();
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_freeze();
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_freeze();
            break;
        case CARTRIDGE_PARTNER64:
            partner64_freeze();
            break;
        case CARTRIDGE_RETRO_REPLAY:
            retroreplay_freeze();
            break;
        case CARTRIDGE_SNAPSHOT64:
            snapshot64_freeze();
            break;
        case CARTRIDGE_SUPER_SNAPSHOT:
            supersnapshot_v4_freeze();
            break;
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            supersnapshot_v5_freeze();
            break;
    }
    if (machine_class == VICE_MACHINE_C128) {
        c128cartridge->freeze();
    }
}

/* called by cart_nmi_alarm_triggered */
void cart_nmi_alarm(CLOCK offset, void *data)
{
    /* "Slot 0" (no freezer carts) */
    /* "Slot 1" */
    if (expert_freeze_allowed()) {
        cart_freeze(CARTRIDGE_EXPERT);
    }
    if (isepic_freeze_allowed()) {
        cart_freeze(CARTRIDGE_ISEPIC);
    }
    /* "I/O Slot" (no freezer carts) */
    /* "Main Slot" */
    cart_freeze(cart_getid_slotmain());
}

/* called by the UI when the freeze button is pressed */
int cart_freeze_allowed(void)
{
    int maintype = cart_getid_slotmain();
    /* "Slot 0" (no freezer carts) */
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
        case CARTRIDGE_ATOMIC_POWER:
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_DIASHOW_MAKER:
        case CARTRIDGE_FINAL_I:
        case CARTRIDGE_FINAL_III:
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_FREEZE_FRAME:
        case CARTRIDGE_FREEZE_FRAME_MK2:
        case CARTRIDGE_FREEZE_MACHINE:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_KCS_POWER:
        case CARTRIDGE_LT_KERNAL:
        case CARTRIDGE_MAGIC_FORMEL:
            return 1;
        case CARTRIDGE_MMC_REPLAY:
            if (mmcreplay_freeze_allowed()) {
                return 1;
            }
            break;
        case CARTRIDGE_PARTNER64:
            return 1;
        case CARTRIDGE_RETRO_REPLAY:
            if (retroreplay_freeze_allowed()) {
                return 1;
            }
            break;
        case CARTRIDGE_SNAPSHOT64:
        case CARTRIDGE_SUPER_SNAPSHOT:
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            return 1;
    }

    if (machine_class == VICE_MACHINE_C128) {
        if (c128cartridge->freeze_allowed() == 1) {
            return 1;
        }
    }

    /* "I/O Slot" (no freezer carts) */
    return 0;
}

/* ------------------------------------------------------------------------- */

/*
    flush cart image

    all carts whose image might be modified at runtime should be hooked up here.
*/
int cartridge_flush_image(int type)
{
    if ((machine_class == VICE_MACHINE_C128) && CARTRIDGE_C128_ISID(mem_cartridge_type)) {
        return c128cartridge->flush_image(mem_cartridge_type);
    }

    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_MMC64:
            return mmc64_flush_image();
        case CARTRIDGE_RAMLINK:
            return ramlink_flush_image();
        /* "Slot 1" */
        case CARTRIDGE_DQBB:
            return dqbb_flush_image();
        case CARTRIDGE_EXPERT:
            return expert_flush_image();
        case CARTRIDGE_ISEPIC:
            return isepic_flush_image();
        case CARTRIDGE_RAMCART:
            return ramcart_flush_image();
        /* "Main Slot" */
        case CARTRIDGE_EASYFLASH:
            return easyflash_flush_image();
        case CARTRIDGE_GMOD2:
            return gmod2_flush_image();
        case CARTRIDGE_GMOD3:
            return gmod3_flush_image();
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_flush_image();
        case CARTRIDGE_RETRO_REPLAY:
            return retroreplay_flush_image();
        case CARTRIDGE_REX_RAMFLOPPY:
            return rexramfloppy_flush_image();
#ifdef HAVE_RAWNET
        case CARTRIDGE_RRNETMK3:
            return rrnetmk3_flush_image();
#endif
        /* "I/O" */
        case CARTRIDGE_GEORAM:
            return georam_flush_image();
        case CARTRIDGE_REU:
            return reu_flush_image();
    }
    log_error(LOG_ERR, "Failed flushing cartridge image for cartridge ID %d.\n", type);
    return -1;
}

/*
    save cartridge to binary file

    *atleast* all carts whose image might be modified at runtime should be hooked up here.

    TODO: add bin save for all ROM carts also
*/
int cartridge_bin_save(int type, const char *filename)
{
    if ((machine_class == VICE_MACHINE_C128) && CARTRIDGE_C128_ISID(mem_cartridge_type)) {
        return c128cartridge->bin_save(mem_cartridge_type, filename);
    }

    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_MMC64:
            return mmc64_bin_save(filename);
        case CARTRIDGE_RAMLINK:
            /* HACK: this will save the RAMlinks RAM - not the actual cartridge
                     image. since we have no API for this special case (yet?)
                     we leave it here */
            return ramlink_bin_save(filename);
        /* "Slot 1" */
        case CARTRIDGE_DQBB:
            return dqbb_bin_save(filename);
        case CARTRIDGE_EXPERT:
            return expert_bin_save(filename);
        case CARTRIDGE_ISEPIC:
            return isepic_bin_save(filename);
        case CARTRIDGE_RAMCART:
            return ramcart_bin_save(filename);
        /* "Main Slot" */
        case CARTRIDGE_EASYFLASH:
            return easyflash_bin_save(filename);
        case CARTRIDGE_GMOD2:
            return gmod2_bin_save(filename);
        case CARTRIDGE_GMOD3:
            return gmod3_bin_save(filename);
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_bin_save(filename);
        case CARTRIDGE_RETRO_REPLAY:
            return retroreplay_bin_save(filename);
        case CARTRIDGE_REX_RAMFLOPPY:
            return rexramfloppy_bin_save(filename);
#ifdef HAVE_RAWNET
        case CARTRIDGE_RRNETMK3:
            return rrnetmk3_bin_save(filename);
#endif
        /* "I/O Slot" */
        case CARTRIDGE_GEORAM:
            return georam_bin_save(filename);
        case CARTRIDGE_REU:
            return reu_bin_save(filename);
    }
    log_error(LOG_ERR, "Failed saving binary cartridge image for cartridge ID %d.\n", type);
    return -1;
}

/*
    save cartridge to crt file

    *atleast* all carts whose image might be modified at runtime AND
    which have a valid crt id should be hooked up here.

    TODO: add crt save for all ROM carts also
*/
int cartridge_crt_save(int type, const char *filename)
{
    if ((machine_class == VICE_MACHINE_C128) && CARTRIDGE_C128_ISID(mem_cartridge_type)) {
        return c128cartridge->crt_save(mem_cartridge_type, filename);
    }

    switch (type) {
        /* "Slot 0" */
        case CARTRIDGE_MMC64:
            return mmc64_crt_save(filename);
        /* "Slot 1" */
        case CARTRIDGE_EXPERT:
            return expert_crt_save(filename);
        case CARTRIDGE_ISEPIC:
            return isepic_crt_save(filename);
        /* "Main Slot" */
        case CARTRIDGE_EASYFLASH:
            return easyflash_crt_save(filename);
        case CARTRIDGE_GMOD2:
            return gmod2_crt_save(filename);
        case CARTRIDGE_GMOD3:
            return gmod3_crt_save(filename);
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_crt_save(filename);
        case CARTRIDGE_RETRO_REPLAY:
            return retroreplay_crt_save(filename);
#ifdef HAVE_RAWNET
        case CARTRIDGE_RRNETMK3:
            return rrnetmk3_crt_save(filename);
#endif
    }
    log_error(LOG_ERR, "Failed saving .crt cartridge image for cartridge ID %d.\n", type);
    return -1;
}

/* ------------------------------------------------------------------------- */

void cartridge_sound_chip_init(void)
{
    digimax_sound_chip_init();
    sfx_soundsampler_sound_chip_init();
    sfx_soundexpander_sound_chip_init();
    magicvoice_sound_chip_init();
    shortbus_digimax_sound_chip_init();
}

/* ------------------------------------------------------------------------- */
/* Perform MMU translation for simple cartridge RAM/ROM/FLASH mappings.
   This function returns a pointer to a continuous memory area where addr
   points into. The boundary of the area are returned in start and limit,
   where limit is the last address where a dword read can be performed (last-3).

   The CPU uses this information to read data from the defined area quickly by
   using base[addr] instead of going through chains of various memory mapping
   hooks.

   It's important to know that this mapping optimization is valid until
   there's no memory mapping configuration change, then the translation is
   performed again. A memory configuration change can be signaled by calling
   maincpu_resync_limits directly, or preferably by calling one of these:
   - mem_pla_config_changed
   - cart_config_changed
   - cart_port_config_changed_slot0
   - cart_config_changed_slot0
   - cart_port_config_changed_slot1
   - cart_config_changed_slot1
   - cart_port_config_changed_slotmain
   - cart_config_changed_slotmain

   If such caching is not desired or not possible a base of NULL with start and
   limit of 0 shall be returned, then all CPU reads go through the normal
   hooks. There could be various reasons to do so:
   - the cartridge has no hooks yet
   - the address does not fall into a continuous memory area (RAM/ROM) managed
     by the cartridge in it's current configuration or banking
   - side effects should happen when the area is accessed (e.g. capacitor discharge,
     i/o area)
   - the memory area is programmable flash and it's not in it's "idle" mode now

   TODO: add more cartridges
*/
void cartridge_mmu_translate(unsigned int addr, uint8_t **base, int *start, int *limit)
{
    /* DBG(("CARTHOOKS: cartridge_mmu_translate(%x)\n",addr)); */
    int res = CART_READ_THROUGH;
#if 0
    /* disable all the mmu translation stuff for testing */
    *base = NULL;
    *start = 0;
    *limit = 0;
    return;
#endif
    /* "Slot 0" */
    if (mmc64_cart_enabled()) {
        if ((res = mmc64_mmu_translate(addr, base, start, limit)) == CART_READ_VALID) {
            return;
        }
    } else if (magicvoice_cart_enabled()) {
        if ((res = magicvoice_mmu_translate(addr, base, start, limit)) == CART_READ_VALID) {
            return;
        }
    } else if (tpi_cart_enabled()) {
        if ((res = tpi_mmu_translate(addr, base, start, limit)) == CART_READ_VALID) {
            return;
        }
    } else if (ieeeflash64_cart_enabled()) {
        if ((res = ieeeflash64_mmu_translate(addr, base, start, limit)) == CART_READ_VALID) {
            return;
        }
    } else if (ramlink_cart_enabled()) {
        if ((res = ramlink_mmu_translate(addr, base, start, limit)) == CART_READ_VALID) {
            return;
        }
    }

    switch (res) {
        case CART_READ_C64MEM:
            *base = NULL;
            *start = 0;
            *limit = 0;
            return;
        case CART_READ_THROUGH_NO_ULTIMAX:
            break;
    }

    /* continue with "Slot 1" */
    if (isepic_cart_active()) {
        isepic_mmu_translate(addr, base, start, limit);
        return;
    }
    if (expert_cart_enabled()) {
        expert_mmu_translate(addr, base, start, limit);
        return;
    }
    if (ramcart_cart_enabled()) {
        ramcart_mmu_translate(addr, base, start, limit);
        return;
    }
    if (dqbb_cart_enabled()) {
        dqbb_mmu_translate(addr, base, start, limit);
        return;
    }

    /* continue with "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_ACTION_REPLAY4:
        case CARTRIDGE_FINAL_III:
        case CARTRIDGE_GENERIC_16KB:
        case CARTRIDGE_GENERIC_8KB:
        case CARTRIDGE_KCS_POWER:
        case CARTRIDGE_SIMONS_BASIC:
        case CARTRIDGE_ULTIMAX:
            generic_mmu_translate(addr, base, start, limit);
            return;
        case CARTRIDGE_ATOMIC_POWER:
            atomicpower_mmu_translate(addr, base, start, limit);
            return;
        case CARTRIDGE_EASYFLASH:
            easyflash_mmu_translate(addr, base, start, limit);
            return;
        case CARTRIDGE_GMOD2:
            gmod2_mmu_translate(addr, base, start, limit);
            return;
        case CARTRIDGE_GMOD3:
            gmod3_mmu_translate(addr, base, start, limit);
            return;
        case CARTRIDGE_IDE64:
            ide64_mmu_translate(addr, base, start, limit);
            return;
        case CARTRIDGE_RETRO_REPLAY:
            retroreplay_mmu_translate(addr, base, start, limit);
            return;
#ifdef HAVE_RAWNET
        case CARTRIDGE_RRNETMK3:
            rrnetmk3_mmu_translate(addr, base, start, limit);
            return;
#endif
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            supersnapshot_v5_mmu_translate(addr, base, start, limit);
            return;
        case CARTRIDGE_EPYX_FASTLOAD: /* must go through roml_read to discharge capacitor */
        case CARTRIDGE_ZIPPCODE48: /* must go through roml_read to discharge capacitor */
        default:
            *base = NULL;
            *start = 0;
            *limit = 0;
            return;
    }
}

/* ------------------------------------------------------------------------- */

/*
    Snapshot reading and writing
*/

#define C64CART_DUMP_MAX_CARTS  16

#define C64CART_DUMP_VER_MAJOR   0
#define C64CART_DUMP_VER_MINOR   1
#define SNAP_MODULE_NAME  "C64CART"

int cartridge_snapshot_write_modules(struct snapshot_s *s)
{
    snapshot_module_t *m;

    uint8_t i;
    uint8_t number_of_carts = 0;
    int cart_ids[C64CART_DUMP_MAX_CARTS];
    int last_cart = 0;

    memset(cart_ids, 0, sizeof(cart_ids));

    /* Find out which carts are attached */
    {
        export_list_t *e = export_query_list(NULL);

        while (e != NULL) {
            if (number_of_carts == C64CART_DUMP_MAX_CARTS) {
                DBG(("CART snapshot save: active carts > max (%i)\n", number_of_carts));
                return -1;
            }
            if (last_cart != (int)e->device->cartid) {
                last_cart = e->device->cartid;
                cart_ids[number_of_carts++] = last_cart;
            }
            e = e->next;
        }
    }

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                               C64CART_DUMP_VER_MAJOR, C64CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (SMW_B(m, number_of_carts) < 0) {
        goto fail;
    }

    /* Not much to do if no carts present */
    if (number_of_carts == 0) {
        return snapshot_module_close(m);
    }

    /* Save "global" cartridge things */
    if (0
        || SMW_DW(m, (uint32_t)mem_cartridge_type) < 0
        || SMW_B(m, export.game) < 0
        || SMW_B(m, export.exrom) < 0
        || SMW_DW(m, (uint32_t)romh_bank) < 0
        || SMW_DW(m, (uint32_t)roml_bank) < 0
        || SMW_B(m, (uint8_t)export_ram) < 0
        || SMW_B(m, export.ultimax_phi1) < 0
        || SMW_B(m, export.ultimax_phi2) < 0
        || SMW_CLOCK(m, cart_freeze_alarm_time) < 0
        || SMW_CLOCK(m, cart_nmi_alarm_time) < 0
        || SMW_B(m, export_slot1.game) < 0
        || SMW_B(m, export_slot1.exrom) < 0
        || SMW_B(m, export_slot1.ultimax_phi1) < 0
        || SMW_B(m, export_slot1.ultimax_phi2) < 0
        || SMW_B(m, export_slotmain.game) < 0
        || SMW_B(m, export_slotmain.exrom) < 0
        || SMW_B(m, export_slotmain.ultimax_phi1) < 0
        || SMW_B(m, export_slotmain.ultimax_phi2) < 0
        || SMW_B(m, export_passthrough.game) < 0
        || SMW_B(m, export_passthrough.exrom) < 0
        || SMW_B(m, export_passthrough.ultimax_phi1) < 0
        || SMW_B(m, export_passthrough.ultimax_phi2) < 0
        /* some room for future expansion */
        || SMW_DW(m, 0) < 0
        || SMW_DW(m, 0) < 0
        || SMW_DW(m, 0) < 0
        || SMW_DW(m, 0) < 0) {
        goto fail;
    }

    /* Save cart IDs */
    for (i = 0; i < number_of_carts; i++) {
        if (SMW_DW(m, (uint32_t)cart_ids[i]) < 0) {
            goto fail;
        }
    }

    /* Main module done */
    snapshot_module_close(m);
    m = NULL;

    /* Save individual cart data */
    for (i = 0; i < number_of_carts; i++) {
        switch (cart_ids[i]) {
            /* "Slot 0" */
            case CARTRIDGE_CPM:
                if (cpmcart_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_IEEE488:
                if (tpi_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_RAMLINK:
                if (ramlink_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_IEEEFLASH64:
                if (ieeeflash64_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_MAGIC_VOICE:
                if (magicvoice_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_MMC64:
                if (mmc64_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;

            /* "Slot 1" */
            case CARTRIDGE_DQBB:
                if (dqbb_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_EXPERT:
                if (expert_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_ISEPIC:
                if (isepic_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_RAMCART:
                if (ramcart_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;

            /* "Main Slot" */
            case CARTRIDGE_ACTION_REPLAY:
                if (actionreplay_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_ACTION_REPLAY2:
                if (actionreplay2_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_ACTION_REPLAY3:
                if (actionreplay3_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_ACTION_REPLAY4:
                if (actionreplay4_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_ATOMIC_POWER:
                if (atomicpower_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_BISPLUS:
                if (bisplus_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_BLACKBOX3:
                if (blackbox3_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_BLACKBOX4:
                if (blackbox4_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_BLACKBOX8:
                if (blackbox8_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_CAPTURE:
                if (capture_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_COMAL80:
                if (comal80_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_DELA_EP64:
                if (delaep64_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_DELA_EP7x8:
                if (delaep7x8_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_DELA_EP256:
                if (delaep256_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_DIASHOW_MAKER:
                if (dsm_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_DINAMIC:
                if (dinamic_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_EASYCALC:
                if (easycalc_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_EASYFLASH:
                if (easyflash_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_EPYX_FASTLOAD:
                if (epyxfastload_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_EXOS:
                if (exos_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_FINAL_I:
                if (final_v1_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_FINAL_III:
                if (final_v3_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_FINAL_PLUS:
                if (final_plus_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_FORMEL64:
                if (formel64_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_FREEZE_FRAME:
                if (freezeframe_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_FREEZE_FRAME_MK2:
                if (freezeframe2_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_FREEZE_MACHINE:
                if (freezemachine_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_FUNPLAY:
                if (funplay_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_GAME_KILLER:
                if (gamekiller_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_GENERIC_16KB:
            case CARTRIDGE_GENERIC_8KB:
            case CARTRIDGE_ULTIMAX:
                if (generic_snapshot_write_module(s, cart_ids[i]) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_GMOD2:
                if (gmod2_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_GMOD3:
                if (gmod3_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_GS:
                if (gs_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_DREAN:
                if (drean_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_IDE64:
                if (ide64_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_KCS_POWER:
                if (kcs_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_KINGSOFT:
                if (kingsoft_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_LT_KERNAL:
                if (ltkernal_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_MACH5:
                if (mach5_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_MAGIC_DESK:
                if (magicdesk_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_MAGIC_DESK_16:
                if (magicdesk16_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_MAGIC_FORMEL:
                if (magicformel_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_MAX_BASIC:
                if (maxbasic_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_MIKRO_ASSEMBLER:
                if (mikroass_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_MMC_REPLAY:
                if (mmcreplay_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_MULTIMAX:
                if (multimax_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_OCEAN:
                if (ocean_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_P64:
                if (p64_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_PAGEFOX:
                if (pagefox_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_PARTNER64:
                if (partner64_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_RETRO_REPLAY:
                if (retroreplay_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_REX:
                if (rex_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_REX_EP256:
                if (rexep256_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_REX_RAMFLOPPY:
                if (rexramfloppy_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_RGCD:
                if (rgcd_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
#ifdef HAVE_RAWNET
            case CARTRIDGE_RRNETMK3:
                if (rrnetmk3_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
#endif
            case CARTRIDGE_ROSS:
                if (ross_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SDBOX:
                if (sdbox_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SILVERROCK_128:
                if (silverrock128_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SIMONS_BASIC:
                if (simon_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SNAPSHOT64:
                if (snapshot64_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_STARDOS:
                if (stardos_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_STRUCTURED_BASIC:
                if (stb_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SUPER_EXPLODE_V5:
                if (se5_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SUPER_GAMES:
                if (supergames_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SUPER_SNAPSHOT:
                if (supersnapshot_v4_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SUPER_SNAPSHOT_V5:
                if (supersnapshot_v5_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_TURTLE_GRAPHICS_II:
                if (turtlegraphics_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_WARPSPEED:
                if (warpspeed_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_WESTERMANN:
                if (westermann_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_ZAXXON:
                if (zaxxon_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_ZIPPCODE48:
                if (zippcode48_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;

            /* "IO Slot" */
            case CARTRIDGE_DIGIMAX:
                if (digimax_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_DS12C887RTC:
                if (ds12c887rtc_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_GEORAM:
                if (georam_write_snapshot_module(s) < 0) {
                    return -1;
                }
                break;
#ifdef HAVE_MIDI
            case CARTRIDGE_MIDI_PASSPORT:
            case CARTRIDGE_MIDI_DATEL:
            case CARTRIDGE_MIDI_SEQUENTIAL:
            case CARTRIDGE_MIDI_NAMESOFT:
            case CARTRIDGE_MIDI_MAPLIN:
                if (c64_midi_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
#endif
            case CARTRIDGE_REU:
                if (reu_write_snapshot_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SFX_SOUND_EXPANDER:
                if (sfx_soundexpander_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SFX_SOUND_SAMPLER:
                if (sfx_soundsampler_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
#ifdef HAVE_RAWNET
            case CARTRIDGE_TFE:
                if (ethernetcart_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
            case CARTRIDGE_TURBO232:
                if (aciacart_snapshot_write_module(s) < 0) {
                    return -1;
                }
                break;
#endif

            default:
                /* If the cart cannot be saved, we obviously can't load it either.
                   Returning an error at this point is better than failing at later. */
                DBG(("CART snapshot save: cart %i handler missing\n", cart_ids[i]));
                return -1;
        }
    }

    return 0;

fail:
    if (m != NULL) {
        snapshot_module_close(m);
    }
    return -1;
}

int cartridge_snapshot_read_modules(struct snapshot_s *s)
{
    snapshot_module_t *m;
    uint8_t vmajor, vminor;

    uint8_t i;
    uint8_t number_of_carts;
    int cart_ids[C64CART_DUMP_MAX_CARTS];
    int local_cartridge_reset;
    uint32_t dummy;

    m = snapshot_module_open(s, SNAP_MODULE_NAME, &vmajor, &vminor);

    if (m == NULL) {
        return -1;
    }

    if ((vmajor != C64CART_DUMP_VER_MAJOR) || (vminor != C64CART_DUMP_VER_MINOR)) {
        goto fail;
    }

    /* disable cartridge reset while detaching old cart */
    resources_get_int("CartridgeReset", &local_cartridge_reset);
    resources_set_int("CartridgeReset", 0);
    cartridge_detach_image(-1);
    resources_set_int("CartridgeReset", local_cartridge_reset);

    if (SMR_B(m, &number_of_carts) < 0) {
        goto fail;
    }

    /* Not much to do if no carts in snapshot */
    if (number_of_carts == 0) {
        return snapshot_module_close(m);
    }

    if (number_of_carts > C64CART_DUMP_MAX_CARTS) {
        DBG(("CART snapshot read: carts %i > max %i\n", number_of_carts, C64CART_DUMP_MAX_CARTS));
        goto fail;
    }

    /* Read "global" cartridge things */
    if (0
        || SMR_DW_INT(m, &mem_cartridge_type) < 0
        || SMR_B(m, &export.game) < 0
        || SMR_B(m, &export.exrom) < 0
        || SMR_DW_INT(m, &romh_bank) < 0
        || SMR_DW_INT(m, &roml_bank) < 0
        || SMR_B_INT(m, &export_ram) < 0
        || SMR_B(m, &export.ultimax_phi1) < 0
        || SMR_B(m, &export.ultimax_phi2) < 0
        || SMR_CLOCK(m, &cart_freeze_alarm_time) < 0
        || SMR_CLOCK(m, &cart_nmi_alarm_time) < 0
        || SMR_B(m, &export_slot1.game) < 0
        || SMR_B(m, &export_slot1.exrom) < 0
        || SMR_B(m, &export_slot1.ultimax_phi1) < 0
        || SMR_B(m, &export_slot1.ultimax_phi2) < 0
        || SMR_B(m, &export_slotmain.game) < 0
        || SMR_B(m, &export_slotmain.exrom) < 0
        || SMR_B(m, &export_slotmain.ultimax_phi1) < 0
        || SMR_B(m, &export_slotmain.ultimax_phi2) < 0
        || SMR_B(m, &export_passthrough.game) < 0
        || SMR_B(m, &export_passthrough.exrom) < 0
        || SMR_B(m, &export_passthrough.ultimax_phi1) < 0
        || SMR_B(m, &export_passthrough.ultimax_phi2) < 0
        /* some room for future expansion */
        || SMR_DW(m, &dummy) < 0
        || SMR_DW(m, &dummy) < 0
        || SMR_DW(m, &dummy) < 0
        || SMR_DW(m, &dummy) < 0) {
        goto fail;
    }

    /* Read cart IDs */
    for (i = 0; i < number_of_carts; i++) {
        if (SMR_DW_INT(m, &cart_ids[i]) < 0) {
            goto fail;
        }
    }

    /* Main module done */
    snapshot_module_close(m);
    m = NULL;

    /* Read individual cart data */
    for (i = 0; i < number_of_carts; i++) {
        switch (cart_ids[i]) {
            /* "Slot 0" */
            case CARTRIDGE_CPM:
                if (cpmcart_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_IEEE488:
                if (tpi_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_RAMLINK:
                if (ramlink_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_IEEEFLASH64:
                if (ieeeflash64_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_MAGIC_VOICE:
                if (magicvoice_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_MMC64:
                if (mmc64_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;

            /* "Slot 1" */
            case CARTRIDGE_DQBB:
                if (dqbb_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_EXPERT:
                if (expert_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_ISEPIC:
                if (isepic_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_RAMCART:
                if (ramcart_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;

            /* "Main Slot" */
            case CARTRIDGE_ACTION_REPLAY:
                if (actionreplay_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_ACTION_REPLAY2:
                if (actionreplay2_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_ACTION_REPLAY3:
                if (actionreplay3_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_ACTION_REPLAY4:
                if (actionreplay4_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_ATOMIC_POWER:
                if (atomicpower_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_BISPLUS:
                if (bisplus_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_BLACKBOX3:
                if (blackbox3_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_BLACKBOX4:
                if (blackbox4_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_BLACKBOX8:
                if (blackbox8_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_CAPTURE:
                if (capture_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_COMAL80:
                if (comal80_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_DELA_EP64:
                if (delaep64_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_DELA_EP7x8:
                if (delaep7x8_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_DELA_EP256:
                if (delaep256_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_DIASHOW_MAKER:
                if (dsm_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_DINAMIC:
                if (dinamic_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_EASYCALC:
                if (easycalc_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_EASYFLASH:
                if (easyflash_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_EPYX_FASTLOAD:
                if (epyxfastload_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_EXOS:
                if (exos_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_FINAL_I:
                if (final_v1_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_FINAL_III:
                if (final_v3_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_FINAL_PLUS:
                if (final_plus_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_FORMEL64:
                if (formel64_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_FREEZE_FRAME:
                if (freezeframe_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_FREEZE_FRAME_MK2:
                if (freezeframe2_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_FREEZE_MACHINE:
                if (freezemachine_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_FUNPLAY:
                if (funplay_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_GAME_KILLER:
                if (gamekiller_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_GENERIC_16KB:
            case CARTRIDGE_GENERIC_8KB:
            case CARTRIDGE_ULTIMAX:
                if (generic_snapshot_read_module(s, cart_ids[i]) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_GMOD2:
                if (gmod2_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_GMOD3:
                if (gmod3_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_GS:
                if (gs_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_DREAN:
                if (drean_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_IDE64:
                if (ide64_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_KCS_POWER:
                if (kcs_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_KINGSOFT:
                if (kingsoft_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_LT_KERNAL:
                if (ltkernal_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_MACH5:
                if (mach5_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_MAGIC_DESK:
                if (magicdesk_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_MAGIC_FORMEL:
                if (magicformel_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_MAX_BASIC:
                if (maxbasic_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_MIKRO_ASSEMBLER:
                if (mikroass_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_MMC_REPLAY:
                if (mmcreplay_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_MULTIMAX:
                if (multimax_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_OCEAN:
                if (ocean_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_P64:
                if (p64_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_PAGEFOX:
                if (pagefox_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_PARTNER64:
                if (partner64_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_RETRO_REPLAY:
                if (retroreplay_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_REX:
                if (rex_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_REX_EP256:
                if (rexep256_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_REX_RAMFLOPPY:
                if (rexramfloppy_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_RGCD:
                if (rgcd_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
#ifdef HAVE_RAWNET
            case CARTRIDGE_RRNETMK3:
                if (rrnetmk3_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
#endif
            case CARTRIDGE_ROSS:
                if (ross_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_SDBOX:
                if (sdbox_snapshot_read_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SILVERROCK_128:
                if (silverrock128_snapshot_read_module(s) < 0) {
                    return -1;
                }
                break;
            case CARTRIDGE_SIMONS_BASIC:
                if (simon_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_SNAPSHOT64:
                if (snapshot64_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_STARDOS:
                if (stardos_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_STRUCTURED_BASIC:
                if (stb_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_SUPER_EXPLODE_V5:
                if (se5_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_SUPER_GAMES:
                if (supergames_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_SUPER_SNAPSHOT:
                if (supersnapshot_v4_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_SUPER_SNAPSHOT_V5:
                if (supersnapshot_v5_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_TURTLE_GRAPHICS_II:
                if (turtlegraphics_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_WARPSPEED:
                if (warpspeed_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_WESTERMANN:
                if (westermann_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_ZAXXON:
                if (zaxxon_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_ZIPPCODE48:
                if (zippcode48_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;

            /* "IO Slot" */
            case CARTRIDGE_DIGIMAX:
                if (digimax_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_DS12C887RTC:
                if (ds12c887rtc_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_GEORAM:
                if (georam_read_snapshot_module(s) < 0) {
                    goto fail2;
                }
                break;
#ifdef HAVE_MIDI
            case CARTRIDGE_MIDI_PASSPORT:
            case CARTRIDGE_MIDI_DATEL:
            case CARTRIDGE_MIDI_SEQUENTIAL:
            case CARTRIDGE_MIDI_NAMESOFT:
            case CARTRIDGE_MIDI_MAPLIN:
                if (c64_midi_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
#endif
            case CARTRIDGE_REU:
                if (reu_read_snapshot_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_SFX_SOUND_EXPANDER:
                if (sfx_soundexpander_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
            case CARTRIDGE_SFX_SOUND_SAMPLER:
                if (sfx_soundsampler_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
#ifdef HAVE_RAWNET
            case CARTRIDGE_TFE:
                if (ethernetcart_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
#endif
#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)
            case CARTRIDGE_TURBO232:
                if (aciacart_snapshot_read_module(s) < 0) {
                    goto fail2;
                }
                break;
#endif

            default:
                DBG(("CART snapshot read: cart %i handler missing\n", cart_ids[i]));
                goto fail2;
        }

        cart_attach_from_snapshot(cart_ids[i]);
    }

    /* set up config */
    mem_pla_config_changed();
    machine_update_memory_ptrs();

    /* restore alarms */
    cart_undump_alarms();

    return 0;

fail:
    if (m != NULL) {
        snapshot_module_close(m);
    }
fail2:
    mem_cartridge_type = CARTRIDGE_NONE; /* Failed to load cartridge! */
    return -1;
}
