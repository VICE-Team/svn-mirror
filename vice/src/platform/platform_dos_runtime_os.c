/*
 * platform_dos_runtime_os.c - DOS runtime version discovery.
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

/* Tested and confirmed working on the following DOS systems:
 - Compaq DOS 3.31
 - Compaq DOS 5.0
 - DR DOS 3.40
 - DR DOS 6.0
 - DR DOS 7.03
 - DR DOS 8.0
 - FreeDOS 0.4
 - FreeDOS 0.5
 - FreeDOS 0.7
 - FreeDOS 0.9
 - FreeDOS 1.1
 - MSDOS 3.20 (Generic)
 - MSDOS 3.20 (Apricot OEM)
 - MSDOS 3.20 (Zenith OEM)
 - MSDOS 3.21 (Generic)
 - MSDOS 3.30 (Generic)
 - MSDOS 3.30a (AT&T)
 - MSDOS 5.0 (Amstrad OEM)
 - MSDOS 5.0 (AST OEM)
 - MSDOS 6.0 (Generic)
 - MSDOS 6.21 (Generic)
 - MSDOS 8.0 (Stand Alone)
 - Novell DOS 7.0
 - PCDOS 3.10
 - PCDOS 3.30
 - PCDOS 4.0
 - PCDOS 5.00
 - REAL/32 7.x
*/

/* Tested and confirmed working on the following DOS GUI's:
 - DESQView
 - Windows For Workgroups 3.10
*/

/* Tested and confirmed working on the following DOS on Windows systems:
 - Windows 95 Original
 - Windows NT 3.50 Server
 - Windows NT 3.51 Server
 - Windows NT 4.0 Embedded
 - Windows NT 4.0 Workstation
 - Windows NT 4.0 Terminal Server
 - Windows 2000 Advanced Server
 - Windows XP Embedded
 - Windows XP PE
 - Windows XP Starter
 - Windows XP Pro
 - Windows 2003 Small Business Server
 - Windows Home Server
 - Windows 2003 R2 Standard Server
 - Windows 2003 R2 Datacenter Server
 - Windows Vista Home Premium
 - Windows 10 Pro
*/

/* Tested and confirmed working on the following other DOS emulations:
 - OS/2 3.0
 - OS/2 4.0
 - OS/2 4.5
 - EComStation 2.0 RC4
*/

#include "vice.h"

#ifdef __MSDOS__

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <dpmi.h>

#include "lib.h"
#include "util.h"

static char archdep_os_version[128];
static int got_os = 0;

typedef struct dos_version_s {
    char *name;
    char *flavor;
    int major;
    int minor;
    int oem;
    char *ver_string;
} dos_version_t;

