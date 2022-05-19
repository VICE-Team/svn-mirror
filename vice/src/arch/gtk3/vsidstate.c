/** \file   vsidstate.c
 * \brief   VSID state module
 *
 * This module is a central place to keep track of the state of the VSID UI and
 * handle communication between the VICE thread and the UI thread.
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

#include "vice.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "lib.h"

#include "vsidstate.h"


/** \brief  UI state object for threading, do NOT use directly
 *
 * Do not use directly, but use lock_vsid_state()/unlock_vsid_state() to access.
 */
vsid_state_t vsid_state_hands_off;


/** \brief  Mutex to handle UI update requests from the VICE thread.
 */
static pthread_mutex_t vsid_lock = PTHREAD_MUTEX_INITIALIZER;



/** \brief  Get the lock on the VSID state
 *
 * \return  VSID state reference
 */
vsid_state_t *vsid_state_lock(void)
{
    pthread_mutex_lock(&vsid_lock);
    return &vsid_state_hands_off;
}


/** \brief  Unlock the VSID state
 */
void vsid_state_unlock(void)
{
    pthread_mutex_unlock(&vsid_lock);
}


/** \brief  Initialize UI state object
 */
void vsid_state_init(void)
{
    /* during setup we can use the object directly */
    vsid_state_t *state = &vsid_state_hands_off;

    /* Let's be explicit and not rely on automatic initialization to 0/NULL,
     * when the meaning of 0/NULL for a member changes we're in for some weird
     * bugs.
     */
    state->psid_filename = NULL;
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
void vsid_state_shutdown(void)
{
    vsid_state_t *state = vsid_state_lock();

    if (state->psid_filename != NULL) {
        lib_free(state->psid_filename);
        state->psid_filename = NULL;
    }
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

    vsid_state_unlock();
}
