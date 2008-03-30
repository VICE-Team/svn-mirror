
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

/*
 * 07mar1999 a.fachat
 * complete timer rewrite
 *
 * The timer state is now saved in a separate structure and 
 * the (inline) functions operate on this structure.
 * Such the timer code need not be duplicated for both timers.
 *
 */

#undef CIAT_DEBUG 	/* is set in c64cia[12].c for debugging :-) */

#ifdef CIAT_DEBUG

#  define	CIAT_TIMER_DEBUG
#  define	INLINE

#include <stdarg.h>

#ifdef myciat_logfl
int myciat_logfl = 0;
#else
static int myciat_logfl = 0:
#endif

static int logtab=0;
static const char spaces[]="                                                  ";

#define	CIAT_LOGIN(a)	while(0) { ciat_login##a##; }
#define	CIAT_LOG(a)	while(0) { ciat_log##a##; }
#define	CIAT_LOGOUT(a)	while(0) { ciat_logout##a##; }

static void ciat_login(const char *format,...) {
    va_list ap;
    va_start(ap,format);
    if(myciat_logfl) {
        printf("%s",spaces+strlen(spaces)-logtab);
        vprintf(format, ap);
        printf(" {\n");
    }
    logtab+=2;
}

static void ciat_logout(const char *format,...) {
    va_list ap;
    va_start(ap,format);
    if(myciat_logfl && strlen(format)) {
        printf("%s",spaces+strlen(spaces)-logtab);
        vprintf(format, ap);
        printf("\n");
    }
    if(logtab>1) logtab-=2;
    if(myciat_logfl) {
        printf("%s}\n",spaces+strlen(spaces)-logtab);
    }
}

static void ciat_log(const char *format,...) {
    va_list ap;
    va_start(ap,format);
    if(myciat_logfl) {
        printf("%s",spaces+strlen(spaces)-logtab);
        vprintf(format, ap);
        printf("\n");
    }
}

#else /* CIAT_DEBUG */

#  define	INLINE			inline

#define	CIAT_LOGIN(a)
#define	CIAT_LOG(a)
#define	CIAT_LOGOUT(a)

#endif  /* CIAT_DEBUG */


typedef struct ciat_state_t {
    CLOCK	clk;		/* valid from cycle (including the cycle) */
    CLOCK	nextz;		/* clk when the timer is 0 again */
    WORD 	cnt;		/* counter value at clk. counts from latch
				   value to zero, including both */
    WORD	latch;		/* latch value at clk */
    int		running;	/* 1 = timer is running */
    int		oneshot;	/* 1 = oneshot, 0 = continous */
    int		single;		/* 1 = single step mode (like tb count ta) */
} ciat_state_t;

typedef struct ciat_t {
    const char 	*name;		/* name of timer */
    ciat_state_t	st[4];	/* timer state stack */
    int			nst;	/* number of states on stack */
    alarm_t		*alarm;	/* alarm handler */
    CLOCK		alarmclk;
    CLOCK		last_update;
} ciat_t;

/*
 * Init the timer 
 */
static void INLINE ciat_init(ciat_t *state, const char *name, 
						CLOCK cclk, alarm_t *alarm)
{
    CIAT_LOGIN(("%s init: cclk=%d",name,cclk));

    state->nst = 1;
    state->name = name;
    state->st[0].clk = cclk;
    state->st[0].cnt = 0xffff;
    state->st[0].latch = 0xffff;
    state->st[0].running = 0;
    state->st[0].oneshot = 0;
    state->st[0].single = 0;
    state->alarm = alarm;
    state->alarmclk = CLOCK_MAX;
    state->last_update = cclk;

    CIAT_LOGOUT((""));
}

