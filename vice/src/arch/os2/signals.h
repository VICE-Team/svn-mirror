/*
 * signals.h - signalnames
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#ifndef _SIGNALS_H
#define _SIGNALS_H

#ifdef __IBMC__
//   #define SIGILL       1       /* illegal instruction - invalid function image    */
//   #define SIGSEGV      2       /* invalid access to memory                        */
//   #define SIGFPE       3       /* floating point exception                        */
//   #define SIGTERM      4       /* OS/2 SIGTERM (killprocess) signal               */
//   #define SIGABRT      5       /* abort() signal                                  */
//   #define SIGINT       6       /* OS/2 SIGINTR signal                             */
//   #define SIGUSR1      7       /* user exception in range 0xa0000000 - 0xbfffffff */
//   #define SIGUSR2      8       /* user exception in range 0xc0000000 - 0xdfffffff */
//   #define SIGUSR3      9       /* user exception in range 0xe0000000 - 0xffffffff */
//   #define SIGBREAK    10       /* OS/2 Ctrl-Break sequence                        */
char sys_siglist[][9] = {
    "NULL",
    "SIGILL",
    "SIGSEGV",
    "SIGFPE",
    "SIGTERM",
    "SIGABRT",
    "SIGINT",
    "SIGUSR1",
    "SIGUSR2",
    "SIGUSR3",
    "SIGBREAK"
};
#endif

#ifdef __EMXC__
//   #define SIGHUP    1 /* Hangup */
//   #define SIGINT    2 /* Interrupt (Ctrl-C) */
//   #define SIGQUIT   3 /* Quit */
//   #define SIGILL    4 /* Illegal instruction */
//   #define SIGTRAP   5 /* Single step (debugging) */
//   #define SIGABRT   6 /* abort () */
//   #define SIGEMT    7 /* EMT instruction */
//   #define SIGFPE    8 /* Floating point */
//   #define SIGKILL   9 /* Kill process */
//   #define SIGBUS   10 /* Bus error */
//   #define SIGSEGV  11 /* Segmentation fault */
//   #define SIGSYS   12 /* Invalid argument to system call */
//   #define SIGPIPE  13 /* Broken pipe */
//   #define SIGALRM  14 /* Alarm */
//   #define SIGTERM  15 /* Termination, process killed */
//   #define SIGUSR1  16 /* User-defined signal #1 */
//   #define SIGUSR2  17 /* User-defined signal #2 */
//   #define SIGCHLD  18 /* Death of a child process */
//   #define SIGBREAK 21 /* Break (Ctrl-Break) */
char sys_siglist[][] = {
    "NULL",
    "SIGHUP",
    "SIGINT",
    "SIGQUIT",
    "SIGILL",
    "SIGTRAP",
    "SIGABRT",
    "SIGEMT",
    "SIGFPE",
    "SIGKILL",
    "SIGBUS",
    "SIGSEGV",
    "SIGSYS",
    "SIGPIPE",
    "SIGALRM",
    "SIGTERM",
    "SIGUSR1",
    "SIGUSR2",
    "SIGCHLD",
    "SIG19",
    "SIG20",
    "SIGBREAK"
};
#endif

#endif
