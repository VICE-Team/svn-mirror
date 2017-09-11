/*
 * soundoutputmodewidget.c - GTK3 sound output mode widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  SoundOutput - output mode: system (0), mono (1), stereo (2) (int)
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
 */


#include "vice.h"

#include <gtk/gtk.h>

#include "lib.h"
#include "ui.h"
#include "resources.h"
#include "vsync.h"
#include "sound.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"

#include "soundoutputmodewidget.h"


/** \brief  List of sound output modes
 */
static ui_text_int_pair_t output_modes[] = {
    { "System", SOUND_OUTPUT_SYSTEM },
    { "Mono", SOUND_OUTPUT_MONO },
    { "Stereo", SOUND_OUTPUT_STEREO },
    { NULL, -1 }
};


/** \brief  Event handler to alter the "SoundMode" resource
 *
 * \param[in]   widget      widget triggering the callback
 * \param[in]   user_data   the SoundMode setting (`int`)
 *
 */
static void on_output_mode_changed(GtkWidget *widget, gpointer user_data)
{
    int mode = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        debug_gtk3("got sound output mode %d\n", mode);
        resources_set_int("SoundOutput", mode);
    }
}


/** \brief  Create widget for "Sound output mode"
 *
 * A simple list of radio buttons for sound output mode: System, Mono or Stereo
 *
 * \return  grid
 */
GtkWidget *create_sound_output_mode_widget(void)
{
    GtkWidget *layout;
    int mode;

    resources_get_int("SoundOutput", &mode);

    layout = uihelpers_create_int_radiogroup_with_label(
            "Sound output mode",
            output_modes,
            on_output_mode_changed,
            mode);

    return layout;
}
