/*
 * parsid.c - PARallel port SID support for WIN32.
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

#include "vice.h"

#ifdef HAVE_PARSID
#include <windows.h>
#include <fcntl.h>
#include <stdio.h>
#include <conio.h>
#include <assert.h>

#include "alarm.h"
#include "parsid.h"
#include "log.h"
#include "sid-resources.h"
#include "types.h"

/* control register bits */
#define parsid_STROBE   0x01
#define parsid_AUTOFEED 0x02
#define parsid_nINIT    0x04
#define parsid_SELECTIN 0x08
#define parsid_PCD      0x20

static unsigned short parsid_ctrport;
static int parsid_use_lib = 0;
static int parsid_port_address[4];

#define sleep(x) Sleep(x * 1000)

#ifndef MSVC_RC
typedef short _stdcall (*inpfuncPtr)(short portaddr);
typedef void _stdcall (*oupfuncPtr)(short portaddr, short datum);

inpfuncPtr inp32fp;
oupfuncPtr oup32fp;
#else
typedef short (CALLBACK* Inp32_t)(short);
typedef void (CALLBACK* Out32_t)(short, short);

static Inp32_t Inp32;
static Out32_t Out32;
#endif

/* input/output functions */
static void parsid_outb(unsigned int addrint, short value)
{
    WORD addr = (WORD)addrint;

    /* make sure the above conversion did not loose any details */
    assert(addr == addrint);

    if (parsid_use_lib) {
#ifndef MSVC_RC
        (oup32fp)(addr, value);
#else
        Out32(addr, value);
#endif
    } else {
#ifdef  _M_IX86
        _outp(addr, value);
#endif
    }
}

static short parsid_inb(unsigned int addrint)
{
    WORD addr = (WORD)addrint;

    /* make sure the above conversion did not loose any details */
    assert(addr == addrint);

    if (parsid_use_lib) {
#ifndef MSVC_RC
        return (inp32fp)(addr);
#else
        return Inp32(addr);
#endif
    } else {
#ifdef  _M_IX86
        return _inp(addr);
#endif
    }
}

