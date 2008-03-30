/*
 * ui_cmdline.c - Commandline output for Vice/2
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

#define INCL_DOSPROCESS
#include "vice.h"

#include <string.h>
#include <stdlib.h>

#include "cmdline.h"

#include "dialogs.h"
#include "utils.h"

extern HAB habMain;

void ui_cmdline_show_help(int num_options, cmdline_option_t *options)
{
    int i, j;
    int jmax=0;
    char *ui_cmdline_text;
    char *ui_cmdline_textopt;
    int ui_cmdline_chars;   // maximum area could be shown
    char optFormat[13];
    CHAR szClientClass [] = "VICE/2 Cmdline";
    ULONG flFrameFlags    = 0;

    //    ui_cmdline_lines=num_options;
    for (i=0; i<num_options; i++) {
        j    =strlen(options[i].name)+1;
        j   +=strlen((options[i].need_arg && options[i].param_name)?
                     options[i].param_name:"")+1;
        jmax = j>jmax ? j : jmax;
        j   += strlen(options[i].description)+1;
        ui_cmdline_chars = j>ui_cmdline_chars ? j : ui_cmdline_chars;
    }
    sprintf(optFormat,"%%-%ds%%s",jmax);

    cmdopt_dialog(HWND_DESKTOP);

    ui_cmdline_text   =(char*)xcalloc(1,ui_cmdline_chars+1);

    for (i=0; i<num_options; i++)
    {
        ui_cmdline_textopt = xmsprintf("%s %s", options[i].name,
                                       (options[i].need_arg && options[i].param_name)?
                                       options[i].param_name:"");
        ui_cmdline_text = xmsprintf(optFormat, ui_cmdline_textopt,
                                    options[i].description);
        free(ui_cmdline_textopt);

        WinSendMsg(hwndCmdopt, WM_INSERT, ui_cmdline_text, 0);
    }

    free(ui_cmdline_text);

    {
        QMSG qmsg;
        while (dlgOpen(DLGO_CMDOPT))
        {
            WinPeekMsg (habMain, &qmsg, NULLHANDLE, 0, 0, PM_REMOVE);
            WinDispatchMsg (habMain, &qmsg);
            DosSleep(1);
        }
    }

}
