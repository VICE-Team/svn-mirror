/** \file   mousedrv.c
 * \brief   Headless UI mouse driver stuff.
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

#include "vsyncapi.h"
#include "mouse.h"
#include "mousedrv.h"


/** \brief Last time the mouse was moved.
 *
 *  \sa mousedrv_get_timestamp
 */
static unsigned long mouse_timestamp = 0;

int mousedrv_cmdline_options_init(void)
{
    /* printf("%s\n", __func__); */
    
    return 0;
}

unsigned long mousedrv_get_timestamp(void)
{
    /* printf("%s\n", __func__); */
    
    return mouse_timestamp;
}

int mousedrv_get_x(void)
{
    /* printf("%s\n", __func__); */
    
    return 0;
}

int mousedrv_get_y(void)
{
    /* printf("%s\n", __func__); */
    
    return 0;
}

void mousedrv_init(void)
{
    /* printf("%s\n", __func__); */
}

void mousedrv_mouse_changed(void)
{
    /* printf("%s\n", __func__); */
}

int mousedrv_resources_init(mouse_func_t *funcs)
{
    /* printf("%s\n", __func__); */
    
    return 0;
}


