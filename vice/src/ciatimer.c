
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

#undef	CIAT_TIMER_DEBUG

#define	INLINE inline


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
}

/* check when the next underflow will occur and set the alarm */
/* needs update before */
static void INLINE ciat_set_alarm(ciat_t *state)
{
    CLOCK tmp = CLOCK_MAX;
    int i;
#ifdef CIAT_TIMER_DEBUG
    printf("%s: set_alarm, nst=%d\n", state->name, state->nst);
#endif
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
#ifdef CIAT_TIMER_DEBUG
    printf(" -> alarm=%d\n", tmp);
#endif
    if (tmp != CLOCK_MAX) {
        state->alarmclk = tmp;
	alarm_set(state->alarm, tmp);
    } else {
        state->alarmclk = tmp;
	alarm_unset(state->alarm);
    }
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

#ifdef CIAT_TIMER_DEBUG
    printf("ciat_tostack(%s, t->clk=%d, st0->clk=%d\n", 
			state->name, timer->clk, state->st[0].clk); 
    printf("             running=%d, t->cnt=%d, -> t->nextz=%d\n", 
			timer->running, timer->cnt, timer->nextz); 
    printf("             alarmclk=%d\n", 
			state->alarmclk); 
#endif

    for (i=0;i<state->nst;i++) {
	if (state->st[i].clk >= timer->clk) {
	    state->nst = i;
	    break;
	}
    }
    state->st[i] = *timer;
    state->nst ++;
}

/* update timer description by checking timer stack for more up-to-date
 * descriptions. After that makes nextz > cclk. 
 * Returns 0 when no underflow has happened, and >0 if so */
/* FIXME: what when alarm is set? */
static int INLINE ciat_update(ciat_t *state, CLOCK cclk) 
{
    int i,j, n;

    n = 0;
#ifdef CIAT_TIMER_DEBUG
    printf("%s: update: cclk=%d, nst=%d\n", state->name, cclk, state->nst);
#endif
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
#ifdef CIAT_TIMER_DEBUG
    printf("%s: cclk=%d, nextz=%d, latch=%d, clk=%d\n", state->name, cclk, state->st[0].nextz, state->st[0].latch, state->st[0].clk);
#endif
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
#ifdef CIAT_TIMER_DEBUG
        printf("tmp=%d, ->clk=%d, nextz=%d\n",tmp, state->st[0].clk, state->st[0].nextz);
#endif
	}
    } else {
	state->st[0].clk = cclk;
    }

    state->last_update = cclk;
    return n;
}

/* handle clock overflow */
static void INLINE ciat_prevent_clock_overflow(ciat_t *state, CLOCK sub)
{
    int i;
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
}

/*
 * Timer operations
 */
 
/* timer reset */
static void INLINE ciat_reset(ciat_t *state, CLOCK cclk)
{
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
    for (i=0; i<state->nst; i++) {
	if (state->st[i].clk <= cclk) {
	    state->st[i].latch = (state->st[i].latch & 0xff) | (byte << 8);
	    if (! state->st[i].running) {
	        state->st[i].cnt = (state->st[i].latch & 0xff) | (byte << 8);
	    }
	}
    }
}

static void INLINE ciat_set_latchlo(ciat_t *state, CLOCK cclk, BYTE byte)
{
    int i;
    for (i=0; i<state->nst; i++) {
	if (state->st[i].clk <= cclk) {
	    state->st[i].latch = (state->st[i].latch & 0xff00) | byte;
	    if (! state->st[i].running) {
	        state->st[i].cnt = (state->st[i].latch & 0xff00) | byte;
	    }
	}
    }
}


/* needs update before */
static void INLINE ciat_set_ctrl(ciat_t *state, CLOCK cclk, BYTE byte) 
{
    ciat_state_t timer;
    WORD current = ciat_read_timer(state, cclk+1);

    /* FIXME: */
    state->st[0].oneshot = byte & 8;

    switch (byte & 0x11) {
      case 0x00:
	if (state->st[0].running) {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = current;
	    timer.running = 0;
	    ciat_tostack(state, &timer);
	}
	break;
      case 0x01:
	if (!state->st[0].running) {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = current;
	    timer.running = 1;
	    ciat_tostack(state, &timer);
	}
	break;
      case 0x10:
	if (state->st[0].running) {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = timer.latch;
	    timer.running = 0;
	    ciat_tostack(state, &timer);
	}
/* FIXME: else */
	break;	
      case 0x11:
	if (!state->st[0].running) {
	    timer = state->st[0];
	    timer.clk = cclk + 2;
	    timer.cnt = timer.latch;
	    timer.running = 1;
	    ciat_tostack(state, &timer);
	    timer.clk ++;
	    ciat_tostack(state, &timer);
	}
/* FIXME: else */
	break;
    }


    ciat_set_alarm(state);
}

static void INLINE ciat_ack_alarm(ciat_t *state, CLOCK cclk)
{
#ifdef CIAT_TIMER_DEBUG
printf("ack_alarm: cclk=%d, alarmclk=%d\n", cclk, state->alarmclk);
#endif
    if(cclk >= state->alarmclk) {
	state->alarmclk = CLOCK_MAX;
	alarm_unset(state->alarm);

	if(cclk == state->st[0].nextz) {
	    state->st[0].nextz += state->st[0].latch + 1;
	}

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
    }
}

