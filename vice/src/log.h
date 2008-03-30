/*
 * log.h - Logging facility.
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

#ifndef _LOG_H
#define _LOG_H

typedef int log_t;
#define LOG_ERR     ((log_t)-1)
#define LOG_DEFAULT ((log_t)-2)

int log_init_resources(void);
int log_init_cmdline_options(void);
int log_init(void);
log_t log_open(const char *id);
int log_close(log_t log);
int log_message(log_t log, const char *format, ...);
int log_warning(log_t log, const char *format, ...);
int log_error(log_t log, const char *format, ...);
int log_debug(const char *format, ...);

#endif
