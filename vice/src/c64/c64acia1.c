
/*
 * ../../../src/c64/c64acia1.c
 * This file is generated from ../../../src/acia-tmpl.c and ../../../src/c64/c64acia1.def,
 * Do not edit!
 */

/*
 *
 * This file is part of Commodore 64 emulator.
 * See README for copyright notice
 *
 * ACIA 6551 rs232 emulation
 *
 * Written by
 *    Andre Fachat (a.fachat@physik.tu-chemnitz.de)
 *
 */

#include <stdio.h>

#include "resources.h"
#include "cmdline.h"
#include "vice.h"
#include "types.h"
#include "vmachine.h"
#include "interrupt.h"
#include "rs232.h"
#include "acia.h"



#undef	DEBUG

#define	ACIA_TICKS	21111

static int fd = -1;
static int intx = 0;	/* indicates that a transmit is currently ongoing */
static int irq = 0;
static BYTE cmd;
static BYTE ctrl;
static BYTE rxdata;	/* data that has been received last */
static BYTE txdata;	/* data prepared to send */
static BYTE status;

/******************************************************************/

/* rs232.h replacement functions if no rs232 device available */

#ifndef HAVE_RS232

static int rs232_open(int device) 
{
    return -1;
}

static void rs232_close(int fd) { }

static int rs232_putc(int fd, BYTE b) { }

static int rs232_getc(int fd, BYTE *b) { }

#endif

/******************************************************************/

static int acia1_device;
static int acia1_irq;

static int acia1_set_device(resource_value_t v) {

    if(fd>=0) {
	fprintf(stderr, "ACIA1: device open, change effective only after "
		"close!\n");
    }
    acia1_device = (int) v;
    return 0;
}

static int acia1_set_irq(resource_value_t v) {
    if((int)v) {
	acia1_irq = IK_NMI;
    } else {
	acia1_irq = IK_IRQ;
    }
    return 0;
}

static resource_t resources[] = {
    { "Acia1Dev", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) & acia1_device, acia1_set_device },
    { "Acia1Irq", RES_INTEGER, (resource_value_t) IK_IRQ,
      (resource_value_t *) & acia1_irq, acia1_set_irq },
    { NULL }
};

int acia1_init_resources(void) {
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-acia1dev", SET_RESOURCE, 1, NULL, NULL, "Acia1Dev", NULL,
	"<0-3>", "Specify RS232 device this ACIA should work on" },
    { NULL }
};

int acia1_init_cmdline_options(void) {
    return cmdline_register_options(cmdline_options);
}

/******************************************************************/

void reset_acia1(void) {

#ifdef DEBUG
	printf("reset_acia1\n");
#endif

	cmd = 0;
	ctrl = 0;
	status = 0x10;
	intx = 0;

	if(fd>=0) rs232_close(fd);
	fd = -1;

	maincpu_unset_alarm(A_ACIA1);
	maincpu_set_int(I_ACIA1, 0);
	irq = 0;
}


void REGPARM2 store_acia1(ADDRESS a, BYTE b) {

#ifdef DEBUG
	printf("store_acia1(%04x,%02x\n",a,b);
#endif

	switch(a & 3) {
	case ACIA_DR:
		txdata = b;
		if(cmd&1) {
		  if(!intx) {
		    maincpu_set_alarm(A_ACIA1, 1);
		    intx = 2;
		  } else
		  if(intx==1) {
		    intx++;
		  }
		  status &= 0xef;		/* clr TDRE */
		}
		break;
	case ACIA_SR:
		if(fd>=0) rs232_close(fd);
		fd = -1;
		status &= ~4;
		cmd &= 0xe0;
		intx = 0;
		maincpu_set_int(I_ACIA1, 0);
		irq = 0;
		maincpu_unset_alarm(A_ACIA1);
		break;
	case ACIA_CTRL:
		ctrl = b;
		/* TODO: use baudrate for int_acia rate */
		break;
	case ACIA_CMD:
		cmd = b;
		if((cmd & 1) && (fd<0)) {
		  fd = rs232_open(acia1_device);
		  maincpu_set_alarm(A_ACIA1, ACIA_TICKS);
		} else
		if(fd>=0 && !(cmd&1)) {
		  rs232_close(fd);
		  maincpu_unset_alarm(A_ACIA1);
		  fd = -1;
		}
		break;
	}
}

BYTE REGPARM1 read_acia1(ADDRESS a) {
#if 0 /* def DEBUG */
	BYTE read_acia1_(ADDRESS);
	BYTE b = read_acia1_(a);
	static ADDRESS lasta = 0;
	static BYTE lastb = 0;

	if((a!=lasta) || (b!=lastb)) {
	  printf("read_acia1(%04x) -> %02x\n",a,b);
	}
	lasta = a; lastb = b;
	return b;
}
BYTE read_acia1_(ADDRESS a) {
#endif

	switch(a & 3) {
	case ACIA_DR:
		status &= ~8;
		return rxdata;
	case ACIA_SR:
		{
		  BYTE c = status | (irq?0x80:0);
		  maincpu_set_int(I_ACIA1, 0);
		  irq = 0;
		  return c;
		}
	case ACIA_CTRL:
		return ctrl;
	case ACIA_CMD:
		return cmd;
	}
	return 0;
}

BYTE peek_acia1(ADDRESS a) {

	switch(a & 3) {
	case ACIA_DR:
		return rxdata;
	case ACIA_SR:
		{
		  BYTE c = status | (irq?0x80:0);
		  return c;
		}
	case ACIA_CTRL:
		return ctrl;
	case ACIA_CMD:
		return cmd;
	}
	return 0;
}

int int_acia1(long offset) {
#if 0 /*def DEBUG*/
	printf("int_acia1(clk=%ld)\n",maincpu_clk-offset);
#endif

	if(intx==2 && fd>=0) rs232_putc(fd,txdata);
	if(intx) intx--;

	if(!(status&0x10)) {
	  status |= 0x10;
	}

        if( fd>=0 && (!(status&8)) && rs232_getc(fd, &rxdata)) {
          status |= 8;
        }

	maincpu_set_int(I_ACIA1, acia1_irq);
	irq = 1;

	maincpu_set_alarm(A_ACIA1, ACIA_TICKS);

	return 0;
}


