/*
 * utils.h - Miscellaneous utility functions.
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

#ifndef _UTILS_H
#define _UTILS_H

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#endif

#include "types.h"

extern char *stralloc(const char *str);
extern void *xmalloc(size_t s);
extern void *xcalloc(size_t nmemb, size_t size);
extern void *xrealloc(void *p, size_t s);
extern char *concat(const char *s1, ...);
extern char *bufcat(char *buf, int *buf_size, int *max_buf_size,
                    const char *src, int src_size);
extern void remove_spaces(char *s);
extern char *make_backup_filename(const char *fname);
extern int make_backup_file(const char *fname);
extern char *get_current_dir(void);

extern unsigned long file_length(FILE *fd);
extern int load_file(const char *name, void *dest, int size);
extern int save_file(const char *name, const void *src, int size);
extern int remove_file(const char *name);

extern int get_line(char *buf, int bufsize, FILE *f);
extern void fname_split(const char *path, char **directory_return,
                        char **name_return);

extern int string_to_long(const char *str, const char **endptr, int base,
                          long *result);
extern char *subst(const char *s, const char *string, const char *replacement);
extern void string_set(char **str, const char *new_value);

extern int read_dword(FILE *fd, DWORD *buf, int num);
extern int write_dword(FILE *fd, DWORD *buf, int num);

extern int file_exists_p(const char *name);

extern char *find_prev_line(const char *text, const char *pos);
extern char *find_next_line(const char *text, const char *pos);

#if !defined HAVE_MEMMOVE
void *memmove(void *target, const void *source, unsigned int length);
#endif

#if !defined HAVE_ATEXIT
int atexit(void (*function)(void));
#endif

#if !defined HAVE_STRERROR
char *strerror(int errnum);
#endif

#if !defined HAVE_STRCASECMP
int strcasecmp(const char *s1, const char *s2);
#endif

#if !defined HAVE_STRNCASECMP
int strncasecmp(const char *s1, const char *s2, unsigned int n);
#endif

#endif /* UTILS_H */
