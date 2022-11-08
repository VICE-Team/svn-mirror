/** \file   vsidcontrolwidget.c
 * \brief   GTK3 control widget for VSID
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * Icons used by this file:
 *
 * $VICEICON    actions/media-skip-backward
 * $VICEICON    actions/media-playback-start
 * $VICEICON    actions/media-playback-pause
 * $VICEICON    actions/media-playback-stop
 * $VICEICON    actions/media-seek-forward
 * $VICEICON    actions/media-skip-forward
 * $VICEICON    actions/media-eject
 * $VICEICON    actions/media-record
 *
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

#include "c64mem.h"
#include "debug.h"
#include "machine.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "psid.h"
#include "ui.h"
#include "uiactions.h"
#include "vice_gtk3.h"
#include "vsidstate.h"
#include "vsidtuneinfowidget.h"
#include "vsync.h"

#include "vsidcontrolwidget.h"


/** \brief  Emulation speed during fast forward
 */
#define FFWD_SPEED  500


/** \brief  Object containing icon, action ID and tooltip
 */
typedef struct vsid_ctrl_button_s {
    const char *icon_name;  /**< icon name */
    int action;             /**< UI action ID */
    const char *tooltip;    /**< tool tip */
} vsid_ctrl_button_t;



/** \brief  Progress bar */
static GtkWidget *progress = NULL;

/** \brief  Repeat toggle button */
static GtkWidget *repeat = NULL;


/** \brief  Event handler for buttons
 *
 * Trigger UI \a action.
 *
 * \param[in]   widget  button
 * \param[in]   action  action ID
 */
static void action_callback(GtkWidget *widget, gpointer action)
{
    int id = GPOINTER_TO_INT(action);

    if (id > 0) {
        debug_gtk3("calling action '%s' (%d).", ui_action_get_name(id), id);
        ui_action_trigger(id);
    }
}


/** \brief  List of media control buttons
 *
 * \todo    Some of these buttons would be better as stateful buttons, ie
 *          GtkToggleButtons.
 */
static const vsid_ctrl_button_t buttons[] = {
    { "media-skip-backward",
      ACTION_PSID_SUBTUNE_PREVIOUS,
      "Select previous subtune" },
    { "media-playback-start",
      ACTION_PSID_PLAY,
      "Play tune" },
    { "media-playback-pause",
      ACTION_PSID_PAUSE,
      "Pause playback" },
    { "media-playback-stop",
      ACTION_PSID_STOP,
      "Stop playback" },
    { "media-seek-forward",
      ACTION_PSID_FFWD,
      "Fast forward" },
    { "media-skip-forward",
      ACTION_PSID_SUBTUNE_NEXT,
      "Select next subtune", },   /* select next tune */
    { "media-eject",
      ACTION_PSID_LOAD,
      "Load PSID file" },   /* active file-open dialog */
#if 0
    { "media-record", fake_callback,
        "Record media" },  /* start recording with current settings*/
#endif
    { NULL, 0, NULL }
};



/** \brief  Create widget with media buttons to control VSID playback
 *
 * \return  GtkGrid
 */
GtkWidget *vsid_control_widget_create(void)
{
    GtkWidget *grid;
    int i;

    grid = vice_gtk3_grid_new_spaced(0, VICE_GTK3_DEFAULT);

    for (i = 0; buttons[i].icon_name != NULL; i++) {
        GtkWidget *button;
        gchar buf[1024];

        g_snprintf(buf, sizeof(buf), "%s-symbolic", buttons[i].icon_name);

        button = gtk_button_new_from_icon_name(buf, GTK_ICON_SIZE_LARGE_TOOLBAR);
        /* always show the image, the button would useless without an image */
        gtk_button_set_always_show_image(GTK_BUTTON(button), TRUE);
        /* don't initialy focus on a button */
        gtk_widget_set_can_focus(button, FALSE);
#if 0
        /* don't grab focus when clicked */
        gtk_widget_set_focus_on_click(button, FALSE);
#endif
        gtk_grid_attach(GTK_GRID(grid), button, i, 0, 1,1);
        g_signal_connect(button,
                         "clicked",
                         G_CALLBACK(action_callback),
                         GINT_TO_POINTER(buttons[i].action));
        if (buttons[i].tooltip != NULL) {
            gtk_widget_set_tooltip_text(button, buttons[i].tooltip);
        }
    }

    /* add progress bar */
    progress = gtk_progress_bar_new();
    gtk_grid_attach(GTK_GRID(grid), progress, 0, 1, i, 1);

    /* Add loop check button
     *
     * I'm pretty sure there's a loop icon, so perhaps add that to the control
     * buttons in stead of using this check button.
     */
    repeat = gtk_check_button_new_with_label("Loop current song");
    gtk_grid_attach(GTK_GRID(grid), repeat, 0, 2, i, 1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(repeat), TRUE);
    gtk_widget_set_can_focus(repeat, FALSE);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set number of tunes
 *
 * \param[in]   n   tune count
 */
void vsid_control_widget_set_tune_count(int n)
{
    vsid_state_t *state = vsid_state_lock();

    state->tune_count = n;
    vsid_state_unlock();
}


/** \brief  Set current tune
 *
 * \param[in]   n   tune number
 */
void vsid_control_widget_set_tune_current(int n)
{
    vsid_state_t *state = vsid_state_lock();

    state->tune_current = n;
    vsid_state_unlock();
}


/** \brief  Set default tune
 *
 * \param[in]   n   tune number
 */
void vsid_control_widget_set_tune_default(int n)
{
    vsid_state_t *state = vsid_state_lock();

    state->tune_default = n;
    vsid_state_unlock();
}


/** \brief  Set tune progress bar value
 *
 * \param[in]   fraction    amount to fill (0.0 - 1.0)
 */
void vsid_control_widget_set_progress(gdouble fraction)
{
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), fraction);
}


/** \brief  Play next tune
 */
void vsid_control_widget_next_tune(void)
{
    ui_action_trigger(ACTION_PSID_SUBTUNE_NEXT);
}


/** \brief  Get repeat/loop widget state
 *
 * \return  loop state
 */
gboolean vsid_control_widget_get_repeat(void)
{
    return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(repeat));
}
