/*
 * c64model.c - C64 model detection and setting.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

#include "c64-resources.h"
#include "c64model.h"
#include "machine.h"
#include "resources.h"
#include "sid.h"
#include "types.h"
#include "vicii.h"


static int is_new_sid(int model)
{
    switch (model) {
        case SID_MODEL_6581:
        case SID_MODEL_6581R4:
        case SID_MODEL_6581R3_4885:
        case SID_MODEL_6581R3_0486S:
        case SID_MODEL_6581R3_3984:
        case SID_MODEL_6581R4AR_3789:
        case SID_MODEL_6581R3_4485:
        case SID_MODEL_6581R4_1986S:
        default:
            return 0;

        case SID_MODEL_8580:
        case SID_MODEL_8580D:
        case SID_MODEL_8580R5_3691:
        case SID_MODEL_8580R5_3691D:
        case SID_MODEL_8580R5_1489:
        case SID_MODEL_8580R5_1489D:
            return 1;
    }
}

struct model_s {
    int vicii;
    int video;
    int luma;
    int cia;
    int glue;
    int sid;
};

static struct model_s c64models[] = {
    { VICII_MODEL_6569,     MACHINE_SYNC_PAL,     1, 0, 0, SID_MODEL_6581R4AR_3789 },
    { VICII_MODEL_8565,     MACHINE_SYNC_PAL,     1, 1, 1, SID_MODEL_8580R5_3691   },
    { VICII_MODEL_6569R1,   MACHINE_SYNC_PAL,     0, 0, 0, SID_MODEL_6581R4AR_3789 },
    { VICII_MODEL_6567,     MACHINE_SYNC_NTSC,    1, 0, 0, SID_MODEL_6581R4AR_3789 },
    { VICII_MODEL_8562,     MACHINE_SYNC_NTSC,    1, 1, 1, SID_MODEL_8580R5_3691   },
    { VICII_MODEL_6567R56A, MACHINE_SYNC_NTSCOLD, 0, 0, 0, SID_MODEL_6581R4AR_3789 },
    { VICII_MODEL_6572,     MACHINE_SYNC_PALN,    1, 0, 0, SID_MODEL_6581R4AR_3789 }
};

/* ------------------------------------------------------------------------- */

int c64model_get_temp(int vicii_model, int sid_model, int glue_logic,
                      int cia1_model, int cia2_model, int new_luma)
{
    int new_sid;
    int i;

    if (cia1_model != cia2_model) {
        return C64MODEL_UNKNOWN;
    }

    new_sid = is_new_sid(sid_model);

    for (i = 0; i < C64MODEL_NUM; ++i) {
        if ((c64models[i].vicii == vicii_model)
         && (c64models[i].luma == new_luma)
         && (c64models[i].cia == cia1_model)
         && (c64models[i].glue == glue_logic)
         && (is_new_sid(c64models[i].sid) == new_sid)) {
            return i;
        }
    }

    return C64MODEL_UNKNOWN;
}

int c64model_get(void)
{
    int vicii_model, sid_model, glue_logic, cia1_model, cia2_model, new_luma;

    if ((resources_get_int("VICIIModel", &vicii_model) < 0)
     || (resources_get_int("SidModel", &sid_model) < 0)
     || (resources_get_int("GlueLogic", &glue_logic) < 0)
     || (resources_get_int("CIA1Model", &cia1_model) < 0)
     || (resources_get_int("CIA2Model", &cia2_model) < 0)
     || (resources_get_int("VICIINewLuminances", &new_luma) < 0)) {
        return -1;
    }

    return c64model_get_temp(vicii_model, sid_model, glue_logic, 
                            cia1_model, cia2_model, new_luma);
}

void c64model_set_temp(int model, int *vicii_model, int *sid_model,
                       int *glue_logic, int *cia1_model, int *cia2_model,
                       int *new_luma)
{
    int old_model;
    int new_sid_model;
    int old_type;
    int new_type;

    old_model = c64model_get_temp(*vicii_model, *sid_model, *glue_logic, 
                                  *cia1_model, *cia2_model, *new_luma);

    if ((model == old_model) || (model == C64MODEL_UNKNOWN)) {
        return;
    }

    *vicii_model = c64models[model].vicii;
    *cia1_model = c64models[model].cia;
    *cia2_model = c64models[model].cia;
    *glue_logic = c64models[model].glue;
    *new_luma = c64models[model].luma;

    /* Only change the SID model if the model changes from 6581 to 8580
       or ReSID-fp wasn't used. This allows to switch between "pal"/"oldpal"
       without changing the specific SID model. ReSID-fp is enforced, since
       x64sc aims for accuracy. */
    new_sid_model = c64models[model].sid;

    old_type = is_new_sid(*sid_model);
    new_type = is_new_sid(new_sid_model);

    if (((*sid_model >> 8) !=  SID_ENGINE_RESID_FP) || (old_type != new_type)) {
        *sid_model = (SID_ENGINE_RESID_FP << 8 ) | new_sid_model;
    }
}

void c64model_set(int model)
{
    int old_model;
    int old_engine;
    int old_sid_model;
    int old_type;
    int new_sid_model;
    int new_type;

    old_model = c64model_get();

    if ((model == old_model) || (model == C64MODEL_UNKNOWN)) {
        return;
    }

    resources_set_int("VICIIModel", c64models[model].vicii);
    resources_set_int("CIA1Model", c64models[model].cia);
    resources_set_int("CIA2Model", c64models[model].cia);
    resources_set_int("GlueLogic", c64models[model].glue);

    /* Only change the SID model if the model changes from 6581 to 8580
       or ReSID-fp wasn't used. This allows to switch between "pal"/"oldpal"
       without changing the specific SID model. ReSID-fp is enforced, since
       x64sc aims for accuracy. */
    resources_get_int("SidEngine", &old_engine);
    resources_get_int("SidModel", &old_sid_model);
    new_sid_model = c64models[model].sid;

    old_type = is_new_sid(old_sid_model);
    new_type = is_new_sid(new_sid_model);

    if ((old_engine != SID_ENGINE_RESID_FP) || (old_type != new_type)) {
        sid_set_engine_model(SID_ENGINE_RESID_FP, new_sid_model);
    }
}
