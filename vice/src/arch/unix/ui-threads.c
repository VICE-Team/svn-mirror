/*
 * ui-threads.c
 * Foundation for multithreaded GUIs to decouple machine (=emulation) from 
 * display drawing routines and from GUI event-processing based on pthreads.
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

#include "vice.h"

#ifndef USE_UI_THREADS
#error "USE_UI_THREADS not evailable in config.h - check config.log"
#endif

/* #define DEBUG_MBUFFER */

#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>

#include "lib.h"
#include "log.h"
#include "resources.h"
#include "machine.h"
#include "videoarch.h"
#include "vsync.h"
#include "ui-threads.h"

#ifdef DEBUG_MBUFFER
#define DBG(_x_) log_debug _x_ 
#define DBG2(_x_) 
#else
#define DBG(_x_)
#define DBG2(_x_)
#endif

#define TS_TOUSEC(x) (x.tv_sec * 1000000L + (x.tv_nsec / 1000))
#define TS_TOMSEC(x) (x.tv_sec * 1000L + (x.tv_nsec / 1000000L))
/* the freq. should be configurable */
#define REFRESH_FREQ (8 * 1000 * 1000)
static struct timespec reltime = { 0, REFRESH_FREQ };
static pthread_t dthread;
static pthread_t ethread;
static pthread_cond_t cond  = PTHREAD_COND_INITIALIZER;
static pthread_cond_t coroutine  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t dlock = PTHREAD_MUTEX_INITIALIZER;
static sem_t mthread_sem, ethread_sem;
static void *widget, *event, *client_data;
static int is_coroutine = 1;	/* start with single threaded execution */
static double machine_freq;
static long laststamp, mrp_usec;
typedef struct {
    int cpos;
    int lpos;
    int csize;
    void *canvas;
    void *widget;
} buffer_ptrs_s;
static struct s_mbufs buffers[MAX_APP_SHELLS][MAX_BUFFERS];
static buffer_ptrs_s bptrs[MAX_APP_SHELLS];
static int update = 0;
static int emu_running = 1;
static int app_shell_count = 0;
#define NEXT(x) ((++x == MAX_BUFFERS) ? 0 : x)
static int width, height, no_autorepeat;
const char *title;
static int *argc;
static char **argv;
static int int_ret;
static video_canvas_t *canvas;

/* coroutine func IDs */
typedef enum { CR_NOTHING, CR_REDRAW, CR_CANVAS_WIDGET, CR_OPEN_CANVAS, 
	       CR_DISPATCH_EVENTS, CR_INIT, CR_INIT_FINISH, CR_CONFIGURE_CALLBACK, 
	       CR_RESIZE, CR_WINDOW_RESIZE, CR_SHUTDOWN } coroutine_t;
static coroutine_t do_action = CR_NOTHING;
/* prototypes for internals */
static void dthread_coroutine(coroutine_t a);
static void *dthread_func(void *attr);
static void *ethread_func(void *attr);
/* resources */
static int do_blending;
static int set_alpha_blending(int val, void *p)
{
    DBG(("Toggle alpha blending: %d -> %d", do_blending, val));
    do_blending = val;
}

static resource_int_t resources_uithreads[] = {
    { "AlphaBlending", 1, RES_EVENT_NO, NULL,
      &do_blending, set_alpha_blending, NULL },
    { NULL }
};



void mbuffer_init(void *canvas, int w, int h, int depth, int shell)
{
    int i;
    DBG(("widget %p, w=%d, h=%d, shell=%d", widget, w, h, shell));
    bptrs[shell].csize = w * h * depth;
    for (i = 0; i < MAX_BUFFERS; i++) { 
	lib_free(buffers[shell][i].buffer);
	buffers[shell][i].buffer = lib_malloc(bptrs[shell].csize);
	memset(buffers[shell][i].buffer, -1, bptrs[shell].csize);
	buffers[shell][i].w = w;
	buffers[shell][i].h = h;
	if (i > 0) {
	    buffers[shell][i-1].next = &buffers[shell][i];
	}
    }
    buffers[shell][i-1].next = &buffers[shell][0];
    bptrs[shell].cpos = 0;
    bptrs[shell].canvas = canvas;
    if (shell > app_shell_count) {
	app_shell_count = shell;
    }
}

