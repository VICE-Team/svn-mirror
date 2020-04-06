/** \file   ui.c
 * \brief   Native GTK3 UI stuff
 *
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  Marcus Sutton <loggedoubt@gmail.com>
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
#include <string.h>
#include <gtk/gtk.h>

#ifdef UNIX_COMPILE
#include <unistd.h>
#endif

#ifdef MACOSX_SUPPORT
#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreFoundation/CFString.h>
#endif

#include "debug_gtk3.h"

#include "archdep.h"

#include "autostart.h"
#include "cmdline.h"
#include "drive.h"
#include "interrupt.h"
#include "kbd.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "lightpen.h"
#include "resources.h"
#include "util.h"
#include "videoarch.h"
#include "vsync.h"
#include "vsyncapi.h"

#include "basedialogs.h"
#include "uiapi.h"
#include "uicommands.h"
#include "uimenu.h"
#include "uisettings.h"
#include "uistatusbar.h"
#include "jamdialog.h"
#include "uicart.h"
#include "uidiskattach.h"
#include "uismartattach.h"
#include "uitapeattach.h"
#include "uimachinewindow.h"
#include "uimedia.h"
#include "mixerwidget.h"
#include "uidata.h"
#include "archdep.h"

/* for the fullscreen_capability() stub */
#include "fullscreen.h"

#include "ui.h"


/* Forward declarations of static functions */

static int set_save_resources_on_exit(int val, void *param);
static int set_confirm_on_exit(int val, void *param);
static int set_window_height(int val, void *param);
static int set_window_width(int val, void *param);
static int set_window_xpos(int val, void *param);
static int set_window_ypos(int val, void *param);
static int set_start_minimized(int val, void *param);
static int set_native_monitor(int val, void *param);
static int set_fullscreen_state(int val, void *param);
static void ui_toggle_warp(void);



/*****************************************************************************
 *                  Defines, enums, type declarations                        *
 ****************************************************************************/


/** \brief  List of drag targets for the drag-n-drop event handler
 *
 * It would appear different OS'es/WM's pass dropped files using various
 * mime-types.
 */
GtkTargetEntry ui_drag_targets[UI_DRAG_TARGETS_COUNT] = {
    { "text/plain",     0, DT_TEXT },   /* we get this on at least my Linux
                                           box with Mate */
    { "text/uri",       0, DT_URI },
    { "text/uri-list",  0, DT_URI_LIST }    /* we get this using Windows
                                               Explorer or macOS Finder */
};


/** \brief  Struct holding basic UI rescources
 */
typedef struct ui_resources_s {

    int save_resources_on_exit; /**< SaveResourcesOnExit (bool) */
    int confirm_on_exit;        /**< ConfirmOnExit (bool) */

    int start_minimized;        /**< StartMinimized (bool) */

    int use_native_monitor;     /**< NativeMonitor (bool) */

#if 0
    int depth;
#endif

    video_canvas_t *canvas[NUM_WINDOWS];
    GtkWidget *window_widget[NUM_WINDOWS]; /**< the toplevel GtkWidget (Window) */
    int window_width[NUM_WINDOWS];
    int window_height[NUM_WINDOWS];
    int window_xpos[NUM_WINDOWS];
    int window_ypos[NUM_WINDOWS];

} ui_resource_t;


/** \brief  Collection of UI resources
 *
 * This needs to stay here, to allow the command line and resources initializers
 * to reference the UI resources.
 */
static ui_resource_t ui_resources;

/** \brief  Fullscreen state
 */
static int fullscreen_enabled = 0;


/** \brief  Row numbers of the various widgets packed in a main GtkWindow
 */
enum {
    ROW_MENU_BAR = 0,   /**< application menu bar */
    ROW_DISPLAY,        /**< emulated display */
    ROW_STATUS_BAR,     /**< status bar */
    ROW_CRT_CONTROLS,   /**< CRT control widgets */
    ROW_MIXER_CONTROLS  /**< mixer control widgets */
};


/** \brief  Default hotkeys for the UI not connected to a menu item
 */
static kbd_gtk3_hotkey_t default_hotkeys[] = {
    /* Alt+P: toggle pause */
    { GDK_KEY_p, VICE_MOD_MASK, (void *)ui_toggle_pause },
    /* Alt+W: toggle warp mode */
    { GDK_KEY_w, VICE_MOD_MASK, ui_toggle_warp },
    /* Alt+Shift+P: Advance frame (only when paused)
     *
     * XXX: seems GDK_KEY_*P* is required here, otherwise the key press isn't
     *      recognized (only tested on Win10)
     */
    { GDK_KEY_P, VICE_MOD_MASK|GDK_SHIFT_MASK, (void *)ui_advance_frame },

    { GDK_KEY_F12, VICE_MOD_MASK|GDK_SHIFT_MASK, uimedia_auto_screenshot },

    /* Alt+J = swap joysticks */
    { GDK_KEY_j, VICE_MOD_MASK,
        (void *)ui_swap_joysticks_callback },
    /* Alt+Shift+U = swap userport joysticks */
    { GDK_KEY_u, VICE_MOD_MASK|GDK_SHIFT_MASK,
        (void *)ui_swap_userport_joysticks_callback },
    { GDK_KEY_J, VICE_MOD_MASK|GDK_SHIFT_MASK,
        (void *)ui_toggle_keyset_joysticks },
    { GDK_KEY_m, VICE_MOD_MASK,
        (void *)ui_toggle_mouse_grab },

    /* Arnie */
    { 0, 0, NULL }
};




/*****************************************************************************
 *                              Static data                                  *
 ****************************************************************************/

/** \brief  String type resources list
 */
#if 0
static const resource_string_t resources_string[] = {
    RESOURCE_STRING_LIST_END
};
#endif


/** \brief  Boolean resources shared between windows
 */
static const resource_int_t resources_int_shared[] = {
    { "SaveResourcesOnExit", 0, RES_EVENT_NO, NULL,
        &ui_resources.save_resources_on_exit, set_save_resources_on_exit, NULL },
    { "ConfirmOnExit", 1, RES_EVENT_NO, NULL,
        &ui_resources.confirm_on_exit, set_confirm_on_exit, NULL },

    { "StartMinimized", 0, RES_EVENT_NO, NULL,
        &ui_resources.start_minimized, set_start_minimized, NULL },

    { "NativeMonitor", 0, RES_EVENT_NO, NULL,
        &ui_resources.use_native_monitor, set_native_monitor, NULL },

    { "FullscreenEnable", 0, RES_EVENT_NO, NULL,
        &fullscreen_enabled, set_fullscreen_state, NULL },


    RESOURCE_INT_LIST_END
};


/** \brief  Window size and position resources for the primary window
 *
 * These are used by all emulators.
 */
static const resource_int_t resources_int_primary_window[] = {
    { "Window0Height", 0, RES_EVENT_NO, NULL,
        &(ui_resources.window_height[PRIMARY_WINDOW]), set_window_height,
        (void*)PRIMARY_WINDOW },
    { "Window0Width", 0, RES_EVENT_NO, NULL,
        &(ui_resources.window_width[PRIMARY_WINDOW]), set_window_width,
        (void*)PRIMARY_WINDOW },
    { "Window0Xpos", 0, RES_EVENT_NO, NULL,
        &(ui_resources.window_xpos[PRIMARY_WINDOW]), set_window_xpos,
        (void*)PRIMARY_WINDOW },
    { "Window0Ypos", 0, RES_EVENT_NO, NULL,
        &(ui_resources.window_ypos[PRIMARY_WINDOW]), set_window_ypos,
        (void*)PRIMARY_WINDOW },

    RESOURCE_INT_LIST_END
};


