/*
 * archdep_dingoo.h
 *
 * Written by
 *  peiselulli <peiselulli@t-online.de>
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

#ifndef VICE_ARCHDEP_DINGOO_H
#define VICE_ARCHDEP_DINGOO_H

#include "archapi.h"

/* Filesystem dependant operators.  */
#define FSDEVICE_DEFAULT_DIR "."
#define FSDEV_DIR_SEP_STR    "\\"
#define FSDEV_DIR_SEP_CHR    '\\'
#define FSDEV_EXT_SEP_STR    "."
#define FSDEV_EXT_SEP_CHR    '.'

/* Path separator.  */
#define ARCHDEP_FINDPATH_SEPARATOR_CHAR   ':'
#define ARCHDEP_FINDPATH_SEPARATOR_STRING ":"

/* Modes for fopen().  */
#define MODE_READ              "r"
#define MODE_READ_TEXT         "rt"
#define MODE_READ_WRITE        "r+"
#define MODE_WRITE             "w"
#define MODE_WRITE_TEXT        "wt"
#define MODE_APPEND            "a"
#define MODE_APPEND_READ_WRITE "a+"

/* Printer default devices.  */
#define ARCHDEP_PRINTER_DEFAULT_DEV1 "print.dump"
#define ARCHDEP_PRINTER_DEFAULT_DEV2 "|lpr"
#define ARCHDEP_PRINTER_DEFAULT_DEV3 "|petlp -F PS|lpr"

/* Default RS232 devices.  */
#define ARCHDEP_RS232_DEV1 "10.0.0.1:25232"
#define ARCHDEP_RS232_DEV2 "10.0.0.1:25232"
#define ARCHDEP_RS232_DEV3 "10.0.0.1:25232"
#define ARCHDEP_RS232_DEV4 "10.0.0.1:25232"

/* Default location of raw disk images.  */
#define ARCHDEP_RAWDRIVE_DEFAULT "C:"

/* Access types */
#define ARCHDEP_R_OK 0
#define ARCHDEP_W_OK 0
#define ARCHDEP_X_OK 0
#define ARCHDEP_F_OK 0

/* Standard line delimiter.  */
#define ARCHDEP_LINE_DELIMITER "\r\n"

/* Ethernet default device */
#define ARCHDEP_ETHERNET_DEFAULT_DEVICE ""

/* Default sound fragment size */
#define ARCHDEP_SOUND_FRAGMENT_SIZE SOUND_FRAGMENT_MEDIUM

#define is_lcd_active() ((REG_LCD_CFG & LCD_CFG_LCDPIN_MASK) >> LCD_CFG_LCDPIN_BIT)

#define archdep_file_is_blockdev (0)
#define archdep_file_is_chardev (0)
#define archdep_spawn(name, argv, pstdout_redir, stderr_redir) (-1)
#define archdep_mkdir mkdir

extern const char *archdep_home_path(void);
extern void set_overclock(int activate);
extern void set_dingoo_pwd(const char *path);

/* set this path to customize the preference storage */
extern const char *archdep_pref_path;

extern char *make_absolute_system_path(const char *s);

/* special video timings for dingoo */

/* this values are not the originals from
   vicii-timing.h, because the screen of
   the dingoo is only 320x240 in size.
   To make the rendering faster, the
   borders are as small as possible
   (without crashing the emulator)
*/

#define VICE_VICII_TIMING_H

/* Screen constants.  */
#define VICII_PAL_SCREEN_HEIGHT                      312
#define VICII_NTSC_SCREEN_HEIGHT                     263
#define VICII_NTSCOLD_SCREEN_HEIGHT                  262
#define VICII_PALN_SCREEN_HEIGHT                     312

/* Sideborder sizes */
#define VICII_SCREEN_PAL_NORMAL_LEFTBORDERWIDTH      0x08
#define VICII_SCREEN_PAL_NORMAL_RIGHTBORDERWIDTH     0x08
#define VICII_SCREEN_PAL_FULL_LEFTBORDERWIDTH        0x08
#define VICII_SCREEN_PAL_FULL_RIGHTBORDERWIDTH       0x08
#define VICII_SCREEN_PAL_DEBUG_LEFTBORDERWIDTH       0x08
#define VICII_SCREEN_PAL_DEBUG_RIGHTBORDERWIDTH      0x08

