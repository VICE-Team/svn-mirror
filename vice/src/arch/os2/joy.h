/*
 * joy.h - Joystick support for MS-DOS.
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#ifndef JOY_H
#define JOY_H

#include "kbd.h"

enum joystick_bits_s {
    CBM_NORTH = 1,
    CBM_SOUTH = 2,
    CBM_WEST = 4,
    CBM_EAST = 8,
    CBM_FIRE = 16
};
typedef enum joystick_bits_s joystick_bits_t;

extern int joy_arch_init(void);
extern void joystick_close(void);

extern void joystick_update(void);
extern int joystick_handle_key(kbd_code_t kcode, int pressed);

// --------------------- OS/2 specific stuff -----------------------------

typedef int joystick_device_t;

#define JOYDEV_NONE    0x00
#define JOYDEV_HW1     0x01
#define JOYDEV_HW2     0x02
#define JOYDEV_NUMPAD  0x04
#define JOYDEV_KEYSET1 0x08
#define JOYDEV_KEYSET2 0x10

int set_joyA_autoCal(const char *value, void *extra_param);
int set_joyB_autoCal(const char *value, void *extra_param);
int get_joy_autoCal(const int nr);

/****************************************************************************/
#define GAMEPDDNAME "GAME$   "
/****************************************************************************/

/****************************************************************************/
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
/****************************************************************************/

/* in use bitmasks originating in 1.0 */
#define GAME_USE_BOTH_OLDMASK 0x01 /* for backward compat with bool */
#define GAME_USE_X_NEWMASK    0x02
#define GAME_USE_Y_NEWMASK    0x04
#define GAME_USE_X_EITHERMASK (GAME_USE_X_NEWMASK | GAME_USE_BOTH_OLDMASK)
#define GAME_USE_Y_EITHERMASK (GAME_USE_Y_NEWMASK | GAME_USE_BOTH_OLDMASK)
#define GAME_USE_BOTH_NEWMASK (GAME_USE_X_NEWMASK | GAME_USE_Y_NEWMASK)

/****************************************************************************/
#define JOY_AX_BIT      0x01
#define JOY_AY_BIT      0x02
#define JOY_A_BITS      (JOY_AX_BIT | JOY_AY_BIT)
#define JOY_BX_BIT      0x04
#define JOY_BY_BIT      0x08
#define JOY_B_BITS      (JOY_BX_BIT | JOY_BY_BIT)
#define JOY_ALLPOS_BITS (JOY_A_BITS | JOY_B_BITS)

#define JOYA_BUT1    0x10
#define JOYA_BUT2    0x20
#define JOYA_BUTTONS (JOYA_BUT1 | JOYA_BUT2)
#define JOYB_BUT1    0x40
#define JOYB_BUT2    0x80
#define JOYB_BUTTONS (JOYB_BUT1 | JOYB_BUT2)
#define JOY_BUTTONS  (JOYA_BUTTONS | JOYB_BUTTONS)

/****************************************************************************/

/****************************************************************************/
typedef signed short GAME_POS;	/* some data formats require signed values */

// simple 2-D position for each joystick
typedef struct {
    GAME_POS x;
    GAME_POS y;
} GAME_2DPOS_STRUCT;

// struct defining the instantaneous state of both sticks and all buttons
typedef struct {
    GAME_2DPOS_STRUCT A;
    GAME_2DPOS_STRUCT B;
    unsigned short butMask;
} GAME_DATA_STRUCT;

typedef struct {
    GAME_POS lower;
    GAME_POS centre;
    GAME_POS upper;
} GAME_3POS_STRUCT;

// status struct returned to OS/2 applications:
// current data for all sticks as well as button counts since last read
typedef struct {
    GAME_3POS_STRUCT Ax;
    GAME_3POS_STRUCT Ay;
    GAME_3POS_STRUCT Bx;
    GAME_3POS_STRUCT By;
} GAME_DIGSET_STRUCT;
/****************************************************************************/

/****************************************************************************/
typedef struct {
    GAME_DATA_STRUCT curdata;
    unsigned short b1cnt;
    unsigned short b2cnt;
    unsigned short b3cnt;
    unsigned short b4cnt;
} GAME_STATUS_STRUCT;

/* only timed sampling implemented in version 1.0 */
#define GAME_MODE_TIMED   1	/* timed sampling */
#define GAME_MODE_REQUEST 2	/* request driven sampling */

/* only raw implemented in version 1.0 */
#define GAME_DATA_FORMAT_RAW    1 /* [l,c,r]   */
#define GAME_DATA_FORMAT_SIGNED 2 /* [-l,0,+r] */
#define GAME_DATA_FORMAT_BINARY 3 /* {-1,0,+1} */
#define GAME_DATA_FORMAT_SCALED 4 /* [-10,+10] */

typedef struct {
    unsigned short useA;     /* bool, !0 = in use */
    unsigned short useB;
    unsigned short mode;     /* see consts above */
    unsigned short format;   /* see consts above */
    unsigned short sampDiv;  /* samp freq = 32 / n */
    unsigned short scale;    /* scaling factor */
    unsigned short res1;
    unsigned short res2;
} GAME_PARM_STRUCT;

typedef struct {
    GAME_3POS_STRUCT Ax;
    GAME_3POS_STRUCT Ay;
    GAME_3POS_STRUCT Bx;
    GAME_3POS_STRUCT By;
} GAME_CALIB_STRUCT;
/****************************************************************************/

#endif