/** \brief  Window size and position resources list for the secondary window
 *
 * These are only used by x128's VDC window.
 */
static const resource_int_t resources_int_secondary_window[] = {
    { "Window1Height", 0, RES_EVENT_NO, NULL,
        &(ui_resources.window_height[SECONDARY_WINDOW]), set_window_height,
        (void*)SECONDARY_WINDOW },
    { "Window1Width", 0, RES_EVENT_NO, NULL,
        &(ui_resources.window_width[SECONDARY_WINDOW]), set_window_width,
        (void*)SECONDARY_WINDOW },
    { "Window1Xpos", 0, RES_EVENT_NO, NULL,
        &(ui_resources.window_xpos[SECONDARY_WINDOW]), set_window_xpos,
        (void*)SECONDARY_WINDOW },
    { "Window1Ypos", 0, RES_EVENT_NO, NULL,
        &(ui_resources.window_ypos[SECONDARY_WINDOW]), set_window_ypos,
        (void*)SECONDARY_WINDOW },

    RESOURCE_INT_LIST_END
};


/** \brief  Command line options shared between emu's, include VSID
 */
static const cmdline_option_t cmdline_options_common[] =
{
    { "-confirmonexit", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "ConfirmOnExit", (void *)1,
        NULL, "Confirm quitting VICE" },
    { "+confirmonexit", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "ConfirmOnExit", (void *)0,
        NULL, "Do not confirm quitting VICE" },
    { "-saveres", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "SaveResourcesOnExit", (void *)1,
        NULL, "Save settings on exit" },
    { "+saveres", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "SaveResourcesOnExit", (void *)0,
        NULL, "Do not save settings on exit" },
    { "-minimized", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "StartMinimized", (void *)1,
        NULL, "Start VICE minimized" },
    { "+minimized", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "StartMinimized", (void *)0,
        NULL, "Do not start VICE minimized" },
    { "-nativemonitor", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "NativeMonitor", (void *)1,
        NULL, "Use native monitor on OS terminal" },
    { "+nativemonitor", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "NativeMonitor", (void *)0,
        NULL, "Use VICE Gtk3 monitor terminal" },
    { "-fullscreen", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "FullscreenEnable", (void*)1,
        NULL, "Enable fullscreen" },
    { "+fullscreen", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "FullscreenEnable", (void*)0,
        NULL, "Disable fullscreen" },

    CMDLINE_LIST_END
};


/** \brief  Flag indicating pause mode
 */
static int is_paused = 0;

/** \brief  Index of the most recently focused main window
 */
static int active_win_index = -1;

/** \brief  Flag indicating whether we're supposed to be in fullscreen
 */
static int is_fullscreen = 0;

/** \brief  Flag inidicating whether fullscreen mode shows the decorations
 */
static int fullscreen_has_decorations = 0;

/** \brief  Function to handle files dropped on a main window
 */
static int (*handle_dropped_files_func)(const char *) = NULL;

/** \brief  Function to help create a main window
 */
static void (*create_window_func)(video_canvas_t *) = NULL;

/** \brief  Function to identify a canvas from its video chip
 */
static int (*identify_canvas_func)(video_canvas_t *) = NULL;

/** \brief  Function to help create a CRT controls widget
 */
static GtkWidget *(*create_controls_widget_func)(int) = NULL;


/******************************************************************************
 *                              Event handlers                                *
 *****************************************************************************/


/** \brief  Handler for the 'drag-drop' event of the GtkWindow(s)
 *
 * Can be used to filter certain drop targets or altering the data before
 * triggering the 'drag-drop-received' event. Currently just returns TRUE
 *
 * \param[in]   widget  widget triggering the event
 * \param[in]   context gtk drag context
 * \param[in]   x       x position of drag event
 * \param[in]   y       y position of drag event
 * \param[in]   time    (I don't have a clue)
 * \param[in]   data    extra event data (unused)
 *
 * \return  TRUE
 */
static gboolean ui_on_drag_drop(
        GtkWidget *widget,
        GdkDragContext *context,
        gint x,
        gint y,
        guint time,
        gpointer data)
{
    debug_gtk3("called.");
    return TRUE;
}


/** \brief  Handler for the 'drag-data-received' event
 *
 * Autostarts an image/prg when valid. Please note that VSID now has its own
 * drag-n-drop handlers.
 *
 * \param[in]   widget      widget triggering the event (unused)
 * \param[in]   context     drag context (unused)
 * \param[in]   x           probably X-coordinate in the drop target?
 * \param[in]   y           probablt Y-coordinate in the drop target?
 * \param[in]   data        dragged data
 * \param[in]   info        int declared in the targets array (unclear)
 * \param[in]   time        no idea
 *
 * \todo    Once this works properly, remove a lot of debugging calls, perhaps
 *          changing a few into log calls.
 */
static void ui_on_drag_data_received(
        GtkWidget *widget,
        GdkDragContext *context,
        int x,
        int y,
        GtkSelectionData *data,
        guint info,
        guint time)
{
    gchar **uris;
    gchar *filename = NULL;
    gchar **files = NULL;
    guchar *text = NULL;
    int i;

    debug_gtk3("got drag-data, info = %u:", info);

    switch (info) {

        case DT_URI_LIST:
            /*
             * This branch appears to be taken on both Windows and macOS.
             */

            /* got possible list of URI's */
            uris = gtk_selection_data_get_uris(data);
            if (uris != NULL) {
                /* dump URI's on stdout */
                debug_gtk3("got URI's:");
                for (i = 0; uris[i] != NULL; i++) {

                    debug_gtk3("URI: '%s'\n", uris[i]);
                    filename = g_filename_from_uri(uris[i], NULL, NULL);
                    debug_gtk3("filename: '%s'.", filename);
                    if (filename != NULL) {
                        g_free(filename);
                    }
                }

                /* use the first/only entry as the autostart file
                 *
                 * XXX: perhaps add any additional files to the fliplist
                 *      if Dxx?
                 */
                if (uris[0] != NULL) {
                    filename = g_filename_from_uri(uris[0], NULL, NULL);
                } else {
                    filename = NULL;
                }

                g_strfreev(uris);
            }
            break;

        case DT_TEXT:
            /*
             * this branch appears to be taken on both Gtk and Qt based WM's
             * on Linux
             */


            /* text will contain a newline separated list of 'file://' URIs,
             * and a trailing newline */
            text = gtk_selection_data_get_text(data);
            /* remove trailing whitespace */
            g_strchomp((gchar *)text);

            debug_gtk3("Got data as text: '%s'.", text);
            files = g_strsplit((const gchar *)text, "\n", -1);
            g_free(text);

            for (i = 0; files[i] != NULL; i++) {
#ifdef HAVE_DEBUG_GTK3UI
                gchar *tmp = g_filename_from_uri(files[i], NULL, NULL);
#endif
                debug_gtk3("URI: '%s', filename: '%s'.",
                        files[i], tmp);
            }
            /* now grab the first file */
            filename = g_filename_from_uri(files[0], NULL, NULL);
            g_strfreev(files);

            debug_gtk3("got filename '%s'.", filename);
            break;

        default:
            debug_gtk3("Warning: unhandled d'n'd target %u.", info);
            filename = NULL;
            break;
    }

    /* can we attempt autostart? */
    if (filename != NULL) {
        debug_gtk3("Attempting to autostart '%s'.", filename);
        if (autostart_autodetect(filename, NULL, 0, AUTOSTART_MODE_RUN) != 0) {
            debug_gtk3("failed.");
        } else {
            debug_gtk3("OK!");
        }
        g_free(filename);
    }
}


