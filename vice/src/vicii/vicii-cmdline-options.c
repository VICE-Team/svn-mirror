/*
 * vicii-cmdline-options.c - Command-line options for the MOS 6569 (VIC-II)
 * emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
 *  Gunnar Ruthenberg <Krill.Plush@gmail.com>
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
 * */

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "cmdline.h"
#include "machine.h"
#include "raster-cmdline-options.h"
#include "resources.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "vicii-cmdline-options.h"
#include "vicii-resources.h"
#include "vicii-timing.h"
#include "vicii.h"
#include "viciitypes.h"


int border_set_func(const char *value, void *extra_param)
{
   int video;
    resources_get_int("MachineVideoStandard", &video);
   
   if (strcmp(value, "1") == 0 || strcmp(value, "full") == 0) {
       vicii_resources.border_mode = VICII_FULL_BORDERS;
   } else if (strcmp(value, "2") == 0 || strcmp(value, "debug") == 0) {
       vicii_resources.border_mode = VICII_DEBUG_BORDERS;
   } else {
       vicii_resources.border_mode = VICII_NORMAL_BORDERS;
   }

   machine_change_timing(video ^ VICII_BORDER_MODE(vicii_resources.border_mode));

   return 0;
}

/* VIC-II command-line options.  */
#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] =
{
    { "-VICIIborders", CALL_FUNCTION, 1, border_set_func, NULL, "VICIIBorderMode",
      (void *)0, IDCLS_P_MODE, IDCLS_SET_BORDER_MODE },
    { "-VICIIchecksb", SET_RESOURCE, 0, NULL, NULL, "VICIICheckSbColl",
      (void *)1, 0, IDCLS_ENABLE_SPRITE_BACKGROUND },
    { "+VICIIchecksb", SET_RESOURCE, 0, NULL, NULL, "VICIICheckSbColl",
      (void *)0, 0, IDCLS_DISABLE_SPRITE_BACKGROUND },
    { "-VICIIcheckss", SET_RESOURCE, 0, NULL, NULL, "VICIICheckSsColl",
      (void *)1, 0, IDCLS_ENABLE_SPRITE_SPRITE },
    { "+VICIIcheckss", SET_RESOURCE, 0, NULL, NULL, "VICIICheckSsColl",
      (void *)0, 0, IDCLS_DISABLE_SPRITE_SPRITE },
    { "-newluminance", SET_RESOURCE, 0, NULL, NULL, "VICIINewLuminances",
      (void *)1, 0, IDCLS_USE_NEW_LUMINANCES },
    { "+newluminance", SET_RESOURCE, 0, NULL, NULL, "VICIINewLuminances",
      (void *)0, 0, IDCLS_USE_OLD_LUMINANCES },
    { "-saturation", SET_RESOURCE, 1, NULL, NULL, "ColorSaturation", NULL,
      IDCLS_P_0_2000, IDCLS_SET_SATURATION },
    { "-contrast", SET_RESOURCE, 1, NULL, NULL, "ColorContrast", NULL,
      IDCLS_P_0_2000, IDCLS_SET_CONTRAST },
    { "-brightness", SET_RESOURCE, 1, NULL, NULL, "ColorBrightness", NULL,
      IDCLS_P_0_2000, IDCLS_SET_BRIGHTNESS },
    { "-gamma", SET_RESOURCE, 1, NULL, NULL, "ColorGamma", NULL,
      IDCLS_P_0_2000, IDCLS_SET_GAMMA },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] =
{
    { "-VICIIborders", CALL_FUNCTION, 1, border_set_func, NULL, "VICIIBorderMode",
      (void *)0, N_("<mode>"), N_("Set VIC-II border display mode (0: normal, 1: full, 2: debug)") },
    { "-VICIIchecksb", SET_RESOURCE, 0, NULL, NULL, "VICIICheckSbColl",
      (void *)1, NULL, N_("Enable sprite-background collision registers") },
    { "+VICIIchecksb", SET_RESOURCE, 0, NULL, NULL, "VICIICheckSbColl",
      (void *)0, NULL, N_("Disable sprite-background collision registers") },
    { "-VICIIcheckss", SET_RESOURCE, 0, NULL, NULL, "VICIICheckSsColl",
      (void *)1, NULL, N_("Enable sprite-sprite collision registers") },
    { "+VICIIcheckss", SET_RESOURCE, 0, NULL, NULL, "VICIICheckSsColl",
      (void *)0, NULL, N_("Disable sprite-sprite collision registers") },
    { "-newluminance", SET_RESOURCE, 0, NULL, NULL, "VICIINewLuminances",
      (void *)1, NULL, N_("Use new luminances") },
    { "+newluminance", SET_RESOURCE, 0, NULL, NULL, "VICIINewLuminances",
      (void *)0, NULL, N_("Use old luminances") },
    { "-saturation", SET_RESOURCE, 1, NULL, NULL, "ColorSaturation", NULL,
      "<0-2000>", N_("Set saturation of internal calculated palette [1000]") },
    { "-contrast", SET_RESOURCE, 1, NULL, NULL, "ColorContrast", NULL,
      "<0-2000>", N_("Set contrast of internal calculated palette [1100]") },
    { "-brightness", SET_RESOURCE, 1, NULL, NULL, "ColorBrightness", NULL,
      "<0-2000>", N_("Set brightness of internal calculated palette [1100]") },
    { "-gamma", SET_RESOURCE, 1, NULL, NULL, "ColorGamma", NULL,
      "<0-2000>", N_("Set gamma of internal calculated palette [900]") },
    { NULL }
};
#endif

int vicii_cmdline_options_init(void)
{
    if (raster_cmdline_options_chip_init("VICII", vicii.video_chip_cap) < 0)
        return -1;

    return cmdline_register_options(cmdline_options);
}

