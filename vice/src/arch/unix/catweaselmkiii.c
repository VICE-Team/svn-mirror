/*
 * catweaselmkiii.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Dirk Jadgmann <doj@cubic.org>
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

#ifdef HAVE_CATWEASELMKIII

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "catweaselmkiii.h"
#include "log.h"
#include "types.h"

#include <cwsid.h>

typedef void (*voidfunc_t)(void);

#define MAXSID 2

static BYTE sidbuf[0x20*MAXSID];
static int sidfh=-1;
static int ntsc=0;
static int atexitinitialized=0;

static void setfreq()
{
  if(sidfh>=0)
    ioctl(sidfh, ntsc?CWSID_IOCTL_NTSC:CWSID_IOCTL_PAL);
}

static int opendevice()
{
  if(sidfh<0)
    {
      sidfh = open("/dev/sid", O_WRONLY);

      if (sidfh < 0)
        sidfh = open("/dev/misc/sid", O_WRONLY);

      if (sidfh < 0) {
        log_error(LOG_DEFAULT,
                  "could not open sid device /dev/sid or /dev/misc/sid");
        return -1;
      }
    }

  return 0;
}

int catweaselmkiii_init(void)
{
  int r=opendevice();
  catweaselmkiii_close();
  if(r < 0)
    log_message(LOG_DEFAULT, "CatWeasel MK3 PCI SID: not found");
  else
    log_message(LOG_DEFAULT, "CatWeasel MK3 PCI SID: found");
  return r;
}

int catweaselmkiii_open(void)
{
  if(!atexitinitialized)
    {
      atexitinitialized=1;
      atexit((voidfunc_t)catweaselmkiii_close);
    }

  if(opendevice() < 0)
    {
      log_message(LOG_DEFAULT, "CatWeasel MK3 PCI SID: could not open");
      return -1;
    }

  memset(sidbuf, 0, sizeof(sidbuf));
  lseek(sidfh, 0, SEEK_SET);
  write(sidfh, sidbuf, sizeof(sidbuf));
  setfreq();

  log_message(LOG_DEFAULT, "CatWeasel MK3 PCI SID: opened");

  return 0;
}

int catweaselmkiii_close(void)
{
  if(sidfh>=0)
    {
      memset(sidbuf, 0, sizeof(sidbuf));
      lseek(sidfh, 0, SEEK_SET);
      write(sidfh, sidbuf, sizeof(sidbuf));

      close(sidfh);
      sidfh=-1;

      /*log_message(LOG_DEFAULT, "CatWeasel MK3 PCI SID: closed");*/
    }
  return 0;
}

int catweaselmkiii_read(ADDRESS addr, int chipno)
{
  if(chipno<MAXSID && addr<0x20)
    {
      if(addr>=0x19 && addr<=0x1C && sidfh>=0)
        {
	  addr+=chipno*0x20;
          lseek(sidfh, addr, SEEK_SET);
          read(sidfh, &sidbuf[addr], 1);
        }
      else
	addr+=chipno*0x20;
      return sidbuf[addr];
    }
  return 0;
}

void catweaselmkiii_store(ADDRESS addr, BYTE val, int chipno)
{
  if(chipno<MAXSID && addr<=0x18)
    {
      addr+=chipno*0x20;
      sidbuf[addr]=val;
      if(sidfh>=0)
        {
          lseek(sidfh, addr, SEEK_SET);
          write(sidfh, &val, 1);
        }
    }
}

void catweaselmkiii_set_machine_parameter(long cycles_per_sec)
{
  ntsc=(cycles_per_sec <= 1000000)?0:1;
  setfreq();
}

int catweaselmkiii_available(void)
{
  int r=opendevice();
  catweaselmkiii_close();
  return r;
}

#endif

