/*
 * rsuser.c - Daniel Dallmann's 9600 baud RS232 userport interface.
 *
 * Written by
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
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
/*
 * This is a very crude emulation. It does not check for a lot of things.
 * It simply tries to work with existing programs that work on the real
 * machine and does not try to catch rogue attempts...
 */

/* FIXME: Should keep its own log.  */

#include "vice.h"

#include <stdio.h>

#include "alarm.h"
#include "clkguard.h"
#include "cmdline.h"
#include "log.h"
#include "maincpu.h"
#include "resources.h"
#include "rs232drv.h"
#include "rsuser.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"


static int fd = -1;

static alarm_t *rsuser_alarm = NULL;

static int dtr;
static int rts;

static int rxstate;
static BYTE rxdata;
static BYTE txdata;
static BYTE txbit;

static long cycles_per_sec = 1000000;

static CLOCK clk_start_rx = 0;
static CLOCK clk_start_tx = 0;
static CLOCK clk_start_bit = 0;

static void (*start_bit_trigger)(void);
static void (*byte_rx_func)(BYTE);

static void clk_overflow_callback(CLOCK sub, void *data);

static void int_rsuser(CLOCK offset, void *data);

#undef DEBUG

#define RSUSER_TICKS    21111

/***********************************************************************
 * resource handling
 */

int rsuser_enabled = 0;                 /* enable flag */
static int rsuser_baudrate = 300;       /* saves the baud rate given */
static int char_clk_ticks = 0;          /* clk ticks per character */
static int bit_clk_ticks = 0;           /* clk ticks per character */

static int rsuser_device;

static void calculate_baudrate(void)
{
    if (rsuser_enabled) {
        char_clk_ticks = (int)(10.0 * cycles_per_sec
                         / ((double)rsuser_baudrate));
    } else {
        char_clk_ticks = RSUSER_TICKS;
    }
    bit_clk_ticks = (int)((double)(char_clk_ticks) / 10.0);

#ifdef DEBUG
    log_debug("RS232: %d cycles per char (cycles_per_sec=%ld).",
              char_clk_ticks, cycles_per_sec);
#endif
}

static int set_enable(resource_value_t v, void *param)
{
    int newval = ((int)v);

    if (newval && !rsuser_enabled) {
        dtr = DTR_OUT;  /* inactive */
        rts = RTS_OUT;  /* inactive */
        fd = -1;
    }
    if (rsuser_enabled && !newval) {
        if (fd != -1) {
            /* if(clk_start_tx) rs232drv_putc(fd, rxdata); */
            rs232drv_close(fd);
        }
        alarm_unset(rsuser_alarm);
        fd = -1;
    }

    rsuser_enabled = newval;

    calculate_baudrate();

    return 0;
}

static int set_baudrate(resource_value_t v, void *param)
{
    rsuser_baudrate = (int)v;

    calculate_baudrate();

    return 0;
}

static int set_up_device(resource_value_t v, void *param)
{
    rsuser_device = (int) v;
    if (fd != -1) {
        rs232drv_close(fd);
        fd = rs232drv_open(rsuser_device);
    }
    return 0;
}

static const resource_t resources[] = {
    { "RsUserEnable", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_STRICT, (resource_value_t)0,
      (void *)&rsuser_enabled, set_enable, NULL },
    { "RsUserBaud", RES_INTEGER, (resource_value_t)300,
      RES_EVENT_NO, NULL,
      (void *)&rsuser_baudrate, set_baudrate, NULL },
    { "RsUserDev", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_NO, NULL,
      (void *)&rsuser_device, set_up_device, NULL },
    { NULL }
};

