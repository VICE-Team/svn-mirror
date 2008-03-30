/*
 * dlg-fileio.c - The fileio-dialog.
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

#define INCL_DOSPROCESS        // DosSleep
#define INCL_GPILCIDS          // GpiLoadFont
#define INCL_GPIPRIMITIVES     // GpiSetCharSet
#define INCL_WINSTDFILE        // FILEDLG
#define INCL_WINDIALOGS        // WinSendDlgItemMsg
#define INCL_WINBUTTONS        // BS_*, BM_*
#define INCL_WINWINDOWMGR      // QWL_USER
#define INCL_WINLISTBOXES      // LS_*, LIT_*
#define INCL_WINENTRYFIELDS    // EN_*
#define INCL_WINPOINTERS       // WinLoadPointer
#define INCL_WININPUT          // WinQueryFocus
#define INCL_WINFRAMEMGR       // WM_QUERYFOCUSCHAIN
#define INCL_WINMENUS          // OWNERITEM
#include "vice.h"
#include "videoarch.h"         // video_canvas_t

#include <stdlib.h>            // free
#include <string.h>            // strcpy
#include <direct.h>            // chdir

#include "log.h"
#include "mem.h"               // mem_romset_resource_list
#include "tape.h"              // tape_image_attach
#include "utils.h"             // xmsprintf
#include "attach.h"            // file_system_attach_disk
#include "romset.h"            // romset_*
#include "machine.h"           // machine_read/write_snapshot
#include "dialogs.h"           // WinLbox*
#include "archdep.h"           // archdep_boot_path
#include "fliplist.h"          // flip_*
#include "charset.h"           // a2p, p2a
#include "resources.h"         // resources_set_value
#include "autostart.h"         // autostart_autodetect
#include "cartridge.h"         // cartridge_attach_image
#include "interrupt.h"         // interrupt_maincpu_trigger_trap
#include "screenshot.h"        // screenshot_canvas_save
#include "imagecontents.h"     // image_contents

#include "dlg-fileio.h"
#include "snippets\pmwin2.h"

#if defined __X64__ || defined __X128__ || defined __XVIC__
static const char *VIDEO_PALETTE="PaletteFile";
#else
//#if defined __XPET__ || defined __XCBM__
static const char *VIDEO_PALETTE="CrtcPaletteFile";
#endif

struct _filter
{
    char *ext;
    char *desc;
};

typedef struct _filter filter_t;

struct _subaction
{
    char     *action;
    filter_t *filter;
};

typedef struct _subaction subaction_t;

struct _action
{
    char        *type;
    subaction_t *subact;
    int          enabled;             // enable subaction?
};

typedef struct _action action_t;

static filter_t FilterDisk[] = {
    {"*.d64*; *.d71*; *.d80*; *.d81.*; *.d82*; *.g64*; *.x64*", "All Disk Images"          },
    {"*.d64*; *.g64*; *.x64*",                                  "All 1541 Images"          },
    {"*.d64*",         "1541"               },
    {"*.g64*",         "1541"               },
    {"*.x64*",         "1541"               },
    {"*.d71*",         "1571"               },
    {"*.d80*",         "8050"               },
    {"*.d81*",         "1581"               },
    {"*.d82*",         "8250"               },
    {NULL}
};

static filter_t FilterTape[] = {
    {"*.t64*; *.tap*", "All Tape Images"    },
    {"*.t64*",         "T64"                },
    {"*.tap*",         "Raw 1531 Tape File" },
    {NULL}
};

static filter_t FilterCart[] = {
    {"*.crt; *.bin",   "All Cartridge Images" },
    {"*.crt",          "CRT"                  },
    {"*.bin",          "BIN"                  },
    {NULL}
};

static filter_t FilterPal[]     = {{"*.vpl", "Vice/2 Color Palette"     }, {NULL}};
static filter_t FilterVsf[]     = {{"*.vsf", "Vice/2 Snapshot File"     }, {NULL}};
static filter_t FilterKbd[]     = {{"*.vkm", "Vice/2 Keyboard Map"      }, {NULL}};
static filter_t FilterRomSet[]  = {{"*.vrs", "Vice/2 Rom Set"           }, {NULL}};
static filter_t FilterPng[]     = {{"*.png", "Portable Network Graphic" }, {NULL}};
static filter_t FilterBmp[]     = {{"*.bmp", "Bitmap"                   }, {NULL}};
static filter_t FilterCfg[]     = {{"*.cfg", "Vice/2 Configuration"     }, {NULL}};
static filter_t FilterFlip[]    = {{"*.vfl",    "Vice/2 Fliplist"       }, {NULL}};
static filter_t FilterKernal[]  = {{"kernal*",  "Kernal ROM"            }, {NULL}};
static filter_t FilterBasic[]   = {{"basic*",   "Basic ROM"             }, {NULL}};
static filter_t FilterChargen[] = {{"chargen*", "Character ROM"         }, {NULL}};
static filter_t FilterZ80[]     = {{"z80bios*", "Z80 BIOS"              }, {NULL}};
static filter_t Filter1541[]    = {{"dos1541*", "1541 ROM"              }, {NULL}};
static filter_t Filter15412[]   = {{"d1541II*", "1541-II ROM"           }, {NULL}};
static filter_t Filter1571[]    = {{"dos1571*", "1571 ROM"              }, {NULL}};
static filter_t Filter1581[]    = {{"dos1581*", "1581 ROM"              }, {NULL}};
static filter_t Filter2031[]    = {{"dos2031*", "2031 ROM"              }, {NULL}};
static filter_t Filter1001[]    = {{"dos1001*", "1001 ROM"              }, {NULL}};
static filter_t Filter2040[]    = {{"dos2040*", "2040 ROM"              }, {NULL}};
static filter_t Filter3040[]    = {{"dos3040*", "3040 ROM"              }, {NULL}};
static filter_t Filter4040[]    = {{"dos4040*", "4040 ROM"              }, {NULL}};

static subaction_t SubDisk[] = {
    { "as Diskette in Drive #8",  FilterDisk },
    { "as Diskette in Drive #9",  FilterDisk },
    { "as Diskette in Drive #10", FilterDisk },
    { "as Diskette in Drive #11", FilterDisk },
    { NULL }
};

static subaction_t SubFlip[] = {
    { "of Drive #8",  FilterFlip },
    { "of Drive #9",  FilterFlip },
    { NULL }
};

static subaction_t SubTape[]   = {{ "as Tape to Datasette",    FilterTape }, {NULL}};
static subaction_t SubKbd[]    = {{ "as new keyboard mapping", FilterKbd },  {NULL}};
static subaction_t SubCfg[]    = {{ "as new configuration",    FilterCfg },  {NULL}};
static subaction_t SubCart2[]  = {{ "as cartridge image",      FilterCart }, {NULL}};
static subaction_t SubVsf[]    = {{ "as Vice/2 snapshot file", FilterVsf },  {NULL}};
static subaction_t SubRomSet[] = {{ "as Vice/2 rom set",       FilterRomSet},{NULL}};

static subaction_t SubScr[] = {
    { "as Portable Network Graphic (PiNG)", FilterPng },
    { "as Native Bitmap (BMP)",             FilterBmp },
    {NULL}
};

static subaction_t SubPal[] = {
    { "as new color palette",    FilterPal },
#ifdef __X128__
    { "as new VDC palette",      FilterPal },
#endif
    {NULL}
};

#if defined __X64__ || defined __X128__
static subaction_t SubCart[] = {
    { "as Generic Cartridge",                  FilterCart },
    { "as Generic 8kB Cartridge",              FilterCart },
    { "as Generic 16kB Cartridge",             FilterCart },
    { "as 32kB Action Replay Cartridge",       FilterCart },
    { "as 32kB Atomic Power Cartridge",        FilterCart },
    { "as 8kB Epyx Fastloader Cartridge",      FilterCart },
    { "as 32kB Super Snapshot Cartridge",      FilterCart },
    { "as 64kB Super Snapshot Cartridge",      FilterCart },
    { "as 16kB Westermann learning Cartridge", FilterCart },
    { "as CBM IEEE488 Cartridge",              FilterCart },
    { "as IDE64 Cartridge",                    FilterCart },
    { NULL }
};
#endif
#ifdef __XPET__
static subaction_t SubExtRom[] = {
    { "to memory address $9000", FilterCart },
    { "to memory address $A000", FilterCart },
    { "to memory address $B000", FilterCart },
    { NULL }
};
#endif
#ifdef __XCBM__
static subaction_t SubCbmCart[] = {
    { "to memory address $1000", FilterCart },
    { "to memory address $2000", FilterCart },
    { "to memory address $4000", FilterCart },
    { "to memory address $6000", FilterCart },
    { NULL }
};
#endif
#ifdef __X128__
static subaction_t SubFuncRom[] = {
    { "internal", FilterCart },
    { "external", FilterCart },
    { NULL }
};
#endif

static subaction_t SubRom[] = {
    { "as Kernal ROM",        FilterKernal  },
    { "as Basic ROM",         FilterBasic   },
    { "as Character ROM",     FilterChargen },
    { "as 1541 ROM",          Filter1541    },
    { "as 1541-II ROM",       Filter15412   },
    { "as 1571 ROM",          Filter1571    },
    { "as 1581 ROM",          Filter1581    },
    { "as 2031 ROM",          Filter2031    },
    { "as 1001 ROM",          Filter1001    },
    { "as 2040 ROM",          Filter2040    },
    { "as 3040 ROM",          Filter3040    },
    { "as 4040 ROM",          Filter4040    },
#ifdef __X128__
    { "as Z80 BIOS",          FilterZ80     },
    { "as C64 Kernal ROM",    FilterKernal  },
    { "as C64 Basic ROM",     FilterBasic   },
    { "as C64 Character ROM", FilterChargen },
#endif
    { NULL }
};

struct _trapaction
{
    int pending;
    int rc;
    const char *path;
    const HWND hwnd;
    int (*execute)(struct _trapaction*);
};

typedef struct _trapaction trapaction_t;

static void check_extension(char path[CCHMAXPATH], const char *ext)
{
    const int plen = strlen(path);
    const int pext = strlen(ext);

    if (plen>pext)
    {
        if (!stricmp(path+plen-pext, ext))
            return;
    }

    if (plen+pext+2>CCHMAXPATH)
        return;

    strcat(path, ext);
}

static int load_snapshot(trapaction_t *act)
{
    return machine_read_snapshot(act->path);
}

static int save_snapshot(trapaction_t *act)
{
    return machine_write_snapshot(act->path, 1, 1);
}

static int save_screenshot(trapaction_t *act)
{
    // FIXME!
    const HWND owner = (HWND)WinDefFileDlgProc(act->hwnd, WM_QUERYFOCUSCHAIN,
                                               (MPARAM)QFC_NEXTINCHAIN, 0);

    video_canvas_t *canvas = (video_canvas_t*)WinQueryWindowPtr(owner, QWL_USER);

    char drv[4];
    strupr(strcpy(drv, act->path+strlen(act->path)-3));

    return screenshot_canvas_save(drv, act->path, canvas);
}

static void exec_func(ADDRESS addr, void *ptr)
{
    trapaction_t *act = (trapaction_t*)ptr;

    act->rc=act->execute(act);

    act->pending = FALSE;
}

static int trap(const HWND hwnd, int (*func)(trapaction_t*), const char *path)
{
    trapaction_t handle = { TRUE, 0, path, hwnd, func };

    const int paused = isEmulatorPaused();

    interrupt_maincpu_trigger_trap(exec_func, &handle);

    emulator_resume();
    while (handle.pending)
        DosSleep(1);

    if (paused)
        emulator_pause();

    return handle.rc;
}

static action_t LoadAction[] = {
    { "Attach Disk Image",         SubDisk,    TRUE  },
    { "Attache Tape Image",        SubTape,    FALSE },
    { "Load Fliplist",             SubFlip,    TRUE  },
    { "Load and Attach Fliplist",  SubFlip,    TRUE  },
    { "Load Snapshot",             SubVsf,     FALSE },
#ifdef __X128__
    { "Load Color Palette",        SubPal,     TRUE  },
#else
    { "Load Color Palette",        SubPal,     FALSE },
#endif
    { "Load Keyboard Map",         SubKbd,     FALSE },
    { "Load ROM Image",            SubRom,     TRUE  },
    { "Load ROM Set",              SubRomSet,  FALSE },
    { "Load Configuration File",   SubCfg,     FALSE },
#if defined __X64__ || defined __X128__
    { "Attach Cartridge Image",    SubCart,    TRUE  },
#endif
#ifdef __XPET__
    { "Load 4kB extension Rom",    SubExtRom,  TRUE  },
#endif
#ifdef __XCBM__
    { "Load 4kB Cartridge image",  SubCbmCart, TRUE },
#endif
#ifdef __X128__
    { "Load Function ROM Image",   SubFuncRom, TRUE },
#endif
    { NULL }
};

static BOOL FdmDoLoadAction(HWND hwnd, const char *szpath, int act, int sact)
{
    switch (act)
    {
    case 0:
        return file_system_attach_disk(sact+8, szpath);
    case 1:
        return tape_image_attach(1, szpath);
    case 2:
        return flip_load_list(sact+8, szpath, FALSE);
    case 3:
        return flip_load_list(sact+8, szpath, TRUE);
    case 4:
        return trap(hwnd, load_snapshot, szpath);
    case 5:
        switch (sact)
        {
        case 0:
            return resources_set_value(VIDEO_PALETTE, (resource_value_t)szpath);
#ifdef __X128__
        case 1:
            return resources_set_value("VDC_PaletteFile", (resource_value_t)szpath);
#endif
        }
        return -1;
    case 6: // rom img
        return resources_set_value("KeymapFile", (resource_value_t)szpath);
    case 7:
        switch (sact)
        {
        case 0:
            return resources_set_value("KernalName",    (resource_value_t)szpath);
        case 1:
            return resources_set_value("BasicName",     (resource_value_t)szpath);
        case 2:
            return resources_set_value("ChargenName",   (resource_value_t)szpath);
        case 3:
            return resources_set_value("DosName1541",   (resource_value_t)szpath);
        case 4:
            return resources_set_value("DosName154ii",  (resource_value_t)szpath);
        case 5:
            return resources_set_value("DosName1571",   (resource_value_t)szpath);
        case 6:
            return resources_set_value("DosName1581",   (resource_value_t)szpath);
        case 7:
            return resources_set_value("DosName2031",   (resource_value_t)szpath);
        case 8:
            return resources_set_value("DosName1001",   (resource_value_t)szpath);
        case 9:
            return resources_set_value("DosName2040",   (resource_value_t)szpath);
        case 10:
            return resources_set_value("DosName3040",   (resource_value_t)szpath);
        case 11:
            return resources_set_value("DosName4040",   (resource_value_t)szpath);
        case 12:
            return resources_set_value("Z80BiosName",   (resource_value_t)szpath);
        case 13:
            return resources_set_value("Kernal64Name",  (resource_value_t)szpath);
        case 14:
            return resources_set_value("Basic64Name",   (resource_value_t)szpath);
        case 15:
            return resources_set_value("Chargen64Name", (resource_value_t)szpath);
        }
        return -1;
    case 8:
        return romset_load(szpath);
    case 9:
        return resources_load(szpath);
#if defined __X64__ || defined __X128__
    case 10:
        switch (sact)
        {
        case 0:
            return cartridge_attach_image(CARTRIDGE_CRT, szpath);
        case 1:
            return cartridge_attach_image(CARTRIDGE_GENERIC_8KB, szpath);
        case 2:
            return cartridge_attach_image(CARTRIDGE_GENERIC_16KB, szpath);
        case 3:
            return cartridge_attach_image(CARTRIDGE_ACTION_REPLAY, szpath);
        case 4:
            return cartridge_attach_image(CARTRIDGE_ATOMIC_POWER, szpath);
        case 5:
            return cartridge_attach_image(CARTRIDGE_EPYX_FASTLOAD, szpath);
        case 6:
            return cartridge_attach_image(CARTRIDGE_SUPER_SNAPSHOT, szpath);
        case 7:
            return cartridge_attach_image(CARTRIDGE_SUPER_SNAPSHOT_V5, szpath);
        case 8:
            return cartridge_attach_image(CARTRIDGE_WESTERMANN, szpath);
        case 9:
            return cartridge_attach_image(CARTRIDGE_IEEE488, szpath);
        case 10:
            return cartridge_attach_image(CARTRIDGE_IDE64, szpath);
        }
        return -1;
#endif
#ifdef __XPET__
    case 10:
        switch (sact)
        {
        case 0:
            return resources_set_value("RomModule9Name", (resource_value_t)szpath);
        case 1:
            return resources_set_value("RomModuleAName", (resource_value_t)szpath);
        case 2:
            return resources_set_value("RomModuleBName", (resource_value_t)szpath);
        }
        return -1;
#endif
#ifdef __XCBM__
    case 10:
        switch (sact)
        {
        case 0:
            return resources_set_value("Cart1Name", (resource_value_t)szpath);
        case 1:
            return resources_set_value("Cart2Name", (resource_value_t)szpath);
        case 2:
            return resources_set_value("Cart4Name", (resource_value_t)szpath);
        case 3:
            return resources_set_value("Cart6Name", (resource_value_t)szpath);
        }
        return -1;
#endif
#ifdef __X128__
    case 11:
        switch (sact)
        {
        case 0:
            return resources_set_value("InternalFunctionName", (resource_value_t)szpath);
        case 1:
            return resources_set_value("ExternalFunctionName", (resource_value_t)szpath);
        }
        return -1;
#endif
    }
    return -1;
}

static action_t SaveAction[] = {
    { "Save Configuration File", SubCfg,   FALSE },
    { "Save Snapshot File",      SubVsf,   FALSE },
    { "Save Screenshot",         SubScr,   FALSE },
    { "Save Fliplist",           SubFlip,  TRUE  },
    // { "Save Color Palette",      SubPal,   FALSE },
    // { "Save Keyboard Map",       SubKbd,   FALSE },
    { "Save ROM Set",       SubRomSet,   FALSE },
#if defined __X64__ || defined __X128__
    { "Save Expert Cardridge",   SubCart2, FALSE },
#endif
    { NULL }
};

static BOOL FdmDoSaveAction(HWND hwnd, char *szpath, int act, int sact)
{
    switch (act)
    {
    case 0:
        check_extension(szpath, ".cfg");
        return resources_save(szpath);
    case 1:
        check_extension(szpath, ".vsf");
        return trap(hwnd, save_snapshot, szpath);
    case 2:
        switch (sact)
        {
        case 0:
            check_extension(szpath, ".png");
            break;
        case 1:
            check_extension(szpath, ".bmp");
            break;
        }
        return trap(hwnd, save_screenshot, szpath);
    case 3:
        check_extension(szpath, ".vfl");
        return flip_save_list(sact+8, szpath);
    case 4:
        return romset_dump(szpath, mem_romset_resources_list);
#if defined __X64__ || defined __X128__
    case 5:
        return cartridge_save_image(szpath);
#endif
    }
    return -1;
}

static void LboxFreeContents(HWND hwnd)
{
    int num = WinQueryLboxCount(hwnd);
    int idx;
    for (idx = 0; idx<num; idx++)
    {
        void *ptr = (void*)WinLboxItemHandle(hwnd, idx);
        if (!ptr)
            continue;

        WinLboxSetItemHandle(hwnd, idx, 0);
        free(ptr);
    }

    WinLboxEmpty(hwnd);
}

static void ShowContents(HWND hwnd, char *image_name)
{
    int   ascii;
    char *text;
    image_contents_t            *image;
    image_contents_screencode_t *line;
    image_contents_screencode_t *lines;

    //
    // delete listbox contents
    //
    const HWND hwnd2 = WinWindowFromID(hwnd, DID_CONTENTS_LB);
    LboxFreeContents(hwnd2);

    //
    // don't call the all the vice stuff if file doesn't exist
    //
    if (!util_file_exists_p(image_name))
        return;

    //
    // try to open as a disk or tape image
    //
    image = image_contents_read(IMAGE_CONTENTS_DISK, image_name, 0);

    if (!image)
        image = image_contents_read_tape(IMAGE_CONTENTS_TAPE, image_name, 0);

    if (!image)
        return;

    //
    // set the wanted font
    //
    WinSendMsg(hwnd2, LM_SETITEMHEIGHT, (MPARAM)9, 0);

    //
    // convert image contents to screencodes
    //
    lines = image_contents_to_screencode(image);

    //
    // Loop over all entries
    //
    {
        int idx = 0;
        line=lines;
        do {
            WinInsertLboxItem(hwnd2, LIT_END, "");
            WinLboxSetItemHandle(hwnd2, idx, (long)line);
            WinSetLboxItemText(hwnd2, idx, "");
            idx++;
        } while ((line=line->next));
    }

    //
    // free image structure
    //
    image_contents_destroy(image);
}

#define numfonts 8
const char fnames[numfonts][25] =
{
    "C64 Upper Case",
    "C64 Lower Case",
    "C128 Upper Case",
    "C128 Lower Case",
    "PET",
    "PET German",
    "VIC20 Upper Case",
    "VIC20 Lower Case"/*,
    "CBM-II 500 Upper Case",
    "CBM-II 500 Lower Case",
    "CBM-II 600 Upper Case",
    "CBM-II 600 Lower Case",
    "CBM-II 700 Upper Case",
    "CBM-II 700 Lower Case"
    */
};

