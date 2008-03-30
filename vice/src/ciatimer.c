
/*
 * ciatimer.c - MOS6526 (CIA) timer emulation.
 *
 * Written by
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *
 * Patches and improvements by
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

#define	_CIATIMER_C

#include "vice.h"
#include "types.h"
#include "alarm.h"
#include "interrupt.h"


#if 1 /* def CIAT_NEED_LOG */

#include <stdarg.h>

static int ciat_logfl = 0;
static int logtab=0;
static const char spaces[]="                                                  ";

void ciat_login(const char *format,...) {
    va_list ap;
    va_start(ap,format);
    if(ciat_logfl) {
        printf("%s",spaces+strlen(spaces)-logtab);
        vprintf(format, ap);
        printf(" {\n");
    }
    logtab+=2;
}

void ciat_logout(const char *format,...) {
    va_list ap;
    va_start(ap,format);
    if(ciat_logfl && strlen(format)) {
        printf("%s",spaces+strlen(spaces)-logtab);
        vprintf(format, ap);
        printf("\n");
    }
    if(logtab>1) logtab-=2;
    if(ciat_logfl) {
        printf("%s}\n",spaces+strlen(spaces)-logtab);
    }
}

void ciat_log(const char *format,...) {
    va_list ap;
    va_start(ap,format);
    if(ciat_logfl) {
        printf("%s",spaces+strlen(spaces)-logtab);
        vprintf(format, ap);
        printf("\n");
    }
}

#endif

#include "ciatimer.h"

