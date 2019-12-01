/** \file   uimon.c
 * \brief   Headless UI monitor stuff
 *
 * \author  Fabrizio Gennari <fabrizio.ge@tiscali.it>
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

#include "console.h"
#include "uimon.h"


console_t *uimon_window_open(void)
{
    /* printf("%s\n", __func__); */

    return uimon_window_resume();
}

console_t *uimon_window_resume(void)
{
    /* printf("%s\n", __func__); */

    return NULL;
}

void uimon_window_suspend(void)
{
    /* printf("%s\n", __func__); */
}

int uimon_out(const char *buffer)
{
    /* printf("%s\n", __func__); */

    return 0;
}

void uimon_window_close(void)
{
    /* printf("%s\n", __func__); */
}

void uimon_notify_change(void)
{
    /* printf("%s\n", __func__); */
}

void uimon_set_interface(struct monitor_interface_s **interf, int i)
{
    /* printf("%s\n", __func__); */
}

char *uimon_get_in(char **ppchCommandLine, const char *prompt)
{
    /* printf("%s\n", __func__); */
    
    return NULL;
}
