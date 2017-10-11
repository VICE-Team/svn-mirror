/*
 * c128ui.c - Native GTK3 C128 UI.
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
#include "widgethelpers.h"
#include "machine.h"
#include "c128model.h"
#include "sampler.h"
#include "machinemodelwidget.h"
#include "videomodelwidget.h"
#include "uisamplersettings.h"

#include "c128ui.h"


static const char *c128_model_list[] = {
    "C128 PAL", "C128D PAL", "C128 NTSC", "C128D NTSC", NULL
};


static ui_radiogroup_entry_t c128_vicii_models[] = {
    { "PAL", MACHINE_SYNC_PAL }, { "NTSC", MACHINE_SYNC_NTSC }, { NULL, -1 }
};



int c128ui_init(void)
{
    /* Some of the work here is done by video.c now, and would need to
     * be shifted over */

    machine_model_widget_getter(c128model_get);
    machine_model_widget_setter(c128model_set);
    machine_model_widget_set_models(c128_model_list);

    video_model_widget_set_title("VIC-II model");
    video_model_widget_set_resource("MachineVideoStandard");
    video_model_widget_set_models(c128_vicii_models);

    uisamplersettings_set_devices_getter(sampler_get_devices);


    INCOMPLETE_IMPLEMENTATION();
    return 0;
}

void c128ui_shutdown(void)
{
    INCOMPLETE_IMPLEMENTATION();
}

