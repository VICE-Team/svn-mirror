/*
 * cascom_hdlc.h - HDLC (AX.25) framing for the cascom modem network gateway.
 *
 * Written by
 *  Felix Erckenbrecht <code@eligs.de>
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

/* Pure, dependency-free (only <stdint.h>/<string.h>) HDLC bit-level framing as
 * used by AX.25: flag (0x7E) delimiting, bit-stuffing, LSB-first byte order and
 * the CRC-CCITT/X.25 FCS. Operates on the RAW (already de-NRZI'd) bit stream;
 * NRZI is handled by the caller. Self-contained so it is unit-testable without
 * VICE.
 */

#ifndef VICE_CASCOM_HDLC_H
#define VICE_CASCOM_HDLC_H

#include <stdint.h>

/* AX.25: up to 256 info + 2*7 (+digis) address + control + pid; 330 is ample */
#define CASCOM_HDLC_MAXFRAME 330

/* CRC-CCITT / X.25 HDLC FCS over 'len' bytes (LSB-first, init 0xFFFF, poly
   0x8408). The on-wire FCS is this value XOR 0xFFFF, sent low byte first. */
uint16_t cascom_hdlc_fcs(const uint8_t *data, int len);

/* ---- encoder: AX.25 frame bytes -> raw HDLC bit stream ---- */
typedef void (*cascom_hdlc_put_bit_t)(void *ctx, int bit);

/* Emit preamble_flags opening flags, then the frame + FCS (bit-stuffed,
   LSB-first), then one closing flag, calling put(ctx, bit) for each raw bit. */
void cascom_hdlc_encode(const uint8_t *frame, int len, int preamble_flags,
                        cascom_hdlc_put_bit_t put, void *ctx);

/* ---- decoder: raw bit stream -> validated, FCS-stripped frames ---- */
typedef struct cascom_hdlc_rx_s {
    int active;                              /* between flags */
    int ones;                                /* consecutive 1-bits */
    int bitcount;                            /* bits in the current byte */
    uint8_t cur;                             /* byte under assembly (LSB first) */
    int nbytes;                              /* bytes accumulated (incl. FCS) */
    uint8_t frame[CASCOM_HDLC_MAXFRAME + 2];
} cascom_hdlc_rx_t;

void cascom_hdlc_rx_init(cascom_hdlc_rx_t *r);

/* Feed one raw bit. When a flag closes a frame with a valid FCS, the frame
   (without the 2 FCS bytes) is copied into 'out' (clamped to outmax) and its
   length is returned; otherwise returns 0. */
int cascom_hdlc_rx_bit(cascom_hdlc_rx_t *r, int bit, uint8_t *out, int outmax);

#endif
