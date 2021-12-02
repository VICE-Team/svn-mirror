/** \file   uivsidwindow.c
 * \brief   Native GTK3 main vsid window code
 *
 * \author  Marcus Sutton <loggedoubt@gmail.com>
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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

/* XXX: It should be possible to compile, link and run all emulators while this
 *      entire file (amongst others) is contained inside an #if 0 wrapper.
 *
 * Well, it doesn't, removing. --compyx
 */

#include "vice.h"

#include <gtk/gtk.h>

#include "hvsc.h"
#include "hvscstilwidget.h"
#include "lib.h"
#include "machine.h"
#include "mainlock.h"
#include "psid.h"
#include "ui.h"
#include "uiapi.h"
#include "uivsidmenu.h"
#include "vice_gtk3.h"
#include "videoarch.h"
#include "videomodelwidget.h"
#include "vsidcontrolwidget.h"
#include "vsidmainwidget.h"
#include "vsidmainwidget.h"
#include "vsidtuneinfowidget.h"
#include "vsidtuneinfowidget.h"
#include "vsidui.h"
#include "vsync.h"

#include "uivsidwindow.h"


/** \brief  VSID UI state object
 *
 * Used to handle communication between the main (VICE) thread and the UI thread.
 *
 * The `_pending` flags indicate a new value has arrived for a widget. There
 * are no flags for string values since non-NULL means a new value has arrived,
 * and no flag for the play time update, since we keep track of the old time
 * to avoid rendering the related widgets (play time/progress bar) again for
 * the same time.
 */
typedef struct vsid_state_s {
    /* PSID data */

    /** \brief  Author field of a PSID file */
    char *author;

    /** \brief  Copyright field of a PSID file
     *
     * This should be called 'released' in the UI.
     */
    char *copyright;

    /** \brief  IRQ type */
    char *irq;

    /** \brief  PSID tune name */
    char *name;

    /** \brief  Number of subtunes */
    int  tune_count;
    /** \brief  Number of subtunes needs to be updated in the UI */
    bool tune_count_pending;

    /** \brief  Current tune number */
    int  tune_current;
    /** \brief  Current tune number needs to be updated the UI */
    bool tune_current_pending;

    /** \brief  Default tune number */
    int  tune_default;
    /** \brief  Default tune number needs to be updated in the UI */
    bool tune_default_pending;

    /** \brief  SID model (0 = 6581, 1 = 8580) */
    int  model;
    /** \brief  SID model needs to be updated in the UI */
    bool model_pending;

    /** \brief  Sync factor (0 = 60Hz, 1 = 50Hz) */
    int  sync;
    /** \brief  Sync factor needs to be updated in the UI */
    bool sync_pending;

    /** \brief  Load address */
    uint16_t load_addr;
    /** \brief  Load address needs to be updated in the UI */
    bool     load_addr_pending;

    /** \brief  Init address */
    uint16_t init_addr;
    /** \brief  Init address needs to be updated in the UI */
    bool     init_addr_pending;

    /** \brief  Play address */
    uint16_t play_addr;
    /** \brief  Play address needs to be updated in the UI */
    bool     play_addr_pending;

    /** \brief  Size of the SID data, excluding PSID header */
    uint16_t data_size;
    /** \brief  Size needs to be updated in the UI */
    bool     data_size_pending;

    /*
     * VICE's driver
     */

    /** \brief  Current displayed play time
     *
     * Set in the UI thread.
     */
    unsigned int current_time;

    /** \brief  New play time
     *
     * Set in the main thread.
     *
     * When this differs from current_time the UI thread will update the time
     * widget and set current_time to this.
     */
    unsigned int new_time;

    /** \brief  Driver address */
    uint16_t driver_addr;
    /** \brief  Driver address needs to be updated in the UI */
    bool     driver_addr_pending;

} vsid_state_t;


/** \brief  Main widget of VSID
 */
static GtkWidget *main_widget = NULL;

static gulong frame_clock_update_handler;

/** \brief  UI state object for threading, do NOT use directly
 *
 * Do not use directly, but use lock_vsid_state()/unlock_vsid_state() to access.
 */
vsid_state_t vsid_state_hands_off;

/** \brief  Mutex to handle UI update requests from the VICE thread.
 */
static pthread_mutex_t vsid_state_lock = PTHREAD_MUTEX_INITIALIZER;



/** \brief  Get the lock on the VSID state
 *
 * \return  VSID state reference
 */
