/*
 * serial.h - Serial device implementation.
 *
 * Written by
 *  Teemu Rantanen  (tvr@cs.hut.fi)
 *  André Fachat    (a.fachat@physik.tu-chemnitz.de)
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

#ifndef _SERIAL_H
#define _SERIAL_H

#include "types.h"
#include "traps.h"

#define MAXDEVICES		16

/* Serial Error Codes. */

#define SERIAL_OK		0
#define SERIAL_WRITE_TIMEOUT	1
#define SERIAL_READ_TIMEOUT	2
#define SERIAL_FILE_NOT_FOUND	64
#define SERIAL_NO_DEVICE	128

#define SERIAL_ERROR		(2)
#define SERIAL_EOF		(64)


/* Device types. */

#define DT_FS			0x0100	/* Filesystem Drive */
#define DT_DISK			0x0200
#define DT_PRINTER		0x0400

#define DT_TAPE			0x0800

#define DT_MASK			0xFF


/* Disk Drives. */

#define DT_1540			 0
#define DT_1541			 1	/* Default */
#define DT_1542			 2
#define DT_1551			 3

#define DT_1570			 4
#define DT_1571			 5
#define DT_1572			 6

#define DT_1581			 8

#define DT_2031			16
#define DT_2040			17
#define DT_2041			18
#define DT_3040			17	/* Same as 2040 */
#define DT_4031			16	/* Same as 2031 */

/* #define DT_2081			*/

#define DT_4040			24

#define DT_8050			32
#define DT_8060			33
#define DT_8061			34

#define DT_SFD1001		48
#define DT_8250			49
#define DT_8280			50


/* Printers. */

#define DT_ASCII		0	/* No printer commands nor graphics */
#define DT_MPS803		1
#define DT_STAR10CCL		2


typedef struct serial_s
{
    int inuse;
    int isopen[16];				/* isopen flag for each secondary address */
    char *info;					/* pointer to run-time data */
    char *name;					/* name of the device */
    int (*getf)(void *, BYTE *, int);		/* serial read function */
    int (*putf)(void *, BYTE, int);		/* serial write function */
    int (*openf)(void *, char *, int, int);	/* serial open function */
    int (*closef)(void *, int);			/* serial close function */
    void (*flushf)(void *, int);		/* tell device that write completed */
    BYTE nextbyte[16];				/* next byte to send to emulator, per sec. addr. */
    char nextok[16];				/* flag if nextbyte is valid */

    int nextst[16];

    /* The PET hardware emulation can be interrupted while
       transferring a byte. Thus we also have to save the byte
       and status last sent, to be able to send it again. */
    BYTE lastbyte[16];
    char lastok[16];
    int lastst[16];
} serial_t;

extern int serial_init(const trap_t *trap_list);
extern int serial_install_traps(void);
extern int serial_remove_traps(void);
extern int serial_attach_device(int device, char *var, char *name,
				int (*getf)(void *, BYTE *, int),
				int (*putf)(void *, BYTE, int),
				int (*openf)(void *, char *, int, int),
				int (*closef)(void *, int),
				void (*flushf)(void *, int));
extern int serial_select_file(int type, int number, const char *file);
extern int serial_remove(int number);
extern char *serial_get_file_name(int number);
extern serial_t *serial_get_device(int number);
extern void serial_reset(void);

extern int parallelattention(int b);
extern int parallelsendbyte(int data);
extern int parallelreceivebyte(BYTE *data, int fake);
extern void serialattention(void);
extern void serialsendbyte(void);
extern void serialreceivebyte(void);
extern void trap_serial_ready(void);

#endif  /* _SERIAL_H */
