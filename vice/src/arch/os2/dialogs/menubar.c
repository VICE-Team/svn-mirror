/*
 * menubar.c - The menubar.
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

#define INCL_WINMENUS      // WinCheckMenuItem
#define INCL_WINDIALOGS    // WinMessageBox
#define INCL_WINWINDOWMGR  // QWL_USER
#include "video.h"         // canvas_*
#include "dialogs.h"

#include "log.h"

#include "mon.h"        // mon
#include "tape.h"       // tape_detach_image
#include "drive.h"      // DRIVE_SYNC_*
#include "attach.h"     // file_system_detach_disk
#include "interrupt.h"  // maincpu_trigger_trap
#include "resources.h"  // resources_save

// --------------------------------------------------------------------------

#if defined __X64__ || defined __X128__ || defined __XVIC__
static const char *VIDEO_CACHE="VideoCache";
#else
//#if defined __XPET__ || defined __XCBM__
static const char *VIDEO_CACHE="CrtcVideoCache";
#endif

extern int isEmulatorPaused(void);
extern void emulator_pause(void);
extern void emulator_resume(void);

// --------------------------------------------------------------------------

static void mon_trap(ADDRESS addr, void *unused_data)
{
    mon(addr);
}

static void resize_trap(ADDRESS addr, void *arg)
{
    canvas_t *c = (canvas_t*) arg;

    //
    // resize canvas, show it again
    // new stretch factor set automatically
    //
    canvas_resize(c, c->width, c->height);
    WinSetWindowPos(c->hwndFrame, 0, 0, 0, 0, 0, SWP_SHOW);
}

/* ------------- Show as context menu ------------------
   WinPopupMenu(HWND_DESKTOP,
                hwnd,
                WinLoadMenu(hwnd, NULLHANDLE, IDM_MAINMENU),
                50, 50,
                IDM_MAINMENU,
                PU_POSITIONONITEM|PU_HCONSTRAIN|PU_VCONSTRAIN|PU_NONE);
*/

