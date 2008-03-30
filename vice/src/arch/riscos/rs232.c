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
#include "utils.h"


static char *SerialFile=NULL;
static int SerialBaud;
static FILE *fd[3] = {NULL, NULL, NULL};

static int set_serial_file(resource_value_t v, void *param)
{
  util_string_set(&SerialFile, (const char*)v);
  return 0;
}


static int set_serial_baud(resource_value_t v, void *param)
{
  SerialBaud = (int)v;
  /* Set receive and transmit rate */
  SerialOp5((int)v); SerialOp6((int)v);
  return 0;
}


static const resource_t resources[] = {
  {"SerialFile", RES_STRING, (resource_value_t)"SerialFile",
    (void *)&SerialFile, set_serial_file, NULL },
  {"SerialBaud", RES_INTEGER, (resource_value_t)7,
    (void *)&SerialBaud, set_serial_baud, NULL },
  {NULL}
};


int rs232_resources_init(void)
{
  return resources_register(resources);
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
