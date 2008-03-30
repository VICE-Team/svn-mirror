/****************************************************************************

	RegExp.c

	This file contains the C code for the regular expression
	matching code.

	The routines supported act as a more friendly, user level
	interface to the regexp regular expression matching system.

 ****************************************************************************/

/*
 * Author:
 *      Brian Totty
 *      Department of Computer Science
 *      University Of Illinois at Urbana-Champaign
 *      1304 West Springfield Avenue
 *      Urbana, IL 61801
 *
 *      totty@cs.uiuc.edu
 *
 * POSIX regexp support added by Ettore Perazzoli (ettore@comm2000.it)
 * See ChangeLog for the list of changes.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "RegExp.h"

#if defined HAVE_REGEX_H

/* POSIX <regex.h> version.  */

void RegExpInit(r)
    fwf_regex_t *r;
{
    return;
}

void RegExpFree(r)
    fwf_regex_t *r;
{
    regfree(r);
}

void RegExpCompile(regexp, r)
    const char *regexp;
    fwf_regex_t *r;
{
    regcomp(r, regexp, 0);
}

int RegExpMatch(string, r)
    const char *string;
    fwf_regex_t *r;
{
    return !regexec(r, string, 0, NULL, 0);
}

#elif defined HAVE_REGEXP_H

/* Insane <regexp.h> version.  */

#define	INIT		register char *sp = instring;
#define	GETC()		(*sp++)
#define	PEEKC()		(*sp)
#define	UNGETC(c)	-- sp
#define	RETURN(ptr)	return NULL;
#define	ERROR(val)	_RegExpError(val)

/* Forward decl required by <regexp.h>.  */
void _RegExpError(int val);

#include <regexp.h>

#define RE_SIZE 1024            /* Completely arbitrary, but who cares.  */

void RegExpInit(r)
    fwf_regex_t *r;
{
    *r = malloc(RE_SIZE);       /* FIXME: missing check!  */
    return;
}

void RegExpFree(r)
    fwf_regex_t *r;
{
    free(*r);
    return;
}

void RegExpCompile(regexp, r)
    const char *regexp;
    fwf_regex_t *r;
{
    char **s = (char **) r;

    /* Mmmh...  while cannot arg 1 of `compile' be const?  Compiler barfs on
       GNU libc 2.0.6.  */
    compile((char *) regexp, *s, *s + RE_SIZE - 1, '\0');
}				/* End RegExpCompile */


int RegExpMatch(string, fsm_ptr)
    const char *string;
    fwf_regex_t *fsm_ptr;
{
    /* Mmmh...  while cannot arg 1 of `compile' be const?  Compiler barfs on
       GNU libc 2.0.6.  */
    if (advance((char *) string, *fsm_ptr) != 0)
	return (TRUE);
    else
	return (FALSE);
}				/* End RegExpMatch */

void _RegExpError(val)
    int val;
{
    fprintf(stderr, "Regular Expression Error %d\n", val);
    exit(-1);
}				/* End _RegExpError */

#else

/* Dummy for system that don't have neither <regex.h> and <regexp.h>.  */

void RegExpInit(r)
    fwf_regex_t *r;
{
    return;
}

void RegExpFree(r)
    fwf_regex_t *r;
{
    return;
}

void RegExpCompile(regexp, r)
    const char *regexp;
    fwf_regex_t *r;
{
    return;
}

int RegExpMatch(string, r)
    const char *string;
    fwf_regex_t *r;
{
    return TRUE;                /* Always match.  */
}

#endif

/* ------------------------------------------------------------------------- */

void RegExpPatternToRegExp(pattern, reg_exp)
    const char *pattern;
    char *reg_exp;
{
    int in_bracket;

    in_bracket = 0;
    while (*pattern != '\0') {
	if (in_bracket) {
	    if (*pattern == ']')
		in_bracket = 0;
	    *reg_exp++ = *pattern++;
	} else {
	    switch (*pattern) {
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
	    ++pattern;
	}
    }
    *reg_exp++ = '$';
    *reg_exp++ = '\0';
}				/* End RegExpPatternToRegExp */
