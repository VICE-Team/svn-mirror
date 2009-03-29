/*
 * fullscrn.c - Common fullscreen related support functions for Win32
 *
 * Written by
 *  Tibor Biczo <crown@matavnet.hu>
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <windows.h>
#include <prsht.h>

#include "fullscrn.h"
#include "lib.h"
#include "res.h"
#include "resources.h"
#include "statusbar.h"
#include "translate.h"
#include "ui.h"
#include "videoarch.h"
#include "winlong.h"
#include "winmain.h"


static int fullscreen_nesting_level = 0;
static int dx_primary;

DirectDrawDeviceList *devices = NULL;
DirectDrawModeList *modes = NULL;


void fullscreen_set_res_from_current_display(void)
{
    int bitdepth, width, height, refreshrate;

    if (video_dx9_enabled()) {
        fullscreen_get_current_display_dx9(
                &bitdepth, &width, &height, &refreshrate);
    } else {
        fullscreen_get_current_display_ddraw(
                &bitdepth, &width, &height, &refreshrate);
    }

    resources_set_int("FullscreenBitdepth", bitdepth);
    resources_set_int("FullscreenWidth", width);
    resources_set_int("FullscreenHeight", height);
    resources_set_int("FullscreenRefreshRate", refreshrate);
}


/* check if the fullscreen resource values are valid */
static int fullscrn_res_valid(void)
{
    int device, width, height, bitdepth, refreshrate;

    GetCurrentModeParameters(&device, &width, &height, &bitdepth,&refreshrate);
    
    /* FIXME: May use modelist to check if combination is valid */
    if (device < 0 || width <= 0 || height <= 0 || bitdepth <= 0 || refreshrate < 0)
        return -1;

    return 0;
}


void fullscreen_getmodes(void)
{
    if (video_dx9_enabled()) {
        fullscreen_getmodes_dx9();
    } else {
        fullscreen_getmodes_ddraw();
    }
}


void ui_fullscreen_init(void)
{
    fullscreen_getmodes();

    /* Use current display parameters if resources are not valid */
    if (fullscrn_res_valid() < 0) {
        fullscreen_set_res_from_current_display();
    }
}


void ui_fullscreen_shutdown(void)
{
    DirectDrawModeList *m1, *m2;
    DirectDrawDeviceList *d1, *d2;

    m1 = modes;
    while (m1 != NULL) {
        m2 = m1->next;
        lib_free(m1);
        m1 = m2;
    }

    d1 = devices;
    while (d1 != NULL) {
        d2 = d1->next;
        lib_free(d1->desc);
        lib_free(d1);
        d1 = d2;
    }
}


void GetCurrentModeParameters(int *device, int *width, int *height,
							  int *bitdepth, int *refreshrate)
{
    resources_get_int("FullscreenDevice", device);
    resources_get_int("FullscreenBitdepth", bitdepth);
    resources_get_int("FullscreenWidth", width);
    resources_get_int("FullscreenHeight", height);
    resources_get_int("FullscreenRefreshRate", refreshrate);
}


int IsFullscreenEnabled(void)
{
    int b;

    resources_get_int("FullscreenEnabled", &b);

    return b;
}


void SwitchToFullscreenMode(HWND hwnd)
{
    if (video_dx9_enabled()) {
        SwitchToFullscreenModeDx9(hwnd);
    } else {
        SwitchToFullscreenModeDDraw(hwnd);
    }
}


void SwitchToWindowedMode(HWND hwnd)
{
    if (video_dx9_enabled()) {
        SwitchToWindowedModeDx9(hwnd);
    } else {
        SwitchToWindowedModeDDraw(hwnd);
    }
}


void StartFullscreenMode(HWND hwnd)
{
    SwitchToFullscreenMode(hwnd);
    resources_set_int("FullScreenEnabled", 1);
}


void EndFullscreenMode(HWND hwnd)
{
    SwitchToWindowedMode(hwnd);
    resources_set_int("FullScreenEnabled", 0);
}


void SwitchFullscreenMode(HWND hwnd)
{
    if (IsFullscreenEnabled()) {
        EndFullscreenMode(hwnd);
    } else {
        StartFullscreenMode(hwnd);
    }
}


void SuspendFullscreenMode(HWND hwnd)
{
    if (IsFullscreenEnabled()) {
        if (fullscreen_nesting_level == 0) {
            SwitchToWindowedMode(hwnd);
        }
        fullscreen_nesting_level++;
    }
}


void ResumeFullscreenMode(HWND hwnd)
{
    if (IsFullscreenEnabled()) {
        fullscreen_nesting_level--;
        if (fullscreen_nesting_level == 0) {
            SwitchToFullscreenMode(hwnd);
        }
    }
}