unsigned char *mbuffer_get_buffer(struct timespec *t, int shell) 
{
    unsigned char *curr;
    int tmppos = bptrs[shell].cpos;
    long tmpstamp, j;
    struct timespec ts = *t;
    
    if (machine_freq != vsync_get_refresh_frequency()) {
        machine_freq=vsync_get_refresh_frequency();
	mrp_usec = 1000.0 / machine_freq * 1000;
	DBG(("machine freq = %f ms period %ld us", (float) machine_freq, mrp_usec));
    }
    
    /* stamp in usecs */
    curr = buffers[shell][bptrs[shell].cpos].buffer;
    tmppos = NEXT(bptrs[shell].cpos);
    /* copy fullframe */
    memcpy(buffers[shell][tmppos].buffer, curr, bptrs[shell].csize); 
    bptrs[shell].cpos = tmppos;
    if (bptrs[shell].cpos == bptrs[shell].lpos) {
	DBG(("out of buffers: %s", __FUNCTION__));
    }

    laststamp += mrp_usec; /* advance by machine cycle */
    tmpstamp = TS_TOUSEC(ts);
    j = tmpstamp - laststamp;
    /* DBG(("emu jitter of: %5ld us", j)); */
    if ((j > 15000) || (j < -15000)) {
	DBG(("resetting jitter: %5ld us", j));
	laststamp = tmpstamp;
    }
    
    buffers[shell][bptrs[shell].cpos].stamp = laststamp;
    return buffers[shell][bptrs[shell].cpos].buffer;
}

void tsAdd (const struct timespec *time1, 
	    const struct timespec *time2,
	    struct timespec *result)
{
    /* Add the two times together. */
    result->tv_sec = time1->tv_sec + time2->tv_sec ;
    result->tv_nsec = time1->tv_nsec + time2->tv_nsec ;
    if (result->tv_nsec >= 1000000000L) {		/* Carry? */
        result->tv_sec++ ;  
	result->tv_nsec = result->tv_nsec - 1000000000L ;
    }
}

/* display thread routines - should go elsewehere later on */
void dthread_build_screen_canvas(video_canvas_t *c)
{
    if (is_coroutine) {
	build_screen_canvas_widget2(c);
	return;
    }
    canvas = bptrs[c->app_shell].canvas = c;
    widget = bptrs[c->app_shell].widget = c->emuwindow;
    dthread_coroutine(CR_CANVAS_WIDGET);
}

int dthread_ui_open_canvas_window(video_canvas_t *c, const char *t, int wi, int he, int na) 
{
    if (is_coroutine) {
	return ui_open_canvas_window2(c, t, wi, he, na);
    }
    
    canvas = bptrs[c->app_shell].canvas = c;
    widget = bptrs[c->app_shell].widget = c->emuwindow;
    title = t;
    width = wi;
    height = he;
    no_autorepeat = na;
    dthread_coroutine(CR_OPEN_CANVAS);
    return int_ret;
}

int dthread_ui_init(int *ac, char **av)
{
    if (is_coroutine) {
	return ui_init2(ac, av);
    }
    
    argc=ac;
    argv=av;
    dthread_coroutine(CR_INIT);
    return 0;
}