static dos_version_t dos_versions[] = {
    { "Compaq DOS 3.31",          "IBMPcDos",    3, 31,  -1, "The COMPAQ Personal Computer MS-DOS " },
    { "Compaq DOS 5.0",           "IBMPcDos",    5,  0,   0, "COMPAQ MS-DOS Version 5.00   " },
    { "DR DOS 3.40",              "IBMPcDos",    3, 31,  -1, "DR DOS Release 3.40" },
    { "DR DOS 6.0",               "IBMPcDos",    3, 31,  -1, "DR DOS Release 6.0" },
    { "DR DOS 7.03",              "IBMPcDos",    6,  0,   0, "Caldera DR-DOS 7.03" },
    { "DR DOS 8.0",               "IBMPcDos",    6,  0,   0, "DeviceLogics DR-DOS 8.0 " },
    { "REAL/32",                  "IBMPcDos",    3, 31,  -1, "C:\\REAL32\\BIN\\COMMAND.COM" },
    { "FreeDOS 0.4",              "??Unknown??", 4,  0,  -1, "FreeCom version 0.76b [Mar 24 1999]" },
    { "FreeDOS 0.5",              "??Unknown??", 4,  0,  -1, "FreeCom version 0.79a [Mar 31 2000]" },
    { "FreeDOS 0.7",              "??Unknown??", 5,  0, 254, "FreeCom version 0.83 Beta 28 [Aug 15 2001]" },
    { "FreeDOS 0.9",              "??Unknown??", 7, 10, 253, "FreeDOS 0.9" },
    { "FreeDOS 1.1",              "??Unknown??", 7, 10, 253, NULL },
    { "MSDOS 3.20",               "MS-DOS",      3, 20,  -1, "Microsoft MS-DOS Version 3.20" },
    { "MSDOS 3.20 (Apricot OEM)", "MS-DOS",      3, 20,  -1, "Microsoft(R) MS-DOS(R)  Version 3.20" },
    { "MSDOS 3.20 (Zenith OEM)",  "ZenitDOS",    3, 20,  -1, NULL },
    { "MSDOS 3.21",               "MS-DOS",      3, 21,  -1, "Microsoft(R) MS-DOS(R)  Version 3.21" },
    { "MSDOS 3.30",               "IBMPcDos",    3, 30,  -1, "Microsoft(R) MS-DOS(R)  Version 3.30" },
    { "MSDOS 3.30A (AT&T OEM)",   "OlivtDOS",    3, 30,  -1, "Microsoft(R) MS-DOS(R)  Version 3.30a" },
    { "MSDOS 5.0 (Amstrad OEM)",  "MS-DOS",      5,  0, 255, "Microsoft(R) MS-DOS(R) Version 5.00" },
    { "MSDOS 5.0 (AST OEM)",      "IBMPcDos",    5,  0,   0, "Microsoft(R) MS-DOS(R) Version 5.00 from AST RESEARCH INC." },
    { "MSDOS 6.0",                "MS-DOS",      6,  0, 255, NULL },
    { "MSDOS 6.21",               "MS-DOS",      6, 20, 255, "Microsoft(R) MS-DOS(R) Version 6.21" },
    { "MSDOS 7.0",                "MS-DOS",      7,  0, 255, NULL },
    { "MSDOS 8.0",                "MS-DOS",      8,  0, 255, NULL },
    { "Novell DOS 7.0",           "IBMPcDos",    6,  0,   0, "Novell DOS 7" },
    { "OS/2 3.0",                 "IBMPcDos",   20, 30,   0, NULL },
    { "OS/2 4.0",                 "IBMPcDos",   20, 40,   0, NULL },
    { "OS/2 4.5",                 "IBMPcDos",   20, 45,   0, "The Operating System/2 Version is 4.50 " },
    { "EComStation 2.0 RC4",      "IBMPcDos",   20, 45,   0, NULL },
    { "PCDOS 3.10",               "IBMPcDos",    3, 10,  -1, "The IBM Personal Computer DOS" },
    { "PCDOS 3.30",               "IBMPcDos",    3, 30,  -1, "The IBM Personal Computer DOS" },
    { "PCDOS 3.31",               "IBMPcDos",    3, 31,  -1, NULL },
    { "PCDOS 4.0",                "IBMPcDos",    4,  0,  -1, "IBM DOS Version 4.00" },
    { "PCDOS 5.00",               "IBMPcDos",    5,  0,   0, "IBM DOS Version 5.00" },
    { NULL,                       NULL,         -1, -1,  -1, NULL }
};

typedef struct dos_win_version_s {
    char *name;
    char *cmd_version;
    char *prod_version;
} dos_win_version_t;

static dos_win_version_t dos_win_versions[] = {
    { "Windows NT 3.50",                                          "Windows NT Version 3.50 ",                   NULL },
    { "Windows NT 3.51",                                          "Windows NT Version 3.51 ",                   NULL },
    { "Windows NT 4.0 Workstation",                               "Windows NT Version 4.0 ",                    "Windows NT Workstation" },
    { "Windows NT 4.0 Terminal Server",                           "Windows NT Version 4.0  ",                   "Windows NT Server, Terminal Server Edition" },
    { "Windows 2000 Advanced Server",                             "Microsoft Windows 2000 [Version 5.00.2195]", "Windows 2000 Advanced Server" },
    { "Windows XP Starter/Home",                                  "Microsoft Windows XP [Version 5.1.2600]",    "Windows XP Home Edition" },
    { "Windows XP Pro",                                           "Microsoft Windows XP [Version 5.1.2600]",    "Windows XP Professional" },
    { "Windows XP",                                               "Microsoft Windows XP [Version 5.1.2600]",    NULL },
    { "Windows 2003 Small Business Server / Windows Home Server", "Microsoft Windows [Version 5.2.3790]",       "Windows Server 2003 for Small Business Server" },
    { "Windows 2003 R2 Standard Server",                          "Microsoft Windows [Version 5.2.3790]",       "Windows Server 2003, Standard Edition" },
    { "Windows 2003 R2 Datacenter Server",                        "Microsoft Windows [Version 5.2.3790]",       "Windows Server 2003, Datacenter Edition" },
    { "Windows Vista Home Basic",                                 "Microsoft Windows [Version 6.0.6000]",       NULL },
    { "Windows 10",                                               "Microsoft Windows [Version 10.0.10240]",     NULL },
    { NULL,                                                       NULL,                                         NULL }
};