void menu_action(HWND hwnd, SHORT idm, MPARAM mp2)
{
    switch (idm)
    {
    case IDM_ATTACHTAPE:
    case IDM_ATTACH8:
    case IDM_ATTACH9:
    case IDM_ATTACH10:
    case IDM_ATTACH11:
        attach_dialog(hwnd, idm&0xf);
        return;

    case IDM_DETACHTAPE:
        tape_detach_image();
        return;

    case IDM_DETACH8:
    case IDM_DETACH9:
    case IDM_DETACH10:
    case IDM_DETACH11:
        file_system_detach_disk(idm&0xf);
        return;

    case IDM_DETACHALL:
        file_system_detach_disk(8);
        file_system_detach_disk(9);
        file_system_detach_disk(10);
        file_system_detach_disk(11);
        return;

    case IDM_READCONFIG:
        return;

    case IDM_WRITECONFIG:
        WinMessageBox(HWND_DESKTOP, hwnd,
                      resources_save(NULL)<0?"Cannot save settings.":
                      "Settings written successfully.",
                      "Resources", 0, MB_OK);
        return;

    case IDM_HARDRESET:
        hardreset_dialog(hwnd);
        return;

    case IDM_SOFTRESET:
        softreset_dialog(hwnd);
        return;

#ifdef __X128__
    case IDM_VDC16K:
    case IDM_VDC64K:
        resources_set_value("VDC_64KB", (resource_value_t*)(idm&1));
        return;
#endif

#if defined __X64__ || defined __X128__
    case IDM_PAL:
        resources_set_value("VideoStandard",
                            (resource_value_t*) DRIVE_SYNC_PAL);
        return;
    case IDM_NTSC:
        resources_set_value("VideoStandard",
                            (resource_value_t*) DRIVE_SYNC_NTSC);
        return;
#ifdef __X64__
    case IDM_NTSCOLD:
        resources_set_value("VideoStandard",
                            (resource_value_t*) DRIVE_SYNC_NTSCOLD);
        return;
#endif // __X64__
#endif // __X64__ || __X128__
    case IDM_REU:
        toggle("REU");
        return;

    case IDM_MOUSE:
        toggle("Mouse");
        return;

    case IDM_HIDEMOUSE:
        toggle("HideMousePtr");
        return;

    case IDM_VCACHE:
        toggle(VIDEO_CACHE);
        return;

    case IDM_SBCOLL:
        toggle("CheckSbColl");
        return;

    case IDM_SSCOLL:
        toggle("CheckSsColl");
        return;

    case IDM_EMUID:
        toggle("EmuID");
        return;

    case IDM_REFRATEAUTO:
    case IDM_REFRATE1:
    case IDM_REFRATE2:
    case IDM_REFRATE3:
    case IDM_REFRATE4:
    case IDM_REFRATE5:
    case IDM_REFRATE6:
    case IDM_REFRATE7:
    case IDM_REFRATE8:
    case IDM_REFRATE9:
    case IDM_REFRATE10:
        resources_set_value("RefreshRate", (resource_value_t)(idm&0xf));
        return;

    case IDM_MONITOR:
        //
        // open monitor dialog
        //
        monitor_dialog(hwnd);

        //
        // make sure that the build in monitor can be invoked
        //
        emulator_resume();

        //
        // trigger invokation of the build in monitor
        //
        maincpu_trigger_trap(mon_trap, NULL);
        return;

        /*
         case IDM_SNAPSAVE:
         maincpu_trigger_trap(save_snapshot, (void*)hwnd);
         return;
         */
    case IDM_DATASETTE:
        datasette_dialog(hwnd);
        return;

    case IDM_DRIVE:
        drive_dialog(hwnd);
        return;

    case IDM_SOUNDDLG:
        sound_dialog(hwnd);
        return;

    case IDM_SC6581:
    case IDM_SC8580:
        resources_set_value("SidModel", (resource_value_t)(idm&1));
        return;

    case IDM_SOUNDON:
        toggle("Sound");
        return;

    case IDM_SIDFILTER:
        toggle("SidFilters");
        return;

    case IDM_RESID:
        toggle("SidUseResid");
        return;

    case IDM_OSOFF:
    case IDM_OS2X:
    case IDM_OS4X:
    case IDM_OS8X:
        resources_set_value("SoundOversample", (resource_value_t)(idm&0xf));
        return;

    case IDM_SR8000:
        resources_set_value("SoundSampleRate", (resource_value_t)8000);
        return;

    case IDM_SR11025:
        resources_set_value("SoundSampleRate", (resource_value_t)11025);
        return;

    case IDM_SR22050:
        resources_set_value("SoundSampleRate", (resource_value_t)22050);
        return;

    case IDM_SR44100:
        resources_set_value("SoundSampleRate", (resource_value_t)44100);
        return;

    case IDM_BUF010:
    case IDM_BUF025:
    case IDM_BUF040:
    case IDM_BUF055:
    case IDM_BUF070:
    case IDM_BUF085:
    case IDM_BUF100:
        resources_set_value("SoundBufferSize", (resource_value_t)((idm&0xff)*50));
        return;

#ifndef __X128__
        //
        // FIXME: we get only the Handle for one canvas here
        //
    case IDM_STRETCH1:
    case IDM_STRETCH2:
    case IDM_STRETCH3:
        {
            extern void wmVrnDisabled(HWND hwnd);
            extern void wmVrnEnabled(HWND hwnd);

            canvas_t *c = (canvas_t *)WinQueryWindowPtr(hwnd, QWL_USER);
            //
            // disable visible region (stop blitting to display)
            //
            wmVrnDisabled(hwnd);

            //
            // set new stretch factor
            //
            resources_set_value("WindowStretchFactor", (resource_value_t)(idm&0xf));

            //
            // resize canvas
            //
            canvas_resize(c, c->width, c->height);

            //
            // set visible region (start blitting again)
            //
            wmVrnEnabled(hwnd);
        }
        return;
#endif

    case IDM_PAUSE:
        if (isEmulatorPaused())
            emulator_resume();
        else
            emulator_pause();
        return;


#ifdef HAS_JOYSTICK
    case IDM_JOYSTICK:
        joystick_dialog(hwnd);
        return;
#endif

    case IDM_ABOUT:
        about_dialog(hwnd);
        return;

    case IDM_EXIT:
        {
            extern int trigger_shutdown;
            trigger_shutdown = 1;
        }
        return;
    }
}

// --------------------------------------------------------------------------

static void WinCheckRes(HWND hwnd, USHORT id, const char *name)
{
    int val;
    resources_get_value(name, (resource_value_t*) &val);
    WinCheckMenuItem(hwnd, id, val);
}

