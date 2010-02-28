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

#include "vice_sdl.h"
#include <stdio.h>

#include "cmdline.h"
#include "color.h"
#include "fullscreenarch.h"
#include "joy.h"
#include "kbd.h"
#include "lib.h"
#include "lightpen.h"
#include "mouse.h"
#include "mousedrv.h"
#include "resources.h"
#include "translate.h"
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
            break;
    }
}

void ui_dispatch_next_event(void)
{
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
        ui_handle_misc_sdl_event(e);
    } else {
        /* Add a small delay to not hog the host CPU when remote
           monitor is being used. */
        SDL_Delay(10);
    }
}

/* Main event handler */
ui_menu_action_t ui_dispatch_events(void)
{
    SDL_Event e;
    ui_menu_action_t retval = MENU_ACTION_NONE;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_KEYDOWN:
                retval = sdlkbd_press(e.key.keysym.sym, e.key.keysym.mod);
                break;
            case SDL_KEYUP:
                retval = sdlkbd_release(e.key.keysym.sym, e.key.keysym.mod);
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
    if (_mouse_enabled && !lightpen_enabled && !sdl_menu_state) {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_WM_GrabInput(SDL_GRAB_ON);
    } else {
        SDL_ShowCursor((sdl_active_canvas->fullscreenconfig->enable && !lightpen_enabled) ? SDL_DISABLE : SDL_ENABLE);
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
    { NULL }
};

void ui_sdl_quit(void)
{
    if (confirm_on_exit) {
        if (message_box("VICE QUESTION", "Do you really want to exit?", MESSAGE_YESNO) == 1) {
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

static const cmdline_option_t cmdline_options[] = {
    { "-menukey", SET_RESOURCE, 1, NULL, NULL, "MenuKey", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<key>", "Keycode of the menu activate key" },
    { "-menukeyup", SET_RESOURCE, 1, NULL, NULL, "MenuKeyUp", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<key>", "Keycode of the menu up key" },
    { "-menukeydown", SET_RESOURCE, 1, NULL, NULL, "MenuKeyDown", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<key>", "Keycode of the menu down key" },
    { "-menukeyleft", SET_RESOURCE, 1, NULL, NULL, "MenuKeyLeft", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<key>", "Keycode of the menu left key" },
    { "-menukeyright", SET_RESOURCE, 1, NULL, NULL, "MenuKeyRight", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<key>", "Keycode of the menu right key" },
    { "-menukeyselect", SET_RESOURCE, 1, NULL, NULL, "MenuKeySelect", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<key>", "Keycode of the menu select key" },
    { "-menukeycancel", SET_RESOURCE, 1, NULL, NULL, "MenuKeyCancel", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<key>", "Keycode of the menu cancel key" },
    { "-menukeyexit", SET_RESOURCE, 1, NULL, NULL, "MenuKeyExit", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<key>", "Keycode of the menu exit key" },
    { "-menukeymap", SET_RESOURCE, 1, NULL, NULL, "MenuKeyMap", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<key>", "Keycode of the menu map key" },
    { "-saveresourcesonexit", SET_RESOURCE, 0, NULL, NULL, "SaveResourcesOnExit", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Enable save resource on exit" },
    { "+saveresourcesonexit", SET_RESOURCE, 0, NULL, NULL, "SaveResourcesOnExit", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Disable save resource on exit" },
    { "-confirmonexit", SET_RESOURCE, 0, NULL, NULL, "ConfirmOnExit", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Enable confirm on exit" },
    { "+confirmonexit", SET_RESOURCE, 0, NULL, NULL, "ConfirmOnExit", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Disable confirm on exit" },
    { NULL }
};

int ui_cmdline_options_init(void)
{
#ifdef SDL_DEBUG
    fprintf(stderr,"%s\n",__func__);
#endif

    return cmdline_register_options(cmdline_options);
}

int ui_init(int *argc, char **argv)
{
#ifdef SDL_DEBUG
    fprintf(stderr,"%s\n",__func__);
#endif
    /* TODO move somewhere else */
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

int uicolor_alloc_color(unsigned int red, unsigned int green, unsigned int blue, unsigned long *color_pixel, BYTE *pixel_return)
{
#ifdef SDL_DEBUG
    fprintf(stderr,"%s\n",__func__);
#endif

    return 0;
}

void uicolor_free_color(unsigned int red, unsigned int green, unsigned int blue, unsigned long color_pixel)
{
#ifdef SDL_DEBUG
    fprintf(stderr,"%s\n",__func__);
#endif
}

void uicolor_convert_color_table(unsigned int colnr, BYTE *data, long color_pixel, void *c)
{
#ifdef SDL_DEBUG
    fprintf(stderr,"%s\n",__func__);
#endif
}

int uicolor_set_palette(struct video_canvas_s *c, const struct palette_s *palette)
{
#ifdef SDL_DEBUG
    fprintf(stderr,"%s\n",__func__);
#endif

    return 0;
}
