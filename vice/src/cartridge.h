/*
 * cartridge.h - Cartridge emulation.
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

#ifndef VICE_CARTRIDGE_H
#define VICE_CARTRIDGE_H

#include "types.h"

extern void cartridge_init(void);
extern void cartridge_reset(void);
extern int cartridge_resources_init(void);
extern void cartridge_resources_shutdown(void);
extern int cartridge_cmdline_options_init(void);

extern int cartridge_attach_image(int type, const char *filename);
extern void cartridge_detach_image(void);
extern void cartridge_set_default(void);
extern void cartridge_trigger_freeze(void);
extern void cartridge_trigger_freeze_nmi_only(void);
extern const char *cartridge_get_file_name(WORD addr);

extern int cartridge_save_image(const char *filename);

extern void cartridge_attach(int type, BYTE *rawcart);
extern void cartridge_detach(int type);
extern void cartridge_freeze(int type);

/* Known cartridge types.  */
#define CARTRIDGE_ULTIMAX           -6
#define CARTRIDGE_GENERIC_8KB       -3
#define CARTRIDGE_GENERIC_16KB      -2
#define CARTRIDGE_NONE              -1
#define CARTRIDGE_CRT                0
/* the following must match the CRT IDs */
#define CARTRIDGE_ACTION_REPLAY       1 /* actionreplay.c */
#define CARTRIDGE_KCS_POWER           2 /* kcs.c */
#define CARTRIDGE_FINAL_III           3 /* final3.c */
#define CARTRIDGE_SIMONS_BASIC        4 /* simonsbasic.c */
#define CARTRIDGE_OCEAN               5 /* ocean.c */
#define CARTRIDGE_EXPERT              6 /* expert.c */
#define CARTRIDGE_FUNPLAY             7 /* funplay.c */
#define CARTRIDGE_SUPER_GAMES         8 /* supergames.c */
#define CARTRIDGE_ATOMIC_POWER        9 /* atomicpower.c */
#define CARTRIDGE_EPYX_FASTLOAD      10 /* epyxfastload.c */
#define CARTRIDGE_WESTERMANN         11 /* westermann.c */
#define CARTRIDGE_REX                12 /* rexutility.c */
#define CARTRIDGE_FINAL_I            13 /* final.c */
#define CARTRIDGE_MAGIC_FORMEL       14 /* magicformel.c */
#define CARTRIDGE_GS                 15 /* gs.c */
#define CARTRIDGE_WARPSPEED          16 /* warpspeed.c */
#define CARTRIDGE_DINAMIC            17 /* dinamic.c */
#define CARTRIDGE_ZAXXON             18 /* zaxxon.c */
#define CARTRIDGE_MAGIC_DESK         19 /* magicdesk.c */
#define CARTRIDGE_SUPER_SNAPSHOT_V5  20 /* supersnapshot.c */
#define CARTRIDGE_COMAL80            21 /* comal80.c */
#define CARTRIDGE_STRUCTURED_BASIC   22 /* stb.c */
#define CARTRIDGE_ROSS               23 /* ross.c */
#define CARTRIDGE_DELA_EP64          24 /* delaep64.c */
#define CARTRIDGE_DELA_EP7x8         25 /* delaep7x8.c */
#define CARTRIDGE_DELA_EP256         26 /* delaep256.c */
#define CARTRIDGE_REX_EP256          27 /* rexep256.c */
#define CARTRIDGE_MIKRO_ASSEMBLER    28 /* mikroass.c */
#define CARTRIDGE_FINAL_PLUS         29 /* finalplus.c */
#define CARTRIDGE_ACTION_REPLAY4     30 /* actionreplay4.c */
#define CARTRIDGE_STARDOS            31 /* stardos.c */
#define CARTRIDGE_EASYFLASH          32 /* easyflash.c */
#define CARTRIDGE_EASYFLASH_XBANK    33 /* easyflash.c */
#define CARTRIDGE_CAPTURE            34 /* capture.c */
#define CARTRIDGE_ACTION_REPLAY3     35 /* actionreplay3.c */
#define CARTRIDGE_RETRO_REPLAY       36 /* retroreplay.c */
#define CARTRIDGE_MMC64              37 /* mmc64.c, spi-sdcard.c */
#define CARTRIDGE_MMC_REPLAY         38 /* mmcreplay.c, ser-eeprom.c, spi-sdcard.c */
#define CARTRIDGE_IDE64              39 /* ide64.c */
#define CARTRIDGE_SUPER_SNAPSHOT     40 /* supersnapshot4.c */
#define CARTRIDGE_IEEE488            41 /* c64tpi.c */
#define CARTRIDGE_GAME_KILLER        42 /* gamekiller.c */
#define CARTRIDGE_P64                43 /* prophet64.c */
#define CARTRIDGE_LASTID 43 /* last valid CRT ID */
/* more carts, which do not have a crt id (yet): */
/* FIXME: those that will never get an ID should probably get negative IDs here */
#define CARTRIDGE_DIGIMAX            100 /* digimax.c */
#define CARTRIDGE_DQBB               101 /* dqbb.c */
#define CARTRIDGE_GEORAM             102 /* georam.c */
#define CARTRIDGE_ISEPIC             103 /* isepic.c */
#define CARTRIDGE_RAMCART            104 /* ramcart.c */
#define CARTRIDGE_REU                105 /* reu.c */
#define CARTRIDGE_SFX_SOUND_EXPANDER 106 /* sfx_soundexpander.c, fmopl.c */
#define CARTRIDGE_SFX_SOUND_SAMPLER  107 /* sfx_soundsampler.c */
#define CARTRIDGE_EXOS               108 /* exos.c */
#define CARTRIDGE_FREEZE_FRAME       109 /* freezeframe.c */
#define CARTRIDGE_FREEZE_MACHINE     110 /* freezemachine.c */
#define CARTRIDGE_MIDI_PASSPORT      111 /* + c64-midi.c */
#define CARTRIDGE_MIDI_DATEL         112 /* + c64-midi.c */
#define CARTRIDGE_MIDI_SEQUENTIAL    113 /* + c64-midi.c */
#define CARTRIDGE_MIDI_NAMESOFT      114 /* + c64-midi.c */
#define CARTRIDGE_MIDI_MAPLIN        115 /* + c64-midi.c */
#define CARTRIDGE_TFE                116 /* + tfe.c */
#define CARTRIDGE_TURBO232           117 /* + c64acia1.c */
#define CARTRIDGE_SNAPSHOT64         118 /* snapshot64.c */
/* FIXME: files marked with + above should be moved into the cart directory */

