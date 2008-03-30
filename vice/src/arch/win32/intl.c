/*
 * intl.c - Localization routines for Win32.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#include "cmdline.h"
#include "intl.h"
#include "lib.h"
#include "res.h" /* 10456 */
#include "resources.h"
#include "translate.h"
#include "util.h"
#include "ui.h"
#include "winmain.h"

#define countof(array) (sizeof(array) / sizeof((array)[0]))

typedef struct windows_iso_s {
    int windows_code;
    char *iso_language_code;
    int code_page;
} windows_iso_t;


/* this table holds only the currently present translation
   languages, to add a new translation look at
   http://www.unicode.org/unicode/onlinedat/languages.html
   for the corresponding windows and iso codes */

static windows_iso_t windows_to_iso[]={
  {LANG_ENGLISH, "en", 28591},
  {LANG_GERMAN, "de", 28591},
  {LANG_FRENCH, "fr", 28591},
  {LANG_ITALIAN, "it", 28591},
  {LANG_DUTCH, "nl", 28591},
  {LANG_POLISH, "pl", 28592},
  {LANG_SWEDISH, "sv", 28591},
  {0, NULL}
};

/* The language table is duplicated in
   the translate.c, make sure they match
   when adding a new language */

static char *language_table[] = {

/* english */
  "en",

/* german */
  "de",

/* french */
  "fr",

/* italian */
  "it",

/* dutch */
  "nl",

/* polish */
  "pl",

/* swedish */
  "sv"
};

