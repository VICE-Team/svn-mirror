/*
 * ui.h - A user interface for OS/2.
 *
 * Written by
 *  Thomas Bretz (tbretz@gsi.de)
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

#ifndef _UI_STATUS_H
#define _UI_STATUS_H

#define INCL_WINSYS
#define INCL_GPILCIDS // vac++
#define INCL_WINSTDFILE
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINSCROLLBARS
#define INCL_DOSSEMAPHORES

#include "vice.h"
#include "ui.h"

typedef struct _ui_status
{
    HPS   hps;
    RECTL rectl;
    UINT  step;
    BOOL  init;
    //    float maxSpeed;
    //    float maxFps;
    float lastSpeed;
    float lastFps;
    int   lastSec;
    float lastTrack[4];
    CHAR  lastImage[4][255];
    ui_drive_enable_t lastDriveState;

} ui_status_t;

ui_status_t ui_status;
int         PM_winActive;
HMTX        hmtxKey;

void PM_status(void *unused);
void ui_open_status_window(void);
void ui_draw_status_window(HWND hwnd);
void ui_display_speed(float spd, float fps, int sec);

#endif
