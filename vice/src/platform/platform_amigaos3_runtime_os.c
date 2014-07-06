/*
 * platform_amigaos3_runtime_os.c - Amiga OS 3.x runtime version discovery.
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

/* Tested and confirmed working on the following CPU types:
 * 68000
 * 68010
 * 68020 (& 68881/68882)
 * 68030 (& 68881/68882)
 * 68040
 * 68060
 */

/* Tested and confirmed working on the following WorkBench versions:
 * 1.0
 * 1.1
 * 1.2 (33.56)
 * 1.3 (34.20)
 * 1.3.2 (34.28)
 * 1.3.3 (34.34)
 * 1.4 (36.8)
 * 2.0 (36.68)
 * 2.04 (37.67)
 * 2.05 (37.71)
 * 2.1 (38.35)
 * 2.1.1 (38.36)
 * 3.0 (39.29)
 * 3.1 (40.42)
 * 3.5 (44.2)
 * 3.5-BB2 (44.5)
 * 3.9 (45.1)
 * 3.9-BB1 (45.2)
 * 3.9-BB2 (45.3)
 */

/* Tested and confirmed working on the following KickStart versions:
 * 1.1 (31.34)
 * 1.1 (32.34)
 * 1.2 (33.166)
 * 1.2 (33.180)
 * 1.3 (34.5)
 * 2.0 (36.67)
 * 2.0 (36.143)
 * 2.04 (37.175)
 * 2.05 (37.210)
 * 2.05 (37.299)
 * 2.05 (37.300)
 * 2.05 (37.350)
 * 3.0 (39.106)
 * 3.1 (40.55)
 * 3.1 (40.60)
 * 3.1 (40.62)
 * 3.1 (40.68)
 * 3.1 (40.70)
 * 3.2 (43.1)
 * AROS KickStart ROM (51.51)
 */

#include "vice.h"

#ifdef AMIGA_M68K

#define __USE_INLINE__

#include <proto/exec.h>
#include <exec/execbase.h>
extern struct ExecBase *SysBase;

struct Library *WorkbenchBase = NULL;
struct Library *VersionBase = NULL;

static char *wbretval = NULL;
static char *ksretval = NULL;
static char osretval[100];
static char *cpuretval = NULL;

typedef struct ksver_s {
    char *name;
    int major;
    int minor;
    int softver;
} ksver_t;

static ksver_t ks_versions[] = {
    { "1.0", 30, -1, -1 },
    { "1.1", 31, -1, -1 },
    { "1.1", 32, -1, -1 },
    { "1.2", 33, -1, -1 },
    { "1.3", 34, -1, -1 },
    { "1.3", 37, 201, 30 },
    { "1.4", 35, -1, -1 },
    { "1.4", 36, -1, 16 },
    { "2.0", 36, -1, -1 },
    { "2.04", 37, -1, 175 },
    { "2.05", 37, -1, 210 },
    { "2.05", 37, -1, 299 },
    { "2.05", 37, -1, 300 },
    { "2.05", 37, -1, 350 },
    { "2.1", 38, -1, -1 },
    { "3.0", 39, -1, -1 },
    { "3.1", 40, -1, -1 },
    { "3.2", 43, -1, -1 },
    { "3.5", 45, -1, -1 },
    { "AROS", 51, -1, -1 },
    { NULL, 0, 0, 0 }
};

static char *number2kickstart(int major, int minor, int softver)
{
    int i;

    for (i = 0; ks_versions[i].name; i++) {
        if (ks_versions[i].major == major) {
            if (ks_versions[i].minor == -1 || ks_versions[i].minor == minor) {
                if (ks_versions[i].softver == -1 || ks_versions[i].softver == softver) {
                    return ks_versions[i].name;
                }
            }
        }
    }
    return NULL;
}

typedef struct wbver_s {
    char *name;
    int major;
    int minor;
} wbver_t;

