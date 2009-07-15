/*
 * ui.c - Common UI routines.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
 *
 * Based on code by
 *  Andreas Boose <viceteam@t-online.de>
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

#include <SDL/SDL.h>
#include <stdio.h>

#include "color.h"
#include "fullscreenarch.h"
#include "joy.h"
#include "kbd.h"
#include "lib.h"
#include "lightpen.h"
#include "mouse.h"
#include "mousedrv.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uiapi.h"
#include "uicolor.h"
#include "uimenu.h"
#include "uimsgbox.h"
#include "uistatusbar.h"
#include "videoarch.h"
#include "vkbd.h"
#include "vsync.h"

#ifndef SDL_DISABLE
#define SDL_DISABLE SDL_IGNORE
#endif

static int sdl_ui_ready = 0;

/* ----------------------------------------------------------------- */
/* ui.h */

/* Misc. SDL event handling */
void ui_handle_misc_sdl_event(SDL_Event e)
{
    switch (e.type) {
        case SDL_QUIT:
            ui_sdl_quit();
            break;
        case SDL_VIDEORESIZE:
            sdl_video_resize((unsigned int)e.resize.w, (unsigned int)e.resize.h);
            break;
        case SDL_VIDEOEXPOSE:
            video_canvas_refresh_all(sdl_active_canvas);
            break;
        default:
/*fprintf(stderr,"%s: %i\n",__func__,e.type);*/
            break;
    }
}

void ui_dispatch_next_event(void)
{
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        ui_handle_misc_sdl_event(e);
    }
}

#ifdef WATCOM_COMPILE
typedef struct watcom_scancode_s {
    Uint8 code;
    SDLKey key;
} watcom_scancode_t;

static watcom_scancode_t watcom_scancode_table[] = {
    {  1, SDLK_ESCAPE },
    {  2, SDLK_1 },
    {  3, SDLK_2 },
    {  4, SDLK_3 },
    {  5, SDLK_4 },
    {  6, SDLK_5 },
    {  7, SDLK_6 },
    {  8, SDLK_7 },
    {  9, SDLK_8 },
    { 10, SDLK_9 },
    { 11, SDLK_0 },
    { 12, SDLK_MINUS },
    { 13, SDLK_EQUALS },
    { 14, SDLK_BACKSPACE },
    { 15, SDLK_TAB },
    { 16, SDLK_q },
    { 17, SDLK_w },
    { 18, SDLK_e },
    { 19, SDLK_r },
    { 20, SDLK_t },
    { 21, SDLK_y },
    { 22, SDLK_u },
    { 23, SDLK_i },
    { 24, SDLK_o },
    { 25, SDLK_p },
    { 26, SDLK_LEFTBRACKET },
    { 27, SDLK_RIGHTBRACKET },
    { 28, SDLK_RETURN },
    { 29, SDLK_LCTRL },
    { 30, SDLK_a },
    { 31, SDLK_s },
    { 32, SDLK_d },
    { 33, SDLK_f },
    { 34, SDLK_g },
    { 35, SDLK_h },
    { 36, SDLK_j },
    { 37, SDLK_k },
    { 38, SDLK_l },
    { 39, SDLK_SEMICOLON },
    { 40, SDLK_QUOTE },
    { 41, SDLK_BACKQUOTE },
    { 42, SDLK_LSHIFT },
    { 43, SDLK_BACKSLASH },
    { 44, SDLK_z },
    { 45, SDLK_x },
    { 46, SDLK_c },
    { 47, SDLK_v },
    { 48, SDLK_b },
    { 49, SDLK_n },
    { 50, SDLK_m },
    { 51, SDLK_COMMA },
    { 52, SDLK_PERIOD },
    { 53, SDLK_SLASH },
    { 54, SDLK_RSHIFT },
    { 55, SDLK_PRINT },
    { 56, SDLK_LALT },
    { 57, SDLK_SPACE },
    { 58, SDLK_CAPSLOCK },
    { 59, SDLK_F1 },
    { 60, SDLK_F2 },
    { 61, SDLK_F3 },
    { 62, SDLK_F4 },
    { 63, SDLK_F5 },
    { 64, SDLK_F6 },
    { 65, SDLK_F7 },
    { 66, SDLK_F8 },
    { 67, SDLK_F9 },
    { 68, SDLK_F10 },
    { 69, SDLK_PAUSE },
    { 71, SDLK_HOME },
    { 71, SDLK_KP7 },
    { 72, SDLK_UP },
    { 73, SDLK_PAGEUP },
    { 74, SDLK_KP_MINUS },
    { 75, SDLK_LEFT },
    { 76, SDLK_KP5 },
    { 77, SDLK_RIGHT },
    { 78, SDLK_KP_PLUS },
    { 79, SDLK_END },
    { 80, SDLK_DOWN },
    { 81, SDLK_PAGEDOWN },
    { 82, SDLK_INSERT },
    { 83, SDLK_DELETE },
    { 87, SDLK_F11 },
    { 88, SDLK_F12 },
    { 91, SDLK_LSUPER },
    { 92, SDLK_RSUPER },
    { 93, SDLK_MENU },
    {  0, SDLK_UNKNOWN }
};

