/*
 * tfearch.h - TFE ("The final ethernet") emulation.
 *                 architecture-dependant stuff
 *
 * Written by
 *  Spiro Trikaliotis <Spiro.Trikaliotis@gmx.de>
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

#ifndef VICE_TFEARCH_H
#define VICE_TFEARCH_H

#include "types.h"

extern int tfe_arch_init(void);
extern void tfe_arch_pre_reset(void);
extern void tfe_arch_post_reset(void);
extern int tfe_arch_activate(const char *interface_name);
extern void tfe_arch_deactivate(void);
extern void tfe_arch_set_mac(const BYTE mac[6]);
extern void tfe_arch_set_hashfilter(const DWORD hash_mask[2]);

extern void tfe_arch_recv_ctl(int bBroadcast, int bIA, int bMulticast, int bCorrect, int bPromiscuous, int bIAHash);

extern void tfe_arch_line_ctl(int bEnableTransmitter, int bEnableReceiver);

extern void tfe_arch_transmit(int force, int onecoll, int inhibit_crc, int tx_pad_dis, int txlength, BYTE *txframe);

extern int tfe_arch_receive(BYTE *pbuffer, int *plen, int *phashed, int *phash_index, int *prx_ok, int *pcorrect_mac, int *pbroadcast, int *pcrc_error);

/*
 This is a helper for tfe_receive() to determine if the received frame should be accepted
 according to the settings.

 This function is even allowed to be called in tfearch.c from tfe_arch_receive() if 
 necessary, which is the reason why its prototype is included here in tfearch.h.
*/
extern int tfe_should_accept(unsigned char *buffer, int length, int *phashed, int *phash_index, int *pcorrect_mac, int *pbroadcast, int *pmulticast);

extern int tfe_arch_enumadapter_open(void);
extern int tfe_arch_enumadapter(char **ppname, char **ppdescription);
extern int tfe_arch_enumadapter_close(void);

#endif
