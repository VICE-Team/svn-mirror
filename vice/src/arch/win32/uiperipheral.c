/*
 * uiperipheral.c - Implementation of the device manager dialog box.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@mail.matav.hu>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#ifdef HAVE_SHLOBJ_H
#include <shlobj.h>
#endif

#ifndef DUMMYUNIONNAME
#define DUMMYUNIONNAME  u1
#endif

#include "attach.h"
#include "autostart.h"
#include "iecdrive.h"
#include "imagecontents.h"
#include "lib.h"
#include "opencbmlib.h"
#include "printer.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "ui.h"
#include "uilib.h"
#include "uiperipheral.h"
#include "winmain.h"


/* -------------------------------------------------------------------------- */
/*                             Disk Peripherals (8-11)                        */
/* -------------------------------------------------------------------------- */

static int have_printer_userport = -1;

static void enable_controls_for_disk_device_type(HWND hwnd, int type)
{
    EnableWindow(GetDlgItem(hwnd, IDC_DISKIMAGE),
                 type == IDC_SELECTDISK);
    EnableWindow(GetDlgItem(hwnd, IDC_BROWSEDISK),
                 type == IDC_SELECTDISK);
    EnableWindow(GetDlgItem(hwnd, IDC_AUTOSTART),
                 type == IDC_SELECTDISK);
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_ATTACH_READONLY),
                 type == IDC_SELECTDISK);
    EnableWindow(GetDlgItem(hwnd, IDC_DIR),
                 type == IDC_SELECTDIR);
    EnableWindow(GetDlgItem(hwnd, IDC_BROWSEDIR),
                 type == IDC_SELECTDIR);
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_READP00),
                 type == IDC_SELECTDIR);
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_WRITEP00),
                 type == IDC_SELECTDIR);
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_HIDENONP00),
                 type == IDC_SELECTDIR);
}


static void enable_controls(HWND hwnd)
{
    int drive_true_emulation, virtual_device_traps;
    BOOL haveIECDevice;

    resources_get_value("DriveTrueEmulation", (void *)&drive_true_emulation);
    resources_get_value("VirtualDevices", (void *)&virtual_device_traps);
    haveIECDevice = IsDlgButtonChecked(hwnd, IDC_TOGGLE_USEIECDEVICE)
                    == BST_CHECKED;
  
    if ((drive_true_emulation || !virtual_device_traps) && !haveIECDevice) {
        EnableWindow(GetDlgItem(hwnd, IDC_SELECTDISK),  FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_SELECTDIR),   FALSE);
        EnableWindow(GetDlgItem(hwnd, IDC_SELECTNONE),  FALSE);
#ifdef HAVE_OPENCBM
        EnableWindow(GetDlgItem(hwnd, IDC_SELECTREAL),  FALSE);
#endif
        CheckRadioButton(hwnd, IDC_SELECTDISK, IDC_SELECTDIR, IDC_SELECTNONE);
        enable_controls_for_disk_device_type(hwnd, IDC_SELECTNONE);
    } else {
        EnableWindow(GetDlgItem(hwnd, IDC_SELECTDISK),  TRUE);
        EnableWindow(GetDlgItem(hwnd, IDC_SELECTDIR),   TRUE);
        EnableWindow(GetDlgItem(hwnd, IDC_SELECTNONE),  TRUE);
#ifdef HAVE_OPENCBM
        if (opencbmlib_is_available())
            EnableWindow(GetDlgItem(hwnd, IDC_SELECTREAL), TRUE);
        else
            EnableWindow(GetDlgItem(hwnd, IDC_SELECTREAL), FALSE);
#endif
    }
}

