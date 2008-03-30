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
#include "res.h" /* 50456 */
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

/* ressid.rc */
/* en */ {IDD_SID_GENERAL_SETTINGS_DIALOG,
/* de */  IDD_SID_GENERAL_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_SID_GENERAL_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_SID_GENERAL_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_SID_GENERAL_SETTINGS_DIALOG_NL,
/* pl */  IDD_SID_GENERAL_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_SID_GENERAL_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* ressid.rc */
/* en */ {IDD_SID_RESID_SETTINGS_DIALOG,
/* de */  IDD_SID_RESID_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_SID_RESID_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_SID_RESID_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_SID_RESID_SETTINGS_DIALOG_NL,
/* pl */  IDD_SID_RESID_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_SID_RESID_SETTINGS_DIALOG_SV},   /* fuzzy, size */

/* ressid.rc */
/* en */ {IDD_SID_HARDSID_SETTINGS_DIALOG,
/* de */  IDD_SID_HARDSID_SETTINGS_DIALOG_DE,    /* fuzzy, size */
/* fr */  IDD_SID_HARDSID_SETTINGS_DIALOG_FR,    /* fuzzy, size */
/* it */  IDD_SID_HARDSID_SETTINGS_DIALOG_IT,    /* fuzzy, size */
/* nl */  IDD_SID_HARDSID_SETTINGS_DIALOG_NL,
/* pl */  IDD_SID_HARDSID_SETTINGS_DIALOG_PL,    /* fuzzy, size */
/* sv */  IDD_SID_HARDSID_SETTINGS_DIALOG_SV},   /* fuzzy, size */

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

};

/* --------------------------------------------------------------------- */

