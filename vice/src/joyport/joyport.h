/*
 * joyport.h - control port abstraction system.
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

#include "snapshot.h"
#include "types.h"

#define JOYPORT_ID_JOY1               -1
#define JOYPORT_ID_JOY2               -2
#define JOYPORT_ID_JOY3               -3
#define JOYPORT_ID_JOY4               -4
#define JOYPORT_ID_JOY5               -5
#define JOYPORT_ID_JOY6               -6
#define JOYPORT_ID_JOY7               -7
#define JOYPORT_ID_JOY8               -8
#define JOYPORT_ID_JOY9               -9
#define JOYPORT_ID_JOY10              -10

/* #define JOYPORT_EXPERIMENTAL_DEVICES */

enum {
    JOYPORT_ID_NONE = 0,
    JOYPORT_ID_JOYSTICK,
    JOYPORT_ID_PADDLES,
    JOYPORT_ID_MOUSE_1351,
    JOYPORT_ID_MOUSE_NEOS,
    JOYPORT_ID_MOUSE_AMIGA,
    JOYPORT_ID_MOUSE_CX22,
    JOYPORT_ID_MOUSE_ST,
    JOYPORT_ID_MOUSE_SMART,
    JOYPORT_ID_MOUSE_MICROMYS,
    JOYPORT_ID_KOALAPAD,
    JOYPORT_ID_LIGHTPEN_U,
    JOYPORT_ID_LIGHTPEN_L,
    JOYPORT_ID_LIGHTPEN_DATEL,
    JOYPORT_ID_LIGHTGUN_Y,
    JOYPORT_ID_LIGHTGUN_L,
    JOYPORT_ID_LIGHTPEN_INKWELL,
#ifdef JOYPORT_EXPERIMENTAL_DEVICES
    JOYPORT_ID_LIGHTGUN_GUNSTICK,
#endif
    JOYPORT_ID_SAMPLER_2BIT,
    JOYPORT_ID_SAMPLER_4BIT,
    JOYPORT_ID_BBRTC,
    JOYPORT_ID_PAPERCLIP64,
    JOYPORT_ID_COPLIN_KEYPAD,
    JOYPORT_ID_CARDCO_KEYPAD,
    JOYPORT_ID_CX85_KEYPAD,
    JOYPORT_ID_RUSHWARE_KEYPAD,
    JOYPORT_ID_CX21_KEYPAD,
    JOYPORT_ID_SCRIPT64_DONGLE,
    JOYPORT_ID_VIZAWRITE64_DONGLE,
    JOYPORT_ID_WAASOFT_DONGLE,
    JOYPORT_ID_TRAPTHEM_SNESPAD,
    JOYPORT_ID_NINJA_SNESPAD,
    JOYPORT_ID_SPACEBALLS,
    JOYPORT_ID_INCEPTION,
    JOYPORT_ID_MULTIJOY_JOYSTICKS,
    JOYPORT_ID_MULTIJOY_CONTROL,
    JOYPORT_MAX_DEVICES
};

#define JOYPORT_RES_ID_NONE        0
#define JOYPORT_RES_ID_MOUSE       1
#define JOYPORT_RES_ID_SAMPLER     2
#define JOYPORT_RES_ID_KEYPAD      3
#define JOYPORT_RES_ID_RTC         4
#define JOYPORT_RES_ID_PAPERCLIP64 5
#define JOYPORT_RES_ID_SCRIPT64    6
#define JOYPORT_RES_ID_VIZAWRITE64 7
#define JOYPORT_RES_ID_WAASOFT     8

#define JOYPORT_1   0   /**< c64/c128/c64dtv/scpu64/cbm5x0/plus4 control port 1,
                             vic20 control port */
#define JOYPORT_2   1   /**< c64/c128/c64dtv/scpu64/cbm5x0/plus4 control port 2 */
#define JOYPORT_3   2   /**< c64/c128/c64dtv/scpu64/cbm2/pet/plus4/vic20 userport
                             joystick adapter port 1 */
#define JOYPORT_4   3   /**< c64/c128/scpu64/cbm2/pet/plus4/vic20 userport
                             joystick adapter port 2 */
#define JOYPORT_5   4   /**< plus4 sidcart control port */
#define JOYPORT_6   5
#define JOYPORT_7   6
#define JOYPORT_8   7
#define JOYPORT_9   8
#define JOYPORT_10  9

#define JOYPORT_MAX_PORTS     10

#define JOYPORT_IS_NOT_LIGHTPEN   0
#define JOYPORT_IS_LIGHTPEN       1