static void LboxDrawLine(HWND hwnd, OWNERITEM *item, RECTL *rcl,
                  image_contents_screencode_t *line)
{
    const HPS hps  =item->hps;
    const int name =WinDlgLboxSelectedItem(hwnd, DID_FONTNAME_LB);
    //const int state=WinQueryButtonCheckstate(hwnd, DID_CBMFONT_CB);

    FATTRS font = { sizeof(FATTRS), 0, 0, "", 0, 0, 8, 8, 0, 0 };
    //sprintf(font.szFacename, "%s %s Case", fnames[name], state?"Upper":"Lower");
    //log_debug("Font: %s", fnames[name]);
    strcpy(font.szFacename, fnames[name]);

    GpiCreateLogFont  (hps, NULL, 1, &font);
    GpiSetCharSet     (hps, 1);
    GpiSetBackColor   (hps, item->fsState?CLR_DARKGRAY:CLR_WHITE);
    GpiSetColor       (hps, item->fsState?CLR_WHITE:CLR_DEFAULT);
    GpiCharStringPosAt(hps, (POINTL*)rcl, rcl, CHS_OPAQUE, line->length, line->line, NULL);
    GpiSetCharSet     (hps, 0);
    GpiDeleteSetId    (hps, 1);
}

static void ContentsUpdate(HWND hwnd)
{
    WinSendMsg(hwnd, WM_CONTROL,
               MPFROM2SHORT(DID_FILENAME_ED, EN_CHANGE),
               (MPARAM)WinWindowFromID(hwnd, DID_FILENAME_ED));
}

