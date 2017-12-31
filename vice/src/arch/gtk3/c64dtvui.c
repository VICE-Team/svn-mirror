/** \file   src/arch/gtk3/c64dtvui.c
 * \brief   Native GTK3 C64DTV UI
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *  Bas Wassink <b.wassink@ziggo.nl>
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
#include "machine.h"
#include "c64dtvmodel.h"
#include "machinemodelwidget.h"
#include "videomodelwidget.h"
#include "uimachinewindow.h"
#include "uisamplersettings.h"

#include "c64ui.h"


/** \brief  List of DTV models
 *
 * Used in the machine-model widget
 */
static const char *c64dtv_model_list[] = {
    "V2 PAL",
    "V2 NTSC",
    "V3 PAL",
    "V3 NTSC",
    "Hummer (NTSC)",
    NULL
};


/** \brief  VIC-II models
 *
 * Used in the VIC-II models widget
 */
static ui_radiogroup_entry_t c64dtv_vicii_models[] = {
     { "PAL-G", MACHINE_SYNC_PAL },
     { "NTSC-M", MACHINE_SYNC_NTSC },
     { NULL, -1 }
};


/** \brief  Pre-initialize the UI before the canvas window gets created
 *
 * \return  0 on success, -1 on failure
 */
int c64dtvui_init_early(void)
{
    ui_machine_window_init();

    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Initialize the UI
 *
 * \return  0 on success, -1 on failure
 */
int c64dtvui_init(void)
{
    /* Some of the work here is done by video.c now, and would need to
     * be shifted over */

    machine_model_widget_getter(dtvmodel_get);
    machine_model_widget_setter(dtvmodel_set);
    machine_model_widget_set_models(c64dtv_model_list);

    video_model_widget_set_title("VIC_II model");
    video_model_widget_set_resource("MachineVideoStandard");
    video_model_widget_set_models(c64dtv_vicii_models);

    uisamplersettings_set_devices_getter(sampler_get_devices);

    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Shut down the UI
 */
void c64dtvui_shutdown(void)
{
    INCOMPLETE_IMPLEMENTATION();
}
