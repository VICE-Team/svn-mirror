/*
 * raster-changes.h - Handling of changes within a raster line.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _RASTER_CHANGES_H
#define _RASTER_CHANGES_H

#include "types.h"

struct raster_s;

/* This should be a lot more than what is actually needed.  */
#define RASTER_CHANGES_MAX 512

enum raster_changes_type_s
{
    RASTER_CHANGES_TYPE_INT,
    RASTER_CHANGES_TYPE_PTR
};
typedef enum raster_changes_type_s raster_changes_type_t;

struct raster_changes_integer_action_value_s
{
    int *oldp;
    int newone;
};
typedef struct raster_changes_integer_action_value_s
    raster_changes_integer_action_value_t;

struct raster_changes_ptr_action_value_s
{
    void **oldp;
    void *newone;
};
typedef struct raster_changes_ptr_action_value_s
    raster_changes_ptr_action_value_t;

union raster_changes_action_value_s
{
    raster_changes_integer_action_value_t integer;
    raster_changes_ptr_action_value_t ptr;
};
typedef union raster_changes_action_value_s raster_changes_action_value_t;

struct raster_changes_action_s
{
    /* "Where" the change happens (eg. character position for foreground
       changes, pixel position for other changes).  */
    int where;

    /* Data type for changed value.  */
    raster_changes_type_t type;

    /* Pointer to where the value is stored and new value to assign.  */
    raster_changes_action_value_t value;
};
typedef struct raster_changes_action_s raster_changes_action_t;

struct raster_changes_s
{
    /* Total number of changes. */
    unsigned int count;

    /* List of changes to be applied in order.  */
    raster_changes_action_t actions[RASTER_CHANGES_MAX];
};
typedef struct raster_changes_s raster_changes_t;

struct raster_changes_all_s {
    raster_changes_t *background;
    raster_changes_t *foreground;
    raster_changes_t *border;
    raster_changes_t *sprites;
    raster_changes_t *next_line;
    int have_on_this_line;
};
typedef struct raster_changes_all_s raster_changes_all_t;

extern void raster_changes_init(struct raster_s *raster);
extern void raster_changes_shutdown(struct raster_s *raster);


/* Inline functions.  These need to be *fast*.  */

/* Apply change number `idx' in `changes'.  */
inline static void raster_changes_apply(raster_changes_t *changes,
                                        const unsigned int idx)
{
    raster_changes_action_t *action;

    action = changes->actions + idx;

    switch (changes->actions[idx].type) {
      case RASTER_CHANGES_TYPE_INT:
        *action->value.integer.oldp = action->value.integer.newone;
        break;
      case RASTER_CHANGES_TYPE_PTR:
      default:                    /* To be faster.  */
        *action->value.ptr.oldp = action->value.ptr.newone;
        break;
    }
}

/* Remove all the changes in `changes'.  */
inline static void raster_changes_remove_all(raster_changes_t *changes)
{
    changes->count = 0;
}

/* Apply all the changes in `changes'.  */
inline static void raster_changes_apply_all(raster_changes_t *changes)
{
    unsigned int i;

    for (i = 0; i < changes->count; i++)
        raster_changes_apply(changes, i);

    raster_changes_remove_all(changes);
}

/* Add an int change.  */
inline static void raster_changes_add_int(raster_changes_t *changes,
                                          const int where,
                                          int *ptr,
                                          const int new_value)
{
    raster_changes_action_t *action;

    action = changes->actions + changes->count++;

    action->where = where;
    action->type = RASTER_CHANGES_TYPE_INT;
    action->value.integer.oldp = ptr;
    action->value.integer.newone = new_value;
}

/* Add a pointer (`void *') change.  */
inline static void raster_changes_add_ptr(raster_changes_t *changes,
                                          const int where,
                                          void **ptr,
                                          void *new_value)
{
    raster_changes_action_t *action;

    action = changes->actions + changes->count++;

    action->where = where;
    action->type = RASTER_CHANGES_TYPE_PTR;
    action->value.ptr.oldp = ptr;
    action->value.ptr.newone = new_value;
}

#endif

