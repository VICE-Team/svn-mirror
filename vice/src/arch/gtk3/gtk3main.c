/** \file   gtk3main.c
 * \brief   Native GTK3 UI startup
 *
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <signal.h>
#include <gtk/gtk.h>

#include "log.h"
#include "machine.h"
#include "main.h"
#include "ui.h"
#include "video.h"

#if defined(USE_VICE_THREAD) && defined(UNIX_COMPILE) && !defined(MACOSX_SUPPORT)
#include <X11/Xlib.h>
#endif

/* For the ugly hack below */
#ifdef WIN32_COMPILE
# include "windows.h"
#endif

/** \brief  Program driver
 *
 * \param[in]   argc    argument count
 * \param[in]   argv    argument vector
 *
 * \return  0 on success, any other value on failure
 *
 * \note    This should return either EXIT_SUCCESS on success or EXIT_FAILURE
 *          on failure. Unfortunately, there are a lot of exit(1)/exit(-1)
 *          calls, so don't expect to get a meaningful exit status.
 */
int main(int argc, char **argv)
{
    /*
     * Ugly hack to make the VTE-based monitor behave on 32-bit Windows.
     *
     * Without this, the monitor outputs all sorts of non-ASCII glyphs resulting
     * in either weird tokens and a red background or a nice crash.
     *
     * The Windows C runtime doesn't actually use this env var, but Gtk/GLib
     * does. Ofcourse properly fixing the monitor code would be better, but I've
     * spent all day trying to figure this out, so it'll have to do for now.
     *
     * --Compyx
     */
#ifdef WIN32_COMPILE
    _putenv("LANG=C");
#endif

#if defined(UNIX_COMPILE) && !defined(MACOSX_SUPPORT)
    /* Our GLX OpenGL init stuff will crash if we let GDK use wayland directly */
    putenv("GDK_BACKEND=x11");

    /* We're calling xlib from our own thread so need this to avoid problems */
    XInitThreads();
#endif

    int init_result = main_program(argc, argv);
    if (init_result) {
        return init_result;
    }

    gtk_main();
    
    return 0;
}

/** \brief  Exit handler
 */
void main_exit(void)
{
    /* Disable SIGINT.  This is done to prevent the user from keeping C-c
       pressed and thus breaking the cleanup process, which might be
       dangerous.  */
    signal (SIGINT, SIG_IGN);

    ui_exit();

    vice_thread_shutdown();
}
