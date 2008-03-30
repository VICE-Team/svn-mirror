/*
 * dlg-sound.c - The sound-dialog.
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

#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINSTDSPIN
#define INCL_WINSTDSLIDER

#include "vice.h"
#include "dialogs.h"

#include "resources.h"

/* Needed prototype functions                                       */
/*----------------------------------------------------------------- */

extern void set_volume(int vol);
extern int  get_volume(void);

/* Dialog procedures                                                */
/*----------------------------------------------------------------- */

static MRESULT EXPENTRY pm_sound(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;
    const int ID_ON  = 1;
    const int ID_OFF = 2;
    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_SOUND);
        first = TRUE;
        break;
    case WM_COMMAND:
        if (LONGFROMMP(mp1)!=DID_CLOSE) break;
    case WM_CLOSE:
        delDlgOpen(DLGO_SOUND);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                int sound;
                resources_get_value("Sound", (resource_value_t*) &sound);
                WinCheckButton   (hwnd, CB_SOUND, sound!=0);
                WinSendDlgItemMsg(hwnd, CS_VOLUME, SLM_SETSLIDERINFO,
                                  MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),
                                  MPFROMSHORT(get_volume()));
                resources_get_value("SoundSampleRate", (resource_value_t*) &sound);
                if (sound==8000 || sound==11025 || sound==22050 || sound==44100)
                    WinCheckButton(hwnd, sound, 1);
                resources_get_value("SoundOversample", (resource_value_t *) &sound);
                if (sound >=0 && sound <= 3)
                    WinCheckButton(hwnd, sound|RB_OFF, 1);
                resources_get_value("SoundBufferSize", (resource_value_t *) &sound);
                WinSetSpinVal(hwnd, SPB_BUFFER, sound);
                resources_get_value("SidModel", (resource_value_t *) &sound);
                WinCheckButton(hwnd, sound?RB_8580:RB_6581 ,1);
                resources_get_value("SidFilters", (resource_value_t *) &sound);
                WinCheckButton(hwnd, CB_SIDFILTER, sound!=0);
#ifdef HAVE_RESID
                resources_get_value("SidUseResid", (resource_value_t *) &sound);
                WinCheckButton(hwnd, CB_RESID, sound!=0);
#endif
                first = FALSE;
            }
        }
        break;
    case WM_CONTROL:
        {
            switch (SHORT1FROMMP(mp1))
            {
            case CB_SOUND:
                toggle("Sound");
                break;
            case CS_VOLUME:
                {
                    if (SHORT2FROMMP(mp1)==SLN_SLIDERTRACK || SHORT2FROMMP(mp1)==SLN_CHANGE)
                        set_volume(LONGFROMMP(mp2));
                }
                break;
            case RB_8000HZ:
            case RB_11025HZ:
            case RB_22050HZ:
            case RB_44100HZ:
                {
                    int sound;
                    resources_get_value("SoundSampleRate", (resource_value_t *) &sound);
                    if (sound!=SHORT1FROMMP(mp1))
                        resources_set_value("SoundSampleRate", (resource_value_t) SHORT1FROMMP(mp1));
                }
                break;
            case RB_OFF:
            case RB_2X:
            case RB_4X:
            case RB_8X:
                {
                    int sound;
                    resources_get_value("SoundOversample", (resource_value_t *) &sound);
                    if (sound!=(SHORT1FROMMP(mp1)&0x3))
                        resources_set_value("SoundOversample", (resource_value_t)(SHORT1FROMMP(mp1)&0x3));
                }
                break;
            case RB_6581:
            case RB_8580:
                resources_set_value("SidModel", (resource_value_t)(SHORT1FROMMP(mp1)&0x1));
                break;
            case CB_SIDFILTER:
                toggle("SidFilters");
                break;
#ifdef HAVE_RESID
            case CB_RESID:
                toggle("SidUseResid");
                break;
#endif
            case SPB_BUFFER:
                if (SHORT2FROMMP(mp1)==SPBN_ENDSPIN) {
                    ULONG sound;
                    WinGetSpinVal(hwnd, SPB_BUFFER, &sound);
                    resources_set_value("SoundBufferSize", (resource_value_t)sound);
                }
                break;
            }
            break;
        }
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void sound_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_SOUND)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_sound, NULLHANDLE,
               DLG_SOUND, NULL);
}
