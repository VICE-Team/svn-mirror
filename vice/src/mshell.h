/*
 * mshell.c - Simple command-line handling.
 *
 * Written by
 *  Jarkko Sonninen (sonninen@lut.fi)
 *  Jouko Valta     (jopi@stekt.oulu.fi)
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

#ifndef _MSHELL_H
#define _MSHELL_H

#define T_QUOTED 1
#define T_NUMBER 2
#define T_OTHER  3

 /*
  * Define 'mode'
  */

#define MODE_HEX	 1
#define MODE_SYMBOL	 2
#define MODE_QUOTE	 4
#define MODE_QUERY	(1 << 5)
#define MODE_SPACE	(1 << 6)	/* space terminates evaluation (MON) */
#define MODE_QUIET	(1 << 7)
#define MODE_VERBOSE	(1 << 8)
#define MODE_INF	(1 << 9)
#define MODE_OUTF	(1 << 10)

#define MODE_BREAK	(1 << 11)	/* mon break */
#define MODE_MON	(1 << 12)
#define MODE_ASM	(1 << 13)
#define MODE_ZILOG	(1 << 14)

struct ms_table {
    char   *command;
    int     min_args;
    int     max_args;
    int    (*funcp) (void);
    char   *help_line;
};


/* values for ms_table->type */
#define	TOGT_NONE	0
#define	TOGT_BOOL	1
#define	TOGT_INT	2

struct ms_vartab {
    int    type;
    char   *name;
    int    *variable;
    int     min_val;			/* bitmask for bit-field variables */
    int     max_val;
    void  (*set_func) (int);
    char   *help_line;
};

extern struct ms_vartab mon_vars[];

char   *read_line ( char *, int );
int     split_args ( char *, int mode, int, int, char **, int  *, int  * ) ;
int     eval_command ( char *, int , struct ms_table * );
int     sconv ( char *, int, int );


#endif  /* _MSHELL_H */
