/*
 * cascom_hdlc.c - HDLC (AX.25) framing for the cascom modem network gateway.
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

#include "vice.h"

#include <string.h>

#include "cascom_hdlc.h"

#define HDLC_FLAG 0x7e

/* CRC-CCITT/X.25 FCS (reflected, poly 0x8408), one byte per step.
   Entry k is the 16-bit remainder of processing the byte k through the
   reflected polynomial, so the running CRC updates as
   crc = (crc >> 8) ^ cascom_fcstab[(crc ^ byte) & 0xff].
   This is the standard AX.25 fcstab */
static const uint16_t cascom_fcstab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78,
};

uint16_t cascom_hdlc_fcs(const uint8_t *data, int len)
{
    uint16_t crc = 0xffff;
    int i;

    for (i = 0; i < len; i++) {
        crc = (uint16_t)((crc >> 8) ^ cascom_fcstab[(crc ^ data[i]) & 0xff]);
    }
    return crc;
}

/* ------------------------------------------------------------------ */
/* encoder                                                            */
/* ------------------------------------------------------------------ */

static void hdlc_put_flag(cascom_hdlc_put_bit_t put, void *ctx)
{
    int i;
    for (i = 0; i < 8; i++) {
        put(ctx, (HDLC_FLAG >> i) & 1);          /* LSB first */
    }
}

static void hdlc_put_byte(uint8_t byte, int *ones,
                          cascom_hdlc_put_bit_t put, void *ctx)
{
    int i;
    for (i = 0; i < 8; i++) {
        int b = (byte >> i) & 1;                 /* LSB first */
        put(ctx, b);
        if (b) {
            if (++(*ones) == 5) {                /* bit-stuff after five 1s */
                put(ctx, 0);
                *ones = 0;
            }
        } else {
            *ones = 0;
        }
    }
}

void cascom_hdlc_encode(const uint8_t *frame, int len, int preamble_flags,
                        cascom_hdlc_put_bit_t put, void *ctx)
{
    uint16_t txfcs = (uint16_t)(cascom_hdlc_fcs(frame, len) ^ 0xffff);
    int ones = 0;
    int i;

    if (preamble_flags < 1) {
        preamble_flags = 1;
    }
    for (i = 0; i < preamble_flags; i++) {
        hdlc_put_flag(put, ctx);
    }
    for (i = 0; i < len; i++) {
        hdlc_put_byte(frame[i], &ones, put, ctx);
    }
    hdlc_put_byte((uint8_t)(txfcs & 0xff), &ones, put, ctx);
    hdlc_put_byte((uint8_t)((txfcs >> 8) & 0xff), &ones, put, ctx);
    hdlc_put_flag(put, ctx);
}

/* ------------------------------------------------------------------ */
/* decoder                                                            */
/* ------------------------------------------------------------------ */

void cascom_hdlc_rx_init(cascom_hdlc_rx_t *r)
{
    memset(r, 0, sizeof(*r));
}

static void hdlc_rx_store(cascom_hdlc_rx_t *r, int bit)
{
    r->cur = (uint8_t)((r->cur >> 1) | (bit ? 0x80u : 0u));   /* LSB first */
    r->bitcount++;
    if (r->bitcount == 8) {
        if (r->nbytes < CASCOM_HDLC_MAXFRAME + 2) {
            r->frame[r->nbytes++] = r->cur;
        }
        r->bitcount = 0;
        r->cur = 0;
    }
}

/* a flag closed the frame: validate the FCS and, if good, hand back the data */
static int hdlc_rx_finish(cascom_hdlc_rx_t *r, uint8_t *out, int outmax)
{
    int dlen = r->nbytes - 2;
    uint16_t txfcs;

    if (dlen < 1) {
        return 0;
    }
    txfcs = (uint16_t)(cascom_hdlc_fcs(r->frame, dlen) ^ 0xffff);
    if (r->frame[dlen] != (txfcs & 0xff) ||
        r->frame[dlen + 1] != ((txfcs >> 8) & 0xff)) {
        return 0;                                /* bad FCS - drop */
    }
    if (dlen > outmax) {
        dlen = outmax;
    }
    memcpy(out, r->frame, (size_t)dlen);
    return dlen;
}

int cascom_hdlc_rx_bit(cascom_hdlc_rx_t *r, int bit, uint8_t *out, int outmax)
{
    int result = 0;

    if (bit) {
        r->ones++;
        if (r->ones > 6) {
            /* 7+ ones: abort / idle - drop any frame in progress */
            r->active = 0;
            r->bitcount = 0;
            r->nbytes = 0;
            r->cur = 0;
            return 0;
        }
        if (r->ones <= 5 && r->active) {
            hdlc_rx_store(r, 1);
        }
        /* ones == 6: pending flag/abort, do not store */
        return 0;
    }

    /* bit == 0 */
    {
        int prev_ones = r->ones;
        r->ones = 0;

        if (prev_ones == 5) {
            return 0;                            /* stuffed zero - discard */
        }
        if (prev_ones == 6) {                    /* FLAG (0 111111 0) */
            if (r->active && r->nbytes >= 2) {
                result = hdlc_rx_finish(r, out, outmax);
            }
            r->active = 1;
            r->bitcount = 0;
            r->nbytes = 0;
            r->cur = 0;
            return result;
        }
        if (r->active) {
            hdlc_rx_store(r, 0);
        }
    }
    return result;
}
