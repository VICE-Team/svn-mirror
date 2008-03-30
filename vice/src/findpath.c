/*
 * findpath.c - Find a file via search path.
 *
 * Written by
 *  Tomi Ollila <Tomi.Ollila@tfi.net>
 *
 * Minor changes for VICE by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef __riscos
#include "vice.h"

#ifdef STDC_HEADERS
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#include "findpath.h"
#include "utils.h"
#include "archdep.h"


/*
 * This function is checked to be robust with all path 3 types possible
 * (cmd has relative, absolute or no path component)
 * The returned path will always contain at least one '/'. (if not NULL).
 * Overflow testing for internal buffer is always done.
 */

char * findpath(const char *cmd, const char *syspath, int mode)
{
    char * pd = NULL;
    char *c;

    PATH_VAR(buf);

    buf[0] = '\0'; /* this will (and needs to) stay '\0' */

    if (strchr(cmd, FSDEV_DIR_SEP_CHR /*'/'*/)) /* absolute or relative path given ???*/
    {
	int l, state;
	const char *ps;

	if (archdep_path_is_relative(cmd))
	{
	    if (getcwd(buf + 1, sizeof buf - 128) == NULL)
		goto fail;

	    l = strlen(buf + 1);
	}
	else l = 0;

	if (l + strlen(cmd) >= sizeof buf - 5)
	    goto fail;

	ps = cmd;
	pd = buf + l; /* buf + 1 + l - 1 */

#if !defined (__MSDOS__) && !defined (WIN32) && !defined (OS2)
	if (*pd++ != '/')
	    *pd++ = '/';
#else
	pd++;
#endif

	state = 1;

	/* delete extra `/./', '/../' and '//':s from the path */
	while (*ps)
	{
	    switch (state)
	    {
	    case 0: if (*ps == '/') state = 1; else state = 0; break;
	    case 1:
		if (*ps == '.') { state = 2; break; }
		if (*ps == '/') pd--; else state = 0; break;
	    case 2:
		if (*ps == '/') { state = 1; pd -= 2; break; }
		if (*ps == '.') state = 3; else state = 0; break;
	    case 3:
		if (*ps != '/') { state = 0; break; }
		state = 1;
		pd -= 4;
		while (*pd != '/' && *pd != '\0')
		    pd--;
		if (*pd == '\0') pd++;
		state = 1;
		break;
	    }
	    *pd++ = *ps++;

	}

        *pd = '\0';
	pd = buf + 1;
    }
    else
    {
	const char * path = syspath;
	const char * s;
	int cl = strlen(cmd) + 1;

	for (s = path; s; path = s + 1)
	{
	    char * p;
	    int l;

	    s = strchr(path, FINDPATH_SEPARATOR_CHAR);
	    l = s? (s - path): strlen(path);

	    if (l + cl > sizeof buf - 5)
		continue;

	    memcpy(buf + 1, path, l);

	    p = buf + l;  /* buf + 1 + l - 1 */

	    if (*p++ != '/')
		*p++ = '/';

	    memcpy(p, cmd, cl);

	    for(c= buf + 1; *c !='\0'; c++)
#if defined (__MSDOS__) || defined (WIN32) || defined (OS2)
	        if(*c=='/') *c='\\';
#else
	        if(*c=='\\') *c='/';
#endif
	    if (access(buf + 1, mode) == 0)
	    {
		pd = p /* + cl*/ ;
		break;
	    }
	}
    }


    if (pd)
    {
#if 0
        do pd--;
	while (*pd != '/'); /* there is at least one '/' */

	if (*(pd - 1) == '\0')
	    pd++;
	*pd = '\0';
#endif

	return stralloc(buf + 1);
    }
 fail:
    return NULL;
}


#ifdef _TEST_FINDPATH

void tstpath(char * string)
{
    char * s;
    printf("*** Testing %s: ", string);

    s = findpath(string, getenv("PATH"));

    if (s) { puts(s); free(s); }
    else puts("path not found");
}

void main(int argc, char ** argv)
{

    if (argc > 1)
    {
	tstpath(argv[1]);
    }
    else
    {
	tstpath("/foo/bar");
	tstpath("../bar");
	tstpath("foo/bar");
	tstpath(".////foo/bar");
	tstpath("../../../../../");
	tstpath("../../../..//./foo/../bar");
	tstpath("bar");
	tstpath("gzip");
	tstpath("perl");
    }
    exit(0);
}
#endif
#endif /* __riscos */
