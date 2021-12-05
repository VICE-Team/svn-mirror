/** \file   vsidstate.h
 * \brief   VSID UI state module - header
 *
 * This module is a central place to keep track of the state of the VSID UI and
 * handle communication between the VICE thread and the UI thread.
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

#ifndef VICE_VSIDSTATE_H
#define VICE_VSIDSTATE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


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


vsid_state_t *  vsid_state_lock    (void);
void            vsid_state_unlock  (void);
void            vsid_state_init    (void);
void            vsid_state_shutdown(void);

#endif
