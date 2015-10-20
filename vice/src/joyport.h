/*
 * joyport.h - joyport abstraction system.
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

#ifndef VICE_JOYPORT_H
#define VICE_JOYPORT_H

#include "types.h"

#define JOYPORT_ID_NONE                0
#define JOYPORT_ID_JOY1                1
#define JOYPORT_ID_JOY2                2
#define JOYPORT_ID_JOY3                3
#define JOYPORT_ID_JOY4                4
#define JOYPORT_ID_PADDLES             5
#define JOYPORT_ID_MOUSE_1351          6
#define JOYPORT_ID_MOUSE_NEOS          7
#define JOYPORT_ID_MOUSE_AMIGA         8
#define JOYPORT_ID_MOUSE_CX22          9
#define JOYPORT_ID_MOUSE_ST           10
#define JOYPORT_ID_MOUSE_SMART        11
#define JOYPORT_ID_MOUSE_MICROMYS     12
#define JOYPORT_ID_KOALAPAD           13
#define JOYPORT_ID_LIGHTPEN_U         14
#define JOYPORT_ID_LIGHTPEN_L         15
#define JOYPORT_ID_LIGHTPEN_DATEL     16
#define JOYPORT_ID_LIGHTGUN_Y         17
#define JOYPORT_ID_LIGHTGUN_L         18
#define JOYPORT_ID_LIGHTPEN_INKWELL   19
#define JOYPORT_ID_SAMPLER            20
#define JOYPORT_ID_COPLIN_KEYPAD      21
#define JOYPORT_ID_CARDCO_KEYPAD      22
#define JOYPORT_ID_CX85_KEYPAD        23
#define JOYPORT_ID_BBRTC              24

#define JOYPORT_MAX_DEVICES           25

#define JOYPORT_RES_ID_NONE      0
#define JOYPORT_RES_ID_MOUSE     1
#define JOYPORT_RES_ID_SAMPLER   2
#define JOYPORT_RES_ID_KEYPAD    3
#define JOYPORT_RES_ID_RTC       4

#define JOYPORT_1    0
#define JOYPORT_2    1

#define JOYPORT_POT_NONE      0
#define JOYPORT_POT_PRESENT   1

#define JOYPORT_PORTS_1       1
#define JOYPORT_PORTS_2       2

typedef struct joyport_s {
    char *name;
    int resource_id;
    int (*enable)(int val);
    BYTE (*read_digital)(void);
    void (*store_digital)(BYTE val);
    BYTE (*read_potx)(void);
    BYTE (*read_poty)(void);
} joyport_t;

extern int joyport_register(int id, joyport_t *device);

extern BYTE read_joyport_dig(int port);
extern void store_joyport_dig(int port, BYTE val);
extern BYTE read_joyport_potx(void);
extern BYTE read_joyport_poty(void);

extern void set_joyport_pot_mask(int mask);

extern int sampler_joyport_register(void);
extern int keypad_coplin_joyport_register(void);
extern int keypad_cardco_joyport_register(void);
extern int keypad_cx85_joyport_register(void);
extern int bbrtc_joyport_register(void);

extern int joyport_resources_init(int pot_present, int ports);
extern int joyport_cmdline_options_init(void);

#endif
