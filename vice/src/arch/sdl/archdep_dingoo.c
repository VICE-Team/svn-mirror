/*
 * archdep.c - Miscellaneous system-specific stuff.
 *
 * Written by
 *  peiselulli (peiselulli@t-online.de)
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

#include "../../video/render1x1.h"
#include "types.h"

#include <stdio.h>

#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif
#if defined(HAVE_DIRENT_H) || defined(AMIGA_AROS)
#include <dirent.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "archdep.h"
#include "ioutil.h"
#include "lib.h"
#include "types.h"
#include "util.h"

#include <stdlib.h>
#include <dingoo/slcd.h>
#include <dingoo/cache.h>
#include <dingoo/keyboard.h>
#include <jz4740/cpu.h>
#include <dingoo/fsys.h>
#include <dingoo/ucos2.h>
#include <dingoo/entry.h>
#include <dingoo/jz4740.h>

#ifdef DINGOO_DEBUG
#include "FontWhite.h"
#endif

#define CONTROL_BUTTON_SELECT 0x00000400
#define CONTROL_BUTTON_START 0x00000800

#include "archdep_SDL_dingoo_video.c"

static char _path[1024];
static char _app_path[1024];
static char *_program_name;

unsigned short *g_pGameDecodeBuf = 0L;

#ifdef DINGOO_DEBUG

void PutChar(int X, int Y, unsigned char Char) {
	unsigned short *Frame;
	int x, y;
	Frame = _lcd_get_frame();
	for(y=0; y<CHAR_HEIGHT; y++)
		for(x=0; x<CHAR_WIDTH; x++)
			Frame[(Y*CHAR_HEIGHT+y)*320 + (X*CHAR_WIDTH+x)] = CHARS[Char][y*CHAR_WIDTH+x];
}

void PutString(int X, int Y, unsigned char *Str) {
	while(*Str != 0) {
		PutChar(X, Y, *Str);
		X++;
		Str++;
		if(X > 39) {
			X = 0;
			Y++;
			if(Y > 13) Y = 0;
		}
	}
}


static char _buffer[256];

int trace(int line, char* file) {
	KEY_STATUS KS;
	char buffer[256];
	
	snprintf(buffer, sizeof(buffer), "%s <%d>", file, line);
	strcpy(_buffer, buffer);
	PutString(10, 5, buffer);
	__dcache_writeback_all();
	_lcd_set_frame();
	//do {
	//	
	//} while((KS.status & CONTROL_BUTTON_SELECT) == 0);
        _kbd_get_status(&KS);
        if(KS.status & CONTROL_BUTTON_START)
                exit(0);
        mdelay(500);
	return EXIT_SUCCESS;
}


void atexitfunc(void)
{
	KEY_STATUS KS;
	char buffer[256];

	if(strlen(_buffer) > 0)
	{
		PutString(10, 7, _buffer);
		__dcache_writeback_all();
		_lcd_set_frame();
		do {
			kbd_get_status(&KS);
		} while((KS.status & CONTROL_BUTTON_SELECT) == 0);
	}
}

#endif


static uintptr_t tempCore;
static uintptr_t tempLcd;
static uintptr_t tempLcdFactor;

char *getwd(char *buf)
{
	errno = 0;
	strcpy(buf, _path);
	return buf;
}

char *getcwd(char *buf, size_t len)
{
	return getwd(buf);
}

int archdep_init_extra(int *argc, char **argv)
{
	uintptr_t tempMemory;

	cpu_clock_get(&tempCore, &tempMemory);
        tempLcd = REG_CPM_LPCDR;
        tempLcdFactor = tempCore/tempLcd;
}

void set_overclock(int activate)
{
	uintptr_t tempCore2, tempMemory;
        if(activate)
        {
		if(is_lcd_active())
		{
                	cpu_clock_set(430000000);
                	REG_CPM_LPCDR = 11;
		}
		else
		{
                	cpu_clock_set(410000000);
                	REG_CPM_LPCDR = 11;
		}
		cpu_clock_get(&tempCore2, &tempMemory);
        }
        else
        {
        	cpu_clock_set(tempCore);
                REG_CPM_LPCDR = tempLcd;
        }
}        

void archdep_shutdown_extra(void)
{
	set_overclock(0);
}

void clearerr(FILE *stream)
{
}

int fflush(FILE *stream)
{
	return 1;
}

typedef enum {
	_file_type_file,
	_file_type_mem
} _file_type_e;

typedef struct {
	_file_type_e type;
	void*        data;
} _file_t;

FILE *fdopen(int fd, const char *mode)
{
	_file_t* tempFile = (_file_t*)malloc(sizeof(_file_t));
	if(tempFile == NULL) {
		return NULL;
	}
	tempFile->type = _file_type_file;
	tempFile->data = (void *)fd;
	return (FILE *)tempFile;
}


int putchar(int c)
{
	fputc(c, stdout);
}


void rewind(FILE *f)
{
	fseek(f, 0, SEEK_SET);
}

int fileno(FILE *f)
{
	_file_t *tempfile;
	tempfile = (_file_t *)f;
	return (int)(tempfile->data);
}


int vfprintf(FILE *stream, const char *format, va_list ap)
{
	char buf[4096];
	int size;
	
	size = vsnprintf(buf, sizeof(buf), format, ap);
	return fwrite(buf, size, 1, stream);
}

int own_fprintf(FILE *stream, const char *format, ...)
{
	va_list list;

	va_start(list, format);
	vfprintf(stream, format, list);
	va_end(list);
}

void setbuf(FILE *stream, char *buf)
{
}


char _app_name[] = "vice";


int isatty(int fd)
{
	return 0;
}

int archdep_require_vkbd(void)
{
	return 1;
}

char *archdep_default_hotkey_file_name(void)
{
	char *ret;
	ret = malloc(32);
	strcpy(ret, "sdl-hotkey-");
        strcat(ret, (char *)machine_get_name());
        strcat(ret, ".vkm");
	return ret;
}

int archdep_default_logger(const char *level_string, const char *txt)
{
        return 0;
}

FILE *archdep_open_default_log_file(void)
{
        return NULL;
}

char *archdep_filename_parameter(const char *name)
{
	return lib_stralloc(name);
}

char *archdep_quote_parameter(const char *name)
{
    	/*not needed(?) */
	return lib_stralloc(name);
}

