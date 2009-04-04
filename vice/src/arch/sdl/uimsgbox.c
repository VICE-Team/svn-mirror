/*
 * uimsgbox.c - Common SDL message box functions.
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

#include <string.h>
#include <SDL/SDL.h>

#include "lib.h"
#include "resources.h"
#include "sound.h"
#include "ui.h"
#include "uimenu.h"
#include "uimsgbox.h"
#include "vsync.h"

static menu_draw_t *menu_draw;

static int make_28_cols(char *text)
{
    int i = 28;
    int j = 1;

    while (i < (strlen(text) - 1)) {
        while (text[i] != ' ') {
            i--;
        }
        text[i] = 0;
        i += 28;
        j++;
    }
    return j;
}

static int handle_message_box(const char *title, const char *message, int message_mode)
{
    char *text;
    char *pos;
    char template[40];
    int lines, before;
    int active = 1;
    int i, j, x;
    int cur_pos = 0;

    text = lib_stralloc(message);
    pos = text;
    lines = make_28_cols(text);
    sdl_ui_clear();
    sdl_ui_print_center("\260\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\256", 2);
    sprintf(template,"\335                            \335");
    before = (28 - strlen(title))/2;
    for (i = 0; i < strlen(title); i++) {
        template[i + before + 1] = title[i];
    }
    sdl_ui_print_center(template, 3);
    sdl_ui_print_center("\253\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\263", 4);
    for (j = 0; j < lines; j++) {
        sprintf(template,"\335                            \335");
        before = (28 - strlen(pos))/2;
        for (i = 0; i < strlen(pos); i++) {
            template[i + before + 1] = pos[i];
        }
        sdl_ui_print_center(template, j + 5);

        pos += strlen(pos) + 1;
    }
    sdl_ui_print_center("\335                            \335", j + 5);
    switch (message_mode) {
        case MESSAGE_OK:
            sdl_ui_print_center("\335            \260\300\300\256            \335", j + 6);
            x = sdl_ui_print_center("\335            \335OK\335            \335", j + 7);
            sdl_ui_print_center("\335            \255\300\300\275            \335", j + 8);
            break;
        case MESSAGE_YESNO:
            sdl_ui_print_center("\335      \260\300\300\300\256       \260\300\300\256      \335", j + 6);
            x = sdl_ui_print_center("\335      \335YES\335       \335NO\335      \335", j + 7);
            sdl_ui_print_center("\335      \255\300\300\300\275       \255\300\300\275      \335", j + 8);
            break;
        case MESSAGE_CPUJAM:
        default:
            sdl_ui_print_center("\335 \260\300\300\300\300\300\256  \260\300\300\300\300\300\300\300\256  \260\300\300\300\300\256 \335", j + 6);
            x = sdl_ui_print_center("\335 \335RESET\335  \335MONITOR\335  \335CONT\335 \335", j + 7);
            sdl_ui_print_center("\335 \255\300\300\300\300\300\275  \255\300\300\300\300\300\300\300\275  \255\300\300\300\300\275 \335", j + 8);
            break;
    }
    sdl_ui_print_center("\255\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\300\275", j + 9);
    lib_free(text);
    x += (menu_draw->max_text_x - 30) / 2;
    while (active) {
        switch (message_mode) {
            case MESSAGE_OK:
                sdl_ui_reverse_colors();
                sdl_ui_print_center("OK", j + 7);
                sdl_ui_reverse_colors();
                break;
            case MESSAGE_YESNO:
                if (cur_pos == 0) {
                    sdl_ui_reverse_colors();
                }
                sdl_ui_print("YES", x - 22, j + 7);
                sdl_ui_reverse_colors();
                sdl_ui_print("NO", x - 10, j + 7);
                if (cur_pos == 1) {
                    sdl_ui_reverse_colors();
                }
                break;
           case MESSAGE_CPUJAM:
           default:
                if (cur_pos == 0) {
                    sdl_ui_reverse_colors();
                }
                sdl_ui_print("RESET", x - 27, j + 7);
                if (cur_pos < 2) {
                    sdl_ui_reverse_colors();
                }
                sdl_ui_print("MONITOR", x - 18, j + 7);
                if (cur_pos > 0) {
                    sdl_ui_reverse_colors();
                }
                sdl_ui_print("CONT", x - 7, j + 7);
                if (cur_pos == 2) {
                    sdl_ui_reverse_colors();
                }
                break;
        }

        sdl_ui_refresh();

        switch (sdl_ui_menu_poll_input()) {
            case MENU_ACTION_CANCEL:
            case MENU_ACTION_EXIT:
                cur_pos = 0;
                active = 0;
                break;
            case MENU_ACTION_SELECT:
                active = 0;
                break;
            case MENU_ACTION_LEFT:
            case MENU_ACTION_UP:
                if (message_mode != MESSAGE_OK) {
                    cur_pos--;
                    if (cur_pos < 0) {
                        cur_pos = message_mode;
                    }
                }
                break;
            case MENU_ACTION_RIGHT:
            case MENU_ACTION_DOWN:
                if (message_mode != MESSAGE_OK) {
                    cur_pos++;
                    if (cur_pos > message_mode) {
                        cur_pos = 0;
                    }
                }
                break;
            default:
                SDL_Delay(10);
                break;
        }
    }
    return cur_pos;
}

static int activate_dialog(const char *title, const char *message, int message_mode)
{
    int retval;

    sdl_ui_activate_pre_action();
    retval = handle_message_box(title, message, message_mode);
    sdl_ui_activate_post_action();

    return retval;
}

int message_box(const char *title, char *message, int message_mode)
{
    sdl_ui_init_draw_params();

    menu_draw = sdl_ui_get_menu_param();

    if (!sdl_menu_state) {
        return activate_dialog(title, (const char *)message, message_mode);
    }
    return handle_message_box(title, (const char *)message, message_mode);
}
