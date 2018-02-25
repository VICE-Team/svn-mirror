/** \file   ui.c
 * \brief   Native GTK3 UI stuff.
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
#include <stdbool.h>

#include "debug_gtk3.h"
#include "not_implemented.h"

#include "cmdline.h"
#include "drive.h"
#include "interrupt.h"
#include "kbd.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "lightpen.h"
#include "resources.h"
#include "translate.h"
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

#include "ui.h"



/*****************************************************************************
 *                  Defines, enums, type declarations                        *
 ****************************************************************************/

/** \brief  Default HTML browser
 *
 * \todo    Needs ifdef's for different archs
 */
#define HTML_BROWSER_COMMAND_DEFAULT    "firefox %s"



/** \brief  Collection of UI resources
 *
 * This needs to stay here, to allow the command line and resources initializers
 * to references the ui resources
 */
ui_resource_t ui_resources; /* public for use in uicommands.c, not ideal */



/* Forward declarations of static functions */

static int set_html_browser_command(const char *val, void *param);
static int set_save_resources_on_exit(int val, void *param);
static int set_confirm_on_exit(int val, void *param);
static int set_window_height(int val, void *param);
static int set_window_width(int val, void *param);
static int set_window_xpos(int val, void *param);
static int set_window_ypos(int val, void *param);


/*****************************************************************************
 *                              Static data                                  *
 ****************************************************************************/


/** \brief  String type resources list
 */
static const resource_string_t resources_string[] = {
    { "HTMLBrowserCommand", HTML_BROWSER_COMMAND_DEFAULT, RES_EVENT_NO, NULL,
        &ui_resources.html_browser_command, set_html_browser_command, NULL },
    RESOURCE_STRING_LIST_END
};


/** \brief  Integer/Boolean type resources list
 */