static wbver_t wb_versions[] = {
    { "1.0", 30, -1 },
    { "1.1", 31, -1 },
    { "1.1", 32, -1 },
    { "1.1", 1, 1 },
    { "1.2", 33, -1 },
    { "1.3.1", 34, 25 },
    { "1.3.2", 34, 28 },
    { "1.3.3", 34, 34 },
    { "1.3", 34, -1 },
    { "1.4", 36, 1123 },
    { "1.4", 36, 1228 },
    { "1.4", 36, 16 },
    { "1.4", 36, 8 },
    { "1.4", 36, 993 },
    { "2.01", 36, 69 },
    { "2.02", 36, 70 },
    { "2.03", 36, 102 },
    { "2.04", 36, 67 },
    { "2.04", 37, 67 },
    { "2.0", 36, -1 },
    { "2.05", 37, -1 },
    { "2.1.1", 38, 36 },
    { "2.1", 38, -1 },
    { "3.0", 39, -1 },
    { "3.1", 40, -1 },
    { "3.2", 43, -1 },
    { "3.5", 44, 2 },
    { "3.5-BB1", 44, 4 },
    { "3.5-BB2", 44, 5 },
    { "3.9", 45, 1 },
    { "3.9-BB1", 45, 2 },
    { "3.9-BB2", 45, 3 },
    { NULL, 0, 0 }
};

static char *number2workbench(int major, int minor)
{
    int i;

    for (i = 0; wb_versions[i].name; i++) {
        if (wb_versions[i].major == major) {
            if (wb_versions[i].minor == -1 || wb_versions[i].minor == minor) {
                return wb_versions[i].name;
            }
        }
    }
    return NULL;
}

char *platform_get_amigaos3_runtime_os(void)
{
    if (!wbretval) {
        /* arosc.library only opens if the aros kernel is used */
        if (WorkbenchBase = OpenLibrary("arosc.library", 0)) {
            wbretval = "AROS";
        } else {
            if (VersionBase = OpenLibrary("version.library", 0)) {
                wbretval = number2workbench(VersionBase->lib_Version, VersionBase->lib_Revision);
                if (!wbretval) {
                    printf("WB major: %d, minor: %d\n", VersionBase->lib_Version, VersionBase->lib_Revision);
                }
            } else {
                wbretval = "1.0";
            }
        }

        if (VersionBase) {
            CloseLibrary(VersionBase);
        }
    }

    if (!ksretval) {
        ksretval = number2kickstart(SysBase->LibNode.lib_Version, SysBase->LibNode.lib_Revision, SysBase->SoftVer);
    }

    if (wbretval && wbretval[0] == 'A') {
        sprintf(osretval, "AROS");
    } else if (ksretval && ksretval[0] == 'A') {
        sprintf(osretval, "WorkBench %s (AROS KickStart ROM)", wbretval ? wbretval : "Unknown");
    } else {
        sprintf(osretval, "WorkBench %s (KickStart %s)", wbretval ? wbretval : "Unknown", ksretval ? ksretval : "Unknown");
    }

    return osretval;
}

#define CPU68000		0
#define CPU68010		1
#define CPU68020		2
#define CPU68030		3
#define CPU68040		4
#define CPU68060		5

#define FPU_NONE		0
#define FPU68881		1
#define FPU68882		2
#define FPU68040		3
#define FPU68060		4

#define MMU_NONE		0
#define MMU68851		1
#define MMU68030		2
#define MMU68040		3
#define MMU68060		4

static ULONG Test010(void)
{
    ULONG retval;

    asm volatile (
        "move.l	4,%%a6\n\t"
        "move.l #0,%%a1\n\t"
        "jsr -294(%%a6)\n\t"
        "move.l %%d0,%%a3\n\t"
        "move.l 50(%%a3),%%a4\n\t"
        "move.l	#MC68010Traps,50(%%a3)\n\t"
        "move.l #-1,%%d0\n\t"
        "subq.l #4,%%sp\n\t"
        "dc.w 0x4e7a,0x0801\n\t"			/* movec vbr,d0 */
        "addq.l #4,%%sp\n\t"
        "move.l %%a4,50(%%a3)\n\t"
        "bra MC68010end\n\t"

"MC68010Traps:\n\t"
        "move.l (%%sp)+,%0\n\t"
        "addq.l #4,2(%%sp)\n\t"
        "rte\n\t"

"MC68010end:\n\t"
        : "=r"(retval)
        :
        : "%d0", "%a0", "%a3", "%a5"
    );
    return retval;
}

