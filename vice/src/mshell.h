/*
 * $Id: mshell.h,v 1.4 1997/05/22 21:28:02 ettore Exp $
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice
 *
 * Defaults for the Monitor shell.
 *
 *
 * Written by
 *   Jarkko Sonninen (sonninen@lut.fi)
 *   Jouko Valta     (jopi@zombie.oulu.fi)
 *
 *
 * $Log: mshell.h,v $
 * Revision 1.4  1997/05/22 21:28:02  ettore
 * *** empty log message ***
 *
 * Revision 1.3  1996/04/01  09:01:41  jopi
 * MODE declarations and struct for variable manipulation
 *
 * Revision 1.2  1995/04/01  07:54:09  jopi
 * X64 0.3 PL 0
 * Prototypes.
 *
 * Revision 1.1  1994/12/12  16:59:44  jopi
 * Initial revision
 *
 *
 *
 */

#ifndef X64_MSHELL_H
#define X64_MSHELL_H


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


#endif  /* X64_MSHELL_H */
