/*
 * mon_util.c - Utilities for the VICE built-in monitor.
 *
 * Written by
 *  Spiro Trikaliotis <spiro.trikaliotis@gmx.de>
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

#include <stdlib.h>

#include "archdep.h"
#include "console.h"
#include "lib.h"
#include "mem.h"
#include "mon_disassemble.h"
#include "mon_util.h"
#include "monitor.h"
#include "types.h"
#include "uimon.h"
#include "utils.h"


int mon_out(const char *format, ...)
{
    va_list ap;
    char *buffer;
    int rc = 0;

    va_start(ap, format);
    buffer = lib_mvsprintf(format, ap);
    rc = uimon_out(buffer);
    lib_free(buffer);

    if (rc < 0)
        mon_abort();

    return rc;
}

char *mon_disassemble_with_label(MEMSPACE memspace, WORD loc, int hex,
                                 unsigned *opc_size_p, unsigned *label_p)
{
    const char *p;

    if (*label_p == 0) {
        /* process a label, if available */
        p = mon_symbol_table_lookup_name(memspace, loc);
        if (p) {
            *label_p = 1;
            *opc_size_p = 0;
            return lib_msprintf("%s:",p);
        }
    } else {
        *label_p = 0;
    }

    /* process the disassembly itself */
    p = mon_disassemble_to_string_ex(memspace, loc,
                                     mon_get_mem_val(memspace, loc),
                                     mon_get_mem_val(memspace,
                                                     (WORD)(loc + 1)),
                                     mon_get_mem_val(memspace,
                                                     (WORD)(loc + 2)),
                                     mon_get_mem_val(memspace,
                                                     (WORD)(loc + 3)),
                                     hex,
                                     opc_size_p);

    return lib_msprintf((hex ? "%04X: %s%10s" : "05u: %s%10s"), loc, p, "");
}

#ifndef __OS2__
char *pchCommandLine = NULL;

void mon_set_command(console_t *console_log, char *command,
                     void (*pAfter)(void))
{
    pchCommandLine = command;

    if (console_log)
        console_out(console_log, "%s\n", command);

    if (pAfter)
        (*pAfter)();
}

char *uimon_in(const char *prompt)
{
    char *p = NULL;

    while (!p && !pchCommandLine) {
        /* as long as we don't have any return value... */

        p = uimon_get_in(&pchCommandLine, prompt);
    }

    if (pchCommandLine) {
        /* we have an "artificially" generated command line */

        if (p)
            lib_free(p);

        p = lib_stralloc(pchCommandLine);
        pchCommandLine = NULL;
    }

    /* return the command (the one or other way...) */
    return p;
}
#endif

