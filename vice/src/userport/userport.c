/*
 * userport.c - userport handling.
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

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "cmdline.h"
#include "joyport.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "snapshot.h"
#include "uiapi.h"
#include "userport.h"
#include "util.h"

static int old_userport_collision_handling = 0;
static unsigned int old_order = 0;
static old_userport_device_list_t userport_head = { NULL, NULL, NULL };
static old_userport_snapshot_list_t userport_snapshot_head = { NULL, NULL, NULL };

/* flag indicating if the userport exists on the current emulated model */
static int userport_active = 1;

/* current userport device */
static int userport_current_device = USERPORT_DEVICE_NONE;

/* this will hold all the information about the userport devices */
static userport_device_t userport_device[USERPORT_MAX_DEVICES] = {0};

/* this will hold all the information about the userport itself */
static userport_port_props_t userport_props;

/* ---------------------------------------------------------------------------------------------------------- */

static int old_valid_device(old_userport_device_t *device)
{
    if ((device->read_pa2 || device->store_pa2) && !userport_props.has_pa2) {
        return 0;
    }

    if ((device->read_pa3 || device->store_pa3) && !userport_props.has_pa3) {
        return 0;
    }

    if (device->needs_pc && !userport_props.has_pc) {
        return 0;
    }

    if ((device->store_sp1 || device->read_sp1 || device->store_sp2 || device->read_sp2) && !userport_props.has_sp12) {
        return 0;
    }

    return 1;
}

static int valid_device(userport_device_t *device)
{
    if ((device->read_pa2 || device->store_pa2) && !userport_props.has_pa2) {
        return 0;
    }

    if ((device->read_pa3 || device->store_pa3) && !userport_props.has_pa3) {
        return 0;
    }

    if (device->needs_pc && !userport_props.has_pc) {
        return 0;
    }

    if ((device->store_sp1 || device->read_sp1 || device->store_sp2 || device->read_sp2) && !userport_props.has_sp12) {
        return 0;
    }

    return 1;
}

/* ---------------------------------------------------------------------------------------------------------- */

void userport_port_register(userport_port_props_t *props)
{
    userport_props.has_pa2 = props->has_pa2;
    userport_props.has_pa3 = props->has_pa3;
    userport_props.set_flag = props->set_flag;
    userport_props.has_pc = props->has_pc;
    userport_props.has_sp12 = props->has_sp12;
}

old_userport_device_list_t *old_userport_device_register(old_userport_device_t *device)
{
    old_userport_device_list_t *current = &userport_head;
    old_userport_device_list_t *retval = NULL;

    if (old_valid_device(device)) {
        retval = lib_malloc(sizeof(old_userport_device_list_t));

        while (current->next != NULL) {
            current = current->next;
        }
        current->next = retval;
        retval->previous = current;
        retval->device = device;
        retval->next = NULL;
        retval->device->order = old_order++;
    }

    return retval;
}

/* register a device to be used in the userport system if possible */
int userport_device_register(int id, userport_device_t *device)
{
    if (id < 1 || id > USERPORT_MAX_DEVICES) {
        return -1;
    }

    if (valid_device(device)) {
        userport_device[id].name = device->name;
        userport_device[id].joystick_adapter_id = device->joystick_adapter_id;
        userport_device[id].device_type = device->device_type;
        userport_device[id].enable = device->enable;
        userport_device[id].read_pbx = device->read_pbx;
        userport_device[id].store_pbx = device->store_pbx;
        userport_device[id].read_pa2 = device->read_pa2;
        userport_device[id].store_pa2 = device->store_pa2;
        userport_device[id].read_pa3 = device->read_pa3;
        userport_device[id].store_pa3 = device->store_pa3;
        userport_device[id].needs_pc = device->needs_pc;
        userport_device[id].store_sp1 = device->store_sp1;
        userport_device[id].read_sp1 = device->read_sp1;
        userport_device[id].store_sp2 = device->store_sp2;
        userport_device[id].read_sp2 = device->read_sp2;
        userport_device[id].write_snapshot = device->write_snapshot;
        userport_device[id].read_snapshot = device->read_snapshot;
        return 0;
    }
    return -1;
}

void old_userport_device_unregister(old_userport_device_list_t *device)
{
    old_userport_device_list_t *prev;

    if (device) {
        prev = device->previous;
        prev->next = device->next;

        if (device->next) {
            device->next->previous = prev;
        }

        if (device->device->order == old_order - 1) {
            if (old_order != 0) {
                old_order--;
            }
        }

        lib_free(device);
    }
}

/* ---------------------------------------------------------------------------------------------------------- */