static BOOL GetFullFile(HWND hwnd, char szpath[CCHMAXPATH])
{
    int len;

    WinQueryDlgItemText(hwnd, DID_DIR_SELECTED,
                        CCHMAXPATH-1, szpath);

    len = strlen(szpath);

    if (len==CCHMAXPATH-1)
        return FALSE;

    WinQueryDlgItemText(hwnd, DID_FILENAME_ED,
                        CCHMAXPATH-len-1, szpath+len);
    return TRUE;
}

static void FillFBox(HWND hwnd)
{
    const HWND fbox  = WinWindowFromID(hwnd, DID_FFILTER_CB);
    const int nentry = WinDlgLboxSelectedItem(hwnd, DID_ACTION_CB);
    const int nsact  = WinDlgLboxSelectedItem(hwnd, DID_SUBACTION_CB);

    const FILEDLG     *fdlg   = (FILEDLG*)WinQueryWindowPtr(hwnd, QWL_USER);
    const action_t    *action = fdlg->fl & FDS_OPEN_DIALOG ? LoadAction : SaveAction;
    const subaction_t *subact = action[nentry].subact;
    const filter_t    *filter = subact[nsact].filter;

    int i=0;

    //
    // fill entries in comboboxes and select starting point
    //
    WinLboxEmpty(fbox);

    while (filter[i].ext)
    {
        int len = strlen(filter[i].desc)+strlen(filter[i].ext)+5;

        char *txt = malloc(len);
        sprintf(txt, "<%s> %s", filter[i].desc, filter[i].ext);
        WinInsertLboxItem(fbox, LIT_END, txt);
        free(txt);
        i++;
    }
    WinInsertLboxItem(fbox, LIT_END, "<All Files> ");
    WinSendMsg(fbox, LM_SELECTITEM, 0, (void*)TRUE);
}

