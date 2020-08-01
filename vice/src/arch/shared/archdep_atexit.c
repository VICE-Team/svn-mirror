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

#ifdef USE_NATIVE_GTK3
#include <gtk/gtk.h>
#include <pthread.h>
#endif

#include "archdep.h"
#include "main.h"

#ifdef USE_NATIVE_GTK3
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

#ifdef USE_NATIVE_GTK3

static gboolean exit_from_main_thread(gpointer user_data)
{
    exit(vice_exit_code);

    return FALSE;
}

void archdep_set_main_thread()
{
    main_thread = pthread_self();
}

#endif /* #ifdef USE_NATIVE_GTK3 */

/** \brief  Wrapper around exit()
 *
 * \param[in]   excode  exit code
 */
void archdep_vice_exit(int excode)
{
#ifdef USE_NATIVE_GTK3
    vice_exit_code = excode;

    if (pthread_equal(pthread_self(), main_thread)) {
        exit(excode);
    } else {
        gdk_threads_add_timeout(0, exit_from_main_thread, NULL);
    }
#else
    exit(excode);
#endif
}