static vsid_state_t *lock_vsid_state(void)
{
    pthread_mutex_lock(&vsid_state_lock);
    return &vsid_state_hands_off;
}


/** \brief  Unlock the VSID state
 */
static void unlock_vsid_state(void)
{
    pthread_mutex_unlock(&vsid_state_lock);
}


/** \brief  Initialize UI state object
 */
static void init_vsid_state(void)
{
    /* during setup we can use the object directly */
    vsid_state_t *state = &vsid_state_hands_off;

    /* Let's be explicit and not rely on automatic initialization to 0/NULL,
     * when the meaning of 0/NULL for a member changes we're in for some weird
     * bugs.
     */
    state->name = NULL;
    state->author = NULL;
    state->copyright = NULL;
    state->irq = NULL;
    state->tune_count = 0;
    state->tune_count_pending = false;
    state->tune_current = 0;
    state->tune_current_pending = false;
    state->tune_default = 0;
    state->tune_default_pending = false;
    state->model = 0;
    state->model_pending = false;
    state->sync = 0;
    state->sync_pending = false;
    state->load_addr = 0;
    state->load_addr_pending = false;
    state->init_addr = 0;
    state->init_addr_pending = false;
    state->play_addr = 0;
    state->play_addr_pending = false;
    state->data_size = 0;
    state->data_size_pending = false;

    state->current_time = ~0;   /*  force rendering of play time once after */
    state->new_time = 0;        /*+ UI realization */
    state->driver_addr = 0;
    state->driver_addr_pending = false;
}


/** \brief  Clean up any resources used by the VSID state object
 */
static void shutdown_vsid_state(void)
{
    vsid_state_t *state = lock_vsid_state();

    if (state->name != NULL) {
        lib_free(state->name);
        state->name = NULL;
    }
    if (state->author != NULL) {
        lib_free(state->author);
        state->author = NULL;
    }
    if (state->copyright != NULL) {
        lib_free(state->copyright);
        state->copyright = NULL;
    }
    if (state->irq != NULL) {
        lib_free(state->irq);
        state->irq = NULL;
    }

    unlock_vsid_state();
}


/** \brief  UI update handler
 *
 */
static void vsid_ui_update(void)
{
    vsid_state_t *state = lock_vsid_state();

    /* play time */
    if (state->current_time != state->new_time) {
        vsid_tune_info_widget_set_time(state->new_time);
        state->current_time = state->new_time;
    }

    /* set tune title if requested */
    if (state->name != NULL) {
        vsid_tune_info_widget_set_name(state->name);
        lib_free(state->name);
        state->name = NULL;
    }
    /* set author if requested */
    if (state->author != NULL) {
        vsid_tune_info_widget_set_author(state->author);
        lib_free(state->author);
        state->author = NULL;
    }
    /* set copyright ('released') if requested */
    if (state->copyright != NULL) {
        vsid_tune_info_widget_set_copyright(state->copyright);
        lib_free(state->copyright);
        state->copyright = NULL;
    }

    /* tune count */
    if (state->tune_count_pending) {
        vsid_tune_info_widget_set_tune_count(state->tune_count);
        state->tune_count_pending = false;
    }
    /* current tune number */
    if (state->tune_current_pending) {
        vsid_tune_info_widget_set_tune_current(state->tune_current);
        state->tune_current_pending = false;
    }
    /* default tune number */
    if (state->tune_default_pending) {
        vsid_tune_info_widget_set_tune_default(state->tune_default);
        state->tune_default_pending = false;
    }

    /* SID model */
    if (state->model_pending) {
        vsid_tune_info_widget_set_model(state->model);
        state->model_pending = false;
    }
    /* IRQ source */
    if (state->irq != NULL) {
        vsid_tune_info_widget_set_irq(state->irq);
        lib_free(state->irq);
        state->irq = NULL;
    }
    /* sync factor */
    if (state->sync_pending) {
        vsid_tune_info_widget_set_sync(state->sync);
        state->sync_pending = false;
    }

    /* driver address in the emulated machine */
    if (state->driver_addr_pending) {
        vsid_tune_info_widget_set_driver_addr(state->driver_addr);
        state->driver_addr_pending = false;
    }
    /* SID load address in the emulated machine */
    if (state->load_addr_pending) {
        vsid_tune_info_widget_set_load_addr(state->load_addr);
        state->load_addr_pending = false;
    }
    /* SID init address in the emulated machine */
    if (state->init_addr_pending) {
        vsid_tune_info_widget_set_init_addr(state->init_addr);
        state->init_addr_pending = false;
    }
    /* SID play address in the emulated machine */
    if (state->play_addr_pending) {
        vsid_tune_info_widget_set_play_addr(state->play_addr);
        state->play_addr_pending = false;
    }
    /* SID data size */
    if (state->data_size_pending) {
        vsid_tune_info_widget_set_data_size(state->data_size);
        state->data_size_pending = false;
    }

    unlock_vsid_state();
}



