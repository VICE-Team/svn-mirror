/*
 * uifilereq.c - Common SDL file selection dialog functions.
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
#include "types.h"

#include <SDL/SDL.h>
#include <string.h>

#include "archdep.h"
#include "ioutil.h"
#include "lib.h"
#include "ui.h"
#include "uimenu.h"
#include "uifilereq.h"
#include "util.h"

static ui_menu_filereq_mode_t filereq_mode;
static menu_draw_t *menu_draw;

/* ------------------------------------------------------------------ */
/* static functions */

static char* sdl_ui_get_file_selector_entry(ioutil_dir_t *directory, int offset, int *isdir)
{
    if (offset >= (directory->dir_amount + directory->file_amount + 2)) {
        return NULL;
    }

    if ((filereq_mode == FILEREQ_MODE_CHOOSE_FILE) && (offset == 0)) {
        *isdir = 0;
        return "<enter filename>";
    }

    if ((filereq_mode == FILEREQ_MODE_CHOOSE_DIR) && (offset == 0)) {
        *isdir = 0;
        return "<choose current directory>";
    }

    if (offset == 1) {
        *isdir = 0;
        return "<enter path>";
    }

    if (offset >= (directory->dir_amount + 2)) {
        *isdir = 0;
        return directory->files[offset - (directory->dir_amount + 2)].name;
    } else {
        *isdir = 1;
        return directory->dirs[offset - 2].name;
    }
}

#if (FSDEV_DIR_SEP_CHR=='\\')
static void sdl_ui_print_translate_seperator(const char *text, int x, int y)
{
    unsigned int len;
    unsigned int i;
    char *new_text = NULL;

    len = strlen(text);
    new_text = lib_stralloc(text);

    for (i=0; i<len; i++) {
        if (new_text[i] == '\\') {
            new_text[i] = '/';
        }
    }
    sdl_ui_print(new_text, x, y);
    lib_free(new_text);
}
#endif

static void sdl_ui_display_path(const char *current_dir)
{
    int len;
    char *text = NULL;
    char *temp = NULL;
    int before = 0;
    int after = 0;
    int pos = 0;
    int amount = 0;
    int i;

    len = strlen(current_dir);

    if (len > menu_draw->max_text_x) {
        text = lib_stralloc(current_dir);
        
        temp = strchr(current_dir + 1, FSDEV_DIR_SEP_CHR);
        before = temp - current_dir + 1;

        while (temp != NULL) {
            amount++;
            temp = strchr(temp + 1, FSDEV_DIR_SEP_CHR);
        }

        while(text[len - after] != FSDEV_DIR_SEP_CHR) {
            after++;
        }

        if (amount > 1 && (before + after + 3) < menu_draw->max_text_x) {

            temp = strchr(current_dir + 1, FSDEV_DIR_SEP_CHR);
            while (((temp - current_dir + 1) < (menu_draw->max_text_x - after - 3)) && temp != NULL) {
                before = temp - current_dir + 1;
                temp = strchr(temp + 1, FSDEV_DIR_SEP_CHR);
            }
        } else {
            before = (menu_draw->max_text_x - 3) /2;
            after = len - (len - menu_draw->max_text_x) - before - 3;
        }
        pos = len - after;
        text[before] = '.';
        text[before + 1] = '.';
        text[before + 2] = '.';
        for (i = 0; i < after; i++) {
            text[before + 3 + i] = text[pos + i];
        }
        text[before + 3 + after] = 0;
#if (FSDEV_DIR_SEP_CHR=='\\')
        sdl_ui_print_translate_seperator(text, 0, 2);
#else
        sdl_ui_print(text, 0, 2);
#endif
    } else {
#if (FSDEV_DIR_SEP_CHR=='\\')
        sdl_ui_print_translate_seperator(current_dir, 0, 2);
#else
        sdl_ui_print(current_dir, 0, 2);
#endif
    }

    lib_free(text);
}

static void sdl_ui_file_selector_redraw(ioutil_dir_t *directory, const char *title, const char *current_dir, int offset, int num_items, int more)
{
    int i, j, isdir = 0;
    char* title_string;
    char* name;

    title_string = lib_malloc(strlen(title) + 9);
    sprintf(title_string, "%s %s", title, (offset) ? ((more) ? "(<- ->)" : "(<-)") : ((more) ? "(->)" : ""));

    sdl_ui_clear();
    sdl_ui_display_title(title_string);
    lib_free(title_string);
    sdl_ui_display_path(current_dir);

    for (i = 0; i < num_items; ++i) {
        j = MENU_FIRST_X;
        name = sdl_ui_get_file_selector_entry(directory, offset + i, &isdir);
        if (isdir) {
            j += 1 + sdl_ui_print("(D)", MENU_FIRST_X, i + MENU_FIRST_Y + 2);
        }
        sdl_ui_print(name, j, i + MENU_FIRST_Y + 2);
    }
}

/* ------------------------------------------------------------------ */
/* External UI interface */

