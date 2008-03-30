/*
 * uiscreenshot.c - Screenshot save dialog for the MS-DOS version of VICE.
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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "machine.h"
#include "tui.h"
#include "tuimenu.h"
#include "tuifs.h"
#include "ui.h"
#include "uiscreenshot.h"
#include "util.h"
#include "videoarch.h"

#define SCREENSHOT_BMP_EXTENSION	"bmp"

#ifdef HAVE_GIF
#define SCREENSHOT_GIF_EXTENSION	"gif"
#endif

#ifdef HAVE_PNG
#define SCREENSHOT_PNG_EXTENSION	"png"
#endif


extern video_canvas_t *last_canvas;

static TUI_MENU_CALLBACK(bmp_file_name_callback);

static TUI_MENU_CALLBACK(write_bmp_screenshot_callback);

#ifdef HAVE_GIF
static TUI_MENU_CALLBACK(write_gif_screenshot_callback);
static TUI_MENU_CALLBACK(gif_file_name_callback);
#endif

#ifdef HAVE_PNG
static TUI_MENU_CALLBACK(write_png_screenshot_callback);
static TUI_MENU_CALLBACK(png_file_name_callback);
#endif

static char *file_name = NULL;

static tui_menu_item_def_t write_bmp_screenshot_menu_def[] = {
    { "_File name:",
      "Specify BMP screenshot file name",
      bmp_file_name_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Do it!",
      "Save BMP screenshot with the specified name",
      write_bmp_screenshot_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

#ifdef HAVE_GIF
static tui_menu_item_def_t write_gif_screenshot_menu_def[] = {
    { "_File name:",
      "Specify GIF screenshot file name",
      gif_file_name_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Do it!",
      "Save GIF screenshot with the specified name",
      write_gif_screenshot_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};
#endif

#ifdef HAVE_PNG
static tui_menu_item_def_t write_png_screenshot_menu_def[] = {
    { "_File name:",
      "Specify PNG screenshot file name",
      png_file_name_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Do it!",
      "Save PNG screenshot with the specified name",
      write_png_screenshot_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};
#endif

tui_menu_item_def_t ui_screenshot_menu_def[] = {
    { "_Write BMP Screenshot",
      "Write a BMP screenshot file",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, write_bmp_screenshot_menu_def, NULL },
#ifdef HAVE_GIF
    { "W_rite GIF Screenshot",
      "Write a GIF screenshot file",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, write_gif_screenshot_menu_def, NULL },
#endif
#ifdef HAVE_PNG
    { "W_rite PNG Screenshot",
      "Write a PNG screenshot file",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, write_png_screenshot_menu_def, NULL },
#endif
    { NULL }
};

static char *screenshot_selector(const char *title, char *extension)
{
    return tui_file_selector(title, NULL, extension, NULL, 0, NULL, 0, NULL,
                             NULL);
}

static TUI_MENU_CALLBACK(bmp_file_name_callback)
{
    if (been_activated) {
        char new_file_name[PATH_MAX];

        if (file_name == NULL)
            memset(new_file_name, 0, PATH_MAX);
        else
            strcpy(new_file_name, file_name);

        while (tui_input_string("Save BMP screenshot", "Enter file name:",
                                new_file_name, PATH_MAX) != -1) {
            util_remove_spaces(new_file_name);
            if (*new_file_name == 0) {
                char *tmp;

                tmp = screenshot_selector("Save BMP screenshot file", "*.bmp");
                if (tmp != NULL) {
                    strcpy(new_file_name, tmp);
                    lib_free(tmp);
                }
            } else {
                char *extension;
                char *last_dot;

                last_dot = strrchr(new_file_name, '.');

                if (last_dot == NULL) {
                    extension = SCREENSHOT_BMP_EXTENSION;
                } else {
                    char *last_slash, *last_backslash, *last_path_separator;

                    last_slash = strrchr(new_file_name, '/');
                    last_backslash = strrchr(new_file_name, '\\');

                    if (last_slash == NULL)
                        last_path_separator = last_backslash;
                    else if (last_backslash == NULL)
                        last_path_separator = last_slash;
                    else if (last_backslash < last_slash)
                        last_path_separator = last_slash;
                    else
                        last_path_separator = last_backslash;

                    if (last_path_separator == NULL
                        || last_path_separator < last_dot)
                        extension = "";
                    else
                        extension = SCREENSHOT_BMP_EXTENSION;
                }
                
                if (file_name == NULL) {
                    file_name = util_concat(new_file_name, ".", extension,
                                            NULL);
                } else {
                    lib_free(file_name);
                    file_name = lib_stralloc(new_file_name);
                    util_add_extension(&file_name, extension);
                }
                break;
            }
        }
    }

    return file_name;
}

#ifdef HAVE_GIF
static TUI_MENU_CALLBACK(gif_file_name_callback)
{
    if (been_activated) {
        char new_file_name[PATH_MAX];

        if (file_name == NULL)
            memset(new_file_name, 0, PATH_MAX);
        else
            strcpy(new_file_name, file_name);

        while (tui_input_string("Save GIF screenshot", "Enter file name:",
                                new_file_name, PATH_MAX) != -1) {
            util_remove_spaces(new_file_name);
            if (*new_file_name == 0) {
                char *tmp;

                tmp = screenshot_selector("Save GIF screenshot file", "*.gif");
                if (tmp != NULL) {
                    strcpy(new_file_name, tmp);
                    lib_free(tmp);
                }
            } else {
                char *extension;
                char *last_dot;

                last_dot = strrchr(new_file_name, '.');

                if (last_dot == NULL) {
                    extension = SCREENSHOT_GIF_EXTENSION;
                } else {
                    char *last_slash, *last_backslash, *last_path_separator;

                    last_slash = strrchr(new_file_name, '/');
                    last_backslash = strrchr(new_file_name, '\\');

                    if (last_slash == NULL)
                        last_path_separator = last_backslash;
                    else if (last_backslash == NULL)
                        last_path_separator = last_slash;
                    else if (last_backslash < last_slash)
                        last_path_separator = last_slash;
                    else
                        last_path_separator = last_backslash;

                    if (last_path_separator == NULL
                        || last_path_separator < last_dot)
                        extension = "";
                    else
                        extension = SCREENSHOT_GIF_EXTENSION;
                }
                
                if (file_name == NULL) {
                    file_name = util_concat(new_file_name, ".", extension,
                                            NULL);
                } else {
                    lib_free(file_name);
                    file_name = lib_stralloc(new_file_name);
                    util_add_extension(&file_name, extension);
                }
                break;
            }
        }
    }

    return file_name;
}
#endif

#ifdef HAVE_PNG
static TUI_MENU_CALLBACK(png_file_name_callback)
{
    if (been_activated) {
        char new_file_name[PATH_MAX];

        if (file_name == NULL)
            memset(new_file_name, 0, PATH_MAX);
        else
            strcpy(new_file_name, file_name);

        while (tui_input_string("Save PNG screenshot", "Enter file name:",
                                new_file_name, PATH_MAX) != -1) {
            util_remove_spaces(new_file_name);
            if (*new_file_name == 0) {
                char *tmp;

                tmp = screenshot_selector("Save PNG screenshot file", "*.png");
                if (tmp != NULL) {
                    strcpy(new_file_name, tmp);
                    lib_free(tmp);
                }
            } else {
                char *extension;
                char *last_dot;

                last_dot = strrchr(new_file_name, '.');

                if (last_dot == NULL) {
                    extension = SCREENSHOT_PNG_EXTENSION;
                } else {
                    char *last_slash, *last_backslash, *last_path_separator;

                    last_slash = strrchr(new_file_name, '/');
                    last_backslash = strrchr(new_file_name, '\\');

                    if (last_slash == NULL)
                        last_path_separator = last_backslash;
                    else if (last_backslash == NULL)
                        last_path_separator = last_slash;
                    else if (last_backslash < last_slash)
                        last_path_separator = last_slash;
                    else
                        last_path_separator = last_backslash;

                    if (last_path_separator == NULL
                        || last_path_separator < last_dot)
                        extension = "";
                    else
                        extension = SCREENSHOT_PNG_EXTENSION;
                }
                
                if (file_name == NULL) {
                    file_name = util_concat(new_file_name, ".", extension,
                                            NULL);
                } else {
                    lib_free(file_name);
                    file_name = lib_stralloc(new_file_name);
                    util_add_extension(&file_name, extension);
                }
                break;
            }
        }
    }

    return file_name;
}
#endif

static TUI_MENU_CALLBACK(write_bmp_screenshot_callback)
{
    if (been_activated) {
        if (file_name == NULL || *file_name == 0) {
            tui_error("Specify a file name first.");
            return NULL;
        }

        if (!util_file_exists(file_name)
            || tui_ask_confirmation("The specified file already exists.  "
                                    "Replace?  (Y/N)")) {
            if (    screenshot_save("BMP", file_name, last_canvas) < 0)
                tui_error("Cannot save BMP screenshot.");
            else
                tui_message("BMP screenshot saved successfully.");
        }
    }

    return NULL;
}

#ifdef HAVE_GIF
static TUI_MENU_CALLBACK(write_gif_screenshot_callback)
{
    if (been_activated) {
        if (file_name == NULL || *file_name == 0) {
            tui_error("Specify a file name first.");
            return NULL;
        }

        if (!util_file_exists(file_name)
            || tui_ask_confirmation("The specified file already exists.  "
                                    "Replace?  (Y/N)")) {
            if (    screenshot_save("GIF", file_name, last_canvas) < 0)
                tui_error("Cannot save GIF screenshot.");
            else
                tui_message("GIF screenshot saved successfully.");
        }
    }

    return NULL;
}
#endif

#ifdef HAVE_PNG
static TUI_MENU_CALLBACK(write_png_screenshot_callback)
{
    if (been_activated) {
        if (file_name == NULL || *file_name == 0) {
            tui_error("Specify a file name first.");
            return NULL;
        }

        if (!util_file_exists(file_name)
            || tui_ask_confirmation("The specified file already exists.  "
                                    "Replace?  (Y/N)")) {
            if (    screenshot_save("PNG", file_name, last_canvas) < 0)
                tui_error("Cannot save PNG screenshot.");
            else
                tui_message("PNG screenshot saved successfully.");
        }
    }

    return NULL;
}
#endif