/** \brief  Set fullscreen state \a val
 *
 * \param[in]   val     fullscreen state (boolean)
 * \param[in]   param   extra argument (unused(
 *
 * \return 0
 */
static int set_fullscreen_state(int val, void *param)
{
    debug_gtk3("called with %d.", val);
    fullscreen_enabled = val;
    return 0;
}



/** \brief  Get the most recently focused toplevel window
 *
 * \return  pointer to a toplevel window, or NULL
 *
 * \note    Not an event handler, needs to be moved
 */
GtkWindow *ui_get_active_window(void)
{
    GtkWindow *window = NULL;
    GList *tlist = gtk_window_list_toplevels();
    GList *list = tlist;

    /* Find the window that has the toplevel focus. */
    while (list != NULL) {
        if (gtk_window_has_toplevel_focus(list->data)) {
            window = list->data;
            break;
        }
        list = list->next;
    }
    g_list_free(tlist);

    /* If no window has the toplevel focus, then fall back
     * to the most recently focused main window.
     */
    if (window == NULL
            && active_win_index >= 0 && active_win_index < NUM_WINDOWS) {
        window = GTK_WINDOW(ui_resources.window_widget[active_win_index]);
    }

    /* If "window" still is NULL, it probably means
     * that no windows have been created yet.
     */
    return window;
}


/** \brief  Get video canvas of active window
 *
 * \return  current active video canvas, or NULL
 */
video_canvas_t *ui_get_active_canvas(void)
{
    if (active_win_index < 0) {
        /* If we end up here it probably means no main window has
         * been created yet. */
        return NULL;
    }
    return ui_resources.canvas[active_win_index];
}


/** \brief  Get the active main window's index
 *
 * \return  index of a main emulator window
 */
int ui_get_main_window_index(void)
{
    return active_win_index;
}


/** \brief  Get a window's index
 *
 * \param[in]   widget      window to get the index of
 *
 * \return  window index, or -1 if not a main window
 */
int ui_get_window_index(GtkWidget *widget)
{
    if (widget == NULL) {
        return -1;
    } else if (widget == ui_resources.window_widget[PRIMARY_WINDOW]) {
        return PRIMARY_WINDOW;
    } else if (widget == ui_resources.window_widget[SECONDARY_WINDOW]) {
        return SECONDARY_WINDOW;
    } else {
        return -1;
    }
}

/** \brief  Handler for the "focus-in-event" of a main window
 *
 * \param[in]   widget      window triggering the event
 * \param[in]   event       window focus details
 * \param[in]   user_data   extra data for the event (ignored)
 *
 * \return  FALSE to continue processing
 *
 * \note    We only use this for canvas-window-specific stuff like
 *          fullscreen mode.
 */
static gboolean on_focus_in_event(GtkWidget *widget, GdkEventFocus *event,
                                  gpointer user_data)
{
    int index = ui_get_window_index(widget);

    /* printf("ui.c:on_focus_in_event\n"); */

    ui_mouse_grab_pointer();

    if (index < 0) {
        /* We should never end up here. */
        log_error(LOG_ERR, "focus-in-event: window not found\n");
        archdep_vice_exit(1);
    }

    if (event->in == TRUE) {
        /* fprintf(stderr, "window %d: focus-in\n", index); */
        active_win_index = index;
    }

    return FALSE;
}

/** \brief  Handler for the "focus-out-event" of a main window
 *
 * \param[in]   widget      window triggering the event
 * \param[in]   event       window focus details
 * \param[in]   user_data   extra data for the event (ignored)
 *
 * \return  FALSE to continue processing
 *
 * \note    We only use this for canvas-window-specific stuff like
 *          fullscreen mode.
 */
static gboolean on_focus_out_event(GtkWidget *widget, GdkEventFocus *event,
                                  gpointer user_data)
{
    /* printf("ui.c:on_focus_out_event\n"); */

    ui_mouse_ungrab_pointer();

    return FALSE;
}

/** \brief  Create an icon by loading it from the vice.gresource file
 *
 * \return  Standard C= icon ripped from the internet (but at least scalable)
 *          Which of course looks weird on Windows for some reason, *sigh*.
 */
static GdkPixbuf *get_default_icon(void)
{
    return uidata_get_pixbuf("CBM_Logo.svg");
}


/** \brief Show or hide the decorations of the active main window as needed
 */
static void ui_update_fullscreen_decorations(void)
{
    GtkWidget *window, *grid, *menu_bar, *crt_grid, *mixer_grid, *status_bar;
    int has_decorations;

    /* FIXME: this function does not work properly for vsid and should never
     * get called by it, but at least on Macs it can get called if the user
     * clicks the fullscreen button in the main vsid window.
     */
    if (active_win_index < 0 || machine_class == VICE_MACHINE_VSID) {
        return;
    }

    has_decorations = (!is_fullscreen) || fullscreen_has_decorations;
    window = ui_resources.window_widget[active_win_index];
    grid = gtk_bin_get_child(GTK_BIN(window));
    menu_bar = gtk_grid_get_child_at(GTK_GRID(grid), 0, ROW_MENU_BAR);
    crt_grid = gtk_grid_get_child_at(GTK_GRID(grid), 0, ROW_CRT_CONTROLS);
    mixer_grid = gtk_grid_get_child_at(GTK_GRID(grid), 0, ROW_MIXER_CONTROLS);
    status_bar = gtk_grid_get_child_at(GTK_GRID(grid), 0, ROW_STATUS_BAR);

    if (has_decorations) {
        gtk_widget_show(menu_bar);
        if (ui_statusbar_crt_controls_enabled(window)) {
            gtk_widget_show(crt_grid);
        }
        if (ui_statusbar_mixer_controls_enabled(window)) {
            gtk_widget_show(mixer_grid);
        }
        gtk_widget_show(status_bar);
    } else {
        gtk_widget_hide(menu_bar);
        gtk_widget_hide(crt_grid);
        gtk_widget_hide(mixer_grid);
        gtk_widget_hide(status_bar);
    }
}

/** \brief  Handler for the "window-state-event" of a main window
 *
 * \param[in]   widget      window triggering the event
 * \param[in]   event       window state details
 * \param[in]   user_data   extra data for the event (ignored)
 *
 * \return  FALSE to continue processing
 */
static gboolean on_window_state_event(GtkWidget *widget,
                                      GdkEventWindowState *event,
                                      gpointer user_data)
{
    GdkWindowState win_state = event->new_window_state;
    int index = ui_get_window_index(widget);

    if (index < 0) {
        /* We should never end up here. */
        log_error(LOG_ERR, "window-state-event: window not found\n");
        archdep_vice_exit(1);
    }

    if (win_state & GDK_WINDOW_STATE_FULLSCREEN) {
        if (!is_fullscreen) {
            is_fullscreen = 1;
            ui_update_fullscreen_decorations();
        }
    } else {
        if (is_fullscreen) {
            is_fullscreen = 0;
            ui_update_fullscreen_decorations();
        }
    }

    return FALSE;
}



/** \brief  Stub to satisfy the various $videochip-resources.c files
 *
 * \param[in]   cap_fullscreen  unused
 */
void fullscreen_capability(struct cap_fullscreen_s *cap_fullscreen)
{
    /*
     * A NOP for the Gtk3 UI, since we don't support custom fullscreen modes.
     */
    return;
}



/** \brief  Checks if we're in fullscreen mode
 *
 * \return  nonzero if we're in fullscreen mode
 */
int ui_is_fullscreen(void)
{
    return is_fullscreen;
}

/** \brief  Updates UI in response to the simulated machine screen
 *          changing its dimensions or aspect ratio
 */
