/*
 * dialogs.h - The dialog windows.
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

#ifndef _DIALOGS_H
#define _DIALOGS_H

#include <os2.h>
#include "config.h"

#define ID_NONE        -1
#define DID_CLOSE      DID_OK

// Sound Dialog
#define DLG_SOUND      0x1010
#define CB_SOUND       0x1011
#define CS_VOLUME      0x1012
#define SPB_BUFFER     0x1013
#ifdef HAVE_RESID
  #define CB_RESID     0x1015
#endif
#define RB_8000HZ      0x1F40
#define RB_11025HZ     0x2B11
#define RB_22050HZ     0x5622
#define RB_44100HZ     0xAC44
#define RB_OFF         0x1030
#define RB_2X          0x1031
#define RB_4X          0x1032
#define RB_8X          0x1033
#if defined __X64__ || defined __X128__ || defined __XCBM__
  #define CB_SIDFILTER   0x1014
  #define RB_6581        0x1040
  #define RB_8580        0x1041
#endif

// Drive Dialog
#define DLG_DRIVE      0x1019
#define RB_DRIVE8      0x1020
#define RB_DRIVE9      0x1021
#define RB_DRIVE10     0x1022
#define RB_DRIVE11     0x1023
#define CB_TRUEDRIVE   0x1024

#define SPB_TRACK8     0x1027
#define SPB_TRACK9     0x1028
#define SS_HALFTRACK8  0x1029
#define SS_HALFTRACK9  0x102a
#define SS_LED8        0x102b
#define SS_LED9        0x102c

#define RB_NONE        0x1030
#define RB_SKIP        0x1031
#define RB_TRAP        0x1032

#define CBS_IMAGE      0x1033
#define CBS_PATH       0x1034
#define CBS_TYPE       0x1035
#define CB_PARALLEL    0x1036

#define CB_ALLOWACCESS 0x1037
#define CB_CONVERTP00  0x1038
#define CB_SAVEP00     0x1039
#define CB_HIDENONP00  0x103a
#define PB_CREATE      0x103b
#define PB_ATTACH      0x103c
#define PB_DETACH      0x103d
#define CB_READONLY    0x103e

#define RB_NEVER       0x1040
#define RB_ASK         0x1041
#define RB_ALWAYS      0x1042

#define PB_FLIP        0x1045
#define PB_FLIPADD     0x1046
#define PB_FLIPREMOVE  0x1047

#define CB_MEM2000     0x1048
#define CB_MEM4000     0x1049
#define CB_MEM6000     0x104a
#define CB_MEM8000     0x104b
#define CB_MEMA000     0x104c

#define WM_SWITCH      WM_USER+0x1
#define WM_DRIVEIMAGE  WM_USER+0x2
#define WM_TRACK       WM_USER+0x3
#define WM_DRIVELEDS   WM_USER+0x4
#define WM_DRIVESTATE  WM_USER+0x5

// About Dialog
#define DLG_ABOUT      0x1050

// Datasette Dialog
#define DLG_DATASETTE  0x1060
#define PB_STOP        0x1070  /* DATASETTE_CONTROL_STOP           0 */
#define PB_START       0x1071  /* DATASETTE_CONTROL_START          1 */
#define PB_FORWARD     0x1072  /* DATASETTE_CONTROL_FORWARD        2 */
#define PB_REWIND      0x1073  /* DATASETTE_CONTROL_REWIND         3 */
#define PB_RECORD      0x1074  /* DATASETTE_CONTROL_RECORD         4 */
#define PB_RESET       0x1075  /* DATASETTE_CONTROL_RESET          5 */
#define PB_RESETCNT    0x1076  /* DATASETTE_CONTROL_RESET_COUNTER  6 */
#define SPB_COUNT      0x1077
#define SS_SPIN        0x1078
#define CB_RESETWCPU   0x1079
#define SPB_DELAY      0x107a
#define SPB_GAP        0x107b
#define WM_COUNTER     WM_USER+0x1
#define WM_TAPESTAT    WM_USER+0x2
#define WM_SPINNING    WM_USER+0x3
//#define PB_ATTACH   see Drive Dialog
//#define PB_DETACH   see Drive Dialog

// Joystick Dialog
#define DLG_JOYSTICK   0x1400
#define CB_JOY11       0x1101
#define CB_JOY12       0x1201
#define CB_JOY21       0x1102
#define CB_JOY22       0x1202

