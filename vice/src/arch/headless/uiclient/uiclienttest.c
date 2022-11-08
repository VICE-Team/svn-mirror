/** \file   uiclienttest.h
 * \brief   Test program for VICE user interface client library.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "uiclient.h"
#include "uiclientmachine.h"
#include "uiclientutil.h"

static void shutdown_and_exit(uiclient_t *uiclient, int exit_code)
{
    uiclient_destroy(uiclient);

    INFO("Finished.");

    exit(exit_code);
}

static void on_disconnected(uiclient_t *uiclient)
{
    INFO("Disconnected");
    
    shutdown_and_exit(uiclient, 1);
}

static void on_connected(uiclient_t *uiclient, uint32_t emulator)
{
    INFO("Connected");
    
    uiclient_ready(uiclient);
}

static void on_screen_available(uiclient_t *uiclient, char *chip_name)
{
    INFO("Screen available: %s, subscribing", chip_name);
    
    uiclient_subscribe_screen(uiclient, chip_name);
}

int main(int argc, char **argv)
{
    uint16_t server_port;
    uiclient_t *uiclient;
    
    INFO("UI process launched");

    if (argc != 1 + 1) {
        ERR("Usage:\n\t%s <server port>\n", argv[0]);
        exit(1);
    }

    server_port = atoi(argv[1]);
    uiclient =
        uiclient_new(
            on_disconnected,
            on_connected,
            on_screen_available
            );

    if (!uiclient_connect(uiclient, server_port)) {
        shutdown_and_exit(uiclient, 1);
    }

    for (;;) {
        uiclient_poll(uiclient);
        usleep(1000000 / 60);
    }

//    shutdown_and_exit(uiclient, 0);
}
