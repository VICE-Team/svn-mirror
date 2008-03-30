/*
 * joystick.h - Joystick support for MS-DOS.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Thomas Bretz (tbretz@gsi.de)
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

#ifndef _VICE_JOYSTICK_H
#define _VICE_JOYSTICK_H

#include "kbd.h"

#include <stddef.h>

typedef enum {
    JOYDEV_NONE,
    JOYDEV_NUMPAD,
    JOYDEV_KEYSET1,
    JOYDEV_KEYSET2,
    JOYDEV_HW1,
    JOYDEV_HW2
} joystick_device_t;

typedef enum {
    KEYSET_NW,
    KEYSET_N,
    KEYSET_NE,
    KEYSET_E,
    KEYSET_SE,
    KEYSET_S,
    KEYSET_SW,
    KEYSET_W,
    KEYSET_FIRE
} joystick_direction_t;

extern void joystick_init(void);
extern void joystick_close(void);
extern int  joystick_init_resources(void);
extern int  joystick_init_cmdline_options(void);
extern void joystick_update(void);
extern int  joystick_handle_key(kbd_code_t kcode, int pressed);
extern const char *joystick_direction_to_string(joystick_direction_t
                                                direction);

// ---------- OS/2 specific stuff ---------

#define IOCTL_CAT_USER           0x80

#define GAME_GET_VERSION         0x01
#define GAME_GET_PARMS           0x02
#define GAME_SET_PARMS           0x03
#define GAME_GET_CALIB           0x04
#define GAME_SET_CALIB           0x05
#define GAME_GET_DIGSET          0x06
#define GAME_SET_DIGSET          0x07

#define GAME_GET_STATUS          0x10
#define GAME_GET_STATUS_BUTWAIT  0x11
#define GAME_GET_STATUS_SAMPWAIT 0x12

/* in use bitmasks originating in 1.0 */
#define GAME_USE_BOTH_OLDMASK    0x01 /* for backward compat with bool */
#define GAME_USE_X_NEWMASK       0x02
#define GAME_USE_Y_NEWMASK       0x04
#define GAME_USE_X_EITHERMASK (GAME_USE_X_NEWMASK|GAME_USE_BOTH_OLDMASK)
#define GAME_USE_Y_EITHERMASK (GAME_USE_Y_NEWMASK|GAME_USE_BOTH_OLDMASK)
#define GAME_USE_BOTH_NEWMASK (GAME_USE_X_NEWMASK|GAME_USE_Y_NEWMASK)

/* only timed sampling implemented in version 1.0 */
#define GAME_MODE_TIMED   1 /* timed sampling */
#define GAME_MODE_REQUEST 2 /* request driven sampling */

/* only raw implemented in version 1.0 */
#define GAME_DATA_FORMAT_RAW    1 /* [l,c,r]   */
#define GAME_DATA_FORMAT_SIGNED 2 /* [-l,0,+r] */
#define GAME_DATA_FORMAT_BINARY 3 /* {-1,0,+1} */
#define GAME_DATA_FORMAT_SCALED 4 /* [-10,+10] */

#define JOYA_BUT1   0x10
#define JOYA_BUT2   0x20
#define JOYB_BUT1   0x40
#define JOYB_BUT2   0x80

// simple 2-D position for each joystick
typedef struct
{
    SHORT x;
    SHORT y;
} GAME_2DPOS_STRUCT;

// struct defining the instantaneous state of both sticks and all buttons
typedef struct
{
  GAME_2DPOS_STRUCT A;       // Joystick A
  GAME_2DPOS_STRUCT B;       // Joystick B
  USHORT            butMask; // Buttons
} GAME_DATA_STRUCT;

// status struct returned to OS/2 applications:
// current data for all sticks as well as button counts since last read
typedef struct
{
  GAME_DATA_STRUCT curdata;
  USHORT b1cnt;
  USHORT b2cnt;
  USHORT b3cnt;
  USHORT b4cnt;
} GAME_STATUS_STRUCT;
// ---------- OS/2 specific stuff ---------


#endif