#define VICII_SCREEN_NTSC_NORMAL_LEFTBORDERWIDTH     0x08
#define VICII_SCREEN_NTSC_NORMAL_RIGHTBORDERWIDTH    0x08
#define VICII_SCREEN_NTSC_FULL_LEFTBORDERWIDTH       0x08
#define VICII_SCREEN_NTSC_FULL_RIGHTBORDERWIDTH      0x08
#define VICII_SCREEN_NTSC_DEBUG_LEFTBORDERWIDTH      0x08
#define VICII_SCREEN_NTSC_DEBUG_RIGHTBORDERWIDTH     0x08

#define VICII_SCREEN_NTSCOLD_NORMAL_LEFTBORDERWIDTH  0x08
#define VICII_SCREEN_NTSCOLD_NORMAL_RIGHTBORDERWIDTH 0x08
#define VICII_SCREEN_NTSCOLD_FULL_LEFTBORDERWIDTH    0x08
#define VICII_SCREEN_NTSCOLD_FULL_RIGHTBORDERWIDTH   0x08
#define VICII_SCREEN_NTSCOLD_DEBUG_LEFTBORDERWIDTH   0x08
#define VICII_SCREEN_NTSCOLD_DEBUG_RIGHTBORDERWIDTH  0x08

#define VICII_SCREEN_PALN_NORMAL_LEFTBORDERWIDTH     0x08
#define VICII_SCREEN_PALN_NORMAL_RIGHTBORDERWIDTH    0x08
#define VICII_SCREEN_PALN_FULL_LEFTBORDERWIDTH       0x08
#define VICII_SCREEN_PALN_FULL_RIGHTBORDERWIDTH      0x08
#define VICII_SCREEN_PALN_DEBUG_LEFTBORDERWIDTH      0x08
#define VICII_SCREEN_PALN_DEBUG_RIGHTBORDERWIDTH     0x08


/* Y display ranges */
/* Notes:
   - If the last displayed line setting is larger than
     the screen height, lines 0+ are displayed in the lower
     border. This is used for NTSC display.
   - "normal" shows all lines visible on a typical monitor
   - "full" shows all lines minus the vertical retrace
   - "debug" mode shows all lines, including vertical retrace
*/
#define VICII_NO_BORDER_FIRST_DISPLAYED_LINE         51
#define VICII_NO_BORDER_LAST_DISPLAYED_LINE          250

#define VICII_PAL_NORMAL_FIRST_DISPLAYED_LINE        0x10   /* 16 */
#define VICII_PAL_NORMAL_LAST_DISPLAYED_LINE         0x11f  /* 287 */
#define VICII_PAL_FULL_FIRST_DISPLAYED_LINE          0x08   /* 8 */
#define VICII_PAL_FULL_LAST_DISPLAYED_LINE           0x12c  /* 300 */
#define VICII_PAL_DEBUG_FIRST_DISPLAYED_LINE         0x00   /* 0 */
#define VICII_PAL_DEBUG_LAST_DISPLAYED_LINE          0x137  /* 311 */

/* Y display ranges */
/* Notes:
   - If the last displayed line setting is larger than
     the screen height, lines 0+ are displayed in the lower
     border. This is used for NTSC display.
   - "normal" shows all lines visible on a typical monitor
   - "full" shows all lines minus the vertical retrace
   - "debug" mode shows all lines, including vertical retrace
*/
#define VICII_PAL_NORMAL_FIRST_DISPLAYED_LINE        0x10   /* 16 */
#define VICII_PAL_NORMAL_LAST_DISPLAYED_LINE         0x11f  /* 287 */
#define VICII_PAL_FULL_FIRST_DISPLAYED_LINE          0x08   /* 8 */
#define VICII_PAL_FULL_LAST_DISPLAYED_LINE           0x12c  /* 300 */
#define VICII_PAL_DEBUG_FIRST_DISPLAYED_LINE         0x00   /* 0 */
#define VICII_PAL_DEBUG_LAST_DISPLAYED_LINE          0x137  /* 311 */

/*
NTSC display ranges:
*/

#define VICII_NTSC_NORMAL_FIRST_DISPLAYED_LINE       (0x32 - 20)
#define VICII_NTSC_NORMAL_LAST_DISPLAYED_LINE        (0x32 + 220)
#define VICII_NTSC_FULL_FIRST_DISPLAYED_LINE         (0x32 - 20)
#define VICII_NTSC_FULL_LAST_DISPLAYED_LINE          (0x32 + 220)
#define VICII_NTSC_DEBUG_FIRST_DISPLAYED_LINE        (0x32 - 20)
#define VICII_NTSC_DEBUG_LAST_DISPLAYED_LINE         (0x32 + 220)

