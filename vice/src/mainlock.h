/** \file   mainlock.h
 * \brief   VICE mutex used to synchronise access to the VICE api - header
 *
 * \author  David Hogan <david.q.hogan@gmail.com>
 */

/*
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

#ifndef VICE_MAIN_LOCK_H
#define VICE_MAIN_LOCK_H

#include "vice.h"

#include <pthread.h>
#include <stdbool.h>
#include <assert.h>

#include "archdep.h"

typedef void (*mainlock_callback_t)(void);

void mainlock_init(void);
void mainlock_shutdown(void);

void mainlock_initiate_shutdown(void);

void mainlock_set_vice_thread(void *thread);
bool mainlock_is_vice_thread(void);

void mainlock_yield(void);
void mainlock_yield_and_sleep(tick_t ticks);
void mainlock_yield_begin(void);
void mainlock_yield_end(void);

void mainlock_obtain(void);
void mainlock_release(void);

#define mainlock_assert_is_vice_thread() assert(mainlock_is_vice_thread())
#define mainlock_assert_is_not_vice_thread() assert(!mainlock_is_vice_thread())

#endif /* #ifndef VICE_MAIN_LOCK_H */
