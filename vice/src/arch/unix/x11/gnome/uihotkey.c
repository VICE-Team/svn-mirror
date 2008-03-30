/*
 * uihotkeys.h - Implementation of UI hotkeys.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "uihotkey.h"

#include "utils.h"

typedef struct {
    ui_hotkey_modifier_t modifier;
    KeySym keysym;
    ui_callback_t callback;
    ui_callback_data_t client_data;
} registered_hotkey_t;

static registered_hotkey_t *registered_hotkeys;
static int num_registered_hotkeys;
static int num_allocated_hotkeys;

static int meta_count, control_count, shift_count;

/* ------------------------------------------------------------------------- */

int ui_hotkey_init(void)
{
    if (registered_hotkeys != NULL) {
        free(registered_hotkeys);
        num_registered_hotkeys = num_allocated_hotkeys = 0;
        meta_count = control_count = shift_count = 0;
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

void ui_hotkey_register(ui_hotkey_modifier_t modifier,
                        KeySym keysym, ui_callback_t callback,
                        ui_callback_data_t client_data)
{
    registered_hotkey_t *p;

    if (registered_hotkeys == 0) {
        num_allocated_hotkeys = 32;
        registered_hotkeys = xmalloc(num_allocated_hotkeys
                                     * sizeof(registered_hotkey_t));
        num_registered_hotkeys = 0;
    } else if (num_registered_hotkeys == num_allocated_hotkeys) {
        num_allocated_hotkeys *= 2;
        registered_hotkeys = xrealloc(registered_hotkeys,
                                      (num_allocated_hotkeys
                                       * sizeof(registered_hotkey_t)));
    }

    p = registered_hotkeys + num_registered_hotkeys;

    p->modifier = modifier;
    p->keysym = keysym;
    p->callback = callback;
    p->client_data = client_data;

    num_registered_hotkeys++;
}

/* ------------------------------------------------------------------------- */

/*
void ui_hotkey_event_handler(Widget w, XtPointer closure,
                             XEvent *xevent,
                             Boolean *continue_to_dispatch)
*/
void ui_hotkey_event_handler(GtkWidget *w, GdkEvent *report,gpointer gp )
{
    gint keysym;
    int i;

    keysym = report->key.keyval;

    /* Bad things could happen if focus goes away and then comes
       back...  */
    if (report->type == GDK_FOCUS_CHANGE) {
        meta_count = control_count = shift_count = 0;
        return;
    }

    switch (keysym) {
      case XK_Shift_L:
      case XK_Shift_R:
        if (report->type == GDK_KEY_PRESS)
            shift_count++;
        else if (report->type == GDK_KEY_RELEASE && shift_count > 0)
            shift_count--;
        break;

      case XK_Control_L:
      case XK_Control_R:
        if (report->type == GDK_KEY_PRESS)
            control_count++;
        else if (report->type == GDK_KEY_RELEASE && control_count > 0)
            control_count--;
        break;

      case XK_Meta_L:
      case XK_Meta_R:
#ifdef ALT_AS_META
      case XK_Alt_L:
      case XK_Alt_R:
#endif
#ifdef MODE_SWITCH_AS_META
      case XK_Mode_switch:
#endif
        if (report->type == GDK_KEY_PRESS)
            meta_count++;
        else if (report->type == GDK_KEY_RELEASE && meta_count > 0)
            meta_count--;
        break;

      default:
        if (report->type == GDK_KEY_PRESS && meta_count != 0) {
            registered_hotkey_t *p = registered_hotkeys;

            /* XXX: Notice that we don't actually check the hotkey modifiers
               here.  */
	    for (i = 0; i < num_registered_hotkeys; i++, p++) {
                if (p->keysym == keysym) {
		  ((void*(*)(GtkWidget*, ui_callback_data_t))
		    p->callback) (NULL, p->client_data);
		  break;
                }
            }
        }
    }
}
