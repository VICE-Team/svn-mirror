/*
 * cw-pci.c - Windows specific PCI cw3 driver.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

/* Tested and confirmed working on:

 - Windows 95C (Direct PCI I/O)
 - Windows 98SE (Direct PCI I/O)
 - Windows ME (Direct PCI I/O)
 */

#include "vice.h"

#ifdef HAVE_CATWEASELMKIII
#include <windows.h>
#include <fcntl.h>
#include <stdio.h>
#include <conio.h>
#include <assert.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "alarm.h"
#include "archdep.h"
#include "catweaselmkiii.h"
#include "cw.h"
#include "log.h"
#include "sid-resources.h"
#include "types.h"

#define CW_SID_DAT 0xd8
#define CW_SID_CMD 0xdc

#define MAXSID 1

static int sids_found = -1;
static int base = -1;

static int cw_use_lib = 0;

#ifndef MSVC_RC
typedef int _stdcall (*initfuncPtr)(void);
typedef void _stdcall (*shutdownfuncPtr)(void);
typedef int _stdcall (*inpfuncPtr)(WORD port, PDWORD value, BYTE size);
typedef int _stdcall (*oupfuncPtr)(WORD port, DWORD value, BYTE size);

static initfuncPtr init32fp;
static shutdownfuncPtr shutdown32fp;
static inpfuncPtr inp32fp;
static oupfuncPtr oup32fp;
#else
typedef bool (CALLBACK* Init32_t)(void);
typedef void (CALLBACK* Shutdown32_t)(void);
typedef bool (CALLBACK* Inp32_t)(WORD, PDWORD, BYTE);
typedef bool (CALLBACK* Out32_t)(WORD, DWORD, BYTE);

static Init32_t Init32;
static Shutdown32_t Shutdown32;
static Inp32_t Inp32;
static Out32_t Out32;
#endif

/* input/output functions */
static void cw_outb(unsigned int addrint, DWORD value)
{
    WORD addr = (WORD)addrint;

    /* make sure the above conversion did not loose any details */
    assert(addr == addrint);

    if (cw_use_lib) {
#ifndef MSVC_RC
        (oup32fp)(addr, value, 1);
#else
        Out32(addr, value, 1);
#endif
    } else {
#ifdef  _M_IX86
#ifdef WATCOM_COMPILE
        outp(addr, (BYTE)value);
#else
        _outp(addr, (BYTE)value);
#endif
#endif
    }
}

static void cw_outl(unsigned int addrint, DWORD value)
{
    WORD addr = (WORD)addrint;

    /* make sure the above conversion did not loose any details */
    assert(addr == addrint);

    if (cw_use_lib) {
#ifndef MSVC_RC
        (oup32fp)(addr, value, 4);
#else
        Out32(addr, value, 4);
#endif
    } else {
#ifdef  _M_IX86
#ifdef WATCOM_COMPILE
        outpd(addr, value);
#else
        _outpd(addr, value);
#endif
#endif
    }
}

static BYTE cw_inb(unsigned int addrint)
{
    WORD addr = (WORD)addrint;
    DWORD retval = 0;

    /* make sure the above conversion did not loose any details */
    assert(addr == addrint);

    if (cw_use_lib) {
#ifndef MSVC_RC
        retval = (inp32fp)(addr, &retval, 1);
#else
        retval = Inp32(addr, &retval, 1);
#endif
        return (BYTE)retval;
    } else {
#ifdef  _M_IX86
#ifdef WATCOM_COMPILE
        return inp(addr);
#else
        return _inp(addr);
#endif
#endif
    }
}

static DWORD cw_inl(unsigned int addrint)
{
    WORD addr = (WORD)addrint;
    DWORD retval = 0;

    /* make sure the above conversion did not loose any details */
    assert(addr == addrint);

    if (cw_use_lib) {
#ifndef MSVC_RC
        retval = (inp32fp)(addr, &retval, 4);
#else
        retval = Inp32(addr, &retval, 4);
#endif
        return retval;
    } else {
#ifdef  _M_IX86
#ifdef WATCOM_COMPILE
        return inpd(addr);
#else
        return _inpd(addr);
#endif
#endif
    }
}