static void old_userport_detach_devices(int collision, unsigned int highest_order)
{
    old_userport_device_list_t *current = userport_head.next;
    char *tmp1 = lib_strdup("Userport collision detected from ");
    char *tmp2;
    int col_found = 0;
    char *last_device_resource = NULL;
    char *last_device = NULL;
    char **detach_resource_list = NULL;
    int i;

    if (old_userport_collision_handling == USERPORT_COLLISION_METHOD_DETACH_ALL) {
        detach_resource_list = lib_malloc(sizeof(char *) * (collision + 1));
        memset(detach_resource_list, 0, sizeof(char *) * (collision + 1));
    }

    while (current) {
        if (current->device->collision) {
            if (old_userport_collision_handling == USERPORT_COLLISION_METHOD_DETACH_ALL) {
                detach_resource_list[col_found] = current->device->resource;
            }
            ++col_found;
            if (current->device->order == highest_order) {
                last_device_resource = current->device->resource;
                last_device = current->device->name;
            }
            if (col_found == collision) {
                tmp2 = util_concat(tmp1, "and ", current->device->name, NULL);
            } else if (col_found == 1) {
                tmp2 = util_concat(tmp1, current->device->name, NULL);
            } else {
                tmp2 = util_concat(tmp1, ", ", current->device->name, NULL);
            }
            lib_free(tmp1);
            tmp1 = tmp2;
        }
        current = current->next;
    }

    if (old_userport_collision_handling == USERPORT_COLLISION_METHOD_DETACH_ALL) {
        tmp2 = util_concat(tmp1, ". All involved devices will be detached.", NULL);
        for (i = 0; detach_resource_list[i]; ++i) {
            resources_set_int(detach_resource_list[i], 0);
        }
        lib_free(detach_resource_list);
    } else {
        tmp2 = util_concat(tmp1, ". Last device (", last_device, ") will be detached.", NULL);
        resources_set_int(last_device_resource, 0);
    }

    lib_free(tmp1);
    ui_error(tmp2);
    lib_free(tmp2);
}

static uint8_t old_userport_detect_collision(uint8_t retval_orig, uint8_t mask)
{
    uint8_t retval = retval_orig;
    uint8_t rm;
    uint8_t rv;
    int collision = 0;
    int first_found = 0;
    old_userport_device_list_t *current;
    unsigned int highest_order = 0;

    /* collision detection */
    current = userport_head.next;

    while (current) {
        if (current->device->read_pbx != NULL) {
            rm = current->device->mask;
            rm &= mask;
            if (rm) {
                rv = current->device->retval;
                rv |= ~rm;
                rv = 0xff & rv;
                if (!first_found) {
                    retval = rv;
                    first_found = 1;
                    current->device->collision = 1;
                    if (highest_order < current->device->order) {
                        highest_order = current->device->order;
                    }
                } else {
                    if (rv != retval) {
                        ++collision;
                        current->device->collision = 1;
                        if (highest_order < current->device->order) {
                            highest_order = current->device->order;
                        }
                    }
                }
            }
        }
    }

    if (collision) {
        old_userport_detach_devices(collision + 1, highest_order);
        if (old_userport_collision_handling == USERPORT_COLLISION_METHOD_DETACH_ALL) {
            retval = 0xff;
        }
    }

    return retval;
}

/* ---------------------------------------------------------------------------------------------------------- */

/* attach device 'id' to the userport */
static int userport_set_device(int id)
{
    /* 1st some sanity checks */
    if (id < USERPORT_DEVICE_NONE || id >= USERPORT_MAX_DEVICES) {
        return -1;
    }

    /* Nothing changes */
    if (id == userport_current_device) {
        return 0;
    }

    /* check if id is registered */
    if (id != USERPORT_DEVICE_NONE && !userport_device[id].name) {
        ui_error("Selected userport device %d is not registered", id);
        return -1;
    }

    /* check if device is a joystick adapter and a different joystick adapter is already active */
    if (id != USERPORT_DEVICE_NONE && userport_device[id].joystick_adapter_id) {
        if (!userport_device[userport_current_device].joystick_adapter_id) {
            /* if the current device in the userport is not a joystick adapter
               we need to check if a different joystick adapter is already
               active */
            if (joystick_adapter_get_id()) {
                ui_error("Selected userport device %s is a joystick adapter, but joystick adapter %s is already active.", userport_device[id].name, joystick_adapter_get_name());
                return -1;
            }
        }
    }

    /* all checks done, now disable the current device and enable the new device */
    if (userport_device[userport_current_device].enable) {
        userport_device[userport_current_device].enable(0);
    }
    if (userport_device[id].enable) {
        if (userport_device[id].enable(1) < 0) {
            return -1;
        }
    }
    userport_current_device = id;

    return 0;
}