static void FillSBox(HWND hwnd, int item)
{
    const HWND sbox  = WinWindowFromID(hwnd, DID_SUBACTION_CB);
    const int nentry = WinDlgLboxSelectedItem(hwnd, DID_ACTION_CB);

    const FILEDLG     *fdlg   = (FILEDLG*)WinQueryWindowPtr(hwnd, QWL_USER);
    const action_t    *action = fdlg->fl & FDS_OPEN_DIALOG ? LoadAction : SaveAction;
    const subaction_t *subact = action[nentry].subact;

    int i=0;

    //
    // fill entries in comboboxes and select starting point
    //
    WinLboxEmpty(sbox);

    while (subact[i].action)
        WinInsertLboxItem(sbox, LIT_END, subact[i++].action);
    WinSendMsg(sbox, LM_SELECTITEM, (MPARAM)item, (MPARAM)TRUE);

    WinEnableWindow(sbox, action[nentry].enabled);
}

static void FillABox(HWND hwnd, int item)
{
    const HWND      abox   = WinWindowFromID(hwnd, DID_ACTION_CB);
    const FILEDLG  *fdlg   = (FILEDLG*)WinQueryWindowPtr(hwnd, QWL_USER);
    const action_t *action = fdlg->fl & FDS_OPEN_DIALOG ? LoadAction : SaveAction;

    int i=0;
    while (action[i].type)
        WinInsertLboxItem(abox, LIT_END, action[i++].type);
    WinSendMsg(abox, LM_SELECTITEM, (MPARAM)item, (MPARAM)TRUE);
}

