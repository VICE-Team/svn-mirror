/*
 * $Id: mshell.c,v 1.3.1.1 1997/05/22 20:16:34 ettore Exp $
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice
 *
 * This file contains simple user interface for the ML monitor in x64.
 * Included are:
 *	o Input line
 *	o Split line
 *	o Evaluate numeric values
 *	o Evaluate command
 *
 *
 * Written by
 *   Jarkko Sonninen (sonninen@lut.fi)
 *   Jouko Valta     (jopi@stekt.oulu.fi)
 *
 *
 * $Log: mshell.c,v $
 * Revision 1.3.1.1  1997/05/22 20:16:34  ettore
 * #include "vice.h" instead of the old "config.h".
 *
 * Revision 1.3  1996/04/01  09:01:41  jopi
 * *** empty log message ***
 *
 * Revision 1.2  1995/04/01  07:54:09  jopi
 * X64 0.3 PL 0
 * In case of argument count mismatch, now shows all possible commands.
 *
 * Revision 1.1  1994/12/12  16:59:44  jopi
 * Initial revision
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#include "vice.h"
#include "types.h"
#include "mshell.h"


/* extern */

/*static void pack_args( int start );*/
#ifdef EDIT
extern void add_history ( char *str );
extern char *readline ( char *prompt );
#endif


/*
 * Terminal Interface
 */

char   *read_line(char *prompt, int mode)
{
    char   *linep, *r_linep;
#ifndef EDIT
    static char line[256];

	fputs(prompt, stdout);
	fflush(stdout);
	r_linep = linep = fgets(line, 255, stdin);
#else
	r_linep = linep = readline(prompt);
	if (linep && *linep)
	    add_history(linep);
#endif

    while (r_linep && *r_linep && isspace(*r_linep))
	r_linep++;

    return (r_linep);
}


int     split_args(char *line, int mode, int maxarg, int maxval, char **args, int  *values, int  *types)
{
    int     len, i, count;
    int     quote = 0;
    char   *p, *r;

    if (!line || !*line)
	return (0);

    len = strlen(line);

    /* split command line to table and parse possible numeric values */
    quote = count = 0;
    for (i = 0; i < maxarg; i++) {
	values[i] = 0;
	types[i] = 0;
	args[i] = NULL;
    }

    p = r = line;
    do {
	if (*p == '\'' && quote != 1) {
	    if (!quote && p[1] && p[2] == '\'') {	/* Single byte */
		values[count] = p[1];
		types[count] = T_NUMBER;
		p += 2;
	    } else {
		quote ^= 2;
		types[count] = T_QUOTED;
	    }
	} else if (*p == '"' && quote != 2) {
	    quote ^= 1;
	    types[count] = T_QUOTED;

	} else if (!*p || (!quote && strchr(" \t\n,", *p))) {
	    *p = '\0';

	    /* r points to start of current word */
	    if (*r) {
		if (*r == '\'' || *r == '"') {
		    if (*(p-1) == *r)
			*(p-1) = 0;
		    r++;
		}

		args[count] = r;
		if (types[count] == T_QUOTED) {
		    values[count] = strlen(r);
		} else {
		    int cb = 0;
		    if (*r == '+') {
			cb++; r++;
		    }
		    if (count && sconv(r, 0, mode | MODE_QUERY)) {
			values[count] = sconv(r, 0, mode);
			if (cb) values[count] += values[count-1];
			if (values[count] > maxval+1) {
			    values[count] &= maxval;
			    printf("Value too large\n");
			}
			types[count] = T_NUMBER;
			/*  formats: "m 400 4ff", "m 400,4ff"
			 * ("m 400-4ff"), ("m 400+ff"), "m 400 +ff"
			 */
		    } else {
			values[count] = strlen(r);
			types[count] = T_OTHER;
		    }
		} /* if (types */

		count++;
	    }		/* if (*r) */
	    r = p+1;	/* skips leading whitespace */
	}		/* else discard character */

    } while (++p && len-- && count < maxarg);

#if 0
    for (i = 0; i < maxarg; i++) {
	printf ("arg %2d = '%s' \ttype %01s  value %d\n",
		i, args[i] ? args[i] :"", (types[i])+"-QNS", values[i]);
    }
#endif

    return (count);
}


/*
 * Execute Command
 * Find the nearest match for given command and execute it. Finally,
 * return the result or error.
 */