void SuspendFullscreenModeKeep(HWND hwnd)
{
    int device, width, height, bitdepth, rate;

    GetCurrentModeParameters(&device, &width, &height, &bitdepth, &rate);
    if (video_dx9_enabled() || ((width < 640) && (height < 480))) {
        SuspendFullscreenMode(hwnd);
    } else {
        if (IsFullscreenEnabled()) {
            if (fullscreen_nesting_level == 0) {
                ShowCursor(TRUE);
            }
        }
    }
}


void ResumeFullscreenModeKeep(HWND hwnd)
{
    int device, width, height, bitdepth, rate;

    GetCurrentModeParameters(&device, &width, &height, &bitdepth, &rate);
    if (video_dx9_enabled() || ((width < 640) && (height < 480))) {
        ResumeFullscreenMode(hwnd);
    } else {
        if (IsFullscreenEnabled()) {
            if (fullscreen_nesting_level == 0) {
                ShowCursor(FALSE);
            }
        }
    }
}



/*---------------------------------------------------------------------------*/
/*   Fullscreen settings UI stuff                                            */
/*---------------------------------------------------------------------------*/

typedef struct _VL {
    struct _VL *next;
    struct _VL *prev;
    char *text;
    int value;
} ValueList;

ValueList *bitdepthlist = NULL;
ValueList *resolutionlist = NULL;
ValueList *refresh_rates = NULL;

int fullscreen_device = 0;
int fullscreen_bitdepth = 0;
int fullscreen_width = 0;
int fullscreen_height = 0;
int fullscreen_refreshrate = 0;


static void validate_mode(int *device, int *width, int *height, int *bitdepth,
                          int *rate)
{
    DirectDrawModeList  *mode;

//  Validate devicenumber
    mode = modes;
    while (mode != NULL) {
        if (mode->devicenumber == *device)
            break;
        mode = mode->next;
    }
    if (mode == NULL) {
        *device = modes->devicenumber;
    }

//  Validate bitdepth
    mode = modes;
    while (mode != NULL) {
        if ((mode->devicenumber == *device) && (mode->bitdepth == *bitdepth))
            break;
        mode = mode->next;
    }
    if (mode == NULL) {
        mode = modes;
        while (mode != NULL) {
            if (mode->devicenumber == *device) {
                *bitdepth = mode->bitdepth;
                break;
            }
            mode = mode->next;
        }
    }

//  Validate resolution
    mode = modes;
    while (mode != NULL) {
        if ((mode->devicenumber == *device) && (mode->bitdepth == *bitdepth)
            && (mode->width == *width) && (mode->height == *height))
            break;
        mode = mode->next;
    }
    if (mode == NULL) {
        mode = modes;
        while (mode != NULL) {
            if ((mode->devicenumber == *device)
                && (mode->bitdepth == *bitdepth)) {
                *width = mode->width;
                *height = mode->height;
                break;
            }
            mode = mode->next;
        }
    }

//  Validate refreshrate
    mode = modes;
    while (mode != NULL) {
        if ((mode->devicenumber == *device) && (mode->bitdepth == *bitdepth)
            && (mode->width == *width) && (mode->height == *height)
            && (mode->refreshrate == *rate))
            break;
        mode = mode->next;
    }
    if (mode == NULL) {
        *rate = 0;
    }
}


static int GetIndexFromList(ValueList *list, int value)
{
    ValueList *search;
    int pos;

    pos = 0;
    search = list;
    while (search != NULL) {
        if (search->value == value)
            return pos;
        search = search->next;
        pos++;
    }
    return -1;
}

static int GetValueFromList(ValueList * list, int index)
{
    ValueList *search;
    int pos;

    search = list;
    pos = 0;
    while (search != NULL) {
        if (pos == index)
            return search->value;
        pos++;
        search = search->next;
    }
    return 0;
}

static void InsertInto(ValueList **list, ValueList *value)
{
    ValueList *after;
    ValueList *before;

    after = *list;
    before = NULL;
    while (after != NULL) {
        if (value->value < after->value) {
            break;
        }
        before = after;
        after = after->next;
    }
    value->prev = before;
    value->next = after;
    if (*list == NULL) {
        *list = value;
    } else if (after == NULL) {
        before->next = value;
    } else if (before == NULL) {
        after->prev = value;
        *list = value;
    } else {
        before->next = value;
        after->prev = value;
    }
}

static void DestroyList(ValueList **list)
{
    ValueList *value;
    ValueList *value2;

    value =* list;
    while (value != NULL) {
        lib_free(value->text);
        value2 = value->next;
        lib_free(value);
        value = value2;
    }
    *list = NULL;
}

