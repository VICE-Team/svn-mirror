/*
 * tfearch.h - TFE ("The final ethernet") emulation.
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

#ifdef HAVE_TFE 
#else
  #error TFEARCH.H should not be included if HAVE_TFE is not defined!
#endif /* #ifdef HAVE_TFE */

#ifndef _TFEARCH_H
#define _TFEARCH_H

#include "types.h"

extern void tfe_arch_init(void);
extern void tfe_arch_pre_reset(void);
extern void tfe_arch_post_reset(void);
extern void tfe_arch_activate(void);
extern void tfe_arch_deactivate(void);
extern void tfe_arch_set_mac(BYTE mac[6]);

/* extern void tfe_arch_receive_remove_committed_frame(void); */

extern
void tfe_arch_recv_ctl( bBroadcast,   /* broadcast */
                        bIA,          /* individual address (IA) */
                        bMulticast,   /* multicast if address passes the hash filter */
                        bCorrect,     /* accept correct frames */
                        bPromiscuous, /* promiscuous mode */
                        bIAHash       /* accept if IA passes the hash filter */
                      );

extern
void tfe_arch_line_ctl(bEnableTransmitter, bEnableReceiver);

/* the following function polls if there is a frame received

   If there's none, it returns a -1.

   If there is one, it returns the length of the frame in bytes.
*/
extern
int tfe_arch_poll_receive(void);

/* the following function receives a frame.

   The caller should not call it unless tfe_arch_poll_receive()
   indicated the availability of a frame (with a return value >= 0).
   The given buffer must have enough memory for storing the frame;
   else, the behaviour of this function is undefined.

   It copies the frame to *buffer and returns the number of copied 
   bytes as return value.
*/
extern
int tfe_arch_receive_frame( BYTE *buffer );

extern
void tfe_arch_transmit(int force,       /* FORCE: Delete waiting frames in transmit buffer */
                       int onecoll,     /* ONECOLL: Terminate after just one collision */
                       int inhibit_crc, /* INHIBITCRC: Do not append CRC to the transmission */
                       int tx_pad_dis,  /* TXPADDIS: Disable padding to 60 Bytes */
                       int txlength,    /* Frame length */
                       BYTE *txframe    /* Pointer to the frame to be transmitted */
                      );

extern
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
                     );

/*
 This is a helper for tfe_receive() to determine if the received frame should be accepted
 according to the settings.

 This function is even allowed to be called in tfearch.c from tfe_arch_receive() if 
 necessary, which is the reason why its prototype is included here in tfearch.h.
*/
extern 
int tfe_should_accept(BYTE *buffer, int length, int *phashed, int *phash_index, 
                      int *pcorrect_mac, int *pbroadcast);


#endif