int rsuser_resources_init(void)
{
    return resources_register(resources);
}

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] = {
    { "-rsuser", SET_RESOURCE, 0, NULL, NULL, "RsUserEnable", (void *)1,
      0, IDCLS_ENABLE_RS232_USERPORT },
    { "+rsuser", SET_RESOURCE, 0, NULL, NULL, "RsUserEnable", (void *)0,
      0, IDCLS_DISABLE_RS232_USERPORT },
    { "-rsuserbaud", SET_RESOURCE, 1, NULL, NULL, "RsUserBaud",
      (resource_value_t)300, IDCLS_P_BAUD,
      IDCLS_SET_BAUD_RS232_USERPORT },
    { "-rsuserdev", SET_RESOURCE, 1, NULL, NULL, "RsUserDev",
      (resource_value_t)0,
      IDCLS_P_0_3, IDCLS_SPECIFY_RS232_DEVICE_USERPORT },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-rsuser", SET_RESOURCE, 0, NULL, NULL, "RsUserEnable", (void *)1,
      NULL, N_("Enable RS232 userport emulation") },
    { "+rsuser", SET_RESOURCE, 0, NULL, NULL, "RsUserEnable", (void *)0,
      NULL, N_("Disable RS232 userport emulation") },
    { "-rsuserbaud", SET_RESOURCE, 1, NULL, NULL, "RsUserBaud",
      (resource_value_t)300, N_("<baud>"),
      N_("Set the baud rate of the RS232 userport emulation.") },
    { "-rsuserdev", SET_RESOURCE, 1, NULL, NULL, "RsUserDev",
      (resource_value_t)0,
      "<0-3>", N_("Specify VICE RS232 device for userport") },
    { NULL }
};
#endif

int rsuser_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/*********************************************************************/

static unsigned char code[256];
static unsigned int buf;
static unsigned int valid;
static const unsigned int masks[] =
    { 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800,
      0x1000, 0x2000, 0x4000, 0x8000 };

void rsuser_init(long cycles, void (*startfunc)(void), void (*bytefunc)(BYTE))
{
    int i, j;
    unsigned char c, d;

    rsuser_alarm = alarm_new(maincpu_alarm_context, "RSUser", int_rsuser, NULL);

    clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback, NULL);

    cycles_per_sec = cycles;
    calculate_baudrate();

    start_bit_trigger = startfunc;
    byte_rx_func = bytefunc;

    for (i = 0; i < 256; i++) {
        c = i; d = 0;
        for (j = 0 ; j < 8; j++) {
            d <<= 1;
            if (c & 1)
                d |= 1;
            c >>= 1;
        }
        code[i] = d;
    }

    dtr = DTR_OUT;      /* inactive */
    rts = RTS_OUT;      /* inactive */
    fd = -1;

    buf = (unsigned int)(~0); /* all 1s */
    valid = 0;
}

void rsuser_reset(void)
{
    rxstate = 0;
    clk_start_rx = 0;
    clk_start_tx = 0;
    clk_start_bit = 0;
    if (fd != -1) {
        rs232drv_close(fd);
    }

    alarm_unset(rsuser_alarm);
    fd = -1;
}

static void rsuser_setup(void)
{
    rxstate = 0;
    clk_start_rx = 0;
    clk_start_tx = 0;
    clk_start_bit = 0;
    fd = rs232drv_open(rsuser_device);
    alarm_set(rsuser_alarm, maincpu_clk + char_clk_ticks / 8);
}

void rsuser_write_ctrl(BYTE b)
{
    int new_dtr = b & DTR_OUT;  /* = 0 is active, != 0 is inactive */
    int new_rts = b & RTS_OUT;  /* = 0 is active, != 0 is inactive */

    if (rsuser_enabled) {
        if (dtr && !new_dtr) {
            rsuser_setup();
        }
        if (new_dtr && !dtr && fd != -1) {
#if 0   /* This is a bug in the X-line handshake of the C64... */
#ifdef DEBUG
            log_message(LOG_DEBUG, "switch rs232 off.");
#endif
            alarm_unset(rsuser_alarm);
            rs232drv_close(fd);
            fd = -1;
#endif
        }
    }

    dtr = new_dtr;
    rts = new_rts;
}

static void check_tx_buffer(void)
{
    BYTE c;

    while (valid >= 10 && (buf & masks[valid-1]))
        valid--;

    if (valid >= 10) {     /* (valid-1)-th bit is not set = start bit! */
        if (!(buf & masks[valid - 10])) {
            log_error(LOG_DEFAULT, "Frame error!");
        } else {
            c = (buf >> (valid - 9)) & 0xff;
            if (fd != -1) {
#ifdef DEBUG
                log_debug("\"%c\" (%02x).", code[c], code[c]);
#endif
                rs232drv_putc(fd, ((BYTE)(code[c])));
            }
        }
        valid -= 10;
    }
}