void dthread_ui_dispatch_events(void) 
{
    if (update || is_coroutine) {
	DBG2(("recursive call to %s - update: %d, is_coroutine %d", __FUNCTION__, 
	     update, is_coroutine));
	ui_dispatch_events2();
	return;
    } else {
	DBG2(("call to %s - update: %d, is_coroutine %d", __FUNCTION__,	
	     update, is_coroutine));
	update = 1;
 	if (sem_post(&ethread_sem) != 0) {
	    log_debug("sem_post() failed, %s", __FUNCTION__);
	    exit (-1);
	}
 	if (sem_wait(&mthread_sem) != 0) {
	    log_debug("sem_post() failed, %s", __FUNCTION__);
	    exit (-1);
	}
	update = 0;
    }
}

int dthread_ui_init_finish()
{
    if (is_coroutine) {
	return ui_init_finish2();
    }
    dthread_coroutine(CR_INIT_FINISH);
    return int_ret;
}

int dthread_configure_callback_canvas(void *w, void *e, void *cd)
{
    if (is_coroutine) {
	DBG(("recursive call to %s, update: %d, is_coroutine: %d", __FUNCTION__,
	     update, is_coroutine));
	return configure_callback_canvas2(w, e, cd);
    }
    
    widget = bptrs[((video_canvas_t *) cd)->app_shell].widget = w;
    event = e;
    client_data = cd;
    dthread_coroutine(CR_CONFIGURE_CALLBACK);
    return int_ret;
}

void dthread_ui_trigger_resize(void)
{
    if (is_coroutine) {
	return ui_trigger_resize2();
    }
    
    dthread_coroutine(CR_RESIZE);
}

void dthread_ui_trigger_window_resize(video_canvas_t *c)
{
    if (is_coroutine) {
	ui_trigger_window_resize2(c);
	return;
    }
    canvas = bptrs[canvas->app_shell].canvas = c;
    widget = bptrs[c->app_shell].widget = c->emuwindow;
    dthread_coroutine(CR_WINDOW_RESIZE);
}

void video_dthread_init(void)
{
    struct sched_param param;
    pthread_attr_t attr;

    resources_register_int(resources_uithreads);
    
    if (console_mode) {
	is_coroutine = 1;	/* enforce single threaded execution */
	return;
    }
    
    if (pthread_attr_init(&attr)) {
	log_debug("pthread_attr_init() failed, %s", __FUNCTION__);
	exit (-1);      
    }

    if (pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
	log_debug("pthread_setinheritsched() failed, %s", __FUNCTION__);
	exit (-1);
    }
    
    if (sem_init(&mthread_sem, 0, 0) < 0) {
	log_debug("sem_init() failed, %s", __FUNCTION__);
	exit (-1);
    }
    
    if (sem_init(&ethread_sem, 0, 0) < 0) {
	log_debug("sem_init() failed, %s", __FUNCTION__);
	exit (-1);
    }

    is_coroutine = 0;		/* use multithreaded executionfrom now on */
    if (pthread_create(&dthread, &attr, dthread_func, NULL) < 0) {
	log_debug("pthread_create() failed, %s", __FUNCTION__);
	exit (-1);
    }
    if (pthread_create(&ethread, &attr, ethread_func, NULL) < 0) {
	log_debug("pthread_create() failed, %s", __FUNCTION__);
	exit (-1);
    }
    
    param.sched_priority = 20;
    if (pthread_setschedparam(dthread, SCHED_RR, &param)) {
	log_message(LOG_DEFAULT, "ui-threads: failed to set realtime priority for VICE - this is no problem!\n\trefer to Readme-Unix.txt (http://vice-emu.svn.sourceforge.net/viewvc/vice-emu/trunk/vice/doc/readmes/Readme-Unix.txt)");
    }
    pthread_detach(dthread);
    pthread_detach(ethread);
}

void video_dthread_shutdown(void)
{
    dthread_coroutine(CR_SHUTDOWN);
}

void dthread_lock(void) 
{
    if (pthread_mutex_lock(&dlock) < 0) {
	log_debug("pthread_mutex_lock() failed, %s", __FUNCTION__);
	exit (-1);
    }
}

void dthread_unlock(void) 
{
    if (pthread_mutex_unlock(&dlock) < 0) {
	log_debug("pthread_mutex_unlock() failed, %s", __FUNCTION__);
	exit (-1);
    }
}

