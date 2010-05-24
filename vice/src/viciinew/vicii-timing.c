/*
 * vicii-timing.c - Timing related settings for the MOS 6569 (VIC-II) emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Gunnar Ruthenberg <Krill.Plush@gmail.com>
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

#include "c64.h"
#include "machine.h"
#include "resources.h"
#include "vicii-timing.h"
#include "vicii.h"
#include "viciitypes.h"


/* Screen constants.  */
#define VICII_PAL_SCREEN_HEIGHT                      312
#define VICII_NTSC_SCREEN_HEIGHT                     263
#define VICII_NTSCOLD_SCREEN_HEIGHT                  262
#define VICII_PALN_SCREEN_HEIGHT                     312

/* Sideborder sizes */
#define VICII_SCREEN_PAL_NORMAL_LEFTBORDERWIDTH      0x20
#define VICII_SCREEN_PAL_NORMAL_RIGHTBORDERWIDTH     0x20
#define VICII_SCREEN_PAL_FULL_LEFTBORDERWIDTH        0x30 /* actually 0x2e, but must be divisible by 8 */
#define VICII_SCREEN_PAL_FULL_RIGHTBORDERWIDTH       0x24
#define VICII_SCREEN_PAL_DEBUG_LEFTBORDERWIDTH       0x88
#define VICII_SCREEN_PAL_DEBUG_RIGHTBORDERWIDTH      0x30

#define VICII_SCREEN_NTSC_NORMAL_LEFTBORDERWIDTH     0x20
#define VICII_SCREEN_NTSC_NORMAL_RIGHTBORDERWIDTH    0x20
#define VICII_SCREEN_NTSC_FULL_LEFTBORDERWIDTH       0x38
#define VICII_SCREEN_NTSC_FULL_RIGHTBORDERWIDTH      0x2c
#define VICII_SCREEN_NTSC_DEBUG_LEFTBORDERWIDTH      0x88
#define VICII_SCREEN_NTSC_DEBUG_RIGHTBORDERWIDTH     0x40

#define VICII_SCREEN_NTSCOLD_NORMAL_LEFTBORDERWIDTH  0x20
#define VICII_SCREEN_NTSCOLD_NORMAL_RIGHTBORDERWIDTH 0x20
#define VICII_SCREEN_NTSCOLD_FULL_LEFTBORDERWIDTH    0x38
#define VICII_SCREEN_NTSCOLD_FULL_RIGHTBORDERWIDTH   0x2c
#define VICII_SCREEN_NTSCOLD_DEBUG_LEFTBORDERWIDTH   0x88
#define VICII_SCREEN_NTSCOLD_DEBUG_RIGHTBORDERWIDTH  0x38

#define VICII_SCREEN_PALN_NORMAL_LEFTBORDERWIDTH     0x20
#define VICII_SCREEN_PALN_NORMAL_RIGHTBORDERWIDTH    0x20
#define VICII_SCREEN_PALN_FULL_LEFTBORDERWIDTH       0x30 /* actually 0x2e, but must be divisible by 8 */
#define VICII_SCREEN_PALN_FULL_RIGHTBORDERWIDTH      0x24
#define VICII_SCREEN_PALN_DEBUG_LEFTBORDERWIDTH      0x88
#define VICII_SCREEN_PALN_DEBUG_RIGHTBORDERWIDTH     0x30

/* Y display ranges */
/* Note: If the last displayed line setting is larger than */
/* the screen height, lines 0+ are displayed in the lower */
/* border. This is used for NTSC display. */
#define VICII_PAL_NORMAL_FIRST_DISPLAYED_LINE        0x10
#define VICII_PAL_NORMAL_LAST_DISPLAYED_LINE         0x11f
#define VICII_PAL_FULL_FIRST_DISPLAYED_LINE          0x08
#define VICII_PAL_FULL_LAST_DISPLAYED_LINE           0x12c
#define VICII_PAL_DEBUG_FIRST_DISPLAYED_LINE         0x00
#define VICII_PAL_DEBUG_LAST_DISPLAYED_LINE          0x137

#define VICII_NTSC_NORMAL_FIRST_DISPLAYED_LINE       0x1c
#define VICII_NTSC_NORMAL_LAST_DISPLAYED_LINE        0x112
#define VICII_NTSC_FULL_FIRST_DISPLAYED_LINE         0x1e /* enough space to show full sprites in the upper border */
#define VICII_NTSC_FULL_LAST_DISPLAYED_LINE          0x10f /* enough space to show full sprites in the lower border */
#define VICII_NTSC_DEBUG_FIRST_DISPLAYED_LINE        0x14
#define VICII_NTSC_DEBUG_LAST_DISPLAYED_LINE         0x11a

#define VICII_NTSCOLD_NORMAL_FIRST_DISPLAYED_LINE    0x1c
#define VICII_NTSCOLD_NORMAL_LAST_DISPLAYED_LINE     0x112
#define VICII_NTSCOLD_FULL_FIRST_DISPLAYED_LINE      0x1e /* enough space to show full sprites in the upper border */
#define VICII_NTSCOLD_FULL_LAST_DISPLAYED_LINE       0x10f /* enough space to show full sprites in the lower border */
#define VICII_NTSCOLD_DEBUG_FIRST_DISPLAYED_LINE     0x14
#define VICII_NTSCOLD_DEBUG_LAST_DISPLAYED_LINE      0x119

