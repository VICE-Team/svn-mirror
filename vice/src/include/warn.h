/*
 * warn.h - General-purpose warning message handler.
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

#ifndef _WARN_H
#define _WARN_H

typedef struct warn_s warn_t;

/*
 * Init warning -pointer. nrwarnings is the maximum warnid used on warn().
 * name is the name of the module, eg "SID".
 */
warn_t *warn_init(char *name, int nrwarnings);

/*
 * Warn every warning with warnid >= 0 only once.
 * Warn other warnings always.
 */
void warn(warn_t *pwarn, int warnid, char *msg, ...);

/*
 * This resets warning-information for one module or all modules
 * (NULL-parameter). After reset warning with warnid >= 0 will be
 * displayed again.
 */
void warn_reset(warn_t *pwarn);

#endif /* _WARN_H */
