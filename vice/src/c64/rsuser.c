/*
 * rsuser.h - Daniel Dallmann's 9600 baud RS232 userport interface
 *
 * Written by
 *  André Fachat        (a.fachat@physik.tu-chemnitz.de)
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

#include <stdio.h>

#include "vice.h"
#include "types.h"
#include "cmdline.h"
#include "resources.h"
#include "rs232.h"
#include "rsuser.h"
#include "c64cia.h"
#include "interrupt.h"
#include "vmachine.h"

static int fd;

static int dtr;
static int rts;

static int rxstate;
static BYTE rxdata;
static BYTE txdata;
static BYTE txbit;

CLOCK clk_start_rx = 0;
CLOCK clk_start_tx = 0;
CLOCK clk_start_bit = 0;

#undef DEBUG

#define	RSUSER_TICKS	21111

/***********************************************************************
 * resource handling
 */

int rsuser_enabled = 0;		/* saves the baud rate given */
int char_clk_ticks = 0;		/* clk ticks per character */
int bit_clk_ticks = 0;		/* clk ticks per character */

static int rsuser_device;

static int set_up_enabled(resource_value_t v) {
    int newval = ((int) v);

    if(newval && !rsuser_enabled) {
	dtr = DTR_OUT;	/* inactive */
	rts = RTS_OUT;	/* inactive */
	fd = -1;
    }
    if(rsuser_enabled && !newval) {
	if(fd>=0) {
	    /* if(clk_start_tx) rs232_putc(fd, rxdata); */
	    rs232_close(fd);
	}
	maincpu_unset_alarm(A_RSUSER);
	fd = -1;
    }

    rsuser_enabled = newval;
    if(newval) {
	char_clk_ticks = 1000000.0 * (10.0 / (double)newval);
    } else {
	char_clk_ticks = RSUSER_TICKS;
    }
    bit_clk_ticks = char_clk_ticks / 10;
#ifdef DEBUG
    printf("RS232: %d cycles per char\n", char_clk_ticks);
#endif
    return 0;
}

static int set_up_device(resource_value_t v) {
    rsuser_device = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "RsUser", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &rsuser_enabled, set_up_enabled },
    { "RsUserDev", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &rsuser_device, set_up_device },
    { NULL }
};

int rsuser_init_resources(void) {
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-rsuser", SET_RESOURCE, 1, NULL, NULL, "RsUser",
        (resource_value_t) 0, "<baud>",
        "Enable the userport RS232 emulation baud=0: off; baud=9600: CIA interface." },
    { "-rsuserdev", SET_RESOURCE, 1, NULL, NULL, "RsUserDev",
        (resource_value_t) 0,
      "<0-2>", "Specify VICE RS232 device for userport" },
    { NULL }
};

int rsuser_init_cmdline_options(void) {
    return cmdline_register_options(cmdline_options);
}

/*********************************************************************/

static unsigned char code[256];
static unsigned int buf;
static unsigned int valid;
static unsigned int masks[] = {
	1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800,
	0x1000, 0x2000, 0x4000, 0x8000
};

void rsuser_init(void) {
	int i, j;
	unsigned char c,d;

	for(i=0;i<256;i++) {
	  c = i; d = 0;
	  for(j=0 ;j<8; j++) {
	    d<<=1;
	    if(c&1) d|=1;
	    c>>=1;
	  }
	  code[i] = d;
	}

	dtr = DTR_OUT;	/* inactive */
	rts = RTS_OUT;	/* inactive */
	fd = -1;

	buf = ~0;	/* all 1s */
	valid = 0;
}

void rsuser_reset(void) {
	rxstate = 0;
	clk_start_rx = 0;
	clk_start_tx = 0;
	clk_start_bit = 0;
	if(fd >= 0) {
	    rs232_close(fd);
	}
	maincpu_unset_alarm(A_RSUSER);
	fd = -1;

	maincpu_unset_alarm(A_RSUSER);
}

static void rsuser_setup(void) 
{
    /* switch rs232 on */
/*printf("switch rs232 on\n");*/
    rxstate = 0;
    clk_start_rx = 0;
    clk_start_tx = 0;
    clk_start_bit = 0;
    fd = rs232_open(rsuser_device);
    maincpu_set_alarm(A_RSUSER, char_clk_ticks / 8);
}

void userport_serial_write_ctrl(int b) {
    int new_dtr = b & DTR_OUT;	/* = 0 is active, != 0 is inactive */
    int new_rts = b & RTS_OUT;	/* = 0 is active, != 0 is inactive */

#ifdef DEBUG
printf("userport_serial_write_ctrl(b=%02x (dtr=%02x, rts=%02x)\n",
		b, new_dtr, new_rts);
#endif
    if(rsuser_enabled) {
        if(dtr && !new_dtr) {
	    rsuser_setup();
        }
        if(new_dtr && !dtr && fd>=0) {
/*printf("switch rs232 off\n");*/
#if 0	/* This is a bug in the X-line handshake of the C64... */
	    maincpu_unset_alarm(A_RSUSER);
            rs232_close(fd);
	    fd = -1;
#endif
        }
    }

    dtr = new_dtr;
    rts = new_rts;
}