static void get_refreshratelist(int device, int bitdepth, int width, int height)
{
    DirectDrawModeList *mode;
    ValueList *value;
    char buff[256];

    DestroyList(&refresh_rates);

    //  We always need 'Default' as when support for different
    //  Refreshrates exists, then it is not reported back
    value = lib_malloc(sizeof(ValueList));
    value->value = 0;
    value->text = lib_stralloc("Default");
    InsertInto(&refresh_rates, value);

    mode=modes;
    while (mode!=NULL) {
        if ((mode->devicenumber == device) && (mode->bitdepth == bitdepth)
            && (mode->width == width) && (mode->height == height)) {
            if (GetIndexFromList(refresh_rates, mode->refreshrate) == -1) {
                value = lib_malloc(sizeof(ValueList));
                value->value = mode->refreshrate;
                itoa(mode->refreshrate, buff, 10);
                value->text = lib_stralloc(buff);
                InsertInto(&refresh_rates, value);
            }
        }
        mode = mode->next;
    }
}

static void get_bitdepthlist(int device)
{
    DirectDrawModeList *mode;
    ValueList *value;
    char buff[256];

    DestroyList(&bitdepthlist);
    mode = modes;
    while (mode != NULL) {
        if ((mode->devicenumber == device)) {
            if (GetIndexFromList(bitdepthlist, mode->bitdepth) == -1) {
                value = lib_malloc(sizeof(ValueList));
                value->value = mode->bitdepth;
                itoa(mode->bitdepth, buff, 10);
                value->text = lib_stralloc(buff);
                InsertInto(&bitdepthlist, value);
            }
        }
        mode = mode->next;
    }
}

static void get_resolutionlist(int device, int bitdepth)
{
    DirectDrawModeList *mode;
    ValueList *value;
    char buff[256];

    DestroyList(&resolutionlist);
    mode = modes;
    while (mode != NULL) {
        if ((mode->devicenumber == device) && (mode->bitdepth == bitdepth)) {
            if (GetIndexFromList(resolutionlist, ((mode->width << 16) +
                mode->height)) == -1) {
                value = lib_malloc(sizeof(ValueList));
                value->value = (mode->width << 16) + mode->height;
                sprintf(buff, "%dx%d", mode->width, mode->height);
                value->text=lib_stralloc(buff);
                InsertInto(&resolutionlist, value);
            }
        }
        mode = mode->next;
    }
}

static int vblank_sync;

static void init_fullscreen_dialog(HWND hwnd)
{
    HWND setting_hwnd;
    DirectDrawDeviceList *dev;
    ValueList *value;

    validate_mode(&fullscreen_device, &fullscreen_width, &fullscreen_height,
                  &fullscreen_bitdepth, &fullscreen_refreshrate);
    setting_hwnd = GetDlgItem(hwnd, IDC_FULLSCREEN_DEVICE);
    SendMessage(setting_hwnd, CB_RESETCONTENT, 0, 0);
    dev = devices;
    while (dev != NULL) {
        SendMessage(setting_hwnd, CB_ADDSTRING, 0, (LPARAM)dev->desc);
        dev = dev->next;
    }
    SendMessage(setting_hwnd, CB_SETCURSEL, (WPARAM)fullscreen_device, 0);

    get_bitdepthlist(fullscreen_device);
    setting_hwnd = GetDlgItem(hwnd, IDC_FULLSCREEN_BITDEPTH);
    SendMessage(setting_hwnd, CB_RESETCONTENT, 0, 0);
    value = bitdepthlist;
    while (value != NULL) {
        SendMessage(setting_hwnd, CB_ADDSTRING, 0, (LPARAM)value->text);
        value = value->next;
    }
    SendMessage(setting_hwnd, CB_SETCURSEL,
                (WPARAM)GetIndexFromList(bitdepthlist, fullscreen_bitdepth), 0);

    get_resolutionlist(fullscreen_device, fullscreen_bitdepth);
    setting_hwnd = GetDlgItem(hwnd, IDC_FULLSCREEN_RESOLUTION);
    SendMessage(setting_hwnd, CB_RESETCONTENT, 0, 0);
    value = resolutionlist;
    while (value != NULL) {
        SendMessage(setting_hwnd, CB_ADDSTRING, 0, (LPARAM)value->text);
        value = value->next;
    }
    SendMessage(setting_hwnd, CB_SETCURSEL,
                (WPARAM)GetIndexFromList(resolutionlist,
                (fullscreen_width << 16) + fullscreen_height), 0);

    get_refreshratelist(fullscreen_device, fullscreen_bitdepth,
                        fullscreen_width, fullscreen_height);
    setting_hwnd = GetDlgItem(hwnd, IDC_FULLSCREEN_REFRESHRATE);
    SendMessage(setting_hwnd, CB_RESETCONTENT, 0, 0);
    value = refresh_rates;
    while (value != NULL) {
        SendMessage(setting_hwnd, CB_ADDSTRING, 0, (LPARAM)value->text);
        value = value->next;
    }
    SendMessage(setting_hwnd, CB_SETCURSEL,
                (WPARAM)GetIndexFromList(refresh_rates, fullscreen_refreshrate),
                0);
    CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_VBLANK_SYNC, vblank_sync
                   ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_TOGGLE_VIDEO_DX_PRIMARY, dx_primary
                   ? BST_CHECKED : BST_UNCHECKED);
}


