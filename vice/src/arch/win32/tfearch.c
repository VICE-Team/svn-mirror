/*
 * tfearch.c - TFE ("The final ethernet") emulation, 
 *                 architecture-dependant stuff
 *
 * Written by
 *  Spiro Trikaliotis <trik-news@gmx.de>
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

#ifdef HAVE_TFE 

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "tfearch.h"
#include "utils.h"


/**/ #define TFE_DEBUG_ARCH 1 /**/

#define TFE_DEBUG_WARN 1 /* this should not be deactivated */

/* ------------------------------------------------------------------------- */
/*    variables needed                                                       */


static log_t tfe_arch_log = LOG_ERR;

/* ------------------------------------------------------------------------- */
/*    the architecture-dependend functions                                   */

void tfe_arch_init(void)
{
    tfe_arch_log = log_open("TFEARCH");
}

void tfe_arch_pre_reset( void )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_pre_reset()." );
#endif
}

void tfe_arch_post_reset( void )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_post_reset()." );
#endif
}

void tfe_arch_activate( void )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_activate()." );
#endif
}

void tfe_arch_deactivate( void )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_deactivate()." );
#endif
}

void tfe_arch_set_mac( BYTE mac[6] )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "New MAC address set: %02X:%02X:%02X:%02X:%02X:%02X.",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
#endif
}

/*
void tfe_arch_receive_remove_committed_frame(void)
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_receive_remove_committed_frame()." );
#endif
}
*/

void tfe_arch_recv_ctl( bBroadcast,   /* broadcast */
                        bIA,          /* individual address (IA) */
                        bMulticast,   /* multicast if address passes the hash filter */
                        bCorrect,     /* accept correct frames */
                        bPromiscuous, /* promiscuous mode */
                        bIAHash       /* accept if IA passes the hash filter */
                      )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_recv_ctl() called with the following parameters:" );
    log_message( tfe_arch_log, "\tbBroadcast   = %s", bBroadcast   ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbIA          = %s", bIA          ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbMulticast   = %s", bMulticast   ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbCorrect     = %s", bCorrect     ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbPromiscuous = %s", bPromiscuous ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbIAHash      = %s", bIAHash      ? "TRUE" : "FALSE" );
#endif
}

void tfe_arch_line_ctl( bEnableTransmitter, bEnableReceiver )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_line_ctl() called with the following parameters:" );
    log_message( tfe_arch_log, "\tbEnableTransmitter = %s", bEnableTransmitter ? "TRUE" : "FALSE" );
    log_message( tfe_arch_log, "\tbEnableReceiver    = %s", bEnableReceiver    ? "TRUE" : "FALSE" );
#endif
}


/* the following function polls if there is a frame received

   If there's none, it returns a -1.

   If there is one, it returns the length of the frame in bytes.
*/
int tfe_arch_poll_receive( void )
{
    int ret = -1;

#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_poll_receive() called, returns %d.", ret );
#endif

    return ret;
}


/* the following function receives a frame.

   The caller should not call it unless tfe_arch_poll_receive()
   indicated the availability of a frame (with a return value >= 0).
   The given buffer must have enough memory for storing the frame;
   else, the behaviour of this function is undefined.

   It copies the frame to *buffer and returns the number of copied 
   bytes as return value.
*/
extern
int tfe_arch_receive_frame( BYTE *buffer )
{
    int ret = 0;

#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_receive_frame() called, returns %d.", ret );
#endif

    return ret;
}

void tfe_arch_transmit(int force,       /* FORCE: Delete waiting frames in transmit buffer */
                       int onecoll,     /* ONECOLL: Terminate after just one collision */
                       int inhibit_crc, /* INHIBITCRC: Do not append CRC to the transmission */
                       int tx_pad_dis,  /* TXPADDIS: Disable padding to 60 Bytes */
                       int txlength,    /* Frame length */
                       BYTE *txframe    /* Pointer to the frame to be transmitted */
                      )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_transmit() called, with: "
        "force = %s, onecoll = %s, inhibit_crc=%s, tx_pad_dis=%s, txlength=%u",
        force ?       "TRUE" : "FALSE", 
        onecoll ?     "TRUE" : "FALSE", 
        inhibit_crc ? "TRUE" : "FALSE", 
        tx_pad_dis ?  "TRUE" : "FALSE", 
        txlength
        );
#endif
}

/*
  tfe_arch_receive()

  This function checks if there was a frame received.
  If so, it returns TRUE, else FALSE.

  If there was no frame, none of the parameters is changed!

  If there was a frame, the following actions are done:

  - at maximum *plen byte are transferred into the buffer given by pbuffer
  - *plen gets the length of the received frame, EVEN if this is more
    than has been copied to pbuffer!
  - if the dest. address was accepted by the hash filter, *phashed is set, else
    cleared.
  - if the dest. address was accepted by the hash filter, *phash_index is
    set to the number of the rule leading to the acceptance
  - if the receive was ok (good CRC and valid length), *prx_ok is set, 
    else cleared.
  - if the dest. address was accepted because it's exactly our MAC address
    (set by tfe_arch_set_mac()), *pcorrect_mac is set, else cleared.
  - if the dest. address was accepted since it was a broadcast address,
    *pbroadcast is set, else cleared.
  - if the received frame had a crc error, *pcrc_error is set, else cleared
*/
int tfe_arch_receive(BYTE *pbuffer  ,    /* where to store a frame */
                     int  *plen,         /* IN: maximum length of frame to copy; 
                                            OUT: length of received frame 
                                            OUT can be bigger than IN if received frame was
                                                longer than supplied buffer */
                     int  *phashed,      /* set if the dest. address is accepted by the hash filter */
                     int  *phash_index,  /* hash table index if hashed == TRUE */   
                     int  *prx_ok,       /* set if good CRC and valid length */
                     int  *pcorrect_mac, /* set if dest. address is exactly our IA */
                     int  *pbroadcast,   /* set if dest. address is a broadcast address */
                     int  *pcrc_error    /* set if received frame had a CRC error */
                    )
{
#ifdef TFE_DEBUG_ARCH
    log_message( tfe_arch_log, "tfe_arch_receive() called, with *plen=%u.", *plen );
#endif

    return 0;
}

#endif /* #ifdef HAVE_TFE */