#define CB_NUMJOY1     0x1104
#define CB_NUMJOY2     0x1204
#define CB_KS1JOY1     0x1108
#define CB_KS1JOY2     0x1208
#define CB_KS2JOY1     0x1110
#define CB_KS2JOY2     0x1210

#define ID_SWAP        0x1402
#define ID_CALIBRATE   0x1403
#define ID_KEYSET      0x1404
#define WM_SETCBS      WM_USER+0x1
#define WM_SETDLGS     WM_USER+0x2

// Calibrate Dialog
#define DLG_CALIBRATE  0x1405
#define RB_JOY1        0x1406
#define RB_JOY2        0x1407
#define SPB_UP         0x1408
#define SPB_DOWN       0x1409
#define SPB_LEFT       0x140a
#define SPB_RIGHT      0x140b
#define ID_START       0x140c
#define ID_STOP        0x140d
#define ID_RESET       0x140e
#define WM_SETJOY      WM_USER+0x3
#define WM_PROCESS     WM_USER+0x4
#define WM_FILLSPB     WM_USER+0x5
#define WM_ENABLECTRL  WM_USER+0x6

// Keyset Dialog
#define DLG_KEYSET     0x140f
#define RB_SET1        0x1410
#define RB_SET2        0x1411
#define SPB_N          0x1412
#define SPB_NW         0x1413
#define SPB_W          0x1414
#define SPB_SW         0x1415
#define SPB_S          0x1416
#define SPB_SE         0x1417
#define SPB_E          0x1418
#define SPB_NE         0x1419
#define SPB_FIRE       0x141a
#define WM_SETKEY      WM_USER+0x7
#define WM_KPROCESS    WM_USER+0x8
#define WM_KFILLSPB    WM_USER+0x9
#define WM_KENABLECTRL WM_USER+0xa

// Emulator Dialog
#define DLG_EMULATOR     0x1090
#define SPB_SPEED        0x1091
#define PB_SPEED100      0x1092
#define CBS_REFRATE      0x1093
#define CB_PAUSE         0x1094
#define CB_VCACHE        0x1095
#define CB_SBCOLL        0x1096
#define CB_SSCOLL        0x1097
#define CB_EMUID         0x1098
#define ID_SPEEDDISP     0x1099
#define ID_REFRATEDISP   0x109a
#define CB_MOUSE         0x109b
#define CB_HIDEMOUSE     0x109c
#define CBS_SSNAME       0x109d
#define PB_SSCHANGE      0x109e
#define CBS_SPSNAME      0x109f
#define PB_SPSCHANGE     0x10a0
#define RB_BMP           0x10a1
#define RB_PNG           0x10a2
#define WM_DISPLAY       WM_USER+1

// Monitor Dialog
#define DLG_MONITOR    0x10b0
#define LB_MONOUT      0x10b1
#define EF_MONIN       0x10b2
#define WM_INSERT      WM_USER+0x1
#define WM_INPUT       WM_USER+0x2
#define WM_PROMPT      WM_USER+0x3
//#define WM_DELETE      WM_USER+0x4

// Contents Dialog
#define DLG_CONTENTS   0x10c0
#define LB_CONTENTS    0x10c1

// Commandline option Dialog
#define DLG_CMDOPT     0x10d0
#define LB_CMDOPT      0x10d1

// Hardware Dialog
#define DLG_HARDWARE   0x10e0
#define RB_PAL         0x10e1
#define RB_NTSC        0x10e2
#define RB_NTSCOLD     0x10e3
#define CB_REU         0x10e4
#define RB_VDC16K      0x10e5
#define RB_VDC64K      0x10e6

#define DLG_VSID       0x10f0
#define ID_TUNENO      0x10f1
#define ID_TUNES       0x10f2
#define SPB_SETTUNE    0x10f3
#define PB_DEFTUNE     0x10f4
#define ID_TIME        0x10f5
#define ID_TNAME       0x10f6
#define ID_TAUTHOR     0x10f7
#define ID_TCOPYRIGHT  0x10f8
#define ID_TSYNC       0x10f9
#define ID_OPTIONS     0x10fa

/* WinPM-Macros                                                     */
/*----------------------------------------------------------------- */

#define WinIsDlgEnabled(hwnd, id) \
    WinIsWindowEnabled(WinWindowFromID(hwnd, id))
#define WinSendDlgMsg(hwnd, id, msg, mp1, mp2) \
    WinSendMsg(WinWindowFromID(hwnd, id), msg, (MPARAM)mp1, (MPARAM)mp2)
