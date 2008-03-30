/*
 * iec-cmdline-options.c
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

#include "cmdline.h"
#include "drive.h"
#include "iec-cmdline-options.h"
#include "lib.h"

#ifdef HAS_TRANSLATION
#include "translate.h"

static const cmdline_option_trans_t cmdline_options[] = {
    { "-dos1541", SET_RESOURCE, 1, NULL, NULL, "DosName1541", "dos1541",
      IDCLS_P_NAME, IDCLS_SPECIFY_1541_DOS_ROM_NAME },
    { "-dos1541II", SET_RESOURCE, 1, NULL, NULL, "DosName1541II", "d1541II",
      IDCLS_P_NAME, IDCLS_SPECIFY_1541_II_DOS_ROM_NAME },
    { "-dos1570", SET_RESOURCE, 1, NULL, NULL, "DosName1570", "dos1570",
      IDCLS_P_NAME, IDCLS_SPECIFY_1570_DOS_ROM_NAME },
    { "-dos1571", SET_RESOURCE, 1, NULL, NULL, "DosName1571", "dos1571",
      IDCLS_P_NAME, IDCLS_SPECIFY_1571_DOS_ROM_NAME },
    { "-dos1581", SET_RESOURCE, 1, NULL, NULL, "DosName1581", "dos1581",
      IDCLS_P_NAME, IDCLS_SPECIFY_1581_DOS_ROM_NAME },
    { NULL }
};

static cmdline_option_trans_t cmd_drive[] = {
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      0, IDCLS_ENABLE_PAR_CABLE },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      0, IDCLS_DISABLE_PAR_CABLE },
    { NULL, SET_RESOURCE, 1, NULL, NULL, NULL, (void *)DRIVE_IDLE_TRAP_IDLE,
      IDCLS_P_METHOD,
      IDCLS_SET_IDLE_METHOD },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      0, IDCLS_ENABLE_DRIVE_RAM_2000 },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      0, IDCLS_DISABLE_DRIVE_RAM_2000 },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      0, IDCLS_ENABLE_DRIVE_RAM_4000 },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      0, IDCLS_DISABLE_DRIVE_RAM_4000 },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      0, IDCLS_ENABLE_DRIVE_RAM_6000 },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      0, IDCLS_DISABLE_DRIVE_RAM_6000 },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      0, IDCLS_ENABLE_DRIVE_RAM_8000 },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      0, IDCLS_DISABLE_DRIVE_RAM_8000 },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      0, IDCLS_ENABLE_DRIVE_RAM_A000 },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      0, IDCLS_DISABLE_DRIVE_RAM_A000 },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-dos1541", SET_RESOURCE, 1, NULL, NULL, "DosName1541", "dos1541",
      "<name>", "Specify name of 1541 DOS ROM image" },
    { "-dos1541II", SET_RESOURCE, 1, NULL, NULL, "DosName1541II", "d1541II",
      "<name>", "Specify name of 1541-II DOS ROM image" },
    { "-dos1570", SET_RESOURCE, 1, NULL, NULL, "DosName1570", "dos1570",
      "<name>", "Specify name of 1570 DOS ROM image" },
    { "-dos1571", SET_RESOURCE, 1, NULL, NULL, "DosName1571", "dos1571",
      "<name>", "Specify name of 1571 DOS ROM image" },
    { "-dos1581", SET_RESOURCE, 1, NULL, NULL, "DosName1581", "dos1581",
      "<name>", "Specify name of 1581 DOS ROM image" },
    { NULL }
};

static cmdline_option_t cmd_drive[] = {
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      NULL, "Enable SpeedDOS-compatible parallel cable" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      NULL, "Disable SpeedDOS-compatible parallel cable" },
    { NULL, SET_RESOURCE, 1, NULL, NULL, NULL, (void *)DRIVE_IDLE_TRAP_IDLE,
      "<method>",
      "Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      NULL, "Enable 8KB RAM expansion at $2000-$3FFF" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      NULL, "Disable 8KB RAM expansion at $2000-$3FFF" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      NULL, "Enable 8KB RAM expansion at $4000-$5FFF" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      NULL, "Disable 8KB RAM expansion at $4000-$5FFF" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      NULL, "Enable 8KB RAM expansion at $6000-$7FFF" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      NULL, "Disable 8KB RAM expansion at $6000-$7FFF" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      NULL, "Enable 8KB RAM expansion at $8000-$9FFF" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      NULL, "Disable 8KB RAM expansion at $8000-$9FFF" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)1,
      NULL, "Enable 8KB RAM expansion at $A000-$BFFF" },
    { NULL, SET_RESOURCE, 0, NULL, NULL, NULL, (void *)0,
      NULL, "Disable 8KB RAM expansion at $A000-$BFFF" },
    { NULL }
};
#endif

int iec_cmdline_options_init(void)
{
    unsigned int dnr, i;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        cmd_drive[0].name = lib_msprintf("-parallel%i", dnr + 8);
        cmd_drive[0].resource_name
            = lib_msprintf("Drive%iParallelCable", dnr + 8);
        cmd_drive[1].name = lib_msprintf("+parallel%i", dnr + 8);
        cmd_drive[1].resource_name
            = lib_msprintf("Drive%iParallelCable", dnr + 8);
        cmd_drive[2].name = lib_msprintf("-drive%iidle", dnr + 8);
        cmd_drive[2].resource_name
            = lib_msprintf("Drive%iIdleMethod", dnr + 8);
        cmd_drive[3].name = lib_msprintf("-drive%iram2000", dnr + 8);
        cmd_drive[3].resource_name
            = lib_msprintf("Drive%iRAM2000", dnr + 8);
        cmd_drive[4].name = lib_msprintf("+drive%iram2000", dnr + 8);
        cmd_drive[4].resource_name
            = lib_msprintf("Drive%iRAM2000", dnr + 8);
        cmd_drive[5].name = lib_msprintf("-drive%iram4000", dnr + 8);
        cmd_drive[5].resource_name
            = lib_msprintf("Drive%iRAM4000", dnr + 8);
        cmd_drive[6].name = lib_msprintf("+drive%iram4000", dnr + 8);
        cmd_drive[6].resource_name
            = lib_msprintf("Drive%iRAM4000", dnr + 8);
        cmd_drive[7].name = lib_msprintf("-drive%iram6000", dnr + 8);
        cmd_drive[7].resource_name
            = lib_msprintf("Drive%iRAM6000", dnr + 8);
        cmd_drive[8].name = lib_msprintf("+drive%iram6000", dnr + 8);
        cmd_drive[8].resource_name
            = lib_msprintf("Drive%iRAM6000", dnr + 8);
        cmd_drive[9].name = lib_msprintf("-drive%iram8000", dnr + 8);
        cmd_drive[9].resource_name
            = lib_msprintf("Drive%iRAM8000", dnr + 8);
        cmd_drive[10].name = lib_msprintf("+drive%iram8000", dnr + 8);
        cmd_drive[10].resource_name
            = lib_msprintf("Drive%iRAM8000", dnr + 8);
        cmd_drive[11].name = lib_msprintf("-drive%irama000", dnr + 8);
        cmd_drive[11].resource_name
            = lib_msprintf("Drive%iRAMA000", dnr + 8);
        cmd_drive[12].name = lib_msprintf("+drive%irama000", dnr + 8);
        cmd_drive[12].resource_name
            = lib_msprintf("Drive%iRAMA000", dnr + 8);

#ifdef HAS_TRANSLATION
        if (cmdline_register_options_trans(cmd_drive) < 0)
#else
        if (cmdline_register_options(cmd_drive) < 0)
#endif
            return -1;

        for (i = 0; i < 13; i++) {
            lib_free((char *)cmd_drive[i].name);
            lib_free((char *)cmd_drive[i].resource_name);
        }
    }


#ifdef HAS_TRANSLATION
    return cmdline_register_options_trans(cmdline_options);
#else
    return cmdline_register_options(cmdline_options);
#endif
}

