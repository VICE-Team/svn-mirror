/*
 * raster-changes.h - Handling of changes within a raster line.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

/* This should be a lot more than what is actually needed.  */
#define RASTER_CHANGES_MAX 512

enum raster_changes_type
  {
    RASTER_CHANGES_TYPE_INT,
    RASTER_CHANGES_TYPE_PTR
  };
typedef enum raster_changes_type raster_changes_type_t;

struct _raster_changes_integer_action_value
  {
    int *oldp;
    int new;
  };
typedef struct _raster_changes_integer_action_value
  raster_changes_integer_action_value_t;

struct _raster_changes_ptr_action_value
  {
    void **oldp;
    void *new;
  };
typedef struct _raster_changes_ptr_action_value
  raster_changes_ptr_action_value_t;

union _raster_changes_action_value
  {
    raster_changes_integer_action_value_t integer;
    raster_changes_ptr_action_value_t ptr;
  };
typedef union _raster_changes_action_value raster_changes_action_value_t;

struct _raster_changes_action
  {
    /* "Where" the change happens (eg. character position for foreground
       changes, pixel position for other changes).  */
    int where;

    /* Data type for changed value.  */
    raster_changes_type_t type;

    /* Pointer to where the value is stored and new value to assign.  */
    raster_changes_action_value_t value;
  };
typedef struct _raster_changes_action raster_changes_action_t;

struct _raster_changes
  {
    /* Total number of changes. */
    unsigned int count;

    /* List of changes to be applied in order.  */
    raster_changes_action_t actions[RASTER_CHANGES_MAX];
  };
typedef struct _raster_changes raster_changes_t;



void raster_changes_init (raster_changes_t *changes);
raster_changes_t *raster_changes_new (void);



/* Inline functions.  These need to be *fast*.  */

/* Apply change number `idx' in `changes'.  */
inline static void
raster_changes_apply (raster_changes_t *changes,
		      unsigned int idx)
{
  raster_changes_action_t *action;

  action = changes->actions + idx;

  switch (changes->actions[idx].type)
    {
    case RASTER_CHANGES_TYPE_INT:
      *action->value.integer.oldp = action->value.integer.new;
      break;
    case RASTER_CHANGES_TYPE_PTR:
    default:			/* To be faster.  */
      *action->value.ptr.oldp = action->value.ptr.new;
      break;
    }
}

/* Remove all the changes in `changes'.  */
inline static void
raster_changes_remove_all (raster_changes_t *changes)
{
  changes->count = 0;
}

/* Apply all the changes in `changes'.  */
inline static void
raster_changes_apply_all (raster_changes_t *changes)
{
  unsigned int i;

  for (i = 0; i < changes->count; i++)
    raster_changes_apply (changes, i);

  raster_changes_remove_all (changes);
}

/* Add an int change.  */
inline static void
raster_changes_add_int (raster_changes_t *changes,
			int where,
			int *ptr,
			int new_value)
{
  raster_changes_action_t *action;

  action = changes->actions + changes->count++;

  action->where = where;
  action->type = RASTER_CHANGES_TYPE_INT;
  action->value.integer.oldp = ptr;
  action->value.integer.new = new_value;
}

/* Add a pointer (`void *') change.  */
inline static void
raster_changes_add_ptr (raster_changes_t *changes,
			int where,
			void **ptr,
			void *new_value)
{
  raster_changes_action_t *action;

  action = changes->actions + changes->count++;

  action->where = where;
  action->type = RASTER_CHANGES_TYPE_PTR;
  action->value.ptr.oldp = ptr;
  action->value.ptr.new = new_value;
}

#endif
