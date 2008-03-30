/*
 * serial.h - Serial device implementation.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
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

#ifndef _SERIAL_H
#define _SERIAL_H

#include "types.h"

/* Serial Error Codes. */

#define SERIAL_OK               0
#define SERIAL_WRITE_TIMEOUT    1
#define SERIAL_READ_TIMEOUT     2
#define SERIAL_FILE_NOT_FOUND   64
#define SERIAL_NO_DEVICE        128

#define SERIAL_ERROR            (2)
#define SERIAL_EOF              (64)

/* Printers. */
#define DT_ASCII                0       /* No printer commands nor graphics */
#define DT_MPS803               1
#define DT_STAR10CCL            2

struct disk_image_s;
struct trap_s;
struct vdrive_s;

typedef struct serial_s
{
    int inuse;
    int isopen[16]; /* isopen flag for each secondary address */
    struct disk_image_s *image; /* pointer to the disk image data  */
    char *name; /* name of the device */
    int (*getf)(struct vdrive_s *, BYTE *, unsigned int);
    int (*putf)(struct vdrive_s *, BYTE, unsigned int);
    int (*openf)(struct vdrive_s *, const char *, int, unsigned int);
    int (*closef)(struct vdrive_s *, unsigned int);
    void (*flushf)(struct vdrive_s *, unsigned int);
    BYTE nextbyte[16]; /* next byte to send, per sec. addr. */
    char nextok[16]; /* flag if nextbyte is valid */

    int nextst[16];

    /* The PET hardware emulation can be interrupted while
       transferring a byte. Thus we also have to save the byte
       and status last sent, to be able to send it again. */
    BYTE lastbyte[16];
    char lastok[16];
    int lastst[16];

} serial_t;

extern int serial_init(const struct trap_s *trap_list, ADDRESS tmpin);
extern int serial_install_traps(void);
extern int serial_remove_traps(void);
extern int serial_attach_device(unsigned int unit, const char *name,
                                int (*getf)(struct vdrive_s *,
                                BYTE *, unsigned int),
                                int (*putf)(struct vdrive_s *, BYTE,
                                unsigned int),
                                int (*openf)(struct vdrive_s *,
                                const char *, int,
                                unsigned int),
                                int (*closef)(struct vdrive_s *, unsigned int),
                                void (*flushf)(struct vdrive_s *,
                                unsigned int));
extern int serial_detach_device(unsigned int unit);
extern serial_t *serial_get_device(unsigned int unit);
extern void serial_reset(void);

extern int parallelattention(int b);
extern int parallelsendbyte(BYTE data);
extern int parallelreceivebyte(BYTE *data, int fake);
extern void serialattention(void);
extern void serialsendbyte(void);
extern void serialreceivebyte(void);
extern void trap_serial_ready(void);

extern void serial_set_eof_callback(void (*func)(void));
extern void serial_set_attention_callback(void (*func)(void));

#endif