static void fullscreen_dialog_end(void)
{
    resources_set_int("FullScreenDevice", fullscreen_device);
    resources_set_int("FullScreenBitdepth", fullscreen_bitdepth);
    resources_set_int("FullScreenWidth", fullscreen_width);
    resources_set_int("FullScreenHeight", fullscreen_height);
    resources_set_int("FullScreenRefreshRate", fullscreen_refreshrate);
    resources_set_int("VBLANKSync", vblank_sync);
    resources_set_int("DXPrimarySurfaceRendering", dx_primary);
    fullscrn_invalidate_refreshrate();
}

static void fullscreen_dialog_init(HWND hwnd)
{
    resources_get_int("FullscreenDevice", &fullscreen_device);
    resources_get_int("FullscreenBitdepth", &fullscreen_bitdepth);
    resources_get_int("FullscreenWidth", &fullscreen_width);
    resources_get_int("FullscreenHeight", &fullscreen_height);
    resources_get_int("FullscreenRefreshRate", &fullscreen_refreshrate);
    resources_get_int("VBLANKSync", &vblank_sync);
    resources_get_int("DXPrimarySurfaceRendering", &dx_primary);
    init_fullscreen_dialog(hwnd);
}

INT_PTR CALLBACK dialog_fullscreen_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                        LPARAM lparam)
{
    int notifycode;
    int item;
    int index;
    int value;
    int command;

    switch (msg) {
      case WM_NOTIFY:
        if (((NMHDR FAR *)lparam)->code == (UINT)PSN_APPLY) {
            fullscreen_dialog_end();
            SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
            return TRUE;
        }
        return FALSE;
      case WM_COMMAND:
        notifycode = HIWORD(wparam);
        item = LOWORD(wparam);
        if (notifycode == CBN_SELENDOK) {
            if (item == IDC_FULLSCREEN_DEVICE) { 
                fullscreen_device = (int)SendMessage(GetDlgItem(hwnd,
                                                     IDC_FULLSCREEN_DEVICE),
                                                     CB_GETCURSEL, 0, 0);
            } else if (item == IDC_FULLSCREEN_BITDEPTH) {
                index = (int)SendMessage(GetDlgItem(hwnd,
                                         IDC_FULLSCREEN_BITDEPTH),
                                         CB_GETCURSEL, 0, 0);
                fullscreen_bitdepth = GetValueFromList(bitdepthlist, index);
            } else if (item == IDC_FULLSCREEN_RESOLUTION) {
                index = (int)SendMessage(GetDlgItem(hwnd,
                                         IDC_FULLSCREEN_RESOLUTION),
                                         CB_GETCURSEL, 0, 0);
                value = GetValueFromList(resolutionlist, index);
                fullscreen_width = value >> 16;
                fullscreen_height = value & 0xffff;
            } else if (item == IDC_FULLSCREEN_REFRESHRATE) {
                index = (int)SendMessage(GetDlgItem(hwnd,
                                         IDC_FULLSCREEN_REFRESHRATE), 
                                         CB_GETCURSEL, 0, 0);
                fullscreen_refreshrate = GetValueFromList(refresh_rates,
                                                          index);
            }
            init_fullscreen_dialog(hwnd);
        } else {
            command = LOWORD(wparam);
            switch (command) {
              case IDC_TOGGLE_VIDEO_VBLANK_SYNC:
                vblank_sync ^= 1;
                return FALSE;
              case IDC_TOGGLE_VIDEO_DX_PRIMARY:
                dx_primary ^= 1;
                return FALSE;
              case IDOK:
                fullscreen_dialog_end();
              case IDCANCEL:
                EndDialog(hwnd,0);
                return TRUE;
            }
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd,0);
        return TRUE;
      case WM_INITDIALOG:
        fullscreen_dialog_init(hwnd);
        return TRUE;
    }
    return FALSE;
}