void ui_trigger_resize(void)
{
    int i;
    for (i = 0; i < NUM_WINDOWS; ++i) {
        if (ui_resources.canvas[i]) {
            video_canvas_adjust_aspect_ratio(ui_resources.canvas[i]);
        }
        if (ui_resources.window_widget[i]) {
            gtk_widget_queue_resize(ui_resources.window_widget[i]);
        }
    }
}

/** \brief  Toggles fullscreen mode in reaction to user request
 *
 * \param[in]   widget      the widget that sent the callback (ignored)
 * \param[in]   user_data   extra data for the callback (ignored)
 *
 * \return  TRUE
 */
gboolean ui_fullscreen_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWindow *window;

    if (active_win_index < 0) {
        return FALSE;
    }

    window = GTK_WINDOW(ui_resources.window_widget[active_win_index]);
    is_fullscreen = !is_fullscreen;

    if (is_fullscreen) {
        gtk_window_fullscreen(window);
    } else {
        gtk_window_unfullscreen(window);
    }

    ui_update_fullscreen_decorations();
    return TRUE;
}

/** \brief Toggles fullscreen window decorations in response to user request
 *
 * \param[in]   widget      the widget that sent the callback (ignored)
 * \param[in]   user_data   extra data for the callback (ignored)
 *
 * \return  TRUE
 */
gboolean ui_fullscreen_decorations_callback(GtkWidget *widget, gpointer user_data)
{
    fullscreen_has_decorations = !fullscreen_has_decorations;
    ui_update_fullscreen_decorations();
    return TRUE;
}


/** \brief  Get a window-spec array index from \a param
 *
 * Also performs a bounds check and returns -1 on boundary violation.
 *
 * \param[in]   param   extra param passed to a setter
 *
 * \return  index in array or -1 on error
 */
static int window_index_from_param(void *param)
{
    int index = vice_ptr_to_int(param);
    return (index >= 0 && index < NUM_WINDOWS) ? index : -1;
}


/*
 * Resource getters/setters
 */


/** \brief  Set SaveResourcesOnExit resource
 *
 * \param[in]   val     new value
 * \param[in]   param   extra param (ignored)
 *
 * \return 0
 */
static int set_save_resources_on_exit(int val, void *param)
{
    ui_resources.save_resources_on_exit = val ? 1 : 0;
    return 0;
}


/** \brief  Set ConfirmOnExit resource (bool)
 *
 * \param[in]   val     new value
 * \param[in]   param   extra param (ignored)
 *
 * \return 0
 */
static int set_confirm_on_exit(int val, void *param)
{
    ui_resources.confirm_on_exit = val ? 1 : 0;
    return 0;
}


/** \brief  Set StartMinimized resource (bool)
 *
 * \param[in]   val     0: start normal 1: start minimized
 * \param[in]   param   extra param (ignored)
 *
 * \return 0
 */
static int set_start_minimized(int val, void *param)
{
    ui_resources.start_minimized = val ? 1 : 0;
    return 0;
}


/** \brief  Set NativeMonitor resource (bool)
 *
 * \param[in]   val     new value
 * \param[in]   param   extra param (ignored)
 *
 * \return 0
 */
static int set_native_monitor(int val, void *param)
{
    /* FIXME: setting this to 1 should probably fail if either stdin or stdout
              is not a terminal. */
#if 0
    if (!isatty(stdin) || !isatty(stdout)) {
        return -1;
    }
#endif
    ui_resources.use_native_monitor = val ? 1 : 0;
    return 0;
}



/** \brief  Set Window[X]Width resource (int)
 *
 * \param[in]   val     width in pixels
 * \param[in]   param   window index
 *
 * \return 0
 */
static int set_window_width(int val, void *param)
{
    int index = window_index_from_param(param);
    if (index < 0 || val < 0) {
        return -1;
    }
    ui_resources.window_width[index] = val;
    return 0;
}


/** \brief  Set Window[X]Height resource (int)
 *
 * \param[in]   val     height in pixels
 * \param[in]   param   window index
 *
 * \return 0
 */
static int set_window_height(int val, void *param)
{
    int index = window_index_from_param(param);
    if (index < 0 || val < 0) {
        return -1;
    }
    ui_resources.window_height[index] = val;
    return 0;
}


/** \brief  Set Window[X]Xpos resource (int)
 *
 * \param[in]   val     x-pos in pixels
 * \param[in]   param   window index
 *
 * \return 0
 */
static int set_window_xpos(int val, void *param)
{
    int index = window_index_from_param(param);
    if (index < 0 || val < 0) {
        return -1;
    }
    ui_resources.window_xpos[index] = val;
    return 0;
}


/** \brief  Set Window[X]Ypos resource (int)
 *
 * \param[in]   val     y-pos in pixels
 * \param[in]   param   window index
 *
 * \return 0
 */
static int set_window_ypos(int val, void *param)
{
    int index = window_index_from_param(param);
    if (index < 0 || val < 0) {
        return -1;
    }
    ui_resources.window_ypos[index] = val;
    return 0;
}


/*
 * Function pointer setters
 */


/** \brief  Set function to handle files dropped on a main window
 *
 * \param[in]   func    handle dropped files function
 */
void ui_set_handle_dropped_files_func(int (*func)(const char *))
{
    handle_dropped_files_func = func;
}


/** \brief  Set function to help create the main window(s)
 *
 * \param[in]   func    create window function
 */
void ui_set_create_window_func(void (*func)(video_canvas_t *))
{
    create_window_func = func;
}


/** \brief  Set function to identify a canvas from its video chip
 *
 * \param[in]   func    identify canvas function
 */
void ui_set_identify_canvas_func(int (*func)(video_canvas_t *))
{
    identify_canvas_func = func;
}


/** \brief  Set function to help create the CRT controls widget(s)
 *
 * \param[in]   func    create CRT controls widget function
 */
void ui_set_create_controls_widget_func(GtkWidget *(*func)(int))
{
    create_controls_widget_func = func;
}


/** \brief  Handler for the "destroy" event of \a widget
 *
 * Looks like debug code. But better keep it here to debug the warnings about
 * GtkEventBox'es getting destroyed prematurely.
 *
 * \param[in]   widget  widget triggering the event
 * \param[in]   data    extra event data (unused)
 */
static void on_window_grid_destroy(GtkWidget *widget, gpointer data)
{
    debug_gtk3("destroy triggered on %p.", (void *)widget);
}


/** \brief  Handler for window 'configure' events
 *
 * Triggered when a window is moved or changes size. Used currently to update
 * the Window[01]* resources to allow restoring window size and position on
 * emulator start.
 *
 * \param[in]   widget  widget triggering the event
 * \param[in]   event   event reference
 * \param[in]   data    extra event data (used to pass window index)
 *
 * \return  bool
 */
static gboolean on_window_configure_event(GtkWidget *widget,
                                          GdkEvent *event,
                                          gpointer data)
{
    if (event->type == GDK_CONFIGURE) {
#if 0
        GdkEventConfigure *cfg = (GdkEventConfigure *)event;
#endif
        /* determine Window index */
        int windex = GPOINTER_TO_INT(data);

        /* DO NOT UNCOMMENT
         * Uncommenting this will cause the code after it compile just fine.
         * But it would trigger C99.
         */
#if 0
        debug_gtk3("updating window #%d coords and size to (%d,%d)/(%d*%d)"
                " in resources.",
                0, cfg->x, cfg->y, cfg->width, cfg->height);
#endif
        /* set resources, ignore failures */

        gint root_x;
        gint root_y;
        gint width;
        gint height;

        gtk_window_get_position(GTK_WINDOW(widget), &root_x, &root_y);
        gtk_window_get_size(GTK_WINDOW(widget), &width, &height);

        resources_set_int_sprintf("Window%dWidth", width, windex);
        resources_set_int_sprintf("Window%dHeight", height, windex);
        resources_set_int_sprintf("Window%dXpos", root_x, windex);
        resources_set_int_sprintf("Window%dYpos", root_y, windex);
    }
    return FALSE;
}

