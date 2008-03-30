/*
 * warn.c - General-purpose warning message handler.
 *
 * Written by
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "vice.h"
#include "warn.h"
#include "utils.h"

struct warn_s
{
    char		*name;
    struct warn_s	*pnext;
    int			 nrwarn;
    char		*pwarn;
};

static warn_t *warnlist = NULL;

warn_t *warn_init(const char *name, int nrwarnings)
{
    warn_t		*p;

    p = malloc(sizeof(*p));
    p->name = stralloc(name);
    p->pnext = warnlist;
    warnlist = p;
    p->nrwarn = nrwarnings;
    if (nrwarnings)
    {
	p->pwarn = malloc((p->nrwarn+7)/8);
	memset(p->pwarn, 0, (p->nrwarn+7)/8);
    }
    else
	p->pwarn = NULL;
    return p;
}

void warn(warn_t *pwarn, int warnid, char *msg, ...)
{
    char			*p;
    int				 m;
    va_list			 ap;

    if (warnid >= 0 && warnid < pwarn->nrwarn)
    {
	p = &pwarn->pwarn[warnid/8];
	m = 1 << (warnid % 8);
	if (*p & m)
	    return;
	*p |= m;
    }
    va_start(ap, msg);
    fprintf(stdout, "%s: warning: ", pwarn->name);
    vfprintf(stdout, msg, ap);
    fprintf(stdout, "\n");
    va_end(ap);
}

void warn_reset(warn_t *pwarn)
{
    warn_t		*p;
    if (pwarn)
    {
	if (pwarn->nrwarn)
	    memset(pwarn->pwarn, 0, (pwarn->nrwarn+7)/8);
	return;
    }
    for (p = warnlist; p; p = p->pnext)
	warn_reset(p);
}

void warn_free(warn_t *pwarn)
{
    free(pwarn->name);
    if (pwarn->pwarn)
	free(pwarn->pwarn);
    free(pwarn);
}
