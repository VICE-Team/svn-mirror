/*
 * shortbus.c - IDE64 Short Bus emulation.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
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
#include <string.h>

#include "shortbus_digimax.h"

/* TODO */
#if 0
#include "shortbus_duart.h"
#include "shortbus_etfe.h"
#include "shortbus_eth64.h"
#endif

int shortbus_resources_init(void)
{
    if (shortbus_digimax_resources_init() < 0) {
        return -1;
    }

/* TODO */
#if 0
    if (shortbus_duart_resources_init() < 0) {
        return -1;
    }

    if (shortbus_etfe_resources_init() < 0) {
        return -1;
    }

    if (shortbus_eth64_resources_init() < 0) {
        return -1;
    }
#endif

    return 0;
}

void shortbus_resources_shutdown(void)
{
    shortbus_digimax_resources_shutdown();

/* TODO */
#if 0
    shortbus_duart_resources_shutdown();
    shortbus_etfe_resources_shutdown();
    shortbus_eth64_resources_shutdown();
#endif

}

int shortbus_cmdline_options_init(void)
{
    if (shortbus_digimax_cmdline_options_init() < 0) {
        return -1;
    }

/* TODO */
#if 0
    if (shortbus_duart_cmdline_options_init() < 0) {
        return -1;
    }

    if (shortbus_etfe_cmdline_options_init() < 0) {
        return -1;
    }

    if (shortbus_eth64_cmdline_options_init() < 0) {
        return -1;
    }
#endif

    return 0;
}

extern void shortbus_unregister(void)
{
    shortbus_digimax_unregister();

/* TODO */
#if 0
    shortbus_duart_unregister();
    shortbus_etfe_unregister();
    shortbus_eth64_unregister();
#endif
}


extern void shortbus_register(void)
{
    shortbus_digimax_register();

/* TODO */
#if 0
    shortbus_duart_register();
    shortbus_etfe_register();
    shortbus_eth64_register();
#endif
}

void shortbus_reset(void)
{
    shortbus_digimax_reset();

/* TODO */
#if 0
    shortbus_duart_reset();
    shortbus_etfe_reset();
    shortbus_eth64_reset();
#endif
}