static void init_dialog(HWND hwnd, unsigned int num)
{
    const char *disk_image, *dir;
    TCHAR *st_disk_image, *st_dir;
    int devtype, n;

    if (num >= 8 && num <= 11) {
        disk_image = file_system_get_disk_name(num);
        st_disk_image = system_mbstowcs_alloc(disk_image);
        SetDlgItemText(hwnd, IDC_DISKIMAGE,
                       st_disk_image != NULL ? st_disk_image : TEXT(""));
        system_mbstowcs_free(st_disk_image);

        resources_get_sprintf("FSDevice%dDir", (void *)&dir, num);
        st_dir = system_mbstowcs_alloc(dir);
        SetDlgItemText(hwnd, IDC_DIR, st_dir != NULL ? st_dir : TEXT(""));
        system_mbstowcs_free(st_dir);

        resources_get_sprintf("FSDevice%dConvertP00", (void *)&n, num);
        CheckDlgButton(hwnd, IDC_TOGGLE_READP00,
                       n ? BST_CHECKED : BST_UNCHECKED);

        resources_get_sprintf("FSDevice%dSaveP00", (void *)&n, num);
        CheckDlgButton(hwnd, IDC_TOGGLE_WRITEP00,
                       n ? BST_CHECKED : BST_UNCHECKED);

        resources_get_sprintf("FSDevice%dHideCBMFiles", (void *)&n, num);
        CheckDlgButton(hwnd, IDC_TOGGLE_HIDENONP00,
                       n ? BST_CHECKED : BST_UNCHECKED);

        resources_get_sprintf("AttachDevice%dReadonly",
                              (void *)&n, num);
        CheckDlgButton(hwnd, IDC_TOGGLE_ATTACH_READONLY,
                       n ? BST_CHECKED : BST_UNCHECKED);

        resources_get_sprintf("FileSystemDevice%d", (void *)&devtype, num);
        switch (devtype) {
          case ATTACH_DEVICE_FS:
            if (disk_image != NULL)
                n = IDC_SELECTDISK;
            else
                n = IDC_SELECTDIR;
            break;
#ifdef HAVE_OPENCBM
          case ATTACH_DEVICE_REAL:
            n = IDC_SELECTREAL;
            break;
#endif
          default:
            n = IDC_SELECTNONE;
        }

#ifdef HAVE_OPENCBM
        CheckRadioButton(hwnd, IDC_SELECTDISK, IDC_SELECTREAL, n);
#else
        CheckRadioButton(hwnd, IDC_SELECTDISK, IDC_SELECTDIR, n);
#endif
        enable_controls_for_disk_device_type(hwnd, n);

        if (iec_available_busses() & IEC_BUS_IEC) {
            resources_get_sprintf("IECDevice%d", (void *)&n, num);
            CheckDlgButton(hwnd, IDC_TOGGLE_USEIECDEVICE,
                           n ? BST_CHECKED : BST_UNCHECKED);
        } else {
            CheckDlgButton(hwnd, IDC_TOGGLE_USEIECDEVICE, BST_UNCHECKED);
            ShowWindow(GetDlgItem(hwnd, IDC_TOGGLE_USEIECDEVICE), FALSE);
        }
        
        enable_controls(hwnd);
    }
}

