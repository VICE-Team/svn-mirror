/*
 * rs232.c - RS232 emulation.
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
 * The RS232 emulation captures the bytes sent to the RS232 interfaces
 * available (currently only ACIA 6551, later UART 16550A, std C64,
 * and Daniel Dallmanns fast RS232 with 9600 Baud).
 * The characters captured are displayed in a special terminal window.
 * Characters typed in the terminal window are sent back to the
 * chip emulations.
 *
 */

#define        DEBUG

#include <stdio.h>

#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#include "vice.h"
#include "types.h"
#include "resources.h"
#include "cmdline.h"
#include "rs232.h"
#include "utils.h"

#define        MAXRS232        4

/*********************************************************************/
/* resource handling */

#define	NUM_DEVICES	3

static char *devfile[NUM_DEVICES];
static int devbaud[NUM_DEVICES];

static int set_devfile(char *v, int dev) {
    const char *name = (const char *) v;

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

static int set_dev1_baud(resource_value_t v) {
    return set_devbaud((int)v, 0);
}

static int set_dev2_baud(resource_value_t v) {
    return set_devbaud((int)v, 1);
}

static int set_dev3_baud(resource_value_t v) {
    return set_devbaud((int)v, 2);
}


static resource_t resources[] = {
    { "RsDevice1", RES_STRING, (resource_value_t) "/dev/ttyS0",
      (resource_value_t *) &devfile[0], set_dev1_file },
    { "RsDevice1Baud", RES_INTEGER, (resource_value_t) 9600,
      (resource_value_t *) &devbaud[0], set_dev1_baud },
    { "RsDevice2", RES_STRING, (resource_value_t) "/dev/ttyS1",
      (resource_value_t *) &devfile[1], set_dev2_file },
    { "RsDevice2Baud", RES_INTEGER, (resource_value_t) 9600,
      (resource_value_t *) &devbaud[1], set_dev2_baud },
    { "RsDevice3", RES_STRING, (resource_value_t) "rs232.dump",
      (resource_value_t *) &devfile[2], set_dev3_file },
    { "RsDevice3Baud", RES_INTEGER, (resource_value_t) 9600,
      (resource_value_t *) &devbaud[2], set_dev3_baud },
    { NULL }
};

int rs232_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-rsdev1", SET_RESOURCE, 1, NULL, NULL, "RsDevice1", NULL,
      "<name>", "Specify name of first RS232 device (/dev/ttyS0)" },
    { "-rsdev1baud", SET_RESOURCE, 1, NULL, NULL, "RsDevice1Baud", NULL,
      "<baudrate>", "Specify baudrate of first RS232 device" },

    { "-rsdev2", SET_RESOURCE, 1, NULL, NULL, "RsDevice2", NULL,
      "<name>", "Specify name of second RS232 device (/dev/ttyS1)" },
    { "-rsdev2baud", SET_RESOURCE, 1, NULL, NULL, "RsDevice2Baud", NULL,
      "<baudrate>", "Specify baudrate of second RS232 device" },

    { "-rsdev3", SET_RESOURCE, 1, NULL, NULL, "RsDevice3", NULL,
      "<name>", "Specify name of third RS232 device (rs232.dump)" },
    { "-rsdev3baud", SET_RESOURCE, 1, NULL, NULL, "RsDevice3Baud", NULL,
      "<baudrate>", "Specify baudrate of third RS232 device" },

    { NULL }
};

int rs232_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/*********************************************************************/

typedef struct RS232 {
       int     inuse;
       int     type;
       int     fd;
       char    *file;
       struct termios saved;
} RS232;

#define	T_FILE		0
#define	T_TTY		1

static RS232 fds[MAXRS232];

/* initializes all RS232 stuff */
void rs232_init(void) {
       int i;

       for(i=0;i<MAXRS232;i++) {
         fds[i].inuse = 0;
       }
}

/* resets terminal to old mode */
static void unset_tty(int i) {
	tcsetattr(fds[i].fd, TCSAFLUSH, &fds[i].saved);
}

static struct { int baud; speed_t speed; } speed_tab[]= {
	{ 300, B300 }, { 600, B600 }, { 1200, B1200 }, { 1800, B1800 },
	{ 2400, B2400 }, { 4800, B4800 }, { 9600, B9600 },
	{ 19200, B19200 }, { 38400, B38400 },
	{ 0, B9600 } /* fallback */
};

