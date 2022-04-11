/*
 * uipoll.c - UI key/button polling.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

#include "vice_sdl.h"
#include <stdio.h>

#include "joy.h"
#include "kbd.h"
#include "lib.h"
#include "log.h"
#include "mainlock.h"
#include "ui.h"
#include "uimenu.h"
#include "uipoll.h"

static void *queue_lock;
static SDL_Event *queue;
static int queue_length;
static int queue_alloc_size;
static int queue_write_index;
static int queue_read_index;

/* ------------------------------------------------------------------ */
/* static functions */

static inline int is_not_modifier(SDLKey k)
{
    return ((k != SDLK_RSHIFT) &&
            (k != SDLK_LSHIFT) &&
            (k != SDLK_RMETA) &&
            (k != SDLK_LMETA) &&
            (k != SDLK_RCTRL) &&
            (k != SDLK_LCTRL) &&
            (k != SDLK_RALT) &&
            (k != SDLK_LALT)) ? 1 : 0;
}

static void sdl_poll_print_timeout(int x, int y, int time)
{
    char *timestr = NULL;

    timestr = lib_msprintf("Timeout in %i...", time);
    sdl_ui_print(timestr, x, y);
    sdl_ui_refresh();
    lib_free(timestr);
}

static void increase_max_queue_size(void)
{
    int new_alloc_size;
    SDL_Event *resized_queue;
    
    new_alloc_size = queue_alloc_size + 1;
    resized_queue = lib_malloc(new_alloc_size * sizeof(SDL_Event));
    
    log_message(LOG_DEFAULT, "Increasing SDL message queue size to %d", new_alloc_size);
    
    /* Copy events to the start of the new queue */
    queue_write_index = 0;
    while(queue_length--) {
        resized_queue[queue_write_index++] = queue[queue_read_index++];
        if (queue_read_index == queue_alloc_size) {
            queue_read_index = 0;
        }
    }
    
    queue_length = queue_write_index;
    queue_read_index = 0;
    
    lib_free(queue);
    queue = resized_queue;
    queue_alloc_size = new_alloc_size;
}

/* ------------------------------------------------------------------ */
/* External interface */

void sdl_ui_poll_init(void)
{
    archdep_mutex_create(&queue_lock);
    
    queue_alloc_size    = 1;
    queue               = lib_malloc(queue_alloc_size * sizeof(SDL_Event));
}

void sdl_ui_poll_shutdown(void)
{
    archdep_mutex_destroy(queue_lock);
    lib_free(queue);
}

int sdl_ui_poll_pop_event(SDL_Event *e)
{
    mainlock_yield_begin();
    archdep_mutex_lock(queue_lock);
    
    if (queue_length == 0) {
        archdep_mutex_unlock(queue_lock);
        mainlock_yield_end();
        return 0;
    }
    
    *e = queue[queue_read_index++];
    queue_length--;
    
    if (queue_read_index == queue_alloc_size) {
        queue_read_index = 0;
    }
    
    archdep_mutex_unlock(queue_lock);
    mainlock_yield_end();
    
    return 1;
}

void sdl_ui_poll_push_event(SDL_Event *e)
{
    archdep_mutex_lock(queue_lock);
    
    if (queue_length == queue_alloc_size) {
        increase_max_queue_size();
    }
    
    queue[queue_write_index++] = *e;
    queue_length++;
    
    if (queue_write_index == queue_alloc_size) {
        queue_write_index = 0;
    }
    
    archdep_mutex_unlock(queue_lock);
}


SDL_Event sdl_ui_poll_specific_event(const char *what, const char *target, int options, int timeout)
{
    SDL_Event e;

    int count = 0;
    int polling = 1;
    int i;

    int allow_keyboard = options & SDL_POLL_KEYBOARD;
    int allow_modifier = options & SDL_POLL_MODIFIER;
#ifdef HAVE_SDL_NUMJOYSTICKS
    int allow_joystick = options & SDL_POLL_JOYSTICK;
#endif

    sdl_ui_clear();
    i = sdl_ui_print("Polling ", 0, 0);
    i = i + sdl_ui_print(what, i, 0);
    sdl_ui_print(" for:", i, 0);
    sdl_ui_print(target, 0, 1);

    if (timeout > 0) {
        sdl_poll_print_timeout(0, 2, timeout);
    }

    /* TODO check if key/event is suitable */
    while (polling) {
        while (polling && sdl_ui_poll_pop_event(&e)) {
            switch (e.type) {
                case SDL_KEYDOWN:
                    if (allow_keyboard && (allow_modifier || is_not_modifier(e.key.keysym.sym))) {
                        polling = 0;
                    }
                    break;
#ifdef HAVE_SDL_NUMJOYSTICKS
                case SDL_JOYBUTTONDOWN:
                    if (allow_joystick) {
                        polling = 0;
                    }
                    break;
                case SDL_JOYHATMOTION:
                    if (allow_joystick && (sdljoy_check_hat_movement(e) != 0)) {
                        polling = 0;
                    }
                    break;
                case SDL_JOYAXISMOTION:
                    if (allow_joystick && (sdljoy_check_axis_movement(e) != 0)) {
                        polling = 0;
                    }
                    break;
#endif
                default:
                    ui_handle_misc_sdl_event(e);
                    break;
            }
        }
        mainlock_yield_and_sleep(tick_per_second() / 60);

        if ((timeout > 0) && (++count == 1000 / 20)) {
            count = 0;
            if (--timeout == 0) {
                e.type = SDL_USEREVENT;
                polling = 0;
            } else {
                sdl_poll_print_timeout(0, 2, timeout);
            }
        }
    }

    if (polling == 1) {
        e.type = SDL_USEREVENT;
    }

    return e;
}
