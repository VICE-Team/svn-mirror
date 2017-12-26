/** \file   src/arch/gtk3/c128ui.c
 * \brief   Native GTK3 C128 UI
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
#include "widgethelpers.h"
#include "machine.h"
#include "c128model.h"
#include "sampler.h"
#include "machinemodelwidget.h"
#include "videomodelwidget.h"
#include "uisamplersettings.h"

#include "clockportdevicewidget.h"
#include "clockport.h"

#include "cartridge.h"
#include "georam.h"
#include "georamwidget.h"
#include "reu.h"
#include "reuwidget.h"
#include "ramcartwidget.h"
#include "dqbbwidget.h"
#include "expertwidget.h"
#include "isepicwidget.h"
#include "gmod2widget.h"
#include "mmcrwidget.h"
#include "mmc64widget.h"
#include "retroreplaywidget.h"
#include "easyflashwidget.h"
#include "rrnetmk3widget.h"
#include "uicart.h"
#include "carthelpers.h"

#include "c128ui.h"


/** \brief  List of C128 models
 *
 * Used in the machine-model widget
 */
static const char *c128_model_list[] = {
    "C128 PAL",
    "C128D PAL",
    "C128 NTSC",
    "C128D NTSC",
    NULL
};


/** \brief  List of VIC-II models
 *
 * Used in the VIC-II model widget
 */
static ui_radiogroup_entry_t c128_vicii_models[] = {
    { "PAL", MACHINE_SYNC_PAL },
    { "NTSC", MACHINE_SYNC_NTSC },
    { NULL, -1 }
};


int c128ui_init_early(void)
{
    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Initialize the UI
 *
 * \return  0 on success, -1 on failure
 */
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
    clockport_device_widget_set_devices((void *)clockport_supported_devices);

    /* I/O extension function pointers */
    carthelpers_set_functions(
            cartridge_save_image,
            cartridge_flush_image,
            cartridge_type_enabled);

    /* uicart_set_detect_func(cartridge_detect); only cbm2/plus4 */
    uicart_set_list_func(cartridge_get_info_list);
    uicart_set_attach_func(cartridge_attach_image);
    uicart_set_freeze_func(cartridge_trigger_freeze);
    uicart_set_detach_func(cartridge_detach_image);
    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Shut down the UI
 */
void c128ui_shutdown(void)
{
    INCOMPLETE_IMPLEMENTATION();
}
