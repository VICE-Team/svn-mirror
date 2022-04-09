/** \file   archdep_exit.c
 * \brief   VICE thread aware archdep_vice_exit
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
#include <stdbool.h>

#ifdef WINDOWS_COMPILE
#include <windows.h>
#include <mmsystem.h>
#include <objbase.h>
#endif

#include <assert.h>

#ifdef USE_GTK3UI
#include <gtk/gtk.h>
#ifdef UNIX_COMPILE
#ifndef MACOS_COMPILE
#include <X11/Xlib.h>
#endif
#endif
#endif /* #ifdef USE_GTK3UI */

static void *lock;
static int vice_exit_code;

#define LOCK()   archdep_mutex_lock(lock)
#define UNLOCK() archdep_mutex_unlock(lock)

#include "archdep.h"
#include "main.h"
#include "mainlock.h"

#ifdef MACOS_COMPILE
#include "macOS-util.h"
#endif

#include "archdep_exit.h"
#include "log.h"

static bool is_exiting;

bool archdep_is_exiting(void) {
    bool result;

    LOCK();
    result = is_exiting;
    UNLOCK();

    return result;
}

static void actually_exit(int exit_code)
{
    LOCK();

    if (is_exiting) {
        log_message(LOG_DEFAULT, "Ignoring recursive call to archdep_vice_exit()");
        UNLOCK();
        return;
    }
    is_exiting = true;

    UNLOCK();

    /* Some exit stuff not safe to run afer exit() is called so we do it here */
    main_exit();

#if defined(WINDOWS_COMPILE)
    /* Relax scheduler accuracy */
    timeEndPeriod(1);
#endif

    archdep_thread_shutdown();
    
    archdep_mutex_destroy(lock);

    exit(exit_code);
}

void archdep_thread_init(void)
{
#if defined(WINDOWS_COMPILE)
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
#endif
}

void archdep_thread_shutdown(void)
{
#if defined(WINDOWS_COMPILE)
    CoUninitialize();
#endif
}

void archdep_set_main_thread(void)
{
    archdep_mutex_create(&lock);

#if defined(MACOS_COMPILE)

    /* macOS specific main thread init written in objective-c */
    vice_macos_set_main_thread();

#elif defined(UNIX_COMPILE)

#ifdef USE_GTK3UI
    /* Our GLX OpenGL init stuff will crash if we let GDK use wayland directly */
    putenv("GDK_BACKEND=x11");
#endif

#ifndef USE_HEADLESSUI
    /* We're calling xlib from our own thread so need this to avoid problems */
    XInitThreads();
#endif

    /* TODO - set UI/main thread priority for X11 */

#elif defined(WINDOWS_COMPILE)

    /* Increase Windows scheduler accuracy */
    timeBeginPeriod(1);

    /* Of course VICE is more important than other puny Windows applications */
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

#endif
}


static void exit_on_main_thread(void)
{
    actually_exit(vice_exit_code);
}


/** \brief  Wrapper around exit()
 *
 * \param[in]   exit_code   exit code
 */
void archdep_vice_exit(int exit_code)
{
    vice_exit_code = exit_code;

    if (archdep_thread_current_is_main()) {
        /* The main thread is calling this, we can shut down directly */
        actually_exit(exit_code);
    } else {
        /* We need the main thread to process the exit handling. */
        mainlock_run_on_main_thread(exit_on_main_thread);

        if (mainlock_is_vice_thread()) {
            /* The vice thread will shut itself down so that archdep_vice_exit does not return */
            mainlock_initiate_shutdown();
            assert(false);
        }
    }
}
