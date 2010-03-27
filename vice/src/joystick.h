/*
 * joystick.h - Common joystick emulation.
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

#ifndef VICE_JOYSTICK_H
#define VICE_JOYSTICK_H

#include "types.h"

#define EXTRA_JOYSTICK_CGA    0
#define EXTRA_JOYSTICK_PET    1
#define EXTRA_JOYSTICK_HUMMER 2
#define EXTRA_JOYSTICK_OEM    3
#define EXTRA_JOYSTICK_HIT    4

struct snapshot_s;

extern int joystick_init(void);
extern int joystick_init_resources(void);

extern int joystick_check_set(signed long key, int keysetnum, unsigned int joyport);
extern int joystick_check_clr(signed long key, int keysetnum, unsigned int joyport);
extern void joystick_joypad_clear(void);

extern void joystick_set_value_absolute(unsigned int joyport, BYTE value);
extern void joystick_set_value_or(unsigned int joyport, BYTE value);
extern void joystick_set_value_and(unsigned int joyport, BYTE value);
extern void joystick_clear(unsigned int joyport);
extern void joystick_clear_all(void);

extern void joystick_event_playback(CLOCK offset, void *data);
extern void joystick_event_delayed_playback(void *data);
extern void joystick_register_delay(unsigned int delay);

typedef void (*joystick_machine_func_t)(void);
extern void joystick_register_machine(joystick_machine_func_t func);

extern int joystick_snapshot_write_module(struct snapshot_s *s);
extern int joystick_snapshot_read_module(struct snapshot_s *s);

extern int extra_joystick_enable;
extern int extra_joystick_type;

extern BYTE extra_joystick_cga_read(void);
extern void extra_joystick_cga_store(BYTE value);

extern BYTE extra_joystick_hit_read_button2(void);
extern BYTE extra_joystick_hit_read_button1(void);
extern BYTE extra_joystick_hit_read(void);
extern void extra_joystick_hit_store(BYTE value);

extern BYTE extra_joystick_pet_read(void);

extern BYTE extra_joystick_hummer_read(void);

extern BYTE extra_joystick_oem_read(void);

/*! the number of joysticks that can be attached to the emu */
#define JOYSTICK_NUM 4

/* virtual joystick mapping */ 
extern int joystick_port_map[JOYSTICK_NUM];

#endif