static int userport_valid_devices_compare_names(const void* a, const void* b)
{
    const userport_desc_t *arg1 = (const userport_desc_t*)a;
    const userport_desc_t *arg2 = (const userport_desc_t*)b;

    if (arg1->device_type != arg2->device_type) {
        if (arg1->device_type < arg2->device_type) {
            return -1;
        } else {
            return 1;
        }
    }

    return strcmp(arg1->name, arg2->name);
}

userport_desc_t *userport_get_valid_devices(int sort)
{
    userport_desc_t *retval = NULL;
    int i;
    int valid = 0;
    int j = 0;

    for (i = 0; i < USERPORT_MAX_DEVICES; ++i) {
        if (userport_device[i].name) {
               ++valid;
        }
    }


    retval = lib_malloc(((size_t)valid + 1) * sizeof(userport_desc_t));
    for (i = 0; i < USERPORT_MAX_DEVICES; ++i) {
        if (userport_device[i].name) {
            retval[j].name = userport_device[i].name;
            retval[j].id = i;
            retval[j].device_type = userport_device[i].device_type;
            ++j;
        }
    }
    retval[j].name = NULL;

    if (sort) {
        qsort(retval, valid, sizeof(userport_desc_t), userport_valid_devices_compare_names);
    }

    return retval;
}

static uint8_t old_read_userport_pbx(uint8_t mask, uint8_t orig)
{
    uint8_t retval = 0xff;
    uint8_t rm;
    uint8_t rv;
    int valid = 0;
    old_userport_device_list_t *current = userport_head.next;

    if (!userport_active) {
        return orig;
    }

    if (!mask) {
        return 0xff;
    }

    /* set retval */
    while (current) {
        current->device->collision = 0;
        if (current->device->read_pbx != NULL) {
            current->device->read_pbx();
            rm = current->device->mask;
            rm &= mask;
            if (rm) {
                rv = current->device->retval;
                rv |= ~rm;
                retval &= rv;
                ++valid;
            }
        }
        current = current->next;
    }

    if (!valid) {
        return orig;
    }

    if (valid > 1 && old_userport_collision_handling != USERPORT_COLLISION_METHOD_AND_WIRES) {
        return old_userport_detect_collision(retval, mask);
    }

    return retval;
}

/* orig variable needs to be removed once the transition is done */
uint8_t read_userport_pbx(uint8_t mask, uint8_t orig)
{
    /* read from new userport system if the device has been registered */
    if (userport_current_device != USERPORT_DEVICE_NONE) {
        if (userport_device[userport_current_device].name) {
            if (userport_device[userport_current_device].read_pbx) {
                return userport_device[userport_current_device].read_pbx();
            }
        }
    }

    /* return old function */
    return old_read_userport_pbx(mask, orig);
}

static void old_store_userport_pbx(uint8_t val)
{
    old_userport_device_list_t *current = userport_head.next;

    if (userport_active) {
        while (current) {
            if (current->device->store_pbx != NULL) {
                current->device->store_pbx(val);
            }
            current = current->next;
        }
    }
}

void store_userport_pbx(uint8_t val)
{
    /* store to new userport system if the device has been registered */
    if (userport_current_device != USERPORT_DEVICE_NONE) {
        if (userport_device[userport_current_device].name) {
            if (userport_device[userport_current_device].store_pbx) {
                userport_device[userport_current_device].store_pbx(val);
                return;
            }
        }
    }

    /* store using old function as well */
    old_store_userport_pbx(val);
}

static uint8_t old_read_userport_pa2(uint8_t orig)
{
    uint8_t mask = 1;
    uint8_t rm;
    uint8_t rv;
    uint8_t retval = 0xff;
    int valid = 0;
    old_userport_device_list_t *current = userport_head.next;

    if (!userport_active) {
        return orig;
    }

    /* set retval */
    while (current) {
        current->device->collision = 0;
        if (current->device->read_pa2 != NULL) {
            current->device->read_pa2();
            rm = current->device->mask;
            rm &= mask;
            if (rm) {
                rv = current->device->retval;
                rv |= ~rm;
                retval &= rv;
                ++valid;
            }
        }
        current = current->next;
    }

    if (valid > 1 && old_userport_collision_handling != USERPORT_COLLISION_METHOD_AND_WIRES) {
        return old_userport_detect_collision(retval, mask);
    }
    if (valid == 0) {
        return orig;
    }

    return retval;
}

