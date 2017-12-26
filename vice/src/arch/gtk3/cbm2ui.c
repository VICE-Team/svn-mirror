/** \file   src/arch/gtk3/cbm2ui.c
 * \brief   Native GTK3 CBM2 UI.
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
#include "cbm2model.h"
#include "machinemodelwidget.h"
#include "sampler.h"
#include "uisamplersettings.h"

#include "cbm2ui.h"

/** \brief  List of CBM-II models
 *
 * Used in the machine-model widget
 *
 * \note    Careful: the first entry has an ID of 2 when calling cbm2model_*()
 *          since xcbm2 skips the 5x0 models.
 */
static const char *cbm2_model_list[] = {
    "CBM 610 PAL", "CBM 610 NTSC", "CBM 620 PAL", "CBM 620 NTSC",
    "CBM 620+ (1M) PAL", "CBM 620+ (1M) NTSC", "CBM 710 NTSC", "CBM 720 NTSC",
    "CBM 720+ (1M) NTSC", NULL
};


int cbm2ui_init_early(void)
{
    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Initialize the UI
 *
 * \return  0 on success, -1 on failure
 */
int cbm2ui_init(void)
{
    /* Some of the work here is done by video.c now, and would need to
     * be shifted over */

    machine_model_widget_getter(cbm2model_get);
    machine_model_widget_setter(cbm2model_set);
    machine_model_widget_set_models(cbm2_model_list);

    uisamplersettings_set_devices_getter(sampler_get_devices);

    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Shut down the UI
 */
void cbm2ui_shutdown(void)
{
    INCOMPLETE_IMPLEMENTATION();
}
