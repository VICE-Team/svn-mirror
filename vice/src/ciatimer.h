
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

#ifndef _CIATIMER_H
#define _CIATIMER_H

#undef	CIAT_DEBUG
#undef	NO_INLINE

/***************************************************************************/
/* Timer debug stuff */

#ifdef CIAT_DEBUG

#  define 	CIAT_NEED_LOG
#  define 	NO_INLINE
#  define	CIAT_LOGIN(a)	do { ciat_login a; } while(0)
#  define	CIAT_LOG(a)	do { ciat_log a; } while(0)
#  define	CIAT_LOGOUT(a)	do { ciat_logout a; } while(0)

#else /* CIAT_DEBUG */

#  define	CIAT_LOGIN(a)
#  define	CIAT_LOG(a)
#  define	CIAT_LOGOUT(a)

#endif  /* CIAT_DEBUG */

#ifdef CIAT_NEED_LOG

extern void ciat_login(const char *format,...);
extern void ciat_logout(const char *format,...);
extern void ciat_log(const char *format,...);

#endif

/***************************************************************************/
/* For maximum performance (these routines are small but used very often), we
   allow use of inlined functions.  This can be overridden by defining
   NO_INLINE (useful for debugging and profiling).  */

#if !defined NO_INLINE
#  ifndef INLINE_CIAT_FUNCS
#    define INLINE_CIAT_FUNCS
#  endif
#else
#  undef INLINE_CIAT_FUNCS
#endif

/***************************************************************************/