/* sets terminal to raw mode */
static void set_tty(int i, int baud) {
       /*
	* set tty to raw mode as of
	* "Advanced Programming in the Unix Environment"
	* by W.R. Stevens, Addison-Wesley.
	*/
	speed_t speed;
	int fd = fds[i].fd;
	struct termios buf;

	if(tcgetattr(fd, &fds[i].saved) < 0) {
	  return /* -1 */;
	}
	buf = fds[i].saved;

	buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
		/* echho off, cononical mode off, extended input processing
		 * off, signal chars off */
	buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
		/* no SIGINT on Break, CR-to-NL off, input parity check off,
		 * don't strip 8th bit on input, output flow control off */
	buf.c_cflag &= ~(CSIZE | PARENB);
		/* clear size bits, parity checking off */
	buf.c_cflag |= CS8;
		/* set 8 bits/char */
	buf.c_oflag &= ~(OPOST);
		/* ouput processing off */
	buf.c_cc[VMIN] = 1;	/* 1 byte at a time, no timer */
	buf.c_cc[VTIME] = 0;

	for(i=0;speed_tab[i].baud;i++) {
	  if(speed_tab[i].baud >= baud) break;
	}
	speed = speed_tab[i].speed;

	cfsetispeed(&buf, speed);
	cfsetospeed(&buf, speed);

	tcsetattr(fd, TCSAFLUSH, &buf);
}

/* reset RS232 stuff */
void rs232_reset(void) {
       int i;

       for(i=0;i<MAXRS232;i++) {
         if(fds[i].inuse) {
	   if(fds[i].type == T_TTY) {
	     unset_tty(i);
	   }
#ifdef DEBUG
	   fprintf(stderr,"RS232 close(fd=%d)\n",i);
#endif
	   close(fds[i].fd);
	 }
         fds[i].inuse = 0;
       }
}

/* opens a rs232 window, returns handle to give to functions below. */
int rs232_open(int device) {
       int i, fd;

       for(i=0;i<MAXRS232;i++) {
         if(!fds[i].inuse) break;
       }
       if(i>=MAXRS232) {
	 fprintf(stderr,"RS232: No device available!\n");
	 return -1;
       }
#ifdef DEBUG
       fprintf(stderr,"rs232_open(device=%d)\n",device);
#endif
       fd = open(devfile[device], O_RDWR | O_NOCTTY | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
       if(fd<0) {
         fprintf(stderr,"open(%s): %s\n",devfile[device],strerror(errno));
         return -1;
       }
       fds[i].fd = fd;
       fds[i].file = devfile[device];

       if(isatty(fd)) {
	 fds[i].type = T_TTY;
	 set_tty(i, devbaud[device]);
       } else {
	 fds[i].type = T_FILE;
       }
       fds[i].inuse = 1;

       return i;
}

/* closes the rs232 window again */
void rs232_close(int fd) {

       if(fd<0 || fd>=MAXRS232) {
         printf("RS232: close with invalid fd %d!\n", fd);
         return;
       }

       if(!fds[fd].inuse) {
         printf("RS232: close with non-open fd %d!\n", fd);
         return;
       }
       close(fds[fd].fd);

       fds[fd].inuse = 0;
}

/* sends a byte to the RS232 line */
int rs232_putc(int fd, BYTE b) {
       size_t n;

       if(fd<0 || fd>=MAXRS232 || !fds[fd].inuse) {
         printf("RS232: putc with invalid or non-open fd %d!\n", fd);
         return -1;
       }

       /* for the beginning... */
#ifdef DEBUG
       printf("%c",b);
       fflush(stdout);
#endif
       do {
         n = write(fds[fd].fd, &b, 1);
         if(n<0) {
           fprintf(stderr,"write(1): %s\n",strerror(errno));
         }
       } while(n!=1);

       return 0;
}

/* gets a byte to the RS232 line, returns !=0 if byte received, byte in *b. */
int rs232_getc(int fd, BYTE *b) {
       int ret;
       size_t n;
       fd_set rdset;
       struct timeval ti;

       if(fd<0 || fd>=MAXRS232 || !fds[fd].inuse) {
         printf("RS232: getc with invalid or non-open fd %d!\n", fd);
         return 0;
       }
       if(fds[fd].type == T_FILE) return 0;

       FD_ZERO(&rdset);
       FD_SET(fds[fd].fd, &rdset);
       ti.tv_sec = ti.tv_usec = 0;
       ret = select(fds[fd].fd+1, &rdset, NULL, NULL, &ti);

       if(ret && (FD_ISSET(fds[fd].fd,&rdset)) ) {
         n = read(fds[fd].fd,b,1);
         if(n) return 1;
       }
       return 0;
}