/* orig variable needs to be removed once the transition is done */
uint8_t read_userport_pa2(uint8_t orig)
{
    /* read from new userport system if the device has been registered */
    if (userport_current_device != USERPORT_DEVICE_NONE) {
        if (userport_device[userport_current_device].name) {
            if (userport_device[userport_current_device].read_pa2) {
                return userport_device[userport_current_device].read_pa2();
            }
        }
    }

    /* return old function */
    return old_read_userport_pa2(orig);
}

static void old_store_userport_pa2(uint8_t val)
{
    old_userport_device_list_t *current = userport_head.next;

    if (userport_active) {
        while (current) {
            if (current->device->store_pa2 != NULL) {
                current->device->store_pa2(val);
            }
            current = current->next;
        }
    }
}

void store_userport_pa2(uint8_t val)
{
    /* store to new userport system if the device has been registered */
    if (userport_current_device != USERPORT_DEVICE_NONE) {
        if (userport_device[userport_current_device].name) {
            if (userport_device[userport_current_device].store_pa2) {
                userport_device[userport_current_device].store_pa2(val);
                return;
            }
        }
    }

    /* store using old function as well */
    old_store_userport_pa2(val);
}

static uint8_t old_read_userport_pa3(uint8_t orig)
{
    uint8_t mask = 1;
    uint8_t rm;
    uint8_t rv;
    uint8_t retval = 0xff;
    int valid = 0;
    old_userport_device_list_t *current = userport_head.next;

    if (!userport_active) {
        return orig;
    }

    /* set retval */
    while (current) {
        current->device->collision = 0;
        if (current->device->read_pa3 != NULL) {
            current->device->read_pa3();
            rm = current->device->mask;
            rm &= mask;
            if (rm) {
                rv = current->device->retval;
                rv |= ~rm;
                retval &= rv;
                ++valid;
            }
        }
        current = current->next;
    }

    if (valid > 1 && old_userport_collision_handling != USERPORT_COLLISION_METHOD_AND_WIRES) {
        return old_userport_detect_collision(retval, mask);
    }
    if (valid == 0) {
        return orig;
    }

    return retval;
}

/* orig variable needs to be removed once the transition is done */
uint8_t read_userport_pa3(uint8_t orig)
{
    /* read from new userport system if the device has been registered */
    if (userport_current_device != USERPORT_DEVICE_NONE) {
        if (userport_device[userport_current_device].name) {
            if (userport_device[userport_current_device].read_pa3) {
                return userport_device[userport_current_device].read_pa3();
            }
        }
    }

    /* return old function */
    return old_read_userport_pa3(orig);
}

static void old_store_userport_pa3(uint8_t val)
{
    old_userport_device_list_t *current = userport_head.next;

    if (userport_active) {
        while (current) {
            if (current->device->store_pa3 != NULL) {
                current->device->store_pa3(val);
            }
            current = current->next;
        }
    }
}

void store_userport_pa3(uint8_t val)
{
    /* store to new userport system if the device has been registered */
    if (userport_current_device != USERPORT_DEVICE_NONE) {
        if (userport_device[userport_current_device].name) {
            if (userport_device[userport_current_device].store_pa3) {
                userport_device[userport_current_device].store_pa3(val);
                return;
            }
        }
    }

    /* store using old function as well */
    old_store_userport_pa3(val);
}

void set_userport_flag(uint8_t val)
{
    if (userport_active) {
        if (userport_props.set_flag) {
            userport_props.set_flag(val);
        }
    }
}

static void old_store_userport_sp1(uint8_t val)
{
    old_userport_device_list_t *current = userport_head.next;

    if (userport_active) {
        while (current) {
            if (current->device->store_sp1 != NULL) {
                current->device->store_sp1(val);
            }
            current = current->next;
        }
    }
}

void store_userport_sp1(uint8_t val)
{
    /* store to new userport system if the device has been registered */
    if (userport_current_device != USERPORT_DEVICE_NONE) {
        if (userport_device[userport_current_device].name) {
            if (userport_device[userport_current_device].store_sp1) {
                userport_device[userport_current_device].store_sp1(val);
                return;
            }
        }
    }

    /* store using old function as well */
    old_store_userport_sp1(val);
}

