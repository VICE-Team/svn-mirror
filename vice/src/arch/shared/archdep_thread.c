/** \file   archdep_thread.c
 * \brief   pthread implementation of archdep_thread.h
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

#ifdef USE_SDL2UI
#error This file should not be used for SDL builds
#endif

#include "vice.h"

#include <pthread.h>
#include <setjmp.h>
#include <stdlib.h>

#include "archdep_thread.h"
#include "archdep_tick.h"
#include "lib.h"
#include "log.h"

#define DEBUG_MUTEX_LONG_BLOCK 0
#define DEBUG_MUTEX_LONG_HOLD  0

static pthread_t main_thread;

typedef struct thread_internal_s {
    pthread_t pthread;
    jmp_buf exit_jmp;
    thread_function_t function;
} thread_internal_t;

typedef struct mutex_internal_s {
    pthread_mutex_t mutex;
#if DEBUG_MUTEX_LONG_HOLD
    bool ignore_long_hold;
    tick_t lock_tick;
    int lock_level;
#endif
} mutex_internal_t;


void archdep_thread_set_main(void)
{
    main_thread = pthread_self();
}


static void *archdep_thread_wrap(void *thread)
{
    int thread_result;
    thread_internal_t *thread_internal = thread;

    thread_result = setjmp(thread_internal->exit_jmp);

    if (thread_result == 0) {
        thread_internal->function(thread);
    }
    
    return NULL;
}


int archdep_thread_create(void **thread, thread_function_t thread_function)
{
    thread_internal_t *thread_internal = lib_malloc(sizeof(thread_internal_t));
    thread_internal->function = thread_function;
    *thread = thread_internal;

    if (pthread_create(&thread_internal->pthread, NULL, archdep_thread_wrap, thread_internal)) {
        lib_free(thread);
        return -1;
    }

    return 0;
}


bool archdep_thread_current_is(void *thread)
{
    thread_internal_t *thread_internal = thread;
    
    if (!thread) {
        return false;
    }

    return pthread_equal(pthread_self(), thread_internal->pthread);
}


bool archdep_thread_current_is_main(void)
{
    return pthread_equal(pthread_self(), main_thread);
}


void archdep_thread_exit(void *thread)
{
    thread_internal_t *thread_internal = thread;

    longjmp(thread_internal->exit_jmp, 1);
}


void archdep_thread_join(void **thread)
{
    thread_internal_t *thread_internal = *thread;

    pthread_join(thread_internal->pthread, NULL);

    lib_free(*thread);
    *thread = NULL;
}


void archdep_mutex_create(void **mutex)
{
    /* can't use lib_malloc HERE, as lib_malloc calls archdep_mutex_lock in debug builds */
    mutex_internal_t *mutex_internal = malloc(sizeof(mutex_internal_t));
    pthread_mutexattr_t lock_attributes;

    /*
     * SDL mutex are always recursive, so set up pthread mutex the same way to
     * avoid arch specific locking bugs
     */

    pthread_mutexattr_init(&lock_attributes);
    pthread_mutexattr_settype(&lock_attributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex_internal->mutex, &lock_attributes);
    pthread_mutexattr_destroy(&lock_attributes);

#if DEBUG_MUTEX_LONG_HOLD
    mutex_internal->ignore_long_hold = false;
#endif

    *mutex = mutex_internal;
}


void archdep_mutex_destroy(void *mutex)
{
    mutex_internal_t *mutex_internal = (mutex_internal_t*)mutex;

    pthread_mutex_destroy(&mutex_internal->mutex);

    free(mutex);
}


void archdep_mutex_lock(void *mutex)
{
    mutex_internal_t *mutex_internal = (mutex_internal_t*)mutex;

#if DEBUG_MUTEX_LONG_BLOCK
    tick_t before = tick_now();

    pthread_mutex_lock(&mutex_internal->mutex);

    uint32_t deltaMs = TICK_TO_MILLI(tick_now_delta(before));

    if (deltaMs >= 10) {
        printf("long block %u ms on mutex\n", deltaMs);
    }
#else
    pthread_mutex_lock(&mutex_internal->mutex);
#endif

#if DEBUG_MUTEX_LONG_HOLD
    if (++mutex_internal->lock_level == 1) {
        mutex_internal->lock_tick = tick_now();
    }
#endif
}


void archdep_mutex_unlock(void *mutex)
{
    mutex_internal_t *mutex_internal = (mutex_internal_t*)mutex;

#if DEBUG_MUTEX_LONG_HOLD
    if (--mutex_internal->lock_level == 0 && !mutex_internal->ignore_long_hold)
    {
        uint32_t deltaMs = TICK_TO_MILLI(tick_now_delta(mutex_internal->lock_tick));
        if (deltaMs >= 40) {
            printf("long hold %u ms on mutex\n", deltaMs);
        }
    }
#endif

    pthread_mutex_unlock(&mutex_internal->mutex);
}


void archdep_cond_create(void **cond)
{
    *cond = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(*cond, NULL);
}


void archdep_cond_destroy(void *cond)
{
    pthread_cond_destroy(cond);
}

void archdep_cond_signal(void *cond)
{
    pthread_cond_signal(cond);
}


void archdep_cond_wait(void *cond, void *locked_mutex)
{
    pthread_cond_wait(cond, locked_mutex);
}
