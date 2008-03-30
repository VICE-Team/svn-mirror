/*
 * ram.c - RAM stuff.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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
#include <string.h>

#include "cmdline.h"
#include "ram.h"
#include "resources.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"


static int start_value;
static int value_invert;
static int pattern_invert;

static int set_start_value(resource_value_t v, void *param)
{
    start_value = (int)v;
    if (start_value < 0)
        start_value = 0;
    if (start_value > 0xff)
        start_value = 0xff;
    return 0;
}

static int set_value_invert(resource_value_t v, void *param)
{
    value_invert = (int)v;
    return 0;
}

static int set_pattern_invert(resource_value_t v, void *param)
{
    pattern_invert = (int)v;
    return 0;
}


/* RAM-related resources. */
static const resource_t resources[] = {
    { "RAMInitStartValue", RES_INTEGER, (resource_value_t)0,
      (void *)&start_value, set_start_value, NULL },
    { "RAMInitValueInvert", RES_INTEGER, (resource_value_t)64,
      (void *)&value_invert, set_value_invert, NULL },
    { "RAMInitPatternInvert", RES_INTEGER, (resource_value_t)0,
      (void *)&pattern_invert, set_pattern_invert, NULL },
    { NULL }
};

int ram_resources_init(void)
{
    return resources_register(resources);
}

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] = {
    { "-raminitstartvalue", SET_RESOURCE, 1, NULL, NULL,
      "RAMInitStartValue", NULL,
      IDCLS_P_VALUE, IDCLS_SET_FIRST_RAM_ADDRESS_VALUE },
    { "-raminitvalueinvert" , SET_RESOURCE, 1, NULL, NULL,
      "RAMInitValueInvert", NULL,
      IDCLS_P_NUM_OF_BYTES, IDCLS_LENGTH_BLOCK_SAME_VALUE },
    { "-raminitpatterninvert", SET_RESOURCE, 1, NULL, NULL,
      "RAMInitPatternInvert", NULL,
      IDCLS_P_NUM_OF_BYTES, IDCLS_LENGTH_BLOCK_SAME_PATTERN },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-raminitstartvalue", SET_RESOURCE, 1, NULL, NULL,
      "RAMInitStartValue", NULL,
      N_("<value>"), N_("Set the value for the very first RAM address after powerup") },
    { "-raminitvalueinvert", SET_RESOURCE, 1, NULL, NULL,
      "RAMInitValueInvert", NULL,
      N_("<num of bytes>"), N_("Length of memory block initialized with the same value") },
    { "-raminitpatterninvert", SET_RESOURCE, 1, NULL, NULL,
      "RAMInitPatternInvert", NULL,
      N_("<num of bytes>"), N_("Length of memory block initialized with the same pattern") },
    { NULL }
};
#endif

int ram_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}


void ram_init(BYTE *memram, unsigned int ramsize)
{

    unsigned int i;
    BYTE v = start_value;

    for (i = 0; i < ramsize; i++)
    {
        memram[i] = v;

        if (value_invert > 0 && (i + 1) % value_invert == 0)
            v ^= 0xff;

        if (pattern_invert > 0 && (i + 1) % pattern_invert == 0)
            v ^= 0xff;
    }
}


const char *ram_init_print_pattern(void)
{
    static char s[512], s_tmp[16], pattern_line[64];
    BYTE v = start_value;
    int i;
    int linenum = 0;
    int last_line_drawn = 0;

    s[0] = 0;

    do {
        pattern_line[0] = 0;

        for (i = 0; i < 8; i++) {
            sprintf(s_tmp," %02x", v);
            
            strcat(pattern_line, s_tmp);

            if (value_invert > 0
                && (linenum * 8 + i + 1) % value_invert == 0)
            {
                v ^= 0xff;
            }

            if (pattern_invert > 0
                && (linenum * 8 + i + 1) % pattern_invert == 0)
            {
                v ^= 0xff;
            }
        }

        if (linenum * 8 == 0
            || linenum * 8 == value_invert
            || linenum * 8 == pattern_invert
            || linenum * 8 == pattern_invert + value_invert)
        {
            sprintf(s_tmp, "%04x ", linenum * 8);
            strcat(s, s_tmp);
            strcat(s, pattern_line);
            strcat(s, "\n");
            last_line_drawn = 1;
        } else {
            if (last_line_drawn == 1)
                strcat(s, "...\n");
            last_line_drawn = 0;
        }

        linenum++;

    } while (linenum * 8 < value_invert * 2 || linenum * 8 < pattern_invert * 2);

    if (last_line_drawn == 1)
        strcat(s, "...\n");

    return s;
}