static BOOL store_dialog_results(HWND hwnd, unsigned int num)
{
    char s[MAX_PATH];
    TCHAR st[MAX_PATH];
    int devtype = ATTACH_DEVICE_NONE;

    if (IsDlgButtonChecked(hwnd, IDC_SELECTDISK) == BST_CHECKED) {
        GetDlgItemText(hwnd, IDC_DISKIMAGE, st, MAX_PATH);
        system_wcstombs(s, st, MAX_PATH);
        if (file_system_attach_disk(num, s) < 0 ) {
            ui_error("Cannot attach specified file");
            return 0;
        }
    } else {
        if ((IsDlgButtonChecked(hwnd, IDC_SELECTDIR) == BST_CHECKED) &&
            file_system_get_disk_name(num))
            file_system_detach_disk(num);
    }

    if (iec_available_busses() & IEC_BUS_IEC)
        resources_set_sprintf("IECDevice%d", 
                              (resource_value_t)(IsDlgButtonChecked(hwnd,
                              IDC_TOGGLE_USEIECDEVICE)==BST_CHECKED), num);

    if (IsDlgButtonChecked(hwnd, IDC_SELECTDISK) == BST_CHECKED
        || IsDlgButtonChecked(hwnd, IDC_SELECTDIR) == BST_CHECKED)
        devtype = ATTACH_DEVICE_FS;
#ifdef HAVE_OPENCBM
    if (IsDlgButtonChecked(hwnd, IDC_SELECTREAL) == BST_CHECKED)
        devtype = ATTACH_DEVICE_REAL;
#endif
    resources_set_sprintf("FileSystemDevice%d", (resource_value_t)devtype,
                          num);

    resources_set_sprintf("FSDevice%dConvertP00", 
                          (resource_value_t)(IsDlgButtonChecked(hwnd,
                          IDC_TOGGLE_READP00) == BST_CHECKED), num);
    resources_set_sprintf("FSDevice%dSaveP00", 
                          (resource_value_t)(IsDlgButtonChecked(hwnd,
                          IDC_TOGGLE_WRITEP00) == BST_CHECKED), num);
    resources_set_sprintf("FSDevice%dHideCBMFiles", 
                          (resource_value_t)(IsDlgButtonChecked(hwnd,
                          IDC_TOGGLE_HIDENONP00) == BST_CHECKED), num);
    resources_set_sprintf("AttachDevice%dReadonly", 
                          (resource_value_t)(IsDlgButtonChecked(hwnd,
                          IDC_TOGGLE_ATTACH_READONLY) == BST_CHECKED), num);

    GetDlgItemText(hwnd, IDC_DIR, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_sprintf("FSDevice%dDir", (resource_value_t)s, num);

    return 1;
}

static BOOL CALLBACK dialog_proc(unsigned int num, HWND hwnd, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
      case WM_INITDIALOG:
        init_dialog(hwnd, num);
        return TRUE;

      case WM_NOTIFY:
        {
            NMHDR *nmhdr = (NMHDR *)(lparam);

            switch (nmhdr->code) {
              case PSN_APPLY:
                SetWindowLong(hwnd, DWL_MSGRESULT, 
                              store_dialog_results(hwnd, num)
                              ? PSNRET_NOERROR : PSNRET_INVALID);
                return TRUE;
            }
            break;
        }

      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDC_SELECTDIR:
          case IDC_SELECTDISK:
          case IDC_SELECTNONE:
            enable_controls_for_disk_device_type(hwnd, LOWORD(wparam));
            break;
  	  case IDC_TOGGLE_USEIECDEVICE:
            enable_controls(hwnd);
            break;
          case IDC_BROWSEDISK:
            {
                TCHAR *st_name;

                st_name = uilib_select_file(hwnd, TEXT("Attach disk image"),
                    UILIB_FILTER_ALL | UILIB_FILTER_DISK | UILIB_FILTER_ZIP,
                    UILIB_SELECTOR_TYPE_FILE_LOAD, UILIB_SELECTOR_STYLE_DISK);

                if (st_name != NULL) {
                    SetDlgItemText(hwnd, IDC_DISKIMAGE, st_name);
                    lib_free(st_name);
                }
            }
            break;
          case IDC_AUTOSTART:
            {
                TCHAR *st_name;

                st_name = uilib_select_file(hwnd, TEXT("Autostart disk image"),
                    UILIB_FILTER_ALL | UILIB_FILTER_DISK | UILIB_FILTER_ZIP,
                    UILIB_SELECTOR_TYPE_FILE_LOAD, UILIB_SELECTOR_STYLE_DISK);

                if (st_name != NULL) {
                    char *name;

                    SetDlgItemText(hwnd, IDC_DISKIMAGE, st_name);
                    name = system_wcstombs_alloc(st_name);
                    if (autostart_autodetect(name, "*", 0, AUTOSTART_MODE_RUN)
                        < 0)
                        ui_error("Cannot autostart specified file.");
                    system_wcstombs_free(name);
                    lib_free(st_name);
                }
            }
            break;
          case IDC_BROWSEDIR:
            {
                BROWSEINFO bi;
                TCHAR st[MAX_PATH];
                LPITEMIDLIST idlist;

                bi.hwndOwner = hwnd;
                bi.pidlRoot = NULL;
                bi.pszDisplayName = st;
                bi.lpszTitle = TEXT("Select file system directory");
                bi.ulFlags = 0;
                bi.lpfn = NULL;
                bi.lParam = 0;
                bi.iImage = 0;
                if ((idlist = SHBrowseForFolder(&bi)) != NULL) {
                    SHGetPathFromIDList(idlist, st);
                    LocalFree(idlist);
                    /*
                    If a root directory is selected, \ is appended
                    and has to be deleted.
                    */
                    if (st[_tcslen(st) - 1] == '\\')
                        st[_tcslen(st) - 1] = '\0';
                    SetDlgItemText(hwnd, IDC_DIR, st);
                }
            }
            break;
        }
        return TRUE;
    }
    return FALSE;
}

#define _CALLBACK(num)                                            \
static BOOL CALLBACK callback_##num(HWND dialog, UINT msg,        \
                                    WPARAM wparam, LPARAM lparam) \
{                                                                 \
    return dialog_proc(num, dialog, msg, wparam, lparam);         \
}

_CALLBACK(8)
_CALLBACK(9)
_CALLBACK(10)
_CALLBACK(11)

