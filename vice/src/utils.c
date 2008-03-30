/*
 * utils.c - Miscellaneous utility functions.
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

/* FIXME: This file should have all the system-specific #ifdefs removed
   someday.  */

#include "vice.h"

#ifdef STDC_HEADERS
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef __riscos
#include "ROlib.h"
#else
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#endif

#include <stdarg.h>

#if (!defined(WIN32) && !defined(__riscos))
#include <sys/wait.h>
#endif

#ifdef HAVE_VFORK_H
#include <vfork.h>
#endif

#ifdef __MSDOS__
#include <process.h>
#include <dir.h>
#include <io.h>
#endif

#include "utils.h"

#include "archdep.h"
#include "log.h"

/* ------------------------------------------------------------------------- */

/* Like malloc, but abort if not enough memory is available.  */
void *xmalloc(size_t size)
{
    void *p = malloc(size);

    if (p == NULL) {
	log_error(LOG_DEFAULT,
                  "Virtual memory exhausted: cannot allocate %lu bytes.",
                  (unsigned long)size);
	exit(-1);
    }

    return p;
}

/* Like calloc, but abort if not enough memory is available.  */
void *xcalloc(size_t nmemb, size_t size)
{
    void *p = calloc(nmemb, size);

    if (p == NULL) {
	log_error(LOG_DEFAULT,
                  "Virtual memory exhausted: cannot allocate %lu bytes.",
                  (unsigned long)size * (unsigned long)nmemb);
	exit(-1);
    }

    return p;
}

/* Like realloc, but abort if not enough memory is available.  */
void *xrealloc(void *p, size_t size)
{
    void *new_p = realloc(p, size);

    if (new_p == NULL) {
	log_error(LOG_DEFAULT,
                  "Virtual memory exhausted: cannot allocate %lu bytes.",
                  (unsigned long)size);
	exit(-1);
    }

    return new_p;
}

/* Malloc enough space for `str', copy `str' into it and return its
   address.  */
char *stralloc(const char *str)
{
    int l = strlen(str);
    char *p = (char *)xmalloc(l + 1);

    memcpy(p, str, l + 1);
    return p;
}

/* Malloc a new string whose contents concatenate the arguments until the
   first NULL pointer (max `_CONCAT_MAX_ARGS' arguments).  */
char *concat(const char *s, ...)
{
#define _CONCAT_MAX_ARGS 128
    const char *arg;
    char *new, *ptr;
    int arg_len[_CONCAT_MAX_ARGS], tot_len, num_args;
    int i;
    va_list ap;

    arg_len[0] = tot_len = strlen(s);

    va_start(ap, s);
    for (i = 1;
	 i < _CONCAT_MAX_ARGS && (arg = va_arg(ap, const char *)) != NULL;
	 i++) {
	arg_len[i] = strlen(arg);
	tot_len += arg_len[i];
    }
    num_args = i;

    new = (char *) xmalloc(tot_len + 1);

    memcpy(new, s, arg_len[0]);
    ptr = new + arg_len[0];

    va_start(ap, s);
    for (i = 1; i < num_args; i++) {
	 memcpy(ptr, va_arg(ap, const char *), arg_len[i]);
	 ptr += arg_len[i];
    }
    *ptr = '\0';

    va_end(ap);
    return new;
}

/* Add the first `src_size' bytes of `src' to the end of `buf', which is a
   malloc'ed block of `max_buf_size' bytes of which only the first `buf_size'
   ones are used.  If the `buf' is not large enough, realloc it.  Return a
   pointer to the new block.  */
char *bufcat(char *buf, int *buf_size, int *max_buf_size,
	     const char *src, int src_size)
{
#define BUFCAT_GRANULARITY 0x1000
    if (*buf_size + src_size > *max_buf_size) {
	char *new_buf;

	*max_buf_size = (((*buf_size + src_size) / BUFCAT_GRANULARITY + 1)
			  * BUFCAT_GRANULARITY);
	new_buf = (char *)xrealloc(buf, *max_buf_size);
	buf = new_buf;
    }
    memcpy(buf + *buf_size, src, src_size);
    *buf_size += src_size;
    return buf;
}