/* check when the next underflow will occur and set the alarm */
/* needs update before */
static void INLINE ciat_set_alarm(ciat_t *state)
{
    CLOCK tmp = CLOCK_MAX;
    int i;

    CIAT_LOGIN(("%s set_alarm: nst=%d", state->name, state->nst));

    for (i=0;i<state->nst;i++) {
/*
	if (state->st[i].running 
		&& (tmp == CLOCK_MAX || state->st[i].clk < tmp)) {
	    tmp = state->st[i].nextz;
	} else {
*/
	    if (state->st[i].clk < tmp) {
	        tmp = state->st[i].nextz;
	    }
/*
	}
*/
    }

    CIAT_LOG((" -> alarmclk=%d", tmp));

    if (tmp != CLOCK_MAX) {
	/* FIXME in alarm.h: if state->alarm is set to a clk < current clk
	 * then the alarm is not reset, causing int_ciata to be called 
	 * even after alarm has been reset in read_icr */
	/* alarm_unset(state->alarm); */

        state->alarmclk = tmp;
	alarm_set(state->alarm, tmp);
    } else {
        state->alarmclk = tmp;
	alarm_unset(state->alarm);
    }

    CIAT_LOGOUT((""));
}

static CLOCK INLINE ciat_alarm_clk(ciat_t *state)
{
    return state->alarmclk;
}


/* 
 * Basic stack operations
 */

/* Add timer state to stack. remove any stack entries for times later
 * than the now given time */
static void INLINE ciat_tostack(ciat_t *state, ciat_state_t *timer)
{
    int i;

    timer->nextz = timer->running ? timer->clk + timer->cnt : CLOCK_MAX;

    CIAT_LOGIN(("%s tostack(t->clk=%d, st0->clk=%d, nst=%d", 
		state->name, timer->clk, state->st[0].clk, state->nst)); 
    CIAT_LOG(("             running=%d, t->cnt=%d, -> t->nextz=%d", 
		timer->running, timer->cnt, timer->nextz)); 
    CIAT_LOG(("             alarmclk=%d, t->latch=%d", 
		state->alarmclk, timer->latch)); 

    for (i=0;i<state->nst;i++) {
	if (state->st[i].clk >= timer->clk) {
	    state->nst = i;
	    break;
	}
    }
    state->st[i] = *timer;
    state->nst ++;

    CIAT_LOGOUT(("-> nst=%d", state->nst));
}

/* update timer description by checking timer stack for more up-to-date
 * descriptions. After that makes nextz > cclk. 
 * Returns 0 when no underflow has happened, and >0 if so */
/* FIXME: what when alarm is set? */
static int INLINE ciat_update(ciat_t *state, CLOCK cclk) 
{
    int i,j, n;

    n = 0;

    CIAT_LOGIN(("%s update: cclk=%d, nst=%d", state->name, cclk, state->nst));

    if (state->nst > 1) {
        for (j=0,i=1;i<state->nst;i++) {
	    if (state->st[i].clk >= state->st[i-1].nextz) {
		n++;   /* FIXME: approximation, but when will this be wrong? */
	    }
	    if (state->st[i].clk <= cclk) {
	        j = i;
	    }
        }
        if (j) {
	    for (i=0; j<state->nst; i++, j++) {
		state->st[i] = state->st[j];
	    }
	}
	state->nst = i;
    }

    CIAT_LOG(("cclk=%d, nextz=%d, latch=%d, clk=%d", 
	cclk, state->st[0].nextz, state->st[0].latch, state->st[0].clk));

    if (state->st[0].running) {
        if (state->st[0].nextz == cclk) {
	    if(state->last_update < cclk) {
	        n++;
	    }
	} else
        if (state->st[0].nextz < cclk) {
	    CLOCK tmp = cclk - state->st[0].clk;
	    n += tmp / (state->st[0].latch + 1); /* FIXME: approximate */
	    tmp -= tmp % (state->st[0].latch + 1);
	    state->st[0].clk += tmp;
            state->st[0].nextz = state->st[0].running ? 
			state->st[0].clk + state->st[0].cnt : CLOCK_MAX;

            CIAT_LOG(("tmp=%d, ->clk=%d, nextz=%d",
				tmp, state->st[0].clk, state->st[0].nextz));
	}
    } else {
	state->st[0].clk = cclk;
    }

    state->last_update = cclk;

    CIAT_LOGOUT(("-> n=%d",n));

    return n;
}

