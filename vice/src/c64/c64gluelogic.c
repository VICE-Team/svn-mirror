/*
 * c64gluelogic.c - C64 glue logic emulation.
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

#include <stdio.h>

#include "alarm.h"
#include "c64_256k.h"
#include "c64gluelogic.h"
#include "c64mem.h"
#include "cmdline.h"
#include "maincpu.h"
#include "resources.h"
#include "translate.h"
#include "types.h"
#include "vicii.h"

static int glue_logic_type = 0;
static int old_vbank = 0;
static alarm_t *glue_alarm = NULL;

/* ------------------------------------------------------------------------- */

static void perform_vbank_switch(int vbank)
{
    if (c64_256k_enabled) {
        c64_256k_cia_set_vbank(vbank);
    } else {
        mem_set_vbank(vbank);
    }
}

static void glue_alarm_handler(CLOCK offset, void *data)
{
    perform_vbank_switch(old_vbank);
    alarm_unset(glue_alarm);
}

void c64_glue_set_vbank(int vbank, int ddr_flag)
{
    int new_vbank = vbank;
    int update_now = 1;

    if (glue_logic_type == 1) {
        if (((old_vbank ^ vbank) == 3) && ((vbank & (vbank - 1)) == 0) && (vbank != 0)) {
            new_vbank = 3;
            alarm_set(glue_alarm, maincpu_clk + 1);
        } else if (ddr_flag && (vbank < old_vbank) && ((old_vbank ^ vbank) != 3)) {
            /* this is not quite accurate; the results flicker in some cases */
            update_now = 0;
            alarm_set(glue_alarm, maincpu_clk + 1);
        }
    }

    if (update_now) {
        perform_vbank_switch(new_vbank);
    }

    old_vbank = vbank;
}

/* ------------------------------------------------------------------------- */

static int set_glue_type(int val, void *param)
{
    glue_logic_type = val;
    return 0;
}

static const resource_int_t resources_int[] = {
    { "GlueLogic", 0, RES_EVENT_NO, NULL,
      &glue_logic_type, set_glue_type, NULL },
    { NULL }
};

int c64_glue_resources_init(void)
{
    return resources_register_int(resources_int);
}

static const cmdline_option_t cmdline_options[] = {
    { "-gluelogictype", SET_RESOURCE, 1,
      NULL, NULL, "GlueLogic", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_TYPE, IDCLS_SET_GLUE_LOGIC_TYPE,
      NULL, NULL },
    { NULL }
};

int c64_glue_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

void c64_glue_init(void)
{
    glue_alarm = alarm_new(maincpu_alarm_context, "Glue", glue_alarm_handler, NULL);
}