static void NewFilter(HWND hwnd)
{
    const HWND ebox = WinWindowFromID(hwnd, DID_FFILTER_CB);
    const HWND name = WinWindowFromID(hwnd, DID_FILENAME_ED);

    //
    // Get new selection
    //
    const int item = WinQueryLboxSelectedItem(ebox);
    const int len  = WinQueryLboxItemTextLength(ebox, item)+1;

    //
    // set corresponding text in entry field
    //
    char *txt = malloc(len);
    WinQueryLboxItemText(ebox, item, txt, len);
    WinSetWindowText(name, strrchr(txt, '>')+2);
    free(txt);

    //
    // set focus to entry field and simulate an Apply
    //
    WinSetFocus(HWND_DESKTOP, name);
    WinDefFileDlgProc(hwnd, WM_COMMAND, (MPARAM)DID_OK,
                      MPFROM2SHORT(CMDSRC_PUSHBUTTON, TRUE));
}

int GetLboxPath(HWND hwnd, int nr, char szpath[CCHMAXPATH])
{
    int len=0;

    int i;
    for (i=0; i<=nr; i++)
    {
        WinQueryLboxItemText(hwnd, i, (szpath+len), CCHMAXPATH-len-1);
        len = strlen(szpath);

        if (*(szpath+len-1)=='\\')
            continue;

        if (len==CCHMAXPATH-1) // should never happen
            return FALSE;

        strcat(szpath, "\\");
        len++;
    }
    return TRUE;
}