/* internal routines */

int dthread_calc_frames(unsigned long dt, int *from, int *to, int *alpha, int shell)
{
    int ret = 1;
    unsigned long dt1, dt2;
    int np = bptrs[shell].lpos;
    int count = bptrs[shell].cpos - bptrs[shell].lpos;
    int i;
    
    if (!do_blending) {
	*from = *to = bptrs[shell].cpos;
	return 1;
    }
    
    /* subtract machine cycle once */
    dt -= mrp_usec;
	
    /* find display frame interval where we fit in */
    if (count < 0) count += MAX_BUFFERS;
    for (i = 0; i < count; i++) {
	if (buffers[shell][np].stamp > dt) {
	    break;
	}
	np ++;
	if (np == MAX_BUFFERS) {
	    np = 0;
	}
    }
    int np2 = np - 1;
    if (np2 < 0) {
	np2 += MAX_BUFFERS;
    }
    dt1 = dt - buffers[shell][np2].stamp;
    dt2 = buffers[shell][np].stamp - buffers[shell][np2].stamp;
    if (dt1 > dt2) {
	/* DBG(("dthread dropping frames")); */
	return 0;
    } else {
	*alpha = 1000 * ((float) dt1 / dt2);
    }
    /* DBG(("delta frames: %u us, dt1: %u us, alpha: %d", 
	 dt2, dt1, *alpha));
    */
#if 0
    for (i = 0; i < MAX_BUFFERS; i++) {
	long ddd;
	ddd = buffers[shell][i].next->stamp - buffers[shell][i].stamp;
	DBG(("shell %d: from %d, i %d  stamp %ld  diff %ld", shell,
	     np2, i, buffers[shell][i].stamp, ddd));
    }
#endif
	
    *to = np;
    *from = np2;
    return ret;
}

static void *dthread_func(void *arg)
{
    static struct timespec now, to;
    /* static struct timespec t1; */
    
    int ret;
    DBG(("Display thread started..."));
    
    while (emu_running) {
	if (pthread_mutex_lock(&mutex) < 0) {
	    log_debug("pthread_mutex_lock() failed, %s", __FUNCTION__);
	    exit (-1);
	}
	while (do_action == CR_NOTHING) {
	    clock_gettime(CLOCK_REALTIME, &now);
	    tsAdd(&now, &reltime, &to);
	    ret = pthread_cond_timedwait(&cond, &mutex, &to);
	    if (ret == ETIMEDOUT) {
		do_action = CR_REDRAW;
	    }
	    if (ret < 0) {
		log_debug("pthread_cond_wait() failed, %s", __FUNCTION__);
		exit (-1);
	    }
	}
	DBG2(("action is: %d, %ld", do_action, TS_TOUSEC(now)/1000));
	
	if (do_action == CR_REDRAW) {
    	    int from, to, alpha, shell;
	    
	    do_action = CR_NOTHING;
	    pthread_mutex_unlock(&mutex);
	    /* clock_gettime(CLOCK_REALTIME, &t1); */
	    /* find frame to time 'now' as this is best in sync with the display
	       refresh cycle, in case vblank synchronization is active */
	    for (shell = 0; shell <= app_shell_count; shell++) {
		if (dthread_calc_frames(TS_TOUSEC(now), &from, &to, &alpha, shell)) {
		    gl_render_canvas(bptrs[shell].widget, bptrs[shell].canvas, 
				     buffers[shell], from, to, alpha, 
				     shell == get_active_shell());
		    bptrs[shell].lpos = from; /* set to `from' as a frame may be drawn twice */
#if 0
		    /* timing probe */
		    {
			static struct timespec t2, t3;
			
			clock_gettime(CLOCK_REALTIME, &t2);
			long diff = TS_TOUSEC(t2) - TS_TOUSEC(t1);
			float fps = 1000 * 1000.0 / (TS_TOUSEC(t1) - TS_TOUSEC(t3));
			DBG(("glrender time: %5ldus  fps %3.2f", diff, fps));
			memcpy(&t3, &t1, sizeof(struct timespec));
		    }
#endif
		}
	    }
	    continue;
	} else if (do_action == CR_CANVAS_WIDGET) {
	    is_coroutine = 1;
	    build_screen_canvas_widget2(canvas);
	} else if (do_action == CR_OPEN_CANVAS) {
	    is_coroutine = 1;
	    int_ret = ui_open_canvas_window2(canvas, title, width, 
					     height, no_autorepeat);
	} else if (do_action == CR_INIT) {
	    is_coroutine = 1;
	    (void) ui_init2(argc, argv);
	} else if (do_action == CR_INIT_FINISH) {
	    is_coroutine = 1;
	    int_ret = ui_init_finish2();
	} else if (do_action == CR_CONFIGURE_CALLBACK) {
	    is_coroutine = 1;
	    int_ret = configure_callback_canvas2(widget, event, client_data);
	} else if (do_action == CR_RESIZE) {
	    is_coroutine = 1;
	    ui_trigger_resize2();
	} else if (do_action == CR_WINDOW_RESIZE) {
	    is_coroutine = 1;
	    ui_trigger_window_resize2(canvas);
	} else if (do_action == CR_SHUTDOWN) {
	    emu_running = 0;
	}
	if (is_coroutine) {
	    DBG2(("syncronised call for action: %d- intermediate", do_action));
	    if (pthread_cond_signal(&coroutine) < 0) {
		log_debug("pthread_cond_signal() failed, %s", __FUNCTION__);
		exit (-1);
	    }
	}
	do_action = CR_NOTHING;
	if (pthread_mutex_unlock(&mutex) < 0) {
	    log_debug("pthread_mutex_unlock() failed, %s", __FUNCTION__);
	    exit (-1);
	}
    }
    exit(0);
}

