/*
 * uivicset.c - Implementation of VIC settings dialog box.
 *
 * Written by
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

#include "intl.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "uilib.h"
#include "uivicset.h"
#include "winmain.h"


enum {
    BLOCK_0 = 1,
    BLOCK_1 = 1 << 1,
    BLOCK_2 = 1 << 2,
    BLOCK_3 = 1 << 3,
    BLOCK_5 = 1 << 5
};

enum {
    MEM_NONE = 0,
    MEM_3K = BLOCK_0,
    MEM_8K = BLOCK_1,
    MEM_16K = BLOCK_1 | BLOCK_2,
    MEM_24K = BLOCK_1 | BLOCK_2 | BLOCK_3,
    MEM_ALL = BLOCK_0 | BLOCK_1 | BLOCK_2 | BLOCK_3 | BLOCK_5
};


int block0,block1,block2,block3,block5;
int current_config;

static void update_block_checkboxes(HWND hwnd)
{
    CheckDlgButton(hwnd, IDC_VIC_MEMORY_BLOCK0,
                   block0 ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_VIC_MEMORY_BLOCK1,
                   block1 ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_VIC_MEMORY_BLOCK2,
                   block2 ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_VIC_MEMORY_BLOCK3,
                   block3 ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_VIC_MEMORY_BLOCK5,
                   block5 ? BST_CHECKED : BST_UNCHECKED);
}


static void update_blocks(int mem_model)
{
    block0 = (mem_model & BLOCK_0) ? 1 : 0;
    block1 = (mem_model & BLOCK_1) ? 1 : 0;
    block2 = (mem_model & BLOCK_2) ? 1 : 0;
    block3 = (mem_model & BLOCK_3) ? 1 : 0;
    block5 = (mem_model & BLOCK_5) ? 1 : 0;
}


static void update_config(HWND hwnd)
{
    int memory_model;

    current_config =
        (block0 ? BLOCK_0 : 0) |
        (block1 ? BLOCK_1 : 0) |
        (block2 ? BLOCK_2 : 0) |
        (block3 ? BLOCK_3 : 0) |
        (block5 ? BLOCK_5 : 0);

    switch (current_config) {
      case MEM_NONE:
        memory_model = IDC_VIC_NOEXPANSION;
        break;
      case MEM_3K:
        memory_model = IDC_VIC_3KEXPANSION;
        break;
      case MEM_8K:
        memory_model = IDC_VIC_8KEXPANSION;
        break;
      case MEM_16K:
        memory_model = IDC_VIC_16KEXPANSION;
        break;
      case MEM_24K:
        memory_model = IDC_VIC_24KEXPANSION;
        break;
      case MEM_ALL:
        memory_model = IDC_VIC_FULLEXPANSION;
        break;
      default:
        memory_model = IDC_VIC_CUSTOMEXPANSION;
    }
    CheckRadioButton(hwnd,IDC_VIC_NOEXPANSION,
                     IDC_VIC_CUSTOMEXPANSION,memory_model);
    update_block_checkboxes(hwnd);

}


static void init_dialog(HWND hwnd)
{
    resources_get_value("RAMBlock0", (void *) &block0);
    resources_get_value("RAMBlock1", (void *) &block1);
    resources_get_value("RAMBlock2", (void *) &block2);
    resources_get_value("RAMBlock3", (void *) &block3);
    resources_get_value("RAMBlock5", (void *) &block5);
    
    update_config(hwnd);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDOK:
            resources_set_value("RAMBlock0", (resource_value_t)block0);
            resources_set_value("RAMBlock1", (resource_value_t)block1);
            resources_set_value("RAMBlock2", (resource_value_t)block2);
            resources_set_value("RAMBlock3", (resource_value_t)block3);
            resources_set_value("RAMBlock5", (resource_value_t)block5);
          case IDCANCEL:
            EndDialog(hwnd,0);
            break;
          case IDC_VIC_NOEXPANSION:
            update_blocks(MEM_NONE);
            break;
          case IDC_VIC_3KEXPANSION:
            update_blocks(MEM_3K);
            break;
          case IDC_VIC_8KEXPANSION:
            update_blocks(MEM_8K);
            break;
          case IDC_VIC_16KEXPANSION:
            update_blocks(MEM_16K);
            break;
          case IDC_VIC_24KEXPANSION:
            update_blocks(MEM_24K);
            break;
          case IDC_VIC_FULLEXPANSION:
            update_blocks(MEM_ALL);
            break;
          case IDC_VIC_MEMORY_BLOCK0:
            block0 = 1 - block0;
            break;
          case IDC_VIC_MEMORY_BLOCK1:
            block1 = 1 - block1;
            break;
          case IDC_VIC_MEMORY_BLOCK2:
            block2 = 1 - block2;
            break;
          case IDC_VIC_MEMORY_BLOCK3:
            block3 = 1 - block3;
            break;
          case IDC_VIC_MEMORY_BLOCK5:
            block5 = 1 - block5;
            break;
          default:
            return FALSE;
        }
        update_config(hwnd);
        return TRUE;

      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}

void ui_vic_settings_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, MAKEINTRESOURCE(intl_translate(IDD_VIC_SETTINGS_DIALOG)), hwnd,
              dialog_proc);
}

