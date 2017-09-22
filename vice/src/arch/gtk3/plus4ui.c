/*
 * plus4ui.c - Native GTK3 PLUS4 UI.
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

#include "not_implemented.h"
#include "plus4model.h"
#include "machinemodelwidget.h"

#include "plus4ui.h"


static const char *plus4_model_list[] = {
    "C16 PAL", "C16 NTSC", "Plus4 PAL", "Plus4 NTSC", "V364 NTSC", "232 NTSC",
    NULL
};


int plus4ui_init(void)
{
    /* Some of the work here is done by video.c now, and would need to
     * be shifted over */

    machine_model_widget_getter(plus4model_get);
    machine_model_widget_setter(plus4model_set);
    machine_model_widget_set_models(plus4_model_list);

    INCOMPLETE_IMPLEMENTATION();
    return 0;
}

void plus4ui_shutdown(void)
{
    INCOMPLETE_IMPLEMENTATION();
}