/*
 * VIC20 cartridge system
 */
/* #define CARTRIDGE_NONE               -1 */
#define CARTRIDGE_VIC20_GENERIC      1
#define CARTRIDGE_VIC20_MEGACART     2
#define CARTRIDGE_VIC20_FINAL_EXPANSION 3
#define CARTRIDGE_VIC20_FP           4

/* 
 * VIC20 Generic cartridges
 *
 * The cartridge types below are only used during attach requests.
 * They will always be converted to CARTRIDGE_VIC20_GENERIC when
 * attached.  This also means they can be remapped at will.
 *
 * VIC20: &1 -> 0=4k, 1=8k; &16 -> 0= < 16k, 1= 16k 2nd half at $a000
 * (this logic is not used AFAIK /tlr)
 */
#define CARTRIDGE_VIC20_DETECT       0x8000
#define CARTRIDGE_VIC20_4KB_2000     0x8002
#define CARTRIDGE_VIC20_8KB_2000     0x8003
#define CARTRIDGE_VIC20_4KB_6000     0x8004
#define CARTRIDGE_VIC20_8KB_6000     0x8005
#define CARTRIDGE_VIC20_4KB_A000     0x8006
#define CARTRIDGE_VIC20_8KB_A000     0x8007
#define CARTRIDGE_VIC20_4KB_B000     0x8008
#define CARTRIDGE_VIC20_8KB_4000     0x8009
#define CARTRIDGE_VIC20_4KB_4000     0x800a

#define CARTRIDGE_VIC20_16KB_2000    0x8013
#define CARTRIDGE_VIC20_16KB_4000    0x8019
#define CARTRIDGE_VIC20_16KB_6000    0x8015


#endif
