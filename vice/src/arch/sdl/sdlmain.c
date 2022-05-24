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
    
    if (e->type == SDL_WINDOWEVENT) {
        if (e->window.event == SDL_WINDOWEVENT_RESIZED) {
            /* Events are intercepted on the thread that pushes the event - let's be certain we're on the right thread */
            if (!archdep_thread_current_is_main()) {
                return 1;
            }
            for (i = 0; i < MAX_CANVAS_NUM; i++) {
                canvas = video_canvas_get(i);
                if (canvas) {
                    /* This will only paint if a new frame is available but better than nothing */
                    video_canvas_display_backbuffer(canvas);
                }
            }
        }
    }
    
    return 1; /* Ignored when just observing */
}

static void process_sdl_event(SDL_Event *e)
{
    if (e->type == sdl_event_mouse_move_processed) {
        ui_autohide_mouse_cursor();
    
    } else if (e->type == sdl_event_ui_needs_refresh) {
        mainlock_obtain();
        sdl_ui_refresh();
        mainlock_release();
    
    } else if (e->type == sdl_event_second_window_show) {
        mainlock_obtain();
        sdl2_show_second_window_impl();
        mainlock_release();

    } else if (e->type == sdl_event_second_window_hide) {
        mainlock_obtain();
        sdl2_hide_second_window_impl();
        mainlock_release();

    } else if (e->type == sdl_event_restore_window_size) {
        sdl2_video_restore_size_impl();

    } else if (e->type == sdl_event_readline_request) {
        sdl_ui_readline_input_impl();
    
    } else if (e->type == sdl_event_run_on_main_thread) {
        mainlock_obtain();
        ((main_thread_function_t)e->user.data1)(e->user.data2);
        mainlock_release();
        
    } else {
        /* Copy SDL_Event to queue for VICE thread */
        sdl_ui_poll_push_event(e);
    }
}

int main(int argc, char **argv)
{
    int init_result;
    SDL_Event e;
    video_canvas_t *canvas;
    int i;
    int deferred_render_count[MAX_CANVAS_NUM];
    int total_outstanding_renders = 0;

    init_result = main_program_init(argc, argv);
    if (init_result) {
        return init_result;
    }
    
    for (i = 0; i < MAX_CANVAS_NUM; i++) {
        deferred_render_count[i] = 0;
    }
    
    /* Note - some events will have already been pushed by now, including render events */
    SDL_AddEventWatch(hack_event_observer, NULL);
    
    /*
     * Loop forever - VICE exits using exit()
     */

    for (;;) {

        /*
         * Perform any deferred rendering we know about, prioritising the active canvas
         */

        if (total_outstanding_renders) {
            if (deferred_render_count[sdl_active_canvas->index]) {
                video_canvas_display_backbuffer(sdl_active_canvas);
                deferred_render_count[sdl_active_canvas->index]--;
                total_outstanding_renders--;
            }
            
            for (i = 0; i < MAX_CANVAS_NUM; i++) {
                if (i != sdl_active_canvas->index) {
                    if (deferred_render_count[i]) {
                        video_canvas_display_backbuffer(video_canvas_get(i));
                        deferred_render_count[i]--;
                        total_outstanding_renders--;
                    }
                }
            }
        }
        
        if (total_outstanding_renders) {
            /* We still have another render to perform, so don't block on SDL events */
            if (SDL_PollEvent(&e) == 0) {
                /* No pending events, loop around to the outstanding render */
                continue;
            }
        } else {
            /* Everything up to date, wait for the next event */
            if (SDL_WaitEvent(&e) == 0) {
                log_error(LOG_DEFAULT, "Error in SDL_WaitEvent(): %s", SDL_GetError());
                archdep_vice_exit(1);
            }
        }

        /*
         * Handle this event, then all queued events, deferring any renders until next iteration as
         * they may block processing for a while on vsync.
         */

        do {
            if (e.type == sdl_event_new_video_frame) {
                canvas = e.user.data1;
                deferred_render_count[canvas->index]++;
                total_outstanding_renders++;
            } else {
                process_sdl_event(&e);
            }
        } while(SDL_PollEvent(&e));
    }
}

void main_exit(void)
{
    log_message(LOG_DEFAULT, "\nExiting...");

    /* log resources with non default values */
    resources_log_active();
    
    /* log the active config as commandline options */
    cmdline_log_active();

    /*
     * Clean up dangling resources due to the 'Quit emu' callback not returning
     * to the calling menu code.
     */
    sdl_ui_menu_shutdown();
    
    vice_thread_shutdown();

    machine_shutdown();
    
    putchar('\n');
}