static SDLKey watcom_scancode_translate(Uint8 scancode)
{
    int i = 0;
    SDLKey retval = 0;

    for (i = 0; watcom_scancode_table[i].code != 0 && retval == 0; i++) {
        if (watcom_scancode_table[i].code == scancode) {
            retval = watcom_scancode_table[i].key;
        }
    }
    return retval;
}
#endif

/* Main event handler */
ui_menu_action_t ui_dispatch_events(void)
{
    SDL_Event e;
    ui_menu_action_t retval = MENU_ACTION_NONE;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_KEYDOWN:
#ifdef WATCOM_COMPILE
                retval = sdlkbd_press(watcom_scancode_translate(e.key.keysym.scancode), e.key.keysym.mod);
#else
                retval = sdlkbd_press(e.key.keysym.sym, e.key.keysym.mod);
#endif
                break;
            case SDL_KEYUP:
#ifdef WATCOM_COMPILE
                retval = sdlkbd_release(watcom_scancode_translate(e.key.keysym.scancode), e.key.keysym.mod);
#else
                retval = sdlkbd_release(e.key.keysym.sym, e.key.keysym.mod);
#endif
                break;
#ifdef HAVE_SDL_NUMJOYSTICKS
            case SDL_JOYAXISMOTION:
                retval = sdljoy_axis_event(e.jaxis.which, e.jaxis.axis, e.jaxis.value);
                break;
            case SDL_JOYBUTTONDOWN:
                retval = sdljoy_button_event(e.jbutton.which, e.jbutton.button, 1);
                break;
            case SDL_JOYBUTTONUP:
                retval = sdljoy_button_event(e.jbutton.which, e.jbutton.button, 0);
                break;
            case SDL_JOYHATMOTION:
                retval = sdljoy_hat_event(e.jhat.which, e.jhat.hat, e.jhat.value);
                break;
#endif
            case SDL_MOUSEMOTION:
                if (_mouse_enabled) {
                    mouse_move((int)(e.motion.xrel), (int)(e.motion.yrel));
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (_mouse_enabled) {
                    mouse_button((int)(e.button.button), (e.button.state == SDL_PRESSED));
                }
                break;
            default:
                ui_handle_misc_sdl_event(e);
                break;
        }
        /* When using the menu or vkbd, pass every meaningful event to the caller */
        if (((sdl_menu_state)||(sdl_vkbd_state & SDL_VKBD_ACTIVE)) && (retval != MENU_ACTION_NONE) && (retval != MENU_ACTION_NONE_RELEASE)) {
            break;
        }
    }
    return retval;
}

void ui_check_mouse_cursor(void)
{
    if(_mouse_enabled && !lightpen_enabled && !sdl_menu_state) {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_WM_GrabInput(SDL_GRAB_ON);
    } else {
        SDL_ShowCursor((sdl_active_canvas->fullscreenconfig->enable && !lightpen_enabled)?SDL_DISABLE:SDL_ENABLE);
        SDL_WM_GrabInput(SDL_GRAB_OFF);
    }
}

void archdep_ui_init(int argc, char *argv[])
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
}

void ui_message(const char* format, ...)
{
    va_list ap;
    char *tmp;

    va_start(ap, format);
    tmp = lib_mvsprintf(format,ap);
    va_end(ap);

    if (sdl_ui_ready) {
        message_box("VICE MESSAGE", tmp, MESSAGE_OK);
    } else {
        fprintf(stderr, "%s\n", tmp);
    }
    lib_free(tmp);
}

/* ----------------------------------------------------------------- */
/* uiapi.h */

static int save_resources_on_exit;
static int confirm_on_exit;

static int set_ui_menukey(int val, void *param)
{
    sdl_ui_menukeys[(ui_menu_action_t)param] = val;
    return 0;
}

static int set_save_resources_on_exit(int val, void *param)
{
    save_resources_on_exit = val;
    return 0;
}

static int set_confirm_on_exit(int val, void *param)
{
    confirm_on_exit = val;
    return 0;
}