/** \brief  Called once per frame for frequent asynchronous UI updates.
 * 
 * For example, vsid.c provides an updated play time from the VICE thread.
 * It's not safe for the VICE thread to call GTK code - so instead we just
 * store the value and update the UI each frame in here.
 */
static void on_frame_clock_update(GdkFrameClock *clock, gpointer user_data)
{
    ui_update_statusbars();
    vsid_main_widget_update();  /* this should be merged with in the function
                                   below, I think */
    vsid_ui_update();
}

static void on_widget_realized(GtkWidget *widget, gpointer data)
{
    GdkFrameClock *frame_clock;

    /* Use a GTK frame clock to periodically update the ui */
    frame_clock = gdk_window_get_frame_clock(gtk_widget_get_window(widget));
    frame_clock_update_handler = g_signal_connect_unlocked(frame_clock, "update", G_CALLBACK(on_frame_clock_update), NULL);
    gdk_frame_clock_begin_updating(frame_clock);
}

static void on_widget_unrealized(GtkWidget *widget, gpointer data)
{
    GdkFrameClock *frame_clock;

    frame_clock = gdk_window_get_frame_clock(gtk_widget_get_window(widget));
    g_signal_handler_disconnect(frame_clock, frame_clock_update_handler);
    gdk_frame_clock_end_updating(frame_clock);
    shutdown_vsid_state();
}


/** \brief  Create  VSID window
 *
 * \param[in]   canvas  something
 */
static void vsid_window_create(video_canvas_t *canvas)
{
    GtkWidget *menu_bar;

    canvas->renderer_backend = NULL;
    canvas->event_box = NULL;

    main_widget = vsid_main_widget_create();
    gtk_widget_set_size_request(main_widget, 400, 300);
    gtk_widget_set_hexpand(main_widget, TRUE);
    gtk_widget_set_vexpand(main_widget, TRUE);
    gtk_widget_show(main_widget);

    menu_bar = ui_vsid_menu_bar_create();
    gtk_container_add(GTK_CONTAINER(canvas->grid), menu_bar);
    gtk_container_add(GTK_CONTAINER(canvas->grid), main_widget);

    /* Set up the frame clock when the top level grid is realized. */
    g_signal_connect (canvas->grid, "realize", G_CALLBACK (on_widget_realized), NULL);
    g_signal_connect (canvas->grid, "unrealize", G_CALLBACK (on_widget_unrealized), NULL);
}


/** \brief  Load and play PSID/SID file \a filename
 *
 * \param[in]   filename    file to play
 *
 * \return  0 on success, -1 on failure
 */
int ui_vsid_window_load_psid(const char *filename)
{
    vsync_suspend_speed_eval();

    if (machine_autodetect_psid(filename) < 0) {
        ui_error("'%s' is not a valid PSID file.", filename);
        return -1;
    }
    psid_init_driver();
    machine_play_psid(0);
    machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
    vsid_tune_info_widget_set_song_lengths(filename);
    hvsc_stil_widget_set_psid(filename);
    ui_pause_disable();

    return 0;
}


/** \brief  Initialize VSID window
 */
void ui_vsid_window_init(void)
{
    init_vsid_state();
    ui_set_create_window_func(vsid_window_create);
    ui_set_handle_dropped_files_func(ui_vsid_window_load_psid);
}


/*
 * API for VICE thread to update the UI
 */

/** \brief  Display tune name in the UI
 *
 * \param[in]   name    tune name
 */
void vsid_ui_display_name(const char *name)
{
    vsid_state_t *state = lock_vsid_state();

    if (state->name == NULL) {
        state->name = lib_strdup(name);
    }
    unlock_vsid_state();

}


/** \brief  Display tune author in the UI
 *
 * \param[in]   author  author name
 */
