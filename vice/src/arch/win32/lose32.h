/*
 * lose32.h - Some ugly portability cruft for M$ Windows.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _LOSE32_H
#define _LOSE32_H

/* These are needed by a few of the functions that we use.  To avoid messing
   with the sources, let's put them in here and that's it.  */
#include <direct.h>
#include <process.h>
#include <io.h>

/* The M$ version of this one has no permissions.  */
#define mkdir(s, p)         _mkdir(s)

/* This is used to query `struct stat'.  */
/*#define S_ISDIR(p)          (p & _S_IFDIR)*/

/* `strcasecmp()' is missing...  */
#define strcasecmp(s1, s2)      _stricmp(s1, s2)

/* Can anybody please explain me why M$ does not #define them and would like
   us to use the hardcoded values instead?...  !$@#$%  */

#define STDIN_FILENO        0
#define STDOUT_FILENO       1
#define STDERR_FILENO       2

#define W_OK                2
#define R_OK                4

/* This is necessary because on Windows the first function to be called is
   `WinMain()' and not `main()'.  */

int main_program(int argc, char **argv);

#define MAIN_PROGRAM        main_program

#endif
