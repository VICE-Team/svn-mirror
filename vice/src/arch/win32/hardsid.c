/*
 * hardsid.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  HardSID Support <support@hardsid.com>
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

#ifdef HAVE_HARDSID

#include <stdio.h>
#include <windows.h>

#include "hardsid.h"
#include "log.h"
#include "types.h"


typedef BYTE (CALLBACK* GetHardSIDCount_t)    (void);
typedef void (CALLBACK* InitHardSID_Mapper_t) (void);
typedef void (CALLBACK* MuteHardSID_Line_t)   (BOOL);
typedef BYTE (CALLBACK* ReadFromHardSID_t)    (BYTE, BYTE);
typedef void (CALLBACK* SetDebug_t)           (BOOL);
typedef void (CALLBACK* WriteToHardSID_t)     (BYTE, BYTE, BYTE);


static GetHardSIDCount_t       GetHardSIDCount;
static InitHardSID_Mapper_t    InitHardSID_Mapper;
static MuteHardSID_Line_t      MuteHardSID_Line;
static ReadFromHardSID_t       ReadFromHardSID;
static SetDebug_t              SetDebug;
static WriteToHardSID_t        WriteToHardSID;

static HINSTANCE dll = NULL;


int hardsid_open(void)
{
    if (dll == NULL) {
        dll = LoadLibrary("HARDSID.DLL");
        if (dll != NULL) {
            GetHardSIDCount = (GetHardSIDCount_t)
                              GetProcAddress(dll, "GetHardSIDCount");
            InitHardSID_Mapper = (InitHardSID_Mapper_t)
                                 GetProcAddress(dll, "InitHardSID_Mapper");
            MuteHardSID_Line = (MuteHardSID_Line_t)
                               GetProcAddress(dll, "MuteHardSID_Line");
            ReadFromHardSID = (ReadFromHardSID_t)
                              GetProcAddress(dll, "ReadFromHardSID");
            SetDebug = (SetDebug_t)
                       GetProcAddress(dll, "SetDebug");
            WriteToHardSID = (WriteToHardSID_t)
                             GetProcAddress(dll, "WriteToHardSID");
        } else {
            return -1;
        }
    }

    return 0;
}

int hardsid_close(void)
{
    if (dll != NULL) {
       int chipno;
       WORD addr;

       for (chipno = 0; chipno < 2; chipno++) {
           for (addr = 0; addr < 24; addr++)
               hardsid_read(addr, chipno);
       }
    }

    return 0;
}

int hardsid_read(WORD addr, int chipno)
{
    if (dll != NULL) {
        switch (chipno) {
          case 0:
            return ReadFromHardSID(0, (UCHAR)(addr & 0x1f));
          case 1:
            return ReadFromHardSID(1, (UCHAR)(addr & 0x1f));
        }
    }

    return 0;
}

void hardsid_store(WORD addr, BYTE val, int chipno)
{
    if (dll != NULL) {
        switch (chipno) {
          case 0:
            WriteToHardSID(0, (UCHAR)(addr & 0x1f), val);
            break;
          case 1:
            WriteToHardSID(1, (UCHAR)(addr & 0x1f), val);
            break;
        }
    }
}

void hardsid_set_machine_parameter(long cycles_per_sec)
{
}

#endif
