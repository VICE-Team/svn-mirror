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

#ifndef USE_HEADLESSUI
#ifdef UNIX_COMPILE
#ifndef MACOSX_SUPPORT
#include <X11/Xlib.h>
#endif
#endif
#endif

#ifdef WIN32_COMPILE
#include <windows.h>
#include <mmsystem.h>
#include <objbase.h>
#endif

#include <assert.h>

#ifdef USE_NATIVE_GTK3
#include <gtk/gtk.h>
#include <pthread.h>

#include "mainlock.h"

static int vice_exit_code;
static pthread_t main_thread;
#endif /* #ifdef USE_NATIVE_GTK3 */

#include "archdep.h"
#include "main.h"

#ifdef MACOSX_SUPPORT
#include "macOS-util.h"
#endif

#include "archdep_exit.h"


static volatile bool is_exiting;

bool archdep_is_exiting(void) {
    return is_exiting;
}

static void actually_exit(int exit_code)
{
    /* Some exit stuff not safe to run afer exit() is called so we do it here */
    main_exit();

#if defined(WIN32_COMPILE)
    /* Relax scheduler accuracy */
    timeEndPeriod(1);
#endif

#ifdef USE_VICE_THREAD
    archdep_thread_shutdown();
#endif

    exit(exit_code);
}

#ifdef USE_NATIVE_GTK3

/*
 * GTK3 needs a more controlled shutdown due to the multiple threads involved.
 * In particular, it's tricky to synchronously shut down rendering threads as
 * certain OpenGL calls can block if the main thread is blocked (either that, or
 * if certain UI resources are destroyed, i'm not sure which at this point --dqh)
 */

static gboolean exit_on_main_thread(gpointer not_used)
{
    actually_exit(vice_exit_code);

    return FALSE;
}

void archdep_thread_init(void)
{
#if defined(WIN32_COMPILE)
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
#endif
}

void archdep_thread_shutdown(void)
{
#if defined(WIN32_COMPILE)
    CoUninitialize();
#endif
}

void archdep_set_main_thread(void)
{
    main_thread = pthread_self();

#if defined(MACOSX_SUPPORT)
    
    /* macOS specific main thread init written in objective-c */
    vice_macos_set_main_thread();
    
#elif defined(UNIX_COMPILE)

#ifdef USE_NATIVE_GTK3    
    /* Our GLX OpenGL init stuff will crash if we let GDK use wayland directly */
    putenv("GDK_BACKEND=x11");
#endif

#ifndef USE_HEADLESSUI
    /* We're calling xlib from our own thread so need this to avoid problems */
    XInitThreads();
#endif

    /* TODO - set UI/main thread priority for X11 */

#elif defined(WIN32_COMPILE)
    
    /* Increase Windows scheduler accuracy */
    timeBeginPeriod(1);

    /* Of course VICE is more important than other puny Windows applications */
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    
#endif
}

/** \brief  Wrapper around exit()
 *
 * \param[in]   exit_code   exit code
 */
void archdep_vice_exit(int exit_code)
{
    is_exiting = true;
    vice_exit_code = exit_code;

    if (pthread_equal(pthread_self(), main_thread)) {
        /* The main thread is calling this, we can shut down directly */
        actually_exit(exit_code);
    } else {
        /* We need the main thread to process the exit handling. */
        gdk_threads_add_timeout(0, exit_on_main_thread, NULL);

        if (mainlock_is_vice_thread()) {
            /* The vice thread will shut itself down so that archdep_vice_exit does not return */
            mainlock_initiate_shutdown();
            assert(false);
        }
    }
}

#else /* #ifdef USE_NATIVE_GTK3 */

/** \brief  Wrapper around exit()
 */
void archdep_vice_exit(int exit_code)
{
    actually_exit(exit_code);
}

#endif /* #ifdef USE_NATIVE_GTK3 else */