char* sdl_ui_file_selection_dialog(const char* title, ui_menu_filereq_mode_t mode)
{
    int total, dirs, files, menu_max;
    int active = 1;
    int offset = 0;
    int redraw = 1;
    char *retval = NULL;
    int cur = 0, cur_old = -1;
    ioutil_dir_t *directory;
    char *current_dir;
    char *backup_dir;
    char *inputstring;
    unsigned int maxpathlen;

    menu_draw = sdl_ui_get_menu_param();
    filereq_mode = mode;

    maxpathlen = ioutil_maxpathlen();

    current_dir = lib_malloc(maxpathlen);

    ioutil_getcwd(current_dir, maxpathlen);
    backup_dir = lib_stralloc(current_dir);

    directory = ioutil_opendir(".");
    if (directory == NULL) {
        return NULL;
    }

    dirs = directory->dir_amount;
    files = directory->file_amount;
    total = dirs + files + 2;
    menu_max = menu_draw->max_text_y - (MENU_FIRST_Y + 2);

    while(active) {
        if (redraw) {
            sdl_ui_file_selector_redraw(directory, title, current_dir, offset, (total-offset > menu_max) ? menu_max : total-offset, (total - offset > menu_max) ? 1 : 0);
            redraw = 0;
        }
        sdl_ui_display_cursor((cur + 2) , (cur_old == -1) ? -1 : (cur_old + 2));
        sdl_ui_refresh();

        switch (sdl_ui_menu_poll_input()) {
            case MENU_ACTION_UP:
                if (cur > 0) {
                    cur_old = cur;
                    --cur;
                } else {
                    if (offset > 0) {
                        offset--;
                        redraw = 1;
                    }
                }
                break;
            case MENU_ACTION_LEFT:
                offset -= menu_max;
                if (offset < 0) {
                    offset = 0;
                    cur_old = -1;
                    cur = 0;
                }
                redraw = 1;
                break;
            case MENU_ACTION_DOWN:
                if (cur < (menu_max - 1)) {
                    if ((cur+offset) < total - 1) {
                        cur_old = cur;
                        ++cur;
                    }
                } else {
                    if (offset < (total - menu_max)) {
                        offset++;
                        redraw = 1;
                    }
                }
                break;
            case MENU_ACTION_RIGHT:
                offset += menu_max;
                if (offset >= total) {
                    offset = total - 1;
                    cur_old = -1;
                    cur = 0;
                } else if ((cur+offset) >= total) {
                    cur_old = -1;
                    cur = total-offset - 1;
                }
                redraw = 1;
                break;
            case MENU_ACTION_SELECT:
                switch (offset+cur) {
                    case 0:
                        if (filereq_mode == FILEREQ_MODE_CHOOSE_FILE) {
                            inputstring = sdl_ui_text_input_dialog("Enter filename", NULL);
                            if (inputstring == NULL) {
                                redraw = 1;
                            } else {
                                if (archdep_path_is_relative(inputstring) && (strchr(inputstring, FSDEV_DIR_SEP_CHR) != NULL)) {
                                    retval = inputstring;
                                } else {
                                    retval = util_concat(current_dir, FSDEV_DIR_SEP_STR, inputstring, NULL);
                                    lib_free(inputstring);
                                }
                            }
                        } else {
                            retval = lib_stralloc(current_dir);
                        }
                        active = 0;
                        break;
                    case 1:
                        inputstring = sdl_ui_text_input_dialog("Enter path", NULL);
                        if (inputstring != NULL) {
                            ioutil_chdir(inputstring);
                            lib_free(inputstring);
                            ioutil_closedir(directory);
                            ioutil_getcwd(current_dir, maxpathlen);
                            directory = ioutil_opendir(".");
                            offset = 0;
                            cur_old = -1;
                            cur = 0;
                            dirs = directory->dir_amount;
                            files = directory->file_amount;
                            total = dirs + files + 2;
                        }
                        redraw = 1;
                        break;
                    default:
                        if (offset+cur < (dirs + 2)) {
                            ioutil_chdir(directory->dirs[offset + cur - 2].name);
                            ioutil_closedir(directory);
                            ioutil_getcwd(current_dir, maxpathlen);
                            directory = ioutil_opendir(".");
                            offset = 0;
                            cur_old = -1;
                            cur = 0;
                            dirs = directory->dir_amount;
                            files = directory->file_amount;
                            total = dirs + files + 2;
                            redraw = 1;
                        } else {
                            retval = util_concat(current_dir, FSDEV_DIR_SEP_STR, directory->files[offset + cur - dirs - 2].name, NULL);
                            active = 0;
                        }
                        break;
                }
                break;
            case MENU_ACTION_CANCEL:
            case MENU_ACTION_EXIT:
                retval = NULL;
                active = 0;
                ioutil_chdir(backup_dir);
                break;
            default:
                SDL_Delay(10);
                break;
        }
    }
    ioutil_closedir(directory);
    lib_free(current_dir);
    lib_free(backup_dir);

    return retval;
}
