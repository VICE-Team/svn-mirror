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

#include "archapi.h"
#include "cmdline.h"
#include "intl.h"
#include "lib.h"
#include "machine.h"
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

};

/* --------------------------------------------------------------------- */

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

char intl_buffer[1024];

static char *intl_scan_resource_file(void)
{
  FILE *f;
  int line_len;
  int done=0;
  char machine_id[16];

  f=fopen(archdep_default_resource_file_name(),"r");
  if (f==NULL)
    return NULL;
  sprintf(machine_id,"[%s]",machine_name);
  while (done==0)
  {
    line_len=util_get_line(intl_buffer, 1024, f);
    if (line_len<0)
      done=-1;
    if (!strcmp(intl_buffer,machine_id))
      done=1;
  }
  if (done!=1)
  {
    fclose(f);
    return NULL;
  }
  done=0;
  while (done==0)
  {
    line_len=util_get_line(intl_buffer, 1024, f);
    if (line_len<0)
      done=-1;
    if (intl_buffer[0]=='[')
      done=-1;
    if (!strncasecmp(intl_buffer,"Language=\"",10))
    {
      intl_buffer[12]=0;
      done=1;
    }
  }
  fclose(f);
  if (done==1)
    return intl_buffer+10;
  else
    return NULL;
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

void intl_pre_ui_init(int argc, char **argv)
{
  int i;
  int command_line_set=0;
  char *lang;
  char *lang_from_resource;

  /* first get the standard windows language
     in case there is no command-line argument
     or resource for the language */

  lang = get_current_windows_language();

  /* secondly check the command-line arguments
     for a language statement */

  for (i = 1; i < argc-1; i++)
  {
    if (!strcasecmp(argv[i],"-lang"))
    {
      command_line_set=1;
      lang = argv[i+1];
    }
  }

  /* thirdly only check the resource file if no language
     command-line was found */

  if (command_line_set==0)
  {
    lang_from_resource=intl_scan_resource_file();
    if (lang_from_resource!=NULL)
      lang=lang_from_resource;
  }
  set_current_language(lang,"");
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
