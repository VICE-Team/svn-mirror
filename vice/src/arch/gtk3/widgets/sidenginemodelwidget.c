/** \file   sidenginemodelwidget.c
 * \brief   Widget to select SID engine and model
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES SidModel    all
 *  (all = if a SidCart is installed)
 * $VICERES SidEngine   all
 *  (all = if a SidCart is installed)
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
#include <gtk/gtk.h>

#include "resources.h"
#include "sid.h"
#include "vice_gtk3.h"

#include "sidenginemodelwidget.h"



/** \brief  Optional extra callback for the SID settings glue logic
 */
static void (*extra_callback)(int, int) = NULL;


/** \brief  Handler for the 'toggled' event of the SID engine/model radio buttons
 *
 * \param[in]   radio   radio button triggering the event
 * \param[in]   data    new value for the SID engine/model
 */
static void on_radio_toggled(GtkWidget *radio, gpointer data)
{
    unsigned int engine;
    unsigned int model;

    int current_engine;
    int current_model;

    /* Do not update engine/model when deactivating a radio button, stupid
     * compyx!
     */
    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio))) {
        return;
    }

    resources_get_int("SidEngine", &current_engine);
    resources_get_int("SidModel",  &current_model);

    /* bit 15-8: engine */
    engine = (GPOINTER_TO_INT(data) >> 8) & 0xff;
    /* bit 7-0: model */
    model = GPOINTER_TO_INT(data) & 0xff;

    if ((int)model != current_model || (int)engine != current_engine) {
        /*resources_set_int("SidModel", (int)model);
        resources_set_int("SidEngine", (int)engine);
        */
        sid_set_engine_model(engine, model);

        /* user-defined callback? */
        if (extra_callback != NULL) {
            extra_callback(engine, model);
        }
    }
}


/** \brief  Create SID engine/model selection widget
 *
 * \return  GtkGrid
 */
GtkWidget *sid_engine_model_widget_create(void)
{
    GtkWidget           *grid;
    GtkWidget           *last = NULL;
    GSList              *group = NULL;
    sid_engine_model_t **list;
    int                  model = 0;
    int                  engine = 0;
    unsigned int         current;
    int                  i;

    resources_get_int("SidEngine", &engine);
    resources_get_int("SidModel",  &model);
    current = (unsigned int)((engine << 8) | model);

#if defined(HAVE_RESID) && defined(HAVE_FASTSID)
# define GRID_TITLE "SID engine and model"
#elif defined(HAVE_RESID)
# define GRID_TITLE "ReSID model"
#elif defined(HAVE_FASTSID)
# define GRID_TITLE "FastSID model"
#else
# define GRID_TITLE "I am a BUG!"
#endif
    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, GRID_TITLE, 1);
#undef GRID_TITLE
    vice_gtk3_grid_set_title_margin(grid, 8);

    list = sid_get_engine_model_list();
    for (i = 0; list[i] != NULL; i++) {
        GtkWidget *radio;

        radio = gtk_radio_button_new_with_label(group, list[i]->name);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio),
                                    GTK_RADIO_BUTTON(last));
        gtk_widget_set_margin_start(radio, 8);
        if (list[i]->value == current) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
        }
        g_signal_connect(radio,
                         "toggled",
                         G_CALLBACK(on_radio_toggled),
                         GINT_TO_POINTER(list[i]->value));
        gtk_grid_attach(GTK_GRID(grid), radio, 0, i + 1, 1, 1);
        last = radio;
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set callback to be triggered on SID model or engine changes
 *
 * The callback function will receive (engine, model).
 *
 * \param[in]   callback    function to call on model/engine change
 */
void sid_engine_model_widget_set_callback(void (*callback)(int, int))
{
    extra_callback = callback;
}