/* Remove spaces from start and end of string `s'.  The string is not
   reallocated even if it becomes smaller.  */
void remove_spaces(char *s)
{
    char *p;
    int l = strlen(s);

    for (p = s; *p == ' '; p++)
        ;

    l -= (p - s);
    memmove(s, p, l + 1);

    if (l > 0) {
        for (p = s + l - 1; l > 0 && *p == ' '; l--, p--)
            ;
        *(p + 1) = '\0';
    }
}

/* Set a new value to the dynamically allocated string *str.  */
void string_set(char **str, const char *new_value)
{
    if (*str == NULL) {
        if (new_value != NULL)
            *str = stralloc(new_value);
    } else if (new_value == NULL) {
        free(*str);
        *str = NULL;
    } else {
        *str = xrealloc(*str, strlen(new_value) + 1);
        strcpy(*str, new_value);
    }
}

/* ------------------------------------------------------------------------- */

int string_to_long(const char *str, const char **endptr, int base,
		   long *result)
{
    const char *sp, *ep;
    long weight, value;
    long sign;
    char last_letter = 0;       /* Initialize to make compiler happy.  */
    char c;

    if (base > 10)
        last_letter = 'A' + base - 11;

    c = toupper((int) *str);

    if (!isspace((int)c)
        && !isdigit((int)c)
        && (base <= 10 || c > last_letter || c < 'A')
        && c != '+' && c != '-')
        return -1;

    if (*str == '+') {
        sign = +1;
        str++;
    } else if (*str == '-') {
        str++;
        sign = -1;
    } else
        sign = +1;

    for (sp = str; isspace((int)*sp); sp++)
	;

    for (ep = sp;
         (isdigit((int)*ep)
          || (base > 10
              && toupper((int)*ep) <= last_letter
              && toupper((int)*ep) >= 'A')); ep++)
	;

    if (ep == sp)
	return -1;

    if (endptr != NULL)
	*endptr = (char *)ep;

    ep--;

    for (value = 0, weight = 1; ep >= sp; weight *= base, ep--) {
        if (base > 10 && toupper((int) *ep) >= 'A')
            value += weight * (toupper((int)*ep) - 'A' + 10);
	else
            value += weight * (int)(*ep - '0');
    }

    *result = value * sign;
    return 0;
}

/* Replace every occurrence of `string' in `s' with `replacement' and return
   the result as a malloc'ed string.  */
char *subst(const char *s, const char *string, const char *replacement)
{
    int num_occurrences;
    int total_size;
    int s_len = strlen(s);
    int string_len = strlen(string);
    int replacement_len = strlen(replacement);
    const char *sp;
    char *dp;
    char *result;

    /* First, count the occurrences so that we avoid re-allocating every
       time.  */
    for (num_occurrences = 0, sp = s;
         (sp = strstr(sp, string)) != NULL;
         num_occurrences++, sp += string_len)
        ;

    total_size = s_len - (string_len - replacement_len) * num_occurrences + 1;

    result = (char *) xmalloc(total_size);

    sp = s;
    dp = result;
    do {
        char *f = strstr(sp, string);

        if (f == NULL)
            break;

        memcpy(dp, sp, f - sp);
        memcpy(dp + (f - sp), replacement, replacement_len);
        dp += (f - sp) + replacement_len;
        s_len -= (f - sp) + string_len;
        sp = f + string_len;
        num_occurrences--;
    } while (num_occurrences != 0);

    memcpy(dp, sp, s_len + 1);

    return result;
}

/* ------------------------------------------------------------------------- */

/* Return a malloc'ed backup file name for file `fname'.  */
char *make_backup_filename(const char *fname)
{
#ifndef __MSDOS__

    /* Just add a '~' to the end of the name.  */
    int l = strlen(fname);
    char *p = (char *)xmalloc(l + 2);

    memcpy(p, fname, l);
    *(p + l) = '~';
    *(p + l + 1) = '\0';
    return p;

#else  /* !__MSDOS__ */

    /* FIXME: only works with 8+3 names.  */
    char d[MAXDRIVE], p[MAXDIR], f[MAXFILE], e[MAXEXT];
    char new[MAXPATH];

    fnsplit(fname, d, p, f, e);
    fnmerge(new, d, p, f, "BAK");

    return stralloc(new);

#endif /* !__MSDOS__ */
}