#ifdef MACOSX_SUPPORT

/* The proper way to use objc_msgSend is to cast it into the right shape each time */
#define OBJC_MSGSEND_FUNC_CAST(...) ((id (*)(__VA_ARGS__))objc_msgSend)

void macos_set_dock_icon_workaround(void);
void macos_activate_application_workaround(void);

/** \brief  Set the macOS dock icon
 *
 * Gtk dock icon support doesn't work on macos (last tested with Gtk 3.24.8)
 * Therefore we get it done via the obj-c API. Except rather than integrate 
 * support for obj-c into the project, leverage some low level C functionality
 * to interact with the obj-c runtime.
 */
void macos_set_dock_icon_workaround()
{
    GBytes *gbytes;
    gconstpointer bytes;
    gsize bytesSize;
    id imageData;
    id logo;
    id application;

    gbytes = uidata_get_bytes("Icon-128@2x.png");

    if (!gbytes) {
        log_error(LOG_ERR, "macos_set_dock_icon_workaround: failed to access icon bytes from gresource file.\n");
        return;
    }

    bytes = g_bytes_get_data(gbytes, &bytesSize);
    imageData =
        OBJC_MSGSEND_FUNC_CAST(id, SEL, gconstpointer, gsize, BOOL)(
            (id)objc_getClass("NSData"),
            sel_getUid("dataWithBytesNoCopy:length:freeWhenDone:"),
            bytes,
            bytesSize,
            NO);
    logo = OBJC_MSGSEND_FUNC_CAST(id, SEL)((id)objc_getClass("NSImage"), sel_getUid("alloc"));
    logo = OBJC_MSGSEND_FUNC_CAST(id, SEL, id)(logo, sel_getUid("initWithData:"), imageData);

    if (logo) {
        application = OBJC_MSGSEND_FUNC_CAST(id, SEL)((id)objc_getClass("NSApplication"), sel_getUid("sharedApplication"));
        OBJC_MSGSEND_FUNC_CAST(id, SEL, id)(application, sel_getUid("setApplicationIconImage:"), logo);
        OBJC_MSGSEND_FUNC_CAST(id, SEL)(logo, sel_getUid("release"));
    } else {
        log_error(LOG_ERR, "macos_set_dock_icon_workaround: failed to initialise image from resource");
    }
}

/** \brief  Bring emulator main window to front on macOS
 *
 * On macOS, this Gtk3 app doesn't activate properly on launch.
 * (last tested with Gtk 3.24.8). This means that the user needs
 * to click the icon in the dock for the emulator window to appear.
 *
 * This workaround is the obj-c runtime equivalent of calling:
 * [[NSApplication sharedApplication] activateIgnoringOtherApps: YES];
 */
void macos_activate_application_workaround()
{
    id ns_application;

    /* [[NSApplication sharedApplication] activateIgnoringOtherApps: YES]; */
    ns_application = OBJC_MSGSEND_FUNC_CAST(id, SEL)((id)objc_getClass("NSApplication"), sel_getUid("sharedApplication"));
    OBJC_MSGSEND_FUNC_CAST(id, SEL, BOOL)(ns_application, sel_getUid("activateIgnoringOtherApps:"), YES);
}

#endif


/** \brief  Event handler for the rendering area's button presses
 *
 * Currently switches fullscreen mode when double-clicking, but can also be
 * used to present a context menu to for some video settings via right-click,
 * which is one of our many TODO's.
 *
 * \param[in]   canvas  rendering area
 * \param[in]   event   event object
 * \param[in]   data    GtkWindow parent of \a canvas
 *
 * \return  TRUE if event accepted
 */
static gboolean rendering_area_event_handler(GtkWidget *canvas,
                                             GdkEventButton *event,
                                             gpointer data)
{
    debug_gtk3("Called!");
    if (event->type == GDK_DOUBLE_BUTTON_PRESS
            && event->button == GDK_BUTTON_PRIMARY) {
        int mouse;

        /* only trigger fullscreen switching when mouse-grab isn't active */
        resources_get_int("Mouse", &mouse);
        if (!mouse) {
            ui_fullscreen_callback(canvas, event);
        }
        /* signal event handled */
        return TRUE;
    }
    /* signal event not handled, avoids the host mouse pointer showing up
     * during mouse grab */
    return FALSE;
}



/** \brief  Create a toplevel window to represent a video canvas
 *
 * This function takes a video canvas structure and builds the widgets
 * that will represent that canvas in the UI as a whole. In the machine
 * emulators, the GtkDrawingArea that represents the actual screen backed
 * by the canvas will be entered into canvas->drawing_area.
 *
 * While it creates the widgets, it does not make them visible. The
 * video canvas routines are expected to do any last-minute processing
 * or preparation, and then call ui_display_main_window() when ready.
 *
 * \param[in]   canvas  the video_canvas_s to initialize
 *
 * \warning The code that calls this apparently creates the VDC window
 *          for x128 before the VIC window (primary) - this is
 *          probably done so the VIC window ends up being on top of
 *          the VDC window. however, we better call some "move window
 *          to front" function instead, and create the windows
 *          starting with the primary one.
 */
