/*
 * winmain.c - Windows startup.
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

#include "vice.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

#include "winmain.h"

HINSTANCE winmain_instance;
HINSTANCE winmain_prev_instance;
LPSTR winmain_cmd_line;
int winmain_cmd_show;

#ifdef _MSC_VER
char **ParseCommandLine(int *argc)
{
char    *cmd_line;
int     _argc;
int     i;
int     scanmode;
char    **_argv;
int     startpos;

    cmd_line=GetCommandLine();
    _argc=0;
    scanmode=2;
    for (i=0; i<=strlen(cmd_line); i++) {
        switch(scanmode) {
            case 0:
                /*  Search for end of argument */
                if (cmd_line[i]==' ') {
                    scanmode=2;
                } else if (cmd_line[i]==0) {
                }
                break;
            case 1:
                /*  Search end of '"' */
                if (cmd_line[i]=='"') {
                    scanmode=2;
                } else if (cmd_line[i]==0) {
                } else {
                }
                break;
            case 2:
                /*  Skip leading spaces and search for start of argument */
                if (cmd_line[i]==' ') {
                } else if (cmd_line[i]==0) {
                } else if (cmd_line[i]=='"') {
                    scanmode=1;
                    _argc++;
                } else {
                    scanmode=0;
                    _argc++;
                }
                break;
        }
    }

    _argc=0;
    _argv=malloc(_argc*sizeof(char*));
    scanmode=2;
    for (i=0; i<=strlen(cmd_line); i++) {
        switch(scanmode) {
            case 0:
                /*  Search for end of argument */
                if (cmd_line[i]==' ') {
                    _argv[_argc]=malloc(i-startpos+1);
                    memcpy(_argv[_argc],&cmd_line[startpos],i-startpos);
                    _argv[_argc][i-startpos]=0;
                    _argc++;
                    scanmode=2;
                } else if (cmd_line[i]==0) {
                    _argv[_argc]=malloc(i-startpos+1);
                    memcpy(_argv[_argc],&cmd_line[startpos],i-startpos);
                    _argv[_argc][i-startpos]=0;
                    _argc++;
                }
                break;
            case 1:
                /*  Search end of '"' */
                if (cmd_line[i]=='"') {
                    _argv[_argc]=malloc(i-startpos+1);
                    memcpy(_argv[_argc],&cmd_line[startpos],i-startpos);
                    _argv[_argc][i-startpos]=0;
                    _argc++;
                    scanmode=2;
                } else if (cmd_line[i]==0) {
                    _argv[_argc]=malloc(i-startpos+1);
                    memcpy(_argv[_argc],&cmd_line[startpos],i-startpos);
                    _argv[_argc][i-startpos]=0;
                    _argc++;
                } else {
                }
                break;
            case 2:
                /*  Skip leading spaces and search for start of argument */
                if (cmd_line[i]==' ') {
                } else if (cmd_line[i]==0) {
                } else if (cmd_line[i]=='"') {
                    scanmode=1;
                    startpos=i+1;
                } else {
                    scanmode=0;
                    startpos=i;
                }
                break;
        }
    }

    *argc=_argc;
    return _argv;
}
#endif


int PASCAL WinMain(HINSTANCE instance, HINSTANCE prev_instance,
                   LPSTR cmd_line, int cmd_show)
{
    winmain_instance = instance;
    winmain_prev_instance = prev_instance;
    winmain_cmd_line = cmd_line;
    winmain_cmd_show = cmd_show;

#ifdef _MSC_VER
    {
    int     argc;
    char    **argv;
//    LPSTR   args[1];

//        args[0]=GetCommandLine();
//        args[0][strlen(args[0])-1]=0;
//        args[0]=args[0]+1;
//        OutputDebugString(args[0]);
        argv=ParseCommandLine(&argc);
        MAIN_PROGRAM(argc,argv);

//        MAIN_PROGRAM(1, args);
    }
#else
    MAIN_PROGRAM(_argc, _argv);
#endif

    return 0;
}