void menu_select(HWND hwnd, USHORT item)
{
    int val;

    switch (item)
    {
#if defined __X64__ || defined __X128__
    case IDM_VIDEOSTD:
        resources_get_value("VideoStandard", (resource_value_t*) &val);
        WinCheckMenuItem(hwnd, IDM_PAL,     val==DRIVE_SYNC_PAL);
        WinCheckMenuItem(hwnd, IDM_NTSC,    val==DRIVE_SYNC_NTSC);
#ifdef __X64__
        WinCheckMenuItem(hwnd, IDM_NTSCOLD, val==DRIVE_SYNC_NTSCOLD);
#endif
        return;
#endif

    case IDM_SETUP:
        WinCheckRes(hwnd, IDM_MOUSE,     "Mouse");
        WinCheckRes(hwnd, IDM_HIDEMOUSE, "HideMousePtr");
        WinCheckRes(hwnd, IDM_HIDEMOUSE, "HideMousePtr");
        WinCheckRes(hwnd, IDM_EMUID,     "EmuID");
        WinCheckRes(hwnd, IDM_VCACHE,    VIDEO_CACHE);
#if defined __X64__ || defined __X128__
        WinCheckRes(hwnd, IDM_REU,       "REU");
#endif
        WinCheckMenuItem(hwnd, IDM_PAUSE, isEmulatorPaused());
        return;

    case IDM_COLLISION:
        WinCheckRes(hwnd, IDM_SBCOLL, "CheckSbColl");
        WinCheckRes(hwnd, IDM_SSCOLL, "CheckSsColl");
        return;

    case IDM_REFRATE:
        resources_get_value("RefreshRate", (resource_value_t*)&val);
        {
            int i;
            for (i=0x0; i<0xb; i++)
                WinCheckMenuItem(hwnd, IDM_REFRATEAUTO|i, i==val);
        }
        return;

    case IDM_SOUND:
        WinCheckRes(hwnd, IDM_SOUNDON,   "Sound");
        WinCheckRes(hwnd, IDM_SIDFILTER, "SidFilters");
        WinCheckRes(hwnd, IDM_RESID,     "SidUseResid");
        return;

    case IDM_SIDCHIP:
        resources_get_value("SidModel", (resource_value_t*)&val);
        WinCheckMenuItem(hwnd, IDM_SC6581, !val);
        WinCheckMenuItem(hwnd, IDM_SC8580, val);
        return;

    case IDM_OVERSAMPLING:
        resources_get_value("SoundOversample", (resource_value_t*)&val);
        WinCheckMenuItem(hwnd, IDM_OSOFF,   val==0);
        WinCheckMenuItem(hwnd, IDM_OS2X,    val==1);
        WinCheckMenuItem(hwnd, IDM_OS4X,    val==2);
        WinCheckMenuItem(hwnd, IDM_OS8X,    val==3);
        return;

    case IDM_SAMPLINGRATE:
        resources_get_value("SoundSampleRate", (resource_value_t*)&val);
        WinCheckMenuItem(hwnd, IDM_SR8000,  val==8000);
        WinCheckMenuItem(hwnd, IDM_SR11025, val==11025);
        WinCheckMenuItem(hwnd, IDM_SR22050, val==22050);
        WinCheckMenuItem(hwnd, IDM_SR44100, val==44100);
        return;

    case IDM_BUFFER:
        resources_get_value("SoundBufferSize", (resource_value_t*)&val);
        WinCheckMenuItem(hwnd, IDM_BUF010, val==100);
        WinCheckMenuItem(hwnd, IDM_BUF025, val==250);
        WinCheckMenuItem(hwnd, IDM_BUF040, val==400);
        WinCheckMenuItem(hwnd, IDM_BUF055, val==550);
        WinCheckMenuItem(hwnd, IDM_BUF070, val==700);
        WinCheckMenuItem(hwnd, IDM_BUF085, val==850);
        WinCheckMenuItem(hwnd, IDM_BUF100, val==1000);
        return;
#ifndef __X128__
    case IDM_STRETCH:
        resources_get_value("WindowStretchFactor", (resource_value_t*)&val);
        WinCheckMenuItem(hwnd, IDM_STRETCH1, val==1);
        WinCheckMenuItem(hwnd, IDM_STRETCH2, val==2);
        WinCheckMenuItem(hwnd, IDM_STRETCH3, val==3);
        return;
#endif // __X128__
    }
}

// --------------------------------------------------------------------------