static ULONG Test020(void)
{
    ULONG retval;

    asm volatile (
        "move.l 4,%%a6\n\t"
        "movem.l %%a0/%%a3/%%a5,-(%%sp)\n\t"
        "moveq #0,%0\n\t"
        "move.l %%a5,%%a0\n\t"
        "lea (super020,%%pc),%%a5\n\t"
        "jsr (-120,%%a6)\n\t"
        "jsr (-30,%%a6)\n\t"
        "jsr (-126,%%a6)\n\t"
        "movem.l (%%sp)+,%%a0/%%a3/%%a5\n\t"
        "bra end020\n\t"

"super020:\n\t"
        "move.l %%a0,%%a5\n\t"
        "dc.w 0x4e7a,0x8801\n\t"			/* movec vbr,a0 */
        "move.l (0x10,%%a0),-(%%sp)\n\t"
        "move.l (0x2c,%%a0),-(%%sp)\n\t"
        "move.l %%a0,-(%%sp)\n\t"
        "lea (cs020illegal,%%pc),%%a1\n\t"
        "move.l %%a1,(0x10,%%a0)\n\t"
        "move.l %%a1,(0x2C,%%a0)\n\t"
        "dc.w 0x4e7a,0x0002\n\t"			 /* movec cacr,d0 */
        "moveq #1,%0\n\t"

"exitillegal020:\n\t"
        "move.l (%%sp)+,%%a0\n\t"
        "move.l (%%sp)+,(0x2C,%%a0)\n\t"
        "move.l (%%sp)+,(0x10,%%a0)\n\t"

"flush020:\n\t"
        "nop\n\t"
        "rte\n\t"

"cs020illegal:\n\t"
        "lea (exitillegal020,%%pc),%%a0\n\t"
        "move.l %%a0,(2,%%sp)\n\t"
        "bra.b flush020\n\t"

"end020:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

static ULONG Test030(void)
{
    ULONG retval;

    asm volatile (
        "move.l 4,%%a6\n\t"
        "moveq #0,%0\n\t"
        "move.l %%a5,%%a0\t\n"
        "lea (super030,%%pc),%%a5\n\t"
        "jsr (-120,%%a6)\n\t"
        "jsr (-30,%%a6)\n\t"
        "jsr (-126,%%a6)\n\t"
        "btst.l	#4,%%d0\n\t"
        "beq not030\n\t"
        "moveq #1,%0\n\t"
        "bra end030\n\t"

"not030:\n\t"
        "moveq #0,%0\n\t"
        "bra end030\n\t"

"super030:\n\t"
        "move.l %%a0,%%a5\n\t"
        "dc.w 0x4e7a,0x1002\n\t"	/* movec cacr,d1 */
        "move.l %%d1,%%d0\n\t"
        "bset.l #4,%%d0\n\t"
        "bclr.l #0,%%d0\n\t"
        "dc.w 0x4e7b,0x0002\n\t"	/* movec d0,cacr */
        "dc.w 0x4e7a,0x0002\n\t"	/* movec cacr,d0 */
        "dc.w 0x4e7b,0x1002\n\t"	/* movec d1,cacr */
	"rte\n\t"

"end030:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

static ULONG Test040(void)
{
    ULONG retval;

    asm volatile (
        "move.l 4,%%a6\n\t"
        "moveq #0,%0\n\t"
        "move.l %%a5,%%a0\n\t"
        "lea (super040,%%pc),%%a5\n\t"
        "jsr (-120,%%a6)\n\t"
        "jsr (-30,%%a6)\n\t"
        "jsr (-126,%%a6)\n\t"
        "btst.l #15,%%d0\n\t"
        "beq not040\n\t"
        "moveq #1,%0\n\t"
        "bra end040\n\t"

"not040:\n\t"
        "moveq #0,%0\n\t"
        "bra end040\n\t"

"super040:\n\t"
        "move.l %%a0,%%a5\n\t"
        "dc.w 0x4e7a,0x1002\n\t"	/* movec cacr,d1 */
        "move.l %%d1,%%d0\n\t"
        "bset.l #15,%%d0\n\t"
        "dc.w 0x4e7b,0x0002\n\t"	/* movec d0,cacr */
        "dc.w 0x4e7a,0x0002\n\t"	/* movec cacr,d0 */
        "dc.w 0x4e7b,0x1002\n\t"	/* movec d1,cacr */
        "rte\n\t"

"end040:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

static ULONG Test060(void)
{
    ULONG retval;

    asm volatile (
        "move.l 4,%%a6\n\t"
        "moveq #0,%0\n\t"
        "move.l %%a5,%%a0\n\t"
        "lea (super060,%%pc),%%a5\n\t"
        "jsr (-120,%%a6)\n\t"
        "jsr (-30,%%a6)\n\t"
        "jsr (-126,%%a6)\n\t"
	"bra end060\n\t"

"super060:\n\t"
        "move.l %%a0,%%a5\n\t"
        "ori.w #0x700,%%sr\n\t"
        "dc.w 0x4E7A,0x8801\n\t"		/* movec vbr, a0 */
        "move.l (0x10,%%a0),-(%%sp)\n\t"
        "move.l (0x2c,%%a0),-(%%sp)\n\t"
        "move.l %%a0,-(%%sp)\n\t"
        "lea (cs060illegal,%%pc),%%a1\n\t"
        "move.l %%a1,(0x10,%%a0)\n\t"
        "move.l	%%a1,(0x2C,%%a0)\n\t"
        "dc.w 0xf4f8,0xf4d8\n\t"
	"dc.w 0x4e7a,0x0008\n\t"
        "dc.w 0x4e7a,0x0808\n\t"
        "moveq #1,%0\n\t"

"exitillegal060:\n\t"
        "move.l (%%sp)+,%%a0\n\t"
        "move.l (%%sp)+,(0x2C,%%a0)\n\t"
        "move.l (%%sp)+,(0x10,%%a0)\n\t"

"flush060:\n\t"
        "dc.w 0xf4f8,0xf4d8\n\t"
        "nop\n\t"
        "rte\n\t"

"cs060illegal:\n\t"
        "lea (exitillegal060,%%pc),%%a0\n\t"
        "move.l %%a0,(2,%%sp)\n\t"
        "bra.b flush060\n\t"

"end060:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

static ULONG TestFPU(void)
{
    ULONG retval;

    asm volatile (
        "move.l 4,%%a6\n\t"
        "moveq #0,%0\n\t"
        "move.l %%a5,%%a0\n\t"
        "lea (superFPU,%%pc),%%a5\n\t"
        "jsr (-120,%%a6)\n\t"
        "jsr (-30,%%a6)\n\t"
        "jsr (-126,%%a6)\n\t"
	"bra endFPU\n\t"

"superFPU:\n\t"
        "move.l %%a0,%%a5\n\t"
        "dc.w 0x4e7a,0x8801\n\t"		/* movec vbr,a0 */
        "move.l (0x10,%%a0),-(%%sp)\n\t"
        "move.l (0x2c,%%a0),-(%%sp)\n\t"
        "move.l %%a0,-(%%sp)\n\t"
        "lea (csFPUillegal,%%pc),%%a1\n\t"
        "move.l %%a1,(0x10,%%a0)\n\t"
        "move.l %%a1,(0x2C,%%a0)\n\t"
        "dc.w 0xf201,0xb000\n\t"		/* FMOVE FPU TO D1 */
        "moveq  #1,%0\n\t"

"exitillegalFPU:\n\t"
        "move.l (%%sp)+,%%a0\n\t"
        "move.l (%%sp)+,(0x2C,%%a0)\n\t"
        "move.l (%%sp)+,(0x10,%%a0)\n\t"

"flushFPU:\n\t"
        "move.l %%d0,-(%%sp)\n\t"
        "dc.w 0x4e7a,0x0002\n\t"		/* movec cacr,d0 */
        "or.w #(1<<3)!(1<<11),%%d0\n\t"
        "dc.w 0x4e7b,0x0002\n\t"		/* movec d0,cacr */
        "move.l (%%sp)+,%%d0\n\t"
        "nop\n\t"
        "rte\n\t"

"csFPUillegal:\n\t"
        "lea (exitillegalFPU,%%pc),%%a0\n\t"
        "move.l %%a0,(2,%%sp)\n\t"
        "bra.b flushFPU\n\t"

"endFPU:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

static ULONG Test88xFPU(void)
{
    ULONG retval;

    asm volatile (
        "move.l 4,%%a6\n\t"
        "move.l %%a5,-(%%sp)\n\t"
        "lea.l FPUSuper,%%a5\n\t"
        "jsr (-120,%%a6)\n\t"
        "jsr -30(%%a6)\n\t"
        "jsr (-126,%%a6)\n\t"
        "move.l (%%sp)+,%%a5\n\t"
        "bra endFPU88x\n\t"

"FPUSuper:\n\t"
        "move.l #68881,%0\n\t"
        "moveq #0,%%d1\n\t"
        "dc.w 0xf201,0x9000\n\t"		/* FMOVE D1 to FPU */
	"dc.w 0xF201,0xB000\n\t"		/* FMOVE FPU to D1 */
        "dc.w 0xf327\n\t"			/* fsave -(sp) */
        "cmpi.b #0x18,1(%%sp)\n\t"
        "beq.s FPU81\n\t"
        "move.l #68882,%0\n\t"

"FPU81:\n\t"
        "dc.w 0xf35f\n\t"			/* frestore (sp)+ */
        "rte\n\t"

"endFPU88x:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

static ULONG Test040FPU(void)
{
    ULONG retval;

    asm volatile (
        "move.l 4,%%a6\n\t"
        "moveq #0,%0\n\t"
        "move.l %%a5,%%a0\n\t"
        "lea (super040FPU,%%pc),%%a5\n\t"
        "jsr (-120,%%a6)\n\t"
        "jsr (-30,%%a6)\n\t"
        "jsr (-126,%%a6)\n\t"
        "bra end040FPU\n\t"

"super040FPU:\n\t"
        "move.l %%a0,%%a5\n\t"
        "dc.w 0x4e7a,0x8801\n\t"		/* movec vbr,a0 */
        "move.l (0x10,%%a0),-(%%sp)\n\t"
        "move.l (0x2c,%%a0),-(%%sp)\n\t"
        "move.l %%a0,-(%%sp)\n\t"
        "lea (cs020illegal,%%pc),%%a1\n\t"
        "move.l %%a1,(0x10,%%a0)\n\t"
        "move.l %%a1,(0x2C,%%a0)\n\t"
        "dc.w 0xf280,0x0000\n\t"		/* fnop */
        "moveq #1,%0\n\t"

"exitillegal040FPU:\n\t"
        "move.l (%%sp)+,%%a0\n\t"
        "move.l (%%sp)+,(0x2C,%%a0)\n\t"
        "move.l (%%sp)+,(0x10,%%a0)\n\t"
        "nop\n\t"
        "rte\n\t"

"flush040FPU:\n\t"
        "dc.w 0xf518\n\t"			/* PFLUSHA */
        "dc.w 0xf4f8\n\t"			/* CPUSHA BC */
        "dc.w 0xf4d8\n\t"			/* INVA	BC */
        "nop\n\t"
        "rte\n\t"

"cs040FPUillegal:\n\t"
        "lea (exitillegal040FPU,%%pc),%%a0\n\t"
        "move.l %%a0,(2,%%sp)\n\t"
        "bra.b flush040FPU\n\t"

"end040FPU:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

static ULONG Test060FPU(void)
{
    ULONG retval;

    asm volatile (
        "move.l 4,%%a6\n\t"
        "moveq #0,%0\n\t"
        "move.l %%a5,%%a0\n\t"
        "lea (super060FPU,%%pc),%%a5\n\t"
        "jsr (-120,%%a6)\n\t"
        "jsr (-30,%%a6)\n\t"
        "jsr (-126,%%a6)\n\t"
        "bra end060FPU\n\t"

"super060FPU:\n\t"
        "move.l %%a0,%%a5\n\t"
        "dc.w 0x4e7a,0x1808\n\t"		/* movec pcr,d1 */
        "move.l %%d1,%0\n\t"
        "rte\n\t"

"end060FPU:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

static ULONG Test040plusMMU(void)
{
    ULONG retval;

    asm volatile (
        "movem.l %%d2-%%a6,-(%%sp)\n\t"
        "move.l 4,%%a6\n\t"
	"lea (gogo040,%%pc),%%a5\n\t"
        "jsr (-120,%%a6)\n\t"
	"jsr (-30,%%a6)\n\t"
        "jsr (-126,%%a6)\n\t"
        "move.l %%d7,%0\n\t"
        "movem.l (%%sp)+,%%d2-%%a6\n\t"
        "bra end040plusMMU\n\t"

"gogo040:\n\t"
        "or.w #0x700,%%sr\n\t"
        "dc.w 0x4e7a,0x8801\n\t"		/* movec vbr,a0 */
        "move.l (0x10,%%a0),-(%%sp)\n\t"
        "move.l (0x2C,%%a0),-(%%sp)\n\t"
        "move.l %%a0,-(%%sp)\n\t"
        "lea (illegal040,%%pc),%%a1\n\t"
        "move.l %%a1,(0x10,%%a0)\n\t"
        "move.l %%a1,(0x2C,%%a0)\n\t"
        "bsr flush040\n\t"
        "moveq #0,%%d0\n\t"
        "moveq #-1,%%d7\n\t"
        "dc.w 0x4e7a,0x0003\n\t"		/* movec tc,d0 */
        "dc.w 0x4e7a,0x1806\n\t"		/* movec urp,d1 */
        "dc.w 0x4e7a,0x1807\n\t"		/* movec srp,d1 */

"exit040:\n\t"
        "nop\n\t"
        "nop\n\t"
        "moveq #0,%%d1\n\t"
        "tst.l %%d7\n\t"
        "beq.b gm4_nommu\n\t"
        "btst #15,%%d0\n\t"
        "beq.b gm4_nommu\n\t"
        "moveq #1,%%d1\n\t"

"gm4_nommu:\n\t"
        "move.l (%%sp)+,%%a0\n\t"
        "move.l (%%sp)+,(0x2C,%%a0)\n\t"
        "move.l (%%sp)+,(0x10,%%a0)\n\t"
        "bsr flush040\n\t"
        "nop\n\t"
        "rte\n\t"

"illegal040:\n\t"
        "moveq #0,%%d7\n\t"
        "lea (exit040,%%pc),%%a0\n\t"
        "move.l %%a0,(2,%%sp)\n\t"
        "bsr flush040\n\t"
        "nop\n\t"
        "rte\n\t"

"flush040:\n\t"
        "dc.w 0xf518\n\t"		/* PFLUSHA */
        "dc.w 0xf4f8\n\t"		/* CPUSHA BC */
        "dc.w 0xf4d8\n\t"		/* INVA	BC */
        "rts\n\t"

"end040plusMMU:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

static ULONG Test020plusMMU(void)
{
    ULONG retval;

    asm volatile (
	"move.l 4,%%a6\n\t"
        "movem.l %%a3/%%a4/%%a5,-(%%sp)\n\t"
        "move.l #0,%%a1\n\t"
        "jsr -294(a6)\n\t"
        "move.l %%d0,%%a3\n\t"
        "move.l 50(%%a3),%%a4\n\t"
        "move.l #MMUTraps,50(%%a3)\n\t"
        "subq.l #4,%%sp\n\t"
        "dc.w 0xf017,0x4200\n\t"		/* pmove tc,(sp) */
        "addq.l #4,%%sp\n\t"
        "move.l %%a4,50(%%a3)\n\t"
        "movem.l (%%sp)+,%%a3/%%a4/%%a5\n\t"
        "bra endMMU\n\t"

"MMUTraps:\n\t"
        "move.l (%%sp)+,%0\n\t"
        "addq.l #4,2(%%sp)\n\t"
        "rte\n\t"

"endMMU:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

static ULONG Test020EC(void)
{
    ULONG retval;

    asm volatile (
        "movem.l %%d2-%%a6,-(%%sp)\n\t"
        "move.l 4,%%a6\n\t"
        "jsr GetVBR\n\t"
        "move.l (0x08,%%a0),-(%%sp)\n\t"
        "move.l %%a0,-(%%sp)\n\t"
        "lea (busfailure,%%pc),%%a1\n\t"
        "move.l %%a1,(0x08,%%a0)\n\t"
        "lea AddrChipData,%%a0\n\t"
        "move.l %%a0,%%d0\n\t"
        "and.l #0xff000000,%%d0\n\t"
        "cmp.l #0,%%d0\n\t"
        "bne is32\n\t"
        "move.l #0xBADCAFFE,%%d1\n\t"
        "move.l #0xDEADBABE,%%d2\n\t"
        "move.l #0x6c4f5645,%%d3\n\t"
        "move.l #0x38626954,%%d4\n\t"
        "move.w #256-1-1,%%d5\n\t"
        "movem.l %%d1-%%d4,(a0)\n\t"
        "move.l %%a0,%%a1\n\t"

"loop:\n\t"
        "add.l #0x1000000,%%a1\n\t"
        "moveq #4-1,%%d0\n\t"
        "move.l %%a0,%%a2\n\t"
        "move.l %%a1,%%a3\n\t"

"compare:\n\t"
        "cmpm.l (%%a2)+,(%%a3)+\n\t"
        "dbne %%d0,compare\n\t"
        "tst.w %%d0\n\t"
        "dbpl %%d5,loop\n\t"
        "tst.w %%d5\n\t"
        "bpl.b exit\n\t"
        "exg %%d1,%%d4\n\t"
        "exg %%d2,%%d3\n\t"
        "movem.l %%d1-%%d4,(%%a0)\n\t"
        "movem.l (%%a1),%%d5-%%d6/%%a4-%%a5\n\t"
        "movem.l %%d1-%%d4,(%%a1)\n\t"
        "moveq #4-1,%%d0\n\t"
        "move.l %%a0,%%a2\n\t"
        "move.l %%a1,%%a3\n\t"

"compare2:\n\t"
        "cmpm.l (%%a2)+,(%%a3)+\n\t"
        "dbne %%d0,compare2\n\t"
        "movem.l %%d5-%%d6/%%a4-%%a5,(%%a1)\n\t"
        "tst.w %%d0\n\t"
        "bmi.b exit\n\t"
        "move.l #24,addressmode\n\t"

"exit:\n\t"
"busfproceed:\n\t"
        "move.l (%%sp)+,%%a0\n\t"
        "move.l (%%sp)+,(0x08,%%a0)\n\t"
        "move.l addressmode,%0\n\t"
        "movem.l (%%sp)+,%%d2-%%a6\n\t"
        "bra end020addr\n\t"
"is32:\n\t"
        "move.l #32,addressmode\n\t"
        "bra exit\n\t"

"busfailure:\n\t"
        "bclr #0,(10,%%sp)\n\t"
        "move.l #32,addressmode\n\t"
        "lea (busfproceed,%%pc),%%a0\n\t"
        "move.l %%a0,(2,%%sp)\n\t"

"bf_exit:\n\t"
        "nop\n\t"
        "rte\n\t"

"GetVBR:\n\t"
        "move.l %%a5,-(%%sp)\n\t"
        "lea (getvbr,%%pc),%%a5\n\t"
        "jsr (-30,%%a6)\n\t"
        "move.l (%%sp)+,%%a5\n\t"
        "rts\n\t"

"getvbr:\n\t"
        "dc.w 0x4e7a,0x8801\n\t"                /* movec vbr,a0 */
        "nop\n\t"
        "rte\n\t"

"AddrChipData:\n\t"
        "ds.l 4\n\t"

"addressmode:\n\t"
        "dc.l 0\n\t"

"end020addr:\n\t"
        : "=r"(retval)
        :
        : "%d1"
    );
    return retval;
}

typedef struct cpuver_s {
    char *name;
    int cpu;
    int fpu;
    int mmu;
    int buswidth;
} cpuver_t;

static cpuver_t cpu_types[] = {
    { "68000", CPU68000, FPU_NONE, MMU_NONE, 24 },
    { "68010", CPU68010, FPU_NONE, MMU_NONE, 24 },
    { "68EC020", CPU68020, FPU_NONE, MMU_NONE, 24 },
    { "68EC020+68851", CPU68020, FPU_NONE, MMU68851, 24 },
    { "68EC020+68881", CPU68020, FPU68881, MMU_NONE, 24 },
    { "68EC020+68882", CPU68020, FPU68882, MMU_NONE, 24 },
    { "68EC020+68851+68881", CPU68020, FPU68881, MMU68851, 24 },
    { "68EC020+68851+68882", CPU68020, FPU68882, MMU68851, 24 },
    { "68020", CPU68020, FPU_NONE, MMU_NONE, 32 },
    { "68020+68851", CPU68020, FPU_NONE, MMU68851, 32 },
    { "68020+68881", CPU68020, FPU68881, MMU_NONE, 32 },
    { "68020+68882", CPU68020, FPU68882, MMU_NONE, 32 },
    { "68020+68851+68881", CPU68020, FPU68881, MMU68851, 32 },
    { "68020+68851+68882", CPU68020, FPU68882, MMU68851, 32 },
    { "68EC030", CPU68030, FPU_NONE, MMU_NONE, 32 },
    { "68EC030+68881", CPU68030, FPU68881, MMU_NONE, 32 },
    { "68EC030+68882", CPU68030, FPU68882, MMU_NONE, 32 },
    { "68030", CPU68030, FPU_NONE, MMU68030, 32 },
    { "68030+68881", CPU68030, FPU68881, MMU68030, 32 },
    { "68030+68882", CPU68030, FPU68882, MMU68030, 32 },
    { "68EC040", CPU68040, FPU_NONE, MMU_NONE, 32 },
    { "68LC040", CPU68040, FPU_NONE, MMU68040, 32 },
    { "68040", CPU68040, FPU68040, MMU68040, 32 },
    { "68040", CPU68040, FPU68040, MMU_NONE, 32 },
    { "68EC060", CPU68060, FPU_NONE, MMU_NONE, 32 },
    { "68LC060", CPU68060, FPU_NONE, MMU68060, 32 },
    { "68060", CPU68060, FPU68060, MMU68060, 32 },
    { NULL, 0, 0, 0, 0 }
};

char *platform_get_amigaos3_runtime_cpu(void)
{
    int max_detect_cpu = CPU68000;
    int max_detect_fpu = FPU_NONE;
    int max_detect_mmu = MMU_NONE;
    int max_bus_width = 24;
    int retval = 0;
    int i = 0;

    if (!cpuretval) {
        if (Test010() == 8) {
            max_detect_cpu = CPU68010;
        }
        if (max_detect_cpu == CPU68010) {
            if (Test020()) {
                max_detect_cpu = CPU68020;
                max_bus_width = 32;
            }
        }
        if (max_detect_cpu == CPU68020) {
            if (Test030()) {
                max_detect_cpu = CPU68030;
            }
        }
        if (max_detect_cpu >= CPU68020) {
            if (Test040()) {
                max_detect_cpu = CPU68040;
            }
        }
        if (max_detect_cpu == CPU68040) {
            if (Test060()) {
                max_detect_cpu = CPU68060;
            }
        }
        if (max_detect_cpu == CPU68020 || max_detect_cpu == CPU68030) {
            if (TestFPU()) {
                retval = Test88xFPU();
                if (retval == 68881) {
                    max_detect_fpu = FPU68881;
                } else {
                    max_detect_fpu = FPU68882;
                }
            }
        }
        if (max_detect_cpu == CPU68040) {
            if (Test040FPU()) {
                max_detect_fpu = FPU68040;
            }
        }
        if (max_detect_cpu == 68060) {
            retval = Test060FPU() >> 16;
            if (i == 430) {
                max_detect_fpu = FPU68060;
            }
        }
        if (max_detect_cpu == 68020) {
            if (Test020plusMMU()) {
                max_detect_mmu = MMU68851;
            }
        }
        if (max_detect_cpu == 68030) {
            if (Test020plusMMU()) {
                max_detect_mmu = MMU68030;
            }
        }
        if (max_detect_cpu == 68040) {
            if (Test040plusMMU()) {
                max_detect_mmu = MMU68040;
            }
        }
        if (max_detect_cpu == 68060) {
            if (Test040plusMMU()) {
                max_detect_mmu = MMU68060;
            }
        }
        if (max_detect_cpu == 68020) {
            if (Test020EC() != 32) {
                max_bus_width = 24;
            }
        }
        for (i = 0; cpu_types[i].name; i++) {
            if (max_detect_cpu == cpu_types[i].cpu) {
                if (max_detect_fpu == cpu_types[i].fpu) {
                    if (max_detect_mmu == cpu_types[i].mmu) {
                        if (max_bus_width == cpu_types[i].buswidth) {
                            cpuretval = cpu_types[i].name;
                        }
                    }
                }
            }
        }
        if (!cpuretval) {
            cpuretval = "M68K";
            printf("CPU: %d, FPU: %d, MMU: %d, bus: %d\n", max_detect_cpu, max_detect_fpu, max_detect_mmu, max_bus_width);
        }
    }
    return cpuretval;
}
#endif