static uint8_t old_read_userport_sp1(uint8_t orig)
{
    uint8_t mask = 0xff;
    uint8_t rm;
    uint8_t rv;
    uint8_t retval = 0xff;
    int valid = 0;
    old_userport_device_list_t *current = userport_head.next;

    if (!userport_active) {
        return orig;
    }

    /* set retval */
    while (current) {
        current->device->collision = 0;
        if (current->device->read_sp1 != NULL) {
            current->device->read_sp1();
            rm = current->device->mask;
            rm &= mask;
            if (rm) {
                rv = current->device->retval;
                rv |= ~rm;
                retval &= rv;
                ++valid;
            }
        }
        current = current->next;
    }

    if (valid > 1 && old_userport_collision_handling != USERPORT_COLLISION_METHOD_AND_WIRES) {
        return old_userport_detect_collision(retval, mask);
    }

    if (!valid) {
        return orig;
    }

    return retval;
}

/* orig variable needs to be removed once the transition is done */
uint8_t read_userport_sp1(uint8_t orig)
{
    /* read from new userport system if the device has been registered */
    if (userport_current_device != USERPORT_DEVICE_NONE) {
        if (userport_device[userport_current_device].name) {
            if (userport_device[userport_current_device].read_sp1) {
                return userport_device[userport_current_device].read_sp1();
            }
        }
    }

    /* return old function */
    return old_read_userport_sp1(orig);
}

static void old_store_userport_sp2(uint8_t val)
{
    old_userport_device_list_t *current = userport_head.next;

    if (userport_active) {
        while (current) {
            if (current->device->store_sp2 != NULL) {
                current->device->store_sp2(val);
            }
            current = current->next;
        }
    }
}

void store_userport_sp2(uint8_t val)
{
    /* store to new userport system if the device has been registered */
    if (userport_current_device != USERPORT_DEVICE_NONE) {
        if (userport_device[userport_current_device].name) {
            if (userport_device[userport_current_device].store_sp2) {
                userport_device[userport_current_device].store_sp2(val);
                return;
            }
        }
    }

    /* store using old function as well */
    old_store_userport_sp2(val);
}

static uint8_t old_read_userport_sp2(uint8_t orig)
{
    uint8_t mask = 0xff;
    uint8_t rm;
    uint8_t rv;
    uint8_t retval = 0xff;
    int valid = 0;
    old_userport_device_list_t *current = userport_head.next;

    if (!userport_active) {
        return orig;
    }

    /* set retval */
    while (current) {
        current->device->collision = 0;
        if (current->device->read_sp2 != NULL) {
            current->device->read_sp2();
            rm = current->device->mask;
            rm &= mask;
            if (rm) {
                rv = current->device->retval;
                rv |= ~rm;
                retval &= rv;
                ++valid;
            }
        }
        current = current->next;
    }

    if (valid > 1 && old_userport_collision_handling != USERPORT_COLLISION_METHOD_AND_WIRES) {
        return old_userport_detect_collision(retval, mask);
    }

    if (!valid) {
        return orig;
    }

    return retval;
}

/* orig variable needs to be removed once the transition is done */
uint8_t read_userport_sp2(uint8_t orig)
{
    /* read from new userport system if the device has been registered */
    if (userport_current_device != USERPORT_DEVICE_NONE) {
        if (userport_device[userport_current_device].name) {
            if (userport_device[userport_current_device].read_sp2) {
                return userport_device[userport_current_device].read_sp2();
            }
        }
    }

    /* return old function */
    return old_read_userport_sp2(orig);
}

/* ---------------------------------------------------------------------------------------------------------- */

void old_userport_snapshot_register(old_userport_snapshot_t *s)
{
    old_userport_snapshot_list_t *current = &userport_snapshot_head;
    old_userport_snapshot_list_t *retval = NULL;

    retval = lib_malloc(sizeof(old_userport_snapshot_list_t));

    while (current->next != NULL) {
        current = current->next;
    }
    current->next = retval;
    retval->previous = current;
    retval->snapshot = s;
    retval->next = NULL;
}

static void old_userport_snapshot_unregister(old_userport_snapshot_list_t *s)
{
    old_userport_snapshot_list_t *prev;

    if (s) {
        prev = s->previous;
        prev->next = s->next;

        if (s->next) {
            s->next->previous = prev;
        }

        lib_free(s);
    }
}

/* ---------------------------------------------------------------------------------------------------------- */

static int old_set_userport_collision_handling(int val, void *param)
{
    switch (val) {
        case USERPORT_COLLISION_METHOD_DETACH_ALL:
        case USERPORT_COLLISION_METHOD_DETACH_LAST:
        case USERPORT_COLLISION_METHOD_AND_WIRES:
            break;
        default:
            return -1;
    }
    old_userport_collision_handling = val;

    return 0;
}