#define WinSetDlgLboxItemText(hwnd, id, index, psz) \
    WinSendDlgMsg(hwnd, id, LM_SETITEMTEXT, MPFROMLONG(index), MPFROMP(psz))
#define WinSetSpinVal(hwnd, id, val) \
    WinSendDlgItemMsg(hwnd, id, SPBM_SETCURRENTVALUE, (MPARAM)val,(MPARAM)0)
#define WinGetSpinVal(hwnd, id, val) \
    WinSendDlgItemMsg(hwnd, id, SPBM_QUERYVALUE, (MPARAM)val, (MPARAM)0)
#define WinLboxSelectItem(hwnd, id, index) \
    WinSendDlgMsg(hwnd, id, LM_SELECTITEM, (void*)(index), TRUE);
#define WinLboxDeselectItem(hwnd, id, index) \
    WinSendDlgMsg(hwnd, id, LM_SELECTITEM, index, FALSE);
#define WinLboxInsertItem(hwnd, id, psz) \
    WinInsertLboxItem(WinWindowFromID(hwnd, id), LIT_END, psz)
#define WinLboxInsertItemAt(hwnd, id, psz, pos) \
    WinInsertLboxItem(WinWindowFromID(hwnd, id), pos, psz)
#define WinLboxQuerySelectedItem(hwnd, id) \
    WinQueryLboxSelectedItem (WinWindowFromID(hwnd, id))
#define WinLboxQueryCount(hwnd, id) \
    WinQueryLboxCount(WinWindowFromID(hwnd, id))
#define WinLboxQueryItem(hwnd, id, pos, psz, max) \
    WinQueryLboxItemText(WinWindowFromID(hwnd, id), pos, psz, max)
#define WinLboxDeleteItem(hwnd, id, pos) \
    WinDeleteLboxItem(WinWindowFromID(hwnd, id), pos)
#define WinSetDlgFont(hwnd, id, font) \
    WinSetPresParam(WinWindowFromID(hwnd, id), PP_FONTNAMESIZE, strlen(font)+1,font)
#define WinLboxQuerySelectedItemText(hwnd, id, psz, max) \
    WinLboxQueryItem(hwnd, id, WinLboxQuerySelectedItem(hwnd, id), psz, max)
#define WinQueryDlgText(hwnd, id, psz, max) \
    WinQueryWindowText(WinWindowFromID(hwnd, id), max, psz)
#define WinSetDlgText(hwnd, id, psz) \
    WinSetWindowText(WinWindowFromID(hwnd, id), psz)
#define WinShowDlg(hwnd, id, bool) \
    WinShowWindow(WinWindowFromID(hwnd, id), bool)

/* Is-this-dialog-open handling                                     */
/*----------------------------------------------------------------- */

#define DLGO_SOUND      0x001
#define DLGO_JOYSTICK   0x002
#define DLGO_DRIVE      0x004
#define DLGO_ABOUT      0x008
#define DLGO_DATASETTE  0x010
#define DLGO_CALIBRATE  0x020
#define DLGO_EMULATOR   0x040
#define DLGO_MONITOR    0x080
#define DLGO_CONTENTS   0x100
#define DLGO_CMDOPT     0x200
#define DLGO_KEYSET     0x400
#define DLGO_HARDWARE   0x800

extern int dlgOpen(int dlg);
extern void setDlgOpen(int dlg);
extern void delDlgOpen(int dlg);

/* Resource funtions                                                */
/*----------------------------------------------------------------- */

extern int toggle(const char *resource_name);

/* Dialog-Function Prototypes                                       */
/*----------------------------------------------------------------- */
extern HWND hwndVsid;
extern HWND hwndDrive;
extern HWND hwndCmdopt;
extern HWND hwndMonitor;
extern HWND hwndEmulator;
extern HWND hwndDatasette;

extern void  drive_dialog     (HWND hwnd);
extern void  sound_dialog     (HWND hwnd);
extern void  about_dialog     (HWND hwnd);
extern void  datasette_dialog (HWND hwnd);
extern void  emulator_dialog  (HWND hwnd);
extern void  monitor_dialog   (HWND hwnd);
extern void  contents_dialog  (HWND hwnd, char *szFullFile);
extern void  attach_dialog    (HWND hwnd, int drive);
extern void  create_dialog    (HWND hwnd);
extern void  cmdopt_dialog    (HWND hwnd);
extern char* screenshot_dialog(HWND hwnd);
extern char* snapshot_dialog  (HWND hwnd);
extern void  hardware_dialog  (HWND hwnd);

