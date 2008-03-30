
/*
 * coproc.c - co-process fork
 *
 * Written by
 *  Andre Fachat     (a.fachat@physik.tu-chemnitz.de)
 *
 * Patches by
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
/*
 * This is modelled after some examples in Stevens, "Advanced Progamming
 * in the Unix environment", Addison Wesley.
 *
 * It simply opens two uni-directional pipes and forks a process to
 * use the pipes as bidirectional connection for the stdin/out of the
 * child.
 * This, however, implies that the child knows its being piped and _buffers_
 * all stdio. To avoid that one has to open a pseudo terminal device,
 * which is too heavily system dependant to be included here.
 * Instead a wrapper like the program "pty" described in the book mentioned
 * above could be used.
 *
 * Technicalities: It does not store the PID of the forked child but
 * instead it relies on the child being killed when the parent terminates
 * prematurely or the child terminates itself on EOF on stdin.
 *
 * The command string is given to "/bin/sh -c cmdstring" such that
 * the shell can do fileexpansion.
 *
 * We ignore all SIGCHLD and SIGPIPE signals that may occur here by
 * installing an ignoring handler.
 */

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "coproc.h"

#define	SHELL	"/bin/sh"

static struct sigaction sigignore = {
	SIG_IGN, 0, SA_NOCLDSTOP | SA_RESTART, NULL
};

int fork_coproc(int *fd_wr, int *fd_rd, char *cmd) {
	int fd1[2], fd2[2];
	pid_t pid;

	sigaction(SIGCHLD, &sigignore, NULL);
	sigaction(SIGPIPE, &sigignore, NULL);

	if(pipe(fd1) < 0) {
	  fprintf(stderr,"Coproc: Couldn't open pipe!\n");
	  return -1;
	}
	if(pipe(fd2) < 0) {
	  fprintf(stderr,"Coproc: Couldn't open pipe!\n");
	  close(fd1[0]);
	  close(fd1[1]);
	  return -1;
	}
	if( (pid = fork()) < 0) {
	  fprintf(stderr,"Coproc: Couldn't fork()!\n");
	  close(fd1[0]);
	  close(fd1[1]);
	  close(fd2[0]);
	  close(fd2[1]);
	  return -1;
	} else
	if(pid==0) {		/* child */
	  close(fd1[0]);
	  if(fd1[1] != STDOUT_FILENO) {
	    dup2(fd1[1], STDOUT_FILENO);
	    close(fd1[1]);
	  }
	  close(fd2[1]);
	  if(fd2[0] != STDIN_FILENO) {
	    dup2(fd2[0], STDIN_FILENO);
	    close(fd2[0]);
	  }
	  /* Hm, we have to close all other files that are currently
	   * open now... */
	  execl(SHELL, "sh", "-c", cmd, NULL);

	  exit(127);	/* child dies on error */

	} else {		/* parent */
	  close(fd1[1]);
	  close(fd2[0]);

	  *fd_rd = fd1[0];
	  *fd_wr = fd2[1];
	}
	return 0;
}


