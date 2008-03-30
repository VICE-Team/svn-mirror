/*
 * catweaselmkiii.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Dirk Jagdmann <doj@cubic.org>
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

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#ifndef __GNUC__
#include <winioctl.h>
#endif

#include "catweaselmkiii.h"
#include "log.h"
#include "types.h"


typedef void (*voidfunc_t)(void);

/* defined for CatWeasel MK3 PCI device driver */
#define SID_SID_PEEK_POKE   CTL_CODE(FILE_DEVICE_SOUND,0x0800UL + 1,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define SID_SET_CLOCK       CTL_CODE(FILE_DEVICE_SOUND,0x0800UL + 4,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define SID_CMD_READ   32

#define MAXCARDS 4
static HANDLE sidhandle[MAXCARDS] = {
  INVALID_HANDLE_VALUE,
  INVALID_HANDLE_VALUE,
  INVALID_HANDLE_VALUE,
  INVALID_HANDLE_VALUE,
};


BYTE sidbuf[MAXCARDS*0x20];

static int ntsc=0;
static int atexitinitialized=0;

static void setfreq(void)
{
  int i;
  BYTE buf=ntsc;
  for(i=0; i<MAXCARDS; i++)
    if(sidhandle[i]!=INVALID_HANDLE_VALUE)
      {
	DWORD w;
	DeviceIoControl(sidhandle[i],SID_SET_CLOCK,&buf,sizeof(buf),0L,0UL,&w,0L);
      }
}

static void mutethem(void)
{
  DWORD w;
  int i;
  BYTE buf[0x19*2];
  for(i=0; i<=0x18; i++)
    buf[i*2]=i, buf[i*2+1]=0;
  for(i=0; i<MAXCARDS; i++)
    if(sidhandle[i]!=INVALID_HANDLE_VALUE)
      DeviceIoControl(sidhandle[i],SID_SID_PEEK_POKE,buf,sizeof(buf),0L,0UL,&w,0L);
  memset(sidbuf, 0, sizeof(sidbuf));
}

static int initthem(void)
{
  int i, z=0;
  char buf[32];
  for(i=0; i<MAXCARDS; i++)
    if(sidhandle[i]!=INVALID_HANDLE_VALUE)
      CloseHandle(sidhandle[i]), sidhandle[i]=INVALID_HANDLE_VALUE;

  for(i=0; i<MAXCARDS; i++)
    {
      sprintf(buf, "\\\\.\\SID6581_%u", i+1);
      sidhandle[z]=CreateFile(buf,GENERIC_READ,FILE_SHARE_WRITE|FILE_SHARE_READ,0L,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL/*|FILE_FLAG_OVERLAPPED*/,0L);
      if(sidhandle[z]!=INVALID_HANDLE_VALUE)
	{
	  log_message(LOG_DEFAULT, "Found CatWeasel MK3 PCI #%i", z+1);
	  z++;
	}
    }

  mutethem();
  setfreq();
  return (z>0)?0:-1;
}

int catweaselmkiii_init(void)
{
  int ret=initthem();
  if(ret==0)
    log_message(LOG_DEFAULT, "Found and initialized a CatWeasel MK3 PCI SID");
  return ret;
}

int catweaselmkiii_open(void)
{
  int ret=initthem();
  if(ret==0)
    log_message(LOG_DEFAULT, "Found and opened a CatWeasel MK3 PCI SID");
  if(!atexitinitialized)
    {
      atexitinitialized=1;
      atexit((voidfunc_t)catweaselmkiii_close);
    }
  return ret;
}

int catweaselmkiii_close(void)
{
  int i;

  mutethem();
  for(i=0; i<MAXCARDS; i++)
    if(sidhandle[i]!=INVALID_HANDLE_VALUE)
      CloseHandle(sidhandle[i]), sidhandle[i]=INVALID_HANDLE_VALUE;

  log_message(LOG_DEFAULT, "Closed CatWeasel MK3 PCI SID");
  return 0;
}

int catweaselmkiii_read(ADDRESS addr, int chipno)
{
  if(chipno<MAXCARDS)
    {
      if(addr>=0x19 && addr<=0x1C && sidhandle[chipno]!=INVALID_HANDLE_VALUE)
	{
	  DWORD w;
	  BYTE buf[2] = { SID_CMD_READ, addr&0xff };
	  DeviceIoControl(sidhandle[chipno],SID_SID_PEEK_POKE,buf,2,buf,1,&w,0L);
	  return buf[0];
	}
      if(addr<=0x18)
	return sidbuf[chipno*0x20+addr];
    }
  log_error(LOG_ERR, "CatWeasel MK3 PCI does not support SID #%i", chipno);
  return 0;
}

void catweaselmkiii_store(ADDRESS addr, BYTE val, int chipno)
{
  if(chipno<MAXCARDS)
    {
      if(addr<=0x18)
	{
	  sidbuf[chipno*0x20+addr]=val;
	  if(sidhandle[chipno]!=INVALID_HANDLE_VALUE)
	    {
	      DWORD w;
	      BYTE buf[2] = { addr&0xff, val };
	      DeviceIoControl(sidhandle[chipno],SID_SID_PEEK_POKE,buf,sizeof(buf),0L,0UL,&w,0L);
	    }
	  return;
	}
      log_error(LOG_ERR, "CatWeasel MK3 PCI store at address %04x", addr);
      return;
    }
  log_error(LOG_ERR, "CatWeasel MK3 PCI does not support SID #%i", chipno);
}

void catweaselmkiii_set_machine_parameter(long cycles_per_sec)
{
  ntsc=(cycles_per_sec <= 1000000)?0:1;
  setfreq();
}

int catweaselmkiii_available(void)
{
  return initthem();
}