static const resource_int_t resources_int_primary_window[] = {
    { "SaveResourcesOnExit", 0, RES_EVENT_NO, NULL,
        &ui_resources.save_resources_on_exit, set_save_resources_on_exit, NULL },
    { "ConfirmOnExit", 1, RES_EVENT_NO, NULL,
        &ui_resources.confirm_on_exit, set_confirm_on_exit, NULL },

    /* Window size and position setters */

    /* primary window (all emulators) */
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

/** \brief  Integer/Boolean type resources list for VDC window
 */
static const resource_int_t resources_int_secondary_window[] = {
    /* secondary window (C128's VDC display) */
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
static const cmdline_option_t cmdline_options_common[] = {
    { "-htmlbrowser", SET_RESOURCE, 1,
        NULL, NULL, "HTMLBrowserCommand", NULL,
        USE_PARAM_STRING, USE_DESCRIPTION_STRING,
        IDCLS_UNUSED, IDCLS_UNUSED,
        N_("<Command>"), N_("Specify and HTML browser for the on-line help") },

    { "-confirmexit", SET_RESOURCE, 0,
        NULL, NULL, "SaveResourcesOnExit", (void*)1,
        USE_PARAM_STRING, USE_DESCRIPTION_STRING,
        IDCLS_UNUSED, IDCLS_UNUSED,
        NULL, N_("Never confirm quitting VICE") },
    { "+confirmexit", SET_RESOURCE, 0,
        NULL, NULL, "SaveResourcesOnExit", (void*)0,
        USE_PARAM_STRING, USE_DESCRIPTION_STRING,
        IDCLS_UNUSED, IDCLS_UNUSED,
        NULL, N_("Don't confirm quitting VICE") },

    { "-saveres", SET_RESOURCE, 0,
        NULL, NULL, "SaveResourcesOnExit", (void *)1,
        USE_PARAM_STRING, USE_DESCRIPTION_STRING,
        IDCLS_UNUSED, IDCLS_UNUSED,
        NULL, N_("Save settings on exit") },
    { "+saveres", SET_RESOURCE, 0,
        NULL, NULL, "SaveResourcesOnExit", (void *)0,
        USE_PARAM_STRING, USE_DESCRIPTION_STRING,
        IDCLS_UNUSED, IDCLS_UNUSED,
        NULL, N_("Never save settings on exit") },

    CMDLINE_LIST_END
};


/** \brief  Flag indicating pause mode
 */
static int is_paused = 0;

/** \brief  Signals the html_browser_command field of the resource got allocated
 */
static int html_browser_command_set = 0;

/** \brief  Index of the most recently focused main window
 */
static int active_win_index = -1;

/** \brief  Flag indicating whether we're supposed to be in fullscreen
 */
static int is_fullscreen = 0;

/** \brief  Flag inidicating whether fullscreen mode shows the decorations
 */
static int fullscreen_has_decorations = 0;

/** \brief  Function to help create a main window
 */
static void (*create_window_func)(video_canvas_t *) = NULL;

/** \brief  Function to identify a canvas from its video chip
 */
static int (*identify_canvas_func)(video_canvas_t *) = NULL;

/******************************************************************************
 *                              Event handlers                                *
 *****************************************************************************/

/** \brief  Get the most recently focused main window
 *
 * \return  pointer to a main window, or NULL
 */
GtkWindow *ui_get_active_window(void)
{
    if (active_win_index < 0) {
        /* If we end up here it probably means no main window has
         * been created yet. */
        return NULL;
    }

    return GTK_WINDOW(ui_resources.window_widget[active_win_index]);
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


/** \brief  Get a window's index
 *
 * \param[in]   widget      window to get the index of
 *
 * \return  window index, or -1 if not a main window
*/
static int ui_get_window_index(GtkWidget *widget)
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

/** \brief  Handler for the "focus-in-event" of a GtkWindow
 *
 * \param[in]   widget      window triggering the event
 * \param[in]   event       window focus details
 * \param[in]   user_data   extra data for the event (ignored)
 *
 * \return  FALSE to continue processing
 *
 * \todo    GtkApplication tracks the currently-active window using a similar
 *          method to this handler, so if we start using GtkApplication we
 *          should only use this for canvas-window-specific stuff like
 *          fullscreen mode, and use gtk_application_get_active_window() to
 *          provide parent windows for dialogs.
 */
static gboolean on_focus_in_event(GtkWidget *widget, GdkEventFocus *event,
                                  gpointer user_data)
{
    int index = ui_get_window_index(widget);

    if (index < 0) {
        /* We should never end up here. */
        log_error(LOG_ERR, "focus-in-event: window not found\n");
        exit(1);
    }

    if (event->in == TRUE) {
        /* fprintf(stderr, "window %d: focus-in\n", index); */
        active_win_index = index;
    }

    return FALSE;
}


/** \brief Show or hide the window decorations as needed
 */
static void ui_update_fullscreen_decorations(void)
{
    GtkWidget *window, *grid, *menu_bar, *status_bar;
    int has_decorations;

    if (active_win_index < 0) {
        return;
    }

    has_decorations = (!is_fullscreen) || fullscreen_has_decorations;
    window = ui_resources.window_widget[active_win_index];
    grid = gtk_bin_get_child(GTK_BIN(window));
    menu_bar = gtk_grid_get_child_at(GTK_GRID(grid), 0, 0);
    status_bar = gtk_grid_get_child_at(GTK_GRID(grid), 0, 2);

    if (has_decorations) {
        gtk_widget_show(menu_bar);
        gtk_widget_show(status_bar);
    } else {
        gtk_widget_hide(menu_bar);
        gtk_widget_hide(status_bar);
    }
}

/** \brief  Handler for the "window-state-event" of a GtkWindow
 *
 * \param[in]   widget      window triggering the event
 * \param[in]   event       window state details
 * \param[in]   user_data   extra data for the event (ignored)
 *
 * \return  FALSE to continue processing
 */
static gboolean on_window_state_event(GtkWidget *widget, GdkEventWindowState *event,
                                      gpointer user_data)
{
    GdkWindowState win_state = event->new_window_state;
    int index = ui_get_window_index(widget);

    if (index < 0) {
        /* We should never end up here. */
        log_error(LOG_ERR, "window-state-event: window not found\n");
        exit(1);
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
 */
void ui_fullscreen_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWindow *window;

    if (active_win_index < 0) {
        return;
    }

    window = GTK_WINDOW(ui_resources.window_widget[active_win_index]);
    is_fullscreen = !is_fullscreen;

    if (is_fullscreen) {
        gtk_window_fullscreen(window);
    } else {
        gtk_window_unfullscreen(window);
    }

    ui_update_fullscreen_decorations();
}

/** \brief Toggles fullscreen window decorations in response to user request
 *
 * \param[in]   widget      the widget that sent the callback (ignored)
 * \param[in]   user_data   extra data for the callback (ignored)
 */
void ui_fullscreen_decorations_callback(GtkWidget *widget, gpointer user_data)
{
    fullscreen_has_decorations = !fullscreen_has_decorations;
    ui_update_fullscreen_decorations();
}

/*****************************************************************************
 *                  Temporary windows atexit() crash workaround              *
 ****************************************************************************/
#ifdef WIN32_COMPILE
#define ATEXIT_MAX_FUNCS 64

static void *atexit_functions[ATEXIT_MAX_FUNCS + 1];

static int atexit_counter = 0;

int vice_atexit(void (*function)(void))
{
    int i;

    if (!atexit_counter) {
        for (i = 0; i <= ATEXIT_MAX_FUNCS; ++i) {
            atexit_functions[i] = NULL;
        }
    }

    if (atexit_counter == ATEXIT_MAX_FUNCS) {
        return 1;
    }

    atexit_functions[atexit_counter++] = function;

    return 0;
}

static void atexit_functions_execute(void)
{
    int i = atexit_counter -1;
    void (*f)(void) = NULL;

    while (i >= 0 && atexit_functions[i]) {
        f = atexit_functions[i--];
        f();
    }
}
#endif  /* ifdef WIN32_COMPILE */



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
    return (index >= 0 || index < NUM_WINDOWS) ? index : -1;
}


/*
 * Resource getters/setters
 */


/** \brief  Set new HTML browser command string
 *
 * \param[in]   val     browser command string
 * \param[in]   param   extra parameter (ignored)
 *
 * \return  0
 */
static int set_html_browser_command(const char *val, void *param)
{
    util_string_set(&ui_resources.html_browser_command, val);
    html_browser_command_set = 1;
    return 0;
}


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


/** \brief  Create a toplevel window to represent a video canvas
 *
 * This function takes a video canvas structure and builds the widgets
 * that will represent that canvas in the UI as a whole. The
 * GtkDrawingArea that represents the actual screen backed by the
 * canvas will be entered into canvas->drawing_area.
 *
 * While it creates the widgets, it does not make them visible. The
 * video canvas routines are expected to do any last-minute processing
 * or preparation, and then call ui_display_toplevel_window() when
 * ready.
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
void ui_create_toplevel_window(video_canvas_t *canvas)
{
    GtkWidget *new_window, *grid, *status_bar;
    int target_window;

    new_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    /* this needs to be here to make the menus with accelerators work */
    ui_menu_init_accelerators(new_window);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(new_window), grid);
    gtk_orientable_set_orientation(GTK_ORIENTABLE(grid), GTK_ORIENTATION_VERTICAL);
    canvas->grid = grid;

    if (create_window_func != NULL) {
        create_window_func(canvas);
    }

    status_bar = ui_statusbar_create();
    gtk_widget_show_all(status_bar);
    gtk_widget_set_no_show_all(status_bar, TRUE);

    gtk_container_add(GTK_CONTAINER(grid), status_bar);

    g_signal_connect(new_window, "focus-in-event",
                     G_CALLBACK(on_focus_in_event), NULL);
    g_signal_connect(new_window, "window-state-event",
                     G_CALLBACK(on_window_state_event), NULL);
    g_signal_connect(new_window, "delete-event",
                     G_CALLBACK(on_delete_event), NULL);
    g_signal_connect(new_window, "destroy",
                     G_CALLBACK(ui_window_destroy_callback), NULL);

    target_window = -1;
    if (identify_canvas_func != NULL) {
        /* Identify the window as the PRIMARY_WINDOW or SECONDARY_WINDOW. */
        target_window = identify_canvas_func(canvas);
    }
    if (target_window < 0) {
        log_error(LOG_ERR, "ui_create_toplevel_window: canvas not identified!\n");
        exit(1);
    }
    if (ui_resources.window_widget[target_window] != NULL) {
        log_error(LOG_ERR, "ui_create_toplevel_window: existing window recreated??\n");
        exit(1);
    }

    ui_resources.canvas[target_window] = canvas;
    ui_resources.window_widget[target_window] = new_window;

    canvas->window_index = target_window;

    /* gtk_window_set_title(GTK_WINDOW(new_window), canvas->viewport->title); */
    ui_display_speed(100.0f, 0.0f, 0); /* initial update of the window status bar */

    /* connect keyboard handlers */
    kbd_connect_handlers(new_window, NULL);
}

/** \brief  Makes the a window visible once it's been initialized
 *
 * \param[in]   index   which window to display
 *
 * \sa      ui_resources_s::window_widget
 */
void ui_display_toplevel_window(int index)
{
    if (ui_resources.window_widget[index]) {
        /* Normally this would show everything in the window,
         * including hidden status bar displays, but we've
         * disabled secondary displays in the status bar code with
         * gtk_widget_set_no_show_all(). */
        gtk_widget_show_all(ui_resources.window_widget[index]);
    }
}



/** \brief  Initialize command line options (generic)
 *
 * \return  0 on success, -1 on failure
 */
int ui_cmdline_options_init(void)
{
    INCOMPLETE_IMPLEMENTATION();
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
    INCOMPLETE_IMPLEMENTATION();
    gtk_init(argc, &argv);
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

    /* XXX: this sucks */
    result = jam_dialog(ui_resources.window_widget[PRIMARY_WINDOW], buffer);
    lib_free(buffer);

    return result;
}


/** \brief  Initialize resources related to the UI in general
 *
 * \return  0 on success, -1 on failure
 */
int ui_resources_init(void)
{
    int i;
    /* initialize string resources */
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }
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

    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Clean up memory used by VICE resources
 */
void ui_resources_shutdown(void)
{
    if (html_browser_command_set) {
        lib_free(ui_resources.html_browser_command);
    }
    INCOMPLETE_IMPLEMENTATION();
}

/** \brief Clean up memory used by the UI system itself
 */
void ui_shutdown(void)
{
    ui_settings_dialog_shutdown();
    ui_statusbar_shutdown();
}

/** \brief  Update all menu item checkmarks on all windows
 *
 * \note    This is called by autostart.c (and probably other files)
 *          when they change the value of resources.
 *
 * \todo    This is unimplemented, but will be much easier to implement if we
 *          switch to using a GtkApplication/GMenu based UI.
 */
void ui_update_menus(void)
{
    /* allows autostart to work */
    NOT_IMPLEMENTED_WARN_ONLY();
}


/** \brief  Dispatch next GLib main context event
 *
 * \warning According to the Gtk3/GLib devs, this will at some point
 *          bite us in the arse.
 */
void ui_dispatch_next_event(void)
{
    g_main_context_iteration(g_main_context_default(), FALSE);
}


/** \brief  Dispatch events pending in the GLib main context loop
 *
 * \warning According to the Gtk3/GLib devs, this will at some point
 *          bite us in the arse.
 */
void ui_dispatch_events(void)
{
    while (g_main_context_pending(g_main_context_default())) {
        ui_dispatch_next_event();
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
    char *warp, *mode[3] = {"", _(" (VDC)"), _(" (Monitor)")};

    for (i = 0; i < NUM_WINDOWS; i++) {
        if (ui_resources.canvas[i] && GTK_WINDOW(ui_resources.window_widget[i])) {
            warp = (warp_flag ? _("(warp)") : "");
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


/** \brief  Pause emulation
 *
 * \param[in]   flag    toggle pause state if true
 */
void ui_pause_emulation(int flag)
{
    if (flag && !is_paused) {
        is_paused = 1;
        interrupt_maincpu_trigger_trap(pause_trap, 0);
    } else {
        ui_display_paused(0);
        is_paused = 0;
    }
}



/** \brief  Check if emulation is paused
 *
 * \return  nonzero if emulation is paused
 */
int ui_emulation_is_paused(void)
{
    return is_paused;
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
    ui_pause_emulation(!is_paused);
    /* TODO: somehow update the checkmark in the menu without reverting to
     *       weird code like Gtk
     */
    return TRUE;    /* has to be TRUE to avoid passing Alt+P into the emu */
}

/** \brief  Advance frame handler
 *
 * \return  TRUE (indicates the Alt+SHIFT+P got consumed by Gtk, so it won't be
 *          passed to the emu)
 *
 * \todo    Update UI tickmarks properly if triggered by a keyboard accelerator.
 */
gboolean ui_advance_frame(void)
{
    if (ui_emulation_is_paused()) {
        vsyncarch_advance_frame();
    } else {
        ui_pause_emulation(1);
    }

    return TRUE;    /* has to be TRUE to avoid passing Alt+SHIFT+P into the emu */
}

/** \brief  Shutdown the UI, clean up resources
 */
void ui_exit(void)
{
    int soe;    /* save on exit */

    resources_get_int("SaveResourcesOnExit", &soe);
    if (soe) {
        resources_save(NULL);
    }

    INCOMPLETE_IMPLEMENTATION();
#ifdef WIN32_COMPILE
    atexit_functions_execute();
#endif
    exit(0);
}

/** \brief  Send current light pen state to the emulator core for all windows
 */
void vice_gtk3_lightpen_update(void)
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
