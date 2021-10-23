/*
 * tapeport.h - tape/datasette port abstraction system.
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

#ifndef VICE_TAPEPORT_H
#define VICE_TAPEPORT_H

#include "snapshot.h"
#include "types.h"

/* #define TAPEPORT_EXPERIMENTAL_DEVICES */

enum {
    TAPEPORT_DEVICE_NONE = 0,
    TAPEPORT_DEVICE_DATASETTE,
    TAPEPORT_DEVICE_CP_CLOCK_F83,
    TAPEPORT_DEVICE_DTL_BASIC_DONGLE,
    TAPEPORT_DEVICE_SENSE_DONGLE,
#ifdef TAPEPORT_EXPERIMENTAL_DEVICES
    TAPEPORT_DEVICE_TAPE_DIAG_586220_HARNESS,
#endif
    TAPEPORT_DEVICE_TAPECART,
    TAPEPORT_MAX_DEVICES
};

enum {
    TAPEPORT_PORT_1 = 0,
    TAPEPORT_PORT_2,
    TAPEPORT_MAX_PORTS
};

/* This struct is used by the OLD tapeport devices */
typedef struct old_tapeport_device_s {
    /* id of the device */
    int device_id;

    /* Name of the device */
    char *name;

    /* id number of attached device, used for the order of the devices */
    int id;

    /* resource of the device, used for detaching when reading snapshots */
    char *resource;

    /* shutdown device */
    void (*shutdown)(void);

    /* reset device */
    void (*reset)(void);

    /* set motor line */
    void (*set_motor)(int flag);

    /* set write line */
    void (*toggle_write_bit)(int write_bit);

    /* set sense line */
    void (*set_sense_out)(int sense);

    /* set read line */
    void (*set_read_out)(int val);

    /* read line change on passthrough port, NULL if no passthrough port present */
    void (*trigger_flux_change_passthrough)(unsigned int on);

    /* sense line change on passthrough port, NULL if no passthrough port present */
    void (*set_tape_sense_passthrough)(int sense);

    /* write in line change on passthrough port, NULL if no passthrough port present */
    void (*set_tape_write_in_passthrough)(int val);

    /* motor in line change on passthrough port, NULL if no passthrough port present */
    void (*set_tape_motor_in_passthrough)(int sense);
} old_tapeport_device_t;

typedef struct old_tapeport_device_list_s {
    struct old_tapeport_device_list_s *previous;
    old_tapeport_device_t *device;
    struct old_tapeport_device_list_s *next;
} old_tapeport_device_list_t;

extern old_tapeport_device_list_t *old_tapeport_device_register(old_tapeport_device_t *device);
extern void old_tapeport_device_unregister(old_tapeport_device_list_t *device);

/* This struct holds all the information about the tapeport devices */
typedef struct tapeport_device_s {
    /* Name of the device */
    char *name;

    /* enable device function */
    int (*enable)(int port, int val);

    /* reset device */
    void (*reset)(int port);

    /* set motor line */
    void (*set_motor)(int port, int flag);

    /* set write line */
    void (*toggle_write_bit)(int port, int write_bit);

    /* set sense line */
    void (*set_sense_out)(int port, int sense);

    /* set read line */
    void (*set_read_out)(int port, int val);

    /* Snapshot write */
    int (*write_snapshot)(struct snapshot_s *s);

    /* Snapshot read */
    int (*read_snapshot)(struct snapshot_s *s, int port);
} tapeport_device_t;

extern int tapeport_device_register(int id, tapeport_device_t *device);

/* This is the OLD tapeport snapshot struct */
typedef struct old_tapeport_snapshot_s {
    /* id of the device */
    int id;

    /* Write snapshot */
    int (*write_snapshot)(struct snapshot_s *s, int write_image);

    /* Read snapshot */
    int (*read_snapshot)(struct snapshot_s *s);
} old_tapeport_snapshot_t;

/* This is the OLD tapeport double linked list */
typedef struct old_tapeport_snapshot_list_s {
    struct old_tapeport_snapshot_list_s *previous;
    old_tapeport_snapshot_t *snapshot;
    struct old_tapeport_snapshot_list_s *next;
} old_tapeport_snapshot_list_t;

extern void old_tapeport_snapshot_register(old_tapeport_snapshot_t *snapshot);

extern void tapeport_set_motor(int port, int flag);
extern void tapeport_toggle_write_bit(int port, int write_bit);
extern void tapeport_set_sense_out(int port, int sense);

extern void tapeport_reset(void);

extern void old_tapeport_trigger_flux_change(unsigned int on, int id);
extern void old_tapeport_set_tape_sense(int sense, int id);

extern void old_tapeport_set_write_in(int val, int id);
extern void old_tapeport_set_motor_in(int val, int id);

extern void tapeport_trigger_flux_change(unsigned int on, int port);
extern void tapeport_set_tape_sense(int sense, int port);
extern void tapeport_set_write_in(int val, int port);
extern void tapeport_set_motor_in(int val, int port);

extern int tapeport_resources_init(void);
extern void tapeport_resources_shutdown(void);
extern int tapeport_cmdline_options_init(void);

extern void tapeport_enable(int val);

typedef struct tapeport_desc_s {
    char *name;
    int id;
} tapeport_desc_t;

extern tapeport_desc_t *tapeport_get_valid_devices(void);

extern int tapeport_snapshot_write_module(struct snapshot_s *s, int save_image);
extern int tapeport_snapshot_read_module(struct snapshot_s *s);

#endif
