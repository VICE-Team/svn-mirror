/*
 * soundsampleratewidget.c - GTK3 sound sample rate widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  SoundSampleRate - sample rate in Hertz (8000-48000) (int)
 *
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

#include "soundsampleratewidget.h"


/** \brief  List of sound sampling rates
 */
static ui_text_int_pair_t sample_rates[] = {
    { "8000 Hz", 8000 },
    { "11025 Hz", 11025 },
    { "22050 Hz", 22050 },
    { "44100 Hz", 44100 },
    { "48000 Hz", 48000 },
    { NULL, -1 }
};


/** \brief  Event handler to alter the "SoundSampleRate" resource
 *
 * \param[in]   widget      widget triggering the callback
 * \param[in]   user_data   the SoundSampleRate setting (`int`)
 *
 */
static void on_sample_rate_changed(GtkWidget *widget, gpointer user_data)
{
    int rate = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        debug_gtk3("got sound sample rate %d Hz\n", rate);
        resources_set_int("SoundSampleRate", rate);
    }
}


/** \brief  Create widget for "Sound sample rate"
 *
 * A simple list of radio buttons for sound sample rates (8000-48000Hz)
 *
 * \return  grid
 */
GtkWidget *create_sound_sample_rate_widget(void)
{
    GtkWidget *layout;
    int rate;
    int i;

    /* turn rate into radio button index */
    resources_get_int("SoundSampleRate", &rate);
    for (i = 0; sample_rates[i].text != NULL; i++) {
        if (sample_rates[i].value == rate) {
            break;
        }
    }
    /* guard against invalid index */
    if (sample_rates[i].text == NULL) {
        i = 0;
    }

    layout = uihelpers_create_int_radiogroup_with_label(
            "Sound sample rate",
            sample_rates,
            on_sample_rate_changed,
            i);

    return layout;
}