static const resource_int_t old_resources_int[] = {
    { "UserportCollisionHandling", USERPORT_COLLISION_METHOD_DETACH_ALL, RES_EVENT_STRICT, (resource_value_t)0,
      &old_userport_collision_handling, old_set_userport_collision_handling, NULL },
    RESOURCE_INT_LIST_END
};

static int set_userport_device(int val, void *param)
{
    return userport_set_device(val);
}

static const resource_int_t resources_int[] = {
    { "UserportDevice", USERPORT_DEVICE_NONE, RES_EVENT_NO, NULL,
      &userport_current_device, set_userport_device, NULL },
    RESOURCE_INT_LIST_END
};

int userport_resources_init(void)
{
    memset(userport_device, 0, sizeof(userport_device));
    userport_device[0].name = "None";
    userport_device[0].joystick_adapter_id = JOYSTICK_ADAPTER_ID_NONE;

    if (resources_register_int(old_resources_int) < 0) {
        return -1;
    }

    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    return machine_register_userport();
}

void userport_resources_shutdown(void)
{
    old_userport_device_list_t *current = userport_head.next;
    old_userport_snapshot_list_t *c = userport_snapshot_head.next;

    while (current) {
        old_userport_device_unregister(current);
        current = userport_head.next;
    }

    while (c) {
        old_userport_snapshot_unregister(c);
        c = userport_snapshot_head.next;
    }
}

struct userport_opt_s {
    const char *name;
    int id;
};

static const struct userport_opt_s id_match[] = {
    { "0",                USERPORT_DEVICE_NONE },
    { "none",             USERPORT_DEVICE_NONE },
#if 0
    { "1",                USERPORT_DEVICE_PRINTER },
    { "printer",          USERPORT_DEVICE_PRINTER },
    { "plotter",          USERPORT_DEVICE_PRINTER },
    { "2",                USERPORT_DEVICE_JOYSTICK_CGA },
    { "cga",              USERPORT_DEVICE_JOYSTICK_CGA },
    { "cgajoy",           USERPORT_DEVICE_JOYSTICK_CGA },
    { "cgajoystick",      USERPORT_DEVICE_JOYSTICK_CGA },
    { "3",                USERPORT_DEVICE_JOYSTICK_PET },
    { "pet",              USERPORT_DEVICE_JOYSTICK_PET },
    { "petjoy",           USERPORT_DEVICE_JOYSTICK_PET },
    { "petjoystick",      USERPORT_DEVICE_JOYSTICK_PET },
    { "4",                USERPORT_DEVICE_JOYSTICK_HUMMER },
    { "hummer",           USERPORT_DEVICE_JOYSTICK_HUMMER },
    { "hummerjoy",        USERPORT_DEVICE_JOYSTICK_HUMMER },
    { "hummerjoystick",   USERPORT_DEVICE_JOYSTICK_HUMMER },
    { "5",                USERPORT_DEVICE_JOYSTICK_OEM },
    { "oem",              USERPORT_DEVICE_JOYSTICK_OEM },
    { "oemjoy",           USERPORT_DEVICE_JOYSTICK_OEM },
    { "oemjoystick",      USERPORT_DEVICE_JOYSTICK_OEM },
    { "6",                USERPORT_DEVICE_JOYSTICK_HIT },
    { "hit",              USERPORT_DEVICE_JOYSTICK_HIT },
    { "dxs",              USERPORT_DEVICE_JOYSTICK_HIT },
    { "hitjoy",           USERPORT_DEVICE_JOYSTICK_HIT },
    { "dxsjoy",           USERPORT_DEVICE_JOYSTICK_HIT },
    { "hitjoystick",      USERPORT_DEVICE_JOYSTICK_HIT },
    { "dxsjoystick",      USERPORT_DEVICE_JOYSTICK_HIT },
    { "7",                USERPORT_DEVICE_JOYSTICK_KINGSOFT },
    { "kingsoft",         USERPORT_DEVICE_JOYSTICK_KINGSOFT },
    { "kingsoftjoy",      USERPORT_DEVICE_JOYSTICK_KINGSOFT },
    { "kingsoftjoystick", USERPORT_DEVICE_JOYSTICK_KINGSOFT },
    { "8",                USERPORT_DEVICE_JOYSTICK_STARBYTE },
    { "starbyte",         USERPORT_DEVICE_JOYSTICK_STARBYTE },
    { "starbytejoy",      USERPORT_DEVICE_JOYSTICK_STARBYTE },
    { "starbytejoystick", USERPORT_DEVICE_JOYSTICK_STARBYTE },
    { "9",                USERPORT_DEVICE_JOYSTICK_SYNERGY },
    { "synergy",          USERPORT_DEVICE_JOYSTICK_SYNERGY },
    { "synergyjoy",       USERPORT_DEVICE_JOYSTICK_SYNERGY },
    { "synergyjoystick",  USERPORT_DEVICE_JOYSTICK_SYNERGY },
#endif
    { "10",               USERPORT_DEVICE_DAC },
    { "dac",              USERPORT_DEVICE_DAC },
    { "11",               USERPORT_DEVICE_DIGIMAX },
    { "digimax",          USERPORT_DEVICE_DIGIMAX },
    { "12",               USERPORT_DEVICE_4BIT_SAMPLER },
    { "4bitsampler",      USERPORT_DEVICE_4BIT_SAMPLER },
    { "13",               USERPORT_DEVICE_8BSS },
    { "8bss",             USERPORT_DEVICE_8BSS },
    { "14",               USERPORT_DEVICE_RTC_58321A },
    { "58321a",           USERPORT_DEVICE_RTC_58321A },
    { "58321artc",        USERPORT_DEVICE_RTC_58321A },
    { "58321rtc",         USERPORT_DEVICE_RTC_58321A },
    { "rtc58321a",        USERPORT_DEVICE_RTC_58321A },
    { "rtc58321",         USERPORT_DEVICE_RTC_58321A },
    { "15",               USERPORT_DEVICE_RTC_DS1307 },
    { "ds1307",           USERPORT_DEVICE_RTC_DS1307 },
    { "ds1307rtc",        USERPORT_DEVICE_RTC_DS1307 },
    { "rtcds1307",        USERPORT_DEVICE_RTC_DS1307 },
    { "rtc1307",          USERPORT_DEVICE_RTC_DS1307 },
    { "16",               USERPORT_DEVICE_PETSCII_SNESPAD },
    { "petscii",          USERPORT_DEVICE_PETSCII_SNESPAD },
    { "petsciisnes",      USERPORT_DEVICE_PETSCII_SNESPAD },
    { "petsciisnespad",   USERPORT_DEVICE_PETSCII_SNESPAD },
#if 0
    { "17",               USERPORT_DEVICE_SPACEBALLS },
    { "spaceballs",       USERPORT_DEVICE_SPACEBALLS },
#endif
    { "18",               USERPORT_DEVICE_SUPERPAD64 },
    { "superpad",         USERPORT_DEVICE_SUPERPAD64 },
    { "superpad64",       USERPORT_DEVICE_SUPERPAD64 },
#ifdef USERPORT_EXPERIMENTAL_DEVICES
    { "19",               USERPORT_DEVICE_DIAG_586220_HARNESS },
    { "diag",             USERPORT_DEVICE_DIAG_586220_HARNESS },
    { "diagharness",      USERPORT_DEVICE_DIAG_586220_HARNESS },
#endif
    { NULL, -1 }
};

