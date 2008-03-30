/*
 * drive-cmdline-options.c - Hardware-level Commodore disk drive emulation,
 *                           command line options module.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "cmdline.h"
#include "drive.h"
#include "resources.h"

static cmdline_option_t cmdline_options[] = {
    { "-truedrive", SET_RESOURCE, 0, NULL, NULL, "DriveTrueEmulation",
      (resource_value_t) 1,
      NULL, "Enable hardware-level emulation of disk drives" },
    { "+truedrive", SET_RESOURCE, 0, NULL, NULL, "DriveTrueEmulation",
      (resource_value_t) 0,
      NULL, "Disable hardware-level emulation of disk drives" },
    { "-drive8type", SET_RESOURCE, 1, NULL, NULL, "Drive8Type",
      (resource_value_t) DRIVE_TYPE_1541, "<type>",
      "Set drive type (0: no drive)" },
    { "-drive9type", SET_RESOURCE, 1, NULL, NULL, "Drive9Type",
      (resource_value_t) DRIVE_TYPE_NONE, "<type>",
      "Set drive type (0: no drive)" },
    { "-parallel8", SET_RESOURCE, 0, NULL, NULL, "Drive8ParallelCable",
      (resource_value_t) 1,
      NULL, "Enable SpeedDOS-compatible parallel cable" },
    { "+parallel8", SET_RESOURCE, 0, NULL, NULL, "Drive8ParallelCable",
      (resource_value_t) 0,
      NULL, "Disable SpeedDOS-compatible parallel cable" },
    { "-parallel9", SET_RESOURCE, 0, NULL, NULL, "Drive9ParallelCable",
      (resource_value_t) 1,
      NULL, "Enable SpeedDOS-compatible parallel cable" },
    { "+parallel9", SET_RESOURCE, 0, NULL, NULL, "Drive9ParallelCable",
      (resource_value_t) 0,
      NULL, "Disable SpeedDOS-compatible parallel cable" },
    { "-drive8idle", SET_RESOURCE, 1, NULL, NULL, "Drive8IdleMethod",
      (resource_value_t) DRIVE_IDLE_TRAP_IDLE, "<method>",
      "Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)" },
    { "-drive9idle", SET_RESOURCE, 1, NULL, NULL, "Drive9IdleMethod",
      (resource_value_t) DRIVE_IDLE_TRAP_IDLE, "<method>",
      "Set drive idling method (0: no traps, 1: skip cycles, 2: trap idle)" },
    { "-drive8extend", SET_RESOURCE, 1, NULL, NULL, "Drive8ExtendImagePolicy",
      (resource_value_t) DRIVE_EXTEND_NEVER, "<method>",
      "Set drive 8 40 track extension policy (0: never, 1: ask, 2: on access)"},
    { "-drive9extend", SET_RESOURCE, 1, NULL, NULL, "Drive9ExtendImagePolicy",
      (resource_value_t) DRIVE_EXTEND_NEVER, "<method>",
      "Set drive 9 40 track extension policy (0: never, 1: ask, 2: on access)"},
    { "-pal", SET_RESOURCE, 0, NULL, NULL, "VideoStandard",
      (resource_value_t) DRIVE_SYNC_PAL,
      NULL, "Use PAL sync factor" },
    { "-ntsc", SET_RESOURCE, 0, NULL, NULL, "VideoStandard",
      (resource_value_t) DRIVE_SYNC_NTSC,
      NULL, "Use NTSC sync factor" },
    { "-ntscold", SET_RESOURCE, 0, NULL, NULL, "VideoStandard",
      (resource_value_t) DRIVE_SYNC_NTSCOLD,
      NULL, "Use old NTSC sync factor" },
    { "-dos1541", SET_RESOURCE, 1, NULL, NULL, "DosName1541", "dos1541",
      "<name>", "Specify name of 1541 DOS ROM image name" },
    { "-dos1541II", SET_RESOURCE, 1, NULL, NULL, "DosName1541II", "d1541II",
      "<name>", "Specify name of 1541-II DOS ROM image name" },
    { "-dos1571", SET_RESOURCE, 1, NULL, NULL, "DosName1571", "dos1571",
      "<name>", "Specify name of 1571 DOS ROM image name" },
    { "-dos1581", SET_RESOURCE, 1, NULL, NULL, "DosName1581", "dos1581",
      "<name>", "Specify name of 1581 DOS ROM image name" },
    { "-dos2031", SET_RESOURCE, 1, NULL, NULL, "DosName2031", "dos2031",
      "<name>", "Specify name of 2031 DOS ROM image name" },
    { "-dos1001", SET_RESOURCE, 1, NULL, NULL, "DosName1001", "dos1001",
      "<name>", "Specify name of 1001/8050/8250 DOS ROM image name" },
    { "-drive8ram2000", SET_RESOURCE, 0, NULL, NULL, "Drive8RAM2000",
      (resource_value_t) 1,
      NULL, "Enable 8KB RAM expansion from $2000-$3FFF" },
    { "+drive8ram2000", SET_RESOURCE, 0, NULL, NULL, "Drive8RAM2000",
      (resource_value_t) 0,
      NULL, "Disable 8KB RAM expansion from $2000-$3FFF" },
    { "-drive9ram2000", SET_RESOURCE, 0, NULL, NULL, "Drive9RAM2000",
      (resource_value_t) 1,
      NULL, "Enable 8KB RAM expansion from $2000-$3FFF" },
    { "+drive9ram2000", SET_RESOURCE, 0, NULL, NULL, "Drive9RAM2000",
      (resource_value_t) 0,
      NULL, "Disable 8KB RAM expansion from $2000-$3FFF" },
    { "-drive8ram4000", SET_RESOURCE, 0, NULL, NULL, "Drive8RAM4000",
      (resource_value_t) 1,
      NULL, "Enable 8KB RAM expansion from $4000-$5FFF" },
    { "+drive8ram4000", SET_RESOURCE, 0, NULL, NULL, "Drive8RAM4000",
      (resource_value_t) 0,
      NULL, "Disable 8KB RAM expansion from $4000-$5FFF" },
    { "-drive9ram4000", SET_RESOURCE, 0, NULL, NULL, "Drive9RAM4000",
      (resource_value_t) 1,
      NULL, "Enable 8KB RAM expansion from $4000-$5FFF" },
    { "+drive9ram4000", SET_RESOURCE, 0, NULL, NULL, "Drive9RAM4000",
      (resource_value_t) 0,
      NULL, "Disable 8KB RAM expansion from $4000-$5FFF" },
    { "-drive8ram6000", SET_RESOURCE, 0, NULL, NULL, "Drive8RAM6000",
      (resource_value_t) 1,
      NULL, "Enable 8KB RAM expansion from $6000-$7FFF" },
    { "+drive8ram6000", SET_RESOURCE, 0, NULL, NULL, "Drive8RAM6000",
      (resource_value_t) 0,
      NULL, "Disable 8KB RAM expansion from $6000-$7FFF" },
    { "-drive9ram6000", SET_RESOURCE, 0, NULL, NULL, "Drive9RAM6000",
      (resource_value_t) 1,
      NULL, "Enable 8KB RAM expansion from $6000-$7FFF" },
    { "+drive9ram6000", SET_RESOURCE, 0, NULL, NULL, "Drive9RAM6000",
      (resource_value_t) 0,
      NULL, "Disable 8KB RAM expansion from $6000-$7FFF" },
    { "-drive8ram8000", SET_RESOURCE, 0, NULL, NULL, "Drive8RAM8000",
      (resource_value_t) 1,
      NULL, "Enable 8KB RAM expansion from $8000-$9FFF" },
    { "+drive8ram8000", SET_RESOURCE, 0, NULL, NULL, "Drive8RAM8000",
      (resource_value_t) 0,
      NULL, "Disable 8KB RAM expansion from $8000-$9FFF" },
    { "-drive9ram8000", SET_RESOURCE, 0, NULL, NULL, "Drive9RAM8000",
      (resource_value_t) 1,
      NULL, "Enable 8KB RAM expansion from $8000-$9FFF" },
    { "+drive9ram8000", SET_RESOURCE, 0, NULL, NULL, "Drive9RAM8000",
      (resource_value_t) 0,
      NULL, "Disable 8KB RAM expansion from $8000-$9FFF" },
    { "-drive8rama000", SET_RESOURCE, 0, NULL, NULL, "Drive8RAMA000",
      (resource_value_t) 1,
      NULL, "Enable 8KB RAM expansion from $A000-$BFFF" },
    { "+drive8rama000", SET_RESOURCE, 0, NULL, NULL, "Drive8RAMA000",
      (resource_value_t) 0,
      NULL, "Disable 8KB RAM expansion from $A000-$BFFF" },
    { "-drive9rama000", SET_RESOURCE, 0, NULL, NULL, "Drive9RAMA000",
      (resource_value_t) 1,
      NULL, "Enable 8KB RAM expansion from $A000-$BFFF" },
    { "+drive9rama000", SET_RESOURCE, 0, NULL, NULL, "Drive9RAMA000",
      (resource_value_t) 0,
      NULL, "Disable 8KB RAM expansion from $A000-$BFFF" },
    { NULL }
};

int drive_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

