/*
 * ui.c - Native GTK3 UI stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *  Bas Wassink <b.wassink@ziggo.nl>
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
#include <string.h>
#include <gtk/gtk.h>

#include "debug_gtk3.h"
#include "not_implemented.h"

#include "cmdline.h"
#include "kbd.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "translate.h"
#include "uiaccelerators.h"
#include "uiapi.h"
#include "uistatusbar.h"
#include "util.h"
#include "videoarch.h"

#include "ui.h"


/** \brief  Default HTML browser
 *
 * \todo    Needs ifdef's for different archs
 */
#define HTML_BROWSER_COMMAND_DEFAULT    "firefox %s"


#define NUM_WINDOWS 3

/** \brief  Windows indici
 */
enum {
    PRIMARY_WINDOW,     /**< primary window, all emulators */
    SECONDARY_WINDOW,   /**< secondary window, C128's VDC */
    MONITOR_WINDOW,     /**< optional monitor window/terminal */
};


/** \brief  Struct holding basic UI rescources
 */
typedef struct ui_resources_s {

    char *html_browser_command; /**< HTMLBrowserCommand (str) */
    int save_resources_on_exit; /**< SaveResourcesOnExit (bool) */
    int confirm_on_exit;        /**< ConfirmOnExit (bool) */

    int depth;

    video_canvas_t *canvas[NUM_WINDOWS];
    GtkWidget *window_widget[NUM_WINDOWS]; /**< the toplevel GtkWidget (Window) */
    int window_width[NUM_WINDOWS];
    int window_height[NUM_WINDOWS];
    int window_xpos[NUM_WINDOWS];
    int window_ypos[NUM_WINDOWS];

} ui_resource_t;


static ui_resource_t ui_resources;

static void window_destroy_cb(void)
{
    ui_exit();
}

/* FIXME: the code that calls this apparently creates the VDC window for x128
          before the VIC window (primary) - this is probably done so the VIC
          window ends up being on top of the VDC window. however, we better call
          some "move window to front" function instead, and create the windows
          starting with the primary one. */
/* FIXME: the code below deals with the above mentioned fact and sets up the
          window_widget pointers correctly. this hackish magic can be eliminated
          when the code that creates the windows was moved over here AND the
          calling code is fixed to create the windows in different order. */
/** \brief Create a toplevel window to represent a video canvas.
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
 * \warning The "meaning" of the window depends on how many times the
 *          function has been called. On a C128, the first call
 *          produces the VDC window and the second produces the
 *          primary window. On all other machines, the first call
 *          produces the primary window. All subsequent calls will
 *          replace or leak the "monitor" window, but the nature of
 *          monitor windows is such that this should never happen.
 */
void ui_create_toplevel_window(struct video_canvas_s *canvas) {
    GtkWidget *new_window, *grid, *new_drawing_area, *status_bar;
    int target_window;

    new_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    grid = gtk_grid_new();
    new_drawing_area = gtk_drawing_area_new();
    status_bar = ui_statusbar_create();

    canvas->drawing_area = new_drawing_area;

    gtk_container_add(GTK_CONTAINER(new_window), grid);
    /* When we have a menu bar, we'll add it at the top here */
    gtk_orientable_set_orientation(GTK_ORIENTABLE(grid), GTK_ORIENTATION_VERTICAL);
    gtk_container_add(GTK_CONTAINER(grid), new_drawing_area);
    gtk_container_add(GTK_CONTAINER(grid), status_bar);

    gtk_widget_set_hexpand(new_drawing_area, TRUE);
    gtk_widget_set_vexpand(new_drawing_area, TRUE);

    g_signal_connect(new_window, "destroy", G_CALLBACK(window_destroy_cb), NULL);

    /* We've defaulted to PRIMARY_WINDOW. C128, however, gets its VDC
     * window created first, so shunt this window to secondary status
     * if that is what it is. */
    target_window = PRIMARY_WINDOW;
    if (machine_class == VICE_MACHINE_C128 && ui_resources.window_widget[SECONDARY_WINDOW] == NULL) {
        target_window = SECONDARY_WINDOW;
    }
    /* Recreated canvases go to MONITOR_WINDOW. */
    if (ui_resources.window_widget[target_window] != NULL) {
        /* TODO: This doesn't make even a little bit of sense. The monitor
         * window doesn't have a Commodore-screen canvas associated with
         * it! Monitors should be tracked completely seperately. */
        /* TODO: Ending up here should be a fatal error */
        target_window = MONITOR_WINDOW;
    }

    ui_resources.canvas[target_window] = canvas;
    ui_resources.window_widget[target_window] = new_window;

    /* gtk_window_set_title(GTK_WINDOW(new_window), canvas->viewport->title); */
    ui_display_speed(100.0f, 0.0f, 0); /* initial update of the window status bar */
/* commented out for now, since this doesn't work properly yet. better avoid
 * introducting weird bugs
 */
#if 0
    add_accelerators_to_window(new_window);
#endif
    kbd_connect_handlers(new_window, NULL);
}

