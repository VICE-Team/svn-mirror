/*
 * rs232drv.c - Common RS232 driver handling.
 *
 * Written by
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

#include "vice.h"

#include "rs232.h"
#include "types.h"


#ifdef HAVE_RS232

void rs232drv_init(void)
{
    rs232_init();
}

void rs232drv_reset(void)
{
    rs232_reset();
}

int rs232drv_open(int device)
{
    return rs232_open(device);
}

void rs232drv_close(int fd)
{
    rs232_close(fd);
}

int rs232drv_putc(int fd, BYTE b)
{
    return rs232_putc(fd, b);
}

int rs232drv_getc(int fd, BYTE *b)
{
    return rs232_getc(fd, b);
}

int rs232drv_resources_init(void)
{
    return rs232_resources_init();
}

int rs232drv_cmdline_options_init(void)
{
    return rs232_cmdline_options_init();
}

#else

void rs232drv_init(void)
{

}

void rs232drv_reset(void)
{

}

int rs232drv_open(int device)
{
    return -1;
}

void rs232drv_close(int fd)
{
}

int rs232drv_putc(int fd, BYTE b)
{
    return -1;
}

int rs232drv_getc(int fd, BYTE *b)
{
    return -1;
}

int rs232drv_resources_init(void)
{
    return 0;
}

int rs232drv_cmdline_options_init(void)
{
    return 0;
}

#endif