static int intl_table[][countof(language_table)] = {

/* ------------------------------ DIALOG RESOURCES ------------------------------ */ 

/* resacia.rc */
/* en */ {IDD_ACIA_SETTINGS_DIALOG,
/* de */  IDD_ACIA_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_ACIA_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_ACIA_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_ACIA_SETTINGS_DIALOG_NL,
/* pl */  IDD_ACIA_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_ACIA_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resc128.rc */
/* en */ {IDD_C128ROM_SETTINGS_DIALOG,
/* de */  IDD_C128ROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C128ROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C128ROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C128ROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_C128ROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C128ROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resc128.rc */
/* en */ {IDD_C128DRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_C128DRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C128DRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C128DRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C128DRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_C128DRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C128DRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resc128.rc */
/* en */ {IDD_C128ROM_RESOURCE_DIALOG,
/* de */  IDD_C128ROM_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C128ROM_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C128ROM_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C128ROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_C128ROM_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C128ROM_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* resc128.rc */
/* en */ {IDD_C128ROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* resc128.rc */
/* en */ {IDD_C128_MACHINE_SETTINGS_DIALOG,
/* de */  IDD_C128_MACHINE_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C128_MACHINE_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C128_MACHINE_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C128_MACHINE_SETTINGS_DIALOG_NL,
/* pl */  IDD_C128_MACHINE_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C128_MACHINE_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resc128.rc */
/* en */ {IDD_C128_FUNCTIONROM_SETTINGS_DIALOG,
/* de */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resc128.rc */
/* en */ {IDD_DRIVE_SETTINGS_DIALOG_C128,
/* de */  IDD_DRIVE_SETTINGS_DIALOG_C128_DE,    /* fuzzy, size */
/* fr */  IDD_DRIVE_SETTINGS_DIALOG_C128_FR,    /* fuzzy, size */
/* it */  IDD_DRIVE_SETTINGS_DIALOG_C128_IT,    /* fuzzy, size */
/* nl */  IDD_DRIVE_SETTINGS_DIALOG_C128_NL,
/* pl */  IDD_DRIVE_SETTINGS_DIALOG_C128_PL,    /* fuzzy, size */
/* sv */  IDD_DRIVE_SETTINGS_DIALOG_C128_SV},   /* fuzzy, size */

/* resc128.rc */
/* en */ {IDD_C128KBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resdrivec64vic20.rc */
/* en */ {IDD_DRIVE_SETTINGS_DIALOG_C64VIC20,
/* de */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_DE,    /* fuzzy, size */
/* fr */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_FR,    /* fuzzy, size */
/* it */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_IT,    /* fuzzy, size */
/* nl */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_NL,
/* pl */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_PL,    /* fuzzy, size */
/* sv */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_SV},   /* fuzzy, size */

/* resdrivepetcbm2.rc */
/* en */ {IDD_DRIVE_SETTINGS_DIALOG_PETCBM2,
/* de */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_DE,    /* fuzzy, size */
/* fr */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_FR,    /* fuzzy, size */
/* it */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_IT,    /* fuzzy, size */
/* nl */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_NL,
/* pl */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_PL,    /* fuzzy, size */
/* sv */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_SV},   /* fuzzy, size */

/* resgeoram.rc */
/* en */ {IDD_GEORAM_SETTINGS_DIALOG,
/* de */  IDD_GEORAM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_GEORAM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_GEORAM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_GEORAM_SETTINGS_DIALOG_NL,
/* pl */  IDD_GEORAM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_GEORAM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resramcart.rc */
/* en */ {IDD_RAMCART_SETTINGS_DIALOG,
/* de */  IDD_RAMCART_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_RAMCART_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_RAMCART_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_RAMCART_SETTINGS_DIALOG_NL,
/* pl */  IDD_RAMCART_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_RAMCART_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resreu.rc */
/* en */ {IDD_REU_SETTINGS_DIALOG,
/* de */  IDD_REU_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_REU_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_REU_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_REU_SETTINGS_DIALOG_NL,
/* pl */  IDD_REU_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_REU_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* reside64.rc */
/* en */ {IDD_IDE64_SETTINGS_DIALOG,
/* de */  IDD_IDE64_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_IDE64_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_IDE64_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_IDE64_SETTINGS_DIALOG_NL,
/* pl */  IDD_IDE64_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_IDE64_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* restfe.rc */
/* en */ {IDD_TFE_SETTINGS_DIALOG,
/* de */  IDD_TFE_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_TFE_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_TFE_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_TFE_SETTINGS_DIALOG_NL,
/* pl */  IDD_TFE_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_TFE_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resrs232user.rc */
/* en */ {IDD_RS232USER_SETTINGS_DIALOG,
/* de */  IDD_RS232USER_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_RS232USER_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_RS232USER_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_RS232USER_SETTINGS_DIALOG_NL,
/* pl */  IDD_RS232USER_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_RS232USER_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resvicii.rc */
/* en */ {IDD_VICII_DIALOG,
/* de */  IDD_VICII_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_VICII_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_VICII_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_VICII_DIALOG_NL,
/* pl */  IDD_VICII_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_VICII_DIALOG_SV},   /* fuzzy, size */

/* resc64.rc */
/* en */ {IDD_C64ROM_SETTINGS_DIALOG,
/* de */  IDD_C64ROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C64ROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C64ROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C64ROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_C64ROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C64ROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resc64.rc */
/* en */ {IDD_C64DRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_C64DRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C64DRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C64DRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C64DRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_C64DRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C64DRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resc64.rc */
/* en */ {IDD_C64ROM_RESOURCE_DIALOG,
/* de */  IDD_C64ROM_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C64ROM_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C64ROM_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C64ROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_C64ROM_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C64ROM_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* resc64.rc */
/* en */ {IDD_C64ROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* resc64.rc */
/* en */ {IDD_C64KBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* rescbm2.rc */
/* en */ {IDD_CBM2ROM_SETTINGS_DIALOG,
/* de */  IDD_CBM2ROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_CBM2ROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_CBM2ROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_CBM2ROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_CBM2ROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_CBM2ROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* rescbm2.rc */
/* en */ {IDD_CBM2DRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* rescbm2.rc */
/* en */ {IDD_CBM2ROM_RESOURCE_DIALOG,
/* de */  IDD_CBM2ROM_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_CBM2ROM_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_CBM2ROM_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_CBM2ROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_CBM2ROM_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_CBM2ROM_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* rescbm2.rc */
/* en */ {IDD_CBM2ROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* rescbm2.rc */
/* en */ {IDD_CBMII_SETTINGS_MODEL_DIALOG,
/* de */  IDD_CBMII_SETTINGS_MODEL_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_CBMII_SETTINGS_MODEL_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_CBMII_SETTINGS_MODEL_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_CBMII_SETTINGS_MODEL_DIALOG_NL,
/* pl */  IDD_CBMII_SETTINGS_MODEL_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_CBMII_SETTINGS_MODEL_DIALOG_SV},   /* fuzzy, size */

/* rescbm2.rc */
/* en */ {IDD_CBMII_SETTINGS_IO_DIALOG,
/* de */  IDD_CBMII_SETTINGS_IO_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_CBMII_SETTINGS_IO_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_CBMII_SETTINGS_IO_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_CBMII_SETTINGS_IO_DIALOG_NL,
/* pl */  IDD_CBMII_SETTINGS_IO_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_CBMII_SETTINGS_IO_DIALOG_SV},   /* fuzzy, size */

/* rescbm2.rc */
/* en */ {IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* respet.rc */
/* en */ {IDD_PETROM_SETTINGS_DIALOG,
/* de */  IDD_PETROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PETROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PETROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PETROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_PETROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PETROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* respet.rc */
/* en */ {IDD_PETDRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_PETDRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PETDRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PETDRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PETDRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_PETDRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PETDRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* respet.rc */
/* en */ {IDD_PETROM_RESOURCE_DIALOG,
/* de */  IDD_PETROM_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PETROM_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PETROM_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PETROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_PETROM_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PETROM_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* respet.rc */
/* en */ {IDD_PETROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_PETROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PETROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PETROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PETROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_PETROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PETROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* respet.rc */
/* en */ {IDD_PET_SETTINGS_MODEL_DIALOG,
/* de */  IDD_PET_SETTINGS_MODEL_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PET_SETTINGS_MODEL_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PET_SETTINGS_MODEL_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PET_SETTINGS_MODEL_DIALOG_NL,
/* pl */  IDD_PET_SETTINGS_MODEL_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PET_SETTINGS_MODEL_DIALOG_SV},   /* fuzzy, size */

/* respet.rc */
/* en */ {IDD_PET_SETTINGS_IO_DIALOG,
/* de */  IDD_PET_SETTINGS_IO_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PET_SETTINGS_IO_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PET_SETTINGS_IO_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PET_SETTINGS_IO_DIALOG_NL,
/* pl */  IDD_PET_SETTINGS_IO_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PET_SETTINGS_IO_DIALOG_SV},   /* fuzzy, size */

/* respet.rc */
/* en */ {IDD_PET_SETTINGS_SUPER_DIALOG,
/* de */  IDD_PET_SETTINGS_SUPER_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PET_SETTINGS_SUPER_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PET_SETTINGS_SUPER_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PET_SETTINGS_SUPER_DIALOG_NL,
/* pl */  IDD_PET_SETTINGS_SUPER_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PET_SETTINGS_SUPER_DIALOG_SV},   /* fuzzy, size */

/* respet.rc */
/* en */ {IDD_PET_SETTINGS_8296_DIALOG,
/* de */  IDD_PET_SETTINGS_8296_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PET_SETTINGS_8296_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PET_SETTINGS_8296_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PET_SETTINGS_8296_DIALOG_NL,
/* pl */  IDD_PET_SETTINGS_8296_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PET_SETTINGS_8296_DIALOG_SV},   /* fuzzy, size */

/* respet.rc */
/* en */ {IDD_PETKBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resplus4.rc */
/* en */ {IDD_PLUS4ROM_SETTINGS_DIALOG,
/* de */  IDD_PLUS4ROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PLUS4ROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PLUS4ROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PLUS4ROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_PLUS4ROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PLUS4ROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resplus4.rc */
/* en */ {IDD_PLUS4DRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resplus4.rc */
/* en */ {IDD_PLUS4ROM_RESOURCE_DIALOG,
/* de */  IDD_PLUS4ROM_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PLUS4ROM_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PLUS4ROM_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PLUS4ROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_PLUS4ROM_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PLUS4ROM_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* resplus4.rc */
/* en */ {IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* resplus4.rc */
/* en */ {IDD_PLUS4_MEMORY_DIALOG,
/* de */  IDD_PLUS4_MEMORY_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PLUS4_MEMORY_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PLUS4_MEMORY_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PLUS4_MEMORY_DIALOG_NL,
/* pl */  IDD_PLUS4_MEMORY_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PLUS4_MEMORY_DIALOG_SV},   /* fuzzy, size */

/* resplus4.rc */
/* en */ {IDD_DRIVE_SETTINGS_DIALOG_PLUS4,
/* de */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_DE,    /* fuzzy, size */
/* fr */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_FR,    /* fuzzy, size */
/* it */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_IT,    /* fuzzy, size */
/* nl */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_NL,
/* pl */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_PL,    /* fuzzy, size */
/* sv */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_SV},   /* fuzzy, size */

/* resplus4.rc */
/* en */ {IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resvic20.rc */
/* en */ {IDD_VIC20ROM_SETTINGS_DIALOG,
/* de */  IDD_VIC20ROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_VIC20ROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_VIC20ROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_VIC20ROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_VIC20ROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_VIC20ROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resvic20.rc */
/* en */ {IDD_VIC20DRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resvic20.rc */
/* en */ {IDD_VIC_SETTINGS_DIALOG,
/* de */  IDD_VIC_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_VIC_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_VIC_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_VIC_SETTINGS_DIALOG_NL,
/* pl */  IDD_VIC_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_VIC_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* resvic20.rc */
/* en */ {IDD_VIC20ROM_RESOURCE_DIALOG,
/* de */  IDD_VIC20ROM_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_VIC20ROM_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_VIC20ROM_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_VIC20ROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_VIC20ROM_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_VIC20ROM_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* resvic20.rc */
/* en */ {IDD_VIC20ROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy, size */

/* resvic20.rc */
/* en */ {IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_DISKDEVICE_DIALOG,
/* de */  IDD_DISKDEVICE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_DISKDEVICE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_DISKDEVICE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_DISKDEVICE_DIALOG_NL,
/* pl */  IDD_DISKDEVICE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_DISKDEVICE_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_DATASETTE_SETTINGS_DIALOG,
/* de */  IDD_DATASETTE_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_DATASETTE_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_DATASETTE_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_DATASETTE_SETTINGS_DIALOG_NL,
/* pl */  IDD_DATASETTE_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_DATASETTE_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_JOY_SETTINGS_DIALOG,
/* de */  IDD_JOY_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_JOY_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_JOY_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_JOY_SETTINGS_DIALOG_NL,
/* pl */  IDD_JOY_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_JOY_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_CONFIG_KEYSET_DIALOG,
/* de */  IDD_CONFIG_KEYSET_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_CONFIG_KEYSET_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_CONFIG_KEYSET_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_CONFIG_KEYSET_DIALOG_NL,
/* pl */  IDD_CONFIG_KEYSET_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_CONFIG_KEYSET_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_SOUND_SETTINGS_DIALOG,
/* de */  IDD_SOUND_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_SOUND_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_SOUND_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_SOUND_SETTINGS_DIALOG_NL,
/* pl */  IDD_SOUND_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_SOUND_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_OPEN_TEMPLATE,
/* de */  IDD_OPEN_TEMPLATE_DE,    /* fuzzy, size */
/* fr */  IDD_OPEN_TEMPLATE_FR,    /* fuzzy, size */
/* it */  IDD_OPEN_TEMPLATE_IT,    /* fuzzy, size */
/* nl */  IDD_OPEN_TEMPLATE_NL,
/* pl */  IDD_OPEN_TEMPLATE_PL,    /* fuzzy, size */
/* sv */  IDD_OPEN_TEMPLATE_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_OPENTAPE_TEMPLATE,
/* de */  IDD_OPENTAPE_TEMPLATE_DE,    /* fuzzy, size */
/* fr */  IDD_OPENTAPE_TEMPLATE_FR,    /* fuzzy, size */
/* it */  IDD_OPENTAPE_TEMPLATE_IT,    /* fuzzy, size */
/* nl */  IDD_OPENTAPE_TEMPLATE_NL,
/* pl */  IDD_OPENTAPE_TEMPLATE_PL,    /* fuzzy, size */
/* sv */  IDD_OPENTAPE_TEMPLATE_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_SNAPSHOT_SAVE_DIALOG,
/* de */  IDD_SNAPSHOT_SAVE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_SNAPSHOT_SAVE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_SNAPSHOT_SAVE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_SNAPSHOT_SAVE_DIALOG_NL,
/* pl */  IDD_SNAPSHOT_SAVE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_SNAPSHOT_SAVE_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_MEDIAFILE_DIALOG,
/* de */  IDD_MEDIAFILE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_MEDIAFILE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_MEDIAFILE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_MEDIAFILE_DIALOG_NL,
/* pl */  IDD_MEDIAFILE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_MEDIAFILE_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_CONSOLE_SAVE_DIALOG,
/* de */  IDD_CONSOLE_SAVE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_CONSOLE_SAVE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_CONSOLE_SAVE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_CONSOLE_SAVE_DIALOG_NL,
/* pl */  IDD_CONSOLE_SAVE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_CONSOLE_SAVE_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_TEXTDLG,
/* de */  IDD_TEXTDLG_DE,    /* fuzzy, size */
/* fr */  IDD_TEXTDLG_FR,    /* fuzzy, size */
/* it */  IDD_TEXTDLG_IT,    /* fuzzy, size */
/* nl */  IDD_TEXTDLG_NL,
/* pl */  IDD_TEXTDLG_PL,    /* fuzzy, size */
/* sv */  IDD_TEXTDLG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_FULLSCREEN_SETTINGS_DIALOG,
/* de */  IDD_FULLSCREEN_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_FULLSCREEN_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_FULLSCREEN_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_FULLSCREEN_SETTINGS_DIALOG_NL,
/* pl */  IDD_FULLSCREEN_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_FULLSCREEN_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_VIDEO_COLORS_DIALOG,
/* de */  IDD_VIDEO_COLORS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_VIDEO_COLORS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_VIDEO_COLORS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_VIDEO_COLORS_DIALOG_NL,
/* pl */  IDD_VIDEO_COLORS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_VIDEO_COLORS_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_VIDEO_ADVANCED_DIALOG,
/* de */  IDD_VIDEO_ADVANCED_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_VIDEO_ADVANCED_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_VIDEO_ADVANCED_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_VIDEO_ADVANCED_DIALOG_NL,    /* parent needs some resizing */
/* pl */  IDD_VIDEO_ADVANCED_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_VIDEO_ADVANCED_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_VIDEO_PALETTE_DIALOG,
/* de */  IDD_VIDEO_PALETTE_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_VIDEO_PALETTE_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_VIDEO_PALETTE_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_VIDEO_PALETTE_DIALOG_NL,
/* pl */  IDD_VIDEO_PALETTE_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_VIDEO_PALETTE_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_RAM_SETTINGS_DIALOG,
/* de */  IDD_RAM_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_RAM_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_RAM_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_RAM_SETTINGS_DIALOG_NL,
/* pl */  IDD_RAM_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_RAM_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_CUSTOM_SPEED_DIALOG,
/* de */  IDD_CUSTOM_SPEED_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_CUSTOM_SPEED_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_CUSTOM_SPEED_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_CUSTOM_SPEED_DIALOG_NL,
/* pl */  IDD_CUSTOM_SPEED_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_CUSTOM_SPEED_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_PRINTER_SETTINGS_DIALOG,
/* de */  IDD_PRINTER_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_PRINTER_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_PRINTER_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_PRINTER_SETTINGS_DIALOG_NL,
/* pl */  IDD_PRINTER_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_PRINTER_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_ROMSET_SETTINGS_DIALOG,
/* de */  IDD_ROMSET_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_ROMSET_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_ROMSET_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_ROMSET_SETTINGS_DIALOG_NL,
/* pl */  IDD_ROMSET_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_ROMSET_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_ROMSET_ENTER_NAME_DIALOG,
/* de */  IDD_ROMSET_ENTER_NAME_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_ROMSET_ENTER_NAME_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_ROMSET_ENTER_NAME_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_ROMSET_ENTER_NAME_DIALOG_NL,
/* pl */  IDD_ROMSET_ENTER_NAME_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_ROMSET_ENTER_NAME_DIALOG_SV},   /* fuzzy, size */

/* res.rc */
/* en */ {IDD_RS232_SETTINGS_DIALOG,
/* de */  IDD_RS232_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_RS232_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_RS232_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_RS232_SETTINGS_DIALOG_NL,    /* size */
/* pl */  IDD_RS232_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_RS232_SETTINGS_DIALOG_SV},   /* fuzzy, size */


/* ------------------------------ MENU RESOURCES ------------------------------ */ 

/* res.rc */
/* en */ {IDR_MENUMONITOR,
/* de */  IDR_MENUMONITOR_DE,    /* fuzzy */
/* fr */  IDR_MENUMONITOR_FR,    /* fuzzy */
/* it */  IDR_MENUMONITOR_IT,    /* fuzzy */
/* nl */  IDR_MENUMONITOR_NL,
/* pl */  IDR_MENUMONITOR_PL,    /* fuzzy */
/* sv */  IDR_MENUMONITOR_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDR_MENUC128,
/* de */  IDR_MENUC128_DE,    /* fuzzy */
/* fr */  IDR_MENUC128_FR,    /* fuzzy */
/* it */  IDR_MENUC128_IT,    /* fuzzy */
/* nl */  IDR_MENUC128_NL,
/* pl */  IDR_MENUC128_PL,    /* fuzzy */
/* sv */  IDR_MENUC128_SV},   /* fuzzy */

/* resc64.rc */
/* en */ {IDR_MENUC64,
/* de */  IDR_MENUC64_DE,    /* fuzzy */
/* fr */  IDR_MENUC64_FR,    /* fuzzy */
/* it */  IDR_MENUC64_IT,    /* fuzzy */
/* nl */  IDR_MENUC64_NL,
/* pl */  IDR_MENUC64_PL,    /* fuzzy */
/* sv */  IDR_MENUC64_SV},   /* fuzzy */

/* rescbm2.rc */
/* en */ {IDR_MENUCBM2,
/* de */  IDR_MENUCBM2_DE,    /* fuzzy */
/* fr */  IDR_MENUCBM2_FR,    /* fuzzy */
/* it */  IDR_MENUCBM2_IT,    /* fuzzy */
/* nl */  IDR_MENUCBM2_NL,
/* pl */  IDR_MENUCBM2_PL,    /* fuzzy */
/* sv */  IDR_MENUCBM2_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDR_MENUPET,
/* de */  IDR_MENUPET_DE,    /* fuzzy */
/* fr */  IDR_MENUPET_FR,    /* fuzzy */
/* it */  IDR_MENUPET_IT,    /* fuzzy */
/* nl */  IDR_MENUPET_NL,
/* pl */  IDR_MENUPET_PL,    /* fuzzy */
/* sv */  IDR_MENUPET_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDR_MENUPLUS4,
/* de */  IDR_MENUPLUS4_DE,    /* fuzzy */
/* fr */  IDR_MENUPLUS4_FR,    /* fuzzy */
/* it */  IDR_MENUPLUS4_IT,    /* fuzzy */
/* nl */  IDR_MENUPLUS4_NL,
/* pl */  IDR_MENUPLUS4_PL,    /* fuzzy */
/* sv */  IDR_MENUPLUS4_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDR_MENUVIC,
/* de */  IDR_MENUVIC_DE,    /* fuzzy */
/* fr */  IDR_MENUVIC_FR,    /* fuzzy */
/* it */  IDR_MENUVIC_IT,    /* fuzzy */
/* nl */  IDR_MENUVIC_NL,
/* pl */  IDR_MENUVIC_PL,    /* fuzzy */
/* sv */  IDR_MENUVIC_SV},   /* fuzzy */

};

/* --------------------------------------------------------------------- */

static int intl_translate_text_table[][countof(language_table)] = {

/* res.rc */
/* en */ {IDS_RS232_DEVICE_I,
/* de */  IDS_RS232_DEVICE_I_DE,    /* fuzzy */
/* fr */  IDS_RS232_DEVICE_I_FR,    /* fuzzy */
/* it */  IDS_RS232_DEVICE_I_IT,    /* fuzzy */
/* nl */  IDS_RS232_DEVICE_I_NL,
/* pl */  IDS_RS232_DEVICE_I_PL,    /* fuzzy */
/* sv */  IDS_RS232_DEVICE_I_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_NONE,
/* de */  IDS_NONE_DE,
/* fr */  IDS_NONE_FR,
/* it */  IDS_NONE_IT,
/* nl */  IDS_NONE_NL,
/* pl */  IDS_NONE_PL,
/* sv */  IDS_NONE_SV},

/* resacia.rc */
/* en */ {IDS_IRQ,
/* de */  IDS_IRQ_DE,    /* fuzzy */
/* fr */  IDS_IRQ_FR,    /* fuzzy */
/* it */  IDS_IRQ_IT,    /* fuzzy */
/* nl */  IDS_IRQ_NL,
/* pl */  IDS_IRQ_PL,    /* fuzzy */
/* sv */  IDS_IRQ_SV},   /* fuzzy */

/* resacia.rc */
/* en */ {IDS_NMI,
/* de */  IDS_NMI_DE,    /* fuzzy */
/* fr */  IDS_NMI_FR,    /* fuzzy */
/* it */  IDS_NMI_IT,    /* fuzzy */
/* nl */  IDS_NMI_NL,
/* pl */  IDS_NMI_PL,    /* fuzzy */
/* sv */  IDS_NMI_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_VICE_ERROR,
/* de */  IDS_VICE_ERROR_DE,    /* fuzzy */
/* fr */  IDS_VICE_ERROR_FR,    /* fuzzy */
/* it */  IDS_VICE_ERROR_IT,    /* fuzzy */
/* nl */  IDS_VICE_ERROR_NL,
/* pl */  IDS_VICE_ERROR_PL,    /* fuzzy */
/* sv */  IDS_VICE_ERROR_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_VICE_INFORMATION,
/* de */  IDS_VICE_INFORMATION_DE,    /* fuzzy */
/* fr */  IDS_VICE_INFORMATION_FR,    /* fuzzy */
/* it */  IDS_VICE_INFORMATION_IT,    /* fuzzy */
/* nl */  IDS_VICE_INFORMATION_NL,
/* pl */  IDS_VICE_INFORMATION_PL,    /* fuzzy */
/* sv */  IDS_VICE_INFORMATION_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_VICE_QUESTION,
/* de */  IDS_VICE_QUESTION_DE,    /* fuzzy */
/* fr */  IDS_VICE_QUESTION_FR,    /* fuzzy */
/* it */  IDS_VICE_QUESTION_IT,    /* fuzzy */
/* nl */  IDS_VICE_QUESTION_NL,
/* pl */  IDS_VICE_QUESTION_PL,    /* fuzzy */
/* sv */  IDS_VICE_QUESTION_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_FFMPEG_DLL_MISMATCH,
/* de */  IDS_FFMPEG_DLL_MISMATCH_DE,    /* fuzzy */
/* fr */  IDS_FFMPEG_DLL_MISMATCH_FR,    /* fuzzy */
/* it */  IDS_FFMPEG_DLL_MISMATCH_IT,    /* fuzzy */
/* nl */  IDS_FFMPEG_DLL_MISMATCH_NL,
/* pl */  IDS_FFMPEG_DLL_MISMATCH_PL,    /* fuzzy */
/* sv */  IDS_FFMPEG_DLL_MISMATCH_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DIRECTDRAW_ERROR,
/* de */  IDS_DIRECTDRAW_ERROR_DE,    /* fuzzy */
/* fr */  IDS_DIRECTDRAW_ERROR_FR,    /* fuzzy */
/* it */  IDS_DIRECTDRAW_ERROR_IT,    /* fuzzy */
/* nl */  IDS_DIRECTDRAW_ERROR_NL,
/* pl */  IDS_DIRECTDRAW_ERROR_PL,    /* fuzzy */
/* sv */  IDS_DIRECTDRAW_ERROR_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_SAVE_SETTINGS,
/* de */  IDS_CANNOT_SAVE_SETTINGS_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_SAVE_SETTINGS_FR,    /* fuzzy */
/* it */  IDS_CANNOT_SAVE_SETTINGS_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_SAVE_SETTINGS_NL,
/* pl */  IDS_CANNOT_SAVE_SETTINGS_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_SAVE_SETTINGS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_LOAD_SETTINGS,
/* de */  IDS_CANNOT_LOAD_SETTINGS_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_LOAD_SETTINGS_FR,    /* fuzzy */
/* it */  IDS_CANNOT_LOAD_SETTINGS_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_LOAD_SETTINGS_NL,
/* pl */  IDS_CANNOT_LOAD_SETTINGS_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_LOAD_SETTINGS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DEFAULT_SETTINGS_RESTORED,
/* de */  IDS_DEFAULT_SETTINGS_RESTORED_DE,    /* fuzzy */
/* fr */  IDS_DEFAULT_SETTINGS_RESTORED_FR,    /* fuzzy */
/* it */  IDS_DEFAULT_SETTINGS_RESTORED_IT,    /* fuzzy */
/* nl */  IDS_DEFAULT_SETTINGS_RESTORED_NL,
/* pl */  IDS_DEFAULT_SETTINGS_RESTORED_PL,    /* fuzzy */
/* sv */  IDS_DEFAULT_SETTINGS_RESTORED_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_ATTACH_FILE,
/* de */  IDS_CANNOT_ATTACH_FILE_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_ATTACH_FILE_FR,    /* fuzzy */
/* it */  IDS_CANNOT_ATTACH_FILE_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_ATTACH_FILE_NL,
/* pl */  IDS_CANNOT_ATTACH_FILE_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_ATTACH_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_START_MONITOR,
/* de */  IDS_START_MONITOR_DE,    /* fuzzy */
/* fr */  IDS_START_MONITOR_FR,    /* fuzzy */
/* it */  IDS_START_MONITOR_IT,    /* fuzzy */
/* nl */  IDS_START_MONITOR_NL,
/* pl */  IDS_START_MONITOR_PL,    /* fuzzy */
/* sv */  IDS_START_MONITOR_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_VICE_CPU_JAM,
/* de */  IDS_VICE_CPU_JAM_DE,    /* fuzzy */
/* fr */  IDS_VICE_CPU_JAM_FR,    /* fuzzy */
/* it */  IDS_VICE_CPU_JAM_IT,    /* fuzzy */
/* nl */  IDS_VICE_CPU_JAM_NL,
/* pl */  IDS_VICE_CPU_JAM_PL,    /* fuzzy */
/* sv */  IDS_VICE_CPU_JAM_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_EXTEND_TO_40_TRACKS,
/* de */  IDS_EXTEND_TO_40_TRACKS_DE,    /* fuzzy */
/* fr */  IDS_EXTEND_TO_40_TRACKS_FR,    /* fuzzy */
/* it */  IDS_EXTEND_TO_40_TRACKS_IT,    /* fuzzy */
/* nl */  IDS_EXTEND_TO_40_TRACKS_NL,
/* pl */  IDS_EXTEND_TO_40_TRACKS_PL,    /* fuzzy */
/* sv */  IDS_EXTEND_TO_40_TRACKS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DETACHED_DEVICE_S,
/* de */  IDS_DETACHED_DEVICE_S_DE,    /* fuzzy */
/* fr */  IDS_DETACHED_DEVICE_S_FR,    /* fuzzy */
/* it */  IDS_DETACHED_DEVICE_S_IT,    /* fuzzy */
/* nl */  IDS_DETACHED_DEVICE_S_NL,
/* pl */  IDS_DETACHED_DEVICE_S_PL,    /* fuzzy */
/* sv */  IDS_DETACHED_DEVICE_S_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACHED_S_TO_DEVICE_S,
/* de */  IDS_ATTACHED_S_TO_DEVICE_S_DE,    /* fuzzy */
/* fr */  IDS_ATTACHED_S_TO_DEVICE_S_FR,    /* fuzzy */
/* it */  IDS_ATTACHED_S_TO_DEVICE_S_IT,    /* fuzzy */
/* nl */  IDS_ATTACHED_S_TO_DEVICE_S_NL,
/* pl */  IDS_ATTACHED_S_TO_DEVICE_S_PL,    /* fuzzy */
/* sv */  IDS_ATTACHED_S_TO_DEVICE_S_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DETACHED_TAPE,
/* de */  IDS_DETACHED_TAPE_DE,    /* fuzzy */
/* fr */  IDS_DETACHED_TAPE_FR,    /* fuzzy */
/* it */  IDS_DETACHED_TAPE_IT,    /* fuzzy */
/* nl */  IDS_DETACHED_TAPE_NL,
/* pl */  IDS_DETACHED_TAPE_PL,    /* fuzzy */
/* sv */  IDS_DETACHED_TAPE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_HISTORY_RECORDED_UNKNOWN,
/* de */  IDS_HISTORY_RECORDED_UNKNOWN_DE,    /* fuzzy */
/* fr */  IDS_HISTORY_RECORDED_UNKNOWN_FR,    /* fuzzy */
/* it */  IDS_HISTORY_RECORDED_UNKNOWN_IT,    /* fuzzy */
/* nl */  IDS_HISTORY_RECORDED_UNKNOWN_NL,
/* pl */  IDS_HISTORY_RECORDED_UNKNOWN_PL,    /* fuzzy */
/* sv */  IDS_HISTORY_RECORDED_UNKNOWN_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_HISTORY_RECORDED_VICE_S,
/* de */  IDS_HISTORY_RECORDED_VICE_S_DE,    /* fuzzy */
/* fr */  IDS_HISTORY_RECORDED_VICE_S_FR,    /* fuzzy */
/* it */  IDS_HISTORY_RECORDED_VICE_S_IT,    /* fuzzy */
/* nl */  IDS_HISTORY_RECORDED_VICE_S_NL,
/* pl */  IDS_HISTORY_RECORDED_VICE_S_PL,    /* fuzzy */
/* sv */  IDS_HISTORY_RECORDED_VICE_S_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PAUSED,
/* de */  IDS_PAUSED_DE,    /* fuzzy */
/* fr */  IDS_PAUSED_FR,    /* fuzzy */
/* it */  IDS_PAUSED_IT,    /* fuzzy */
/* nl */  IDS_PAUSED_NL,
/* pl */  IDS_PAUSED_PL,    /* fuzzy */
/* sv */  IDS_PAUSED_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_RESUMED,
/* de */  IDS_RESUMED_DE,    /* fuzzy */
/* fr */  IDS_RESUMED_FR,    /* fuzzy */
/* it */  IDS_RESUMED_IT,    /* fuzzy */
/* nl */  IDS_RESUMED_NL,
/* pl */  IDS_RESUMED_PL,    /* fuzzy */
/* sv */  IDS_RESUMED_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_REALLY_EXIT,
/* de */  IDS_REALLY_EXIT_DE,    /* fuzzy */
/* fr */  IDS_REALLY_EXIT_FR,    /* fuzzy */
/* it */  IDS_REALLY_EXIT_IT,    /* fuzzy */
/* nl */  IDS_REALLY_EXIT_NL,
/* pl */  IDS_REALLY_EXIT_PL,    /* fuzzy */
/* sv */  IDS_REALLY_EXIT_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_AUTOSTART_FILE,
/* de */  IDS_CANNOT_AUTOSTART_FILE_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_AUTOSTART_FILE_FR,    /* fuzzy */
/* it */  IDS_CANNOT_AUTOSTART_FILE_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_AUTOSTART_FILE_NL,
/* pl */  IDS_CANNOT_AUTOSTART_FILE_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_AUTOSTART_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACHED_TAPE_S,
/* de */  IDS_ATTACHED_TAPE_S_DE,    /* fuzzy */
/* fr */  IDS_ATTACHED_TAPE_S_FR,    /* fuzzy */
/* it */  IDS_ATTACHED_TAPE_S_IT,    /* fuzzy */
/* nl */  IDS_ATTACHED_TAPE_S_NL,
/* pl */  IDS_ATTACHED_TAPE_S_PL,    /* fuzzy */
/* sv */  IDS_ATTACHED_TAPE_S_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SETTINGS_SAVED_SUCCESS,
/* de */  IDS_SETTINGS_SAVED_SUCCESS_DE,    /* fuzzy */
/* fr */  IDS_SETTINGS_SAVED_SUCCESS_FR,    /* fuzzy */
/* it */  IDS_SETTINGS_SAVED_SUCCESS_IT,    /* fuzzy */
/* nl */  IDS_SETTINGS_SAVED_SUCCESS_NL,
/* pl */  IDS_SETTINGS_SAVED_SUCCESS_PL,    /* fuzzy */
/* sv */  IDS_SETTINGS_SAVED_SUCCESS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SETTINGS_LOADED_SUCCESS,
/* de */  IDS_SETTINGS_LOADED_SUCCESS_DE,    /* fuzzy */
/* fr */  IDS_SETTINGS_LOADED_SUCCESS_FR,    /* fuzzy */
/* it */  IDS_SETTINGS_LOADED_SUCCESS_IT,    /* fuzzy */
/* nl */  IDS_SETTINGS_LOADED_SUCCESS_NL,
/* pl */  IDS_SETTINGS_LOADED_SUCCESS_PL,    /* fuzzy */
/* sv */  IDS_SETTINGS_LOADED_SUCCESS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_DISK_IMAGE,
/* de */  IDS_ATTACH_DISK_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_DISK_IMAGE_FR,    /* fuzzy */
/* it */  IDS_ATTACH_DISK_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_DISK_IMAGE_NL,
/* pl */  IDS_ATTACH_DISK_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_DISK_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_TAPE_IMAGE,
/* de */  IDS_ATTACH_TAPE_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_TAPE_IMAGE_FR,    /* fuzzy */
/* it */  IDS_ATTACH_TAPE_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_TAPE_IMAGE_NL,
/* pl */  IDS_ATTACH_TAPE_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_TAPE_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_AUTOSTART_IMAGE,
/* de */  IDS_AUTOSTART_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_AUTOSTART_IMAGE_FR,    /* fuzzy */
/* it */  IDS_AUTOSTART_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_AUTOSTART_IMAGE_NL,
/* pl */  IDS_AUTOSTART_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_AUTOSTART_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_INVALID_CARTRIDGE,
/* de */  IDS_INVALID_CARTRIDGE_DE,    /* fuzzy */
/* fr */  IDS_INVALID_CARTRIDGE_FR,    /* fuzzy */
/* it */  IDS_INVALID_CARTRIDGE_IT,    /* fuzzy */
/* nl */  IDS_INVALID_CARTRIDGE_NL,
/* pl */  IDS_INVALID_CARTRIDGE_PL,    /* fuzzy */
/* sv */  IDS_INVALID_CARTRIDGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_BAD_CARTRIDGE_CONFIG,
/* de */  IDS_BAD_CARTRIDGE_CONFIG_DE,    /* fuzzy */
/* fr */  IDS_BAD_CARTRIDGE_CONFIG_FR,    /* fuzzy */
/* it */  IDS_BAD_CARTRIDGE_CONFIG_IT,    /* fuzzy */
/* nl */  IDS_BAD_CARTRIDGE_CONFIG_NL,
/* pl */  IDS_BAD_CARTRIDGE_CONFIG_PL,    /* fuzzy */
/* sv */  IDS_BAD_CARTRIDGE_CONFIG_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_INVALID_CARTRIDGE_IMAGE,
/* de */  IDS_INVALID_CARTRIDGE_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_INVALID_CARTRIDGE_IMAGE_FR,    /* fuzzy */
/* it */  IDS_INVALID_CARTRIDGE_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_INVALID_CARTRIDGE_IMAGE_NL,
/* pl */  IDS_INVALID_CARTRIDGE_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_INVALID_CARTRIDGE_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_LOG_CONSOLE_OUTPUT_IMAGE,
/* de */  IDS_LOG_CONSOLE_OUTPUT_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_LOG_CONSOLE_OUTPUT_IMAGE_FR,    /* fuzzy */
/* it */  IDS_LOG_CONSOLE_OUTPUT_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_LOG_CONSOLE_OUTPUT_IMAGE_NL,
/* pl */  IDS_LOG_CONSOLE_OUTPUT_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_LOG_CONSOLE_OUTPUT_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_LOG_FILES_TYPE,
/* de */  IDS_LOG_FILES_TYPE_DE,    /* fuzzy */
/* fr */  IDS_LOG_FILES_TYPE_FR,    /* fuzzy */
/* it */  IDS_LOG_FILES_TYPE_IT,    /* fuzzy */
/* nl */  IDS_LOG_FILES_TYPE_NL,
/* pl */  IDS_LOG_FILES_TYPE_PL,    /* fuzzy */
/* sv */  IDS_LOG_FILES_TYPE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_WRITE_LOGFILE_S,
/* de */  IDS_CANNOT_WRITE_LOGFILE_S_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_WRITE_LOGFILE_S_FR,    /* fuzzy */
/* it */  IDS_CANNOT_WRITE_LOGFILE_S_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_WRITE_LOGFILE_S_NL,
/* pl */  IDS_CANNOT_WRITE_LOGFILE_S_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_WRITE_LOGFILE_S_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_LOAD_FLIP_LIST_FILE,
/* de */  IDS_LOAD_FLIP_LIST_FILE_DE,    /* fuzzy */
/* fr */  IDS_LOAD_FLIP_LIST_FILE_FR,    /* fuzzy */
/* it */  IDS_LOAD_FLIP_LIST_FILE_IT,    /* fuzzy */
/* nl */  IDS_LOAD_FLIP_LIST_FILE_NL,
/* pl */  IDS_LOAD_FLIP_LIST_FILE_PL,    /* fuzzy */
/* sv */  IDS_LOAD_FLIP_LIST_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_READ_FLIP_LIST,
/* de */  IDS_CANNOT_READ_FLIP_LIST_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_READ_FLIP_LIST_FR,    /* fuzzy */
/* it */  IDS_CANNOT_READ_FLIP_LIST_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_READ_FLIP_LIST_NL,
/* pl */  IDS_CANNOT_READ_FLIP_LIST_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_READ_FLIP_LIST_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SAVE_FLIP_LIST_FILE,
/* de */  IDS_SAVE_FLIP_LIST_FILE_DE,    /* fuzzy */
/* fr */  IDS_SAVE_FLIP_LIST_FILE_FR,    /* fuzzy */
/* it */  IDS_SAVE_FLIP_LIST_FILE_IT,    /* fuzzy */
/* nl */  IDS_SAVE_FLIP_LIST_FILE_NL,
/* pl */  IDS_SAVE_FLIP_LIST_FILE_PL,    /* fuzzy */
/* sv */  IDS_SAVE_FLIP_LIST_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_WRITE_FLIP_LIST,
/* de */  IDS_CANNOT_WRITE_FLIP_LIST_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_WRITE_FLIP_LIST_FR,    /* fuzzy */
/* it */  IDS_CANNOT_WRITE_FLIP_LIST_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_WRITE_FLIP_LIST_NL,
/* pl */  IDS_CANNOT_WRITE_FLIP_LIST_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_WRITE_FLIP_LIST_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SELECT_KEYMAP_FILE,
/* de */  IDS_SELECT_KEYMAP_FILE_DE,    /* fuzzy */
/* fr */  IDS_SELECT_KEYMAP_FILE_FR,    /* fuzzy */
/* it */  IDS_SELECT_KEYMAP_FILE_IT,    /* fuzzy */
/* nl */  IDS_SELECT_KEYMAP_FILE_NL,
/* pl */  IDS_SELECT_KEYMAP_FILE_PL,    /* fuzzy */
/* sv */  IDS_SELECT_KEYMAP_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SAVE_KEYMAP_FILE,
/* de */  IDS_SAVE_KEYMAP_FILE_DE,    /* fuzzy */
/* fr */  IDS_SAVE_KEYMAP_FILE_FR,    /* fuzzy */
/* it */  IDS_SAVE_KEYMAP_FILE_IT,    /* fuzzy */
/* nl */  IDS_SAVE_KEYMAP_FILE_NL,
/* pl */  IDS_SAVE_KEYMAP_FILE_PL,    /* fuzzy */
/* sv */  IDS_SAVE_KEYMAP_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_WRITE_KEYMAP_FILE,
/* de */  IDS_CANNOT_WRITE_KEYMAP_FILE_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_WRITE_KEYMAP_FILE_FR,    /* fuzzy */
/* it */  IDS_CANNOT_WRITE_KEYMAP_FILE_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_WRITE_KEYMAP_FILE_NL,
/* pl */  IDS_CANNOT_WRITE_KEYMAP_FILE_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_WRITE_KEYMAP_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_MAPPING,
/* de */  IDS_MAPPING_DE,    /* fuzzy */
/* fr */  IDS_MAPPING_FR,    /* fuzzy */
/* it */  IDS_MAPPING_IT,    /* fuzzy */
/* nl */  IDS_MAPPING_NL,
/* pl */  IDS_MAPPING_PL,    /* fuzzy */
/* sv */  IDS_MAPPING_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_KEYBOARD_SETTINGS,
/* de */  IDS_KEYBOARD_SETTINGS_DE,    /* fuzzy */
/* fr */  IDS_KEYBOARD_SETTINGS_FR,    /* fuzzy */
/* it */  IDS_KEYBOARD_SETTINGS_IT,    /* fuzzy */
/* nl */  IDS_KEYBOARD_SETTINGS_NL,
/* pl */  IDS_KEYBOARD_SETTINGS_PL,    /* fuzzy */
/* sv */  IDS_KEYBOARD_SETTINGS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH,
/* de */  IDS_ATTACH_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_FR,    /* fuzzy */
/* it */  IDS_ATTACH_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_NL,
/* pl */  IDS_ATTACH_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PLEASE_ENTER_A_FILENAME,
/* de */  IDS_PLEASE_ENTER_A_FILENAME_DE,    /* fuzzy */
/* fr */  IDS_PLEASE_ENTER_A_FILENAME_FR,    /* fuzzy */
/* it */  IDS_PLEASE_ENTER_A_FILENAME_IT,    /* fuzzy */
/* nl */  IDS_PLEASE_ENTER_A_FILENAME_NL,
/* pl */  IDS_PLEASE_ENTER_A_FILENAME_PL,    /* fuzzy */
/* sv */  IDS_PLEASE_ENTER_A_FILENAME_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_OVERWRITE_EXISTING_IMAGE,
/* de */  IDS_OVERWRITE_EXISTING_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_OVERWRITE_EXISTING_IMAGE_FR,    /* fuzzy */
/* it */  IDS_OVERWRITE_EXISTING_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_OVERWRITE_EXISTING_IMAGE_NL,
/* pl */  IDS_OVERWRITE_EXISTING_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_OVERWRITE_EXISTING_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_CREATE_IMAGE,
/* de */  IDS_CANNOT_CREATE_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_CREATE_IMAGE_FR,    /* fuzzy */
/* it */  IDS_CANNOT_CREATE_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_CREATE_IMAGE_NL,
/* pl */  IDS_CANNOT_CREATE_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_CREATE_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_COMMAND_LINE_OPTIONS,
/* de */  IDS_COMMAND_LINE_OPTIONS_DE,    /* fuzzy */
/* fr */  IDS_COMMAND_LINE_OPTIONS_FR,    /* fuzzy */
/* it */  IDS_COMMAND_LINE_OPTIONS_IT,    /* fuzzy */
/* nl */  IDS_COMMAND_LINE_OPTIONS_NL,
/* pl */  IDS_COMMAND_LINE_OPTIONS_PL,    /* fuzzy */
/* sv */  IDS_COMMAND_LINE_OPTIONS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_COMMAND_OPTIONS_AVAIL,
/* de */  IDS_COMMAND_OPTIONS_AVAIL_DE,    /* fuzzy */
/* fr */  IDS_COMMAND_OPTIONS_AVAIL_FR,    /* fuzzy */
/* it */  IDS_COMMAND_OPTIONS_AVAIL_IT,    /* fuzzy */
/* nl */  IDS_COMMAND_OPTIONS_AVAIL_NL,
/* pl */  IDS_COMMAND_OPTIONS_AVAIL_PL,    /* fuzzy */
/* sv */  IDS_COMMAND_OPTIONS_AVAIL_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_NO_DRIVER_SELECT_SUPPORT,
/* de */  IDS_NO_DRIVER_SELECT_SUPPORT_DE,    /* fuzzy */
/* fr */  IDS_NO_DRIVER_SELECT_SUPPORT_FR,    /* fuzzy */
/* it */  IDS_NO_DRIVER_SELECT_SUPPORT_IT,    /* fuzzy */
/* nl */  IDS_NO_DRIVER_SELECT_SUPPORT_NL,
/* pl */  IDS_NO_DRIVER_SELECT_SUPPORT_PL,    /* fuzzy */
/* sv */  IDS_NO_DRIVER_SELECT_SUPPORT_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANT_WRITE_SCREENSHOT_S,
/* de */  IDS_CANT_WRITE_SCREENSHOT_S_DE,    /* fuzzy */
/* fr */  IDS_CANT_WRITE_SCREENSHOT_S_FR,    /* fuzzy */
/* it */  IDS_CANT_WRITE_SCREENSHOT_S_IT,    /* fuzzy */
/* nl */  IDS_CANT_WRITE_SCREENSHOT_S_NL,
/* pl */  IDS_CANT_WRITE_SCREENSHOT_S_PL,    /* fuzzy */
/* sv */  IDS_CANT_WRITE_SCREENSHOT_S_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_AUTOSTART_DISK_IMAGE,
/* de */  IDS_AUTOSTART_DISK_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_AUTOSTART_DISK_IMAGE_FR,    /* fuzzy */
/* it */  IDS_AUTOSTART_DISK_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_AUTOSTART_DISK_IMAGE_NL,
/* pl */  IDS_AUTOSTART_DISK_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_AUTOSTART_DISK_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SELECT_FS_DIRECTORY,
/* de */  IDS_SELECT_FS_DIRECTORY_DE,    /* fuzzy */
/* fr */  IDS_SELECT_FS_DIRECTORY_FR,    /* fuzzy */
/* it */  IDS_SELECT_FS_DIRECTORY_IT,    /* fuzzy */
/* nl */  IDS_SELECT_FS_DIRECTORY_NL,
/* pl */  IDS_SELECT_FS_DIRECTORY_PL,    /* fuzzy */
/* sv */  IDS_SELECT_FS_DIRECTORY_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRINTER_USERPORT,
/* de */  IDS_PRINTER_USERPORT_DE,    /* fuzzy */
/* fr */  IDS_PRINTER_USERPORT_FR,    /* fuzzy */
/* it */  IDS_PRINTER_USERPORT_IT,    /* fuzzy */
/* nl */  IDS_PRINTER_USERPORT_NL,
/* pl */  IDS_PRINTER_USERPORT_PL,    /* fuzzy */
/* sv */  IDS_PRINTER_USERPORT_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRINTER_4,
/* de */  IDS_PRINTER_4_DE,    /* fuzzy */
/* fr */  IDS_PRINTER_4_FR,    /* fuzzy */
/* it */  IDS_PRINTER_4_IT,    /* fuzzy */
/* nl */  IDS_PRINTER_4_NL,
/* pl */  IDS_PRINTER_4_PL,    /* fuzzy */
/* sv */  IDS_PRINTER_4_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRINTER_5,
/* de */  IDS_PRINTER_5_DE,    /* fuzzy */
/* fr */  IDS_PRINTER_5_FR,    /* fuzzy */
/* it */  IDS_PRINTER_5_IT,    /* fuzzy */
/* nl */  IDS_PRINTER_5_NL,
/* pl */  IDS_PRINTER_5_PL,    /* fuzzy */
/* sv */  IDS_PRINTER_5_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DRIVE_8,
/* de */  IDS_DRIVE_8_DE,    /* fuzzy */
/* fr */  IDS_DRIVE_8_FR,    /* fuzzy */
/* it */  IDS_DRIVE_8_IT,    /* fuzzy */
/* nl */  IDS_DRIVE_8_NL,
/* pl */  IDS_DRIVE_8_PL,    /* fuzzy */
/* sv */  IDS_DRIVE_8_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DRIVE_9,
/* de */  IDS_DRIVE_9_DE,    /* fuzzy */
/* fr */  IDS_DRIVE_9_FR,    /* fuzzy */
/* it */  IDS_DRIVE_9_IT,    /* fuzzy */
/* nl */  IDS_DRIVE_9_NL,
/* pl */  IDS_DRIVE_9_PL,    /* fuzzy */
/* sv */  IDS_DRIVE_9_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DRIVE_10,
/* de */  IDS_DRIVE_10_DE,    /* fuzzy */
/* fr */  IDS_DRIVE_10_FR,    /* fuzzy */
/* it */  IDS_DRIVE_10_IT,    /* fuzzy */
/* nl */  IDS_DRIVE_10_NL,
/* pl */  IDS_DRIVE_10_PL,    /* fuzzy */
/* sv */  IDS_DRIVE_10_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DRIVE_11,
/* de */  IDS_DRIVE_11_DE,    /* fuzzy */
/* fr */  IDS_DRIVE_11_FR,    /* fuzzy */
/* it */  IDS_DRIVE_11_IT,    /* fuzzy */
/* nl */  IDS_DRIVE_11_NL,
/* pl */  IDS_DRIVE_11_PL,    /* fuzzy */
/* sv */  IDS_DRIVE_11_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PERIPHERAL_SETTINGS,
/* de */  IDS_PERIPHERAL_SETTINGS_DE,    /* fuzzy */
/* fr */  IDS_PERIPHERAL_SETTINGS_FR,    /* fuzzy */
/* it */  IDS_PERIPHERAL_SETTINGS_IT,    /* fuzzy */
/* nl */  IDS_PERIPHERAL_SETTINGS_NL,
/* pl */  IDS_PERIPHERAL_SETTINGS_PL,    /* fuzzy */
/* sv */  IDS_PERIPHERAL_SETTINGS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANT_WRITE_SNAPSHOT_FILE,
/* de */  IDS_CANT_WRITE_SNAPSHOT_FILE_DE,    /* fuzzy */
/* fr */  IDS_CANT_WRITE_SNAPSHOT_FILE_FR,    /* fuzzy */
/* it */  IDS_CANT_WRITE_SNAPSHOT_FILE_IT,    /* fuzzy */
/* nl */  IDS_CANT_WRITE_SNAPSHOT_FILE_NL,
/* pl */  IDS_CANT_WRITE_SNAPSHOT_FILE_PL,    /* fuzzy */
/* sv */  IDS_CANT_WRITE_SNAPSHOT_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_READ_SNAPSHOT_IMG,
/* de */  IDS_CANNOT_READ_SNAPSHOT_IMG_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_READ_SNAPSHOT_IMG_FR,    /* fuzzy */
/* it */  IDS_CANNOT_READ_SNAPSHOT_IMG_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_READ_SNAPSHOT_IMG_NL,
/* pl */  IDS_CANNOT_READ_SNAPSHOT_IMG_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_READ_SNAPSHOT_IMG_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_LOAD_S_ROM_IMAGE,
/* de */  IDS_LOAD_S_ROM_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_LOAD_S_ROM_IMAGE_FR,    /* fuzzy */
/* it */  IDS_LOAD_S_ROM_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_LOAD_S_ROM_IMAGE_NL,
/* pl */  IDS_LOAD_S_ROM_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_LOAD_S_ROM_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SELECT_ROMSET_ARCHIVE,
/* de */  IDS_SELECT_ROMSET_ARCHIVE_DE,    /* fuzzy */
/* fr */  IDS_SELECT_ROMSET_ARCHIVE_FR,    /* fuzzy */
/* it */  IDS_SELECT_ROMSET_ARCHIVE_IT,    /* fuzzy */
/* nl */  IDS_SELECT_ROMSET_ARCHIVE_NL,
/* pl */  IDS_SELECT_ROMSET_ARCHIVE_PL,    /* fuzzy */
/* sv */  IDS_SELECT_ROMSET_ARCHIVE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_LOAD_ROMSET_ARCH,
/* de */  IDS_CANNOT_LOAD_ROMSET_ARCH_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_LOAD_ROMSET_ARCH_FR,    /* fuzzy */
/* it */  IDS_CANNOT_LOAD_ROMSET_ARCH_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_LOAD_ROMSET_ARCH_NL,
/* pl */  IDS_CANNOT_LOAD_ROMSET_ARCH_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_LOAD_ROMSET_ARCH_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_SAVE_ROMSET_ARCH,
/* de */  IDS_CANNOT_SAVE_ROMSET_ARCH_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_SAVE_ROMSET_ARCH_FR,    /* fuzzy */
/* it */  IDS_CANNOT_SAVE_ROMSET_ARCH_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_SAVE_ROMSET_ARCH_NL,
/* pl */  IDS_CANNOT_SAVE_ROMSET_ARCH_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_SAVE_ROMSET_ARCH_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_LOAD_ROMSET_FILE,
/* de */  IDS_CANNOT_LOAD_ROMSET_FILE_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_LOAD_ROMSET_FILE_FR,    /* fuzzy */
/* it */  IDS_CANNOT_LOAD_ROMSET_FILE_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_LOAD_ROMSET_FILE_NL,
/* pl */  IDS_CANNOT_LOAD_ROMSET_FILE_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_LOAD_ROMSET_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_SAVE_ROMSET_FILE,
/* de */  IDS_CANNOT_SAVE_ROMSET_FILE_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_SAVE_ROMSET_FILE_FR,    /* fuzzy */
/* it */  IDS_CANNOT_SAVE_ROMSET_FILE_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_SAVE_ROMSET_FILE_NL,
/* pl */  IDS_CANNOT_SAVE_ROMSET_FILE_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_SAVE_ROMSET_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SELECT_ROMSET_FILE,
/* de */  IDS_SELECT_ROMSET_FILE_DE,    /* fuzzy */
/* fr */  IDS_SELECT_ROMSET_FILE_FR,    /* fuzzy */
/* it */  IDS_SELECT_ROMSET_FILE_IT,    /* fuzzy */
/* nl */  IDS_SELECT_ROMSET_FILE_NL,
/* pl */  IDS_SELECT_ROMSET_FILE_PL,    /* fuzzy */
/* sv */  IDS_SELECT_ROMSET_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ROMSET,
/* de */  IDS_ROMSET_DE,    /* fuzzy */
/* fr */  IDS_ROMSET_FR,    /* fuzzy */
/* it */  IDS_ROMSET_IT,    /* fuzzy */
/* nl */  IDS_ROMSET_NL,
/* pl */  IDS_ROMSET_PL,    /* fuzzy */
/* sv */  IDS_ROMSET_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_COMPUTER,
/* de */  IDS_COMPUTER_DE,    /* fuzzy */
/* fr */  IDS_COMPUTER_FR,    /* fuzzy */
/* it */  IDS_COMPUTER_IT,    /* fuzzy */
/* nl */  IDS_COMPUTER_NL,
/* pl */  IDS_COMPUTER_PL,    /* fuzzy */
/* sv */  IDS_COMPUTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DRIVE,
/* de */  IDS_DRIVE_DE,    /* fuzzy */
/* fr */  IDS_DRIVE_FR,    /* fuzzy */
/* it */  IDS_DRIVE_IT,    /* fuzzy */
/* nl */  IDS_DRIVE_NL,
/* pl */  IDS_DRIVE_PL,    /* fuzzy */
/* sv */  IDS_DRIVE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ROM_SETTINGS,
/* de */  IDS_ROM_SETTINGS_DE,    /* fuzzy */
/* fr */  IDS_ROM_SETTINGS_FR,    /* fuzzy */
/* it */  IDS_ROM_SETTINGS_IT,    /* fuzzy */
/* nl */  IDS_ROM_SETTINGS_NL,
/* pl */  IDS_ROM_SETTINGS_PL,    /* fuzzy */
/* sv */  IDS_ROM_SETTINGS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SAVE_SNAPSHOT_IMAGE,
/* de */  IDS_SAVE_SNAPSHOT_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_SAVE_SNAPSHOT_IMAGE_FR,    /* fuzzy */
/* it */  IDS_SAVE_SNAPSHOT_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_SAVE_SNAPSHOT_IMAGE_NL,
/* pl */  IDS_SAVE_SNAPSHOT_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_SAVE_SNAPSHOT_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CANNOT_WRITE_SNAPSHOT_S,
/* de */  IDS_CANNOT_WRITE_SNAPSHOT_S_DE,    /* fuzzy */
/* fr */  IDS_CANNOT_WRITE_SNAPSHOT_S_FR,    /* fuzzy */
/* it */  IDS_CANNOT_WRITE_SNAPSHOT_S_IT,    /* fuzzy */
/* nl */  IDS_CANNOT_WRITE_SNAPSHOT_S_NL,
/* pl */  IDS_CANNOT_WRITE_SNAPSHOT_S_PL,    /* fuzzy */
/* sv */  IDS_CANNOT_WRITE_SNAPSHOT_S_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_LOAD_SNAPSHOT_IMAGE,
/* de */  IDS_LOAD_SNAPSHOT_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_LOAD_SNAPSHOT_IMAGE_FR,    /* fuzzy */
/* it */  IDS_LOAD_SNAPSHOT_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_LOAD_SNAPSHOT_IMAGE_NL,
/* pl */  IDS_LOAD_SNAPSHOT_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_LOAD_SNAPSHOT_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_COULD_NOT_LOAD_PALETTE,
/* de */  IDS_COULD_NOT_LOAD_PALETTE_DE,    /* fuzzy */
/* fr */  IDS_COULD_NOT_LOAD_PALETTE_FR,    /* fuzzy */
/* it */  IDS_COULD_NOT_LOAD_PALETTE_IT,    /* fuzzy */
/* nl */  IDS_COULD_NOT_LOAD_PALETTE_NL,
/* pl */  IDS_COULD_NOT_LOAD_PALETTE_PL,    /* fuzzy */
/* sv */  IDS_COULD_NOT_LOAD_PALETTE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_LOAD_VICE_PALETTE_FILE,
/* de */  IDS_LOAD_VICE_PALETTE_FILE_DE,    /* fuzzy */
/* fr */  IDS_LOAD_VICE_PALETTE_FILE_FR,    /* fuzzy */
/* it */  IDS_LOAD_VICE_PALETTE_FILE_IT,    /* fuzzy */
/* nl */  IDS_LOAD_VICE_PALETTE_FILE_NL,
/* pl */  IDS_LOAD_VICE_PALETTE_FILE_PL,    /* fuzzy */
/* sv */  IDS_LOAD_VICE_PALETTE_FILE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_FULLSCREEN,
/* de */  IDS_FULLSCREEN_DE,    /* fuzzy */
/* fr */  IDS_FULLSCREEN_FR,    /* fuzzy */
/* it */  IDS_FULLSCREEN_IT,    /* fuzzy */
/* nl */  IDS_FULLSCREEN_NL,
/* pl */  IDS_FULLSCREEN_PL,    /* fuzzy */
/* sv */  IDS_FULLSCREEN_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_COLORS,
/* de */  IDS_COLORS_DE,    /* fuzzy */
/* fr */  IDS_COLORS_FR,    /* fuzzy */
/* it */  IDS_COLORS_IT,    /* fuzzy */
/* nl */  IDS_COLORS_NL,
/* pl */  IDS_COLORS_PL,    /* fuzzy */
/* sv */  IDS_COLORS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_VIDEO_SETTINGS,
/* de */  IDS_VIDEO_SETTINGS_DE,    /* fuzzy */
/* fr */  IDS_VIDEO_SETTINGS_FR,    /* fuzzy */
/* it */  IDS_VIDEO_SETTINGS_IT,    /* fuzzy */
/* nl */  IDS_VIDEO_SETTINGS_NL,
/* pl */  IDS_VIDEO_SETTINGS_PL,    /* fuzzy */
/* sv */  IDS_VIDEO_SETTINGS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_FILE_SYSTEM,
/* de */  IDS_FILE_SYSTEM_DE,    /* fuzzy */
/* fr */  IDS_FILE_SYSTEM_FR,    /* fuzzy */
/* it */  IDS_FILE_SYSTEM_IT,    /* fuzzy */
/* nl */  IDS_FILE_SYSTEM_NL,
/* pl */  IDS_FILE_SYSTEM_PL,    /* fuzzy */
/* sv */  IDS_FILE_SYSTEM_SV},   /* fuzzy */

#ifdef HAVE_OPENCBM
/* res.rc */
/* en */ {IDS_REAL_IEC_DEVICE,
/* de */  IDS_REAL_IEC_DEVICE_DE,    /* fuzzy */
/* fr */  IDS_REAL_IEC_DEVICE_FR,    /* fuzzy */
/* it */  IDS_REAL_IEC_DEVICE_IT,    /* fuzzy */
/* nl */  IDS_REAL_IEC_DEVICE_NL,
/* pl */  IDS_REAL_IEC_DEVICE_PL,    /* fuzzy */
/* sv */  IDS_REAL_IEC_DEVICE_SV},   /* fuzzy */
#endif

/* resc128.rc */
/* en */ {IDS_SELECT_INT_FUNCTION_ROM,
/* de */  IDS_SELECT_INT_FUNCTION_ROM_DE,    /* fuzzy */
/* fr */  IDS_SELECT_INT_FUNCTION_ROM_FR,    /* fuzzy */
/* it */  IDS_SELECT_INT_FUNCTION_ROM_IT,    /* fuzzy */
/* nl */  IDS_SELECT_INT_FUNCTION_ROM_NL,
/* pl */  IDS_SELECT_INT_FUNCTION_ROM_PL,    /* fuzzy */
/* sv */  IDS_SELECT_INT_FUNCTION_ROM_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDS_SELECT_EXT_FUNCTION_ROM,
/* de */  IDS_SELECT_EXT_FUNCTION_ROM_DE,    /* fuzzy */
/* fr */  IDS_SELECT_EXT_FUNCTION_ROM_FR,    /* fuzzy */
/* it */  IDS_SELECT_EXT_FUNCTION_ROM_IT,    /* fuzzy */
/* nl */  IDS_SELECT_EXT_FUNCTION_ROM_NL,
/* pl */  IDS_SELECT_EXT_FUNCTION_ROM_PL,    /* fuzzy */
/* sv */  IDS_SELECT_EXT_FUNCTION_ROM_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDS_MACHINE_TYPE,
/* de */  IDS_MACHINE_TYPE_DE,    /* fuzzy */
/* fr */  IDS_MACHINE_TYPE_FR,    /* fuzzy */
/* it */  IDS_MACHINE_TYPE_IT,    /* fuzzy */
/* nl */  IDS_MACHINE_TYPE_NL,
/* pl */  IDS_MACHINE_TYPE_PL,    /* fuzzy */
/* sv */  IDS_MACHINE_TYPE_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDS_FUNCTION_ROM,
/* de */  IDS_FUNCTION_ROM_DE,    /* fuzzy */
/* fr */  IDS_FUNCTION_ROM_FR,    /* fuzzy */
/* it */  IDS_FUNCTION_ROM_IT,    /* fuzzy */
/* nl */  IDS_FUNCTION_ROM_NL,
/* pl */  IDS_FUNCTION_ROM_PL,    /* fuzzy */
/* sv */  IDS_FUNCTION_ROM_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDS_C128_SETTINGS,
/* de */  IDS_C128_SETTINGS_DE,    /* fuzzy */
/* fr */  IDS_C128_SETTINGS_FR,    /* fuzzy */
/* it */  IDS_C128_SETTINGS_IT,    /* fuzzy */
/* nl */  IDS_C128_SETTINGS_NL,
/* pl */  IDS_C128_SETTINGS_PL,    /* fuzzy */
/* sv */  IDS_C128_SETTINGS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_CRT_CART_IMAGE,
/* de */  IDS_ATTACH_CRT_CART_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_CRT_CART_IMAGE_FR,    /* fuzzy */
/* it */  IDS_ATTACH_CRT_CART_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_CRT_CART_IMAGE_NL,
/* pl */  IDS_ATTACH_CRT_CART_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_CRT_CART_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_RAW_8KB_CART_IMAGE,
/* de */  IDS_ATTACH_RAW_8KB_CART_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_RAW_8KB_CART_IMAGE_FR,    /* fuzzy */
/* it */  IDS_ATTACH_RAW_8KB_CART_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_RAW_8KB_CART_IMAGE_NL,
/* pl */  IDS_ATTACH_RAW_8KB_CART_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_RAW_8KB_CART_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_RAW_16KB_CART_IMG,
/* de */  IDS_ATTACH_RAW_16KB_CART_IMG_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_RAW_16KB_CART_IMG_FR,    /* fuzzy */
/* it */  IDS_ATTACH_RAW_16KB_CART_IMG_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_RAW_16KB_CART_IMG_NL,
/* pl */  IDS_ATTACH_RAW_16KB_CART_IMG_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_RAW_16KB_CART_IMG_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_AR_CART_IMAGE,
/* de */  IDS_ATTACH_AR_CART_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_AR_CART_IMAGE_FR,    /* fuzzy */
/* it */  IDS_ATTACH_AR_CART_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_AR_CART_IMAGE_NL,
/* pl */  IDS_ATTACH_AR_CART_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_AR_CART_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_ATOMIC_P_CART_IMG,
/* de */  IDS_ATTACH_ATOMIC_P_CART_IMG_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_ATOMIC_P_CART_IMG_FR,    /* fuzzy */
/* it */  IDS_ATTACH_ATOMIC_P_CART_IMG_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_ATOMIC_P_CART_IMG_NL,
/* pl */  IDS_ATTACH_ATOMIC_P_CART_IMG_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_ATOMIC_P_CART_IMG_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_EPYX_FL_CART_IMG,
/* de */  IDS_ATTACH_EPYX_FL_CART_IMG_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_EPYX_FL_CART_IMG_FR,    /* fuzzy */
/* it */  IDS_ATTACH_EPYX_FL_CART_IMG_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_EPYX_FL_CART_IMG_NL,
/* pl */  IDS_ATTACH_EPYX_FL_CART_IMG_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_EPYX_FL_CART_IMG_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_IEEE488_CART_IMG,
/* de */  IDS_ATTACH_IEEE488_CART_IMG_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_IEEE488_CART_IMG_FR,    /* fuzzy */
/* it */  IDS_ATTACH_IEEE488_CART_IMG_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_IEEE488_CART_IMG_NL,
/* pl */  IDS_ATTACH_IEEE488_CART_IMG_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_IEEE488_CART_IMG_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_RETRO_R_CART_IMG,
/* de */  IDS_ATTACH_RETRO_R_CART_IMG_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_RETRO_R_CART_IMG_FR,    /* fuzzy */
/* it */  IDS_ATTACH_RETRO_R_CART_IMG_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_RETRO_R_CART_IMG_NL,
/* pl */  IDS_ATTACH_RETRO_R_CART_IMG_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_RETRO_R_CART_IMG_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_IDE64_CART_IMAGE,
/* de */  IDS_ATTACH_IDE64_CART_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_IDE64_CART_IMAGE_FR,    /* fuzzy */
/* it */  IDS_ATTACH_IDE64_CART_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_IDE64_CART_IMAGE_NL,
/* pl */  IDS_ATTACH_IDE64_CART_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_IDE64_CART_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_SS4_CART_IMAGE,
/* de */  IDS_ATTACH_SS4_CART_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_SS4_CART_IMAGE_FR,    /* fuzzy */
/* it */  IDS_ATTACH_SS4_CART_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_SS4_CART_IMAGE_NL,
/* pl */  IDS_ATTACH_SS4_CART_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_SS4_CART_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ATTACH_SS5_CART_IMAGE,
/* de */  IDS_ATTACH_SS5_CART_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_SS5_CART_IMAGE_FR,    /* fuzzy */
/* it */  IDS_ATTACH_SS5_CART_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_SS5_CART_IMAGE_NL,
/* pl */  IDS_ATTACH_SS5_CART_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_SS5_CART_IMAGE_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDS_ATTACH_FUNCTION_LOW_CART,
/* de */  IDS_ATTACH_FUNCTION_LOW_CART_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_FUNCTION_LOW_CART_FR,    /* fuzzy */
/* it */  IDS_ATTACH_FUNCTION_LOW_CART_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_FUNCTION_LOW_CART_NL,
/* pl */  IDS_ATTACH_FUNCTION_LOW_CART_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_FUNCTION_LOW_CART_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDS_ATTACH_FUNCTION_HIGH_CART,
/* de */  IDS_ATTACH_FUNCTION_HIGH_CART_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_FUNCTION_HIGH_CART_FR,    /* fuzzy */
/* it */  IDS_ATTACH_FUNCTION_HIGH_CART_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_FUNCTION_HIGH_CART_NL,
/* pl */  IDS_ATTACH_FUNCTION_HIGH_CART_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_FUNCTION_HIGH_CART_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDS_ATTACH_CART1_LOW,
/* de */  IDS_ATTACH_CART1_LOW_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_CART1_LOW_FR,    /* fuzzy */
/* it */  IDS_ATTACH_CART1_LOW_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_CART1_LOW_NL,
/* pl */  IDS_ATTACH_CART1_LOW_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_CART1_LOW_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDS_ATTACH_CART1_HIGH,
/* de */  IDS_ATTACH_CART1_HIGH_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_CART1_HIGH_FR,    /* fuzzy */
/* it */  IDS_ATTACH_CART1_HIGH_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_CART1_HIGH_NL,
/* pl */  IDS_ATTACH_CART1_HIGH_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_CART1_HIGH_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDS_ATTACH_CART2_LOW,
/* de */  IDS_ATTACH_CART2_LOW_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_CART2_LOW_FR,    /* fuzzy */
/* it */  IDS_ATTACH_CART2_LOW_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_CART2_LOW_NL,
/* pl */  IDS_ATTACH_CART2_LOW_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_CART2_LOW_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDS_ATTACH_CART2_HIGH,
/* de */  IDS_ATTACH_CART2_HIGH_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_CART2_HIGH_FR,    /* fuzzy */
/* it */  IDS_ATTACH_CART2_HIGH_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_CART2_HIGH_NL,
/* pl */  IDS_ATTACH_CART2_HIGH_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_CART2_HIGH_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDS_ATTACH_4_8_16_CART_2000,
/* de */  IDS_ATTACH_4_8_16_CART_2000_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_4_8_16_CART_2000_FR,    /* fuzzy */
/* it */  IDS_ATTACH_4_8_16_CART_2000_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_4_8_16_CART_2000_NL,
/* pl */  IDS_ATTACH_4_8_16_CART_2000_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_4_8_16_CART_2000_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDS_ATTACH_4_8_16_CART_4000,
/* de */  IDS_ATTACH_4_8_16_CART_4000_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_4_8_16_CART_4000_FR,    /* fuzzy */
/* it */  IDS_ATTACH_4_8_16_CART_4000_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_4_8_16_CART_4000_NL,
/* pl */  IDS_ATTACH_4_8_16_CART_4000_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_4_8_16_CART_4000_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDS_ATTACH_4_8_16_CART_6000,
/* de */  IDS_ATTACH_4_8_16_CART_6000_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_4_8_16_CART_6000_FR,    /* fuzzy */
/* it */  IDS_ATTACH_4_8_16_CART_6000_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_4_8_16_CART_6000_NL,
/* pl */  IDS_ATTACH_4_8_16_CART_6000_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_4_8_16_CART_6000_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDS_ATTACH_4_8_CART_A000,
/* de */  IDS_ATTACH_4_8_CART_A000_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_4_8_CART_A000_FR,    /* fuzzy */
/* it */  IDS_ATTACH_4_8_CART_A000_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_4_8_CART_A000_NL,
/* pl */  IDS_ATTACH_4_8_CART_A000_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_4_8_CART_A000_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDS_ATTACH_4_CART_B000,
/* de */  IDS_ATTACH_4_CART_B000_DE,    /* fuzzy */
/* fr */  IDS_ATTACH_4_CART_B000_FR,    /* fuzzy */
/* it */  IDS_ATTACH_4_CART_B000_IT,    /* fuzzy */
/* nl */  IDS_ATTACH_4_CART_B000_NL,
/* pl */  IDS_ATTACH_4_CART_B000_PL,    /* fuzzy */
/* sv */  IDS_ATTACH_4_CART_B000_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_MODEL,
/* de */  IDS_MODEL_DE,    /* fuzzy */
/* fr */  IDS_MODEL_FR,    /* fuzzy */
/* it */  IDS_MODEL_IT,    /* fuzzy */
/* nl */  IDS_MODEL_NL,
/* pl */  IDS_MODEL_PL,    /* fuzzy */
/* sv */  IDS_MODEL_SV},   /* fuzzy */

/* rescbm2.rc */
/* en */ {IDS_MEMORY,
/* de */  IDS_MEMORY_DE,    /* fuzzy */
/* fr */  IDS_MEMORY_FR,    /* fuzzy */
/* it */  IDS_MEMORY_IT,    /* fuzzy */
/* nl */  IDS_MEMORY_NL,
/* pl */  IDS_MEMORY_PL,    /* fuzzy */
/* sv */  IDS_MEMORY_SV},   /* fuzzy */

/* rescbm2.rc */
/* en */ {IDS_CBM2_SETTINGS,
/* de */  IDS_CBM2_SETTINGS_DE,    /* fuzzy */
/* fr */  IDS_CBM2_SETTINGS_FR,    /* fuzzy */
/* it */  IDS_CBM2_SETTINGS_IT,    /* fuzzy */
/* nl */  IDS_CBM2_SETTINGS_NL,
/* pl */  IDS_CBM2_SETTINGS_PL,    /* fuzzy */
/* sv */  IDS_CBM2_SETTINGS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DRIVE_SETTINGS,
/* de */  IDS_DRIVE_SETTINGS_DE,    /* fuzzy */
/* fr */  IDS_DRIVE_SETTINGS_FR,    /* fuzzy */
/* it */  IDS_DRIVE_SETTINGS_IT,    /* fuzzy */
/* nl */  IDS_DRIVE_SETTINGS_NL,
/* pl */  IDS_DRIVE_SETTINGS_PL,    /* fuzzy */
/* sv */  IDS_DRIVE_SETTINGS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SELECT_START_SNAP_EVENT,
/* de */  IDS_SELECT_START_SNAP_EVENT_DE,    /* fuzzy */
/* fr */  IDS_SELECT_START_SNAP_EVENT_FR,    /* fuzzy */
/* it */  IDS_SELECT_START_SNAP_EVENT_IT,    /* fuzzy */
/* nl */  IDS_SELECT_START_SNAP_EVENT_NL,
/* pl */  IDS_SELECT_START_SNAP_EVENT_PL,    /* fuzzy */
/* sv */  IDS_SELECT_START_SNAP_EVENT_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SELECT_END_SNAP_EVENT,
/* de */  IDS_SELECT_END_SNAP_EVENT_DE,    /* fuzzy */
/* fr */  IDS_SELECT_END_SNAP_EVENT_FR,    /* fuzzy */
/* it */  IDS_SELECT_END_SNAP_EVENT_IT,    /* fuzzy */
/* nl */  IDS_SELECT_END_SNAP_EVENT_NL,
/* pl */  IDS_SELECT_END_SNAP_EVENT_PL,    /* fuzzy */
/* sv */  IDS_SELECT_END_SNAP_EVENT_SV},   /* fuzzy */

#ifdef UNSTABLE
/* res.rc */
/* en */ {IDS_VERSION_S_UNSTABLE,
/* de */  IDS_VERSION_S_UNSTABLE_DE,    /* fuzzy */
/* fr */  IDS_VERSION_S_UNSTABLE_FR,    /* fuzzy */
/* it */  IDS_VERSION_S_UNSTABLE_IT,    /* fuzzy */
/* nl */  IDS_VERSION_S_UNSTABLE_NL,
/* pl */  IDS_VERSION_S_UNSTABLE_PL,    /* fuzzy */
/* sv */  IDS_VERSION_S_UNSTABLE_SV},   /* fuzzy */
#else
/* res.rc */
/* en */ {IDS_VERSION_S,
/* de */  IDS_VERSION_S_DE,    /* fuzzy */
/* fr */  IDS_VERSION_S_FR,    /* fuzzy */
/* it */  IDS_VERSION_S_IT,    /* fuzzy */
/* nl */  IDS_VERSION_S_NL,
/* pl */  IDS_VERSION_S_PL,    /* fuzzy */
/* sv */  IDS_VERSION_S_SV},   /* fuzzy */
#endif

/* res.rc */
/* en */ {IDS_VICE_CONTRIBUTORS,
/* de */  IDS_VICE_CONTRIBUTORS_DE,    /* fuzzy */
/* fr */  IDS_VICE_CONTRIBUTORS_FR,    /* fuzzy */
/* it */  IDS_VICE_CONTRIBUTORS_IT,    /* fuzzy */
/* nl */  IDS_VICE_CONTRIBUTORS_NL,
/* pl */  IDS_VICE_CONTRIBUTORS_PL,    /* fuzzy */
/* sv */  IDS_VICE_CONTRIBUTORS_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_WHO_MADE_WHAT,
/* de */  IDS_WHO_MADE_WHAT_DE,    /* fuzzy */
/* fr */  IDS_WHO_MADE_WHAT_FR,    /* fuzzy */
/* it */  IDS_WHO_MADE_WHAT_IT,    /* fuzzy */
/* nl */  IDS_WHO_MADE_WHAT_NL,
/* pl */  IDS_WHO_MADE_WHAT_PL,    /* fuzzy */
/* sv */  IDS_WHO_MADE_WHAT_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_LICENSE,
/* de */  IDS_LICENSE_DE,    /* fuzzy */
/* fr */  IDS_LICENSE_FR,    /* fuzzy */
/* it */  IDS_LICENSE_IT,    /* fuzzy */
/* nl */  IDS_LICENSE_NL,
/* pl */  IDS_LICENSE_PL,    /* fuzzy */
/* sv */  IDS_LICENSE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_NO_WARRANTY,
/* de */  IDS_NO_WARRANTY_DE,    /* fuzzy */
/* fr */  IDS_NO_WARRANTY_FR,    /* fuzzy */
/* it */  IDS_NO_WARRANTY_IT,    /* fuzzy */
/* nl */  IDS_NO_WARRANTY_NL,
/* pl */  IDS_NO_WARRANTY_PL,    /* fuzzy */
/* sv */  IDS_NO_WARRANTY_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_VICE_WITHOUT_WARRANTY,
/* de */  IDS_VICE_WITHOUT_WARRANTY_DE,    /* fuzzy */
/* fr */  IDS_VICE_WITHOUT_WARRANTY_FR,    /* fuzzy */
/* it */  IDS_VICE_WITHOUT_WARRANTY_IT,    /* fuzzy */
/* nl */  IDS_VICE_WITHOUT_WARRANTY_NL,
/* pl */  IDS_VICE_WITHOUT_WARRANTY_PL,    /* fuzzy */
/* sv */  IDS_VICE_WITHOUT_WARRANTY_SV},   /* fuzzy */

/* reside64.rc */
/* en */ {IDS_TOTAL_SIZE_I_KB,
/* de */  IDS_TOTAL_SIZE_I_KB_DE,    /* fuzzy */
/* fr */  IDS_TOTAL_SIZE_I_KB_FR,    /* fuzzy */
/* it */  IDS_TOTAL_SIZE_I_KB_IT,    /* fuzzy */
/* nl */  IDS_TOTAL_SIZE_I_KB_NL,
/* pl */  IDS_TOTAL_SIZE_I_KB_PL,    /* fuzzy */
/* sv */  IDS_TOTAL_SIZE_I_KB_SV},   /* fuzzy */

/* reside64.rc */
/* en */ {IDS_SELECT_HD_IMAGE,
/* de */  IDS_SELECT_HD_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_SELECT_HD_IMAGE_FR,    /* fuzzy */
/* it */  IDS_SELECT_HD_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_SELECT_HD_IMAGE_NL,
/* pl */  IDS_SELECT_HD_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_SELECT_HD_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRESS_KEY_NORTHWEST,
/* de */  IDS_PRESS_KEY_NORTHWEST_DE,    /* fuzzy */
/* fr */  IDS_PRESS_KEY_NORTHWEST_FR,    /* fuzzy */
/* it */  IDS_PRESS_KEY_NORTHWEST_IT,    /* fuzzy */
/* nl */  IDS_PRESS_KEY_NORTHWEST_NL,
/* pl */  IDS_PRESS_KEY_NORTHWEST_PL,    /* fuzzy */
/* sv */  IDS_PRESS_KEY_NORTHWEST_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRESS_KEY_NORTH,
/* de */  IDS_PRESS_KEY_NORTH_DE,    /* fuzzy */
/* fr */  IDS_PRESS_KEY_NORTH_FR,    /* fuzzy */
/* it */  IDS_PRESS_KEY_NORTH_IT,    /* fuzzy */
/* nl */  IDS_PRESS_KEY_NORTH_NL,
/* pl */  IDS_PRESS_KEY_NORTH_PL,    /* fuzzy */
/* sv */  IDS_PRESS_KEY_NORTH_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRESS_KEY_NORTHEAST,
/* de */  IDS_PRESS_KEY_NORTHEAST_DE,    /* fuzzy */
/* fr */  IDS_PRESS_KEY_NORTHEAST_FR,    /* fuzzy */
/* it */  IDS_PRESS_KEY_NORTHEAST_IT,    /* fuzzy */
/* nl */  IDS_PRESS_KEY_NORTHEAST_NL,
/* pl */  IDS_PRESS_KEY_NORTHEAST_PL,    /* fuzzy */
/* sv */  IDS_PRESS_KEY_NORTHEAST_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRESS_KEY_EAST,
/* de */  IDS_PRESS_KEY_EAST_DE,    /* fuzzy */
/* fr */  IDS_PRESS_KEY_EAST_FR,    /* fuzzy */
/* it */  IDS_PRESS_KEY_EAST_IT,    /* fuzzy */
/* nl */  IDS_PRESS_KEY_EAST_NL,
/* pl */  IDS_PRESS_KEY_EAST_PL,    /* fuzzy */
/* sv */  IDS_PRESS_KEY_EAST_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRESS_KEY_SOUTHEAST,
/* de */  IDS_PRESS_KEY_SOUTHEAST_DE,    /* fuzzy */
/* fr */  IDS_PRESS_KEY_SOUTHEAST_FR,    /* fuzzy */
/* it */  IDS_PRESS_KEY_SOUTHEAST_IT,    /* fuzzy */
/* nl */  IDS_PRESS_KEY_SOUTHEAST_NL,
/* pl */  IDS_PRESS_KEY_SOUTHEAST_PL,    /* fuzzy */
/* sv */  IDS_PRESS_KEY_SOUTHEAST_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRESS_KEY_SOUTH,
/* de */  IDS_PRESS_KEY_SOUTH_DE,    /* fuzzy */
/* fr */  IDS_PRESS_KEY_SOUTH_FR,    /* fuzzy */
/* it */  IDS_PRESS_KEY_SOUTH_IT,    /* fuzzy */
/* nl */  IDS_PRESS_KEY_SOUTH_NL,
/* pl */  IDS_PRESS_KEY_SOUTH_PL,    /* fuzzy */
/* sv */  IDS_PRESS_KEY_SOUTH_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRESS_KEY_SOUTHWEST,
/* de */  IDS_PRESS_KEY_SOUTHWEST_DE,    /* fuzzy */
/* fr */  IDS_PRESS_KEY_SOUTHWEST_FR,    /* fuzzy */
/* it */  IDS_PRESS_KEY_SOUTHWEST_IT,    /* fuzzy */
/* nl */  IDS_PRESS_KEY_SOUTHWEST_NL,
/* pl */  IDS_PRESS_KEY_SOUTHWEST_PL,    /* fuzzy */
/* sv */  IDS_PRESS_KEY_SOUTHWEST_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRESS_KEY_WEST,
/* de */  IDS_PRESS_KEY_WEST_DE,    /* fuzzy */
/* fr */  IDS_PRESS_KEY_WEST_FR,    /* fuzzy */
/* it */  IDS_PRESS_KEY_WEST_IT,    /* fuzzy */
/* nl */  IDS_PRESS_KEY_WEST_NL,
/* pl */  IDS_PRESS_KEY_WEST_PL,    /* fuzzy */
/* sv */  IDS_PRESS_KEY_WEST_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PRESS_KEY_FIRE,
/* de */  IDS_PRESS_KEY_FIRE_DE,    /* fuzzy */
/* fr */  IDS_PRESS_KEY_FIRE_FR,    /* fuzzy */
/* it */  IDS_PRESS_KEY_FIRE_IT,    /* fuzzy */
/* nl */  IDS_PRESS_KEY_FIRE_NL,
/* pl */  IDS_PRESS_KEY_FIRE_PL,    /* fuzzy */
/* sv */  IDS_PRESS_KEY_FIRE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_NUMPAD_AND_RCTRL,
/* de */  IDS_NUMPAD_AND_RCTRL_DE,    /* fuzzy */
/* fr */  IDS_NUMPAD_AND_RCTRL_FR,    /* fuzzy */
/* it */  IDS_NUMPAD_AND_RCTRL_IT,    /* fuzzy */
/* nl */  IDS_NUMPAD_AND_RCTRL_NL,
/* pl */  IDS_NUMPAD_AND_RCTRL_PL,    /* fuzzy */
/* sv */  IDS_NUMPAD_AND_RCTRL_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_KEYSET_A,
/* de */  IDS_KEYSET_A_DE,    /* fuzzy */
/* fr */  IDS_KEYSET_A_FR,    /* fuzzy */
/* it */  IDS_KEYSET_A_IT,    /* fuzzy */
/* nl */  IDS_KEYSET_A_NL,
/* pl */  IDS_KEYSET_A_PL,    /* fuzzy */
/* sv */  IDS_KEYSET_A_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_KEYSET_B,
/* de */  IDS_KEYSET_B_DE,    /* fuzzy */
/* fr */  IDS_KEYSET_B_FR,    /* fuzzy */
/* it */  IDS_KEYSET_B_IT,    /* fuzzy */
/* nl */  IDS_KEYSET_B_NL,
/* pl */  IDS_KEYSET_B_PL,    /* fuzzy */
/* sv */  IDS_KEYSET_B_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ALL_BUTTONS_AS_FIRE,
/* de */  IDS_ALL_BUTTONS_AS_FIRE_DE,    /* fuzzy */
/* fr */  IDS_ALL_BUTTONS_AS_FIRE_FR,    /* fuzzy */
/* it */  IDS_ALL_BUTTONS_AS_FIRE_IT,    /* fuzzy */
/* nl */  IDS_ALL_BUTTONS_AS_FIRE_NL,
/* pl */  IDS_ALL_BUTTONS_AS_FIRE_PL,    /* fuzzy */
/* sv */  IDS_ALL_BUTTONS_AS_FIRE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_NUMERIC_SEE_ABOVE,
/* de */  IDS_NUMERIC_SEE_ABOVE_DE,    /* fuzzy */
/* fr */  IDS_NUMERIC_SEE_ABOVE_FR,    /* fuzzy */
/* it */  IDS_NUMERIC_SEE_ABOVE_IT,    /* fuzzy */
/* nl */  IDS_NUMERIC_SEE_ABOVE_NL,
/* pl */  IDS_NUMERIC_SEE_ABOVE_PL,    /* fuzzy */
/* sv */  IDS_NUMERIC_SEE_ABOVE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_NO_BUTTON_NO_AUTOFIRE,
/* de */  IDS_NO_BUTTON_NO_AUTOFIRE_DE,    /* fuzzy */
/* fr */  IDS_NO_BUTTON_NO_AUTOFIRE_FR,    /* fuzzy */
/* it */  IDS_NO_BUTTON_NO_AUTOFIRE_IT,    /* fuzzy */
/* nl */  IDS_NO_BUTTON_NO_AUTOFIRE_NL,
/* pl */  IDS_NO_BUTTON_NO_AUTOFIRE_PL,    /* fuzzy */
/* sv */  IDS_NO_BUTTON_NO_AUTOFIRE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ALL_FILES_FILTER,
/* de */  IDS_ALL_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_ALL_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_ALL_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_ALL_FILES_FILTER_NL,
/* pl */  IDS_ALL_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_ALL_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PALETTE_FILES_FILTER,
/* de */  IDS_PALETTE_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_PALETTE_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_PALETTE_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_PALETTE_FILES_FILTER_NL,
/* pl */  IDS_PALETTE_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_PALETTE_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SNAPSHOT_FILES_FILTER,
/* de */  IDS_SNAPSHOT_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_SNAPSHOT_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_SNAPSHOT_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_SNAPSHOT_FILES_FILTER_NL,
/* pl */  IDS_SNAPSHOT_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_SNAPSHOT_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_DISK_IMAGE_FILES_FILTER,
/* de */  IDS_DISK_IMAGE_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_DISK_IMAGE_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_DISK_IMAGE_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_DISK_IMAGE_FILES_FILTER_NL,
/* pl */  IDS_DISK_IMAGE_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_DISK_IMAGE_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_TAPE_IMAGE_FILES_FILTER,
/* de */  IDS_TAPE_IMAGE_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_TAPE_IMAGE_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_TAPE_IMAGE_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_TAPE_IMAGE_FILES_FILTER_NL,
/* pl */  IDS_TAPE_IMAGE_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_TAPE_IMAGE_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ZIPPED_FILES_FILTER,
/* de */  IDS_ZIPPED_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_ZIPPED_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_ZIPPED_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_ZIPPED_FILES_FILTER_NL,
/* pl */  IDS_ZIPPED_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_ZIPPED_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CRT_FILES_FILTER,
/* de */  IDS_CRT_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_CRT_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_CRT_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_CRT_FILES_FILTER_NL,
/* pl */  IDS_CRT_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_CRT_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_RAW_CART_FILES_FILTER,
/* de */  IDS_RAW_CART_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_RAW_CART_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_RAW_CART_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_RAW_CART_FILES_FILTER_NL,
/* pl */  IDS_RAW_CART_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_RAW_CART_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_FLIP_LIST_FILES_FILTER,
/* de */  IDS_FLIP_LIST_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_FLIP_LIST_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_FLIP_LIST_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_FLIP_LIST_FILES_FILTER_NL,
/* pl */  IDS_FLIP_LIST_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_FLIP_LIST_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ROMSET_FILES_FILTER,
/* de */  IDS_ROMSET_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_ROMSET_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_ROMSET_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_ROMSET_FILES_FILTER_NL,
/* pl */  IDS_ROMSET_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_ROMSET_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ROMSET_ARCHIVES_FILTER,
/* de */  IDS_ROMSET_ARCHIVES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_ROMSET_ARCHIVES_FILTER_FR,    /* fuzzy */
/* it */  IDS_ROMSET_ARCHIVES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_ROMSET_ARCHIVES_FILTER_NL,
/* pl */  IDS_ROMSET_ARCHIVES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_ROMSET_ARCHIVES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_KEYMAP_FILES_FILTER,
/* de */  IDS_KEYMAP_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_KEYMAP_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_KEYMAP_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_KEYMAP_FILES_FILTER_NL,
/* pl */  IDS_KEYMAP_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_KEYMAP_FILES_FILTER_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDS_INPUT_OUTPUT,
/* de */  IDS_INPUT_OUTPUT_DE,    /* fuzzy */
/* fr */  IDS_INPUT_OUTPUT_FR,    /* fuzzy */
/* it */  IDS_INPUT_OUTPUT_IT,    /* fuzzy */
/* nl */  IDS_INPUT_OUTPUT_NL,
/* pl */  IDS_INPUT_OUTPUT_PL,    /* fuzzy */
/* sv */  IDS_INPUT_OUTPUT_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDS_PET_SETTINGS,
/* de */  IDS_PET_SETTINGS_DE,    /* fuzzy */
/* fr */  IDS_PET_SETTINGS_FR,    /* fuzzy */
/* it */  IDS_PET_SETTINGS_IT,    /* fuzzy */
/* nl */  IDS_PET_SETTINGS_NL,
/* pl */  IDS_PET_SETTINGS_PL,    /* fuzzy */
/* sv */  IDS_PET_SETTINGS_SV},   /* fuzzy */

/* resgeoram.rc */
/* en */ {IDS_SELECT_FILE_GEORAM,
/* de */  IDS_SELECT_FILE_GEORAM_DE,    /* fuzzy */
/* fr */  IDS_SELECT_FILE_GEORAM_FR,    /* fuzzy */
/* it */  IDS_SELECT_FILE_GEORAM_IT,    /* fuzzy */
/* nl */  IDS_SELECT_FILE_GEORAM_NL,
/* pl */  IDS_SELECT_FILE_GEORAM_PL,    /* fuzzy */
/* sv */  IDS_SELECT_FILE_GEORAM_SV},   /* fuzzy */

/* resramcart.rc */
/* en */ {IDS_SELECT_FILE_RAMCART,
/* de */  IDS_SELECT_FILE_RAMCART_DE,    /* fuzzy */
/* fr */  IDS_SELECT_FILE_RAMCART_FR,    /* fuzzy */
/* it */  IDS_SELECT_FILE_RAMCART_IT,    /* fuzzy */
/* nl */  IDS_SELECT_FILE_RAMCART_NL,
/* pl */  IDS_SELECT_FILE_RAMCART_PL,    /* fuzzy */
/* sv */  IDS_SELECT_FILE_RAMCART_SV},   /* fuzzy */

/* resreu.rc */
/* en */ {IDS_SELECT_FILE_REU,
/* de */  IDS_SELECT_FILE_REU_DE,    /* fuzzy */
/* fr */  IDS_SELECT_FILE_REU_FR,    /* fuzzy */
/* it */  IDS_SELECT_FILE_REU_IT,    /* fuzzy */
/* nl */  IDS_SELECT_FILE_REU_NL,
/* pl */  IDS_SELECT_FILE_REU_PL,    /* fuzzy */
/* sv */  IDS_SELECT_FILE_REU_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_FLEXIBLE,
/* de */  IDS_FLEXIBLE_DE,    /* fuzzy */
/* fr */  IDS_FLEXIBLE_FR,    /* fuzzy */
/* it */  IDS_FLEXIBLE_IT,    /* fuzzy */
/* nl */  IDS_FLEXIBLE_NL,
/* pl */  IDS_FLEXIBLE_PL,    /* fuzzy */
/* sv */  IDS_FLEXIBLE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_ADJUSTING,
/* de */  IDS_ADJUSTING_DE,    /* fuzzy */
/* fr */  IDS_ADJUSTING_FR,    /* fuzzy */
/* it */  IDS_ADJUSTING_IT,    /* fuzzy */
/* nl */  IDS_ADJUSTING_NL,
/* pl */  IDS_ADJUSTING_PL,    /* fuzzy */
/* sv */  IDS_ADJUSTING_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_EXACT,
/* de */  IDS_EXACT_DE,    /* fuzzy */
/* fr */  IDS_EXACT_FR,    /* fuzzy */
/* it */  IDS_EXACT_IT,    /* fuzzy */
/* nl */  IDS_EXACT_NL,
/* pl */  IDS_EXACT_PL,    /* fuzzy */
/* sv */  IDS_EXACT_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SOUND_DRIVER_DIRECTX,
/* de */  IDS_SOUND_DRIVER_DIRECTX_DE,    /* fuzzy */
/* fr */  IDS_SOUND_DRIVER_DIRECTX_FR,    /* fuzzy */
/* it */  IDS_SOUND_DRIVER_DIRECTX_IT,    /* fuzzy */
/* nl */  IDS_SOUND_DRIVER_DIRECTX_NL,
/* pl */  IDS_SOUND_DRIVER_DIRECTX_PL,    /* fuzzy */
/* sv */  IDS_SOUND_DRIVER_DIRECTX_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SOUND_DRIVER_WMM,
/* de */  IDS_SOUND_DRIVER_WMM_DE,    /* fuzzy */
/* fr */  IDS_SOUND_DRIVER_WMM_FR,    /* fuzzy */
/* it */  IDS_SOUND_DRIVER_WMM_IT,    /* fuzzy */
/* nl */  IDS_SOUND_DRIVER_WMM_NL,
/* pl */  IDS_SOUND_DRIVER_WMM_PL,    /* fuzzy */
/* sv */  IDS_SOUND_DRIVER_WMM_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_MEDIA_FILES_FILTER,
/* de */  IDS_MEDIA_FILES_FILTER_DE,    /* fuzzy */
/* fr */  IDS_MEDIA_FILES_FILTER_FR,    /* fuzzy */
/* it */  IDS_MEDIA_FILES_FILTER_IT,    /* fuzzy */
/* nl */  IDS_MEDIA_FILES_FILTER_NL,
/* pl */  IDS_MEDIA_FILES_FILTER_PL,    /* fuzzy */
/* sv */  IDS_MEDIA_FILES_FILTER_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SPACE_KB,
/* de */  IDS_SPACE_KB_DE,    /* fuzzy */
/* fr */  IDS_SPACE_KB_FR,    /* fuzzy */
/* it */  IDS_SPACE_KB_IT,    /* fuzzy */
/* nl */  IDS_SPACE_KB_NL,
/* pl */  IDS_SPACE_KB_PL,    /* fuzzy */
/* sv */  IDS_SPACE_KB_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CONFIGURE_KEYSET_A,
/* de */  IDS_CONFIGURE_KEYSET_A_DE,    /* fuzzy */
/* fr */  IDS_CONFIGURE_KEYSET_A_FR,    /* fuzzy */
/* it */  IDS_CONFIGURE_KEYSET_A_IT,    /* fuzzy */
/* nl */  IDS_CONFIGURE_KEYSET_A_NL,
/* pl */  IDS_CONFIGURE_KEYSET_A_PL,    /* fuzzy */
/* sv */  IDS_CONFIGURE_KEYSET_A_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_CONFIGURE_KEYSET_B,
/* de */  IDS_CONFIGURE_KEYSET_B_DE,    /* fuzzy */
/* fr */  IDS_CONFIGURE_KEYSET_B_FR,    /* fuzzy */
/* it */  IDS_CONFIGURE_KEYSET_B_IT,    /* fuzzy */
/* nl */  IDS_CONFIGURE_KEYSET_B_NL,
/* pl */  IDS_CONFIGURE_KEYSET_B_PL,    /* fuzzy */
/* sv */  IDS_CONFIGURE_KEYSET_B_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_SAVE_MEDIA_IMAGE,
/* de */  IDS_SAVE_MEDIA_IMAGE_DE,    /* fuzzy */
/* fr */  IDS_SAVE_MEDIA_IMAGE_FR,    /* fuzzy */
/* it */  IDS_SAVE_MEDIA_IMAGE_IT,    /* fuzzy */
/* nl */  IDS_SAVE_MEDIA_IMAGE_NL,
/* pl */  IDS_SAVE_MEDIA_IMAGE_PL,    /* fuzzy */
/* sv */  IDS_SAVE_MEDIA_IMAGE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_S_AT_D_SPEED,
/* de */  IDS_S_AT_D_SPEED_DE,    /* fuzzy */
/* fr */  IDS_S_AT_D_SPEED_FR,    /* fuzzy */
/* it */  IDS_S_AT_D_SPEED_IT,    /* fuzzy */
/* nl */  IDS_S_AT_D_SPEED_NL,
/* pl */  IDS_S_AT_D_SPEED_PL,    /* fuzzy */
/* sv */  IDS_S_AT_D_SPEED_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_TAPE,
/* de */  IDS_TAPE_DE,    /* fuzzy */
/* fr */  IDS_TAPE_FR,    /* fuzzy */
/* it */  IDS_TAPE_IT,    /* fuzzy */
/* nl */  IDS_TAPE_NL,
/* pl */  IDS_TAPE_PL,    /* fuzzy */
/* sv */  IDS_TAPE_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_JOYSTICK,
/* de */  IDS_JOYSTICK_DE,    /* fuzzy */
/* fr */  IDS_JOYSTICK_FR,    /* fuzzy */
/* it */  IDS_JOYSTICK_IT,    /* fuzzy */
/* nl */  IDS_JOYSTICK_NL,
/* pl */  IDS_JOYSTICK_PL,    /* fuzzy */
/* sv */  IDS_JOYSTICK_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_RECORDING,
/* de */  IDS_RECORDING_DE,    /* fuzzy */
/* fr */  IDS_RECORDING_FR,    /* fuzzy */
/* it */  IDS_RECORDING_IT,    /* fuzzy */
/* nl */  IDS_RECORDING_NL,
/* pl */  IDS_RECORDING_PL,    /* fuzzy */
/* sv */  IDS_RECORDING_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_PLAYBACK,
/* de */  IDS_PLAYBACK_DE,    /* fuzzy */
/* fr */  IDS_PLAYBACK_FR,    /* fuzzy */
/* it */  IDS_PLAYBACK_IT,    /* fuzzy */
/* nl */  IDS_PLAYBACK_NL,
/* pl */  IDS_PLAYBACK_PL,    /* fuzzy */
/* sv */  IDS_PLAYBACK_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDS_UNKNOWN,
/* de */  IDS_UNKNOWN_DE,    /* fuzzy */
/* fr */  IDS_UNKNOWN_FR,    /* fuzzy */
/* it */  IDS_UNKNOWN_IT,    /* fuzzy */
/* nl */  IDS_UNKNOWN_NL,
/* pl */  IDS_UNKNOWN_PL,    /* fuzzy */
/* sv */  IDS_UNKNOWN_SV}    /* fuzzy */

};

/* --------------------------------------------------------------------- */

/* codepage 28591 (ISO 8859-1) to current codepage conversion tables */

static char cp28591[256];

static const WCHAR wcp28591[256] =
{
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f,
    0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
    0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
    0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
    0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
    0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7,
    0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
    0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
    0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf,
    0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,
    0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
    0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7,
    0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df,
    0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
    0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
    0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
    0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff
};


/* codepage 28592 (ISO 8859-2) to current codepage conversion tables */

static char cp28592[256];

static const WCHAR wcp28592[256] =
{
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f,
    0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
    0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
    0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
    0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
    0x00a0, 0x0104, 0x02d8, 0x0141, 0x00a4, 0x013d, 0x015a, 0x00a7,
    0x00a8, 0x0160, 0x015e, 0x0164, 0x0179, 0x00ad, 0x017d, 0x017b,
    0x00b0, 0x0105, 0x02db, 0x0142, 0x00b4, 0x013e, 0x015b, 0x02c7,
    0x00b8, 0x0161, 0x015f, 0x0165, 0x017a, 0x02dd, 0x017e, 0x017c,
    0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7,
    0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e,
    0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7,
    0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df,
    0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7,
    0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f,
    0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7,
    0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9
};

char *intl_convert_cp(char *text, int cp)
{
  int len,i;
  char *cp_table;
  char *buffer;

  if (text==NULL)
    return NULL;
  len=strlen(text);
  if (len==0)
    return NULL;
  switch (cp)
  {
    case 28591:
      cp_table=cp28591;
      break;
    case 28592:
      cp_table=cp28592;
      break;
    default:
      cp_table=cp28591;
  }
  buffer=lib_stralloc(text);
  for (i = 0; i < len; i++)
  {
    buffer[i]=cp_table[(unsigned char)text[i]];
  }
  return buffer;
}

/* --------------------------------------------------------------------- */

static char *intl_text_table[countof(intl_translate_text_table)][countof(language_table)];
static char *intl_text_table2[16384][countof(language_table)];
// the 16384 will be removed


/* this routine fills in the table of resources and pointers to the
   text that belongs to them with the right data, this is because of
   the way that the text resources have to be copied into a buffer
   before they can be used, so it might be best to do that at init.

   It also prepares any codepage conversion tables. */

static void intl_text_init(void)
{
int i, j, k;
char temp_buffer[4098 * sizeof(TCHAR)];
HRSRC hRes;
HGLOBAL hGlob;
WORD *p;
int length;

    for (i = 0; i < countof(language_table); i++) {
        for (j = (13008 >> 4) + 1; j < (16384 >> 4); j++) {
            hRes = FindResourceEx(NULL, RT_STRING, MAKEINTRESOURCE(j), MAKELANGID(windows_to_iso[i].windows_code, SUBLANG_NEUTRAL));
            if (hRes) {
                hGlob = LoadResource(NULL, hRes);
                p = LockResource(hGlob);
                for (k = 0; k < 16; k++) {
                    ZeroMemory(temp_buffer, sizeof(temp_buffer));
                    length = *p++;
                    WideCharToMultiByte(CP_ACP, 0, p, length, temp_buffer, 4096, NULL, NULL);
                    p = p + length;
                    intl_text_table2[((j - 1) << 4) + k][i] = strdup(temp_buffer);
                }
                FreeResource(hGlob);
            } else {
                for (k = 0; k < 16; k++) {
                    intl_text_table2[((j - 1) << 4) + k][i] = NULL;
                }
            }
        }
    }
  for (i = 0; i < countof(language_table); i++)
  {
    for (j = 0; j < countof(intl_translate_text_table); j++)
    {
      if (LoadString(winmain_instance, intl_translate_text_table[j][i], temp_buffer, 4097)==0)
      {
        intl_text_table[j][i]=NULL;
      }
      else
      {
        intl_text_table[j][i]=strdup(temp_buffer);
      }
    }
  }

  /* prepare the codepage 28591 (ISO 8859-1) to current codepage conversion */
  WideCharToMultiByte(CP_ACP, 0, wcp28591, 256, cp28591, 256, NULL, NULL);  

  /* prepare the codepage 28592 (ISO 8859-2) to current codepage conversion */
  WideCharToMultiByte(CP_ACP, 0, wcp28592, 256, cp28592, 256, NULL, NULL);  
}

static void intl_text_free(void)
{
  int i,j;

  for (i = 0; i < countof(language_table); i++)
  {
    for (j = 0; j < countof(intl_translate_text_table); j++)
    {
      if (intl_text_table[j][i]!=NULL)
        lib_free(intl_text_table[j][i]);
    }
  }

    for (i = 0; i < countof(language_table); i++) {
        for (j = 0; j < countof(intl_text_table2); j++) {
            if (intl_text_table2[j][i] != NULL) {
                lib_free(intl_text_table2[j][i]);
            }
        }
    }
}

char *intl_translate_text(int en_resource)
{
  int i;

  for (i = 0; i < countof(intl_translate_text_table); i++)
  {
    if (intl_translate_text_table[i][0]==en_resource)
    {
      if (intl_translate_text_table[i][current_language_index]!=0 &&
          intl_text_table[i][current_language_index]!=NULL &&
          strlen(intl_text_table[i][current_language_index])!=0)
        return intl_text_table[i][current_language_index];
      else
        return intl_text_table[i][0];
    }
  }
  return "";
}

char *intl_translate_text_new(int en_resource)
{
char *text;

    text = intl_text_table2[en_resource][current_language_index];
    if (text == NULL) {
        text = intl_text_table2[en_resource][0];
    }
    return text;
/*
HRSRC hRes;
HGLOBAL hGlob;
WORD *p;
int n;
int i;
int length;
char buffer[1024];

    ZeroMemory(buffer, 1024);
    hRes = FindResourceEx(NULL, RT_STRING, MAKEINTRESOURCE((en_resource >> 4) + 1), MAKELANGID(windows_to_iso[current_language_index].windows_code, SUBLANG_NEUTRAL));
    hGlob = LoadResource(NULL, hRes);
    p = LockResource(hGlob);
    n = en_resource & 0xf;
    for (i = 0; i < n; i++) {
        length = *p++;
        p = p + length;
    }
    length = *p++;
    WideCharToMultiByte(CP_ACP, 0, p, length, buffer, 1024, NULL, NULL);

    return strdup(buffer);
    */
}

/* pre-translated main window caption text so the emulation won't
   slow down because of all the translation calls */

char *intl_speed_at_text;

/* --------------------------------------------------------------------- */

void intl_init(void)
{
  intl_text_init();
}

int intl_translate_res(int en_resource)
{
  int i;

  if (!strcmp(current_language,"en"))
    return en_resource;

  for (i = 0; i < countof(intl_table); i++)
  {
    if (intl_table[i][0]==en_resource)
      return intl_table[i][current_language_index];
  }
  return en_resource;
}

void intl_shutdown(void)
{
  intl_text_free();
}

/* --------------------------------------------------------------------- */


static void intl_update_pre_translated_text(void)
{
  intl_speed_at_text=intl_translate_text(IDS_S_AT_D_SPEED);
}

char *intl_arch_language_init(void)
{
  int i;
  WORD winlang;

  winlang=GetUserDefaultLangID()&0x3ff;
  for (i = 0; windows_to_iso[i].iso_language_code != NULL; i++)
  {
    if (windows_to_iso[i].windows_code==winlang)
      return windows_to_iso[i].iso_language_code;
  }
  return "en";
}

void intl_update_ui(void)
{
  intl_update_pre_translated_text();
  ui_update_menu();
}