int    eval_command(char *command, int nargs, struct ms_table *cmds)
{
    int   i, j, b = 0, bi = 0;
    char *p=NULL, *q;


    if (!nargs || !command || !*command)
	return (0);

    /* now search through command table and find nearest command */
    b = 0;
    for (i = 0; cmds[i].command; i++) {
	for (p = cmds[i].command, q = command, j = 0;
	     *p && *q && *p == *q; p++, q++, j++);
	/*
	 * printf ("compare %s %s - %d %d %d\n", cmds[i].command,
	 * command, j, b, bi);
	 */
	if (j > b && !*q) {
	    b = j;
	    bi = i;

	    /* found an exact command */
	    if (*p == *q)
		break;
	}
    } /* for */

    /* command should have been identified by now */
    if (b) {
	if (nargs - 1 > cmds[bi].max_args ||
	    nargs - 1 < cmds[bi].min_args) {
	    printf("\nWrong number of arguments.\n");

	    if (!*p)  /* exact */
		printf("%s\n", cmds[bi].help_line);
	    else {
		j = strlen(command);
		/*printf("Precedence is as follows:\n\n");*/

		for (i = 0; cmds[i].command; i++) {
		    if (!strncmp(cmds[i].command, command, j))
			printf(" %s", cmds[i].help_line);
		}
	    }
	    printf("\n");
	    return (0);
	}

	return ( (*cmds[bi].funcp)() );
    }

    return (-1); /* Command not recognized */
}


/*
 *  Numeric evaluation with error checking
 *
 *   char   *s;		pointer to input string
 *   int     level;	recursion level
 *   int     mode;	flag: dec/hex mode
 */

int     sconv(char *s, int level, int mode)
{
    static char hexas[] = "0123456789abcdefg";
    char   *p = s;
    int     base = 0;
    int     result = 0, sign = 1;
    int     i = 0;

    if (!p)
	return (0);

    switch (tolower(*p)) {
      case '\'':

	if (*(++p) == '\\' && p[2] == '\'') {	/* Single Escaped byte */
	    ++p;
	    *p = *p & 0x1f;
	}

	if (p[1] != '\'') {			/* Single ASCII byte */
		if (!(mode & MODE_QUERY))
		    printf ("Bad character near '\n");
		return (0);
	    }
	return ((mode & MODE_QUERY) ? 1 : *p);

      case '%':
	p++;
	base = 2;
	break;

      case 'o':
      case '&':
	p++;
	base = 8;
	break;

      case 'x':
      case '$':
	p++;
	base = 16;
	break;

      case 'u':
      case 'i':
      case '#':
	p++;
	base = 10;
	break;

      case '0':	/* 0x 0b 0d */
	if (!*++p) return ((mode & MODE_QUERY) ? 1 : 0);
	if (!isdigit(*p))
	    return (sconv(p, level+1, mode));

      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
	base = (mode & MODE_HEX) ? 16 : 10;
	break;

      case 'a':
      case 'c':
      case 'e':
      case 'f':
	if (mode & MODE_HEX)
	    base = 16;
	break;

      case 'b':	/* hex or binary */
	if (mode & MODE_HEX)
	    base = 16;
	else {
	    base = 2;
	    p++;
	}
	break;

      case 'd':	/* hex or decimal */
	if (mode & MODE_HEX)
	    base = 16;
	else {
	    base = 10;
	    p++;
	}
	break;

      default:
	break;
    }

    /*
     * now p points to start of string to convert and base hold its base
     * number 2, 8, 10 or 16
     */

    if (!base)
	return (0);

    if (*p == '-') {
	sign = -1;
	p++;
    }
    while (tolower(*p)) {
	for (i = 0; i < base; i++)
	    if (tolower(*p) == hexas[i]) {
		result = result * base + i;
		break;
	    }
	if (i >= base) {
	    /* unknown char has occurred, return value or error */
	  if (strchr(",-+()", *p) || isspace(*p))
	      i = 0;
	  else if (!level && !(mode & MODE_QUERY))
	    printf ("Bad character near '%s'\n", p);

	  break;
	}
	p++;
    }
    /* printf ("mode %02X  last %d base %d value %d\n",
       mode, i, base, result); */

    /* return final value */
    return ((mode & MODE_QUERY) ? (i < base) : result * sign);
}
