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
#include <windows.h>

#include "cmdline.h"
#include "intl.h"
#include "lib.h"
#include "res.h" /* 50348 */
#include "resources.h"
#include "util.h"


#define countof(array) (sizeof(array) / sizeof((array)[0]))


static char *current_language = NULL;
static int current_language_index = 0;

typedef struct windows_iso_s {
    int windows_code;
    char *iso_language_code;
} windows_iso_t;


/* this table holds only the currently present translation
   languages, to add a new translation look at
   http://www.unicode.org/unicode/onlinedat/languages.html
   for the corresponding windows and iso codes */

static windows_iso_t windows_to_iso[]={
  {LANG_GERMAN, "de"},
  {LANG_ENGLISH, "en"},
  {LANG_FRENCH, "fr"},
  {LANG_ITALIAN, "it"},
  {LANG_DUTCH, "nl"},
  {LANG_POLISH, "pl"},
  {LANG_SWEDISH, "sv"},
  {0, NULL}
};

static char *intl_language_table[] = {

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

static int intl_idd_table[][countof(intl_language_table)] = {

/* resacia.rc */
/* en */ {IDD_ACIA_SETTINGS_DIALOG,
/* de */  IDD_ACIA_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_ACIA_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_ACIA_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_ACIA_SETTINGS_DIALOG_NL,
/* pl */  IDD_ACIA_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_ACIA_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDD_C128ROM_SETTINGS_DIALOG,
/* de */  IDD_C128ROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C128ROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C128ROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C128ROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_C128ROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C128ROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDD_C128DRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_C128DRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C128DRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C128DRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C128DRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_C128DRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C128DRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDD_C128ROM_RESOURCE_DIALOG,
/* de */  IDD_C128ROM_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C128ROM_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C128ROM_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C128ROM_RESOURCE_DIALOG_NL,	
/* pl */  IDD_C128ROM_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C128ROM_RESOURCE_DIALOG_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDD_C128ROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C128ROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDD_C128_MACHINE_SETTINGS_DIALOG,
/* de */  IDD_C128_MACHINE_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C128_MACHINE_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C128_MACHINE_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C128_MACHINE_SETTINGS_DIALOG_NL,
/* pl */  IDD_C128_MACHINE_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C128_MACHINE_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDD_C128_FUNCTIONROM_SETTINGS_DIALOG,
/* de */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C128_FUNCTIONROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDD_DRIVE_SETTINGS_DIALOG_C128,
/* de */  IDD_DRIVE_SETTINGS_DIALOG_C128_DE,    /* fuzzy */
/* fr */  IDD_DRIVE_SETTINGS_DIALOG_C128_FR,    /* fuzzy */
/* it */  IDD_DRIVE_SETTINGS_DIALOG_C128_IT,    /* fuzzy */
/* nl */  IDD_DRIVE_SETTINGS_DIALOG_C128_NL,
/* pl */  IDD_DRIVE_SETTINGS_DIALOG_C128_PL,    /* fuzzy */
/* sv */  IDD_DRIVE_SETTINGS_DIALOG_C128_SV},   /* fuzzy */

/* resc128.rc */
/* en */ {IDD_C128KBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C128KBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resdrivec64vic20.rc */
/* en */ {IDD_DRIVE_SETTINGS_DIALOG_C64VIC20,
/* de */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_DE,    /* fuzzy */
/* fr */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_FR,    /* fuzzy */
/* it */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_IT,    /* fuzzy */
/* nl */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_NL,
/* pl */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_PL,    /* fuzzy */
/* sv */  IDD_DRIVE_SETTINGS_DIALOG_C64VIC20_SV},   /* fuzzy */

/* resdrivepetcbm2.rc */
/* en */ {IDD_DRIVE_SETTINGS_DIALOG_PETCBM2,
/* de */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_DE,    /* fuzzy */
/* fr */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_FR,    /* fuzzy */
/* it */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_IT,    /* fuzzy */
/* nl */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_NL,
/* pl */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_PL,    /* fuzzy */
/* sv */  IDD_DRIVE_SETTINGS_DIALOG_PETCBM2_SV},   /* fuzzy */

/* resgeoram.rc */
/* en */ {IDD_GEORAM_SETTINGS_DIALOG,
/* de */  IDD_GEORAM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_GEORAM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_GEORAM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_GEORAM_SETTINGS_DIALOG_NL,
/* pl */  IDD_GEORAM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_GEORAM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resramcart.rc */
/* en */ {IDD_RAMCART_SETTINGS_DIALOG,
/* de */  IDD_RAMCART_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_RAMCART_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_RAMCART_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_RAMCART_SETTINGS_DIALOG_NL,
/* pl */  IDD_RAMCART_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_RAMCART_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resreu.rc */
/* en */ {IDD_REU_SETTINGS_DIALOG,
/* de */  IDD_REU_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_REU_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_REU_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_REU_SETTINGS_DIALOG_NL,
/* pl */  IDD_REU_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_REU_SETTINGS_DIALOG_SV},   /* fuzzy */

/* reside64.rc */
/* en */ {IDD_IDE64_SETTINGS_DIALOG,
/* de */  IDD_IDE64_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_IDE64_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_IDE64_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_IDE64_SETTINGS_DIALOG_NL,
/* pl */  IDD_IDE64_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_IDE64_SETTINGS_DIALOG_SV},   /* fuzzy */

/* restfe.rc */
/* en */ {IDD_TFE_SETTINGS_DIALOG,
/* de */  IDD_TFE_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_TFE_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_TFE_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_TFE_SETTINGS_DIALOG_NL,
/* pl */  IDD_TFE_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_TFE_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resrs232user.rc */
/* en */ {IDD_RS232USER_SETTINGS_DIALOG,
/* de */  IDD_RS232USER_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_RS232USER_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_RS232USER_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_RS232USER_SETTINGS_DIALOG_NL,
/* pl */  IDD_RS232USER_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_RS232USER_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resvicii.rc */
/* en */ {IDD_VICII_DIALOG,
/* de */  IDD_VICII_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_VICII_DIALOG_FR,    /* fuzzy */
/* it */  IDD_VICII_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_VICII_DIALOG_NL,
/* pl */  IDD_VICII_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_VICII_DIALOG_SV},   /* fuzzy */

/* ressid.rc */
/* en */ {IDD_SID_GENERAL_SETTINGS_DIALOG,
/* de */  IDD_SID_GENERAL_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_SID_GENERAL_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_SID_GENERAL_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_SID_GENERAL_SETTINGS_DIALOG_NL,
/* pl */  IDD_SID_GENERAL_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_SID_GENERAL_SETTINGS_DIALOG_SV},   /* fuzzy */

/* ressid.rc */
/* en */ {IDD_SID_RESID_SETTINGS_DIALOG,
/* de */  IDD_SID_RESID_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_SID_RESID_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_SID_RESID_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_SID_RESID_SETTINGS_DIALOG_NL,
/* pl */  IDD_SID_RESID_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_SID_RESID_SETTINGS_DIALOG_SV},   /* fuzzy */

/* ressid.rc */
/* en */ {IDD_SID_HARDSID_SETTINGS_DIALOG,
/* de */  IDD_SID_HARDSID_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_SID_HARDSID_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_SID_HARDSID_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_SID_HARDSID_SETTINGS_DIALOG_NL,
/* pl */  IDD_SID_HARDSID_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_SID_HARDSID_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resc64.rc */
/* en */ {IDD_C64ROM_SETTINGS_DIALOG,
/* de */  IDD_C64ROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C64ROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C64ROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C64ROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_C64ROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C64ROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resc64.rc */
/* en */ {IDD_C64DRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_C64DRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C64DRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C64DRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C64DRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_C64DRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C64DRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resc64.rc */
/* en */ {IDD_C64ROM_RESOURCE_DIALOG,
/* de */  IDD_C64ROM_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C64ROM_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C64ROM_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C64ROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_C64ROM_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C64ROM_RESOURCE_DIALOG_SV},   /* fuzzy */

/* resc64.rc */
/* en */ {IDD_C64ROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C64ROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy */

/* resc64.rc */
/* en */ {IDD_C64KBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_C64KBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy */

/* rescbm2.rc */
/* en */ {IDD_CBM2ROM_SETTINGS_DIALOG,
/* de */  IDD_CBM2ROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_CBM2ROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_CBM2ROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_CBM2ROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_CBM2ROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_CBM2ROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* rescbm2.rc */
/* en */ {IDD_CBM2DRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_CBM2DRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* rescbm2.rc */
/* en */ {IDD_CBM2ROM_RESOURCE_DIALOG,
/* de */  IDD_CBM2ROM_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_CBM2ROM_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_CBM2ROM_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_CBM2ROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_CBM2ROM_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_CBM2ROM_RESOURCE_DIALOG_SV},   /* fuzzy */

/* rescbm2.rc */
/* en */ {IDD_CBM2ROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_CBM2ROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy */

/* rescbm2.rc */
/* en */ {IDD_CBMII_SETTINGS_MODEL_DIALOG,
/* de */  IDD_CBMII_SETTINGS_MODEL_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_CBMII_SETTINGS_MODEL_DIALOG_FR,    /* fuzzy */
/* it */  IDD_CBMII_SETTINGS_MODEL_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_CBMII_SETTINGS_MODEL_DIALOG_NL,
/* pl */  IDD_CBMII_SETTINGS_MODEL_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_CBMII_SETTINGS_MODEL_DIALOG_SV},   /* fuzzy */

/* rescbm2.rc */
/* en */ {IDD_CBMII_SETTINGS_IO_DIALOG,
/* de */  IDD_CBMII_SETTINGS_IO_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_CBMII_SETTINGS_IO_DIALOG_FR,    /* fuzzy */
/* it */  IDD_CBMII_SETTINGS_IO_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_CBMII_SETTINGS_IO_DIALOG_NL,
/* pl */  IDD_CBMII_SETTINGS_IO_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_CBMII_SETTINGS_IO_DIALOG_SV},   /* fuzzy */

/* rescbm2.rc */
/* en */ {IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_CBM2KBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDD_PETROM_SETTINGS_DIALOG,
/* de */  IDD_PETROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PETROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PETROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PETROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_PETROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PETROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDD_PETDRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_PETDRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PETDRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PETDRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PETDRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_PETDRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PETDRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDD_PETROM_RESOURCE_DIALOG,
/* de */  IDD_PETROM_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PETROM_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PETROM_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PETROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_PETROM_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PETROM_RESOURCE_DIALOG_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDD_PETROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_PETROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PETROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PETROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PETROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_PETROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PETROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDD_PET_SETTINGS_MODEL_DIALOG,
/* de */  IDD_PET_SETTINGS_MODEL_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PET_SETTINGS_MODEL_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PET_SETTINGS_MODEL_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PET_SETTINGS_MODEL_DIALOG_NL,
/* pl */  IDD_PET_SETTINGS_MODEL_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PET_SETTINGS_MODEL_DIALOG_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDD_PET_SETTINGS_IO_DIALOG,
/* de */  IDD_PET_SETTINGS_IO_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PET_SETTINGS_IO_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PET_SETTINGS_IO_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PET_SETTINGS_IO_DIALOG_NL,
/* pl */  IDD_PET_SETTINGS_IO_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PET_SETTINGS_IO_DIALOG_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDD_PET_SETTINGS_SUPER_DIALOG,
/* de */  IDD_PET_SETTINGS_SUPER_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PET_SETTINGS_SUPER_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PET_SETTINGS_SUPER_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PET_SETTINGS_SUPER_DIALOG_NL,
/* pl */  IDD_PET_SETTINGS_SUPER_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PET_SETTINGS_SUPER_DIALOG_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDD_PET_SETTINGS_8296_DIALOG,
/* de */  IDD_PET_SETTINGS_8296_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PET_SETTINGS_8296_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PET_SETTINGS_8296_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PET_SETTINGS_8296_DIALOG_NL,
/* pl */  IDD_PET_SETTINGS_8296_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PET_SETTINGS_8296_DIALOG_SV},   /* fuzzy */

/* respet.rc */
/* en */ {IDD_PETKBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PETKBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDD_PLUS4ROM_SETTINGS_DIALOG,
/* de */  IDD_PLUS4ROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PLUS4ROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PLUS4ROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PLUS4ROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_PLUS4ROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PLUS4ROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDD_PLUS4DRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PLUS4DRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDD_PLUS4ROM_RESOURCE_DIALOG,
/* de */  IDD_PLUS4ROM_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PLUS4ROM_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PLUS4ROM_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PLUS4ROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_PLUS4ROM_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PLUS4ROM_RESOURCE_DIALOG_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PLUS4ROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDD_PLUS4_MEMORY_DIALOG,
/* de */  IDD_PLUS4_MEMORY_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PLUS4_MEMORY_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PLUS4_MEMORY_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PLUS4_MEMORY_DIALOG_NL,
/* pl */  IDD_PLUS4_MEMORY_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PLUS4_MEMORY_DIALOG_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDD_DRIVE_SETTINGS_DIALOG_PLUS4,
/* de */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_DE,    /* fuzzy */
/* fr */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_FR,    /* fuzzy */
/* it */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_IT,    /* fuzzy */
/* nl */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_NL,
/* pl */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_PL,    /* fuzzy */
/* sv */  IDD_DRIVE_SETTINGS_DIALOG_PLUS4_SV},   /* fuzzy */

/* resplus4.rc */
/* en */ {IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_PLUS4KBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDD_VIC20ROM_SETTINGS_DIALOG,
/* de */  IDD_VIC20ROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_VIC20ROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_VIC20ROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_VIC20ROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_VIC20ROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_VIC20ROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDD_VIC20DRIVEROM_SETTINGS_DIALOG,
/* de */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_NL,
/* pl */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_VIC20DRIVEROM_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDD_VIC_SETTINGS_DIALOG,
/* de */  IDD_VIC_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_VIC_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_VIC_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_VIC_SETTINGS_DIALOG_NL,
/* pl */  IDD_VIC_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_VIC_SETTINGS_DIALOG_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDD_VIC20ROM_RESOURCE_DIALOG,
/* de */  IDD_VIC20ROM_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_VIC20ROM_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_VIC20ROM_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_VIC20ROM_RESOURCE_DIALOG_NL,
/* pl */  IDD_VIC20ROM_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_VIC20ROM_RESOURCE_DIALOG_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDD_VIC20ROMDRIVE_RESOURCE_DIALOG,
/* de */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_NL,
/* pl */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_VIC20ROMDRIVE_RESOURCE_DIALOG_SV},   /* fuzzy */

/* resvic20.rc */
/* en */ {IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG,
/* de */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_NL,
/* pl */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_VIC20KBD_MAPPING_SETTINGS_DIALOG_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDD_DISKDEVICE_DIALOG,
/* de */  IDD_DISKDEVICE_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_DISKDEVICE_DIALOG_FR,    /* fuzzy */
/* it */  IDD_DISKDEVICE_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_DISKDEVICE_DIALOG_NL,
/* pl */  IDD_DISKDEVICE_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_DISKDEVICE_DIALOG_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDD_DATASETTE_SETTINGS_DIALOG,
/* de */  IDD_DATASETTE_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_DATASETTE_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_DATASETTE_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_DATASETTE_SETTINGS_DIALOG_NL,
/* pl */  IDD_DATASETTE_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_DATASETTE_SETTINGS_DIALOG_SV},   /* fuzzy */

/* res.rc */
/* en */ {IDD_JOY_SETTINGS_DIALOG,
/* de */  IDD_JOY_SETTINGS_DIALOG_DE,    /* fuzzy */
/* fr */  IDD_JOY_SETTINGS_DIALOG_FR,    /* fuzzy */
/* it */  IDD_JOY_SETTINGS_DIALOG_IT,    /* fuzzy */
/* nl */  IDD_JOY_SETTINGS_DIALOG_NL,
/* pl */  IDD_JOY_SETTINGS_DIALOG_PL,    /* fuzzy */
/* sv */  IDD_JOY_SETTINGS_DIALOG_SV},   /* fuzzy */

};

int intl_translate_dialog(int en_dialog)
{
  int i, lang;

  if (!strcmp(current_language,"en"))
    return en_dialog;

  for (i = 0; i < countof(intl_idd_table); i++)
  {
    if (intl_idd_table[i][0]==en_dialog)
      return intl_idd_table[i][current_language_index];
  }
  return en_dialog;
}

static char *get_current_windows_language(void)
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

static int set_current_language(resource_value_t v, void *param)
{
  int i;

  const char *lang = (const char *)v;

  util_string_set(&current_language, "en");
  current_language_index=0;
  if (strlen(lang)!=2)
    return 0;

  for (i = 0; i < countof(intl_language_table); i++)
  {
    if (!strcasecmp(lang,intl_language_table[i]))
    {
      current_language_index=i;
      util_string_set(&current_language, intl_language_table[i]);
      return 0;
    }
  }
  return 0;
}

static const resource_t resources[] = {
  { "Language", RES_STRING, (resource_value_t)"en",
    (void *)&current_language, set_current_language, NULL },
  { NULL }
};

int intl_resources_init(void)
{
  return resources_register(resources);
}

void intl_resources_shutdown(void)
{
  lib_free(current_language);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-lang", SET_RESOURCE, 1, NULL, NULL, "Language", NULL,
      "<iso language code>", "Specify the iso code of the language" },
    { NULL }
};

int intl_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}