static char *illegal_strings[] = {
    "The shell is about to be terminated, though, this is",
    NULL
};

static int check_illegal_string(char *string)
{
    int i;

    for (i = 0; illegal_strings[i]; ++i) {
        if (!strcmp(string, illegal_strings[i])) {
            return 1;
        }
    }
    return 0;
}

static char *get_prod_spec_string(char *command)
{
    char buffer[160];
    FILE *infile = NULL;
    char *retval = NULL;
    int found = 0;

    printf("Trying a %s\n", command);
    infile = popen(command, "r");
    if (infile) {
        do {
            if (!fgets(buffer, 159, infile)) {
                found = 2;
            } else {
                if (!strncmp(buffer, "Product=", 8)) {
                    found = 1;
                }
            }
        } while (!found);
        pclose(infile);
        if (found == 1) {
            buffer[strlen(buffer) - 1] = 0;
            retval = lib_stralloc(buffer + 8);
            printf("Got version %s!!!\n", retval);
        }

    }
    if (!retval) {
        printf("Returning NULL\n");
    } else {
        if (check_illegal_string(retval)) {
            lib_free(retval);
            retval = NULL;
        }
    }
    return retval;
}

static char *get_cmd_ver_string(char *command)
{
    char buffer[160];
    FILE *infile = NULL;
    char *retval = NULL;
    int found = 0;
    int i;

    printf("Trying a %s\n", command);
    infile = popen(command, "r");
    if (infile) {
        do {
            if (!fgets(buffer, 159, infile)) {
                found = 2;
            } else {
                if (strlen(buffer) > 1) {
                    for (i = 0; buffer[i]; ++i) {
                        if (buffer[i] != ' ' && buffer[i] != '\r' && buffer[i] != '\n') {
                            found = 1;
                        }
                    }
                }
            }
        } while (!found);
        pclose(infile);
        if (found == 1) {
            buffer[strlen(buffer) - 1] = 0;
            retval = lib_stralloc(buffer);
            printf("Got version %s!!!\n", retval);
        }

    }
    if (!retval) {
        printf("Returning NULL\n");
    } else {
        if (check_illegal_string(retval)) {
            lib_free(retval);
            retval = NULL;
        }
    }
    return retval;
}

static char *get_command_com_string(void)
{
    FILE *infile = NULL;
    char buffer[65280];
    char *ptr = NULL;
    char *ptr2 = NULL;
    int i;
    int found = 0;
    char *retval = NULL;
    char *comspec = NULL;

    printf("Analyzing command.com\n");

    comspec = getenv("COMSPEC");

    if (!comspec) {
        comspec = "C:\\COMMAND.COM";
    }

    if (!strcmp(comspec, "C:\\BIN\\CMDXSWP.COM")) {
        return lib_stralloc("FreeDOS 0.9");
    }

    printf("opening %s\n", comspec);
    infile = fopen(comspec, "rb");
    if (infile) {
        memset(buffer, 0, 65280);
        fread(buffer, 1, 65280, infile);
        fclose(infile);
        for (i = 0; !found; ++i) {
            if (i == 65280 - 4) {
                found = 2;
            } else if (buffer[i] == '\r' && buffer[i + 1] == '\n' && buffer[i + 2] == '\r' && buffer[i + 3] == '\n') {
                found = 1;
            }
        }
        if (found == 1) {
            ptr = buffer + i + 3;
            ptr2 = strstr(ptr, "\r");
            if (ptr2) {
                ptr2[0] = 0;
                retval = lib_stralloc(ptr);
                printf("Command.com string : %s!!!\n", retval);
            }
        }
    }

    if (retval) {
        if (check_illegal_string(retval)) {
            lib_free(retval);
            retval = NULL;
        }
    }

    return retval;
}

