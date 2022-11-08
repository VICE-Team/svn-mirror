/*
 * ui.c - Common UI routines.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

/* #define SDL_DEBUG */

#include "vice.h"

#include "vice_sdl.h"
#include <stdio.h>

#include "autostart.h"
#include "cmdline.h"
#include "archdep.h"
#include "color.h"
#include "fullscreenarch.h"
#include "joy.h"
#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "lightpen.h"
#include "log.h"
#include "mainlock.h"
#include "machine.h"
#include "mouse.h"
#include "mousedrv.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uiapi.h"
#include "uicolor.h"
#include "uifilereq.h"
#include "uimenu.h"
#include "uimsgbox.h"
#include "uipoll.h"
#include "uistatusbar.h"
#include "videoarch.h"
#include "vkbd.h"
#include "vsidui_sdl.h"
#include "vsync.h"

#ifndef SDL_DISABLE
#define SDL_DISABLE SDL_IGNORE
#endif

#ifdef SDL_DEBUG
#define DBG(x)  log_debug x
#else
#define DBG(x)
#endif

static int sdl_ui_ready = 0;


static void (*psid_init_func)(void) = NULL;
static void (*psid_play_func)(int) = NULL;


/* ----------------------------------------------------------------- */
/* ui.h */

/* Misc. SDL event handling */
void ui_handle_misc_sdl_event(SDL_Event e)
{
    int capslock;

    if (e.type == SDL_WINDOWEVENT) {
        SDL_Window* window = SDL_GetWindowFromID(e.window.windowID);
        video_canvas_t* canvas = (video_canvas_t*)(SDL_GetWindowData(window, VIDEO_SDL2_CANVAS_INDEX_KEY));

        switch (e.window.event) {
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                DBG(("ui_handle_misc_sdl_event: SDL_WINDOWEVENT_FOCUS_GAINED"));
                sdl_video_canvas_switch(canvas->index);
                capslock = (SDL_GetModState() & KMOD_CAPS) ? 1 : 0;
                if (keyboard_get_shiftlock() != capslock) {
                    keyboard_set_shiftlock(capslock);
                }
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                DBG(("ui_handle_misc_sdl_event: SDL_WINDOWEVENT_EXPOSED"));
                video_canvas_refresh_all(sdl_active_canvas, true);
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                DBG(("ui_handle_misc_sdl_event: SDL_WINDOWEVENT_FOCUS_LOST"));
                keyboard_key_clear();
                break;
            case SDL_WINDOWEVENT_CLOSE:
                DBG(("ui_handle_misc_sdl_event: SDL_WINDOWEVENT_CLOSE"));
                ui_sdl_quit();
                break;
        }
    }

    switch (e.type) {
        case SDL_QUIT:
            DBG(("ui_handle_misc_sdl_event: SDL_QUIT"));
            ui_sdl_quit();
            break;
        case SDL_DROPFILE:
            if (machine_class != VICE_MACHINE_VSID) {
                if (autostart_autodetect(e.drop.file, NULL, 0,
                            AUTOSTART_MODE_RUN) < 0) {
                    ui_error("Cannot autostart specified file.");
                }
            } else {
                /* try to load PSID file */

                if (machine_autodetect_psid(e.drop.file) < 0) {
                    ui_error("%s is not a valid SID file.", e.drop.file);
                }
                if (psid_init_func != NULL && psid_play_func != NULL) {
                    psid_init_func();
                    psid_play_func(0);
                    machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
                }
            }
            break;
#ifdef SDL_DEBUG
        case SDL_USEREVENT:
            DBG(("ui_handle_misc_sdl_event: SDL_USEREVENT"));
            break;
        case SDL_SYSWMEVENT:
            DBG(("ui_handle_misc_sdl_event: SDL_SYSWMEVENT"));
            break;
#endif
        default:
            DBG(("ui_handle_misc_sdl_event: unhandled"));
            break;
    }
}