static int intl_idr_table[][countof(intl_language_table)] = {

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

typedef struct intl_text_s {
    /* pointer to english text */
    char *en_text;

    /* index of text in text table */
    int index;

    /* number of next hash entry */
    int hash_next;
} intl_text_t;

static unsigned int num_intl_text, num_allocated_intl_text;
static intl_text_t *intl_text;

/* use a hash table with 1024 entries */
static const unsigned int logHashSize = 10;

static int *hashTable = NULL;

/* calculate the hash key */
static unsigned int intl_text_calc_hash_key(const char *text)
{
  unsigned int key, i, shift;

  key = 0; shift = 0;
  for (i = 0; text[i] != '\0'; i++)
  {
    unsigned int sym = (unsigned int)text[i];

    if (shift >= logHashSize)
      shift = 0;

    key ^= (sym << shift);
    if (shift + 8 > logHashSize)
    {
      key ^= (((unsigned int)sym) >> (logHashSize - shift));
    }
    shift++;
  }
  return (key & ((1 << logHashSize) - 1));
}

static int intl_text_register(const char *text, const int index)
{
  intl_text_t *dp;
  unsigned int hashkey;

  dp = intl_text + num_intl_text;

  if (num_allocated_intl_text <= num_intl_text)
  {
    num_allocated_intl_text *= 2;
    intl_text = lib_realloc(intl_text, num_allocated_intl_text * sizeof(intl_text_t));
    dp = intl_text + num_intl_text;
  }

  dp->en_text = lib_stralloc(text);
  dp->index = index;

  hashkey = intl_text_calc_hash_key(text);
  dp->hash_next = hashTable[hashkey];
  hashTable[hashkey] = (dp - intl_text);

  num_intl_text++;

  return 0;
}

static void intl_text_free(void)
{
  unsigned int i;

  for (i = 0; i < num_intl_text; i++)
    lib_free((intl_text + i)->en_text);
}

static void intl_text_shutdown(void)
{
  intl_text_free();

  lib_free(intl_text);
  lib_free(hashTable);
}

static intl_text_t *intl_text_lookup(const char *text)
{
  intl_text_t *res;
  unsigned int hashkey;

  hashkey = intl_text_calc_hash_key(text);
  res = (hashTable[hashkey] >= 0) ? intl_text + hashTable[hashkey] : NULL;
  while (res != NULL)
  {
    if (strcmp(res->en_text, text) == 0)
      return res;
    res = (res->hash_next >= 0) ? intl_text + res->hash_next : NULL;
  }
  return NULL;
}

static int intl_text_init(void)
{
  unsigned int i;

  num_allocated_intl_text = 100;
  num_intl_text = 0;
  intl_text = (intl_text_t *)lib_malloc(num_allocated_intl_text * sizeof(intl_text_t));

  hashTable = (int *)lib_malloc((1 << logHashSize) * sizeof(int));

  for (i = 0; i < (unsigned int)(1 << logHashSize); i++)
    hashTable[i] = -1;

  return 0;
}

static int intl_text_get_value(const char *text)
{
  intl_text_t *r = intl_text_lookup(text);
  if (r==NULL)
    return -1;
  else
    return r->index;
}

/* --------------------------------------------------------------------- */

static char *intl_text_table[][countof(intl_language_table)] = {

/* the following entries are also present in the unix po files */

/* vsync.c */
/* en */ {"Your machine is too slow for current settings!",
/* de */  "Deine Maschine ist zu langsam für aktuelle Einstellungen!",
/* fr */  "Désolé mais votre ordinateur est trop lent pour les paramètres choisis!",
/* it */  "Il computer è troppo lento per queste impostazioni!",
/* nl */  "Uw machine is te traag voor de huidige instellingen!",
/* pl */  "Twój komputer jest zbyt wolny na obecne ustawienia!",
/* sv */  "Din maskin är för långsam för nuvarande inställningar!"},

/* screenshot.c */
/* en */ {"Sorry. Multiple recording is not supported.",
/* de */  "Eine Aufnahme ist zur Zeit aktiv. Mehrfache Aufnahme ist nicht möglich.",
/* fr */  "Désolé. Vous ne pouvez enregistrer plus d'une chose à la fois.",
/* it */  "Le registrazioni multiple non sono supportate.",
/* nl */  "Sorry. Meerdere opnames wordt niet ondersteunt.",
/* pl */  "",     /* fuzzy */
/* sv */  "Endast en inspelning kan göras åt gången."},

/* autostart.c */
/* en */ {"Cannot load snapshot file.",
/* de */  "Kann Snapshot Datei nicht laden.",
/* fr */  "Impossible de charger le fichier de sauvegarde.",
/* it */  "Non è possibile caricare il file di snapshot.",
/* nl */  "Kan momentopname bestand niet laden.",
/* pl */  "",     /* fuzzy */
/* sv */  "Kan inte ladda ögonblicksbildfil."},

/* sound.c */
/* en */ {"write to sound device failed.",
/* de */  "Schreiben auf Sound Gerät ist fehlgeschlagen.",
/* fr */  "Impossible d'écriture sur le périphérique de son.",
/* it */  "scrittura sulla scheda audio fallita.",
/* nl */  "Schrijf-actie naar geluidsapparaat faalt.",
/* pl */  "",     /* fuzzy */
/* sv */  "misslyckades att skriva till ljudenhet."},

/* sound.c */
/* en */ {"Cannot open SID engine",
/* de */  "Kann SID Engine nicht öffnen",
/* fr */  "Erreur de chargement de l'engin de son SID",
/* it */  "Non è possibile aprire il motore SID",
/* nl */  "Kan de SID kern niet openen",
/* pl */  "Ustawienia uk³adu SID",     /* fuzzy */
/* sv */  "Kan inte öppna SID-motor"},

/* sound.c */
/* en */ {"Cannot initialize SID engine",
/* de */  "Kann SID Engine nicht initialisieren",
/* fr */  "Erreur d'initialisation de l'engin de son SID",
/* it */  "Non è possibile inizializzare il motore SID",
/* nl */  "Kan de SID kern niet initialiseren",
/* pl */  "",     /* fuzzy */
/* sv */  "Kan inte initiera SID-motor"},

/* sound.c */
/* en */ {"initialization failed for device `%s'.",
/* de */  "Initialisierung von Gerät `%s' fehlgeschlagen.",
/* fr */  "erreur d'initialisation du périphérique `%s':",
/* it */  "inizializzazione fallita per il device `%s'.",
/* nl */  "Initialisatie faalt voor apparaar `%s'.",
/* pl */  "Kalibracja sprzêtowego joysticka dla urz±dzenia `%s':",     /* fuzzy */
/* sv */  "initiering misslyckades för enhet \"%s\"."},

/* sound.c */
/* en */ {"sound device lacks stereo capability",
/* de */  "Sound Gerät unterstützt keine Stereo Ausgabe",
/* fr */  "le périphérique de son n'est pas stéréo",
/* it */  "la scheda audio non dispone di una modalità stereofonica",
/* nl */  "Geluid apparaat heeft geen stereo mogelijkheid.",
/* pl */  "",     /* fuzzy */
/* sv */  "ljudenhet saknar stereofunktion"},

/* sound.c */
/* en */ {"device '%s' not found or not supported.",
/* de */  "Gerät '%s' konnte nicht gefunden werden oder ist nicht unterstützt.",
/* fr */  "périphérique '%s' non trouvé ou non supporté.",
/* it */  "il device '%s' non è stato trovato oppure non è supportato.",
/* nl */  "apparaat '%s' niet gevonden of niet ondersteunt.",
/* pl */  "",     /* fuzzy */
/* sv */  "enheten \"%s\" hittades eller stöds ej."},

/* sound.c */
/* en */ {"Recording device %s doesn't exist!",
/* de */  "Aufnahme Gerät %s existiert nicht!",
/* fr */  "Le périphérique d'enregistrement %s n'existe pas!",
/* it */  "Il device di registrazione %s non esiste!",
/* nl */  "Opname apparaat %s bestaat niet!",
/* pl */  "",     /* fuzzy */
/* sv */  "Inspelningsenhet %s finns inte!"},

/* sound.c */
/* en */ {"Recording device must be different from playback device",
/* de */  "Aufnahme Gerät muß unteschiedlich vom Abspielgerät sein",
/* fr */  "Le périphérique d'enregistrement doit être différent du périphérique de lecture",
/* it */  "Il device di registrazione deve essere differente da quello di riproduzione",
/* nl */  "Opname apparaat moet anders zijn dan afspeel apparaat",
/* pl */  "",     /* fuzzy */
/* sv */  "Inspelningsenhet och återspelningsenhet kan inte vara samma"},

/* sound.c */
/* en */ {"Warning! Recording device %s seems to be a realtime device!",
/* de */  "Warnung! Aufnahme Gerät %s scheint ein Echtzeitgerät zu sein!",
/* fr */  "Attention! Le périphérique d'enregistrement %s semble être un périphérique en temps réel",
/* it */  "Attenzione! Il device di registrazione %s sembra essere un dispositivo realtime!",
/* nl */  "Waarschuwing! Opname apparaat %s lijkt een realtime apparaat te zijn!",
/* pl */  "",     /* fuzzy */
/* sv */  "Varning! Inspelningsenheten %s verkar vara en realtidsenhet!"},

/* sound.c */
/* en */ {"Initialization failed for device `%s'.",
/* de */  "Initialisierung von Gerät `%s' fehlgeschlagen.",
/* fr */  "Échec de l'initialisation du périphérique `%s'.",
/* it */  "Inizializzazione fallita per il device `%s'.",
/* nl */  "Initialisatie faalt voor apparaat `%s'.",
/* pl */  "Kalibracja sprzêtowego joysticka dla urz±dzenia `%s':",     /* fuzzy */
/* sv */  "Initiering misslyckades för enhet \"%s\"."},

/* sound.c */
/* en */ {"The recording device doesn't support current sound parameters",
/* de */  "Aufnahmegerät unterstütz derzeitige Soundeinstellungen nicht",
/* fr */  "Le périphérique d'enregistrement ne supporte pas les paramètres de son actuellement configurés",
/* it */  "Il device di registrazione non supporta i parametri attuali",
/* nl */  "Opname apparaat ondersteunt de huidige geluid parameters niet",
/* pl */  "",     /* fuzzy */
/* sv */  "Inspelningsenheten stöder inte aktuella ljudinställningar"},

/* sound.c */
/* en */ {"Sound buffer overflow (cycle based)",
/* de */  "Sound Puffer Überlauf (Zyklus basiert)",
/* fr */  "Erreur de dépassement de limite du tampon son (basé sur les cycles)",
/* it */  "Overflow del buffer sonoro (cycle based)",
/* nl */  "Geluidsbuffer overstroming (cycli gebaseerd)",
/* pl */  "",     /* fuzzy */
/* sv */  "För mycket data i ljudbufferten (cykelbaserad)"},

/* sound.c */
/* en */ {"Sound buffer overflow.",
/* de */  "Sound Puffer Überlauf",
/* fr */  "Erreur de dépassement de limite du tampon son.",
/* it */  "Overflow del buffer sonoro.",
/* nl */  "Geluidsbuffer overstroming",
/* pl */  "",     /* fuzzy */
/* sv */  "För mycket data i ljudbufferten."},

/* sound.c */
/* en */ {"cannot flush.",
/* de */  "Entleerung nicht möglich.",
/* fr */  "impossible de vider.",
/* it */  "non è possibile svuotare.",
/* nl */  "kan niet spoelen.",
/* pl */  "",     /* fuzzy */
/* sv */  "kan inte tömma."},

/* sound.c */
/* en */ {"fragment problems.",
/* de */  "Fragmentierungsproblem.",
/* fr */  "problèmes de fragments.",
/* it */  "problemi di frammentazione.",
/* nl */  "fragment problemen.",
/* pl */  "",     /* fuzzy */
/* sv */  "fragmentprogram."},

/* sound.c */
/* en */ {"Buffer drained",
/* de */  "Puffer geleert",
/* fr */  "Tampon vide",
/* it */  "Buffer vuoto",
/* nl */  "Buffer leeg",
/* pl */  "Rozmiar buffora",     /* fuzzy */
/* sv */  "Buffert tömd"},

/* sound.c */
/* en */ {"running too slow.",
/* de */  "Ablauf zu langsam.",
/* fr */  "l'exécution est trop lente.",
/* it */  "esecuzione troppo lenta.",
/* nl */  "draait te langzaam.",
/* pl */  "",     /* fuzzy */
/* sv */  "går för långsamt."},

/* sound.c */
/* en */ {"write to sounddevice failed.",
/* de */  "Schreiben auf Sound Gerät ist fehlgeschlagen.",
/* fr */  "erreur d'écriture sur le périphérique de son.",
/* it */  "scrittura sulla scheda audio fallita.",
/* nl */  "schrijf actie naar geluidsapparaat lukt niet.",
/* pl */  "",     /* fuzzy */
/* sv */  "misslyckades skriva till ljudenheten."},

/* sound.c */
/* en */ {"store to sounddevice failed.",
/* de */  "Speichern auf Sound Gerät ist fehlgeschlagen.",
/* fr */  "erreur d'enregistrement sur le périphérique de son.",
/* it */  "memorizzazione sulla scheda audio fallita.",
/* nl */  "opslag naar geluidsapparaat lukt niet.",
/* pl */  "",     /* fuzzy */
/* sv */  "misslyckades spara i ljudenheten."},

/* event.c */
/* en */ {"Could not create start snapshot file %s.",
/* de */  "Kann Start Snapshot Datei nicht erzeugen: %s",
/* fr */  "Impossible de créer le fichier de sauvegarde de départ %s.",
/* it */  "Non è possibile creare il file di inizio snapshot %s.",
/* nl */  "Kon het start momentopname bestand %s niet maken.",
/* pl */  "Nie mo¿na za³adowaæ pliku zrzutu\n`%s'",     /* fuzzy */
/* sv */  "Kunde inte skapa startögonblicksbildfilen %s."},

/* event.c */
/* en */ {"Error reading end snapshot file %s.",
/* de */  "Kann Ende Snapshot Datei nicht lesen: %s",
/* fr */  "Erreur de lecture dans le fichier de sauvegarde de fin %s.",
/* it */  "Errore durante la lettura del file di fine snapshot %s.",
/* nl */  "Fout bij het lezen van het eind van het momentopname bestand %s.",
/* pl */  "Nie mo¿na za³adowaæ pliku zrzutu\n`%s'",     /* fuzzy */
/* sv */  "Fel vid läsning av slutögonblicksbildfilen %s."},

/* event.c */
/* en */ {"Could not create end snapshot file %s.",
/* de */  "Kann Ende Snapshot Datei nicht erzeugen: %s",
/* fr */  "Impossible de créer le fichier de sauvegarde de fin %s.",
/* it */  "Non è possibile creare il file di fine snapshot %s.",
/* nl */  "Kon het eind momentopname bestand %s niet maken.",
/* pl */  "Nie mo¿na za³adowaæ pliku zrzutu\n`%s'",     /* fuzzy */
/* sv */  "Kunde inte skapa slutögonblicksbildfilen %s."},

/* event.c */
/* en */ {"Could not open end snapshot file %s.",
/* de */  "Kann Ende Snapshot Datei nicht öffnen: %s",
/* fr */  "Impossible d'ouvrir le fichier de sauvegarde de fin %s.",
/* it */  "Non è possibile aprire il file di fine snapshot %s.",
/* nl */  "Kon het eind momentopname bestand %s niet openen.",
/* pl */  "Nie mo¿na za³adowaæ pliku zrzutu\n`%s'",     /* fuzzy */
/* sv */  "Kunde inte öppna slutögonblicksbildfilen %s."},

/* event.c */
/* en */ {"Could not find event section in end snapshot file.",
/* de */  "Kann Sektion in Ende Snapshotdatei nicht finden.",
/* fr */  "Impossible de trouver la section des événements dans le fichier de sauvegarde de fin.",
/* it */  "Non è possibile trovare la sezione eventi nel file di fine snapshot.",
/* nl */  "Kon de gebeurtenis sectie in eind momentopname bestand niet vinden.",
/* pl */  "",     /* fuzzy */
/* sv */  "Kunde inte hinna händelsedelen i slutögonblicksbilden."},

/* event.c */
/* en */ {"Error reading start snapshot file. Tried %s and %s",
/* de */  "Fehler beim Lesen der Start Snapshot Datei. Versuch gescheitert bei %s und %s.",
/* fr */  "Erreur de lecture  du fichier de sauvegarde de départ. %s et %s ont été testés",
/* it */  "Errore durante la lettura del file di inizio snapshot. Ho provato %s e %s",
/* nl */  "Fout bij het lezen van het start momentopname bestand. Heb %s en %s geprobeerd",
/* pl */  "B³±d czytania urz±dzenia cyfrowego joysticka.",     /* fuzzy */
/* sv */  "Fel vid läsning av startögonblicksbildfil. Försökte med %s och %s"},

/* event.c */
/* en */ {"Error reading start snapshot file.",
/* de */  "Fehler beim Lesen der Start Snapshot Datei.",
/* fr */  "Erreur de lecture du fichier de sauvegarde de départ.",
/* it */  "Errore durante la lettura del file di inizio snapshot.",
/* nl */  "Fout bij het lezen van het start momentopname bestand.",
/* pl */  "B³±d czytania urz±dzenia cyfrowego joysticka.",     /* fuzzy */
/* sv */  "Fel vid läsning av startögonblicksbildfil."},


/* the following things I marked with _() because they also apply to unix gettext,
   but they are not in the po files yet. */

/* event.c */
/* en */ {"Cannot create image file %s",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan bestand %s niet maken",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* event.c */
/* en */ {"Cannot write image file %s",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan niet schrijven naar bestand %s",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* event.c */
/* en */ {"Cannot find mapped name for %s",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan vertaalde naam voor %s niet vinden",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* c64export.c */
/* en */ {"Resource IO1 blocked by %s.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Bron IO1 geblokeerd door %s.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* c64export.c */
/* en */ {"Resource IO2 blocked by %s.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Bron IO2 geblokeerd door %s.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* c64export.c */
/* en */ {"Resource ROML blocked by %s.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Bron ROML geblokeerd door %s.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* c64export.c */
/* en */ {"Resource ROMH blocked by %s.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Bron ROMH geblokeerd door %s.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */


/* the following are windows arch files */

/* arch/win32/ffmpeglib.c */
/* en */ {"Your ffmpeg dll version doesn't match.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Uw ffmpeg dll versie is niet juist.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/fullscrn.c */
/* en */ {"DirectDraw error: Code:%8x Error:%s",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "DirectDraw fout: Code:%8x Fout:%s",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"VICE Error!",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE Fout!",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"VICE Information",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE Informatie",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"Cannot save settings.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan de instellingen niet schrijven",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"Cannot load settings.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan de instellingen niet laden",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"Default settings restored.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Standaard instellingen hersteld",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c, arch/win32/uiattach.c, arch/win32/uiperipheral.c */
/* en */ {"Cannot attach specified file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan het opgegeven bestand niet gebruiken",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"%s\n\nStart monitor?",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"VICE CPU JAM",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"Extend image to 40-track format?",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "bestand uitbreiden naar 40-sporen?",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c, arch/win32/uilib.c */
/* en */ {"VICE question",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE vraag",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"%s at %d%% speed, %d fps%s",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "%s met %d%% snelheid, %d fps%s",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {" (warp)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"Detached device %s",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Apparaat %s ontkoppelt",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"Attached %s to device#%s",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "%s gekoppelt aan apparaat#%s",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"Detached tape",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Tape ontkoppelt",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"History recorded with unknown release",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geschiedenis opgenomen met onbekende VICE versie",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"History recorded with VICE-%s",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geschiedenis opgenomen met VICE-%s",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"paused",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "gepauzeerd",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"resumed",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "hervat",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c */
/* en */ {"Do you really want to exit?\n\n"
          "All the data present in the emulated RAM will be lost.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Wilt u echt afsluiten?\n\n"
          "Alle data in geëmuleerd geheugen zal verloren gaan.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/ui.c, arch/win32/uiattach.c, arch/win32/uiperipheral.c */
/* en */ {"Cannot autostart specified file.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan opgegeven bestand niet autostarten.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiattach.c, arch/win32/uiperipheral.c */
/* en */ {"Attach disk image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Disk bestand koppelen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiattach.c */
/* en */ {"Attach tape image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Tape bestand koppelen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiattach.c */
/* en */ {"Autostart disk/tape image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Autostart disk/tape bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Invalid cartridge",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Ongeldige cartridge",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uicart.c, arch/win32/uiplus4cart.c */
/* en */ {"Bad cartridge config in UI!",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Slechte cartridge configuratie in UI!",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uicart.c, arch/win32/uiplus4cart.c */
/* en */ {"Invalid cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Ongeldig cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiconsole.c */
/* en */ {"Logging console output image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Logboek console uitvoer bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiconsole.c */
/* en */ {"VICE console logging files (*.dbg)\0*.dbg\0",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE console logboek bestanden (*.dbg)\0*.dbg\0",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiconsole.c */
/* en */ {"Cannot write log file `%s'.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan logboek bestand `%s' niet schrijven.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uifliplist.c */
/* en */ {"Load flip list file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Laad flip lijst bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uifliplist.c */
/* en */ {"Cannot read flip list file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan flip lijst bestand niet lezen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uifliplist.c */
/* en */ {"Save flip list file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Flip lijst bestand opslaan",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uifliplist.c */
/* en */ {"Cannot write flip list file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan flip lijst bestand niet schrijven",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uikeyboard.c */
/* en */ {"Select keymap file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer keymap bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uikeyboard.c */
/* en */ {"Save keymap file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Keymap bestand opslaan",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uikeyboard.c */
/* en */ {"Cannot write keymap file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan keymap bestand niet schrijven",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uikeyboard.c */
/* en */ {"Mapping",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uikeyboard.c */
/* en */ {"Keyboard settings",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Toetsenbord instellingen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"&Attach",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "&Koppelen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"Please enter a filename.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geef aub een bestandsnaam op.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"Overwrite existing image?",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Overschrijven bestaand bestand?",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"Cannot create image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan bestand niet maken",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"Command line options",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Commando opties",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"Which command line options are available?",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Welke commando opties zijn beschikbaar?",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uimediafile.c */
/* en */ {"Media files (*.bmp;*.png;*.wav;*.mp3;*.avi;*.mpg)\0*.bmp;*.png;*.wav;*.mp3;*.avi;*.mpg\0",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Media bestanden (*.bmp;*.png;*.wav;*.mp3;*.avi;*.mpg)\0*.bmp;*.png;*.wav;*.mp3;*.avi;*.mpg\0",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uimediafile.c */
/* en */ {"No driver selected or selected driver not supported",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geen stuurprogramma geselecteerd, of geselecteerd stuurprogramma wordt niet ondersteunt",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uimediafile.c */
/* en */ {"Cannot write screenshot file `%s'.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan scherm afdruk bestand `%s' niet schrijven.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiperipheral.c */
/* en */ {"Autostart disk image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Autostart disk bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiperipheral.c */
/* en */ {"Select file system directory",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer bestand systeem directory",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiperipheral.c */
/* en */ {"Printer Userport",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiperipheral.c */
/* en */ {"Printer 4",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiperipheral.c */
/* en */ {"Printer 5",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiperipheral.c, arch/win32/uidrivec128.c,
   arch/win32/uidrivec64vic20.c, arch/win32/uidrivepetcbm2.c,
   arch/win32/uidriveplus4.c */
/* en */ {"Drive 8",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Station 8",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiperipheral.c, arch/win32/uidrivec128.c,
   arch/win32/uidrivec64vic20.c, arch/win32/uidrivepetcbm2.c,
   arch/win32/uidriveplus4.c */
/* en */ {"Drive 9",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Station 9",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiperipheral.c, arch/win32/uidrivec128.c,
   arch/win32/uidrivec64vic20.c, arch/win32/uidriveplus4.c */
/* en */ {"Drive 10",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Station 10",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiperipheral.c, arch/win32/uidrivec128.c,
   arch/win32/uidrivec64vic20.c, arch/win32/uidriveplus4.c */
/* en */ {"Drive 11",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Station 11",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiperipheral.c */
/* en */ {"Peripheral Settings",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Rand apparaat instellingen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiquicksnapshot.c */
/* en */ {"Can't write snapshot file.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan momentopname bestand niet schrijven.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uiquicksnapshot.c, arch/win32/uisnapshot.c */
/* en */ {"Cannot read snapshot image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan momentopname bestand niet lezen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"Load %s ROM image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Laad %s ROM bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"Select romset archive",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer romset archief",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"Cannot load romset archive!",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan romset archief niet laden!",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"Cannot save romset archive!",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan romset archief niet opslaan!",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"Cannot load romset file!",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan romset bestand niet laden!",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"Cannot save romset file!",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan romset bestand niet opslaan!",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"Select romset file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer romset bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"Romset",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"Computer",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"Drive",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Station",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uirom.c */
/* en */ {"ROM settings",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "ROM instellingen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uisid.c */
/* en */ {"This machine may not have a SID",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Deze machine heeft geen SID",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uisid.c */
/* en */ {"General",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Algemeen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uisid.c */
/* en */ {"SID settings",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "SID instellingen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uisnapshot.c */
/* en */ {"Save snapshot image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Momentopname bestand opslaan",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uisnapshot.c */
/* en */ {"VICE snapshot files (*.vsf)\0*.vsf\0",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE momentopname bestanden (*.vsf)\0*.vsf\0",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uisnapshot.c */
/* en */ {"Cannot write snapshot file `%s'.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kan momentopname bestand `%s' niet schrijven.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uisnapshot.c */
/* en */ {"Load snapshot image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Laad momentopname bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uivideo.c */
/* en */ {"Could not load palette file.",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kon palette bestand niet laden.",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uivideo.c */
/* en */ {"Load VICE palette file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Laad VICE palette bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uivideo.c */
/* en */ {"Fullscreen",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Volscherm",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uivideo.c */
/* en */ {"Colors",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Kleuren",
/* pl */  "",     /* fuzzy */
/* sv */  ""},    /* fuzzy */

/* arch/win32/uivideo.c */
/* en */ {"Video settings",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Video instellingen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/statusbar.c */
/* en */ {"Tape:",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/statusbar.c */
/* en */ {"Joystick:",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/statusbar.c */
/* en */ {"Recording\n%02d:%02d",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Opnemen\n%02d:%02d",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/statusbar.c */
/* en */ {"Playback\n%02d:%02d (%02d:%02d)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Afspelen\n%02d:%02d (%02d:%02d)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/statusbar.c */
/* en */ {"Unknown",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Onbekend",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiacia.c, arch/win32/uijoystick.c,
   arch/win32/uiperipheral.c, arcj/win32/uisound.c */
/* en */ {"None",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiacia.c */
/* en */ {"IRQ",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiacia.c */
/* en */ {"NMI",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiacia.c, arch/win32/uirs232user.c */
/* en */ {"RS232 device %i",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "RS232 apparaat %i",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic128.c */
/* en */ {"Select internal function ROM image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer intern functie ROM bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic128.c */
/* en */ {"Select external function ROM image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer extern functie ROM bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic128.c */
/* en */ {"Machine type",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Machine soort",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic128.c */
/* en */ {"Function ROM",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Functie ROM",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic128.c */
/* en */ {"C128 settings",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "C128 instellingen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach CRT cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel CRT cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach raw 8KB cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel binair 8KB cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach raw 16KB cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel binair 16KB cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach Action Replay cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel Action Replay cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach Atomic Power cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel Atomic Power cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach Epyx fastload cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel Epyx fastload cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach IEEE interface cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel IEEE interface cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach Retro Replay cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel Retro Replay cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach IDE64 interface cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel IDE64 interface cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach Super Snapshot 4 cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel Super Snapshot 4 cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uic64cart.c */
/* en */ {"Attach Super Snapshot 5 cartridge image",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel Super Snapshot 5 cartridge bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uicbm2set.c, arch/win32/uipetset.c */
/* en */ {"Model",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uicbm2set.c */
/* en */ {"Memory",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geheugen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uicbm2set.c */
/* en */ {"CBM2 settings",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "CBM2 instellingen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uidrivec128.c, arch/win32/uidrivec64vic20.c,
   arch/win32/uidrivepetcbm2.c, arch/win32/uidriveplus4.c */
/* en */ {"Drive Settings",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Drive instellingen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uievent.c */
/* en */ {"Select start snapshot for event history",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer start momentopname voor gebeurtenis geschiedenis",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uievent.c */
/* en */ {"Select end snapshot for event history",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer eind momentopname voor gebeurtenis geschiedenis",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uihelp.c */
/* en */ {"Version %s *UNSTABLE*",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Versie %s *ONSTABIEL*",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uihelp.c */
/* en */ {"Version %s",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Versie %s",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uihelp.c */
/* en */ {"VICE contributors",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Vice medewerkers",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uihelp.c */
/* en */ {"Who made what?",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Wie heeft wat gemaakt?",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uihelp.c */
/* en */ {"License",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Licensie",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uihelp.c */
/* en */ {"No warranty!",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geen garantie!",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uihelp.c */
/* en */ {"No warranty!",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geen garantie!",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uihelp.c */
/* en */ {"No warranty!",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geen garantie!",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uihelp.c */
/* en */ {"VICE is distributed WITHOUT ANY WARRANTY!",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE heeft ABSOLUUT GEEN GARANTIE!",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiide64.c */
/* en */ {"Total size: %iKB",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Totale grootte: %iKB",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiide64.c */
/* en */ {"Total size: %iKB",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Totale grootte: %iKB",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiide64.c */
/* en */ {"Select HD image file",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer HD bestand",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Press key for NorthWest",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Druk toets voor NoordWest",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Press key for North",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Druk toets voor Noord",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Press key for NorthEast",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Druk toets voor NoordOost",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Press key for East",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Druk toets voor Oost",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Press key for SouthEast",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Druk toets voor ZuidOost",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Press key for South",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Druk toets voor Zuid",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Press key for SouthWest",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Druk toets voor ZuidWest",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Press key for West",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Druk toets voor West",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Press key for Fire",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Druk toets voor Vuur",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Numpad + RCtrl",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Keyset A",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"Keyset B",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"All buttons used as fire",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Alle knoppen gebruiken als vuur",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"numeric (see above)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "numeriek (zie boven)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uijoystick.c */
/* en */ {"No button - Autofire disabled",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geen knop - Autovuren is uit",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"All files (*.*)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Alle bestanden (*.*)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"VICE palette files (*.vpl)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE palette bestanden (*.vpl)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"VICE snapshot files (*.vsf)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE momentopname bestanden (*.vsf)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"Disk image files (*.d64;*.d71;*.d80;*.d81;*.d82;*.g64;*.g41;*.x64)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Disk bestanden (*.d64;*.d71;*.d80;*.d81;*.d82;*.g64;*.g41;*.x64)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"Tape image files (*.t64;*.p00;*.tap;*.prg)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Tape bestanden (*.t64;*.p00;*.tap;*.prg)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"Zipped files (*.zip;*.bz2;*.gz;*.d6z;*.d7z;*.d8z;*.g6z;*.g4z;*.x6z)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Ingepakte bestanden (*.zip;*.bz2;*.gz;*.d6z;*.d7z;*.d8z;*.g6z;*.g4z;*.x6z)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"CRT cartridge image files (*.crt)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "CRT cartridge bestanden (*.crt)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"Raw cartridge image files (*.bin)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Binaire cartridge bestanden (*.bin)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"VICE flip list files (*.vfl)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE flip lijst bestanden (*.vfl)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"VICE romset files (*.vrs)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE romset bestanden (*.vrs)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"VICE romset archives (*.vra)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE romset archieven (*.vra)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uilib.c */
/* en */ {"VICE keymap files (*.vkm)",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "VICE keymap bestanden (*.vkm)",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiperipheral.c */
/* en */ {"File system",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Bestand systeem",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiperipheral.c */
/* en */ {"Real IEC device",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Echt IEC apparaat",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiperipheral.c */
/* en */ {"Input/Output",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Invoer/Uitvoer",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiperipheral.c */
/* en */ {"PET settings",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "PET instellingen",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiplus4cart.c */
/* en */ {"Attach cartridge image for Function Low",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel cartridge bestand voor 'Function Low'",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiplus4cart.c */
/* en */ {"Attach cartridge image for Function High",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel cartridge bestand voor 'Function High'",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiplus4cart.c */
/* en */ {"Attach cartridge image for Cartridge1 Low",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel cartridge bestand voor 'Cartridge1 Low'",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiplus4cart.c */
/* en */ {"Attach cartridge image for Cartridge1 High",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel cartridge bestand voor 'Cartridge1 High'",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiplus4cart.c */
/* en */ {"Attach cartridge image for Cartridge2 Low",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel cartridge bestand voor 'Cartridge2 Low'",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiplus4cart.c */
/* en */ {"Attach cartridge image for Cartridge2 High",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel cartridge bestand voor 'Cartridge2 High'",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uigeoram.c */
/* en */ {"Select file for GEORAM",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer bestand voor GEORAM",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiramcart.c */
/* en */ {"Select file for RAMCART",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer bestand voor RAMCART",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uiramcart.c */
/* en */ {"Select file for REU",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Selecteer bestand voor REU",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uisid.c */
/* en */ {"fast",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "snel",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uisid.c */
/* en */ {"interpolating",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uisid.c */
/* en */ {"resampling",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uisid.c */
/* en */ {"fast resampling",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "snelle resampling",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uisound.c */
/* en */ {"Flexible",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Flexibel",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uisound.c */
/* en */ {"Adjusting",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Aanpassend",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uisound.c */
/* en */ {"Exact",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uisound.c */
/* en */ {"Sound driver: DirectX",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geluid stuurprogramma: DirectX",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/uisound.c */
/* en */ {"Sound driver: WMM",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Geluid stuurprogramma: WMM",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/vic20ui.c */
/* en */ {"Attach 4/8/16KB cartridge image at $2000",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel 4/8/16KB cartridge bestand op $2000",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/vic20ui.c */
/* en */ {"Attach 4/8/16KB cartridge image at $4000",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel 4/8/16KB cartridge bestand op $4000",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/vic20ui.c */
/* en */ {"Attach 4/8/16KB cartridge image at $6000",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel 4/8/16KB cartridge bestand op $6000",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/vic20ui.c */
/* en */ {"Attach 8KB cartridge image at $A000",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel 8KB cartridge bestand op $A000",
/* pl */  "",     /* fuzzy */
/* sv */  ""},     /* fuzzy */

/* arch/win32/vic20ui.c */
/* en */ {"Attach 4KB cartridge image at $B000",
/* de */  "",     /* fuzzy */
/* fr */  "",     /* fuzzy */
/* it */  "",     /* fuzzy */
/* nl */  "Koppel 4KB cartridge bestand op $B000",
/* pl */  "",     /* fuzzy */
/* sv */  ""}     /* fuzzy */

};

/* --------------------------------------------------------------------- */

static void intl_init(void)
{
  int i;

  intl_text_init();
  for (i = 0; i < countof(intl_text_table); i++)
  {
    intl_text_register(intl_text_table[i][0], i);
  }
}

int intl_translate_dialog(int en_dialog)
{
  int i;

  if (!strcmp(current_language,"en"))
    return en_dialog;

  for (i = 0; i < countof(intl_idd_table); i++)
  {
    if (intl_idd_table[i][0]==en_dialog)
      return intl_idd_table[i][current_language_index];
  }
  return en_dialog;
}

int intl_translate_menu(int en_menu)
{
  int i;

  if (!strcmp(current_language,"en"))
    return en_menu;

  for (i = 0; i < countof(intl_idr_table); i++)
  {
    if (intl_idr_table[i][0]==en_menu)
      return intl_idr_table[i][current_language_index];
  }
  return en_menu;
}

char *intl_translate_text(char *text)
{
  int i;

  if (text==NULL)
    return text;
  i=intl_text_get_value(text);
  if (i<0)
    return text;
  if (strlen(intl_text_table[i][current_language_index])==0)
    return text;
  return intl_text_table[i][current_language_index];
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

void intl_windows_language_init(void)
{
  char *lang;

  lang=get_current_windows_language();
  set_current_language(lang,"");
}

static const resource_t resources[] = {
  { "Language", RES_STRING, (resource_value_t)"en",
    (void *)&current_language, set_current_language, NULL },
  { NULL }
};

int intl_resources_init(void)
{
  intl_init();
  return resources_register(resources);
}

void intl_resources_shutdown(void)
{
  intl_text_shutdown();
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