MRESULT EXPENTRY ViceFileDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        {
            FILEDLG *fdlg = (FILEDLG*)WinQueryWindowPtr(hwnd, QWL_USER);

            int i;
            char *szpath;

            const int action = fdlg->ulUser;
            const int nact   = (action>>8)&0xff;
            const int nsact  = (action)   &0xff;

            FillABox(hwnd, action?nact-1 :0);
            FillSBox(hwnd, action?nsact-1:0);
            FillFBox(hwnd);
            NewFilter(hwnd);

            for (i=0; i<numfonts; i++)
                WinDlgLboxInsertItem(hwnd, DID_FONTNAME_LB, fnames[i]);
            WinDlgLboxSelectItem(hwnd, DID_FONTNAME_LB, 0);

            if (action)
            {
                WinEnableControl(hwnd, DID_ACTION_CB,    FALSE);
                WinEnableControl(hwnd, DID_SUBACTION_CB, FALSE);
            }

            if (action || fdlg->fl&FDS_SAVEAS_DIALOG)
                WinEnableControl(hwnd, DID_AUTOSTART_PB, FALSE);

            szpath = concat(fdlg->pszIDrive, fdlg->szFullFile, NULL);
            *(strrchr(szpath, '\\')+1)='\0';

            WinSetDlgItemText(hwnd, DID_DIR_SELECTED, szpath);
            free (szpath);

            szpath = concat(archdep_boot_path(), "\\vice2.fon", NULL);
            if (!GpiLoadFonts(WinQueryAnchorBlock(hwnd), szpath))
            {
                log_debug("dlg-fileio.c: GpiLoadFonts('%s') failed.", szpath);
                WinEnableControl(hwnd, DID_FONTNAME_LB, FALSE);
            }
            free(szpath);
        }
        return FALSE;

    case WM_DESTROY:
        {
            char *szpath = concat(archdep_boot_path(), "\\vice2.fon", NULL);
            LboxFreeContents(WinWindowFromID(hwnd, DID_CONTENTS_LB));
            if (!GpiUnloadFonts(WinQueryAnchorBlock(hwnd), szpath))
                log_debug("dlg-fileio.c: GpiUnloadFonts('%s') failed.", szpath);
            free (szpath);
        }
        break;

    case WM_CONTROL:
        switch (SHORT1FROMMP(mp1))
        {
        case DID_ACTION_CB:
            if (SHORT2FROMMP(mp1)==CBN_ENTER)
                FillSBox(hwnd, 0);
        case DID_SUBACTION_CB:
            if (SHORT2FROMMP(mp1)==CBN_ENTER)
                FillFBox(hwnd);
        case DID_FFILTER_CB:
            if (SHORT2FROMMP(mp1)==CBN_ENTER)
                NewFilter(hwnd);
            return FALSE;

        case DID_DIRECTORY_LB:
            WinDefFileDlgProc(hwnd, msg, mp1, mp2);
            if (SHORT2FROMMP(mp1)==LN_SELECT ||
                SHORT2FROMMP(mp1)==LN_ENTER)
            {
                char szpath[CCHMAXPATH];

                const int nr = WinQueryLboxSelectedItem((HWND)mp2);

                if (nr<0)
                    return FALSE;

                if (SHORT2FROMMP(mp1)==LN_SELECT &&
                    WinQueryFocus(HWND_DESKTOP)==(HWND)mp2)
                    return FALSE;

                if (!GetLboxPath((HWND)mp2, nr, szpath))
                    return FALSE;

                WinSetDlgItemText(hwnd, DID_DIR_SELECTED, szpath);
            }
            return FALSE;

        case DID_FILENAME_ED:
            if (WinQueryButtonCheckstate(hwnd, DID_CONTENTS_CB) &&
                SHORT2FROMMP(mp1)==EN_CHANGE)
            {
                char szpath[CCHMAXPATH];
                if (GetFullFile(hwnd, szpath))
                    ShowContents(hwnd, szpath);
            }
            break;

        case DID_CONTENTS_CB:
            {
                const int state = WinQueryButtonCheckstate(hwnd, DID_CONTENTS_CB);
                WinEnableControl(hwnd, DID_CONTENTS_LB, state);
                //WinEnableControl(hwnd, DID_CBMFONT_CB,  state);
                WinEnableControl(hwnd, DID_FONTNAME_LB, state);
                if (state)
                    ContentsUpdate(hwnd);
                else
                    LboxFreeContents(WinWindowFromID(hwnd, DID_CONTENTS_LB));
            }
            return FALSE;

        case DID_FONTNAME_LB:
            if (SHORT2FROMMP(mp1)!=CBN_ENTER)
                break;
            //case DID_CBMFONT_CB:
            ContentsUpdate(hwnd);
            return FALSE;

        }
        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
        {
        case DID_DIRUP:
            {
                const HWND name = WinWindowFromID(hwnd, DID_FILENAME_ED);
                const int  pos  = WinDlgLboxSelectedItem(hwnd, DID_DIRECTORY_LB);

                char szpath[CCHMAXPATH];
                char *cpos;

                int len;

                if (!pos)
                    return FALSE;

                WinQueryDlgItemText(hwnd, DID_DIR_SELECTED,
                                    CCHMAXPATH-1, szpath);

                if (strlen(szpath)<4)
                    return FALSE;

                *strrchr(szpath, '\\')='\0';
                cpos = strrchr(szpath,'\\')+1;

                len = cpos-szpath+2;

                if (len==CCHMAXPATH-1)
                    return FALSE;

                WinQueryDlgItemText(hwnd, DID_FILENAME_ED,
                                    CCHMAXPATH-len-1, cpos);

                WinSetWindowText(name, szpath);

                *cpos = '\0';

                WinSetDlgItemText(hwnd, DID_DIR_SELECTED, szpath);

                //
                // set focus to entry field and simulate an Apply
                //
                WinSetFocus(HWND_DESKTOP, name);
                WinDefFileDlgProc(hwnd, WM_COMMAND, (MPARAM)DID_OK,
                                  MPFROM2SHORT(CMDSRC_PUSHBUTTON, TRUE));
            }
            return FALSE;

        case DID_AUTOSTART_PB:
            {
                const int pos = WinDlgLboxSelectedItem(hwnd, DID_CONTENTS_LB);

                FILEDLG *fdlg = (FILEDLG*)WinQueryWindowPtr(hwnd, QWL_USER);

                size_t length;
                char szpath[CCHMAXPATH];

                if (!GetFullFile(hwnd, szpath))
                    return FALSE;

                if (autostart_autodetect(szpath, NULL, pos < 0 ? 0 : pos,
                                         AUTOSTART_MODE_RUN))
                    return FALSE;

                length = strrchr(szpath, '\\') - szpath;

                fdlg->lReturn = DID_OK;

                if (length>0)
                    fdlg->szFullFile[length] = 0;
            }
            break;
        }
        break;

    case WM_MEASUREITEM:
        if (SHORT1FROMMP(mp1)==DID_CONTENTS_LB)
            return MRFROM2SHORT(9, 9);
        break;

    case WM_DRAWITEM:
        if (SHORT1FROMMP(mp1)==DID_CONTENTS_LB)
        {
            OWNERITEM *item = (OWNERITEM*)mp2;

            RECTL *rcl = &(item->rclItem);

            if (rcl->yTop-rcl->yBottom == 9)
            {
                image_contents_screencode_t *line =
                    (image_contents_screencode_t *)WinLboxItemHandle(item->hwnd, item->idItem);

                if (line)
                    LboxDrawLine(hwnd, item, rcl, line);

                item->fsState = item->fsStateOld = 0;
            }

            return (MRESULT)TRUE;
        }
        break;

    case FDM_VALIDATE:
        // mp1 = PSZ pszPathName
        // mp2 = USHORT Field name id
        // mr  = TRUE -> Valid name
        {
            const int sact = WinDlgLboxSelectedItem(hwnd, DID_SUBACTION_CB);
            const int act  = WinDlgLboxSelectedItem(hwnd, DID_ACTION_CB);

            const FILEDLG *fdlg = (FILEDLG*)WinQueryWindowPtr(hwnd, QWL_USER);

            char *szpath = (char*)mp1;

            int rc;

            char *slash;

            if (fdlg->fl&FDS_OPEN_DIALOG)
                rc = FdmDoLoadAction(hwnd, szpath, act, sact);
            else
                rc = FdmDoSaveAction(hwnd, szpath, act, sact);

            if (rc<0)
            {
                const action_t *action = fdlg->fl & FDS_OPEN_DIALOG ? LoadAction : SaveAction;

                char    *txt = concat("The following action couldn't be performed:\n", action[act].type, " ", action[act].subact[sact].action, NULL);
                HPOINTER hpt = WinLoadPointer(HWND_DESKTOP, NULLHANDLE, 0x101/*PTR_INFO*/);
                MB2INFO  mb  =
                {
                    sizeof(MB2INFO), hpt, 1, MB_CUSTOMICON|WS_VISIBLE, NULLHANDLE,
                    "      OK      ", 0, BS_DEFAULT
                };

                WinMessageBox2(HWND_DESKTOP, hwnd, txt, "VICE/2 Error", 0, &mb);
                free(txt);
                return FALSE;
            }

            //
            // FIXME! Better solution?
            //
            slash = strrchr(szpath, '\\');
            if (slash)
                *slash = '\0';

            chdir(szpath);
        }
        return (MRESULT)TRUE;

    }
    return WinDefFileDlgProc(hwnd, msg, mp1, mp2);
}