static void keepup_tx_buffer(void)
{

    if ((!clk_start_bit) || maincpu_clk < clk_start_bit)
        return;

    while(clk_start_bit < (clk_start_tx + char_clk_ticks)) {
#ifdef DEBUG
        log_debug("keepup: clk=%d, _bit=%d (%d), _tx=%d.",
                  clk, clk_start_bit-clk_start_tx, clk_start_bit,
                  clk_start_tx);
#endif
        buf= buf << 1;
        if (txbit)
            buf |= 1;
        valid ++;
        if (valid >= 10)
            check_tx_buffer();

        clk_start_bit += bit_clk_ticks;

        if (clk_start_bit >= maincpu_clk)
           break;
    }
    if (clk_start_bit >= clk_start_tx + char_clk_ticks) {
        clk_start_tx = 0;
        clk_start_bit = 0;
    }
}

void rsuser_set_tx_bit(int b)
{
#ifdef DEBUG
    log_debug("rsuser_set_tx(clk=%d, clk_start_tx=%d, b=%d).",
              clk, clk_start_tx, b);
#endif

    if (fd == -1 || rsuser_baudrate > 2400) {
        clk_start_tx = 0;
        return;
    }

    /* feeds the output buffer with enough bits till clk */
    keepup_tx_buffer();
    txbit = b;

    if (!clk_start_tx && !b) {
        /* the clock where we start sampling - in the middle of the bit */
        clk_start_tx = maincpu_clk + (bit_clk_ticks / 2) ;
        clk_start_bit = clk_start_tx;
        txdata = 0;
    }
}

BYTE rsuser_get_rx_bit(void)
{
    int bit = 0, byte = 1;
    if (clk_start_rx) {
        byte = 0;
        bit = (maincpu_clk - clk_start_rx) / (bit_clk_ticks);
#ifdef DEBUG
        log_debug("read ctrl(_rx=%d, clk-start_rx=%d -> bit=%d)",
                  clk_start_rx, clk - clk_start_rx, bit);
#endif
        if (!bit) {
            byte = 0;   /* start bit */
        } else
        if (bit < 9) {    /* 8 data bits */
            byte = rxdata & (1 << (bit - 1));
            if (byte)
                byte = 1;
        } else {        /* stop bits */
            byte = 1;
        }
    }
    return byte;
}

BYTE rsuser_read_ctrl(void)
{
    return rsuser_get_rx_bit() | CTS_IN | (rsuser_baudrate > 2400 ? 0 : DCD_IN);
}

void rsuser_tx_byte(BYTE b)
{

    buf = (buf << 8) | b;
    valid += 8;

    check_tx_buffer();
}

static void int_rsuser(CLOCK offset, void *data)
{
    CLOCK rclk = maincpu_clk - offset;

    keepup_tx_buffer();

    switch (rxstate) {
      case 0:
        if (fd != -1 && rs232drv_getc(fd, &rxdata)) {
            /* byte received, signal startbit on flag */
            rxstate ++;
            if (start_bit_trigger)
                start_bit_trigger();
            clk_start_rx = rclk;
        }
        alarm_set(rsuser_alarm, maincpu_clk + char_clk_ticks);
        break;
      case 1:
        /* now byte should be in shift register */
        if (byte_rx_func)
            byte_rx_func((BYTE)(code[rxdata]));
        rxstate = 0;
        clk_start_rx = 0;
        alarm_set(rsuser_alarm, maincpu_clk + char_clk_ticks / 8);
        break;
    }
}

static void clk_overflow_callback(CLOCK sub, void *data)
{
    if (clk_start_tx)
        clk_start_tx -= sub;
    if (clk_start_rx)
        clk_start_rx -= sub;
    if (clk_start_bit)
        clk_start_bit -= sub;
}