/* Make a backup for file `fname'.  */
int make_backup_file(const char *fname)
{
    char *backup_name = make_backup_filename(fname);
    int retval;

    /* Cannot do it...  */
    if (backup_name == NULL)
	return -1;

    retval = rename(fname, backup_name);

    free(backup_name);
    return retval;
}

/* Get the current working directory as a malloc'ed string.  */
char *get_current_dir(void)
{
#ifdef __riscos
    return GetCurrentDirectory();
#else
    static int len = 128;
    char *p = (char *) xmalloc(len);

    while (getcwd(p, len) == NULL) {
        if (errno == ERANGE) {
            len *= 2;
            p = (char *) xrealloc(p, len);
        } else
            return NULL;
    }

    return p;
#endif
}

/* ------------------------------------------------------------------------- */

/* Return the length of an open file in bytes.  */
unsigned long file_length(file_desc_t fd)
{
#ifdef __riscos
    size_t off, filesize;

    off = ftell(fd);
    fseek(fd, 0, SEEK_END);
    filesize = ftell(fd);
    fseek(fd, off, SEEK_SET);
    return filesize;
#else
    struct stat statbuf;

    if (fstat(fd, &statbuf) < 0)
	return -1;

    return statbuf.st_size;
#endif
}

/* Load the first `size' bytes of file named `name' into `dest'.  Return 0 on
   success, -1 on failure.  */
int load_file(const char *name, void *dest, int size)
{
    file_desc_t fd;
    int r;

    fd = open(name, O_RDONLY);
    if (fd == ILLEGAL_FILE_DESC)
	return -1;

    r = read(fd, (char *)dest, size);

    if (r != size) {
	close(fd);
	return -1;
    } else {
	close(fd);
	return 0;
    }
}

/* Write the first `size' bytes of `src' into a newly created file `name'.
   If `name' already exists, it is replaced by the new one.  Returns 0 on
   success, -1 on failure.  */