static void dthread_coroutine(coroutine_t action) 
{
    struct timespec ts;
    int ret;

    if (pthread_mutex_lock(&mutex) < 0) {
	log_debug("pthread_mutex_lock() failed, %s", __FUNCTION__);
	exit (-1);
    }
  retry:    
    do_action = action;
    DBG2(("syncronised call for action: %d - start", action));
    if (pthread_cond_signal(&cond) < 0) {
	log_debug("pthread_cond_signal() failed, %s", __FUNCTION__);
	exit (-1);
    }
    while (!is_coroutine) {
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 3;
	DBG2(("syncronised call for action: %d - before condwait", action));
	ret = pthread_cond_timedwait(&coroutine, &mutex, &ts);
	if (ret == ETIMEDOUT) {
	    DBG2(("synchronized call for action %d - retrying", action));
	    goto retry;
	}
	if (ret < 0) {
	    log_debug("pthread_cond_timedwait() failed, %s", __FUNCTION__);
	    exit (-1);
	}
    }
    
    is_coroutine = 0;
    if (pthread_mutex_unlock(&mutex) < 0) {
	log_debug("pthread_mutex_unlock() failed, %s", __FUNCTION__);
	exit (-1);
    }
    DBG2(("syncronised call for action: %d - done", action));
}

static void *ethread_func(void *arg)
{
    int ret;
    
    /* this thread takes only care on gtk+ events */
    DBG(("GUI Event handler thread started..."));
    while (emu_running) {
	ret = sem_wait(&ethread_sem);
	if (ret < 0) {
	    if (errno == EINTR) {
		continue; /* dont' ask */
	    }
	    log_debug("sem_wait() failed, %s", __FUNCTION__);
	    exit (-1);
	    
	}
	ui_dispatch_events2();
	if (sem_post(&mthread_sem) != 0) {
	    log_debug("sem_wait() failed, %s", __FUNCTION__);
	    exit (-1);
	}
    }
    exit(0);
}