/** \brief  Finds the window associated with this canvas and makes it visible. */

void ui_display_toplevel_window(struct video_canvas_s *canvas)
{
    int i;
    for (i = 0; i < NUM_WINDOWS; ++i) {
        if (ui_resources.canvas[i] == canvas) {
            /* Normally this would show everything in the window,
             * including hidden status bar displays, but we've
             * disabled secondary displays in the status bar code with
             * gtk_widget_set_no_show_all(). */
            gtk_widget_show_all(ui_resources.window_widget[i]);
            break;
        }
    }
}


/** \brief  Signals the html_browser_command field of the resource got allocated
 */
static int html_browser_command_set = 0;

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

int ui_cmdline_options_init(void)
{
    INCOMPLETE_IMPLEMENTATION();
    return cmdline_register_options(cmdline_options_common);
}

char *ui_get_file(const char *format, ...)
{
    NOT_IMPLEMENTED();
    return NULL;
}

int ui_init(int *argc, char **argv)
{
    gtk_init(argc, &argv);
    ui_statusbar_init();
    INCOMPLETE_IMPLEMENTATION();
    return 0;
}

int ui_init_finalize(void)
{
    int status = 0;
#if 0
    char *argv[] = { "x64", NULL };
#endif

    VICE_GTK3_FUNC_ENTERED();
    NOT_IMPLEMENTED_WARN_ONLY();
    return status;
}

int ui_init_finish(void)
{
    return 0;
}

ui_jam_action_t ui_jam_dialog(const char *format, ...)
{
    NOT_IMPLEMENTED();
    return UI_JAM_NONE;
}


/** \brief  Initialize resources related to the UI in general
 *
 * \return  0 on success, < 0 on failure
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

void ui_shutdown(void)
{
    ui_statusbar_shutdown();
}

void ui_update_menus(void)
{
    /* allows autostart to work */
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_dispatch_next_event(void) {
    gtk_main_iteration();
}

void ui_dispatch_events(void)
{
    while (gtk_events_pending()) {
        ui_dispatch_next_event();
    }
}

int ui_extend_image_dialog(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

void ui_error(const char *format, ...)
{
    va_list ap;
    char str[1024];

    va_start(ap, format);
    vsprintf(str, format, ap);
    va_end(ap);
    fprintf(stderr, "VICE Error!:%s\n", str);
    TEMPORARY_IMPLEMENTATION();
}

void ui_message(const char *format, ...)
{
    NOT_IMPLEMENTED();
}

/* display FPS (and some other stuff) in the title bar of the window(s) */
void ui_display_speed(float percent, float framerate, int warp_flag)
{
    int i;
    char str[128];
    int percent_int = (int)(percent + 0.5);
    int framerate_int = (int)(framerate + 0.5);
    char *warp, *mode[3] = {"", _(" (VDC)"), _(" (Monitor)")};

    for (i = 0; i < NUM_WINDOWS; i++) {
        if (ui_resources.canvas[i] && GTK_WINDOW(ui_resources.window_widget[i])) {
            /* FIXME: handle paused mode */
            warp = (warp_flag ? _("(warp)") : "");
            str[0] = 0;
            snprintf(str, 128, "%s%s - %3d%%, %2d fps %s",
                     ui_resources.canvas[i]->viewport->title, mode[i], percent_int, framerate_int, warp);
            str[127] = 0;
            gtk_window_set_title(GTK_WINDOW(ui_resources.window_widget[i]), str);
        }
    }
}

void ui_pause_emulation(int flag)
{
    NOT_IMPLEMENTED();
}

int ui_emulation_is_paused(void)
{
    INCOMPLETE_IMPLEMENTATION();
    return 0;
}

void ui_exit(void)
{
    /* TODO: Confirmation dialog, etc. */
    INCOMPLETE_IMPLEMENTATION();
    exit(0);
}
