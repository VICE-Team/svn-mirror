/*
 * vsid-cmdline-options.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "c64model.h"
#include "c64rom.h"
#include "c64-cmdline-options.h"
#include "c64-resources.h"
#include "cmdline.h"
#include "log.h"
#include "machine.h"
#include "patchrom.h"
#include "resources.h"
#include "vicii.h"

static int set_video_standard(const char *param, void *extra_param)
{
    int value = vice_ptr_to_int(extra_param);
    int vicii_model;

    switch (machine_class) {
        case VICE_MACHINE_C64SC:
            resources_get_int("VICIIModel", &vicii_model);
            switch (value) {
                case MACHINE_SYNC_PAL:
                default:
                    if (vicii_model == VICII_MODEL_8562 || vicii_model == VICII_MODEL_8565) {
                        return resources_set_int("VICIIModel", VICII_MODEL_8565);
                    } else if (vicii_model == VICII_MODEL_6567R56A) {
                        return resources_set_int("VICIIModel", VICII_MODEL_6569R1);
                    } else {
                        return resources_set_int("VICIIModel", VICII_MODEL_6569);
                    }
                    break;
                case MACHINE_SYNC_NTSC:
                    if (vicii_model == VICII_MODEL_8562 || vicii_model == VICII_MODEL_8565) {
                        return resources_set_int("VICIIModel", VICII_MODEL_8562);
                    } else {
                        return resources_set_int("VICIIModel", VICII_MODEL_6567);
                    }
                    break;
                case MACHINE_SYNC_NTSCOLD:
                        return resources_set_int("VICIIModel", VICII_MODEL_6567R56A);
                case MACHINE_SYNC_PALN:
                        return resources_set_int("VICIIModel", VICII_MODEL_6572);
            }
        default:
            return resources_set_int("MachineVideoStandard", value);
    }
}

static const cmdline_option_t cmdline_options[] =
{
    { "-pal", CALL_FUNCTION, CMDLINE_ATTRIB_NONE,
      set_video_standard, (void *)MACHINE_SYNC_PAL, NULL, NULL,
      NULL, "Use PAL sync factor" },
    { "-ntsc", CALL_FUNCTION, CMDLINE_ATTRIB_NONE,
      set_video_standard, (void *)MACHINE_SYNC_NTSC, NULL, NULL,
      NULL, "Use NTSC sync factor" },
    { "-ntscold", CALL_FUNCTION, CMDLINE_ATTRIB_NONE,
      set_video_standard, (void *)MACHINE_SYNC_NTSCOLD, NULL, NULL,
      NULL, "Use old NTSC sync factor" },
    { "-paln", CALL_FUNCTION, CMDLINE_ATTRIB_NONE,
      set_video_standard, (void *)MACHINE_SYNC_PALN, NULL, NULL,
      NULL, "Use PAL-N sync factor" },
    { "-hvsc-root", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
        NULL, NULL, "HVSCRoot", NULL,
        "<path>", "Set path to HVSC root directory" },
    CMDLINE_LIST_END
};

int c64_cmdline_options_init(void)
{
    printf("VSID CMDLINE INIT!\n");
    return cmdline_register_options(cmdline_options);
}
