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

#include "vice.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "types.h"
#include "archdep.h"
#include "lib.h"
#include "machine.h"
#include "utils.h"



/* File types */
static const int FileType_GZIP = 0xf89;

static FILE *defaultLogFile = NULL;
char *defaultLogName = NULL;


int archdep_init(int *argc, char **argv)
{
  return 0;
}


void archdep_closedown(void)
{
  if (defaultLogFile != NULL)
  {
    fclose(defaultLogFile);
    defaultLogFile = NULL;
    remove(defaultLogName);
    lib_free(defaultLogName);
    defaultLogName = NULL;
  }
}


char *archdep_program_name(void)
{
  char *name=NULL;

  if (machine_name != NULL)
  {
    if ((name = (char*)lib_malloc(strlen("Vice") + strlen(machine_name) + 1)) != NULL)
      sprintf(name, "Vice%s", machine_name);
  }

  return name;
}


FILE *archdep_open_default_log_file(void)
{
  const char *name = tmpnam(NULL);

  if ((defaultLogName = (char*)malloc(strlen(name)+1)) != NULL)
  {
    strcpy(defaultLogName, name);
    if ((defaultLogFile = fopen(defaultLogName, "w+")) != NULL)
    {
      return defaultLogFile;
    }
    else
    {
      lib_free(defaultLogName);
      defaultLogName = NULL;
    }
  }

  return fopen("null:", "w");
}


FILE *archdep_get_default_log_file(void)
{
  return defaultLogFile;
}


/* Return a malloc'ed backup file name for file `fname'.  */
char *archdep_make_backup_filename(const char *fname)
{
    return util_concat(fname, "~", NULL);
}

char *archdep_default_resource_file_name(void)
{
  char *name;
  const char *basename;

  if (vsid_mode)
    basename = "VSID";
  else
    basename = (machine_name == NULL) ? "DRIVES" : machine_name;

  if ((name = (char*)lib_malloc(strlen("Vice:.vicerc") + strlen(basename) + 1)) != NULL)
     sprintf(name, "Vice:%s.vicerc", basename);

  return name;
}


char *archdep_default_fliplist_file_name(void)
{
    return NULL;
}


char *archdep_default_save_resource_file_name(void)
{
  return archdep_default_resource_file_name();
}


char *archdep_default_sysfile_pathlist(const char *emu_id)
{
  return lib_msprintf("Vice:%s.", emu_id);
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

  retbuf = (char*)lib_malloc(strlen(readbuffer) + 1);
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

int archdep_default_logger(const char *level_string, const char *txt)
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

/* return malloc´d version of full pathname of orig_name */
int archdep_expand_path(char **return_path, const char *orig_name)
{
    /* Always treat it as the full pathname... */
    *return_path = (char*)lib_malloc(strlen(orig_name) + 1);
    strcpy(*return_path, orig_name);
    return 0;
}

void archdep_startup_log_error(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vfprintf(stderr, format, ap);
}

char *archdep_filename_parameter(const char *name)
{
    /* nothing special(?) */
    return lib_stralloc(name);
}

char *archdep_quote_parameter(const char *name)
{
    /*not needed(?) */
    return lib_stralloc(name);
}

char *archdep_tmpnam(void)
{
    return lib_stralloc(tmpnam(NULL));
}


const char *archdep_extract_dir_and_leaf(const char *path)
{
  const char *dir, *leaf, *b;

  b = path; leaf = b; dir = b;
  while (*b != '\0')
  {
    if ((*b == '.') || (*b == ':'))
    {
      dir = leaf; leaf = b + 1;
    }
    b++;
  }
  return dir;
}

int archdep_file_is_gzip(const char *name)
{
    int type;

    type = GetFileType(name);
    /* only look at filename if the filetype differs from 0xF89 */
    if (type != FileType_GZIP)
    {
        size_t l = strlen(name);

        if ((l < 4 || strcasecmp(name + l - 3, "/gz"))
            && (l < 3 || strcasecmp(name + l - 2, "/z"))
            && (l < 4 || toupper(name[l - 1]) != 'Z' || name[l - 4] != '/'))
            return 0;
    }
    return 1;
}

int archdep_file_set_gzip(const char *name)
{
  if (SetFileType(name, FileType_GZIP) != NULL)
    return -1;

  return 0;
}

int archdep_mkdir(const char *pathname, int mode)
{
    return mkdir(pathname, (mode_t)mode);
}

int archdep_file_is_blockdev(const char *name)
{
    return 0;
}

int archdep_file_is_chardev(const char *name)
{
    return 0;
}

void archdep_shutdown(void)
{

}