int archdep_expand_path(char **return_path, const char *orig_name)
{
        if(archdep_path_is_relative(orig_name))
        {
             char tmp[1024];
             getwd(tmp);
             *return_path = util_concat(tmp, 
                                FSDEV_DIR_SEP_STR,
                                orig_name,
                                NULL);
        }
        else
             *return_path = strdup(orig_name);
 	return -0;
}

char *archdep_program_name(void)
{
	return strdup(_program_name);
}

char *make_absolute_system_path(const char *s)
{
	char *ret;
        int len;
        
        len = strlen(s)+2;
	ret = malloc(strlen(_app_path)+len);
	strcpy(ret, _app_path);
	strcat(ret, FSDEV_DIR_SEP_STR);
	strcat(ret, s);
	return ret;   
}

char *archdep_default_resource_file_name(void)
{
        return make_absolute_system_path("vicerc");
}

char *archdep_default_save_resource_file_name(void)
{
        return make_absolute_system_path("vicerc");
}

char *archdep_make_backup_filename(const char *fname)
{
        return make_absolute_system_path("vicerc.bu");
}

char *archdep_default_fliplist_file_name(void)
{
        return make_absolute_system_path("fliplist");
}

int archdep_file_set_gzip(const char *name)
{
}

int archdep_path_is_relative(const char *p)
{
    if (p == NULL) {
        return 0;
    }
    if(*p != FSDEV_DIR_SEP_CHR)
        return 0;
    if(p[1] == ':')
        return 0;
    return 1;
}

int archdep_file_is_gzip(const char *name)
{
	return 0;
}

FILE *archdep_mkstemp_fd(char **filename, const char *mode)
{
	return NULL;
}

void archdep_startup_log_error(const char *format, ...)
{
}

char *archdep_tmpnam(void)
{
        static char s[1024];
	strcpy(s, _app_path);
	strcat(s, FSDEV_DIR_SEP_STR);
	strcat(s, "tmp");
	return s;
}

int archdep_spawn(const char *name, char **argv,
                         char **pstdout_redir, const char *stderr_redir)
{
	return -1;
}

char *archdep_default_autostart_disk_image_file_name(void)
{
	return util_concat("autostart-", machine_get_name(), ".d64", NULL);
}

char *archdep_default_sysfile_pathlist(const char *emu_id)
{
	return strdup(_app_path);
}

int archdep_file_is_blockdev(const char *filename)
{
	return 0;
}

int archdep_file_is_chardev(const char *filename)
{
	return 0;
}

int archdep_num_text_lines(void)
{
	return 40;
}

int archdep_num_text_columns(void)
{
	return 30;
}

int archdep_mkdir(const char *pathname, int mode)
{
	return -1;
}

