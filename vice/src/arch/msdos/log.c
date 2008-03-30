/*
 * log.c - Simple routines for logging messages to a file on MS-DOS.
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

/* This is just a quick hack to avoid writing real loggin routines.  The real
   solution should be to provide standard logging routines that all the
   versions can use and use them instead of `printf()'/`fprintf()'.  */

#include "vice.h"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <unistd.h>

#include "log.h"

#include "utils.h"

/* ------------------------------------------------------------------------- */

static int log_fd;

void log_enable(int new)
{
    char *path = concat(boot_path, "/", "vice.log", NULL);

    printf("Writing log to `%s'\n", path);

    if (new)
	log_fd = open(path, O_TEXT | O_WRONLY | O_CREAT | O_TRUNC, 0666);
    else
	log_fd = open(path, O_TEXT | O_WRONLY | O_APPEND);
    if (log_fd == -1) {
	perror(path);
	exit(-1);
    }

    free(path);

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    dup2(log_fd, STDOUT_FILENO);
    dup2(log_fd, STDERR_FILENO);
    close(log_fd);
}

void log_disable(void)
{
}
