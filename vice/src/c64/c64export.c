/*
 * c64export.c - Expansion port handling for the C64.
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
#include <string.h>

#include "c64export.h"
#include "translate.h"
#include "uiapi.h"


static const char *usage_roml, *usage_romh;


int c64export_query(const c64export_resource_t *export_res)
{
    if (export_res->use_roml > 0) {
        if (usage_roml != NULL && strcmp(usage_roml, export_res->name)!=0) {
            ui_error(translate_text(IDGS_RESOURCE_S_BLOCKED_BY_S),"ROML", usage_roml);
           return -1;
        }
    }
    if (export_res->use_romh > 0) {
        if (usage_romh != NULL && strcmp(usage_romh, export_res->name)!=0) {
            ui_error(translate_text(IDGS_RESOURCE_S_BLOCKED_BY_S),"ROMH", usage_romh);
            return -1;
        }
    }

    return 0;
}

int c64export_add(const c64export_resource_t *export_res)
{
    if (c64export_query(export_res) < 0)
        return -1;

    if (export_res->use_roml > 0)
        usage_roml = export_res->name;
    if (export_res->use_romh > 0)
        usage_romh = export_res->name;

    return 0;
}

int c64export_remove(const c64export_resource_t *export_res)
{
    if (export_res->use_roml > 0) {
        if (usage_roml == NULL)
            return -1;
    }
    if (export_res->use_romh > 0) {
        if (usage_romh == NULL)
            return -1;
    }

    if (export_res->use_roml > 0)
        usage_roml = NULL;
    if (export_res->use_romh > 0)
        usage_romh = NULL;

    return 0;
}

int c64export_resources_init(void)
{
    usage_roml = NULL;
    usage_romh = NULL;

    return 0;
}
