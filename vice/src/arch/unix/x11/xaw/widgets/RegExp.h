/****************************************************************************

	RegExp.h

	This file contains the C definitions and declarations for
	the regular expression matching code.

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
 * POSIX regexp support added by Ettore Perazzoli (ettore@comm2000.it)
 * See ChangeLog for the list of changes.
 */

#ifndef _FWF_REGEXP_H_
#define	_FWF_REGEXP_H_

#include <stdio.h>

/* Workaround for {Free,Net}BSD.  Ettore Perazzoli <ettore@comm2000.it>
   03/19/98 */
#if defined __FreeBSD__ || defined __NetBSD__
#undef HAVE_REGEXP_H
#endif

#if defined HAVE_REGEX_H      /* POSIX */

/* POSIX <regex.h> version.  */
#include <regex.h>
typedef regex_t fwf_regex_t;

#elif defined HAVE_REGEXP_H

/* Insane <regexp.h> version.  */

typedef char *fwf_regex_t;

/* XXX: We cannot do it here, or we get multiple definitions of
   `compile()'.  */
/* #include <regexp.h> */

#else

typedef char fwf_regex_t;

#endif

void    RegExpInit(fwf_regex_t *r);
void    RegExpFree(fwf_regex_t *r);
void	RegExpCompile(const char *regexp, fwf_regex_t *r);
int	RegExpMatch(const char *string, fwf_regex_t *r);
void	_RegExpError(int val);
void	RegExpPatternToRegExp(const char *pattern, char *reg_exp);

#ifndef TRUE
#define TRUE				1
#endif

#ifndef FALSE
#define	FALSE				0
#endif

#endif
