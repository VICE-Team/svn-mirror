/** \file   src/arch/gtk3/vic20ui.c
 * \brief   Native GTK3 VIC20 UI
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

#include "machine.h"
#include "machinemodelwidget.h"
#include "not_implemented.h"
#include "sampler.h"
#include "ui.h"
#include "uimachinewindow.h"
#include "settings_sampler.h"
#include "vic.h"
#include "vic20model.h"
#include "videomodelwidget.h"
#include "widgethelpers.h"

#include "cartridge.h"
#include "carthelpers.h"
#include "georamwidget.h"
#include "uicart.h"

#include "vic20ui.h"


/** \brief  List of VIC-20 models
 *
 * Used in the machine-model widget
 */
static const char *vic20_model_list[] = {
    "VIC20 PAL",
    "VIC20 NTSC",
    "VIC21",
    NULL
};


/** \brief  List of VIC models
 *
 * Used in the VIC model widget
 */
static const vice_gtk3_radiogroup_entry_t vic20_vic_models[] = {
    { "PAL", MACHINE_SYNC_PAL },
    { "NTSC", MACHINE_SYNC_NTSC },
    { NULL, -1 }
};


/** \brief  Identify the canvas used to create a window
 *
 * \return  window index on success, -1 on failure
 */
static int identify_canvas(video_canvas_t *canvas)
{
    if (canvas != vic_get_canvas()) {
        return -1;
    }

    return PRIMARY_WINDOW;
}

/** \brief  Pre-initialize the UI before the canvas window gets created
 *
 * \return  0 on success, -1 on failure
 */
int vic20ui_init_early(void)
{
    ui_machine_window_init();
    ui_set_identify_canvas_func(identify_canvas);

    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Initialize the UI
 *
 * \return  0 on success, -1 on failure
 */
int vic20ui_init(void)
{
    machine_model_widget_getter(vic20model_get);
    machine_model_widget_setter(vic20model_set);
    machine_model_widget_set_models(vic20_model_list);

    video_model_widget_set_title("VIC model");
    video_model_widget_set_resource("MachineVideoStandard");
    video_model_widget_set_models(vic20_vic_models);

    settings_sampler_set_devices_getter(sampler_get_devices);

    /* I/O extension function pointers */
    carthelpers_set_functions(cartridge_save_image, cartridge_flush_image,
            cartridge_type_enabled, NULL, NULL);

    /* uicart_set_detect_func(cartridge_detect); only cbm2/plus4 */
/*    uicart_set_list_func(cartridge_get_info_list); */
    uicart_set_attach_func(cartridge_attach_image);
/*    uicart_set_freeze_func(cartridge_trigger_freeze); */
    uicart_set_detach_func(cartridge_detach_image);


    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Shut down the UI
 */
void vic20ui_shutdown(void)
{
    INCOMPLETE_IMPLEMENTATION();
}