int save_file(const char *name, const void *src, int size)
{
    file_desc_t fd;
    int r;

    fd = open(name, O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if (fd == ILLEGAL_FILE_DESC)
	return -1;

    r = write(fd, (char *)src, size);

    if (r != size) {
	close(fd);
	return -1;
    } else {
	close(fd);
	return 0;
    }
}

/* Input one line from the file descriptor `f'.  FIXME: we need something
   better, line GNU `getline()'.  */
int get_line(char *buf, int bufsize, FILE *f)
{
    char *r;
    int len;

    r = fgets(buf, bufsize, f);
    if (r == NULL)
	return -1;

    len = strlen(buf);

    if (len > 0) {
	char *p;

	/* Remove trailing newline character.  */
	if (*(buf + len - 1) == '\n')
	    len--;

	/* Remove useless spaces.  */
	while (*(buf + len - 1) == ' ')
	    len--;
	for (p = buf; *p == ' '; p++, len--)
	    ;
	memmove(buf, p, len + 1);
	*(buf + len) = '\0';
    }

    return len;
}

/* Split `path' into a file name and a directory component.  Unlike
   the MS-DOS `fnsplit', the directory does not have a trailing '/'.  */
void fname_split(const char *path, char **directory_return, char **name_return)
{
    const char *p;

    if (path == NULL) {
	*directory_return = *name_return = NULL;
	return;
    }

    p = strrchr(path, '/');

#if defined __MSDOS__ || defined WIN32
    /* Both `/' and `\' are valid.  */
    {
        const char *p1;

        p1 = strrchr(path, '\\');
        if (p == NULL || p < p1)
            p = p1;
    }
#endif

    if (p == NULL) {
	if (directory_return != NULL)
	    *directory_return = NULL;
	if (name_return != NULL)
 	    *name_return = stralloc(path);
	return;
    }

    if (directory_return != NULL) {
        *directory_return = xmalloc(p - path + 1);
        memcpy(*directory_return, path, p - path);
	(*directory_return)[p - path] = '\0';
    }

    if (name_return != NULL)
        *name_return = stralloc(p + 1);

    return;
}

int path_is_relative(const char *directory)
{
    return archdep_path_is_relative(directory);
}

/* ------------------------------------------------------------------------- */

/* Launch program `name' (searched via the PATH environment variable)
   passing `argv' as the parameters, wait for it to exit and return its
   exit status. If `stdout_redir' or `stderr_redir' are != NULL,
   redirect stdout or stderr to the corresponding file.  */
int spawn(const char *name, char **argv,
	  const char *stdout_redir, const char *stderr_redir)
{
#if !defined __MSDOS__ && !defined WIN32 && !defined WINCE && !defined(__riscos)

    /* Unix & OS2(?) version.  */
    pid_t child_pid;
    int child_status;

    child_pid = vfork();
    if (child_pid < 0) {
	log_error(LOG_DEFAULT, "vfork() failed: %s.", strerror(errno));
	return -1;
    } else if (child_pid == 0) {
	if (stdout_redir && freopen(stdout_redir, "w", stdout) == NULL) {
	    log_error(LOG_DEFAULT, "freopen(\"%s\") failed: %s.",
                      stdout_redir, strerror(errno));
	    _exit(-1);
	}
	if (stderr_redir && freopen(stderr_redir, "w", stderr) == NULL) {
	    log_error(LOG_DEFAULT, "freopen(\"%s\") failed: %s.",
                      stderr_redir, strerror(errno));
	    _exit(-1);
	}
	execvp(name, argv);
	_exit(-1);
    }

    if (waitpid(child_pid, &child_status, 0) != child_pid) {
        log_error(LOG_DEFAULT, "waitpid() failed: %s", strerror(errno));
	return -1;
    }

    if (WIFEXITED(child_status))
	return WEXITSTATUS(child_status);
    else
	return -1;

#elif defined __MSDOS__

    /* MS-DOS version.  */

    int new_stdout, new_stderr;
    int old_stdout_mode, old_stderr_mode;
    int old_stdout, old_stderr;
    int retval;

    new_stdout = new_stderr = old_stdout = old_stderr = -1;

    /* Make sure we are in binary mode.  */
    old_stdout_mode = setmode(STDOUT_FILENO, O_BINARY);
    old_stderr_mode = setmode(STDERR_FILENO, O_BINARY);

    /* Redirect stdout and stderr as requested, saving the old
       descriptors.  */
    if (stdout_redir != NULL) {
	old_stdout = dup(STDOUT_FILENO);
	new_stdout = open(stdout_redir, O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if (new_stdout == -1) {
	    log_error(LOG_DEFAULT, "open(\"%s\") failed: %s.",
                      stdout_redir, strerror(errno));
	    retval = -1;
	    goto cleanup;
	}
	dup2(new_stdout, STDOUT_FILENO);
    }
    if (stderr_redir != NULL) {
	old_stderr = dup(STDERR_FILENO);
	new_stderr = open(stderr_redir, O_WRONLY | O_TRUNC | O_CREAT, 0666);
	if (new_stderr == -1) {
	    log_error(LOG_DEFAULT, "open(\"%s\") failed: %s.",
                      stderr_redir, strerror(errno));
	    retval = -1;
	    goto cleanup;
	}
	dup2(new_stderr, STDERR_FILENO);
    }

    /* Spawn the child process.  */
    retval = spawnvp(P_WAIT, name, argv);

cleanup:
    if (old_stdout >= 0) {
	dup2(old_stdout, STDOUT_FILENO);
        close(old_stdout);
    }
    if (old_stderr >= 0) {
	dup2(old_stderr, STDERR_FILENO);
        close(old_stderr);
    }
    if (old_stdout_mode >= 0)
	setmode(STDOUT_FILENO, old_stdout_mode);
    if (old_stderr_mode >= 0)
	setmode(STDERR_FILENO, old_stderr_mode);
    if (new_stdout >= 0)
	close(new_stdout);
    if (new_stderr >= 0)
	close(new_stderr);

    return retval;

#elif (defined(__riscos__))
    return 0;
#else

    /*  Win32 version.  */

#ifndef _S_IREAD
#define _S_IREAD S_IREAD
#endif
#ifndef _S_IWRITE
#define _S_IWRITE S_IWRITE
#endif

    int new_stdout, new_stderr;
    int old_stdout_mode, old_stderr_mode;
    int old_stdout, old_stderr;
    int retval;

    new_stdout = new_stderr = old_stdout = old_stderr = -1;

    /* Make sure we are in binary mode.  */
    old_stdout_mode = _setmode(STDOUT_FILENO, _O_BINARY);
    old_stderr_mode = _setmode(STDERR_FILENO, _O_BINARY);

    /* Redirect stdout and stderr as requested, saving the old
       descriptors.  */
    if (stdout_redir != NULL) {
        old_stdout = _dup(STDOUT_FILENO);
        new_stdout = _open(stdout_redir, _O_WRONLY | _O_TRUNC | _O_CREAT, _S_IWRITE | _S_IREAD);
        if (new_stdout == -1) {
            log_error(LOG_DEFAULT, "open(\"%s\") failed: %s.",
                        stdout_redir, strerror(errno));
            retval = -1;
            goto cleanup;
        }
        _dup2(new_stdout, STDOUT_FILENO);
    }
    if (stderr_redir != NULL) {
        old_stderr = _dup(STDERR_FILENO);
        new_stderr = _open(stderr_redir, _O_WRONLY | _O_TRUNC | _O_CREAT, _S_IWRITE | _S_IREAD);
        if (new_stderr == -1) {
            log_error(LOG_DEFAULT, "open(\"%s\") failed: %s.",
                        stderr_redir, strerror(errno));
            retval = -1;
            goto cleanup;
        }
        _dup2(new_stderr, STDERR_FILENO);
    }

    /* Spawn the child process.  */
    retval = _spawnvp(_P_WAIT, name, argv);

cleanup:
    if (old_stdout >= 0) {
        _dup2(old_stdout, STDOUT_FILENO);
        _close(old_stdout);
    }
    if (old_stderr >= 0) {
        _dup2(old_stderr, STDERR_FILENO);
        _close(old_stderr);
    }
    if (old_stdout_mode >= 0)
        _setmode(STDOUT_FILENO, old_stdout_mode);
    if (old_stderr_mode >= 0)
        _setmode(STDERR_FILENO, old_stderr_mode);
    if (new_stdout >= 0)
        _close(new_stdout);
    if (new_stderr >= 0)
        _close(new_stderr);

    return retval;

#endif
}

/* ------------------------------------------------------------------------- */

int read_dword(file_desc_t fd, DWORD *buf, int num)
{
    int i;
    BYTE *tmpbuf;

    tmpbuf = xmalloc(num);

    if (read(fd, (char *)tmpbuf, num) < num) {
	free(tmpbuf);
	return -1;
    }

    for (i = 0; i < (num / 4); i++)
	buf[i] = (tmpbuf[i * 4] + (tmpbuf[i * 4 + 1] << 8)
                  + (tmpbuf[i * 4 + 2] << 16) + (tmpbuf[i * 4 + 3] << 24));

    free(tmpbuf);
    return 0;
}

int write_dword(file_desc_t fd, DWORD *buf, int num)
{
    int i;
    BYTE *tmpbuf;

    tmpbuf = xmalloc(num);

    for (i = 0; i < (num / 4); i++) {
	tmpbuf[i * 4] = buf[i] & 0xff;
	tmpbuf[i * 4 + 1] = (buf[i] >> 8) & 0xff;
	tmpbuf[i * 4 + 2] = (buf[i] >> 16) & 0xff;
	tmpbuf[i * 4 + 3] = (buf[i] >> 24) & 0xff;
    }

    if (write(fd, (char *)tmpbuf, num) < 0) {
	free(tmpbuf);
	return -1;
    }

    free(tmpbuf);
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Check for existance of file named `name'.  */
int file_exists_p(const char *name)
{
    FILE *f;

    f = fopen(name, "r");
    if (f != NULL) {
        fclose(f);
        return 1;
    } else {
        return 0;
    }
}

/* ------------------------------------------------------------------------- */

char *find_next_line(const char *text, const char *pos)
{
    char *p = strchr(pos, '\n');

    return (char *) (p == NULL ? pos : p + 1);
}

char *find_prev_line(const char *text, const char *pos)
{
    const char *p;

    if (pos - text <= 2)
	return (char *) text;

    for (p = pos - 2; p != text; p--)
	if (*p == '\n')
	    break;

    if (*p == '\n')
	p++;

    return (char *) p;
}

/* ------------------------------------------------------------------------- */

/* This code is grabbed from GNU make.  It returns the maximum path length by
   using `pathconf'.  */
#ifdef NEED_GET_PATH_MAX
unsigned int get_path_max(void)
{
    static unsigned int value;

    if (value == 0) {
	long int x = pathconf("/", _PC_PATH_MAX);

	if (x > 0)
	    value = x;
	else
	    return MAXPATHLEN;
    }

    return value;
}
#endif

/* The following are replacements for libc functions that could be missing.  */

#if !defined HAVE_MEMMOVE

void *memmove(void *target, const void *source, unsigned int length)
{
    char *tptr = (char *) target;
    const char *sptr = (const char *) source;

    if (tptr > sptr) {
	tptr += length;
	sptr += length;
	while (length--)
	    *(--tptr) = *(--sptr);
    } else if (tptr < sptr) {
	while (length--)
	    *(tptr++) = *(sptr++);
    }

    return target;
}

#endif /* !defined HAVE_MEMMOVE */

#if !defined HAVE_ATEXIT

static void atexit_support_func(int status, void *arg)
{
    void (*f)(void) =(void (*)(void)) arg;

    f();
}

int atexit(void (*function)(void))
{
    return on_exit(atexit_support_func, (void *)function);
}

#endif /* !defined HAVE_ATEXIT */

#if !defined HAVE_STRERROR

char *strerror(int errnum)
{
    static char buffer[100];

    sprintf(buffer, "Error %d", errnum);
    return buffer;
}

#endif /* !defined HAVE_STRERROR */

/* The following `strcasecmp()' and `strncasecmp()' implementations are
   taken from:
   GLIB - Library of useful routines for C programming
   Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh
   MacDonald.
   The source is available from http://www.gtk.org/.  */

#if !defined HAVE_STRCASECMP

int strcasecmp(const char *s1, const char *s2)
{
    int c1, c2;

    if (s1 == NULL || s2 == NULL)
        return 0;

    while (*s1 && *s2) {
        /* According to A. Cox, some platforms have islower's that don't work
           right on non-uppercase.  */
        c1 = isupper ((unsigned int)*s1) ? tolower ((unsigned int)*s1) : *s1;
        c2 = isupper ((unsigned int)*s2) ? tolower ((unsigned int)*s2) : *s2;
        if (c1 != c2)
            return (c1 - c2);
        s1++; s2++;
    }

    return (((int)(unsigned char) *s1) - ((int)(unsigned char) *s2));
}

#endif

#if !defined HAVE_STRNCASECMP

int strncasecmp(const char *s1, const char *s2, unsigned int n)
{
    int c1, c2;

    if (s1 == NULL || s2 == NULL)
        return 0;

    while (n-- && *s1 && *s2) {
        /* According to A. Cox, some platforms have islower's that don't work
           right on non-uppercase.  */
        c1 = isupper ((unsigned int)*s1) ? tolower ((unsigned int)*s1) : *s1;
        c2 = isupper ((unsigned int)*s2) ? tolower ((unsigned int)*s2) : *s2;
        if (c1 != c2)
            return (c1 - c2);
        s1++; s2++;
    }

    if (n)
        return (((int)(unsigned char) *s1) - ((int)(unsigned char) *s2));
    else
        return 0;
}

#endif