/* pin functions */
static void parsid_chip_select(void)
{
    parsid_ctrport |= parsid_STROBE;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_chip_deselect(void)
{
    parsid_ctrport &= ~parsid_STROBE;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_reset_start(void)
{
    parsid_ctrport |= parsid_SELECTIN;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_reset_end(void)
{
    parsid_ctrport &= ~parsid_SELECTIN;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_latch_open(void)
{
    parsid_ctrport &= ~parsid_AUTOFEED;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_latch_lock(void)
{
    parsid_ctrport |= parsid_AUTOFEED;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_RW_write(void)
{
    parsid_ctrport &= ~parsid_nINIT;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_RW_read(void)
{
    parsid_ctrport |= parsid_nINIT;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

/* parallel port direction control */
static void parsid_port_write(void)
{
    parsid_ctrport &= ~parsid_PCD;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

static void parsid_port_read(void)
{
    parsid_ctrport |= parsid_PCD;
    parsid_outb(parsid_port_address[parsid_port] + 2, parsid_ctrport);
}

#define TEST_WINDOWS_NT (!(GetVersion() & 0x80000000))

/*----------------------------------------------------------------------*/

static void parsid_sidwait(void)
{
}

/* original code by Peter Burke, modified by me. */
static int parsid_GetParallelControllerKey(char *parKey)
{
    HKEY hKey, hKey1, hKey2;
    char myData[255];
    LONG res;
    DWORD mySize, dwIndex1, dwIndex2, dwIndex3;
    FILETIME ftLastWriteTime;
    char myKey[255], myKey1[255], myKey2[255];

    if (NULL == parKey) {
        return -1;
    }

    *parKey = 0;

    sprintf(myKey, "HARDWARE\\DESCRIPTION\\System");

    res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, myKey, 0, KEY_READ, &hKey);

    if (res != ERROR_SUCCESS) {
        return -1;
    }

    for (dwIndex1 = 0; dwIndex1 <= 10; dwIndex1++) {
        mySize = sizeof(myData);
        res = RegEnumKeyEx(hKey, dwIndex1, myData, &mySize, NULL, NULL, NULL, &ftLastWriteTime);

        if (res == ERROR_SUCCESS) {
            strcpy(myKey1, myKey);
            strcat(myKey1, "\\");
            strcat(myKey1, myData);

            res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, myKey1, 0, KEY_READ, &hKey1);

            if (res != ERROR_SUCCESS) {
                return -1;
            }

            for (dwIndex2 = 0; dwIndex2 <= 10; dwIndex2++) {
                mySize = sizeof(myData);
                res = RegEnumKeyEx(hKey1, dwIndex2, myData, &mySize, NULL, NULL, NULL, &ftLastWriteTime);

                if (res == ERROR_SUCCESS) {
                    strcpy(myKey2, myKey1);
                    strcat(myKey2, "\\");
                    strcat(myKey2, myData);

                    res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, myKey2, 0, KEY_READ, &hKey2);

                    if (res != ERROR_SUCCESS) {
                        return -1;
                    }

                    for (dwIndex3 = 0; dwIndex3 <= 10; dwIndex3++) {
                        mySize = sizeof(myData);
                        res = RegEnumKeyEx(hKey2, dwIndex3, myData, &mySize, NULL, NULL, NULL, &ftLastWriteTime);

                        if (res == ERROR_SUCCESS) {
                            if (0 == strcmp(myData, "ParallelController")) {
                                strcpy(parKey, myKey2);
                                strcat(parKey, "\\");
                                strcat(parKey, myData);
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return -1;
}

static int parsid_GetAddressLptPortInTheRegistry(int myPort)
{
    HKEY phkResult;
    char myKey[255], myData[255];
    LONG res;
    DWORD mySize, myType;

    res = parsid_GetParallelControllerKey(myKey);
    if (res < 0) {
        return -1;
    }

    sprintf(myData, "%s\\%d", myKey, myPort - 1);

    res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, myData, 0, KEY_READ, &phkResult);
    if (res != ERROR_SUCCESS) {
        return -1;
    }

    mySize = sizeof(myData);
    myType = REG_BINARY;

    res = RegQueryValueEx(phkResult, "Configuration Data", NULL, &myType, (unsigned char *)myData, &mySize);
    if (res != ERROR_SUCCESS) {
        return -1;
    }

    return (myData[0x14] | myData[0x15] << 8);
}
         
typedef BOOL (CALLBACK * PROCTYPE_Toolhelp32ReadProcessMemory)(DWORD, LPCVOID, LPVOID, DWORD, LPDWORD);

static int parsid_GetAddressLptPortInTheMemory(int myPort)
{
    HINSTANCE hDLL = NULL;
    PROCTYPE_Toolhelp32ReadProcessMemory myProcPointer = NULL;
    int portAddresses[] = { 0, 0, 0, 0, 0 };
    BOOL rtn = 0;
    DWORD cbLen = 0;

    hDLL = LoadLibrary("kernel32");
    if (hDLL == NULL) {
        return -1;
    }

    myProcPointer = (PROCTYPE_Toolhelp32ReadProcessMemory)GetProcAddress(hDLL, "Toolhelp32ReadProcessMemory");
    if (myProcPointer == NULL) {
        FreeLibrary(hDLL);
        return -1;
    }

    rtn = myProcPointer(0, (LPCVOID *)0x408, portAddresses, 8, NULL);
    FreeLibrary(hDLL);

    if (rtn == 0) {
        return -1;
    }

    if (portAddresses[myPort - 1] <= 0 || portAddresses[myPort - 1] >= 0x1000) {
        return -1;
    }

    return portAddresses[myPort - 1];
}

static int parsid_GetAddressLptPort(int myPort)
{
    if (myPort < 1 || myPort > 3) {
        return -1;
    }

    if (!(GetVersion() & 0x80000000)) {
        return parsid_GetAddressLptPortInTheRegistry(myPort);
    }
    return parsid_GetAddressLptPortInTheMemory(myPort);
}

/*----------------------------------------------------------------------*/

int parsid_check_port(int port)
{
    if (port < 1 || port > 3) {
        return -1;
    }

    if (parsid_port_address[port - 1] <= 0) {
        return -1;
    }

    return 0;
}

HINSTANCE hLib = NULL;

#ifdef _MSC_VER
#  ifdef _WIN64
#    define INPOUTDLLNAME "inpoutx64.dll"
#  else
#    define INPOUTDLLNAME "inpout32.dll"
#  endif
#else
#  if defined(__amd64__) || defined(__x86_64__)
#    define INPOUTDLLNAME "inpoutx64.dll"
#  else
#    define INPOUTDLLNAME "inpout32.dll"
#  endif
#endif

static int parsid_init(void)
{
    int j;
    int ports = 0;

    if (hLib == NULL) {
        hLib = LoadLibrary(INPOUTDLLNAME);
    }

    parsid_use_lib = 0;

    if (hLib != NULL) {
#ifndef MSVC_RC
        inp32fp = (inpfuncPtr)GetProcAddress(hLib, "Inp32");
        if (inp32fp != NULL) {
            oup32fp = (oupfuncPtr)GetProcAddress(hLib, "Out32");
            if (oup32fp != NULL) {
                parsid_use_lib = 1;
            }
        }
#else
        Inp32 = (Inp32_t)GetProcAddress(hLib, "Inp32");
        if (Inp32 != NULL) {
            Out32 = (Out32_t)GetProcAddress(hLib, "Out32");
            if (Out32 != NULL) {
                parsid_use_lib = 1;
            }
        }
#endif
    }

    for (j = 0; j < 3; j++) {
        parsid_port_address[j] = parsid_GetAddressLptPort(j + 1);
        if (parsid_port_address[j] > 0) {
            ports++;
        }
    }
    if (ports == 0) {
        return -1;
    }

    if (!(GetVersion() & 0x80000000) && parsid_use_lib == 0) {
        return -1;
    }

    parsid_ctrport = parsid_inb(parsid_port_address[parsid_port] + 2);
    return 0;
}

void parsid_reset(void)
{
    parsid_RW_write();
    parsid_port_write();
    parsid_chip_select();
    parsid_latch_open();
    parsid_outb(parsid_port_address[parsid_port], 0);
    parsid_reset_start();
    sleep(1);
    parsid_reset_end();
    parsid_latch_lock();
    parsid_chip_deselect();
}

int parsid_open(int port)
{
    if (parsid_init() < 0) {
        return -1;
    }
    if (parsid_check_port(port) < 0) {
        return -1;
    }
    parsid_reset();
    return 0;
}

int parsid_close(void)
{
    parsid_reset();
    if (parsid_use_lib) {
       FreeLibrary(hLib);
       hLib = NULL;
    }
    return 0;
}

int parsid_read(WORD addr, int chipno)
{
    int value;

    parsid_outb(parsid_port_address[parsid_port], addr);
    parsid_latch_open();
    parsid_sidwait();
    parsid_latch_lock();
    parsid_port_read();
    parsid_RW_read();
    parsid_chip_select();
    parsid_sidwait();
    value = parsid_inb(parsid_port_address[parsid_port]);
    parsid_chip_deselect();
    return value;
}

void parsid_store(WORD addr, BYTE outval, int chipno)
{
    parsid_outb(parsid_port_address[parsid_port], addr);
    parsid_latch_open();
    parsid_sidwait();
    parsid_latch_lock();
    parsid_outb(parsid_port_address[parsid_port], outval);
    parsid_chip_select();
    parsid_sidwait();
    parsid_chip_deselect();
}
#endif