/* handle clock overflow */
static void INLINE ciat_prevent_clock_overflow(ciat_t *state, CLOCK sub)
{
    int i;

    CIAT_LOGIN(("%s prevent_clock_overflow", state->name));

    for (i=0; i<state->nst; i++) {
	state->st[i].clk -= sub;
	if (state->st[i].nextz != CLOCK_MAX) 
	    state->st[i].nextz -= sub;
    }
    if (state->alarmclk != CLOCK_MAX)
	state->alarmclk -= sub;
    if (state->last_update > sub)
	state->last_update -= sub;
    else
	state->last_update = 0;

    CIAT_LOGOUT((""));
}

/*
 * Timer operations
 */
 
/* timer reset */
static void INLINE ciat_reset(ciat_t *state, CLOCK cclk)
{
    CIAT_LOGIN(("%s reset: cclk=%d",state->name, cclk));
    
    state->nst = 1;
    state->st[0].clk = cclk;
    state->st[0].cnt = 0xffff;
    state->st[0].latch = 0xffff;
    state->st[0].running = 0;
    state->st[0].oneshot = 0;
    state->st[0].single = 0;
    state->last_update = cclk;

    state->alarmclk = CLOCK_MAX;
    alarm_unset(state->alarm);

    CIAT_LOGOUT((""));
}
/* read timer value - ciat_update _must_ have been called before! */
static WORD INLINE ciat_read_latch(ciat_t *state, CLOCK cclk)
{
    return state->st[0].latch;
}

/* read timer value - ciat_update _must_ have been called before! */
static WORD INLINE ciat_read_timer(ciat_t *state, CLOCK cclk)
{
    WORD current;
    CLOCK tmp;
    /*  (cclk - state->st[0].clk) 	= number of cycles since valid */
    /*  (state->st[0].clk + state->st[0].cnt 
					= clk of next underflow (when cnt=0) */
    if (state->st[0].running) {
	tmp = state->st[0].nextz;
	if (cclk <= tmp) {
	    current = tmp - cclk;
	} else {
	    current = state->st[0].latch - 
		((cclk - (tmp + 1)) % (state->st[0].latch + 1));
	}
    } else {
	current = state->st[0].cnt;
    }
#if 0 /* def CIAT_TIMER_DEBUG */
    printf("ciat_read_timer(cclk=%d) -> %s=%d\n",cclk, state->name, 
           (current == 0) ? state->st[0].latch : current);
#endif
    return (current == 0) ? state->st[0].latch : current;
}

/* single-step a timer. update _must_ have been called before */
static void INLINE ciat_single_step(ciat_t *state) 
{
    if (state->st[0].cnt) {
	state->st[0].cnt --;
    } else {
	/* FIXME: alarm handling */
	state->st[0].cnt = state->st[0].latch;
    }
}

static void INLINE ciat_set_latchhi(ciat_t *state, CLOCK cclk, BYTE byte)
{
    int i;

    CIAT_LOGIN(("%s set_latchhi: cclk=%d, byte=%02x", state->name, cclk, byte));

    for (i=0; i<state->nst; i++) {
	if (state->st[i].clk <= cclk) {
	    state->st[i].latch = (state->st[i].latch & 0xff) | (byte << 8);
	    if (! state->st[i].running) {
	        state->st[i].cnt = (state->st[i].latch & 0xff) | (byte << 8);
	    }
	}
    }

    CIAT_LOGOUT((""));
}

