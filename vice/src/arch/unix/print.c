/*
 * print.c - Printer interface.
 *
 * Written by
 *  André Fachat (a.fachat@physik.tu-chemnitz.de)
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
 * The printer emulation captures the bytes sent to device 4 on the
 * IEC bus and/or the bytes sent to an emulated userport interface
 */

#define        DEBUG

#include <stdio.h>

#include <errno.h>
#include <string.h>

#include "vice.h"
#include "types.h"
#include "resources.h"
#include "cmdline.h"
#include "print.h"
#include "utils.h"

#define        MAXPRINT        4

/*********************************************************************/
/* resource handling */

#define	NUM_DEVICES	3

static char *devfile[NUM_DEVICES];
static int devbaud[NUM_DEVICES];

static int set_devfile(char *v, int dev) {
    const char *name = (const char *) v;
/*printf("set_devfile(v=%s, dev=%d)\n",v,dev);*/
    if (devfile[dev] != NULL && name != NULL
        && strcmp(name, devfile[dev]) == 0)
        return 0;

    string_set(&devfile[dev], name);
    return 0;
}

static int set_devbaud(int v, int dev) {
    devbaud[dev] = v;
    return 0;
}

/********************************/

static int set_dev1_file(resource_value_t v) {
    return set_devfile((char*)v, 0);
}

static int set_dev2_file(resource_value_t v) {
    return set_devfile((char*)v, 1);
}

static int set_dev3_file(resource_value_t v) {
    return set_devfile((char*)v, 2);
}

static resource_t resources[] = {
    { "PrDevice1", RES_STRING, (resource_value_t) "print.dump",
      (resource_value_t *) &devfile[0], set_dev1_file },
    { "PrDevice2", RES_STRING, (resource_value_t) "|petlp |lpr",
      (resource_value_t *) &devfile[1], set_dev2_file },
    { "PrDevice3", RES_STRING, (resource_value_t) "|lpr",
      (resource_value_t *) &devfile[2], set_dev3_file },
    { NULL }
};

int print_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-prdev1", SET_RESOURCE, 1, NULL, NULL, "PrDevice1", NULL,
      "<name>", "Specify name of printer dump file (print.dump)" },
    { "-prdev2", SET_RESOURCE, 1, NULL, NULL, "PrDevice2", NULL,
      "<name>", "Specify command for printer 1 (|petlp |lpr)" },
    { "-prdev3", SET_RESOURCE, 1, NULL, NULL, "PrDevice3", NULL,
      "<name>", "Specify command for printer 2 (|lpr)" },
    { NULL }
};

int print_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/*********************************************************************/

typedef struct Printer {
       int     inuse;
       int     type;
       FILE    *fp;
       char    *file;
} Printer;

#define	T_FILE		0
#define	T_PROC		2

static Printer fds[MAXPRINT];

/* initializes all Printer stuff */
void print_init(void) {
       int i;

       for(i=0;i<MAXPRINT;i++) {
         fds[i].inuse = 0;
       }
}

/* reset RS232 stuff */
void print_reset(void) {
       int i;

       for(i=0;i<MAXPRINT;i++) {
         if(fds[i].inuse) {
	    print_close(i);
	 }
       }
}

/* opens a rs232 window, returns handle to give to functions below. */
int print_open(int device) {
       int i;

       for(i=0;i<MAXPRINT;i++) {
         if(!fds[i].inuse) break;
       }
       if(i>=MAXPRINT) {
	 fprintf(stderr,"PRINT: No device available!\n");
	 return -1;
       }
#ifdef DEBUG
       fprintf(stderr,"print_open(device=%d)\n",device);
#endif
       if(devfile[device][0] == '|') {
	 fds[i].fp = popen(devfile[device]+1, "w");
	 if(!fds[i].fp) {
           fprintf(stderr,"print(%s): %s\n",devfile[device],strerror(errno));
	   return -1;
	 }
         fds[i].type = T_PROC;
       } else {
	 fds[i].fp = fopen(devfile[device], "ab");
	 if(!fds[i].fp) {
           fprintf(stderr,"print(%s): %s\n",devfile[device],strerror(errno));
	   return -1;
	 }
	 fds[i].type = T_FILE;
       }
       fds[i].inuse = 1;
       fds[i].file = devfile[device];

       return i;
}

/* closes the Printer again */
void print_close(int fd) {
#ifdef DEBUG
       fprintf(stderr,"PRINT close(fd=%d)\n",fd);
#endif
       if(fd<0 || fd>=MAXPRINT) {
         printf("PRINT: close with invalid fd %d!\n", fd);
         return;
       }

       if(!fds[fd].inuse) {
         printf("PRINT: close with non-open fd %d!\n", fd);
         return;
       }

       if(fds[fd].type == T_PROC) {
	 pclose(fds[fd].fp);
       } else {
         fclose(fds[fd].fp);
       }
       fds[fd].inuse = 0;
}

/* sends a byte to the RS232 line */
int print_putc(int fd, BYTE b) {

       if(fd<0 || fd>=MAXPRINT || !fds[fd].inuse) {
         printf("PRINT: putc with invalid or non-open fd %d!\n", fd);
         return -1;
       }

       fputc(b, fds[fd].fp);

       return 0;
}


int print_flush(int fd) {
       if(fd<0 || fd>=MAXPRINT || !fds[fd].inuse) {
         printf("PRINT: flush with invalid or non-open fd %d!\n", fd);
         return -1;
       }

       fflush(fds[fd].fp);

       return 0;
}



