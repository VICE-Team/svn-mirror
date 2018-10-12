/** \file   archdep_atexit.c
 * \brief   atexit(3) work arounds
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  Blacky Stardust
 */

/*
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

#include "archdep_atexit.h"


/*****************************************************************************
 *                  Temporary windows atexit() crash workaround              *
 ****************************************************************************/

/* FIXME: we need to move the whole atexit() and exit() mechanism to archdep,
          since otherwise it would crash on windows when using the GTK3 UI. */

#if defined(USE_NATIVE_GTK3) && defined(WIN32_COMPILE) && !defined(__cplusplus)
#define ATEXIT_MAX_FUNCS 64

#include "debug_gtk3.h"

static void (*atexit_functions[ATEXIT_MAX_FUNCS + 1])(void);

static int atexit_counter = 0;


int vice_atexit(void (*function)(void))
{
    INCOMPLETE_IMPLEMENTATION();
    debug_gtk3("registering function %p.", function);
    if (atexit_counter == ATEXIT_MAX_FUNCS) {
        debug_gtk3("ERROR: max atexit functions reached.");
        return 1;
    }

    atexit_functions[atexit_counter] = function;
    atexit_counter++;

    return 0;
}

void vice_exit(int excode)
{
    const void (*f)(void);

    INCOMPLETE_IMPLEMENTATION();
    debug_gtk3("unrolling atexit stack:");
    /* don't check for NULL, segfaults allow backtraces in gdb */
    while (atexit_counter > 0) {
        atexit_counter--;
        f = atexit_functions[atexit_counter];
        debug_gtk3("running atexit %d: %p.", atexit_counter, f)
        f();
    }
    exit(excode);
}
#else  /* ifdef WIN32_COMPILE */

int vice_atexit(void (*function)(void))
{
    return atexit(function);
}


void vice_exit(int excode)
{
    exit(excode);
}

#endif


