/****************************************************************************

	RegExp.c

	This file contains the C code for the regular expression
	matching code.

	The routines supported act as a more friendly, user level
	interface to the regexp regular expression matching system.

 ****************************************************************************/

/*
 * Author:
 * 	Brian Totty
 * 	Department of Computer Science
 * 	University Of Illinois at Urbana-Champaign
 *	1304 West Springfield Avenue
 * 	Urbana, IL 61801
 *
 * 	totty@cs.uiuc.edu
 *
 */

#include "autoconf.h"

#include "RegExp.h"

/* Workaround for {Free,Net}BSD.  Ettore Perazzoli <ettore@comm2000.it>
   03/19/98 */
#if defined __FreeBSD__ || defined __NetBSD__
#undef HAVE_REGEXP_H
#endif

#ifdef HAVE_REGEXP_H
#include <regexp.h>
#endif

void RegExpCompile(char *regexp, char *fsm_ptr, int fsm_length)
{
#if defined HAVE_REGEXP_H && !defined __FreeBSD__
	compile(regexp,fsm_ptr,&(fsm_ptr[fsm_length]),'\0');
#endif
} /* End RegExpCompile */


int RegExpMatch(string,fsm_ptr)
char *string,*fsm_ptr;
{
#ifdef HAVE_REGEXP_H
	if (advance(string,fsm_ptr) != 0)
		return(TRUE);
	    else
		return(FALSE);
#else
	return(TRUE);
#endif
} /* End RegExpMatch */


void _RegExpError(val)
int val;
{
	fprintf(stderr,"Regular Expression Error %d\n",val);
	exit(-1);
} /* End _RegExpError */


void RegExpPatternToRegExp(pattern,reg_exp)
char *pattern,*reg_exp;
{
	int in_bracket;

	in_bracket = 0;
	while (*pattern != '\0')
	{
		if (in_bracket)
		{
			if (*pattern == ']') in_bracket = 0;
			*reg_exp++ = *pattern++;
		}
		    else
		{
			switch (*pattern)
			{
			    case '[':
				in_bracket = 1;
				*reg_exp++ = '[';
				break;
			    case '?':
				*reg_exp++ = '.';
				break;
			    case '*':
				*reg_exp++ = '.';
				*reg_exp++ = '*';
				break;
			    case '.':
				*reg_exp++ = '\\';
				*reg_exp++ = '.';
				break;
			    default:
				*reg_exp++ = *pattern;
				break;
			}
			++ pattern;
		}
	}
	*reg_exp++ = '$';
	*reg_exp++ = '\0';
} /* End RegExpPatternToRegExp */