static char *get_version_from_env(void)
{
    char *os = getenv("OS");
    char *ver = getenv("VER");
    char *retval = NULL;

    if (!os && !ver) {
        retval = util_concat(os, " ", ver, NULL);
    } else {
        printf("env returns no OS and VERSION\n");
    }
    return retval;
}

static int get_real32_ver(void)
{
    __dpmi_regs r;

    printf("Trying to get real32 version\n");

    r.h.cl = 0xa3;
    __dpmi_int(0xe0, &r);

    printf("Returning real32 %d\n", r.x.ax);

    return (int)r.x.ax;
}

static int desqview_present(void)
{
    __dpmi_regs r;

    printf("Trying to see if desqview is present\n");

    r.x.ax = 0xde00;
    __dpmi_int(0x2f, &r);

    return (r.h.al == 0xff);
}

static void get_desqview_version(int *major, int *minor)
{
    __dpmi_regs r;

    printf("Getting desqview version\n");

    r.h.ah = 0x2b;
    r.x.bx = 0;
    r.x.cx = 0x4445;
    r.x.dx = 0x5451;
    r.h.al = 0x01;
    __dpmi_int(0x21, &r);

    *major = r.h.bh;
    *minor = r.h.bl;
}

static int get_dos_oem_nr(void)
{
    __dpmi_regs r;

    printf("Getting oem number\n");

    r.h.ah = 0x30;
    r.h.al = 0x00;
    __dpmi_int(0x21, &r);

    printf("oem number: %d\n", r.h.bh);

    return (int)r.h.bh;
}

static int get_windows_version(int *major, int *minor, int *mode)
{
    __dpmi_regs r;
    int retval = 0;

    printf("Getting windows version\n");

    r.x.ax = 0x160a;
    __dpmi_int(0x2f, &r);

    if (r.x.ax == 0) {
        *major = r.h.bh;
        *minor = r.h.bl;
        *mode = r.x.cx;
        retval = 1;
    }
    return retval;
}