int cw_pci_read(WORD addr, int chipno)
{
    unsigned char cmd;

    if (chipno < MAXSID && base != -1 && addr < 0x20) {
        cmd = (addr & 0x1f) | 0x20;
        if (catweaselmkiii_get_ntsc()) {
            cmd |= 0x40;
        }
        cw_outb(base + CW_SID_CMD, cmd);
        usleep(1);
        return cw_inb(base + CW_SID_DAT);
    }
    return 0;
}

void cw_pci_store(WORD addr, BYTE outval, int chipno)
{
    unsigned char cmd;

    if (chipno < MAXSID && base != -1 && addr < 0x20) {
        cmd = addr & 0x1f;
        if (catweaselmkiii_get_ntsc()) {
            cmd |= 0x40;
        }
        cw_outb(base + CW_SID_DAT, outval);
        cw_outb(base + CW_SID_CMD, cmd);
        usleep(1);
    }
}

/*----------------------------------------------------------------------*/

static HINSTANCE hLib = NULL;

#ifdef _MSC_VER
#  ifdef _WIN64
#    define INPOUTDLLNAME "winio64.dll"
#  else
#    define INPOUTDLLNAME "winio32.dll"
#    define INPOUTDLLOLDNAME "winio.dll"
#  endif
#else
#  if defined(__amd64__) || defined(__x86_64__)
#    define INPOUTDLLNAME "winio64.dll"
#  else
#    define INPOUTDLLNAME "winio32.dll"
#    define INPOUTDLLOLDNAME "winio.dll"
#  endif
#endif

static int detect_sid(void)
{
    int i;

    for (i = 0x18; i >= 0; --i) {
        cw_pci_store((WORD)i, 0, 0);
    }

    cw_pci_store(0x12, 0xff, 0);

    for (i = 0; i < 100; ++i) {
        if (cw_pci_read(0x1b, 0)) {
            return 0;
        }
    }

    cw_pci_store(0x0e, 0xff, 0);
    cw_pci_store(0x0f, 0xff, 0);
    cw_pci_store(0x12, 0x20, 0);

    for (i = 0; i < 100; ++i) {
        if (cw_pci_read(0x1b, 0)) {
            return 1;
        }
    }
    return 0;
}

#ifndef KEY_WOW64_64KEY
#define KEY_WOW64_64KEY 0x0100
#endif

#ifndef KEY_WOW64_32KEY
#define KEY_WOW64_32KEY 0x0200
#endif

/* RegOpenKeyEx wrapper for smart access to both 32bit and 64bit registry entries */
static LONG RegOpenKeyEx3264(HKEY hKey, LPCTSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
    LONG retval = 0;

    /* Check 64bit first */
    retval = RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired | KEY_WOW64_64KEY, phkResult);

    if (retval == ERROR_SUCCESS) {
        return retval;
    }

    /* Check 32bit second */
    retval = RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired | KEY_WOW64_32KEY, phkResult);

    if (retval == ERROR_SUCCESS) {
        return retval;
    }

    /* Fallback to normal open */
    retval = RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);

    return retval;
}