int archdep_stat(const char *file_name, unsigned int *len, unsigned int *isdir)
{
	FILE *f;
	DIR *d;
	d = opendir(file_name);
	if(d)
	{
		*len = 0;
		*isdir = 1;
		closedir(d);
		return 0;
	}
	f = fopen(file_name, "rb");
	if(f)
	{
		fseek(f, 0, FSYS_SEEK_END);
		*len = ftell(f);
		*isdir = 0;
		fclose(f);
		return 0;
	}
	return -1;
}


int puts(const char *s)
{
	return fputs(s, stdout);
}

int ungetc(int c, FILE *f)
{
	int pos;
	int ret;
	
	pos = ftell(f);
	if(pos > 0)
	{
		ret = fseek(f, pos-1, SEEK_SET);
	}
	else
	{
		ret = -1;
	}
	return ret;
}

int unlink(const char *pathname)
{
	remove(pathname);
	return 0;
}

int access(const char *pathname, int mode)
{
	return 0;
}

int chdir(const char* path)
{
	if(strcmp(path, "..") == 0)
	{
		char *s;
		s = (char *)strrchr(_path, FSDEV_DIR_SEP_CHR);
		if(s)
		{
			*s = 0;
		}
		else
		{
			if(_path[0] == 'b')
				_path[0] = 'a';
			else
				_path[0] = 'b';
		}
	}
	else
	{
		if(strchr(path, ':'))
			return 0;
		strcat(_path, FSDEV_DIR_SEP_STR);
		strcat(_path, path);
	}
	return 0;
}


int rename(const char *oldpath, const char *newpath)
{
	FILE *f_old;
	FILE *f_new;
	char buffer[4096];
	int size;
	
	if(strcmp(oldpath, newpath) ==0)
	{
		return 0;
	}
	f_old = fopen(oldpath, "rb");
	if(!f_old)
	{
		return -1;
	}
	f_new = fopen(newpath, "wb");
	if(!f_new)
	{
		fclose(f_old);
		return -1;
	}
	while(!feof(f_old))
	{
		size = fread(buffer, 1, sizeof(buffer), f_old);
		fwrite(buffer, 1, size, f_new);
	}
	fclose(f_old);
	fclose(f_new);
	return unlink(oldpath);
}

#define FL_UNSIGNED   1
#define FL_NEG        2
#define FL_OVERFLOW   4
#define FL_READDIGIT  8

static unsigned long strtoxl(const char *nptr, const char **endptr, int ibase, int flags)
{
  const char *p;
  char c;
  unsigned long number;
  unsigned digval;
  unsigned long maxval;

  p = nptr;
  number = 0;

  c = *p++;
  while (isspace((int)(unsigned char) c)) c = *p++;

  if (c == '-') 
  {
    flags |= FL_NEG;
    c = *p++;
  }
  else if (c == '+')
    c = *p++;

  if (ibase < 0 || ibase == 1 || ibase > 36) 
  {
    if (endptr) *endptr = nptr;
    return 0L;
  }
  else if (ibase == 0)
  {
    if (c != '0')
      ibase = 10;
    else if (*p == 'x' || *p == 'X')
      ibase = 16;
    else
      ibase = 8;
  }

  if (ibase == 16)
  {
    if (c == '0' && (*p == 'x' || *p == 'X')) 
    {
      ++p;
      c = *p++;
    }
  }

  maxval = ULONG_MAX / ibase;

  for (;;) 
  {
    if (isdigit((int) (unsigned char) c))
      digval = c - '0';
    else if (isalpha((int) (unsigned char) c))
      digval = toupper(c) - 'A' + 10;
    else
      break;

    if (digval >= (unsigned) ibase) break;

    flags |= FL_READDIGIT;

    if (number < maxval || (number == maxval && (unsigned long) digval <= ULONG_MAX % ibase)) 
      number = number * ibase + digval;
    else 
      flags |= FL_OVERFLOW;

    c = *p++;
  }

  --p;

  if (!(flags & FL_READDIGIT)) 
  {
    if (endptr) p = nptr;
    number = 0L;
  }
  else if ((flags & FL_OVERFLOW) || (!(flags & FL_UNSIGNED) && (((flags & FL_NEG) && (number > -LONG_MAX)) || (!(flags & FL_NEG) && (number > LONG_MAX)))))
  {
    errno = ERANGE;

    if (flags & FL_UNSIGNED)
      number = ULONG_MAX;
    else if (flags & FL_NEG)
      number = (unsigned long) (-LONG_MAX);
    else
      number = LONG_MAX;
  }

  if (endptr != NULL) *endptr = p;

  if (flags & FL_NEG) number = (unsigned long) (-(long) number);

  return number;
}

