/** \file   archdep_thread.h
 * \brief   Thin wrapper arround pthread / SDL_Thread
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

#ifndef VICE_THREAD_H
#define VICE_THREAD_H

#include <stdbool.h>

typedef void (*thread_function_t)(void *thread);

void archdep_thread_set_main(void);

int archdep_thread_create(void **thread, thread_function_t thread_function);
bool archdep_thread_current_is(void *thread);
bool archdep_thread_current_is_main(void);
void archdep_thread_exit(void *thread);
void archdep_thread_join(void **thread);

void archdep_mutex_create(void **mutex);
void archdep_mutex_destroy(void *mutex);
void archdep_mutex_lock(void *mutex);
void archdep_mutex_unlock(void *mutex);

void archdep_cond_create(void **cond);
void archdep_cond_destroy(void *cond);
void archdep_cond_signal(void *cond);
void archdep_cond_wait(void *cond, void *locked_mutex);

#endif /* #ifndef VICE_THREAD_H */