#define VICII_PALN_NORMAL_FIRST_DISPLAYED_LINE       0x10
#define VICII_PALN_NORMAL_LAST_DISPLAYED_LINE        0x11f
#define VICII_PALN_FULL_FIRST_DISPLAYED_LINE         0x08
#define VICII_PALN_FULL_LAST_DISPLAYED_LINE          0x12c
#define VICII_PALN_DEBUG_FIRST_DISPLAYED_LINE        0x00
#define VICII_PALN_DEBUG_LAST_DISPLAYED_LINE         0x137

/* Number of cycles per line.  */
#define VICII_PAL_CYCLES_PER_LINE      C64_PAL_CYCLES_PER_LINE
#define VICII_NTSC_CYCLES_PER_LINE     C64_NTSC_CYCLES_PER_LINE
#define VICII_NTSCOLD_CYCLES_PER_LINE  C64_NTSCOLD_CYCLES_PER_LINE
#define VICII_PALN_CYCLES_PER_LINE     C64_PALN_CYCLES_PER_LINE


void vicii_timing_set(machine_timing_t *machine_timing, int border_mode)
{
    int mode;

    resources_get_int("MachineVideoStandard", &mode);

    switch (mode) {
      case MACHINE_SYNC_NTSC:
        vicii.screen_height = VICII_NTSC_SCREEN_HEIGHT;
        switch (border_mode) {
          default:
          case VICII_NORMAL_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_NTSC_NORMAL_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_NTSC_NORMAL_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_NTSC_NORMAL_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_NTSC_NORMAL_LAST_DISPLAYED_LINE;
            break;       
          case VICII_FULL_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_NTSC_FULL_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_NTSC_FULL_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_NTSC_FULL_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_NTSC_FULL_LAST_DISPLAYED_LINE;
            break;
          case VICII_DEBUG_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_NTSC_DEBUG_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_NTSC_DEBUG_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_NTSC_DEBUG_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_NTSC_DEBUG_LAST_DISPLAYED_LINE;
            break;
        }
        vicii.cycles_per_line = VICII_NTSC_CYCLES_PER_LINE;
        break;
      case MACHINE_SYNC_NTSCOLD:
        vicii.screen_height = VICII_NTSCOLD_SCREEN_HEIGHT;
        switch (border_mode) {
          default:
          case VICII_NORMAL_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_NTSCOLD_NORMAL_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_NTSCOLD_NORMAL_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_NTSCOLD_NORMAL_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_NTSCOLD_NORMAL_LAST_DISPLAYED_LINE;
            break;
          case VICII_FULL_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_NTSCOLD_FULL_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_NTSCOLD_FULL_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_NTSCOLD_FULL_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_NTSCOLD_FULL_LAST_DISPLAYED_LINE;
            break;
          case VICII_DEBUG_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_NTSCOLD_DEBUG_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_NTSCOLD_DEBUG_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_NTSCOLD_DEBUG_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_NTSCOLD_DEBUG_LAST_DISPLAYED_LINE;
            break;
        }
        vicii.cycles_per_line = VICII_NTSCOLD_CYCLES_PER_LINE;
        break;
      case MACHINE_SYNC_PALN:
        vicii.screen_height = VICII_PALN_SCREEN_HEIGHT;
        switch (border_mode) {
          default:
          case VICII_NORMAL_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_PALN_NORMAL_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_PALN_NORMAL_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_PALN_NORMAL_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_PALN_NORMAL_LAST_DISPLAYED_LINE;
            break;
          case VICII_FULL_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_PALN_FULL_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_PALN_FULL_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_PALN_FULL_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_PALN_FULL_LAST_DISPLAYED_LINE;
            break;
          case VICII_DEBUG_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_PALN_DEBUG_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_PALN_DEBUG_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_PALN_DEBUG_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_PALN_DEBUG_LAST_DISPLAYED_LINE;
            break;
        }
        vicii.cycles_per_line = VICII_PALN_CYCLES_PER_LINE;
        break;
      case MACHINE_SYNC_PAL:
      default:
        vicii.screen_height = VICII_PAL_SCREEN_HEIGHT;
        switch (border_mode) {
          default:
          case VICII_NORMAL_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_PAL_NORMAL_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_PAL_NORMAL_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_PAL_NORMAL_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_PAL_NORMAL_LAST_DISPLAYED_LINE;
            break;
          case VICII_FULL_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_PAL_FULL_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_PAL_FULL_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_PAL_FULL_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_PAL_FULL_LAST_DISPLAYED_LINE;
            break;
          case VICII_DEBUG_BORDERS:
            vicii.screen_leftborderwidth = VICII_SCREEN_PAL_DEBUG_LEFTBORDERWIDTH;
            vicii.screen_rightborderwidth = VICII_SCREEN_PAL_DEBUG_RIGHTBORDERWIDTH;
            vicii.first_displayed_line = VICII_PAL_DEBUG_FIRST_DISPLAYED_LINE;
            vicii.last_displayed_line = VICII_PAL_DEBUG_LAST_DISPLAYED_LINE;
            break;
        }
        vicii.cycles_per_line = VICII_PAL_CYCLES_PER_LINE;
        break;
    }

}
