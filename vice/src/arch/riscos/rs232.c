/*
 * rs232.c - RS232 emulation.
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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

#include <stdio.h>

#include "types.h"
#include "resources.h"
#include "util.h"


static char *SerialFile=NULL;
static int SerialBaud;
static FILE *fd[3] = {NULL, NULL, NULL};

static int set_serial_file(const char *val, void *param)
{
  util_string_set(&SerialFile, val);
  return 0;
}


static int set_serial_baud(int val, void *param)
{
  SerialBaud = val;
  /* Set receive and transmit rate */
  SerialOp5(val); SerialOp6(val);
  return 0;
}


static const resource_string_t resources_string[] = {
  {"SerialFile", "SerialFile", RES_EVENT_NO, NULL,
    &SerialFile, set_serial_file, NULL },
  {NULL}
};

static const resource_int_t resources_int[] = {
  {"SerialBaud", 7, RES_EVENT_NO, NULL,
    &SerialBaud, set_serial_baud, NULL },
  {NULL}
};


int rs232_resources_init(void)
{
  if (resources_register_string(resources_string) < 0)
    return -1;

  return resources_register_int(resources_int);
}


int rs232_resources_shutdown(void)
{
  return 0;
}


int rs232_cmdline_options_init(void)
{
  return 0;
}


void rs232_init(void)
{
}


void rs232_reset(void)
{
}


int rs232_open(int device)
{
  switch (device)
  {
    case 0:
      if (SerialFile == NULL) return -1;
      if (fd[0] == NULL) fd[0] = fopen(SerialFile, "ab+");
      return 0;
    case 1:
      if (fd[1] == NULL) fd[1] = fopen("parallel:", "ab+");
      return 1;
    case 2:
      if (fd[2] == NULL) fd[2] = fopen("serial:", "ab+");
      return 2;
    default:
      return -1;
  }
}


void rs232_close(int fi)
{
  if (fd[fi] != NULL) fclose(fd[fi]);
  fd[fi] = NULL;
}


int rs232_putc(int fi, BYTE b)
{
  if (fd[fi] == NULL) return -1;
  fputc(b, fd[fi]);
  return 0;
}


int rs232_getc(int fi, BYTE *b)
{
  if (fd[fi] == NULL) return -1;
  *b = fgetc(fd[fi]);
  return 0;
}

/* set the status lines of the RS232 device */
int rs232_set_status(int fd, enum rs232handshake_out status)
{
    return -1;
}

/* get the status lines of the RS232 device */
enum rs232handshake_in rs232_get_status(int fd)
{
    /*! \todo dummy */
    return RS232_HSI_CTS | RS232_HSI_DSR;
}

/* set the bps rate of the physical device */
void rs232_set_bps(int fd, unsigned int bps)
{
}
