/**
 * \file render_queue.c
 * \brief Manage a queue of rendered backbuffers to display
 *
 * \author David Hogan <david.q.hogan@gmail.com>
 */

/* This file is part of VICE, the Versatile Commodore Emulator.
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
#include "render_thread.h"

#include <glib.h>
#include <stdbool.h>
#include <string.h>

#include "archdep.h"
#include "lib.h"
#include "log.h"

struct render_thread_s {
    int index;
    void *lock;    
    GThreadPool *executor;
    bool is_shutdown_initiated;
    bool is_shut_down;
};

#define RENDER_THREAD_MAX 2 /* Current max of two windows (x128) */

static struct render_thread_s threads[RENDER_THREAD_MAX];
static int thread_count;

render_thread_t render_thread_create(render_thread_callback_t callback, void *thread_context)
{
    render_thread_t thread;

    if (thread_count == RENDER_THREAD_MAX) {
        log_error(LOG_ERR, "Reach maximum render thread count (%d), cannot create another", RENDER_THREAD_MAX);
        archdep_vice_exit(-1);
    }

    thread = threads + thread_count;
    memset(thread, 0, sizeof(struct render_thread_s));
    thread->index = thread_count++;

    archdep_mutex_create(&thread->lock);

    thread->executor = g_thread_pool_new(callback, thread_context, 1, TRUE, NULL);

    /* Schedule the init job */
    g_thread_pool_push(thread->executor, (void *)render_thread_init, NULL);

    log_message(LOG_DEFAULT, "Created render thread %d", thread->index);

    return thread;
}

void render_thread_initiate_shutdown(render_thread_t thread)
{
    archdep_mutex_lock(thread->lock);

    if (thread->is_shutdown_initiated) {
        archdep_mutex_unlock(thread->lock);
        return;
    }

    log_message(LOG_DEFAULT, "Initiating render thread %d shutdown", thread->index);
    thread->is_shutdown_initiated = true;

    /* Schedule the shutdown job */
    g_thread_pool_push(thread->executor, (void *)render_thread_shutdown, NULL);

    archdep_mutex_unlock(thread->lock);
}

void render_thread_join(render_thread_t thread)
{
    log_message(LOG_DEFAULT, "Joining render thread %d ...", thread->index);

    /* TODO: We should block until all jobs are done - but there's a race condition deadlock outcome here. Fix needed */
    g_thread_pool_free(thread->executor, TRUE, TRUE);

    archdep_mutex_lock(thread->lock);
    thread->is_shut_down = true;
    archdep_mutex_unlock(thread->lock);

    archdep_mutex_destroy(thread->lock);

    log_message(LOG_DEFAULT, "Joined render thread %d.", thread->index);
}

void render_thread_shutdown_and_join_all(void)
{
    for (int i = 0; i < thread_count; i++) {
        render_thread_initiate_shutdown(threads + i);
    }

    for (int i = 0; i < thread_count; i++) {
        render_thread_join(threads + i);
    }
}

void render_thread_push_job(render_thread_t thread, render_job_t job)
{
    archdep_mutex_lock(thread->lock);

    if (thread->is_shutdown_initiated)
    {
        log_message(LOG_DEFAULT, "Ignoring new render job as render thread %d %s down", thread->index, thread->is_shut_down ? "has shut" : "is shutting");
        archdep_mutex_unlock(thread->lock);
        return;
    }

    g_thread_pool_push(thread->executor, int_to_void_ptr(job), NULL);

    archdep_mutex_unlock(thread->lock);
}