static int set_userport_cmdline_device(const char *param, void *extra_param)
{
    int temp = -1;
    int i = 0;

    if (!param) {
        return -1;
    }

    do {
        if (strcmp(id_match[i].name, param) == 0) {
            temp = id_match[i].id;
        }
        i++;
    } while ((temp == -1) && (id_match[i].name != NULL));

    if (temp == -1) {
        return -1;
    }

    return set_userport_device(temp, NULL);
}

static char *build_userport_string(int something)
{
    int i = 0;
    char *tmp1;
    char *tmp2;
    char number[4];
    userport_desc_t *devices = userport_get_valid_devices(0);

    tmp1 = lib_msprintf("Set userport device (0: None");

    for (i = 1; devices[i].name; ++i) {
        sprintf(number, "%d", devices[i].id);
        tmp2 = util_concat(tmp1, ", ", number, ": ", devices[i].name, NULL);
        lib_free(tmp1);
        tmp1 = tmp2;
    }
    tmp2 = util_concat(tmp1, ")", NULL);
    lib_free(tmp1);
    lib_free(devices);
    return tmp2;
}

static const cmdline_option_t old_cmdline_options[] =
{
    { "-userportcollision", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "UserportCollisionHandling", NULL,
      "<method>", "Select the way the Userport collisions should be handled, (0: error message and detach all involved devices, 1: error message and detach last attached involved device, 2: warning in log and 'AND' the valid return values" },
    CMDLINE_LIST_END
};

/* no new cmdline options yet */
static cmdline_option_t cmdline_options[] =
{
    { "-userportdevice", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS | CMDLINE_ATTRIB_DYNAMIC_DESCRIPTION,
      set_userport_cmdline_device, NULL, NULL, NULL,
      "Device", NULL },
    CMDLINE_LIST_END
};