extern void hardreset_dialog (HWND hwnd);
extern void softreset_dialog (HWND hwnd);

#ifdef HAS_JOYSTICK
extern void joystick_dialog  (HWND hwnd);
extern void calibrate_dialog (HWND hwnd);
extern void keyset_dialog    (HWND hwnd);
#endif

extern void vsid_dialog(void);

/*----------------------------------------------------------------- */
/*                           Menubar                                */
/*----------------------------------------------------------------- */

#define IDM_MAINMENU     0x100

#define IDM_FILE         0x101
#define IDM_ATTACH       0x102
#define IDM_DETACH       0x103
#define IDM_SNAPSHOT     0x104
#define IDM_RESET        0x105
#define IDM_VIEW         0x106
#define IDM_SOUNDDLG     0x107
#define IDM_DRIVE        0x108
#define IDM_JOYSTICK     0x109
#define IDM_DATASETTE    0x10a

#define IDM_SETUP        0x10b
#define IDM_SOUND        0x10c
#define IDM_SIDCHIP      0x10d
#define IDM_OVERSAMPLING 0x10e
#define IDM_SAMPLINGRATE 0x10f
#define IDM_VDCMEMORY    0x110
#define IDM_VIDEOSTD     0x111
#define IDM_COLLISION    0x112
#define IDM_REFRATE      0x113
#define IDM_BUFFER       0x114
#define IDM_STRETCH      0x115
#define IDM_MONITOR      0x116
#define IDM_ABOUT        0x117

#define IDM_ATTACHTAPE   0x120
#define IDM_ATTACH8      0x128
#define IDM_ATTACH9      0x129
#define IDM_ATTACH10     0x12a
#define IDM_ATTACH11     0x12b

#define IDM_DETACHTAPE   0x130
#define IDM_DETACH8      0x138
#define IDM_DETACH9      0x139
#define IDM_DETACH10     0x13a
#define IDM_DETACH11     0x13b
#define IDM_DETACHALL    0x13f

#define IDM_SNAPLOAD     0x121
#define IDM_SNAPSAVE     0x122

#define IDM_READCONFIG   0x123
#define IDM_WRITECONFIG  0x124
#define IDM_SOFTRESET    0x125
#define IDM_HARDRESET    0x126
#define IDM_EXIT         0x127

#define IDM_VDC16K       0x140
#define IDM_VDC64K       0x141

#define IDM_PAL          0x142
#define IDM_NTSC         0x143
#define IDM_NTSCOLD      0x144

#define IDM_REU          0x145

#define IDM_MOUSE        0x150
#define IDM_HIDEMOUSE    0x151
#define IDM_EMUID        0x152
#define IDM_VCACHE       0x153
#define IDM_SBCOLL       0x154
#define IDM_SSCOLL       0x155
#define IDM_REFRATEAUTO  0x160
#define IDM_REFRATE1     0x161
#define IDM_REFRATE2     0x162
#define IDM_REFRATE3     0x163
#define IDM_REFRATE4     0x164
#define IDM_REFRATE5     0x165
#define IDM_REFRATE6     0x166
#define IDM_REFRATE7     0x167
#define IDM_REFRATE8     0x168
#define IDM_REFRATE9     0x169
#define IDM_REFRATE10    0x16a

#define IDM_SC6581       0x170
#define IDM_SC8580       0x171
#define IDM_SOUNDON      0x172
#define IDM_SIDFILTER    0x173
#define IDM_RESID        0x174

#define IDM_OSOFF        0x180
#define IDM_OS2X         0x181
#define IDM_OS4X         0x182
#define IDM_OS8X         0x183

#define IDM_SR8000       0x190
#define IDM_SR11025      0x191
#define IDM_SR22050      0x192
#define IDM_SR44100      0x193

#define IDM_BUF010       0x202
#define IDM_BUF025       0x205
#define IDM_BUF040       0x208
#define IDM_BUF055       0x20b
#define IDM_BUF070       0x20e
#define IDM_BUF085       0x211
#define IDM_BUF100       0x214

#define IDM_STRETCH1     0x221
#define IDM_STRETCH2     0x222
#define IDM_STRETCH3     0x223

#define IDM_PAUSE        0x224

void menu_action(HWND hwnd, SHORT idm, MPARAM mp2);
void menu_select(HWND hwnd, USHORT item);

#endif

