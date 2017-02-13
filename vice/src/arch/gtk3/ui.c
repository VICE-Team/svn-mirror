/*
 * ui.c - Native GTK3 UI stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "not_implemented.h"

#include "lib.h"
#include "resources.h"
#include "uiapi.h"
#include "util.h"


/** \brief  Default HTML browser
 *
 * \todo    Needs ifdef's for different archs
 */
#define HTML_BROWSER_COMMAND_DEFAULT    "firefox %s"


typedef struct ui_resources_s {
    char *html_browser_command;
    int save_resources_on_exit;
    int confirm_on_exit;
} ui_resource_t;


static ui_resource_t ui_resources;


/** \brief  Signals the html_browser_command field of the resource got allocated
 */
static int html_browser_command_set = 0;

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


/** \brief  Set ConfirmOnExit resource
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


/** \brief  String type resources list
 */
static const resource_string_t resources_string[] = {
    { "HTMLBrowserCommand", HTML_BROWSER_COMMAND_DEFAULT, RES_EVENT_NO, NULL,
        &ui_resources.html_browser_command, set_html_browser_command, NULL },
    RESOURCE_STRING_LIST_END
};


/** \brief  Integer/Boolean type resources list
 */
static const resource_int_t resources_int[] = {
    { "SaveResourcesOnExit", 0, RES_EVENT_NO, NULL,
        &ui_resources.save_resources_on_exit, set_save_resources_on_exit, NULL },
    { "ConfirmOnExit", 1, RES_EVENT_NO, NULL,
        &ui_resources.confirm_on_exit, set_confirm_on_exit, NULL },
    RESOURCE_INT_LIST_END
};



int ui_cmdline_options_init(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

void ui_error(const char *format, ...)
{
    NOT_IMPLEMENTED();
}

char *ui_get_file(const char *format, ...)
{
    NOT_IMPLEMENTED();
    return NULL;
}

int ui_init(int *argc, char **argv)
{
    NOT_IMPLEMENTED();
    return 0;
}

int ui_init_finalize(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

int ui_init_finish(void)
{
    NOT_IMPLEMENTED();
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
    /* initialize string resources */
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }
    /* initialize int/bool resources */
    if (resources_register_int(resources_int) < 0) {
        return -1;
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
    NOT_IMPLEMENTED();
}

void ui_update_menus(void)
{
    NOT_IMPLEMENTED();
}

void ui_dispatch_events(void)
{
    NOT_IMPLEMENTED();
}

int ui_extend_image_dialog(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

void ui_message(const char *format, ...)
{
    NOT_IMPLEMENTED();
}