void vsid_ui_display_author(const char *author)
{
    vsid_state_t *state = lock_vsid_state();

    if (state->author == NULL) {
        state->author = lib_strdup(author);
    }
    unlock_vsid_state();
}


/** \brief  Display tune copyright info in the UI
 *
 * \param[in]   copright    copyright info
 */
void vsid_ui_display_copyright(const char *copyright)
{
    vsid_state_t *state = lock_vsid_state();

    if (state->copyright == NULL) {
        state->copyright = lib_strdup(copyright);
    }
    unlock_vsid_state();
}


/** \brief  Set IRQ type for the UI
 *
 * \param[in]   irq IRQ type
 */
void vsid_ui_display_irqtype(const char *irq)
{
    vsid_state_t *state = lock_vsid_state();

    if (state->irq == NULL) {
        state->irq = lib_strdup(irq);
    }
    unlock_vsid_state();
}


/** \brief  Set SID model for the UI
 *
 * \param[in]   model   SID model
 */
void vsid_ui_display_sid_model(int model)
{
    vsid_state_t *state = lock_vsid_state();

    state->model = model;
    state->model_pending = true;
    unlock_vsid_state();
}


/** \brief  Set sync factor for the UI
 *
 * \param[in]   sync    sync factor
 */
void vsid_ui_display_sync(int sync)
{
    vsid_state_t *state = lock_vsid_state();

    state->sync = sync;
    state->sync_pending = true;
    unlock_vsid_state();
}


/** \brief  Set run time of tune in the UI
 *
 * \param[in]   sec seconds of play time
 */
void vsid_ui_display_time(unsigned int dsec)
{
    vsid_state_t *state = lock_vsid_state();

    state->new_time = dsec;
    unlock_vsid_state();
}


/** \brief  Set number of tunes for the UI
 *
 * \param[in]   count   number of tunes
 */
void vsid_ui_display_nr_of_tunes(int count)
{
    vsid_state_t *state = lock_vsid_state();

    state->tune_count = count;
    state->tune_count_pending = true;
    unlock_vsid_state();
}


/** \brief  Set default tune number in the UI
 *
 * \param[in]   nr  tune number
 */
void vsid_ui_set_default_tune(int nr)
{
    vsid_state_t *state = lock_vsid_state();

    state->tune_default = nr;
    state->tune_default_pending = true;
    unlock_vsid_state();

    vsid_main_widget_set_tune_default(nr);
}


/** \brief  Set current tune number in the UI
 *
 * \param[in]   nr  tune number
 */
void vsid_ui_display_tune_nr(int nr)
{
    vsid_state_t *state = lock_vsid_state();

    state->tune_current = nr;
    state->tune_current_pending = true;
    unlock_vsid_state();
}


/** \brief  Set driver info text for the UI
 *
 * \param[in]   driver_info_text    text with driver info (duh)
 */
void vsid_ui_setdrv(char *driver_info_text)
{
    /* NOP: replaced with more specific driver info elements */
}


/** \brief  Set driver address
 *
 * \param[in]   addr    driver address
 */
void vsid_ui_set_driver_addr(uint16_t addr)
{
    vsid_state_t *state = lock_vsid_state();

    state->driver_addr = addr;
    state->driver_addr_pending = true;
    unlock_vsid_state();
}


/** \brief  Set load address
 *
 * \param[in]   addr    load address
 */
void vsid_ui_set_load_addr(uint16_t addr)
{
    vsid_state_t *state = lock_vsid_state();

    state->load_addr = addr;
    state->load_addr_pending = true;
    unlock_vsid_state();
}


/** \brief  Set init routine address
 *
 * \param[in]   addr    init routine address
 */
void vsid_ui_set_init_addr(uint16_t addr)
{
    vsid_state_t *state = lock_vsid_state();

    state->init_addr = addr;
    state->init_addr_pending = true;
    unlock_vsid_state();
}


/** \brief  Set play routine address
 *
 * \param[in]   addr    play routine address
 */
void vsid_ui_set_play_addr(uint16_t addr)
{
    vsid_state_t *state = lock_vsid_state();

    state->play_addr = addr;
    state->play_addr_pending = true;
    unlock_vsid_state();
}


/** \brief  Set size of SID on actual machine
 *
 * \param[in]   size    size of SID
 */
void vsid_ui_set_data_size(uint16_t size)
{
    vsid_state_t *state = lock_vsid_state();

    state->data_size = size;
    state->data_size_pending = true;
    unlock_vsid_state();
}
