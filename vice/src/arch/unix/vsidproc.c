/*
 * vsidproc.c - UI support for vsid 
 *
 * Written by
 *  Dag Lem
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

#include "vice.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "psid.h"
#include "resources.h"
#include "ui.h"
#include "vsync.h"

/* ------------------------------------------------------------------------- */
/* Process for the user interface */
static pid_t ui_pid = 0;
static int ui_pipefd[2];
static log_t vsid_log = LOG_ERR;

void psid_dispatch_events(void)
{
    static char buf[1000];
    static int pause = 0;

    /* Only the main process should do any work here. */
    if (ui_pid == 0) {
      return;
    }

 read_msg:
    /* Read any pending message from the UI process. */
    if (!ui_proc_read_msg(buf, sizeof(buf), pause)) {
      return;
    }

    if (strcmp(buf, "exit") == 0) {
      /* ui_proc_wait() will be called from the exit code to kill
	 and wait for the UI process */
      exit(0);
    }
    else if (strncmp(buf, "load", 4) == 0) {
      char* filename = buf + 5;
      if (machine_autodetect_psid(filename) < 0) {
	log_error(vsid_log, "`%s' is not a valid PSID file.", filename);
	return;
      }
      machine_play_psid(0);
      suspend_speed_eval();
      maincpu_trigger_reset();
    }
    else if (strncmp(buf, "pause", 5) == 0) {
      pause = atoi(buf + 6);
      suspend_speed_eval();
    }
    else if (strcmp(buf, "powerup") == 0) {
      suspend_speed_eval();
      machine_powerup();
    }
    else if (strcmp(buf, "reset") == 0) {
      suspend_speed_eval();
      maincpu_trigger_reset();
    }
    else if (strncmp(buf, "resource", 8) == 0) {
      char resource_name[100];
      int resource_value;
      sscanf(buf + 9, "%s %d", resource_name, &resource_value);
      resources_set_value(resource_name, (resource_value_t)resource_value);
    }
    else if (strncmp(buf, "tune", 4) == 0) {
      int tune = atoi(buf + 5);
      machine_play_psid(tune);
      suspend_speed_eval();
      maincpu_trigger_reset();
    }
    else {
      log_error(vsid_log, "Unrecognized command from UI process: %s", buf);
    }

    if (pause) {
      goto read_msg;
    }
}

void ui_proc_write_msg(char *msg)
{
  write(ui_pipefd[1], msg, strlen(msg));
}

int ui_proc_read_msg(char* msg, size_t size, int block)
{
  int num, total = 0;
  char* ptr;

  fd_set readfds;
  struct timeval timeout;
  struct timeval* timeout_ptr = block ? NULL : &timeout;

  FD_ZERO(&readfds);
  FD_SET(ui_pipefd[0], &readfds);

  /* Don't block. */
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  num = select(ui_pipefd[0] + 1, &readfds, NULL, NULL, timeout_ptr);

  if (num == -1) {
    log_error(vsid_log, "select(): %s", strerror(errno));
    strcpy(msg, "exit");
    return strlen(msg);
  }

  if (!num) {
    return 0;
  }

  ptr = msg;
  do {
    num = read(ui_pipefd[0], ptr, size);
    if (num == -1) {
      log_error(vsid_log, "read(): %s", strerror(errno));
      strcpy(msg, "exit");
      return strlen(msg);
    }
    ptr += num;
    total += num;
    size -= num;
  } while (ptr[-1] != '\n');

  ptr[-1] = '\0';

  return total;
}

void ui_proc_exit(void)
{
  /* The UI process will be killed by the main process;
     there is no need to call exit() here. */
  ui_proc_write_msg("exit\n");
}

int ui_proc_create(void)
{
  XSync(display, False);

  if (pipe(ui_pipefd) < 0) {
    log_error(vsid_log, "Cannot create pipe: %s", strerror(errno));
    return -1;
  }

  if ((ui_pid = fork()) < 0) {
    log_error(vsid_log, "Cannot create UI process: %s", strerror(errno));
    return -1;
  }

  if (ui_pid == 0) {
    /* Child process. */
    close(ui_pipefd[0]);
    ui_proc_start();
  }
  else {
    /* Parent process. */
    close(ui_pipefd[1]);
  }
  
  return 0;
}

int ui_proc_wait(void)
{
  int status;

  /* The UI process has no business here. */
  if (ui_pid == 0) {
    return 0;
  }

  if (kill(ui_pid, 0) != ESRCH) {
    if (kill(ui_pid, SIGTERM) < 0) {
      log_error(vsid_log, "Cannot kill UI process: %s", strerror(errno));
      return -1;
    }
  }

  if (wait(&status) != ui_pid) {
    log_error(vsid_log, "wait for UI process failed: %s", strerror(errno));
    return -1;
  }
  
  return 0;
}


