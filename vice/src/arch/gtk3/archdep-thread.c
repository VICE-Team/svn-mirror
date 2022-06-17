/**
 * \file archdep-thread.c
 *
 * \author David Hogan <david.q.hogan@gmail.com>
 */

/* This file is part of VICE, the Versatile Commodore Emulator.
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

#include <gtk/gtk.h>

#include "archdep_thread.h"
#include "lib.h"


typedef struct wrapper_s {
    main_thread_function_t callback;
    void *data;
} wrapper_t;

static gboolean timeout_wrapper(gpointer data)
{
    wrapper_t *wrapper = (wrapper_t*)data;
    wrapper->callback(wrapper->data);
    lib_free(wrapper);
    
    return FALSE;
}

void archdep_thread_run_on_main(main_thread_function_t callback, void *data)
{
    wrapper_t *wrapper = lib_malloc(sizeof(wrapper_t));
    wrapper->callback = callback;
    wrapper->data = data;
    gdk_threads_add_timeout(0, timeout_wrapper, (gpointer)wrapper);
}
