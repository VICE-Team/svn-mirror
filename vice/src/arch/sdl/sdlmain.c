/*
 * sdlmain.c - SDL startup.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

#include "archdep.h"
#include "cmdline.h"
#include "log.h"
#include "machine.h"
#include "main.h"
#include "mainlock.h"
#include "render-queue.h"
#include "resources.h"
#include "ui.h"
#include "uimenu.h"
#include "uipoll.h"
#include "videoarch.h"

#include "vice_sdl.h"

/* FIXME: Ugly hack for preventing SDL crash using -help */
int sdl_help_shutdown = 0;

#define HACK_REDRAW_EACH_RESIZE_EVENT 0

#if HACK_REDRAW_EACH_RESIZE_EVENT
static void *hack_lock;
static bool hack_canvas_redraws[MAX_CANVAS_NUM];

/*
 * HACK: Enables redraw of the SDL window during a resize.
 *
 * SDL blocks event polling completely while the window is being resized. However,
 * we can hook new SDL_Events being added to the queue from the context of the thread
 * adding the event. By observing events as they are added, we can note which redraws
 * are ready. In addition, as resize events are queued on the main thread during a resize,
 * we can check if any redraws have been requested and execute them at that point, rather
 * than waiting for the event polling to unfreeze.
 */
static int hack_event_observer(void *userdata, SDL_Event *e)
{
    video_canvas_t *canvas;
    int i;
    bool redraw_needed[MAX_CANVAS_NUM];

    if (archdep_thread_current_is_main()) {
        
        archdep_mutex_lock(hack_lock);
        for (i = 0; i < MAX_CANVAS_NUM; i++) {
            if (hack_canvas_redraws[i]) {
                redraw_needed[i] = true;
                hack_canvas_redraws[i]--;
            } else {
                redraw_needed[i] = false;
            }
        }
        archdep_mutex_unlock(hack_lock);
        
        for (i = 0; i < MAX_CANVAS_NUM; i++) {
            if (redraw_needed[i]) {
                video_canvas_display_backbuffer(video_canvas_get(i));
            }
        }
        
    } else {
        /*
         * Another thread is posting an event, likely the vice thread
         */
        
        if (e->type == sdl_event_new_video_frame) {
            /* A render event */
            canvas = e->user.data1;
            archdep_mutex_lock(hack_lock);
            hack_canvas_redraws[canvas->index]++;
            archdep_mutex_unlock(hack_lock);
        }
    }
    
    return 1; /* Ignored when just observing */
}
#endif

int main(int argc, char **argv)
{
    int init_result;
    SDL_Event e;
    video_canvas_t *canvas;

    init_result = main_program_init(argc, argv);
    if (init_result) {
        return init_result;
    }
    
#if HACK_REDRAW_EACH_RESIZE_EVENT
    archdep_mutex_create(&hack_lock);
    
    /* Note - some events will have already been pushed by now, including render events */
    SDL_AddEventWatch(hack_event_observer, NULL);
#endif
    
    /*
     * Loop forever - VICE exits using exit()
     */

    for (;;) {
        while(SDL_WaitEventTimeout(&e, 1000 / 60) != 0)
        {
            /* Check for custom render frame event, which we handle directly */
            if (e.type == sdl_event_new_video_frame) {
                canvas = e.user.data1;
                
#if 0
                archdep_mutex_lock(hack_lock);
                if (hack_canvas_redraws[canvas->index]) {
                    hack_canvas_redraws[canvas->index]--;
                }
                archdep_mutex_unlock(hack_lock);
#endif
                /*
                 * Unless we can get our event observer installed before the VICE thread
                 * starts generating sdl_event_new_video_frame events, we can't use the flag
                 * above to conditionally render here. REndering with no backbuffers queued
                 * exits early with minimum performance impact so it's fine (for a hack).
                 */
                
                video_canvas_display_backbuffer(canvas);
                
            } else if (e.type == sdl_event_mouse_move_processed) {
                ui_autohide_mouse_cursor();
            
            } else if (e.type == sdl_event_canvas_resized) {
                mainlock_obtain();
                sdl2_video_canvas_resize_impl(e.user.data1);
                mainlock_release();
            
            } else if (e.type == sdl_event_ui_needs_refresh) {
                mainlock_obtain();
                sdl_ui_refresh();
                mainlock_release();

#ifdef USE_SDL2UI
            } else if (e.type == sdl_event_second_window_show) {
                mainlock_obtain();
                sdl2_show_second_window_impl();
                mainlock_release();

            } else if (e.type == sdl_event_second_window_hide) {
                mainlock_obtain();
                sdl2_hide_second_window_impl();
                mainlock_release();
#endif
                
            } else {
                /* Copy SDL_Event to queue for VICE thread */
                sdl_ui_poll_push_event(&e);
            }
        }

        mainlock_execute_main_thread_callbacks();
    }
}

void main_exit(void)
{
    /* FIXME: Ugly hack for preventing SDL crash using -help */
    if (!sdl_help_shutdown) {
        /* log resources with non default values */
        resources_log_active();
        /* log the active config as commandline options */
        cmdline_log_active();
    }

    log_message(LOG_DEFAULT, "\nExiting...");

    /*
     * Clean up dangling resources due to the 'Quit emu' callback not returning
     * to the calling menu code.
     */
    sdl_ui_menu_shutdown();
    
    vice_thread_shutdown();

    machine_shutdown();
    
#if HACK_REDRAW_EACH_RESIZE_EVENT
    archdep_mutex_destroy(hack_lock);
#endif
    
    putchar('\n');
}
