/** \file   archdep_atexit.c
 * \brief   atexit(3) work arounds
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  Blacky Stardust
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
#include <stdio.h>
#include <stdlib.h>

/* FIXME: Probably shouldn't be doing GTK3 specific stuff in shared archdep code .. maybe? --dqh */

#ifdef USE_NATIVE_GTK3
#include <assert.h>
#include <gtk/gtk.h>
#include <pthread.h>
#endif

#include "archdep.h"

#ifdef USE_NATIVE_GTK3
#include "main.h"
#include "mainlock.h"
#include "render_thread.h"

static int vice_exit_code;
static pthread_t main_thread;
#endif

/** \brief  Register \a function to be called on exit() or return from main
 *
 * Wrapper to work around Windows not handling the C library's atexit()
 * mechanism properly.
 * 
 * Now also used on Windows too because 'properly' wasn't defined here,
 * it seems to work, and we've refactored the shutdown code for multithreading.
 *
 * \param[in]   function    function to call at exit
 *
 * \return  0 on success, 1 on failure
 */
int archdep_vice_atexit(void (*function)(void))
{
    return atexit(function);
}

#ifndef USE_NATIVE_GTK3

/** \brief  Wrapper around exit()
 *
 * \param[in]   excode  exit code
 */
void archdep_vice_exit(int excode)
{
    exit(excode);
}

#else /* #ifndef USE_NATIVE_GTK3 */

/*
 * GTK3 needs a more controlled shutdown due to the multiple threads involved.
 * In particular, it's tricky to synchronously shut down rendering threads as
 * certain OpenGL calls can block if the main thread is blocked (either that, or
 * if certain UI resources are destroyed, i'm not sure which at this point --dqh)
 */

static gboolean exit_on_main_thread(gpointer not_used)
{
    assert(pthread_equal(pthread_self(), main_thread));

    /* The render thread MUST be joined before exit() is called otherwise gl calls can deadlock */
    render_thread_shutdown_and_join_all();

    exit(vice_exit_code);

    return FALSE;
}

void archdep_set_main_thread()
{
    main_thread = pthread_self();
}

/** \brief  Wrapper around exit()
 *
 * \param[in]   excode  exit code
 */
void archdep_vice_exit(int excode)
{
    vice_exit_code = excode;

    if (pthread_equal(pthread_self(), main_thread)) {
        /* The main thread is calling this, we can start shutting down directly */
        exit_on_main_thread(NULL);
    } else {
        /* We need the main thread to process the exit handling. */
        gdk_threads_add_timeout(0, exit_on_main_thread, NULL);

        if (mainlock_is_vice_thread()) {
            /* The vice thread should shut itself down so that archdep_vice_exit does not return */
            mainlock_initiate_shutdown();            
        }
    }
}

#endif /* #ifndef USE_NATIVE_GTK3 */