void ui_create_main_window(video_canvas_t *canvas)
{
    GtkWidget *new_window;
    GtkWidget *grid;
    GtkWidget *status_bar;
    int target_window;

    GtkWidget *crt_controls;
    GtkWidget *mixer_controls;

    GtkWidget *kbd_widget;
    int kbd_status = 0;


    GdkPixbuf *icon;

    int xpos = -1;
    int ypos = -1;
    int width = 0;
    int height = 0;

    gchar title[256];

    int minimized = 0;
    int full = 0;
    int restore;

    new_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    /* this needs to be here to make the menus with accelerators work */
    ui_menu_init_accelerators(new_window);

    /* set a default C= icon for now */
    icon = get_default_icon();
#ifdef MACOSX_SUPPORT
    macos_set_dock_icon_workaround();
#else
    if (icon != NULL) {
        gtk_window_set_icon(GTK_WINDOW(new_window), icon);
    }
#endif

    /* set title */
    g_snprintf(title, 256, "VICE (%s)", machine_get_name());
    gtk_window_set_title(GTK_WINDOW(new_window), title);

    grid = gtk_grid_new();
    g_signal_connect(grid, "destroy", G_CALLBACK(on_window_grid_destroy), NULL);
    gtk_container_add(GTK_CONTAINER(new_window), grid);
    gtk_orientable_set_orientation(GTK_ORIENTABLE(grid), GTK_ORIENTATION_VERTICAL);
    canvas->grid = grid;

    if (create_window_func != NULL) {
        create_window_func(canvas);
    }

    target_window = -1;
    if (identify_canvas_func != NULL) {
        /* Identify the window as the PRIMARY_WINDOW or SECONDARY_WINDOW. */
        target_window = identify_canvas_func(canvas);
    }
    if (target_window < 0) {
        log_error(LOG_ERR, "ui_create_main_window: canvas not identified!\n");
        archdep_vice_exit(1);
    }
    if (ui_resources.window_widget[target_window] != NULL) {
        log_error(LOG_ERR, "ui_create_main_window: existing window recreated??\n");
        archdep_vice_exit(1);
    }

    /* add status bar */
    status_bar = ui_statusbar_create();
    gtk_widget_show_all(status_bar);
    gtk_widget_set_no_show_all(status_bar, TRUE);

    gtk_container_add(GTK_CONTAINER(grid), status_bar);

    /* add CRT controls */
    crt_controls = NULL;

    if (machine_class != VICE_MACHINE_VSID) {

        if (create_controls_widget_func != NULL) {
            crt_controls = create_controls_widget_func(target_window);
        }
        if (crt_controls != NULL) {
            gtk_widget_hide(crt_controls);
            gtk_container_add(GTK_CONTAINER(grid), crt_controls);
            gtk_widget_set_no_show_all(crt_controls, TRUE);
        }
    }

    if (machine_class != VICE_MACHINE_VSID) {

        /* add sound mixer controls */
        mixer_controls = mixer_widget_create(TRUE, GTK_ALIGN_END);
        gtk_widget_hide(mixer_controls);
        gtk_container_add(GTK_CONTAINER(grid), mixer_controls);
        gtk_widget_set_no_show_all(mixer_controls, TRUE);
    }

    g_signal_connect(new_window, "focus-in-event",
                     G_CALLBACK(on_focus_in_event), NULL);
    g_signal_connect(new_window, "focus-out-event",
                     G_CALLBACK(on_focus_out_event), NULL);
    g_signal_connect(new_window, "window-state-event",
                     G_CALLBACK(on_window_state_event), NULL);
    g_signal_connect(new_window, "delete-event",
                     G_CALLBACK(ui_main_window_delete_event), NULL);
    g_signal_connect(new_window, "destroy",
                     G_CALLBACK(ui_main_window_destroy_callback), NULL);
    /* can probably use the `user_data` to pass window index */
    g_signal_connect(new_window, "configure-event",
                     G_CALLBACK(on_window_configure_event),
                     GINT_TO_POINTER(target_window));
    /*
     * Set up drag-n-drop handling for files
     */
    if (machine_class != VICE_MACHINE_VSID) {
        /* VSID has its own drag-n-drop handlers */

        gtk_drag_dest_set(
                new_window,
                GTK_DEST_DEFAULT_ALL,
                ui_drag_targets,
                UI_DRAG_TARGETS_COUNT,
                GDK_ACTION_COPY);
        g_signal_connect(new_window, "drag-data-received",
                         G_CALLBACK(ui_on_drag_data_received), NULL);
        g_signal_connect(new_window, "drag-drop",
                         G_CALLBACK(ui_on_drag_drop), NULL);
        if (ui_resources.start_minimized) {
            gtk_window_iconify(GTK_WINDOW(new_window));
        }
    }
    ui_resources.canvas[target_window] = canvas;
    ui_resources.window_widget[target_window] = new_window;

    canvas->window_index = target_window;

    /* gtk_window_set_title(GTK_WINDOW(new_window), canvas->viewport->title); */
    ui_display_speed(100.0f, 0.0f, 0); /* initial update of the window status bar */

    /* Connect keyboard handlers, except for VSID
     *
     * TODO:    support hotkeys (if required) for VSID
     */
    if (machine_class != VICE_MACHINE_VSID) {
        kbd_connect_handlers(new_window, NULL);

        /* Add default hotkeys that don't have a menu item */
        if (!kbd_hotkey_add_list(default_hotkeys)) {
            debug_gtk3("adding hotkeys failed, see the log for details.");
        }
    }

    /*
     * Try to restore windows position and size
     */


    /*
     * Do we need to restore window(s) position/size?
     */
    debug_gtk3("Getting value for 'RestoreWindowGeometry'");
    if (resources_get_int("RestoreWindowGeometry", &restore) < 0) {
        debug_gtk3("failed to get value for 'RestoreWindowGeometry'");
        restore = 0;
    }

    if (restore) {
        if (resources_get_int_sprintf("Window%dXpos", &xpos, target_window) < 0) {
            log_error(LOG_ERR, "No for Window%dXpos", target_window);
        }
        resources_get_int_sprintf("Window%dYpos", &ypos, target_window);
        resources_get_int_sprintf("Window%dwidth", &width, target_window);
        resources_get_int_sprintf("Window%dheight", &height, target_window);

        debug_gtk3("X: %d, Y: %d, W: %d, H: %d", xpos, ypos, width, height);
        if (xpos < 0 || ypos < 0 || width <= 0 || height <= 0) {
            /* def. not legal */
            debug_gtk3("shit ain't legal!");
        } else {
            gtk_window_move(GTK_WINDOW(new_window), xpos, ypos);
            gtk_window_resize(GTK_WINDOW(new_window), width, height);
        }
    }

    /*
     * Do we start minimized?
     */
    if (resources_get_int("StartMinimized", &minimized) < 0) {
        debug_gtk3("failed to get resource 'StartMinimized', ignoring.");
        minimized = 0;  /* fallback : not minimized */
    }
    if (minimized) {
        /* there's no gtk_window_minimize() so we do this:
         * (there is a gtk_window_maximize(), so for API consistency I'd would
         *  probably have added gtk_window_minimize() to mirror the maximize
         *  function)
         */
        gtk_window_iconify(GTK_WINDOW(new_window));
    } else {
        /* my guess is a minimized/iconified window cannot be fullscreen */
        if (resources_get_int("FullscreenEnable", &full) < 0) {
            debug_gtk3("failed to get FullscreenEnabled resource.");
        } else {
            if (full) {
                gtk_window_fullscreen(GTK_WINDOW(new_window));
            } else {
                gtk_window_unfullscreen(GTK_WINDOW(new_window));
            }
        }
    }

    if (resources_get_int("KbdStatusbar", &kbd_status) < 0) {
        debug_gtk3("Failed to get KbdStatusbar resource, defaulting to False.");
        kbd_status = 0;
    }
    kbd_widget = gtk_grid_get_child_at(GTK_GRID(status_bar), 0, 2);

    if (kbd_status) {
        gtk_widget_show_all(kbd_widget);
    } else {
        gtk_widget_hide(kbd_widget);
    }

    if (grid != NULL) {
        /* get rendering area */
        GtkWidget *render_area = gtk_grid_get_child_at(GTK_GRID(grid), 0, 1);

        /* set up event handler for clicks on the canvas */
        g_signal_connect(
                render_area,
                "button-press-event",
                G_CALLBACK(rendering_area_event_handler),
                new_window);
    }


#ifdef MACOSX_SUPPORT
    macos_activate_application_workaround();
#endif
}


/** \brief  Makes a main window visible once it's been initialized
 *
 * \param[in]   index   which window to display
 *
 * \sa      ui_resources_s::window_widget
 */
void ui_display_main_window(int index)
{
    if (ui_resources.window_widget[index]) {
        /* Normally this would show everything in the window,
         * including hidden status bar displays, but we've
         * disabled secondary displays in the status bar code with
         * gtk_widget_set_no_show_all(). */
        gtk_widget_show_all(ui_resources.window_widget[index]);
        active_win_index = index;
    }
}

/** \brief  Destroy a main window
 *
 * \param[in]   index   which window to destroy
 *
 * \sa      ui_resources_s::window_widget
 */
void ui_destroy_main_window(int index)
{
    if (ui_resources.window_widget[index]) {
        gtk_widget_destroy(ui_resources.window_widget[index]);
    }
}


/** \brief  Initialize command line options (generic)
 *
 * \return  0 on success, -1 on failure
 */
int ui_cmdline_options_init(void)
{
    /* seems complete to me -- compyx */
#if 0
    INCOMPLETE_IMPLEMENTATION();
#endif
    return cmdline_register_options(cmdline_options_common);
}