static const resource_int_t resources_int[] = {
    { "MenuKey", SDLK_F12, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[0], set_ui_menukey, (void *)MENU_ACTION_NONE },
    { "MenuKeyUp", SDLK_UP, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[1], set_ui_menukey, (void *)MENU_ACTION_UP },
    { "MenuKeyDown", SDLK_DOWN, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[2], set_ui_menukey, (void *)MENU_ACTION_DOWN },
    { "MenuKeyLeft", SDLK_LEFT, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[3], set_ui_menukey, (void *)MENU_ACTION_LEFT },
    { "MenuKeyRight", SDLK_RIGHT, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[4], set_ui_menukey, (void *)MENU_ACTION_RIGHT },
    { "MenuKeySelect", SDLK_RETURN, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[5], set_ui_menukey, (void *)MENU_ACTION_SELECT },
    { "MenuKeyCancel", SDLK_BACKSPACE, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[6], set_ui_menukey, (void *)MENU_ACTION_CANCEL },
    { "MenuKeyExit", SDLK_ESCAPE, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[7], set_ui_menukey, (void *)MENU_ACTION_EXIT },
    { "MenuKeyMap", SDLK_m, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[8], set_ui_menukey, (void *)MENU_ACTION_MAP },
    { "SaveResourcesOnExit", 0, RES_EVENT_NO, NULL,
      &save_resources_on_exit, set_save_resources_on_exit, NULL },
    { "ConfirmOnExit", 0, RES_EVENT_NO, NULL,
      &confirm_on_exit, set_confirm_on_exit, NULL },
    { NULL },
};

void ui_sdl_quit(void)
{
    if (confirm_on_exit) {
        if (message_box("VICE QUESTION","Do you really want to exit?", MESSAGE_YESNO) == 1) {
            return;
        }
    }

    if (save_resources_on_exit) {
        if (resources_save(NULL) < 0) {
            ui_error("Cannot save current settings.");
        }
    }
    exit(0);
}

/* Initialization  */
int ui_resources_init(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif

    return resources_register_int(resources_int);
}

void ui_resources_shutdown(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
}

int ui_cmdline_options_init(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif

    return 0;
}

int ui_init(int *argc, char **argv)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
    /* TODO move somewhere else */
    sdlkbd_init_resources();
    uistatusbar_init_resources();

    return 0;
}

int ui_init_finish(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif

    return 0;
}

int ui_init_finalize(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif

    SDL_WM_SetCaption(sdl_active_canvas->viewport->title, "VICE");
    sdl_ui_ready = 1;
    return 0;
}

void ui_shutdown(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif

    /* TODO find a better place */
    kbd_arch_shutdown();
}

/* Print an error message.  */
void ui_error(const char *format,...)
{
    va_list ap;
    char *tmp;

    va_start(ap, format);
    tmp = lib_mvsprintf(format, ap);
    va_end(ap);

    if (sdl_ui_ready) {
        message_box("VICE ERROR", tmp, MESSAGE_OK);
    } else {
        fprintf(stderr, "%s\n", tmp);
    }
    lib_free(tmp);
}


/* Let the user browse for a filename; display format as a titel */
char* ui_get_file(const char *format,...)
{
    return NULL;
}

/* Drive related UI.  */
int ui_extend_image_dialog(void)
{
    if (message_box("VICE QUESTION", "Extend image to 40-track format?", MESSAGE_YESNO) == 0) {
        return 1;
    }
    return 0;
}

/* Show a CPU JAM dialog.  */
ui_jam_action_t ui_jam_dialog(const char *format, ...)
{
    int retval;

    retval = message_box("VICE CPU JAM", "a CPU JAM has occured, choose the action to take", MESSAGE_CPUJAM);
    if (retval == 0) {
        return UI_JAM_HARD_RESET;
    }
    if (retval == 1) {
        return UI_JAM_MONITOR;
    }
    return UI_JAM_NONE;
}

/* Update all menu entries.  */
void ui_update_menus(void){}

/* ----------------------------------------------------------------- */
/* uicolor.h */

int uicolor_alloc_color(unsigned int red, unsigned int green,
                        unsigned int blue, unsigned long *color_pixel,
                        BYTE *pixel_return)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif

    return 0;
}

void uicolor_free_color(unsigned int red, unsigned int green,
                        unsigned int blue, unsigned long color_pixel)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
}

void uicolor_convert_color_table(unsigned int colnr, BYTE *data,
                                 long color_pixel, void *c)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
}

int uicolor_set_palette(struct video_canvas_s *c,
                        const struct palette_s *palette)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif

    return 0;
}