static void INLINE ciat_set_latchlo(ciat_t *state, CLOCK cclk, BYTE byte)
{
    int i;

    CIAT_LOGIN(("%s set_latchlo: cclk=%d, byte=%02x", state->name, cclk, byte));

    for (i=0; i<state->nst; i++) {
	if (state->st[i].clk <= cclk) {
	    state->st[i].latch = (state->st[i].latch & 0xff00) | byte;
	}
    }

    CIAT_LOGOUT((""));
}


/* needs update before */
static void INLINE ciat_set_ctrl(ciat_t *state, CLOCK cclk, BYTE byte) 
{
    ciat_state_t timer;
    WORD current = ciat_read_timer(state, cclk+1);

    CIAT_LOGIN((" %s set_ctrl: cclk=%d, byte=%02x, CRA=%02x, CRB=%02x, int=%02x",
	state->name, cclk, byte, cia[CIA_CRA], cia[CIA_CRB], ciaint));
 
    /* FIXME: */
    if ( (byte ^ state->st[0].oneshot) & 8) {
        timer = state->st[0];
        timer.oneshot = byte & 8;
	timer.clk = cclk + 1;
	ciat_tostack(state, &timer);
    }

    switch (byte & 0x11) {
      case 0x00:
	if (state->st[0].running) {
            current = ciat_read_timer(state, cclk+2);
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = current;
	    timer.running = 0;
	    timer.oneshot = byte & 8;
	    ciat_tostack(state, &timer);
	}
	break;
      case 0x01:
	if (!state->st[0].running) {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = current;
	    timer.running = 1;
	    timer.oneshot = byte & 8;
	    ciat_tostack(state, &timer);
	}
	break;
      case 0x10:
	if (state->st[0].running) {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = timer.latch;
	    timer.running = 0;
	    timer.oneshot = byte & 8;
	    ciat_tostack(state, &timer);
	} else {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = timer.latch;
	    timer.running = 0;
	    timer.oneshot = byte & 8;
	    ciat_tostack(state, &timer);
	}
	break;	
      case 0x11:
	if (!state->st[0].running) {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = timer.latch;
	    timer.running = 1;
	    timer.oneshot = byte & 8;
	    ciat_tostack(state, &timer);
	    timer.clk ++;
	    ciat_tostack(state, &timer);
	} else {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = timer.latch;
	    timer.running = 1;
	    timer.oneshot = byte & 8;
	    ciat_tostack(state, &timer);
	    timer.clk ++;
	    ciat_tostack(state, &timer);
	}
	break;
    }

    ciat_set_alarm(state);

    CIAT_LOGOUT((""));
}

static void INLINE ciat_ack_alarm(ciat_t *state, CLOCK cclk)
{
    CIAT_LOGIN(("%s ack_alarm: cclk=%d, alarmclk=%d", 
					state->name, cclk, state->alarmclk));
    
    if(cclk >= state->alarmclk) {
	state->alarmclk = CLOCK_MAX;
	alarm_unset(state->alarm);

	if(cclk == state->st[0].nextz) {
/*
            ciat_state_t timer;
            WORD current = ciat_read_timer(state, cclk+1);

printf("%s: ack_alarm hit @ cclk=%d!\n", state->name, cclk);
	    timer = state->st[0];
	    timer.clk = cclk;
	    timer.cnt = current;
	    ciat_tostack(state, &timer);
*/
            state->st[0].nextz += state->st[0].latch + 1;
	}
	/* FIXME: what happens when the else clause here happens...????? */


	if (state->st[0].oneshot) {
            ciat_state_t timer;
            WORD current = ciat_read_timer(state, cclk+1);

            timer = state->st[0];
            timer.clk = cclk + 2;
            timer.cnt = current;
            timer.running = 0;
            ciat_tostack(state, &timer);

	    ciat_set_alarm(state);
	}
    } else {
#ifdef CIAT_TIMER_DEBUG
	/* this should not happen! */
        cclk=*((CLOCK*)0);
#endif
    }

    CIAT_LOGOUT((""));
}