static void check_tx_buffer(void) {
    BYTE c;

    while(valid >= 10 && (buf & masks[valid-1])) valid--;

    /* printf("rsuser_write_sr(%02x), buf=%x, valid=%d\n",b, buf, valid); */

    if(valid>=10) {	/* (valid-1)-th bit is not set = start bit! */
	if(!(buf & masks[valid-10])) {
	    fprintf(stderr, "frame error!\n");
	} else {
	    c = (buf >> (valid-9)) & 0xff;
	    /*printf("rsuser_send %c (%02x), buf=%x, valid=%d\n",
						code[c],code[c], buf, valid);*/
	    if(fd>=0) {
#ifdef DEBUG
		printf("\"%c\"", code[c]);
#endif
		rs232_putc(fd, code[c]);
	    }
	}
	valid -= 10;
    }
}

static void keepup_tx_buffer(void) {

    if((!clk_start_bit) || clk < clk_start_bit) return;

    while(clk_start_bit < (clk_start_tx + char_clk_ticks)) {
#ifdef DEBUG
	printf("keeup: clk=%d, _bit=%d, _tx=%d\n",
		clk, clk_start_bit-clk_start_tx, clk_start_tx);
#endif
	buf= buf<< 1;
	if(txbit) buf|= 1;
	valid ++;
	if(valid >=10) check_tx_buffer();

	clk_start_bit += bit_clk_ticks;

	if(clk_start_bit >= clk) break;
    }
    if(clk_start_bit > clk_start_tx + char_clk_ticks) {
	clk_start_tx = 0;
	clk_start_bit = 0;
    }
}

void rsuser_set_tx_bit(int b) {
    int bit=0;
#ifdef DEBUG
    printf("rsuser_set_tx(clk=%d, clk_start_tx=%d, b=%d)\n", 
		clk, clk_start_tx, b);
#endif

    if(fd<0 || rsuser_enabled > 2400) {
	clk_start_tx = 0;
	return;
    }

    /* feeds the output buffer with enough bits till clk */
    keepup_tx_buffer();
    txbit = b;
 
    if(!clk_start_tx && !b) {
	/* the clock where we start sampling - in the middle of the bit */
	clk_start_tx = clk + (bit_clk_ticks / 2) ;
	clk_start_bit = clk_start_tx;
	txdata = 0;
#ifdef DEBUG
printf("\n");
#endif
    }
}

BYTE rsuser_get_rx_bit(void) {
    int bit=0, byte=1;
    if(clk_start_rx) {
	byte = 0;
	bit = (clk - clk_start_rx)/(bit_clk_ticks);
#ifdef DEBUG
	printf("read ctrl(clk-start_rx=%d -> bit=%d)\n",clk-clk_start_rx, bit);
#endif
	if(!bit) {
	    byte = 0;	/* start bit */
	} else 
	if(bit<9) {	/* 8 data bits */
	    byte = rxdata & (1 << (bit-1));
	    if(byte) byte=1;
	} else {	/* stop bits */
	    byte = 1;
	}
    }
    return byte;
}

BYTE userport_serial_read_ctrl(void) {
    return rsuser_get_rx_bit() | CTS_IN | (rsuser_enabled > 2400 ? 0 : DCD_IN);
}

void userport_serial_write_sr(BYTE b) {

    buf = (buf << 8) | b;
    valid += 8;

    check_tx_buffer();
}



int int_rsuser(long offset) {
	CLOCK rclk = clk - offset;
#if 0 /* def DEBUG */
        printf("int_rsuser(clk=%d, rclk=%ld)\n",clk, clk-offset);
#endif

        keepup_tx_buffer();

	switch(rxstate) {
	case 0:
        	if( fd>=0 && rs232_getc(fd, &rxdata)) {
		  /* byte received, signal startbit on flag */
                  rxstate ++;
		  cia2_set_flag();
		  clk_start_rx = rclk;
		}
		maincpu_set_alarm(A_RSUSER, char_clk_ticks);
		break;
	case 1:
		/* now byte should be in shift register */
		cia2_set_sdr(code[rxdata]);
		rxstate = 0;
		clk_start_rx = 0;
		maincpu_set_alarm(A_RSUSER, char_clk_ticks / 8);
		break;
        }

        return 0;
}

void rsuser_prevent_clk_overflow(CLOCK sub) 
{
    if(clk_start_tx) 
	clk_start_tx -= sub;
    if(clk_start_rx) 
	clk_start_rx -= sub;
    if(clk_start_bit) 
	clk_start_bit -= sub;
}

