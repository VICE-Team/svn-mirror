/*
 * alarm.h - Alarm handling.
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

#ifndef _ALARM_H
#define _ALARM_H

#include "types.h"

#define ALARM_CONTEXT_MAX_PENDING_ALARMS 0x100

/* FIXME: `long' is for backwards compatibility, but it should really be
   `CLOCK', as the offset is always non-negative.  */
typedef int (*alarm_callback_t) (long offset);

typedef struct _alarm alarm_t;
typedef struct _alarm_context alarm_context_t;

/* An alarm.  */
struct _alarm {
    /* Descriptive name of the alarm.  */
    char *name;

    /* Alarm context this alarm is in.  */
    alarm_context_t *context;

    /* Callback to be called when the alarm is dispatched.  */
    alarm_callback_t callback;

    /* Index into the pending alarm list.  If < 0, the alarm is not
       pending.  */
    int pending_idx;

    /* Link to the next and previous alarms in the list.  */
    struct _alarm *next, *prev;
};

/* An alarm context.  */
struct _alarm_context {
    /* Descriptive name of the alarm context.  */
    char *name;

    /* Alarm list.  */
    alarm_t *alarms;

    /* Pending alarm array.  Statically allocated because it's slightly
       faster this way.  */
    struct {
        /* The alarm.  */
        alarm_t *alarm;

        /* Clock tick at which this alarm should be activated.  */
        CLOCK clk;
    } pending_alarms[ALARM_CONTEXT_MAX_PENDING_ALARMS];
    unsigned int num_pending_alarms;

    /* Clock tick for the next pending alarm.  */
    CLOCK next_pending_alarm_clk;

    /* Pending alarm number.  */
    int next_pending_alarm_idx;
};

/* ------------------------------------------------------------------------ */

alarm_context_t *alarm_context_new(const char *name);
void alarm_context_init(alarm_context_t *context, const char *name);
void alarm_context_destroy(alarm_context_t *context);

alarm_t *alarm_new(alarm_context_t *context, const char *name,
                   alarm_callback_t callback);
void alarm_init(alarm_t *alarm, alarm_context_t *context, const char *name,
                alarm_callback_t callback);
void alarm_destroy(alarm_t *alarm);
void alarm_set(alarm_t *alarm, CLOCK clk);
void alarm_unset(alarm_t *alarm);

/* ------------------------------------------------------------------------- */

/* Inline functions.  */

inline static CLOCK alarm_context_next_pending_clk(alarm_context_t *context)
{
    return context->next_pending_alarm_clk;
}

inline static void alarm_context_update_next_pending(alarm_context_t *context)
{
    CLOCK next_pending_alarm_clk = (CLOCK) ~0L;
    unsigned int next_pending_alarm_idx;
    unsigned int i;

    next_pending_alarm_idx = context->next_pending_alarm_idx;

    for (i = 0; i < context->num_pending_alarms; i++) {
        CLOCK pending_clk = context->pending_alarms[i].clk;

        if (pending_clk <= next_pending_alarm_clk) {
            next_pending_alarm_clk = pending_clk;
            next_pending_alarm_idx = i;
        }
    }

    context->next_pending_alarm_clk = next_pending_alarm_clk;
    context->next_pending_alarm_idx = next_pending_alarm_idx;
}

inline static void alarm_context_dispatch(alarm_context_t *context, CLOCK clk)
{
    long offset;
    unsigned int idx;

    offset = (long) (clk - context->next_pending_alarm_clk);
    if (offset < 0)
        return;

    idx = context->next_pending_alarm_idx;

    (context->pending_alarms[idx].alarm->callback)(offset);
}

#endif
