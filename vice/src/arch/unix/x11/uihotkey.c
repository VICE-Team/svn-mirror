/*
 * uihotkeys.c - Implementation of UI hotkeys.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "lib.h"
#include "uihotkey.h"
#include "uimenu.h"


typedef struct {
    ui_hotkey_modifier_t modifier;
    ui_keysym_t keysym;
    ui_callback_t callback;
    ui_callback_data_t client_data;
} registered_hotkey_t;

static registered_hotkey_t *registered_hotkeys = NULL;
static int num_registered_hotkeys;
static int num_allocated_hotkeys;


/* ------------------------------------------------------------------------- */

int ui_hotkey_init(void)
{
    if (registered_hotkeys != NULL) {
        lib_free(registered_hotkeys);
        num_registered_hotkeys = num_allocated_hotkeys = 0;
    }
    return 0;
}

void ui_hotkey_shutdown(void)
{
    lib_free(registered_hotkeys);
}

/* ------------------------------------------------------------------------- */

void ui_hotkey_register(ui_hotkey_modifier_t modifier, signed long keysym,
                        void *callback, void *client_data)
{
    registered_hotkey_t *p;

    if (registered_hotkeys == 0) {
        num_allocated_hotkeys = 32;
        registered_hotkeys = lib_malloc(num_allocated_hotkeys
                                        * sizeof(registered_hotkey_t));
        num_registered_hotkeys = 0;
    } else if (num_registered_hotkeys == num_allocated_hotkeys) {
        num_allocated_hotkeys *= 2;
        registered_hotkeys = lib_realloc(registered_hotkeys,
                                         (num_allocated_hotkeys
                                         * sizeof(registered_hotkey_t)));
    }

    p = registered_hotkeys + num_registered_hotkeys;

    p->modifier = modifier;
    p->keysym = (ui_keysym_t)keysym;
    p->callback = (ui_callback_t)callback;
    p->client_data = (ui_callback_data_t)client_data;

    num_registered_hotkeys++;
}

/* ------------------------------------------------------------------------- */

int ui_dispatch_hotkeys(int key)
{
    int i, ret = 0;
    registered_hotkey_t *p = registered_hotkeys;

    /* XXX: Notice that we don't actually check the hotkey modifiers
       here.  */
    for (i = 0; i < num_registered_hotkeys; i++, p++) {
	if (p->keysym == key) {
	    ((void *(*)(void *, void *, void *))
	     p->callback)(NULL, p->client_data, NULL);
	    ret = 1;
	    break;
	}
    }
    return ret;
}