int userport_cmdline_options_init(void)
{
    union char_func cf;

    cf.f = build_userport_string;
    cmdline_options[0].description = cf.c;

    if (cmdline_register_options(cmdline_options) < 0) {
        return -1;
    }
    return cmdline_register_options(old_cmdline_options);
}

void userport_enable(int val)
{
    userport_active = val ? 1 : 0;
}

/* ---------------------------------------------------------------------------------------------------------- */

/* USERPORT snapshot module format:

   type  | name               | description
   ----------------------------------------
   BYTE  | active             | userport active flag
   BYTE  | collision handling | useport collision handling
   BYTE  | amount             | amount of attached devices

   if 'amount' is non-zero the following is also saved per attached device:

   type  | name | description
   --------------------------
   BYTE  | id   | device id
 */

static char snap_module_name[] = "USERPORT";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0

static int old_userport_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;
    int amount = 0;
    int *devices = NULL;
    old_userport_device_list_t *current = userport_head.next;
    old_userport_snapshot_list_t *c = NULL;
    int i = 0;

    while (current) {
        ++amount;
        current = current->next;
    }

    if (amount) {
        devices = lib_malloc(sizeof(int) * (amount + 1));
        current = userport_head.next;
        while (current) {
            devices[i++] = current->device->id;
            current = current->next;
        }
        devices[i] = -1;
    }

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_B(m, (uint8_t)userport_active) < 0
        || SMW_B(m, (uint8_t)old_userport_collision_handling) < 0
        || SMW_B(m, (uint8_t)amount) < 0) {
        goto fail;
    }

    /* Save device id's */
    if (amount) {
        for (i = 0; devices[i]; ++i) {
            if (SMW_B(m, (uint8_t)devices[i]) < 0) {
                goto fail;
            }
        }
    }

    snapshot_module_close(m);

    /* save device snapshots */
    if (amount) {
        for (i = 0; devices[i]; ++i) {
            c = userport_snapshot_head.next;
            while (c) {
                if (c->snapshot->id == devices[i]) {
                    if (c->snapshot->write_snapshot) {
                        if (c->snapshot->write_snapshot(s) < 0) {
                            lib_free(devices);
                            return -1;
                        }
                    }
                }
                c = c->next;
            }
        }
    }

    lib_free(devices);

    return 0;

fail:
    snapshot_module_close(m);
    return -1;
}

int userport_snapshot_write_module(snapshot_t *s)
{
    /* return old function for now */
    return old_userport_snapshot_write_module(s);
}

static int old_userport_snapshot_read_module(snapshot_t *s)
{
    uint8_t major_version, minor_version;
    snapshot_module_t *m;
    int amount = 0;
    char **detach_resource_list = NULL;
    old_userport_device_list_t *current = userport_head.next;
    int *devices = NULL;
    old_userport_snapshot_list_t *c = NULL;
    int i = 0;

    /* detach all userport devices */
    while (current) {
        ++amount;
        current = current->next;
    }

    if (amount) {
        detach_resource_list = lib_malloc(sizeof(char *) * (amount + 1));
        memset(detach_resource_list, 0, sizeof(char *) * (amount + 1));
        current = userport_head.next;
        while (current) {
            detach_resource_list[i++] = current->device->resource;
            current = current->next;
        }
        for (i = 0; i < amount; ++i) {
            resources_set_int(detach_resource_list[i], 0);
        }
        lib_free(detach_resource_list);
    }

    m = snapshot_module_open(s, snap_module_name, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(major_version, minor_version, SNAP_MAJOR, SNAP_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (0
        || SMR_B_INT(m, &userport_active) < 0
        || SMR_B_INT(m, &old_userport_collision_handling) < 0
        || SMR_B_INT(m, &amount) < 0) {
        goto fail;
    }

    if (amount) {
        devices = lib_malloc(sizeof(int) * (amount + 1));
        for (i = 0; i < amount; ++i) {
            if (SMR_B_INT(m, &devices[i]) < 0) {
                lib_free(devices);
                goto fail;
            }
        }
        snapshot_module_close(m);
        for (i = 0; i < amount; ++i) {
            c = userport_snapshot_head.next;
            while (c) {
                if (c->snapshot->id == devices[i]) {
                    if (c->snapshot->read_snapshot) {
                        if (c->snapshot->read_snapshot(s) < 0) {
                            lib_free(devices);
                            return -1;
                        }
                    }
                }
                c = c->next;
            }
        }
        return 0;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

int userport_snapshot_read_module(snapshot_t *s)
{
    /* return old function for now */
    return old_userport_snapshot_read_module(s);
}
