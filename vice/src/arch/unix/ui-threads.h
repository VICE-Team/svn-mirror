/*
 * ui-threads.h 
 *
 * Written by
 *  pottendo <pottendo@gmx.net>
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

#ifndef __ui_threads_h
#define __ui_threads_h

#define MAX_BUFFERS 8
void mbuffer_init(void *widget, int w, int h, int depth);
unsigned char *mbuffer_get_buffer(struct timespec *ts);
struct s_mbufs
{
    long stamp; /* timestamp in usecs */
    int w;
    int h;
    unsigned char *buffer;
    struct s_mbufs *next;
    /*GLu */ unsigned int bindId;	/* XXX Fixme: try to avoid GL specifics */
};

void video_dthread_init(void);
void dthread_lock();
void dthread_unlock();
int dthread_ui_init(int *argc, char **argv);
int dthread_ui_init_finish(void);
void dthread_build_screen_canvas(video_canvas_t *c);
int dthread_ui_open_canvas_window(video_canvas_t *c, const char *t, int wi, int he, int na);
void dthread_ui_dispatch_events(void);
void dthread_ui_trigger_resize(void);
void dthread_ui_trigger_window_resize(video_canvas_t *c);
int dthread_configure_callback_canvas(void *w, void *e, void *cd);
void dthread_trigger_refresh(void *w, video_canvas_t *c);

#endif /* __ui_threads_h */