char *platform_get_dos_runtime_os(void)
{
    unsigned short real_version;

    int version_major = -1;
    int version_minor = -1;
    int version_oem = -1;
    int win_major = 0;
    int win_minor = 0;
    int win_mode = 0;
    int real32_version = 0;
    const char *version_flavor = _os_flavor;
    char *version_ver_string = NULL;
    char *cmd_ver_string = NULL;
    char *prodver_string = NULL;
    char *systemroot = NULL;
    char *prodver_command = NULL;
    char *real32_string = NULL;

    int i;
    int found = 0;

    if (!got_os) {
        real_version = _get_dos_version(1);
        version_major = real_version >> 8;
        version_minor = real_version & 0xff;

        if (version_major >= 5) {
            version_oem = get_dos_oem_nr();
        }

        version_ver_string = get_command_com_string();

        if (!version_ver_string) {
            version_ver_string = get_cmd_ver_string("ver");
        }

        if (!version_ver_string) {
            version_ver_string = get_version_from_env();
        }

        if (!version_ver_string) {
            version_ver_string = getenv("COMSPEC");
            if (version_ver_string) {
                printf("comspec string is %s\n", version_ver_string);
                version_ver_string = lib_stralloc(version_ver_string);
            }
        }

        if (version_major == 5 && version_minor == 50) {
            if (!cmd_ver_string) {
                cmd_ver_string = get_cmd_ver_string("ver");
                if (!strcmp(cmd_ver_string, "MS-DOS Version 5.00.500")) {
                    lib_free(cmd_ver_string);
                    cmd_ver_string = get_cmd_ver_string("command.com /c cmd /c ver");
                }
            }

            for (i = 0; !found && dos_win_versions[i].name; ++i) {
                if (!strcmp(cmd_ver_string, dos_win_versions[i].cmd_version)) {
                    if (!prodver_string && dos_win_versions[i].prod_version) {
                        if (!systemroot) {
                            systemroot = getenv("SYSTEMROOT");
                            if (systemroot) {
                                printf("system root is %s\n", systemroot);
                            } else {
                                printf("system root returned NULL\n");
                            }
                        }
                        prodver_command = util_concat("type ", systemroot, "\\system32\\prodspec.ini", NULL);
                        prodver_string = get_prod_spec_string(prodver_command);
                        lib_free(prodver_command);
                        prodver_command = NULL;
                    }
                    if (!strcmp(prodver_string, dos_win_versions[i].prod_version)) {
                        found = 1;
                    }
                }
            }
            if (!found) {
                sprintf(archdep_os_version, "Unknown DOS on Windows version");
            } else {
                sprintf(archdep_os_version, "DOS on %s", dos_win_versions[i - 1].name);
            }
            if (cmd_ver_string) {
                lib_free(cmd_ver_string);
                cmd_ver_string = NULL;
            }
            if (prodver_string) {
                lib_free(prodver_string);
                prodver_string = NULL;
            }
        } else {
            for (i = 0; !found && dos_versions[i].name; ++i) {
                if (!strcmp(version_flavor, dos_versions[i].flavor)) {
                    if (version_major == dos_versions[i].major) {
                        if (version_minor == dos_versions[i].minor) {
                            if ((dos_versions[i].ver_string && version_ver_string && !strcmp(dos_versions[i].ver_string, version_ver_string)) || !dos_versions[i].ver_string) {
                                found = 1;
                            }
                        }
                    }
                }
            }

            if (!found) {
                sprintf(archdep_os_version, "Unknown DOS version: %s %d %d %d %s", version_flavor, version_major, version_minor, version_oem, (version_ver_string) ? version_ver_string : "(N/A)");
            } else {
                sprintf(archdep_os_version, dos_versions[i - 1].name);
                if (desqview_present()) {
                    get_desqview_version(&win_major, &win_minor);
                    if (!win_major && !win_minor) {
                        sprintf(archdep_os_version, "%s [DESQView]", archdep_os_version);
                    } else {
                        sprintf(archdep_os_version, "%s [DESQView %d.%d]", archdep_os_version, win_major, win_minor);
                    }
                }
                if (get_windows_version(&win_major, &win_minor, &win_mode)) {
                    if (win_major == 4 && win_minor == 0) {
                        sprintf(archdep_os_version, "%s [Windows 95 Original]", archdep_os_version);
                    } else {
                        sprintf(archdep_os_version, "%s [Windows %d.%d]", archdep_os_version, win_major, win_minor);
                    }
                }
                if (!strcmp(archdep_os_version, "REAL/32")) {
                    real32_version = get_real32_ver();
                    if (real32_version >> 8 == 0x14) {
                        switch (real32_version & 0xff) {
                            case 0x32:
                                real32_string = "DR Concurrent PC DOS 3.2";
                                break;
                            case 0x41:
                                real32_string = "DR Concurrent DOS 4.1";
                                break;
                            case 0x50:
                                real32_string = "DR Concurrent DOS/XM 5.0";
                                break;
                            case 0x63:
                                real32_string = "DR Multiuser DOS 5.0";
                                break;
                            case 0x65:
                                real32_string = "DR Multiuser DOS 5.01";
                                break;
                            case 0x66:
                                real32_string = "DR Multiuser DOS 5.1";
                                break;
                            case 0x67:
                                real32_string = "IMS Multiuser DOS 7.0/7.1";
                                break;
                            case 0x68:
                                real32_string = "IMS REAL/32 7.50/7.51";
                                break;
                            case 0x69:
                                real32_string = "IMS REAL/32 7.52/7.53";
                                break;
                            default:
                                real32_string = "IMS REAL/32 7.6";
                        }
                        sprintf(archdep_os_version, real32_string);
                    }
                }
            }
        }
        got_os = 1;
        if (version_ver_string) {
            lib_free(version_ver_string);
            version_ver_string = NULL;
        }
    }
    return archdep_os_version;
}
#endif
