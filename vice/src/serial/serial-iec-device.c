/*
 * serial-iec-device.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "cmdline.h"
#include "iecbus.h"
#include "resources.h"
#include "serial-iec-device.h"
#include "serial.h"


static unsigned int iec_device_enabled[IECBUS_NUM];


static int set_iec_device_enable(resource_value_t v, void *param)
{
    int enable;
    unsigned int unit;

    enable = (int)v;
    unit = (unsigned int)param;

    if ((unit < 4 || unit > 5) && (unit < 8 || unit > 11))
        return -1;

    iec_device_enabled[unit] = enable;

    iecbus_status_set(IECBUS_STATUS_IECDEVICE, unit, enable);

    return 0;
}

static const resource_t resources[] = {
    { "IECDevice4", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[4],
      set_iec_device_enable, (void *)4 },
    { "IECDevice5", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[5],
      set_iec_device_enable, (void *)5 },
    { "IECDevice8", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[8],
      set_iec_device_enable, (void *)8 },
    { "IECDevice9", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[9],
      set_iec_device_enable, (void *)9 },
    { "IECDevice10", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[10],
      set_iec_device_enable, (void *)10 },
    { "IECDevice11", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[11],
      set_iec_device_enable, (void *)11 },
    { NULL }
};

int serial_iec_device_resources_init(void)
{
    return resources_register(resources);
}

static const cmdline_option_t cmdline_options[] = {
    { "-iecdevice4", SET_RESOURCE, 0, NULL, NULL, "IECDevice4",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #4" },
    { "+iecdevice4", SET_RESOURCE, 0, NULL, NULL, "IECDevice4",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #4" },
    { "-iecdevice5", SET_RESOURCE, 0, NULL, NULL, "IECDevice5",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #5" },
    { "+iecdevice5", SET_RESOURCE, 0, NULL, NULL, "IECDevice5",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #5" },
    { "-iecdevice8", SET_RESOURCE, 0, NULL, NULL, "IECDevice8",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #8" },
    { "+iecdevice8", SET_RESOURCE, 0, NULL, NULL, "IECDevice8",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #8" },
    { "-iecdevice9", SET_RESOURCE, 0, NULL, NULL, "IECDevice9",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #9" },
    { "+iecdevice9", SET_RESOURCE, 0, NULL, NULL, "IECDevice9",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #9" },
    { "-iecdevice10", SET_RESOURCE, 0, NULL, NULL, "IECDevice10",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #10" },
    { "+iecdevice10", SET_RESOURCE, 0, NULL, NULL, "IECDevice10",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #10" },
    { "-iecdevice11", SET_RESOURCE, 0, NULL, NULL, "IECDevice11",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #11" },
    { "+iecdevice11", SET_RESOURCE, 0, NULL, NULL, "IECDevice11",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #11" },
    { NULL }
};


int serial_iec_device_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/*------------------------------------------------------------------------*/

/* Implement IEC devices here.  */

/*------------------------------------------------------------------------*/

void serial_iec_device_init(void)
{
}

