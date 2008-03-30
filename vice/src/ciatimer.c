
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

#include "ciatimer.h"

#if 1 /* def CIAT_NEED_LOG */

#include <stdarg.h>

static int ciat_logfl = 0;
static int logtab=0;
static const char spaces[]="                                                  ";

/* extern int traceflg; */

void ciat_login(const char *format,...) {
    va_list ap;
    va_start(ap,format);
    if(/* traceflg ||*/ ciat_logfl) {
        printf("%s",spaces+strlen(spaces)-logtab);
        vprintf(format, ap);
        printf(" {\n");
    }
    logtab+=2;
}

void ciat_logout(const char *format,...) {
    va_list ap;
    va_start(ap,format);
    if((/* traceflg ||*/ ciat_logfl) && strlen(format)) {
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
    if(/* traceflg ||*/ ciat_logfl) {
        printf("%s",spaces+strlen(spaces)-logtab);
        vprintf(format, ap);
        printf("\n");
    }
}

void ciat_print_state(const ciat_t *state) 
{
    printf("%s print: clk=%d, cnt=%04x (%d), latch=%04x (%d)\n",
	state->name, state->clk, 
	state->cnt, state->cnt, 
	state->latch, state->latch
    );
    printf("          state=%04x = %s%s%s%s%s%s%s%s%s%s%s%s%s\n",
	state->state,
	(state->state & CIAT_CR_START) 	? "start " : "",
	(state->state & CIAT_CR_ONESHOT)? "cr_oneshot " : "",
	(state->state & CIAT_CR_FLOAD) 	? "cr_fload " : "",
	(state->state & CIAT_PHI2IN) 	? "Phi2 " : "",
	(state->state & CIAT_STEP) 	? "step " : "",
	(state->state & CIAT_COUNT2) 	? "cnt2 " : "",
	(state->state & CIAT_COUNT3) 	? "cnt3 " : "",
	(state->state & CIAT_COUNT) 	? "cnt " : "",
	(state->state & CIAT_LOAD1) 	? "load1 " : "",
	(state->state & CIAT_LOAD) 	? "load " : "",
	(state->state & CIAT_ONESHOT0) 	? "oneshot0 " : "",
	(state->state & CIAT_ONESHOT) 	? "oneshot " : "",
	(state->state & CIAT_OUT) 	? "out " : ""
    );
    printf("          alarm at %d\n", state->alarmclk);
}

#endif



ciat_tstate_t ciat_table[CIAT_TABLEN];

void ciat_init_table(void) 
{
    int i;
    ciat_tstate_t tmp;

    for (i = 0; i < CIAT_TABLEN; i ++) {

	tmp = i & (CIAT_CR_START 
		| CIAT_CR_ONESHOT 
		| CIAT_PHI2IN);

	if ((i & CIAT_CR_START) && (i & CIAT_PHI2IN)) 
	    tmp |= CIAT_COUNT2;
	if ((i & CIAT_COUNT2) || ((i & CIAT_STEP) && (i & CIAT_CR_START)))
	    tmp |= CIAT_COUNT3;
	if (i & CIAT_COUNT3) 
	    tmp |= CIAT_COUNT;

	if (i & CIAT_CR_FLOAD) 
	    tmp |= CIAT_LOAD1;
	if (i & CIAT_LOAD1) 
	    tmp |= CIAT_LOAD;

   	if (i & CIAT_CR_ONESHOT)
	    tmp |= CIAT_ONESHOT0;
   	if (i & CIAT_ONESHOT0)
	    tmp |= CIAT_ONESHOT;

        ciat_table[i] = tmp;
    }
}