/** \brief  Display a generic file chooser dialog
 *
 * \param[in]   format  format string for the dialog's title
 *
 * \return  a copy of the chosen file's name; free it with lib_free()
 *
 * \note    This is currently only called by event_playback_attach_image()
 *
 * \warning This function is unimplemented and will intentionally crash
 *          VICE if it is called.
 */
char *ui_get_file(const char *format, ...)
{
    /*
     * Also not called when trying to play back events, at least, I've never
     * seen this called.
     */
    NOT_IMPLEMENTED();
    return NULL;
}


/** \brief  Initialize Gtk3/GLib
 *
 * \param[in]   argc    pointer to main()'s argc
 * \param[in]   argv    main()'s argv
 *
 * \return  0 on success, -1 on failure
 */
int ui_init(int *argc, char **argv)
{

    GSettings *settings;
    GVariant *variant;
    GtkSettings *settings_default;

#if 0
    INCOMPLETE_IMPLEMENTATION();
#endif
    gtk_init(argc, &argv);

    kbd_hotkey_init();

    /*
     * Make sure F10 doesn't trigger the menu bar
     *
     * I tried unmapping via CSS, but according to the Gtk devs, this little
     * hack works, and it does.
     */
    settings_default = gtk_settings_get_default();
    g_object_set(settings_default, "gtk-menu-bar-accel", "F20", NULL);


    if (!uidata_init()) {
        log_error(LOG_ERR,
                "failed to initialize GResource data, don't expect much"
                " when it comes to icons, fonts or logos.");
    }

    debug_gtk3("Registering CBM font.");
    if (!archdep_register_cbmfont()) {
        debug_gtk3("failed, continuing");
        log_error(LOG_ERR, "failed to register CBM font.");
    }

    /*
     * Sort directories before files in GtkFileChooser
     *
     * Perhaps turn this into a resource when people start complaining? Though
     * personally I'm used to having directories sorted before files.
     *
     * FIXME:   This alters Gtk/GLib settings globally, ie Wm/Desktop-wide.
     *          Which probably isn't the correct way.
     */
    settings = g_settings_new("org.gtk.Settings.FileChooser");
    variant = g_variant_new("b", TRUE);
    g_settings_set_value(settings, "sort-directories-first", variant);

    ui_statusbar_init();
    return 0;
}


/** \brief  Finish initialization after loading the resources
 *
 * \note    This function exists for compatibility with other UIs.
 *
 * \return  0 on success, -1 on failure
 *
 * \sa      ui_init_finalize()
 */
int ui_init_finish(void)
{
    return 0;
}


/** \brief  Finalize initialization after creating the main window(s)
 *
 * \note    This function exists for compatibility with other UIs,
 *          but could perhaps be used to activate fullscreen from the
 *          command-line or saved settings file (as it is in WinVICE.)
 *
 * \return  0 on success, -1 on failure
 *
 * \sa      ui_init_finish()
 */
int ui_init_finalize(void)
{
    return 0;
}


/** \brief  Display a dialog box in response to a CPU jam
 *
 * \param[in]   format  format string for the message to display
 *
 * \return  the action the user selected in response to the jam
 */
ui_jam_action_t ui_jam_dialog(const char *format, ...)
{
    va_list args;
    char *buffer;
    int result;

    va_start(args, format);
    buffer = lib_mvsprintf(format, args);
    va_end(args);

    ui_set_ignore_mouse_hide(TRUE);

    /* XXX: this probably needs a variable index into the window_widget array */
    result = jam_dialog(ui_resources.window_widget[PRIMARY_WINDOW], buffer);
    lib_free(buffer);

    ui_set_ignore_mouse_hide(FALSE);

    return result;
}


/** \brief  Initialize resources related to the UI in general
 *
 * \return  0 on success, -1 on failure
 */
int ui_resources_init(void)
{
    int i;

    /* initialize command int/bool resources */
    if (resources_register_int(resources_int_shared) != 0) {
        return -1;
    }
#if 0
    /* initialize string resources */
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }
#endif
    /* initialize int/bool resources */
    if (resources_register_int(resources_int_primary_window) < 0) {
        return -1;
    }

    if (machine_class == VICE_MACHINE_C128) {
        if (resources_register_int(resources_int_secondary_window) < 0) {
            return -1;
        }
    }

    for (i = 0; i < NUM_WINDOWS; ++i) {
        ui_resources.canvas[i] = NULL;
        ui_resources.window_widget[i] = NULL;
    }

    return 0;
}


/** \brief  Clean up memory used by VICE resources
 */
void ui_resources_shutdown(void)
{
}

/** \brief Clean up memory used by the UI system itself
 */
void ui_shutdown(void)
{
    uidata_shutdown();
    ui_statusbar_shutdown();
}

/** \brief  Update all menu item checkmarks on all windows
 *
 * \note    This is called from multiple functions in autostart.c and also
 *          mon_resource_set() in monitor/monitor.c when they change the
 *          value of resources.
 *
 * \todo    This is unimplemented, but will be much easier to implement if we
 *          switch to using a GtkApplication/GMenu based UI.
 */
void ui_update_menus(void)
{
    /* NOP: Gtk3 doesn't need this */
}


/** \brief  Dispatch next GLib main context event
 *
 * \warning According to the Gtk3/GLib devs, this will at some point
 *          bite us in the arse.
 */
void ui_dispatch_next_event(void)
{
    g_main_context_iteration(NULL, FALSE);
}


/** \brief  Dispatch events pending in the GLib main context loop
 *
 * \warning According to the Gtk3/GLib devs, this will at some point
 *          bite us in the arse.
 */
void ui_dispatch_events(void)
{
    while (g_main_context_iteration(NULL, FALSE)) {
        /* NOP */
    }
}

/** \brief  Display the "Do you want to extend the disk image to
 *          40-track format?" dialog
 *
 * \return  nonzero to extend the image, 0 otherwise
 *
 * \warning This function is not implemented and it will intentionally
 *          crash VICE if called.
 */
int ui_extend_image_dialog(void)
{
    /* FIXME: this dialog needs to be implemented. */
    NOT_IMPLEMENTED();
    return 0;
}


/** \brief  Display error message through the UI
 *
 * \param[in]   format  format string for the error
 */
void ui_error(const char *format, ...)
{
    char *buffer;
    va_list ap;

    va_start(ap, format);
    buffer = lib_mvsprintf(format, ap);
    va_end(ap);

    vice_gtk3_message_error("VICE Error", buffer);
    lib_free(buffer);
}


/** \brief  Display a message through the UI
 *
 * \param[in]   format  format string for message
 */
void ui_message(const char *format, ...)
{
    char *buffer;
    va_list ap;

    va_start(ap, format);
    buffer = lib_mvsprintf(format, ap);
    va_end(ap);

    vice_gtk3_message_info("VICE Message", buffer);
    lib_free(buffer);
}

#if 0
/** \brief  Display FPS (and some other stuff) in the title bar of each
 *          window
 *
 * \param[in]   percent    CPU speed ratio
 * \param[in]   framerate  frame rate
 * \param[in]   warp_flag  nonzero if warp mode is active
 */
