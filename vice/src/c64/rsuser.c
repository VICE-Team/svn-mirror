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

#undef DEBUG

#define	RSUSER_TICS	21111

/***********************************************************************
 * resource handling
 */

int rsuser_enabled = 0;

static int rsuser_device;

static int set_up_enabled(resource_value_t v) {
    int newval = ((int) v) ? 1 : 0;

    if(newval && !rsuser_enabled) {
	dtr = DTR_OUT;	/* inactive */
	rts = RTS_OUT;	/* inactive */
	fd = -1;
    }
    if(rsuser_enabled && !newval) {
	if(fd>=0) rs232_close(fd);
	maincpu_unset_alarm(A_RSUSER);
	fd = -1;
    }

    rsuser_enabled = newval;
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
    { "-rsuser", SET_RESOURCE, 0, NULL, NULL, "RsUser",
        (resource_value_t) 1, NULL,
        "Enable the userport 9600 baud RS232 emulation" },
    { "+rsuser", SET_RESOURCE, 0, NULL, NULL, "RsUser",
        (resource_value_t) 0, NULL,
        "Disable the userport 9600 baud RS232 emulation" },
    { "-rsuserdev", SET_RESOURCE, 1, NULL, NULL, "RsUserDev",
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
	if(fd >= 0) rs232_close(fd);
	maincpu_unset_alarm(A_RSUSER);
	fd = -1;

	maincpu_unset_alarm(A_RSUSER);
}

void userport_serial_write_ctrl(int b) {
    int new_dtr = b & DTR_OUT;	/* = 0 is active, != 0 is inactive */
    int new_rts = b & RTS_OUT;	/* = 0 is active, != 0 is inactive */

#ifdef DEBUG
printf("userport_serial_write_ctrl(b=%02x)\n",b);
#endif
    if(rsuser_enabled) {
        if(dtr && !new_dtr) {
/*printf("switch rs232 on\n");*/
            /* switch rs232 on */
            fd = rs232_open(rsuser_device);
	    maincpu_set_alarm(A_RSUSER, RSUSER_TICS);
        }
        if(new_dtr && !dtr && fd>=0) {
/*printf("switch rs232 off\n");*/
	    maincpu_unset_alarm(A_RSUSER);
            rs232_close(fd);
	    fd = -1;
        }
    }

    dtr = new_dtr;
    rts = new_rts;
}

BYTE userport_serial_read_ctrl(void) {
    return CTS_IN | (rsuser_enabled ? 0 : (CTS_IN | DCD_IN));
}

void userport_serial_write_sr(BYTE b) {
    BYTE c;

    buf = (buf << 8) | b;
    valid += 8;

    while(valid >= 10 && (buf & masks[valid-1])) valid--;

    /* printf("rsuser_write_sr(%02x), buf=%x, valid=%d\n",b, buf, valid); */

    if(valid>=10) {	/* (valid-1)-th bit is not set = start bit! */
	if(!(buf & masks[valid-10])) {
	    fprintf(stderr, "frame error!\n");
	} else {
	    c = (buf >> (valid-9)) & 0xff;
	    /*printf("rsuser_send %c (%02x), buf=%x, valid=%d\n",
						code[c],code[c], buf, valid);*/
	    if(fd>=0) rs232_putc(fd, code[c]);
	}
	valid -= 10;
    }
}


int int_rsuser(long offset) {
#ifdef DEBUG
        printf("int_rsuser(clk=%d, rclk=%ld)\n",clk, clk-offset);
#endif

	switch(rxstate) {
	case 0:
        	if( fd>=0 && rs232_getc(fd, &rxdata)) {

		  /* byte received, signal startbit on flag */
                  rxstate ++;
		  cia2_set_flag();
		  maincpu_set_alarm(A_RSUSER, RSUSER_TICS);

		} else {
		  /* no byte received */
		  maincpu_set_alarm(A_RSUSER, RSUSER_TICS);
		}
		break;
	case 1:
		/* now byte should be in shift register */
		cia2_set_sdr(code[rxdata]);
		rxstate = 0;
		maincpu_set_alarm(A_RSUSER, RSUSER_TICS);
		break;
        }

        return 0;
}


