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

#include "archdep.h"
#include "cmdline.h"
#include "log.h"
#include "machine.h"
#include "main.h"
#include "mainlock.h"
#include "render_thread.h"
#include "resources.h"
#include "ui.h"
#include "video.h"

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
    int init_result;

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
#ifdef WINDOWS_COMPILE
    _putenv("LANG=C");
#endif

    init_result = main_program_init(argc, argv);
    if (init_result) {
        return init_result;
    }

    gtk_main();

    /*
     * Because gtk_main will  never return, we call archdep_thread_shutdown()
     * for the main thread in the exit subsystem rather than here.
     */
    return 0;
}


/** \brief  Exit handler
 */
void main_exit(void)
{
    log_message(LOG_DEFAULT, "\nExiting...");
    
    /* log resources with non default values */
    resources_log_active();
    
    /* log the active config as commandline options */
    cmdline_log_active();

    /*
     * The render thread MUST be joined before the platform exit() is called
     * otherwise gl calls can deadlock
     */
    render_thread_shutdown_and_join_all();

    /*
     * This needs to happen before machine_shutdown as various things get freed
     * in that process.
     */
    ui_exit();

    vice_thread_shutdown();

    machine_shutdown();
}