void ui_display_speed(float percent, float framerate, int warp_flag)
{
    int i;
    char str[128];
    int percent_int = (int)(percent + 0.5);
    int framerate_int = (int)(framerate + 0.5);
    char *warp, *mode[3] = {"", " (VDC)", " (Monitor)"};

    for (i = 0; i < NUM_WINDOWS; i++) {
        if (ui_resources.canvas[i] && GTK_WINDOW(ui_resources.window_widget[i])) {
            warp = (warp_flag ? "(warp)" : "");
            str[0] = 0;
            if (machine_class != VICE_MACHINE_VSID) {
                snprintf(str, 128, "%s%s - %3d%%, %2d fps %s%s",
                         ui_resources.canvas[i]->viewport->title, mode[i],
                         percent_int, framerate_int, warp,
                         is_paused ? " (Paused)" : "");
            } else {
                snprintf(str, 128, "VSID - %3d%% %s%s",
                         percent_int, warp,
                         is_paused ? " (Paused)" : "");
            }
            str[127] = 0;
            gtk_window_set_title(GTK_WINDOW(ui_resources.window_widget[i]), str);
        }
    }
}
#endif


/** \brief  Keeps the ui events going while the emulation is paused
 *
 * \param[in]   addr    unused
 * \param[in]   data    unused
 */
static void pause_trap(uint16_t addr, void *data)
{
    ui_display_paused(1);
    vsync_suspend_speed_eval();
    while (is_paused) {
        ui_dispatch_next_event();
        g_usleep(10000);
    }
}


/** \brief  This should display some 'pause' status indicator on the statusbar
 *
 * \param[in]   flag    pause state
 */
void ui_display_paused(int flag)
{
    ui_display_speed(0.0, 0.0, 0);
}


/** \brief  Get pause active state
 *
 * \return  boolean
 */
int ui_pause_active(void)
{
    return is_paused;
}


/** \brief  Pause emulation
 */
void ui_pause_enable(void)
{
    if (!ui_pause_active()) {
        is_paused = 1;
        interrupt_maincpu_trigger_trap(pause_trap, 0);
        ui_display_paused(1);
    }
}


/** \brief  Unpause emulation
 */
void ui_pause_disable(void)
{
    if (ui_pause_active()) {
        is_paused = 0;
        ui_display_paused(0);
    }
}


/** \brief  Toggle pause state
 */
void ui_pause_toggle(void)
{
    if (ui_pause_active()) {
        ui_pause_disable();
    } else {
        ui_pause_enable();
    }
}


/** \brief  Pause toggle handler
 *
 * \return  TRUE (indicates the Alt+P got consumed by Gtk, so it won't be
 *          passed to the emu)
 *
 * \todo    Update UI tickmarks properly if triggered by a keyboard
 *          accelerator, or the settings dialog.
 */
gboolean ui_toggle_pause(void)
{
    ui_pause_toggle();
    /* TODO: somehow update the checkmark in the menu without reverting to
     *       weird code like Gtk
     */
    return TRUE;    /* has to be TRUE to avoid passing Alt+P into the emu */
}


/** \brief  Toggle warp mode
 */
static void ui_toggle_warp(void)
{
    ui_toggle_resource(NULL, (gpointer)"WarpMode");
}



/** \brief  Advance frame handler
 *
 * \return  TRUE (indicates the Alt+SHIFT+P got consumed by Gtk, so it won't be
 *          passed to the emu)
 *
 * \note    The gboolean return value is no longer required since the 'hotkey'
 *          handling in kbd.c takes care of passing TRUE to Gtk3.
 */
gboolean ui_advance_frame(void)
{
    if (ui_pause_active()) {
        vsyncarch_advance_frame();
    } else {
        ui_pause_enable();
    }

    return TRUE;    /* has to be TRUE to avoid passing Alt+SHIFT+P into the emu */
}

/** \brief  Shutdown the UI, clean up resources
 */
void ui_exit(void)
{
    int soe;    /* save on exit */

    /* clean up UI resources */
    if (machine_class != VICE_MACHINE_VSID) {
        uicart_shutdown();
        ui_disk_attach_shutdown();
        ui_tape_attach_shutdown();
        ui_smart_attach_shutdown();
    }

    ui_settings_shutdown();

    /* Destroy the main window(s) */
    ui_destroy_main_window(PRIMARY_WINDOW);
    ui_destroy_main_window(SECONDARY_WINDOW);

    resources_get_int("SaveResourcesOnExit", &soe);
    if (soe) {
        resources_save(NULL);
    }

    /* unregister the CBM font */
    archdep_unregister_cbmfont();

    /* deallocate memory used by the unconnected keyboard shortcuts */
    kbd_hotkey_shutdown();

    /* trigger any remaining Gtk/GLib events */
    while (g_main_context_pending(g_main_context_default())) {
        debug_gtk3("processing pending event.");
        g_main_context_iteration(g_main_context_default(), TRUE);
    }
    archdep_vice_exit(0);
}

/** \brief  Send current light pen state to the emulator core for all windows
 */
void ui_update_lightpen(void)
{
    video_canvas_t *canvas;
    canvas = ui_resources.canvas[PRIMARY_WINDOW];
    if (machine_class == VICE_MACHINE_C128) {
        /* According to lightpen.c, x128 flips primary and secondary
         * windows compared to what the GTK3 backend expects. */
        if (canvas) {
            lightpen_update(1, canvas->pen_x, canvas->pen_y, canvas->pen_buttons);
        }
        canvas = ui_resources.canvas[SECONDARY_WINDOW];
    }
    if (canvas) {
        lightpen_update(0, canvas->pen_x, canvas->pen_y, canvas->pen_buttons);
    }
}


/** \brief  Enable/disable CRT controls
 *
 * \param[in]   enabled enabled state for the CRT controls
 */
void ui_enable_crt_controls(int enabled)
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *crt;

    if (active_win_index < 0 || active_win_index >= NUM_WINDOWS) {
        /* No window created yet, most likely. */
        return;
    }

    window = ui_resources.window_widget[active_win_index];
    grid = gtk_bin_get_child(GTK_BIN(window));
    crt = gtk_grid_get_child_at(GTK_GRID(grid), 0, ROW_CRT_CONTROLS);

    if (enabled) {
        gtk_widget_show(crt);
    } else {
        gtk_widget_hide(crt);
        /*
         * This is completely counter-intuitive, but it works, unlike all other
         * size_request()/set_size_hint() stuff.
         * Appearently setting a size of 1x1 pixels forces Gtk3 to render the
         * window to the appropriate (minimum) size,
         */
        gtk_window_resize(GTK_WINDOW(window), 1, 1);
    }
}


/** \brief  Enable/disable mixer controls
 *
 * \param[in]   enabled enabled state for the mixer controls
 */
void ui_enable_mixer_controls(int enabled)
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *mixer;

    if (active_win_index < 0 || active_win_index >= NUM_WINDOWS) {
        /* No window created yet, most likely. */
        return;
    }

    window = ui_resources.window_widget[active_win_index];
    grid = gtk_bin_get_child(GTK_BIN(window));
    mixer = gtk_grid_get_child_at(GTK_GRID(grid), 0, ROW_MIXER_CONTROLS);

    if (enabled) {
        gtk_widget_show(mixer);
    } else {
        gtk_widget_hide(mixer);
        /*
         * This is completely counter-intuitive, but it works, unlike all other
         * size_request()/set_size_hint() stuff.
         * Appearently setting a size of 1x1 pixels forces Gtk3 to render the
         * window to the appropriate (minimum) size,
         */
        gtk_window_resize(GTK_WINDOW(window), 1, 1);
    }
}


/** \brief  Get GtkWindow instance by \a index
 *
 * \param[in]   index   index in the windows widgets array (0-2)
 *
 * \return  GtkWindow instance
 */
GtkWidget *ui_get_window_by_index(int index)
{
    if (index < 0 || index >= NUM_WINDOWS) {
        debug_gtk3("invalid window index %d.", index);
        return NULL;
    }
    return ui_resources.window_widget[index];
}