#define VICII_NTSCOLD_NORMAL_FIRST_DISPLAYED_LINE    (0x32 - 20)
#define VICII_NTSCOLD_NORMAL_LAST_DISPLAYED_LINE     (0x32 + 220)
#define VICII_NTSCOLD_FULL_FIRST_DISPLAYED_LINE      (0x32 - 20)
#define VICII_NTSCOLD_FULL_LAST_DISPLAYED_LINE       (0x32 + 220)
#define VICII_NTSCOLD_DEBUG_FIRST_DISPLAYED_LINE     (0x32 - 20)
#define VICII_NTSCOLD_DEBUG_LAST_DISPLAYED_LINE      (0x32 + 220)

#define VICII_PALN_NORMAL_FIRST_DISPLAYED_LINE       (0x32 - 20)
#define VICII_PALN_NORMAL_LAST_DISPLAYED_LINE        (0x32 + 220)
#define VICII_PALN_FULL_FIRST_DISPLAYED_LINE         (0x32 - 20)
#define VICII_PALN_FULL_LAST_DISPLAYED_LINE          (0x32 + 220)
#define VICII_PALN_DEBUG_FIRST_DISPLAYED_LINE        (0x32 - 20)
#define VICII_PALN_DEBUG_LAST_DISPLAYED_LINE         (0x32 + 220)


#define VICII_SCREEN_PAL_NORMAL_WIDTH  (320 + VICII_SCREEN_PAL_NORMAL_LEFTBORDERWIDTH + VICII_SCREEN_PAL_NORMAL_RIGHTBORDERWIDTH)
#define VICII_SCREEN_PAL_NORMAL_HEIGHT (1 + (VICII_PAL_NORMAL_LAST_DISPLAYED_LINE - VICII_PAL_NORMAL_FIRST_DISPLAYED_LINE))
#define VICII_SCREEN_PALN_NORMAL_WIDTH  (320 + VICII_SCREEN_PALN_NORMAL_LEFTBORDERWIDTH + VICII_SCREEN_PALN_NORMAL_RIGHTBORDERWIDTH)
#define VICII_SCREEN_PALN_NORMAL_HEIGHT (1 + (VICII_PALN_NORMAL_LAST_DISPLAYED_LINE - VICII_PALN_NORMAL_FIRST_DISPLAYED_LINE))
#define VICII_SCREEN_NTSC_NORMAL_WIDTH  (320 + VICII_SCREEN_NTSC_NORMAL_LEFTBORDERWIDTH + VICII_SCREEN_NTSC_NORMAL_RIGHTBORDERWIDTH)
#define VICII_SCREEN_NTSC_NORMAL_HEIGHT (1 + (VICII_NTSC_NORMAL_LAST_DISPLAYED_LINE - VICII_NTSC_NORMAL_FIRST_DISPLAYED_LINE))
#define VICII_SCREEN_NTSCOLD_NORMAL_WIDTH  (320 + VICII_SCREEN_NTSCOLD_NORMAL_LEFTBORDERWIDTH + VICII_SCREEN_NTSCOLD_NORMAL_RIGHTBORDERWIDTH)
#define VICII_SCREEN_NTSCOLD_NORMAL_HEIGHT (1 + (VICII_NTSCOLD_NORMAL_LAST_DISPLAYED_LINE - VICII_NTSCOLD_NORMAL_FIRST_DISPLAYED_LINE))

struct machine_timing_s;

extern void vicii_timing_set(struct machine_timing_s *machine_timing,
                             int border_mode);

/* end of special video timings for dingoo */

/*
    FIXME: confirm wether SIGPIPE must be handled or not. if the emulator quits
           or crashes when the connection is closed, you might have to install
           a signal handler which calls monitor_abort().

           see archdep_unix.c and bug #3201796
*/
#define archdep_signals_init(x)
#define archdep_signals_pipe_set()
#define archdep_signals_pipe_unset()

#define BUFSIZ 512 /* for ATA */

/* what to use to return an error when a socket error happens */
#define ARCHDEP_SOCKET_ERROR errno

/* Keyword to use for a static prototype */
#define STATIC_PROTOTYPE static

#endif