/* -------------------------------------------------------------------------- */
/*                           Printers (Userport, 4-5)                         */
/* -------------------------------------------------------------------------- */

static char *printertextdevice[3] = { NULL, NULL, NULL };

static const TCHAR *ui_printer[] =
{
    TEXT("None"),
    TEXT("File system"),
#ifdef HAVE_OPENCBM
    TEXT("Real IEC device"),
#endif
    NULL
};

static const TCHAR *ui_printer_driver[] =
{
    TEXT("ASCII"),
    TEXT("MPS803"),
    TEXT("NL10"),
    NULL
};

static const char *ui_printer_driver_ascii[] =
{
    "ascii",
    "mps803",
    "nl10",
    NULL
};

static const char *ui_printer_output[] =
{
    "Text",
    "Graphics",
    NULL
};

static const char *ui_printer_output_ascii[] =
{
    "text",
    "graphics",
    NULL
};

static const TCHAR *ui_printer_text_device[] =
{
    TEXT("1"),
    TEXT("2"),
    TEXT("3"),
    NULL
};

static void enable_printer_controls(unsigned int num, HWND hwnd)
{
    int res_value, is_enabled;
    int drive_true_emulation, virtual_device_traps;
    BOOL haveIECDevice;
  
    resources_get_value("DriveTrueEmulation", (void *)&drive_true_emulation);
    resources_get_value("VirtualDevices", (void *)&virtual_device_traps);
    haveIECDevice = IsDlgButtonChecked(hwnd,
                                       IDC_PRINTER_USEIECDEVICE) == BST_CHECKED;
  
    if (num > 0 && ((drive_true_emulation || !virtual_device_traps)
        && !haveIECDevice)) {
        EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_TYPE), FALSE);
        SendMessage(GetDlgItem(hwnd, IDC_PRINTER_TYPE),
                    CB_SETCURSEL, 0, 0);
    } else
        EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_TYPE), TRUE);

    res_value = SendMessage(GetDlgItem(hwnd, IDC_PRINTER_TYPE),
                            CB_GETCURSEL, 0, 0);
  
    is_enabled = res_value == PRINTER_DEVICE_FS;
  
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_FORMFEED), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_DRIVER), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_TEXTOUT), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE1_NAME), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE2_NAME), is_enabled);
    EnableWindow(GetDlgItem(hwnd, IDC_PRINTER_OUTPUT_FILE3_NAME), is_enabled);
}

static void init_printer_dialog(unsigned int num, HWND hwnd)
{
    HWND printer_hwnd;
    int i, res_value, res_value_loop;
    char  printer_name[30];
    const char *res_string;
    int current = 0;

    if (num == 0)
        sprintf(printer_name, "PrinterUserport");
    else
        sprintf(printer_name, "Printer%d", num);

    resources_get_value(printer_name, (void *)&res_value);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTER_TYPE);
    for (res_value_loop = 0; ui_printer[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer[res_value_loop]);
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    resources_get_sprintf("%sDriver", (void *)&res_string, printer_name);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTER_DRIVER);
    for (res_value_loop = 0; ui_printer_driver[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer_driver[res_value_loop]);
        if (!strcmp(ui_printer_driver_ascii[res_value_loop], res_string))
            current = res_value_loop;
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)current, 0);

    resources_get_sprintf("%sOutput", (void *)&res_string, printer_name);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTER_OUTPUT);
    for (res_value_loop = 0; ui_printer_output[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer_output[res_value_loop]);
        if (!strcmp(ui_printer_output_ascii[res_value_loop], res_string))
            current = res_value_loop;
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)current, 0);

    resources_get_sprintf("%sTextDevice", (void *)&res_value, printer_name);
    printer_hwnd = GetDlgItem(hwnd, IDC_PRINTER_TEXTOUT);
    for (res_value_loop = 0; ui_printer_text_device[res_value_loop];
        res_value_loop++) {
        SendMessage(printer_hwnd, CB_ADDSTRING, 0,
                    (LPARAM)ui_printer_text_device[res_value_loop]);
    }
    SendMessage(printer_hwnd, CB_SETCURSEL, (WPARAM)res_value, 0);

    if (num > 0 && (iec_available_busses() & IEC_BUS_IEC)) {
        resources_get_sprintf("IECDevice%d", (void *)&res_value, num);
        CheckDlgButton(hwnd, IDC_PRINTER_USEIECDEVICE,
                       res_value ? BST_CHECKED : BST_UNCHECKED);
    } else {
        ShowWindow(GetDlgItem(hwnd, IDC_PRINTER_USEIECDEVICE), FALSE);
        CheckDlgButton(hwnd, IDC_PRINTER_USEIECDEVICE, BST_UNCHECKED);
    }
    
    for (i = 0; i < 3; i++) {
        resources_get_sprintf("PrinterTextDevice%d", (void *)&res_string, i+1);
        if (res_string)
            strncpy(printertextdevice[i], res_string, MAX_PATH);
    }

    enable_printer_controls(num, hwnd);
}

