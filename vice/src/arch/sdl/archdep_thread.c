/** \file   archdep_thread.c
 * \brief   SDL_Thread implementation of archdep_thread.h
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

#include "vice.h"

#include "vice_sdl.h"
#include <SDL_thread.h>
#include <setjmp.h>
#include <stdlib.h>

#include "archdep_thread.h"
#include "lib.h"
#include "log.h"

static SDL_threadID main_thread_id;

typedef struct thread_internal_s {
    SDL_Thread *thread;
    jmp_buf exit_jmp;
    thread_function_t function;
} thread_internal_t;


void archdep_thread_set_main(void)
{
    main_thread_id = SDL_ThreadID();
}


static int archdep_thread_wrap(void *thread)
{
    int thread_result;
    thread_internal_t *thread_internal = thread;

    thread_result = setjmp(thread_internal->exit_jmp);

    if (thread_result == 0) {
        thread_internal->function(thread);
    }
    
    return 0;
}


int archdep_thread_create(void **thread, thread_function_t thread_function)
{
    thread_internal_t *thread_internal = lib_malloc(sizeof(thread_internal_t));
    thread_internal->function = thread_function;
    *thread = thread_internal;

#ifdef USE_SDL2UI
    thread_internal->thread = SDL_CreateThread(archdep_thread_wrap, "VICE thread", thread_internal);
#else
    thread_internal->thread = SDL_CreateThread(archdep_thread_wrap, thread_internal);
#endif
    
    if (thread_internal->thread == NULL) {
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

    return SDL_ThreadID() == SDL_GetThreadID(thread_internal->thread);
}


bool archdep_thread_current_is_main(void)
{
    return SDL_ThreadID() == main_thread_id;
}


void archdep_thread_exit(void *thread)
{
    thread_internal_t *thread_internal = thread;

    longjmp(thread_internal->exit_jmp, 1);
}


void archdep_thread_join(void **thread)
{
    thread_internal_t *thread_internal = *thread;
    int thread_result;
    
    SDL_WaitThread(thread_internal->thread, &thread_result);

    lib_free(*thread);
    *thread = NULL;
}


void archdep_mutex_create(void **mutex)
{
    *mutex = SDL_CreateMutex();
}


void archdep_mutex_destroy(void *mutex)
{
    SDL_DestroyMutex(mutex);
}


void archdep_mutex_lock(void *mutex)
{
    SDL_LockMutex(mutex);
}


void archdep_mutex_unlock(void *mutex)
{
    SDL_UnlockMutex(mutex);
}


void archdep_cond_create(void **cond)
{
    *cond = SDL_CreateCond();
}


void archdep_cond_destroy(void *cond)
{
    SDL_DestroyCond(cond);
}

void archdep_cond_signal(void *cond)
{
    SDL_CondSignal(cond);
}


void archdep_cond_wait(void *cond, void *locked_mutex)
{
    SDL_CondWait(cond, locked_mutex);
}