#define JOYPORT_POT_REQUIRED   0
#define JOYPORT_POT_OPTIONAL   1

#define JOYSTICK_ADAPTER_ID_NONE                  0
#define JOYSTICK_ADAPTER_ID_GENERIC_USERPORT      1
#define JOYSTICK_ADAPTER_ID_TRAPTHEM_SNES         2
#define JOYSTICK_ADAPTER_ID_NINJA_SNES            3
#define JOYSTICK_ADAPTER_ID_USERPORT_PETSCII_SNES 4
#define JOYSTICK_ADAPTER_ID_USERPORT_SUPERPAD64   5
#define JOYSTICK_ADAPTER_ID_SPACEBALLS            6
#define JOYSTICK_ADAPTER_ID_MULTIJOY              7
#define JOYSTICK_ADAPTER_ID_INCEPTION             8

#define JOYPORT_DEVICE_TYPE_NONE          0
#define JOYPORT_DEVICE_JOYSTICK           1
#define JOYPORT_DEVICE_JOYSTICK_ADAPTER   2
#define JOYPORT_DEVICE_SNES_ADAPTER       3
#define JOYPORT_DEVICE_MOUSE              4
#define JOYPORT_DEVICE_LIGHTPEN           5
#define JOYPORT_DEVICE_LIGHTGUN           6
#define JOYPORT_DEVICE_DRAWING_PAD        7
#define JOYPORT_DEVICE_KEYPAD             8
#define JOYPORT_DEVICE_SAMPLER            9
#define JOYPORT_DEVICE_RTC                10
#define JOYPORT_DEVICE_DONGLE             11

/* this structure is used for control port devices */
typedef struct joyport_s {
    char *name;                                            /* name of the device */
    int resource_id;                                       /* type of device, to determine if there can be multiple instances of the type of device */
    int is_lp;                                             /* flag to indicate the device is a lightpen */
    int pot_optional;                                      /* flag to indicate that the device can work without a potentiometer */
    int joystick_adapter_id;                               /* flag to indicate that the device is a joystick/pad adapter */
    int device_type;                                       /* device type */
    int (*enable)(int port, int val);                      /* pointer to the device enable function */
    uint8_t (*read_digital)(int port);                     /* pointer to the device digital lines read function */
    void (*store_digital)(uint8_t val, uint8_t mask);      /* pointer to the device digital lines store function */
    uint8_t (*read_potx)(int port);                        /* pointer to the device X potentiometer read function */
    uint8_t (*read_poty)(int port);                        /* pointer to the device Y potentiometer read function */
    int (*write_snapshot)(struct snapshot_s *s, int port); /* pointer to the device snapshot write function */
    int (*read_snapshot)(struct snapshot_s *s, int port);  /* pointer to the device snapshot read function */
} joyport_t;

typedef struct joyport_desc_s {
    char *name;
    int id;
    int device_type;
} joyport_desc_t;

/* this structure is used for control ports */
typedef struct joyport_port_props_s {
    char *name;              /* name of the port */
    int has_pot;             /* flag to indicate that the port has potentiometer support */
    int has_lp_support;      /* flag to indicate that the port has lightpen support */
    int has_adapter_support; /* flag to indicate that the port can handle joystick adapters */
    int active;              /* flag to indicate if the port is currently active */
} joyport_port_props_t;

extern int joyport_device_register(int id, joyport_t *device);

extern uint8_t read_joyport_dig(int port);
extern void store_joyport_dig(int port, uint8_t val, uint8_t mask);
extern uint8_t read_joyport_potx(void);
extern uint8_t read_joyport_poty(void);

extern void set_joyport_pot_mask(int mask);

extern int joyport_resources_init(void);
extern int joyport_cmdline_options_init(void);

extern int joyport_port_register(int port, joyport_port_props_t *props);

extern joyport_desc_t *joyport_get_valid_devices(int port, int sort);

extern void joyport_display_joyport(int id, uint16_t status);

extern char *joyport_get_port_name(int port);

extern void joyport_clear_devices(void);

extern int joyport_port_is_active(int port);

extern char *joystick_adapter_get_name(void);
extern uint8_t joystick_adapter_get_id(void);
extern uint8_t joystick_adapter_activate(uint8_t id, char *name);
extern void joystick_adapter_deactivate(void);

extern void joystick_adapter_set_ports(int ports);
extern int joystick_adapter_get_ports(void);
extern void joystick_adapter_set_add_ports(int ports);

extern int joyport_snapshot_write_module(struct snapshot_s *s, int port);
extern int joyport_snapshot_read_module(struct snapshot_s *s, int port);

#endif