static BOOL store_printer_dialog_results(HWND hwnd, unsigned int num)
{
    char printer_name[30];
  
    if (num == 0)
        sprintf(printer_name, "PrinterUserport");
    else
        sprintf(printer_name, "Printer%d", num);

    resources_set_value(printer_name, 
                        (resource_value_t)SendMessage(GetDlgItem(hwnd,
                        IDC_PRINTER_TYPE), CB_GETCURSEL, 0, 0));

    resources_set_sprintf("%sDriver", (resource_value_t)
                        ui_printer_driver_ascii[SendMessage(GetDlgItem(hwnd,
                        IDC_PRINTER_DRIVER), CB_GETCURSEL, 0, 0)],
                        printer_name);

    resources_set_sprintf("%sOutput", (resource_value_t)
                          ui_printer_output_ascii[SendMessage(GetDlgItem(hwnd,
                          IDC_PRINTER_OUTPUT), CB_GETCURSEL, 0, 0)],
                          printer_name);

    resources_set_sprintf("%sTextDevice", (resource_value_t)
                          SendMessage(GetDlgItem(hwnd, IDC_PRINTER_TEXTOUT),
                          CB_GETCURSEL, 0, 0), printer_name);
  
    resources_set_value("PrinterTextDevice1", (void *)printertextdevice[0]);
    resources_set_value("PrinterTextDevice2", (void *)printertextdevice[1]);
    resources_set_value("PrinterTextDevice3", (void *)printertextdevice[2]);

    if (num > 0 && (iec_available_busses() & IEC_BUS_IEC))
        resources_set_sprintf("IECDevice%d", (resource_value_t)
                          (IsDlgButtonChecked(hwnd,
                          IDC_PRINTER_USEIECDEVICE)==BST_CHECKED), num);

    return 1;
}

static BOOL CALLBACK printer_dialog_proc(unsigned int num, HWND hwnd, UINT msg, 
                                         WPARAM wparam, LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDC_PRINTER_TYPE:
          case IDC_PRINTER_USEIECDEVICE:
            enable_printer_controls(num, hwnd);
            break;
          case IDC_PRINTER_FORMFEED:
            switch(num) {
              case 4:
                printer_formfeed(0);
                break;
              case 5:
                printer_formfeed(1);
                break;
              case 0:
                printer_formfeed(2);
                break;
            }
            break;
        }
        return FALSE;

      case WM_NOTIFY:
        {
            NMHDR FAR *nmhdr = (NMHDR FAR *)(lparam);

            switch (nmhdr->code) {
              case PSN_APPLY:
                SetWindowLong(hwnd, DWL_MSGRESULT, 
                              store_printer_dialog_results(hwnd, num)
                              ? PSNRET_NOERROR : PSNRET_INVALID);
                return TRUE;

              case PSN_SETACTIVE:
                {
                    TCHAR *st;

                    st = system_mbstowcs_alloc(printertextdevice[0]);
                    SetDlgItemText(hwnd, IDC_PRINTER_OUTPUT_FILE1_NAME, st);
                    system_mbstowcs_free(st);
                    st = system_mbstowcs_alloc(printertextdevice[1]);
                    SetDlgItemText(hwnd, IDC_PRINTER_OUTPUT_FILE2_NAME, st);
                    system_mbstowcs_free(st);
                    st = system_mbstowcs_alloc(printertextdevice[2]);
                    SetDlgItemText(hwnd, IDC_PRINTER_OUTPUT_FILE3_NAME, st);
                    system_mbstowcs_free(st);
                    return TRUE;
                }
              
              case PSN_KILLACTIVE:
                {
                    TCHAR st[MAX_PATH];

                    GetDlgItemText(hwnd, IDC_PRINTER_OUTPUT_FILE1_NAME,
                                   st, MAX_PATH);
                    system_wcstombs(printertextdevice[0], st, MAX_PATH);
                    GetDlgItemText(hwnd, IDC_PRINTER_OUTPUT_FILE2_NAME,
                                   st, MAX_PATH);
                    system_wcstombs(printertextdevice[1], st, MAX_PATH);
                    GetDlgItemText(hwnd, IDC_PRINTER_OUTPUT_FILE3_NAME,
                                   st, MAX_PATH);
                    system_wcstombs(printertextdevice[2], st, MAX_PATH);
                    return TRUE;
                }
            }
            break;
        }
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_printer_dialog(num, hwnd);
        return TRUE;
    }

    return FALSE;
}


