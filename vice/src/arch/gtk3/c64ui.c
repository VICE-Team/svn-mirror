/*
 * c64ui.c - Native GTK3 C64 UI.
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
#include "c64model.h"
#include "vicii.h"
#include "sampler.h"
#include "widgethelpers.h"
#include "machinemodelwidget.h"
#include "videomodelwidget.h"
#include "uisamplersettings.h"

#include "clockportdevicewidget.h"
#include "clockport.h"

#include "georam.h"
#include "georamwidget.h"
#include "reu.h"
#include "reuwidget.h"
#include "cartridge.h"
#include "ramcartwidget.h"
#include "dqbbwidget.h"
#include "expertwidget.h"
#include "isepicwidget.h"
#include "gmod2widget.h"
#include "mmcrwidget.h"
#include "mmc64widget.h"
#include "retroreplaywidget.h"

#include "c64ui.h"


static const char *c64_model_list[] = {
    "C64 PAL", "C64C PAL", "C64 old PAL",
    "C64 NTSC", "C64C NTSC", "C64 old NTSC",
    "Drean",
    "C64 SX PAL", "C64 SX NTSC",
    "Japanese", "C64 GS",
    "PET64 PAL", "PET64 NTSC",
    "Ultimax", NULL
};


static ui_radiogroup_entry_t c64_vicii_models[] = {
    { "PAL",        VICII_MODEL_PALG },
    { "Old PAL",    VICII_MODEL_PALG_OLD },
    { "NTSC",       VICII_MODEL_NTSCM },
    { "Old NTSC",   VICII_MODEL_NTSCM_OLD },
    { "PAL-N",      VICII_MODEL_PALN },
    { NULL, -1 }
};



int c64ui_init(void)
{
    /* Some of the work here is done by video.c now, and would need to
     * be shifted over */

    machine_model_widget_getter(c64model_get);
    machine_model_widget_setter(c64model_set);
    machine_model_widget_set_models(c64_model_list);

    video_model_widget_set_title("VIC-II model");
    video_model_widget_set_resource("MachineVideoStandard");
    video_model_widget_set_models(c64_vicii_models);

    uisamplersettings_set_devices_getter(sampler_get_devices);

    /* work around VSID again */
    clockport_device_widget_set_devices((void *)clockport_supported_devices);

    /* I/O extension function pointers */
    georam_widget_set_save_handler(cartridge_save_image);
    georam_widget_set_flush_handler(cartridge_flush_image);
    reu_widget_set_save_handler(cartridge_save_image);
    reu_widget_set_flush_handler(cartridge_flush_image);
    ramcart_widget_set_save_handler(cartridge_save_image);
    ramcart_widget_set_flush_handler(cartridge_flush_image);
    dqbb_widget_set_save_handler(cartridge_save_image);
    dqbb_widget_set_flush_handler(cartridge_flush_image);
    expert_widget_set_save_handler(cartridge_save_image);
    expert_widget_set_flush_handler(cartridge_flush_image);
    isepic_widget_set_save_handler(cartridge_save_image);
    isepic_widget_set_flush_handler(cartridge_flush_image);
    gmod2_widget_set_save_handler(cartridge_save_image);
    gmod2_widget_set_flush_handler(cartridge_flush_image);
    mmcr_widget_set_eeprom_save_func(cartridge_save_image);
    mmcr_widget_set_eeprom_flush_func(cartridge_flush_image);
    mmc64_widget_set_eeprom_save_func(cartridge_save_image);
    mmc64_widget_set_eeprom_flush_func(cartridge_flush_image);
    retroreplay_widget_set_save_func(cartridge_save_image);
    retroreplay_widget_set_flush_func(cartridge_flush_image);


    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


void c64ui_shutdown(void)
{
    INCOMPLETE_IMPLEMENTATION();
}

