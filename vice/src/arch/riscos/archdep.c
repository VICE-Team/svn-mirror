/*
 * archdep.c - Architecture dependent functions (non-UI)
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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
#include <string.h>

#include "config.h"
#include "types.h"
#include "ROlib.h"
#include "archdep.h"
#include "machine.h"
#include "utils.h"



int archdep_startup(int *argc, char **argv)
{
  return 0;
}


char *archdep_program_name(void)
{
  char *name=NULL;

  if (machine_name != NULL)
  {
    if ((name = (char*)malloc(strlen("Vice") + strlen(machine_name) + 1)) != NULL)
      sprintf(name, "Vice%s", machine_name);
  }

  return name;
}


FILE *archdep_open_default_log_file(void)
{
  /*char basename[64]="";

  if (machine_name != 0)
  {
    sprintf(basename, "Vice:%s.log", machine_name);
    return open_logfile(basename);
  }
  else
    return NULL;*/

  return fopen("null:", "w");
}


char *archdep_default_resource_file_name(void)
{
  char *name;
  char *basename;

  basename = (machine_name == NULL) ? "DRIVES" : (char*)machine_name;

  if ((name = (char*)malloc(strlen("Vice:.vicerc") + strlen(basename) + 1)) != NULL)
     sprintf(name, "Vice:%s.vicerc", basename);

  return name;
}


char *archdep_default_save_resource_file_name(void)
{
  return archdep_default_resource_file_name();
}


char *archdep_default_sysfile_pathlist(const char *emu_id)
{
  char *name;

  if ((name = (char*)malloc(strlen("Vice:") + strlen(emu_id) + 2)) != NULL)
    sprintf(name, "Vice:%s.", emu_id);

  return name;
}


int archdep_num_text_lines(void)
{
  return 0;
}

int archdep_num_text_columns(void)
{
  return 0;
}


int archdep_path_is_relative(const char *directory)
{
  const char *b;

  b = directory;
  while (*b != 0)
  {
    if ((*b == '$') || (*b == ':')) return 1;
    b++;
  }
  return 0;
}




#define READLINE_BUFFER		512

static char readbuffer[READLINE_BUFFER];

/* Readline emulation */
char *readline(const char *prompt)
{
  char *retbuf;
  int len;

  if (prompt != NULL) printf("%s", prompt);

  readbuffer[0] = '\0';
  len = OS_ReadLine(readbuffer, READLINE_BUFFER, 0, 255, 0);

  if ((len <= 0) || (readbuffer[0] < 32)) return NULL;
  readbuffer[len] = '\0';

  retbuf = (char*)malloc(strlen(readbuffer) + 1);
  strcpy(retbuf, readbuffer);

  return retbuf;
}


void add_history(const char *p)
{
}


/* Logfile handling */
FILE *open_logfile(const char *basename)
{
  FILE *fp;
  char buffer[256];
  int number;

  for (number=0; number<16; number++)
  {
    sprintf(buffer, "%s%d", basename, number);
    if ((fp = fopen(buffer, "w")) != NULL) return fp;
  }
  return NULL;
}

int archdep_default_logger(const char *level_string, const char *format, va_list ap)
{
    return 0;
}

void archdep_setup_signals(int do_core_dumps)
{
    /* What is a signal?  */
    return;
}

int archdep_spawn(const char *name, char **argv,
                  const char *stdout_redir, const char *stderr_redir)
{
    return 0;
}