/* Main event handler */
ui_menu_action_t ui_dispatch_events(void)
{
    SDL_Event e;
    ui_menu_action_t retval = MENU_ACTION_NONE;
    int joynum;

    while (sdl_ui_poll_pop_event(&e)) {
        switch (e.type) {
            case SDL_KEYDOWN:
                ui_display_kbd_status(&e);
                retval = sdlkbd_press(SDL2x_to_SDL1x_Keys(e.key.keysym.sym), e.key.keysym.mod);
                break;
            case SDL_KEYUP:
                ui_display_kbd_status(&e);
                retval = sdlkbd_release(SDL2x_to_SDL1x_Keys(e.key.keysym.sym), e.key.keysym.mod);
                break;
#ifdef HAVE_SDL_NUMJOYSTICKS
            case SDL_JOYAXISMOTION:
                joynum = sdljoy_get_joynum_for_event(e.jaxis.which);
                if (joynum != -1) {
                    retval = sdljoy_axis_event(joynum, e.jaxis.axis, e.jaxis.value);
                }
                break;
            case SDL_JOYBUTTONDOWN:
                joynum = sdljoy_get_joynum_for_event(e.jbutton.which);
                if (joynum != -1) {
                    retval = sdljoy_button_event(joynum, e.jbutton.button, 1);
                }
                break;
            case SDL_JOYBUTTONUP:
                joynum = sdljoy_get_joynum_for_event(e.jbutton.which);
                if (joynum != -1) {
                    retval = sdljoy_button_event(joynum, e.jbutton.button, 0);
                }
                break;
            case SDL_JOYHATMOTION:
                joynum = sdljoy_get_joynum_for_event(e.jhat.which);
                if (joynum != -1) {
                    retval = sdljoy_hat_event(joynum, e.jhat.hat, e.jhat.value);
                }
                break;
            case SDL_JOYDEVICEADDED:
            case SDL_JOYDEVICEREMOVED:
                retval = sdljoy_rescan();
                break;
#endif
            case SDL_MOUSEMOTION:
                sdl_ui_consume_mouse_event(&e);
                if (_mouse_enabled) {
                    mouse_move(e.motion.xrel, e.motion.yrel);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (_mouse_enabled) {
                    mouse_button((int)(e.button.button), (e.button.state == SDL_PRESSED));
                }
                break;
            default:
                /* SDL_EventState(SDL_VIDEORESIZE, SDL_IGNORE); */
                ui_handle_misc_sdl_event(e);
                /* SDL_EventState(SDL_VIDEORESIZE, SDL_ENABLE); */
                break;
        }
        /* When using the menu or vkbd, pass every meaningful event to the caller */
        if (((sdl_menu_state) ||
             (sdl_vkbd_state & SDL_VKBD_ACTIVE)) && (retval != MENU_ACTION_NONE) && (retval != MENU_ACTION_NONE_RELEASE)) {
            break;
        }
    }

    return retval;
}

/* note: we need to be a bit more "radical" about disabling the (mouse) pointer.
 * in practise, we really only need it for the lightpen emulation.
 *
 * TODO: and perhaps in windowed mode enable it when the mouse is moved.
 */

static SDL_Cursor *arrow_cursor = NULL;
static SDL_Cursor *crosshair_cursor = NULL;

static void set_arrow_cursor(void)
{
    if (!arrow_cursor) {
        arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    }
    SDL_SetCursor(arrow_cursor);
}

static void set_crosshair_cursor(void)
{
    if (!crosshair_cursor) {
        crosshair_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    }
    SDL_SetCursor(crosshair_cursor);
}

static int mouse_pointer_hidden = 0;

void ui_check_mouse_cursor(void)
{
    if (_mouse_enabled && !lightpen_enabled && !sdl_menu_state) {
        /* mouse grabbed, not in menu. grab input but do not show a pointer */
        SDL_ShowCursor(SDL_DISABLE);
        set_arrow_cursor();
        SDL_SetRelativeMouseMode(SDL_TRUE);
    } else if (lightpen_enabled && !sdl_menu_state) {
        /* lightpen active, not in menu. show a pointer for the lightpen emulation */
        SDL_ShowCursor(SDL_ENABLE);
        set_crosshair_cursor();
        SDL_SetRelativeMouseMode(SDL_FALSE);
    } else {
        if (sdl_active_canvas->fullscreenconfig->enable) {
            /* fullscreen, never show pointer (we really never need it) */
            SDL_ShowCursor(SDL_DISABLE);
            set_arrow_cursor();
            SDL_SetRelativeMouseMode(SDL_FALSE);
        } else {
            /* windowed */
            SDL_ShowCursor(mouse_pointer_hidden ? SDL_DISABLE : SDL_ENABLE);
            set_arrow_cursor();
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }
    }
}

void ui_set_mouse_grab_window_title(int enabled)
{
    char title[256];
    char name[32];
    char *mouse_key = kbd_get_path_keyname("Machine settings&Mouse emulation&Grab mouse events");

    if (machine_class != VICE_MACHINE_C64SC) {
        strcpy(name, machine_get_name());
    } else {
        strcpy(name, "C64 (x64sc)");
    }
    if (enabled && mouse_key != NULL) {
        snprintf(title, sizeof(title), "VICE: %s%s Use %s to disable mouse grab.",
            name, archdep_extra_title_text(), mouse_key);
    } else {
        snprintf(title, sizeof(title), "VICE: %s%s", name, archdep_extra_title_text());
    }
    title[sizeof(title) - 1] = '\0';

    if (mouse_key != NULL) {
        lib_free(mouse_key);
    }

    sdl_ui_set_window_title(title);
}

void ui_autohide_mouse_cursor(void)
{
    int local_x, local_y;
    static int last_x = 0, last_y = 0;
    static int hidecounter = 60;
    SDL_GetMouseState(&local_x, &local_y);
    if ((local_x != last_x) || (local_y != last_y)) {
        /* mouse was moved, reset counter and unhide */
        hidecounter = 60;
        mouse_pointer_hidden = 0;
        ui_check_mouse_cursor();
    } else {
        /* mouse was not moved, decrement counter and hide on underflow */
        if (hidecounter > 0) {
            hidecounter--;
            if (hidecounter == 0) {
                mouse_pointer_hidden = 1;
                ui_check_mouse_cursor();
            }
        }
    }
    last_x = local_x;
    last_y = local_y;
}

void ui_message(const char* format, ...)
{
    va_list ap;
    char *tmp;

    va_start(ap, format);
    tmp = lib_mvsprintf(format, ap);
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
static int start_minimized;

static int set_ui_menukey(int val, void *param)
{
    sdl_ui_menukeys[vice_ptr_to_int(param)] = SDL2x_to_SDL1x_Keys(val);
    return 0;
}

static int set_save_resources_on_exit(int val, void *param)
{
    save_resources_on_exit = val ? 1 : 0;

    return 0;
}

static int set_confirm_on_exit(int val, void *param)
{
    confirm_on_exit = val ? 1 : 0;

    return 0;
}

#ifdef ALLOW_NATIVE_MONITOR
int native_monitor;

static int set_native_monitor(int val, void *param)
{
    native_monitor = val;
    return 0;
}
#endif

/** \brief  Set StartMinimized resource (bool)
 *
 * \param[in]   val     0: start normal 1: start minimized
 * \param[in]   param   extra param (ignored)
 *
 * \return 0
 */
static int set_start_minimized(int val, void *param)
{
    start_minimized = val ? 1 : 0;
    return 0;
}

#ifndef DEFAULT_MENU_KEY
# ifdef MACOS_COMPILE
#  define DEFAULT_MENU_KEY SDLK_F10
# else
#  define DEFAULT_MENU_KEY SDLK_F12
# endif
#endif

static resource_int_t resources_int[] = {
    /* caution: position of menukeys is hardcoded below */
    { "MenuKey", DEFAULT_MENU_KEY, RES_EVENT_NO, NULL,
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
    { "MenuKeyPageUp", SDLK_PAGEUP, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[9], set_ui_menukey, (void *)MENU_ACTION_PAGEUP },
    { "MenuKeyPageDown", SDLK_PAGEDOWN, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[10], set_ui_menukey, (void *)MENU_ACTION_PAGEDOWN },
    { "MenuKeyHome", SDLK_HOME, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[11], set_ui_menukey, (void *)MENU_ACTION_HOME },
    { "MenuKeyEnd", SDLK_END, RES_EVENT_NO, NULL,
      &sdl_ui_menukeys[12], set_ui_menukey, (void *)MENU_ACTION_END },
    { "SaveResourcesOnExit", 0, RES_EVENT_NO, NULL,
      &save_resources_on_exit, set_save_resources_on_exit, NULL },
    { "ConfirmOnExit", 0, RES_EVENT_NO, NULL,
      &confirm_on_exit, set_confirm_on_exit, NULL },
#ifdef ALLOW_NATIVE_MONITOR
    { "NativeMonitor", 0, RES_EVENT_NO, NULL,
      &native_monitor, set_native_monitor, NULL },
#endif
    { "StartMinimized", 0, RES_EVENT_NO, NULL,
      &start_minimized, set_start_minimized, NULL },
    RESOURCE_INT_LIST_END
};

void ui_sdl_quit(void)
{
    if (confirm_on_exit) {
        if (message_box("VICE QUESTION", "Do you really want to exit?", MESSAGE_YESNO) != 0) {
            return;
        }
    }
    archdep_vice_exit(0);
}

/* Initialization  */
int ui_resources_init(void)
{
    int i;
    DBG(("%s", __func__));

    /* this converts the default keycodes as needed */
    for (i = 0; i < 13; i++) {
        resources_int[i].factory_value = SDL2x_to_SDL1x_Keys(resources_int[i].factory_value);
    }

    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    if (machine_class != VICE_MACHINE_VSID) {
        return uistatusbar_init_resources();
    }
    return 0;
}

void ui_resources_shutdown(void)
{
    DBG(("%s", __func__));
    joy_arch_resources_shutdown();
    sdlkbd_resources_shutdown();
}

static const cmdline_option_t cmdline_options[] =
{
    { "-menukey", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKey", NULL,
      "<key>", "Keycode of the menu activate key" },
    { "-menukeyup", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyUp", NULL,
      "<key>", "Keycode of the menu up key" },
    { "-menukeydown", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyDown", NULL,
      "<key>", "Keycode of the menu down key" },
    { "-menukeyleft", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyLeft", NULL,
      "<key>", "Keycode of the menu left key" },
    { "-menukeyright", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyRight", NULL,
      "<key>", "Keycode of the menu right key" },
    { "-menukeypageup", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyPageUp", NULL,
      "<key>", "Keycode of the menu page up key" },
    { "-menukeypagedown", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyPageDown", NULL,
      "<key>", "Keycode of the menu page down key" },
    { "-menukeyhome", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyHome", NULL,
      "<key>", "Keycode of the menu home key" },
    { "-menukeyend", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyEnd", NULL,
      "<key>", "Keycode of the menu end key" },
    { "-menukeyselect", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeySelect", NULL,
      "<key>", "Keycode of the menu select key" },
    { "-menukeycancel", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyCancel", NULL,
      "<key>", "Keycode of the menu cancel key" },
    { "-menukeyexit", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyExit", NULL,
      "<key>", "Keycode of the menu exit key" },
    { "-menukeymap", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MenuKeyMap", NULL,
      "<key>", "Keycode of the menu map key" },
    { "-saveres", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "SaveResourcesOnExit", (resource_value_t)1,
      NULL, "Enable saving of the resources on exit" },
    { "+saveres", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "SaveResourcesOnExit", (resource_value_t)0,
      NULL, "Disable saving of the resources on exit" },
    { "-confirmonexit", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "ConfirmOnExit", (resource_value_t)1,
      NULL, "Enable confirm on exit" },
    { "+confirmonexit", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "ConfirmOnExit", (resource_value_t)0,
      NULL, "Disable confirm on exit" },
#ifdef ALLOW_NATIVE_MONITOR
    { "-nativemonitor", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "NativeMonitor", (resource_value_t)1,
      NULL, "Enable native monitor" },
    { "+nativemonitor", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "NativeMonitor", (resource_value_t)0,
      NULL, "Disable native monitor" },
#endif
    { "-minimized", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "StartMinimized", (void *)1,
        NULL, "Start VICE minimized" },
    { "+minimized", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
        NULL, NULL, "StartMinimized", (void *)0,
        NULL, "Do not start VICE minimized" },
    CMDLINE_LIST_END
};

static const cmdline_option_t statusbar_cmdline_options[] =
{
    { "-statusbar", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "SDLStatusbar", (resource_value_t)1,
      NULL, "Enable statusbar" },
    { "+statusbar", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "SDLStatusbar", (resource_value_t)0,
      NULL, "Disable statusbar" },
#if 0
    { "-kbdstatusbar", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "KbdStatusbar", (resource_value_t)1,
      NULL, "Enable keyboard-status bar" },
    { "+kbdstatusbar", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "KbdStatusbar", (resource_value_t)0,
      NULL, "Disable keyboard-status bar" },
#endif
    CMDLINE_LIST_END
};

int ui_cmdline_options_init(void)
{
    DBG(("%s", __func__));

    if (machine_class != VICE_MACHINE_VSID) {
        if (cmdline_register_options(statusbar_cmdline_options) < 0) {
            return -1;
        }
    }

    return cmdline_register_options(cmdline_options);
}

void ui_init_with_args(int *argc, char **argv)
{
    DBG(("%s", __func__));
}

int ui_init(void)
{
    sdl_ui_poll_init();
    
    DBG(("%s", __func__));
    return 0;
}

int ui_init_finalize(void)
{
    DBG(("%s", __func__));

    if (!console_mode) {
        sdl_ui_init_finalize();
        sdl_ui_ready = 1;
    }
    return 0;
}

void ui_shutdown(void)
{
    DBG(("%s", __func__));

    if (arrow_cursor) {
        SDL_FreeCursor(arrow_cursor);
        arrow_cursor = NULL;
    }
    if (crosshair_cursor) {
        SDL_FreeCursor(crosshair_cursor);
        crosshair_cursor = NULL;
    }

    sdl_ui_file_selection_dialog_shutdown();
    sdl_ui_poll_shutdown();
}

/* Print an error message.  */
void ui_error(const char *format, ...)
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
char* ui_get_file(const char *format, ...)
{
    return NULL;
}

/* Drive related UI.  */
int ui_extend_image_dialog(void)
{
    if (message_box("VICE QUESTION", "Do you want to extend the disk image?", MESSAGE_YESNO) == 0) {
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
        return UI_JAM_RESET;
    }
    if (retval == 1) {
        return UI_JAM_HARD_RESET;
    }
    if (retval == 2) {
        return UI_JAM_NONE;
    }
    if (retval == 3) {
        return UI_JAM_MONITOR;
    }
    if (retval == 4) {
        archdep_vice_exit(0);
    }
    return UI_JAM_NONE;
}

/* ----------------------------------------------------------------- */
/* uicolor.h */

int uicolor_alloc_color(unsigned int red, unsigned int green, unsigned int blue, unsigned long *color_pixel, uint8_t *pixel_return)
{
    DBG(("%s", __func__));
    return 0;
}

void uicolor_free_color(unsigned int red, unsigned int green, unsigned int blue, unsigned long color_pixel)
{
    DBG(("%s", __func__));
}

void uicolor_convert_color_table(unsigned int colnr, uint8_t *data, long color_pixel, void *c)
{
    DBG(("%s", __func__));
}

int uicolor_set_palette(struct video_canvas_s *c, const struct palette_s *palette)
{
    DBG(("%s", __func__));
    return 0;
}


/* FIXME: temporary stub to support calling ui_hotkeys_init() from src/main.c
 */
void ui_hotkeys_init(void)
{
    /* NOP */
}

/* ---------------------------------------------------------------------*/
/* vsidui_sdl.h */

/* These items are here so they can be used in generic UI
   code without vsidui.c being linked into all emulators. */
int sdl_vsid_state = 0;

static ui_draw_func_t vsid_draw_func = NULL;

void sdl_vsid_activate(void)
{
    sdl_vsid_state = SDL_VSID_ACTIVE | SDL_VSID_REPAINT;
}

void sdl_vsid_close(void)
{
    sdl_vsid_state = 0;
}

void sdl_vsid_draw_init(ui_draw_func_t func)
{
    vsid_draw_func = func;
}

void sdl_vsid_draw(void)
{
    if (vsid_draw_func) {
        vsid_draw_func();
    }
}


/*
 * Work around linking differences between VSID and other emu's
 */

/** \brief  Set PSID init function pointer
 *
 * \param[in]   func    function pointer
 */
void sdl_vsid_set_init_func(void (*func)(void))
{
    psid_init_func = func;
}


/** \brief  Set PSID play function pointer
 *
 * \param[in]   func    function pointer
 */
void sdl_vsid_set_play_func(void (*func)(int))
{
    psid_play_func = func;
}