static int has_pci(void)
{
    HKEY hKey;
    LONG ret;

    ret = RegOpenKeyEx3264(HKEY_LOCAL_MACHINE, "Enum\\PCI", 0, KEY_QUERY_VALUE, &hKey);
    if (ret == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return 1;
    }

    ret = RegOpenKeyEx3264(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Enum\\PCI", 0, KEY_QUERY_VALUE, &hKey);
    if (ret == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return 1;
    }

    return 0;
}

static int find_pci_device(int vendorID, int deviceID)
{
    int bus_index;
    int slot_index;
    int func_index;
    unsigned int address;
    unsigned int device;

    for (bus_index = 0; bus_index < 256; ++bus_index) {
        for (slot_index = 0; slot_index < 32; ++slot_index) {
            for (func_index = 0; func_index < 8; ++func_index) {
                address = 0x80000000 | (bus_index << 16) | (slot_index << 11) | (func_index << 8);
                cw_outl(0xCF8, address);
                device = cw_inl(0xCFC);
                if (device == (vendorID | (deviceID << 16))) {
                    address |= 0x10;
                    cw_outl(0xCF8, address);
                    base = cw_inl(0xCFC) & 0xFFFC;
                    return 0;
                }
            }
        }
    }
    return -1;
}

int cw_pci_open(void)
{
    int i;
    int res;
    char *openedlib = NULL;

    if (!sids_found) {
        return -1;
    }

    if (sids_found > 0) {
        return 0;
    }

    sids_found = 0;

    log_message(LOG_DEFAULT, "Detecting PCI CatWeasel boards.");

    if (!has_pci()) {
        log_message(LOG_DEFAULT, "No PCI bus present.");
        return -1;
    }

    /* Only use dll when on win nt and up */
    if (!(GetVersion() & 0x80000000) && cw_use_lib == 0) {

#ifdef INPOUTDLLOLDNAME
        if (hLib == NULL) {
            openedlib = INPOUTDLLOLDNAME;
            hLib = LoadLibrary(INPOUTDLLOLDNAME);
        }
#endif

        if (hLib == NULL) {
            hLib = LoadLibrary(INPOUTDLLNAME);
            openedlib = INPOUTDLLNAME;
        }
    }

    cw_use_lib = 0;

    if (hLib != NULL) {
        log_message(LOG_DEFAULT, "Opened %s.", openedlib);

#ifndef MSVC_RC
        inp32fp = (inpfuncPtr)GetProcAddress(hLib, "GetPortVal");
        if (inp32fp != NULL) {
            oup32fp = (oupfuncPtr)GetProcAddress(hLib, "SetPortVal");
            if (oup32fp != NULL) {
                init32fp = (initfuncPtr)GetProcAddress(hLib, "InitializeWinIo");
                if (init32fp != NULL) {
                    shutdown32fp = (shutdownfuncPtr)GetProcAddress(hLib, "ShutdownWinIo");
                    if (shutdown32fp != NULL) {
                        if (init32fp()) {
                            cw_use_lib = 1;
                            log_message(LOG_DEFAULT, "Using %s for PCI I/O access.", openedlib);
                        }
                    }
                }
            }
        }
#else
        Inp32 = (Inp32_t)GetProcAddress(hLib, "GetPortVal");
        if (Inp32 != NULL) {
            Out32 = (Out32_t)GetProcAddress(hLib, "SetPortVal");
            if (Out32 != NULL) {
                Init32 = (Init32_t)GetProcAddress(hLib, "InitializeWinIo");
                if (Init32 != NULL) {
                    Shutdown32 = (Shutdown32_t)GetProcAddress(hLib, "ShutdownWinIo");
                    if (Shutdown32 != NULL) {
                        if (Init32()) {
                            cw_use_lib = 1;
                            log_message(LOG_DEFAULT, "Using %s for PCI I/O access.", openedlib);
                        }
                    }
                }
            }
        }
#endif
        if (!cw_use_lib) {
            log_message(LOG_DEFAULT, "Cannot get I/O functions in %s, using direct PCI I/O access.", openedlib);
        }
    } else {
        log_message(LOG_DEFAULT, "Cannot open %s, trying direct PCI I/O access.", openedlib);
    }


    if (!(GetVersion() & 0x80000000) && cw_use_lib == 0) {
        log_message(LOG_DEFAULT, "Cannot use direct PCI I/O access on Windows NT/2000/Server/XP/Vista/7/8/10.");
        return -1;
    }

    res = find_pci_device(0xe159, 0x0001);

    if (res < 0) {
        log_message(LOG_DEFAULT, "No PCI CatWeasel found.");
        return -1;
    }

    log_message(LOG_DEFAULT, "PCI CatWeasel board found at $%04X.", base);

    if (detect_sid()) {
        sids_found++;
    }

    if (!sids_found) {
        log_message(LOG_DEFAULT, "No PCI CatWeasel found.");
        return -1;
    }

    log_message(LOG_DEFAULT, "PCI CatWeasel: opened, found %d SIDs.", sids_found);

    return 0;
}

int cw_pci_close(void)
{
    int i;

    if (cw_use_lib) {
#ifndef MSVC_RC
        shutdown32fp();
#else
        Shutdown32();
#endif
        FreeLibrary(hLib);
        hLib = NULL;
    }

    base = -1;

    sids_found = -1;

    log_message(LOG_DEFAULT, "PCI CatWeasel: closed");

    return 0;
}

int cw_pci_available(void)
{
    return sids_found;
}
#endif
