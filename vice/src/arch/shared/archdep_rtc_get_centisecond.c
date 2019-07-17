/** \file   archdep_rtc_get_centisecond.c
 * \brief   get centiseconds time when gettimeofday() is not available
 * \author  groepaz <groepaz@gmx.net>
 */

/*
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

/* FIXME: includes for OS2 */
/* FIXME: includes for windows */

#if defined(ARCHDEP_OS_OS2)

#ifndef HAVE_GETTIMEOFDAY
int archdep_rtc_get_centisecond(void)
{
    struct timeb tb;

    __ftime(&tb);

    return (int)tb.millitm / 10;
}
#endif

#endif

#if defined(ARCHDEP_OS_WINDOWS)

int archdep_rtc_get_centisecond(void)
{
    SYSTEMTIME t;

    GetSystemTime(&t);
    return (int)(t.wMilliseconds / 10);
}

#endif