long strtol(const char *nptr, char **endptr, int ibase)
{
  return (long) strtoxl(nptr, (const char **) endptr, ibase, 0);
}

unsigned long strtoul(const char *nptr, char **endptr, int ibase)
{
  return strtoxl(nptr, (const char **)endptr, ibase, FL_UNSIGNED);
}

void set_dingoo_pwd(const char *path)
{
	char *ptr;
	strcpy(_path, path);
	ptr = (char *)strrchr(_path, FSDEV_DIR_SEP_CHR);
	_program_name = ptr+1;
	ptr[0] = 0;
	strcpy(_app_path, _path);
}

static int dirs_amount = 0;
static int files_amount = 0;

static int ioutil_compare_names(const void* a, const void* b)
{
    ioutil_name_table_t *arg1 = (ioutil_name_table_t*)a;
    ioutil_name_table_t *arg2 = (ioutil_name_table_t*)b;
    return strcmp(arg1->name, arg2->name);
}


static void ioutil_count_dir_items(const char *path, int *dir_count, int *files_count)
{
	int ret;
	fsys_file_info_t fData;;
	
	*dir_count =1; /* with ".." */
	*files_count = 0;
	char path_string[1024];

	getwd(path_string);
	strcat(path_string, FSDEV_DIR_SEP_STR "*");
	ret = fsys_findfirst(path_string, FSYS_FIND_DIR, &fData);
	if(ret == 0) 
	{
		do
		{
			++(*dir_count);
		} while (fsys_findnext(&fData) == 0);
		fsys_findclose(&fData);
	}
	ret = fsys_findfirst(path_string, FSYS_FIND_FILE, &fData);
	if(ret == 0)
	{
		do
		{
			++(*files_count);
		} while (fsys_findnext(&fData) == 0);
		fsys_findclose(&fData);
	}
}

static void ioutil_filldir(const char *path,
                           ioutil_name_table_t *dirs,
                           ioutil_name_table_t *files)
{
	fsys_file_info_t fData;;
    	int dir_count, file_count;
	int ret;
	char *filename;

	dir_count = 0;
	file_count = 0;

	char path_string[1024];

	getwd(path_string);
	strcat(path_string, FSDEV_DIR_SEP_STR "*");


	dirs[dir_count].name = lib_stralloc("..");
	++dir_count;
	ret = fsys_findfirst(path_string, FSYS_FIND_DIR, &fData);
	if(ret == 0) 
	{
		do
		{
			dirs[dir_count].name = lib_stralloc(fData.name);
			++dir_count;
		} while (fsys_findnext(&fData) == 0);
		fsys_findclose(&fData);
	}
	ret = fsys_findfirst(path_string, FSYS_FIND_FILE, &fData);
	if(ret == 0) 
	{
		do
		{
	                files[file_count].name = lib_stralloc(fData.name);
			++file_count;
		} while (fsys_findnext(&fData) == 0);
		fsys_findclose(&fData);
	}
}

ioutil_dir_t *ioutil_opendir(const char *path)
{
    int dir_count, files_count;
    ioutil_dir_t *ioutil_dir;

    ioutil_count_dir_items(path, &dir_count, &files_count);
    if (dir_count + files_count < 0) {
        return NULL;
    }

    ioutil_dir = lib_malloc(sizeof(ioutil_dir_t));

    ioutil_dir->dirs = lib_malloc(sizeof(ioutil_name_table_t)*dir_count);
    ioutil_dir->files = lib_malloc(sizeof(ioutil_name_table_t)*files_count);

    ioutil_filldir(path, ioutil_dir->dirs, ioutil_dir->files);
    qsort(ioutil_dir->dirs, dir_count, sizeof(ioutil_name_table_t), ioutil_compare_names);
    qsort(ioutil_dir->files, files_count, sizeof(ioutil_name_table_t), ioutil_compare_names);

    ioutil_dir->dir_amount = dir_count;
    ioutil_dir->file_amount = files_count;
    ioutil_dir->counter = 0;

    return ioutil_dir;
}

void dingoo_reboot(void)
{
	REG_WDT_TCSR = WDT_TCSR_PRESCALE4 | WDT_TCSR_EXT_EN;
	REG_WDT_TCNT = 0;
	REG_WDT_TDR = JZ_EXTAL/1000;   /* reset after 4ms */
	REG_TCU_TSCR = TCU_TSSR_WDTSC; /* enable wdt clock */
	REG_WDT_TCER = WDT_TCER_TCEN;  /* wdt start */
	while (1);
}
   