#define _CALLBACK_PRINTER(num)                                    \
static BOOL CALLBACK callback_##num(HWND dialog, UINT msg,        \
                                    WPARAM wparam, LPARAM lparam) \
{                                                                 \
    return printer_dialog_proc(num, dialog, msg, wparam, lparam); \
}

_CALLBACK_PRINTER(0)
_CALLBACK_PRINTER(4)
_CALLBACK_PRINTER(5)

/* -------------------------------------------------------------------------- */
/*                               Main Dialog                                  */
/* -------------------------------------------------------------------------- */

void ui_peripheral_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[7];
    PROPSHEETHEADER psh;
    int i, no_of_drives, no_of_printers;

    for (i = 0; i < 3; i++ ) {
        printertextdevice[i] = (char *)lib_malloc(MAX_PATH);
        strcpy(printertextdevice[i], "");
    }

    no_of_drives = 4;
    no_of_printers = 2;

    if (have_printer_userport < 0)
        have_printer_userport = (resources_touch("PrinterUserport")) < 0
                                ? 0 : 1;
    if (have_printer_userport)
        no_of_printers++;

    for (i = 0; i < no_of_printers; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
        psp[i].pszTemplate = MAKEINTRESOURCE(IDD_PRINTER_SETTINGS_DIALOG);
        psp[i].pszIcon = NULL;
#else
        psp[i].DUMMYUNIONNAME.pszTemplate
            = MAKEINTRESOURCE(IDD_PRINTER_SETTINGS_DIALOG);
        psp[i].u2.pszIcon = NULL;
#endif
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    for (i = 0; i < no_of_drives; i++) {
        psp[no_of_printers+i].dwSize = sizeof(PROPSHEETPAGE);
        psp[no_of_printers+i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[no_of_printers+i].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
        psp[no_of_printers+i].pszTemplate
            = MAKEINTRESOURCE(IDD_DISKDEVICE_DIALOG);
        psp[no_of_printers+i].pszIcon = NULL;
#else
        psp[no_of_printers+i].DUMMYUNIONNAME.pszTemplate
            = MAKEINTRESOURCE(IDD_DISKDEVICE_DIALOG);
        psp[no_of_printers+i].u2.pszIcon = NULL;
#endif
        psp[no_of_printers+i].lParam = 0;
        psp[no_of_printers+i].pfnCallback = NULL;
    }

    if (have_printer_userport) {
        psp[0].pfnDlgProc = callback_0;
        psp[0].pszTitle = TEXT("Printer Userport");
        i = 1;
    } else
        i = 0;

    psp[i+0].pfnDlgProc = callback_4;
    psp[i+0].pszTitle = TEXT("Printer 4");
    psp[i+1].pfnDlgProc = callback_5;
    psp[i+1].pszTitle = TEXT("Printer 5");
    psp[i+2].pfnDlgProc = callback_8;
    psp[i+2].pszTitle = TEXT("Drive 8");
    psp[i+3].pfnDlgProc = callback_9;
    psp[i+3].pszTitle = TEXT("Drive 9");
    psp[i+4].pfnDlgProc = callback_10;
    psp[i+4].pszTitle = TEXT("Drive 10");
    psp[i+5].pfnDlgProc = callback_11;
    psp[i+5].pszTitle = TEXT("Drive 11");

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = TEXT("Peripheral Settings");
    psh.nPages = no_of_drives + no_of_printers;
#ifdef _ANONYMOUS_UNION
    psh.pszIcon = NULL;
    psh.nStartPage = 0;
    psh.ppsp = psp;
#else
    psh.DUMMYUNIONNAME.pszIcon = NULL;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
#endif
    psh.pfnCallback = NULL;

    PropertySheet(&psh);

    for( i = 0; i < 3; i++ )
      lib_free(printertextdevice[i]);
}

