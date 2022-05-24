/** \file   main.c
 * \brief   Headless UI startup
 *
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "archdep.h"
#include "cmdline.h"
#include "log.h"
#include "machine.h"
#include "main.h"
#include "resources.h"
#include "video.h"

/* For the ugly hack below */
#ifdef WINDOWS_COMPILE
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
    int init_result;

    init_result = main_program_init(argc, argv);
    if (init_result) {
        log_error(LOG_ERR, "Failed to initialise, will exit");
        return init_result;
    }

    /*
     * VICE is now running on its own thread.
     * Run forever until the client disconnects.
     * TODO: Does VICE still benefit from this?
     */

    for (;;) {
        tick_sleep(tick_per_second() / 60);
    }
}


/** \brief  Exit handler
 */
void main_exit(void)
{
    /* printf("%s\n", __func__); */

    log_message(LOG_DEFAULT, "\nExiting...");

    /* log resources with non default values */
    resources_log_active();
    
    /* log the active config as commandline options */
    cmdline_log_active();

    machine_shutdown();
}