typedef struct ciat_state_t {
    CLOCK	clk;		/* valid from cycle (including the cycle) */
    CLOCK	nextz;		/* clk when the timer is 0 again */
    WORD 	cnt;		/* counter value at clk. counts from latch
				   value to zero, including both */
    WORD	latch;		/* latch value at clk */
    int		runp2;		/* 1 = timer counts clk pulses */
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

/***************************************************************************/

/* If we do not want the interrupt functions to be inlined, they are only
   compiled once when included in `ciatimer.c'.  */

#ifdef INLINE_CIAT_FUNCS
#  define _CIAT_FUNC inline static
#else
#  define _CIAT_FUNC
#endif

#if defined INLINE_CIAT_FUNCS || defined _CIATIMER_C

/*
 * Init the timer 
 */
_CIAT_FUNC void ciat_init(ciat_t *state, const char *name, 
						CLOCK cclk, alarm_t *alarm)
{
    CIAT_LOGIN(("%s init: cclk=%d",name,cclk));

    state->nst = 1;
    state->name = name;
    state->st[0].clk = cclk;
    state->st[0].cnt = 0xffff;
    state->st[0].latch = 0xffff;
    state->st[0].runp2 = 0;
    state->st[0].oneshot = 0;
    state->st[0].single = 0;
    state->alarm = alarm;
    state->alarmclk = CLOCK_MAX;
    state->last_update = cclk;

    CIAT_LOGOUT((""));
}

/* check when the next underflow will occur and set the alarm */
/* needs update before */
_CIAT_FUNC void ciat_set_alarm(ciat_t *state)
{
    CLOCK tmp = CLOCK_MAX;
    int i;

    CIAT_LOGIN(("%s set_alarm: nst=%d", state->name, state->nst));

    for (i=0;i<state->nst;i++) {
	if (state->st[i].clk <= tmp) {
	    tmp = state->st[i].nextz;
	}
    }

    CIAT_LOG((" -> alarmclk=%d", tmp));

    state->alarmclk = tmp;
    if (tmp != CLOCK_MAX) {
	alarm_set(state->alarm, tmp);
    } else {
	alarm_unset(state->alarm);
    }

    CIAT_LOGOUT((""));
}

_CIAT_FUNC CLOCK ciat_alarm_clk(ciat_t *state)
{
    return state->alarmclk;
}


/* 
 * Basic stack operations
 */

/* Add timer state to stack. remove any stack entries for times later
 * than the now given time */
_CIAT_FUNC void ciat_tostack(ciat_t *state, ciat_state_t *timer)
{
    int i;

    timer->nextz = timer->runp2 ? timer->clk + timer->cnt : CLOCK_MAX;

    CIAT_LOGIN(("%s tostack(t->clk=%d, st0->clk=%d, nst=%d", 
		state->name, timer->clk, state->st[0].clk, state->nst)); 
    CIAT_LOG(("             runp2=%d, t->cnt=%d, -> t->nextz=%d", 
		timer->runp2, timer->cnt, timer->nextz)); 
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
 * Returns 0 when no underflow has happened, or n the number of underflows.
 * *now is set to 1 if the last underflow is at the current clock */
/* FIXME: what when alarm is set? */
_CIAT_FUNC int ciat_update(ciat_t *state, CLOCK cclk, int *now) 
{
    int i,j, n, neg;

    neg = 0;
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

    CIAT_LOG(("cclk=%d, nextz=%d, latch=%d, st0.clk=%d, st0.cnt=%d", 
	cclk, state->st[0].nextz, state->st[0].latch, state->st[0].clk,
	state->st[0].cnt));

    if (state->st[0].runp2) {
        if (state->st[0].nextz == cclk) {
	    if(state->last_update < cclk) {
	        n++;
	    }
	    neg = 1;
	} else
        if (state->st[0].nextz < cclk) {
	    CLOCK tmp = cclk - state->st[0].nextz;

	    n += 1 + (tmp / (state->st[0].latch + 1)); 
	    tmp -= tmp % (state->st[0].latch + 1);
	    state->st[0].clk += tmp;
            state->st[0].nextz = state->st[0].runp2 ? 
			state->st[0].clk + state->st[0].cnt : CLOCK_MAX;

            CIAT_LOG(("tmp=%d, ->clk=%d, nextz=%d",
				tmp, state->st[0].clk, state->st[0].nextz));
	}
    } else {
	state->st[0].clk = cclk;
    }

    state->last_update = cclk;

    CIAT_LOGOUT(("-> n=%d, neg=%d",n,neg));

    if(now) *now=neg;
 
    return n;
}

/* handle clock overflow */
_CIAT_FUNC void ciat_prevent_clock_overflow(ciat_t *state, CLOCK sub)
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
_CIAT_FUNC void ciat_reset(ciat_t *state, CLOCK cclk)
{
    CIAT_LOGIN(("%s reset: cclk=%d",state->name, cclk));
    
    state->nst = 1;
    state->st[0].clk = cclk;
    state->st[0].cnt = 0xffff;
    state->st[0].latch = 0xffff;
    state->st[0].runp2 = 0;
    state->st[0].oneshot = 0;
    state->st[0].single = 0;
    state->last_update = cclk;

    state->alarmclk = CLOCK_MAX;
    alarm_unset(state->alarm);

    CIAT_LOGOUT((""));
}
/* read timer value - ciat_update _must_ have been called before! */
_CIAT_FUNC WORD ciat_read_latch(ciat_t *state, CLOCK cclk)
{
    return state->st[0].latch;
}

/* read timer value - ciat_update _must_ have been called before! */
_CIAT_FUNC WORD ciat_read_timer(ciat_t *state, CLOCK cclk)
{
    WORD current;
    CLOCK tmp;
    /*  (cclk - state->st[0].clk) 	= number of cycles since valid */
    /*  (state->st[0].clk + state->st[0].cnt 
					= clk of next underflow (when cnt=0) */
    if (state->st[0].runp2) {
	tmp = state->st[0].nextz;
	if (cclk <= tmp) {
	    current = tmp - cclk;
	} else {
	    current = state->st[0].latch - 
		((cclk - (tmp + 1)) % (state->st[0].latch + 1));
	}
	/* FIXME: what is when counter is stopped with "0" in it? */
	if (current == 0)
	    current = state->st[0].latch;
    } else {
	current = state->st[0].cnt;
    }
#if 0 /* def CIAT_TIMER_DEBUG */

    ciat_log("ciat_read_timer(cclk=%d) -> %s=%d\n",cclk, state->name, 
           (current == 0) ? state->st[0].latch : current);

if(current==0x0f) { extern int ciat_logfl; ciat_logfl=1; }
if(current==0xe7 || current==0xf1) { extern int ciat_logfl; ciat_logfl=0; }

#endif
    return current;
}

/* single-step a timer. update _must_ have been called before */
_CIAT_FUNC void ciat_single_step(ciat_t *state, CLOCK cclk) 
{
    ciat_state_t timer;

    timer = state->st[state->nst-1];

    if (!timer.single)
	return;


    if (timer.cnt) {
	timer.cnt --;
        timer.clk = cclk + 0;
        ciat_tostack(state, &timer);
    } else {
	/* FIXME: alarm handling */
	if (timer.oneshot) {
	    timer.single = 0;
	}
        timer.clk = cclk + 0;
	timer.cnt = timer.latch;
        ciat_tostack(state, &timer);
    }


}

_CIAT_FUNC void ciat_set_latchhi(ciat_t *state, CLOCK cclk, BYTE byte)
{
    int i;

    CIAT_LOGIN(("%s set_latchhi: cclk=%d, byte=%02x", state->name, cclk, byte));

    for (i=0; i<state->nst; i++) {
	if (state->st[i].clk <= cclk) {
	    state->st[i].latch = (state->st[i].latch & 0xff) | (byte << 8);
	    if ( (!state->st[i].runp2) && (!state->st[i].single)) {
	        state->st[i].cnt = (state->st[i].latch & 0xff) | (byte << 8);
	    }
	}
    }

    CIAT_LOGOUT((""));
}

_CIAT_FUNC void ciat_set_latchlo(ciat_t *state, CLOCK cclk, BYTE byte)
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
_CIAT_FUNC void ciat_set_ctrl(ciat_t *state, CLOCK cclk, BYTE byte) 
{
    ciat_state_t timer;
    WORD current = ciat_read_timer(state, cclk+1);

/*
    CIAT_LOGIN(("%s set_ctrl: cclk=%d, byte=%02x, CRA=%02x, CRB=%02x, int=%02x",
	state->name, cclk, byte, cia[CIA_CRA], cia[CIA_CRB], ciaint));
*/
    CIAT_LOGIN(("%s set_ctrl: cclk=%d, byte=%02x",
	state->name, cclk, byte));
 
    /* FIXME: */
    if ( (byte ^ state->st[0].oneshot) & 8) {
        timer = state->st[0];
        timer.oneshot = byte & 8;
	timer.clk = cclk + 1;
	ciat_tostack(state, &timer);
    }

#ifdef CIAT_NEED_LOG
if(byte & 0x20) 
/*    ciat_log(" %s set_ctrl: cclk=%d, byte=%02x, CRA=%02x, CRB=%02x, int=%02x",
	state->name, cclk, byte, cia[CIA_CRA], cia[CIA_CRB], ciaint);*/
    ciat_log(" %s set_ctrl: cclk=%d, byte=%02x",
	state->name, cclk, byte);
#endif

    /* bit 0= start/stop, 4=force load, 5=0:count phi2 1:singlestep */
     switch (byte & 0x31) {
      case 0x00:
      case 0x20:
	if (state->st[0].runp2 || state->st[0].single) {
            current = ciat_read_timer(state, cclk+2);
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = current;
	    timer.runp2 = 0;
	    timer.single = 0;
	    timer.oneshot = byte & 8;
	    ciat_tostack(state, &timer);
	}
	break;
      case 0x01:
	if (!state->st[0].runp2) {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = current;
	    timer.runp2 = 1;
	    timer.single = 0;
	    timer.oneshot = byte & 8;
	    ciat_tostack(state, &timer);
	}
	break;
      case 0x21:
	if (!state->st[0].runp2) {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = current;
	    timer.runp2 = 0;
	    timer.single = 1;
	    timer.oneshot = byte & 8;
	    ciat_tostack(state, &timer);
	}
	break;
      case 0x10:
      case 0x30:
	timer = state->st[0];
	timer.clk = cclk + 2;
	timer.cnt = timer.latch;
	timer.runp2 = 0;
	timer.single = 0;
	timer.oneshot = byte & 8;
	ciat_tostack(state, &timer);
	break;	
      case 0x11:
	timer = state->st[0];
	timer.clk = cclk + 2;
	timer.cnt = timer.latch;
	timer.runp2 = 0;
	timer.single = 0;
	timer.oneshot = byte & 8;
	ciat_tostack(state, &timer);
	timer.runp2 = 1;
	timer.clk ++;
	ciat_tostack(state, &timer);
	break;
      case 0x31:
	timer = state->st[0];
	timer.clk = cclk + 2;
	timer.cnt = timer.latch;
	timer.runp2 = 0;
	timer.single = 1;
	timer.oneshot = byte & 8;
	ciat_tostack(state, &timer);
	timer.clk ++;
	ciat_tostack(state, &timer);
	break;
    }

    ciat_set_alarm(state);

    CIAT_LOGOUT((""));
}

_CIAT_FUNC void ciat_ack_alarm(ciat_t *state, CLOCK cclk)
{
    CIAT_LOGIN(("%s ack_alarm: cclk=%d, alarmclk=%d", 
					state->name, cclk, state->alarmclk));
    
    if(cclk >= state->alarmclk) {
	state->alarmclk = CLOCK_MAX;
	alarm_unset(state->alarm);

	if(cclk == state->st[0].nextz) {
            ciat_state_t timer;
            WORD current = ciat_read_timer(state, cclk+1);
/*

printf("%s: ack_alarm hit @ cclk=%d!\n", state->name, cclk);
	    timer = state->st[0];
	    timer.clk = cclk;
	    timer.cnt = current;
	    ciat_tostack(state, &timer);
*/
	    if (state->st[0].oneshot) {
                timer = state->st[0];
                timer.clk = cclk + 1;
                timer.cnt = current;
                timer.runp2 = 0;
                timer.single = 0;
                ciat_tostack(state, &timer);
	    }
	    /* FIXME: hack? */
            state->st[0].nextz += state->st[0].latch + 1;
	}
	/* FIXME: what happens when the else clause here happens...????? */


	if (state->st[0].oneshot) {
            ciat_state_t timer;
            WORD current = ciat_read_timer(state, cclk+1);

            timer = state->st[0];
            timer.clk = cclk + 2;
            timer.cnt = current;
            timer.runp2 = 0;
            timer.single = 0;
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

#else 	/* defined INLINE_CIAT_FUNCS || defined _CIATIMER_C */

/* We don't want inline definitions: just provide the prototypes.  */

extern void ciat_ack_alarm(ciat_t *state, CLOCK cclk);
extern void ciat_set_ctrl(ciat_t *state, CLOCK cclk, BYTE byte);
extern void ciat_set_latchlo(ciat_t *state, CLOCK cclk, BYTE byte);
extern void ciat_set_latchhi(ciat_t *state, CLOCK cclk, BYTE byte);
extern void ciat_single_step(ciat_t *state, CLOCK cclk);
extern WORD ciat_read_timer(ciat_t *state, CLOCK cclk);
extern WORD ciat_read_latch(ciat_t *state, CLOCK cclk);
extern void ciat_reset(ciat_t *state, CLOCK cclk);
extern void ciat_prevent_clock_overflow(ciat_t *state, CLOCK sub);
extern int ciat_update(ciat_t *state, CLOCK cclk, int *now);
extern void ciat_tostack(ciat_t *state, ciat_state_t *timer);
extern CLOCK ciat_alarm_clk(ciat_t *state);
extern void ciat_set_alarm(ciat_t *state);
extern void ciat_init(ciat_t *state, const char *name,
                                                CLOCK cclk, alarm_t *alarm);

#endif 	/* defined INLINE_CIAT_FUNCS || defined _CIATIMER_C */

#endif  /* _CIATIMER_H */