#ifdef __EMXC__
#define _getcwd _getcwd2
#endif

void ViceFileDialog(HWND hwnd, ULONG action, ULONG fl)
{
    FILEDLG filedlg; // File dialog info structure
    const char *dat = filedlg.szFullFile+2;

    static char drive[3]="f:";                        // maybe a resource
    static char path[CCHMAXPATH-2]="\\c64\\images";   // maybe a resource

    char  wdir[CCHMAXPATH];            // working directory
    char  odir[CCHMAXPATH];            // original directory

    ULONG rc;
    //
    // check for existance of directory
    //
    _getcwd(odir, CCHMAXPATH);         // store working dir

    strcat(strcpy(wdir, drive),path);
    if (chdir(wdir))                   // try if actual image dir exist
    {                                  // if it doesn't exist, set
        drive[0]=odir[0];              // imagedir to working dir
        drive[1]=':';                  // maybe drive is empty at first call
        strcpy(path, odir+2);
    }
    chdir(odir);                       // change back to working dir

    //
    // Setup 'MUST' values
    //
    memset(&filedlg, 0, sizeof(FILEDLG)); // Initially set all fields to 0
    filedlg.cbSize      = sizeof(FILEDLG);
    filedlg.fl          = FDS_CENTER | FDS_CUSTOM | fl;
    filedlg.usDlgId     = DLG_FILEIO;
    filedlg.pfnDlgProc  = ViceFileDlgProc;
    filedlg.pszTitle    = fl&FDS_OPEN_DIALOG?"Open File":"Save as";
    filedlg.pszOKButton = fl&FDS_OPEN_DIALOG?"Apply":"Save";
    filedlg.pszIDrive   = drive;
    filedlg.ulUser      = action;

    strcat(strcpy(filedlg.szFullFile, path), "\\");

    //
    // let the file dialog do it's work
    //
    rc=WinFileDlg(HWND_DESKTOP, hwnd, &filedlg);

    //
    // if error or not 'Attach' button
    //
    if (!rc || filedlg.lReturn!=DID_OK)
        return;

    //
    // if the result was ok store the path
    //
    drive[0]=filedlg.szFullFile[0];
    strcpy(path, dat);
}

