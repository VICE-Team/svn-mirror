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

#define INCL_WINHELP         // WinQueryHelpInstance
#define INCL_WINMENUS        // WinCheckMenuItem
#define INCL_WINDIALOGS      // WinMessageBox
#define INCL_WINWINDOWMGR    // QWL_USER

#include "videoarch.h"       // video_canvas_*
#include "dialogs.h"
#include "menubar.h"
#include "dlg-color.h"
#include "dlg-drive.h"
#include "dlg-monitor.h"
#include "dlg-joystick.h"
#include "dlg-datasette.h"
#include "dlg-ide64.h"

#include "snippets\\pmwin2.h"

#include <string.h>          // strcmp

#include "lib.h"
#include "log.h"
#include "monitor.h"         // mon
#include "tape.h"            // tape_image_detach
#include "util.h"           //
#include "sound.h"           // SOUND_ADJUST_*
#include "attach.h"          // file_system_detach_disk
#include "archdep.h"         // archdep_boot_path
#include "machine.h"         // machine_read/write_snapshot
#include "cmdline.h"         // cmdline_show_help, include resources.h
#include "vsyncapi.h"        // vsyncarch
#include "fliplist.h"        // fliplist_attach_head
#include "cartridge.h"       // CARTRIDGE_*
#include "resources.h"       // resource_value_t
#include "interrupt.h"       // interrupt_maincpu_trigger_trap
#include "screenshot.h"      // screenshot_save
#include "dlg-fileio.h"      // ViceFileDialog
#include "video-resources.h" // VIDEO_RESOURCE_PAL_*
#include "userport_joystick.h"

#if defined(__X128__) || defined(__X64__)
#include "cartridge.h"
#endif

#if defined(__X128__) || defined(__X64__) || defined(__X64DTV__) || defined(__XCBM5X0__) || defined(__XSCPU64__)
#include "vicii.h"
#endif

#ifdef HAVE_MOUSE
#include "mouse.h"
#endif

#ifdef __X64DTV__
#include "c64dtv-resources.h"
#endif

#if defined(__X64__) || defined(__XSCPU64__)
#include "c64model.h"        // c64model_set(), C64MODEL_*
#endif

#ifdef __X128__
#include "c128model.h"
#endif

#ifdef __XPLUS4__
#include "plus4model.h"
#include "ted.h"
#endif

#ifdef __XPET__
#include "petmodel.h"
#endif

#ifdef __XVIC__
#include "vic20model.h"
#include "vic.h"
#endif

#ifdef __X64DTV__
#include "c64dtvmodel.h"
#endif

#ifdef __XCBM__
#include "cbm2model.h"
#endif

#if defined(__X128__) || defined(__X64__) || defined(__X64DTV__) || defined(__XCBM5X0__) || defined(__XSCPU64__)
static const char *VIDEO_CACHE = "VICIIVideoCache";
static const char *DOUBLE_SIZE = "VICIIDoubleSize";
static const char *DOUBLE_SCAN = "VICIIDoubleScan";
static const char *PALETTE_FILE = "VICIIPaletteFile";
static const char *EXTERNAL_PALETTE = "VICIIExternalPalette";
static const char *COLOR_GAMMA = "VICIIColorGamma";
static const char *COLOR_TINT = "VICIIColorTint";
static const char *COLOR_SATURATION = "VICIIColorSaturation";
static const char *COLOR_CONTRAST = "VICIIColorContrast";
static const char *COLOR_BRIGHTNESS = "VICIIColorBrightness";
static const char *PAL_SCANLINE_SHADE = "VICIIPALScanLineShade";
static const char *PAL_BLUR = "VICIIPALBlur";
static const char *PAL_ODDLINE_PHASE = "VICIIPALOddLinePhase";
static const char *PAL_ODDLINE_OFFSET = "VICIIPALOddLineOffset";
static const char *RENDER_FILTER = "VICIIFilter";
static const char *AUDIO_LEAK = "VICIIAudioLeak";
static const char *BORDER_MODE = "VICIIBordermode";
#define NORMAL_BORDERS VICII_NORMAL_BORDERS
#define FULL_BORDERS VICII_FULL_BORDERS
#define DEBUG_BORDERS VICII_DEBUG_BORDERS
#define NO_BORDERS VICII_NO_BORDERS
#endif

#if defined(__XCBM2__) || defined(__XPET__)
static const char *VIDEO_CACHE = "CrtcVideoCache";
static const char *DOUBLE_SIZE = "CrtcDoubleSize";
static const char *DOUBLE_SCAN = "CrtcDoubleScan";
static const char *VERTICAL_STRETCH = "CrtcStretchVertical";
static const char *PALETTE_FILE = "CrtcPaletteFile";
static const char *EXTERNAL_PALETTE = "CrtcExternalPalette";
static const char *COLOR_GAMMA = "CrtcColorGamma";
static const char *COLOR_TINT = "CrtcColorTint";
static const char *COLOR_SATURATION = "CrtcColorSaturation";
static const char *COLOR_CONTRAST = "CrtcColorContrast";
static const char *COLOR_BRIGHTNESS = "CrtcColorBrightness";
static const char *PAL_SCANLINE_SHADE = "CrtcPALScanLineShade";
static const char *PAL_BLUR = "CrtcPALBlur";
static const char *PAL_ODDLINE_PHASE = "CrtcPALOddLinePhase";
static const char *PAL_ODDLINE_OFFSET = "CrtcPALOddLineOffset";
static const char *RENDER_FILTER = "CrtcFilter";
static const char *AUDIO_LEAK = "CrtcAudioLeak";
#endif

#ifdef __XPLUS4__
static const char *VIDEO_CACHE = "TEDVideoCache";
static const char *DOUBLE_SIZE = "TEDDoubleSize";
static const char *DOUBLE_SCAN = "TEDDoubleScan";
static const char *PALETTE_FILE = "TEDPaletteFile";
static const char *EXTERNAL_PALETTE = "TEDExternalPalette";
static const char *COLOR_GAMMA = "TEDColorGamma";
static const char *COLOR_TINT = "TEDColorTint";
static const char *COLOR_SATURATION = "TEDColorSaturation";
static const char *COLOR_CONTRAST = "TEDColorContrast";
static const char *COLOR_BRIGHTNESS = "TEDColorBrightness";
static const char *PAL_SCANLINE_SHADE = "TEDPALScanLineShade";
static const char *PAL_BLUR = "TEDPALBlur";
static const char *PAL_ODDLINE_PHASE = "TEDPALOddLinePhase";
static const char *PAL_ODDLINE_OFFSET = "TEDPALOddLineOffset";
static const char *RENDER_FILTER = "TEDFilter";
static const char *AUDIO_LEAK = "TEDAudioLeak";
static const char *BORDER_MODE = "TEDBordermode";
#define NORMAL_BORDERS TED_NORMAL_BORDERS
#define FULL_BORDERS TED_FULL_BORDERS
#define DEBUG_BORDERS TED_DEBUG_BORDERS
#define NO_BORDERS TED_NO_BORDERS
#endif

#ifdef __XVIC__
static const char *VIDEO_CACHE = "VICVideoCache";
static const char *DOUBLE_SIZE = "VICDoubleSize";
static const char *DOUBLE_SCAN = "VICDoubleScan";
static const char *PALETTE_FILE = "VICPaletteFile";
static const char *EXTERNAL_PALETTE = "VICExternalPalette";
static const char *COLOR_GAMMA = "VICColorGamma";
static const char *COLOR_TINT = "VICColorTint";
static const char *COLOR_SATURATION = "VICColorSaturation";
static const char *COLOR_CONTRAST = "VICColorContrast";
static const char *COLOR_BRIGHTNESS = "VICColorBrightness";
static const char *PAL_SCANLINE_SHADE = "VICPALScanLineShade";
static const char *PAL_BLUR = "VICPALBlur";
static const char *PAL_ODDLINE_PHASE = "VICPALOddLinePhase";
static const char *PAL_ODDLINE_OFFSET = "VICPALOddLineOffset";
static const char *RENDER_FILTER = "VICFilter";
static const char *AUDIO_LEAK = "VICAudioLeak";
static const char *BORDER_MODE = "VICBordermode";
#define NORMAL_BORDERS VIC_NORMAL_BORDERS
#define FULL_BORDERS VIC_FULL_BORDERS
#define DEBUG_BORDERS VIC_DEBUG_BORDERS
#define NO_BORDERS VIC_NO_BORDERS
#endif

#ifdef __X128__
static const char *VERTICAL_STRETCH = "VDCStretchVertical";
#endif

extern void set_volume(int vol);
extern int get_volume(void);

// --------------------------------------------------------------------------

static void toggle_async(WORD addr, void *name)
{
    toggle(name);
}

static void load_snapshot(WORD addr, void *hwnd)
{
    char *name = util_concat(archdep_boot_path(), "\\vice2.vsf", NULL);

    if (machine_read_snapshot(name , 0) < 0) {
        WinMessageBox(HWND_DESKTOP, (HWND)hwnd, "Unable to load snapshot - sorry!", "Load Snapshot", 0, MB_OK);
    } else {
        log_debug("Snapshot '%s' loaded successfully.", name);
    }
    lib_free(name);
}

static void save_snapshot(WORD addr, void *hwnd)
{
    // FIXME !!!!! roms, disks
    char *name = util_concat(archdep_boot_path(), "\\vice2.vsf", NULL);

    if (machine_write_snapshot(name, 1, 1, 0) < 0) {
        WinMessageBox(HWND_DESKTOP, (HWND)hwnd, "Unable to save snapshot - sorry!", "Save Snapshot", 0, MB_OK);
    } else {
        log_debug("Snapshot saved as '%s' successfully.", name);
    }
    lib_free(name);
}

void save_screenshot(HWND hwnd)
{
    char *name = util_concat(archdep_boot_path(), "\\vice2.png", NULL);

    if (!screenshot_save("PNG", name, (video_canvas_t *)WinQueryWindowPtr(hwnd, QWL_USER))) {
        log_debug("Screenshot saved as '%s' successfully.", name);
    }
    lib_free(name);
}

// --------------------------------------------------------------------------

char *printer_res(const char *res, const int num)
{
    switch (num) {
        case 0:
            return lib_msprintf(res, "4");
        case 1:
            return lib_msprintf(res, "5");
        case 2:
            return lib_msprintf(res, "6");
        case 3:
            return lib_msprintf(res, "Userport");
    }
    log_debug("Res: %d", num);
    return lib_msprintf(res, "ERROR");
}

void set_printer_res(const char *res, int num, resource_value_t *val)
{
    char *txt = printer_res(res, num);

    resources_set_value(txt, val);
    lib_free(txt);
}

void get_printer_res(const char *res, int num, resource_value_t *val)
{
    char *txt = printer_res(res, num);

    resources_get_value(txt, (void *)val);
    lib_free(txt);
}

// --------------------------------------------------------------------------

void ChangeSpeed(HWND hwnd, int idm)
{
    static ULONG tm = 0; //vsyncarch_gettime();
    static int step = 1;
    static int calls = 0;
    const video_canvas_t *c = (video_canvas_t *)WinQueryWindowPtr(hwnd, QWL_USER);
    ULONG time = vsyncarch_gettime();
    char *txt;
    int speed;

    resources_get_int("Speed", &speed);

    if ((signed long)(time-tm) < vsyncarch_frequency()) {
        if (calls == 25) {
            calls = 0;
            step *= 5;

            if (step > 100) {
                step = 100;
            }
        }
    } else {
        step = 1;
        calls = 0;
    }
    tm = time;

    calls++;

    speed += idm == IDM_PLUS ? step : -step;

    if (speed < 0) {
        speed = 0;
    }

    resources_set_int("Speed", speed);

    if (speed) {
        txt = lib_msprintf("%s - Set Max.Speed: %d%%", c->title, speed);
    } else {
        txt = lib_msprintf("%s - Set Max.Speed: unlimited", c->title);
    }
    WinSetWindowText(c->hwndTitlebar, txt);
    lib_free(txt);
}

// --------------------------------------------------------------------------

#ifdef __XPET__
static int sidaddr1 = 0x8f00;
static int sidaddr2 = 0xe900;
#endif

#ifdef __XPLUS4__
static int sidaddr1 = 0xfd40;
static int sidaddr2 = 0xfe80;
#endif

#ifdef __XVIC__
static int sidaddr1 = 0x9800;
static int sidaddr2 = 0x9c00;
#endif

void menu_action(HWND hwnd, USHORT idm) //, MPARAM mp2)
{
    switch (idm) {
        case IDM_PLUS:
        case IDM_MINUS:
            ChangeSpeed(hwnd, idm);
            return;
        case IDM_FILEOPEN:
            ViceFileDialog(hwnd, 0, FDS_OPEN_DIALOG);
            return;
        case IDM_FILESAVE:
            ViceFileDialog(hwnd, 0, FDS_ENABLEFILELB | FDS_SAVEAS_DIALOG);
            return;
        case IDM_ATTACHTAPE:
            ViceFileDialog(hwnd, 0x0201, FDS_OPEN_DIALOG);
            return;
        case IDM_ATTACH8:
        case IDM_ATTACH9:
        case IDM_ATTACH10:
        case IDM_ATTACH11:
            ViceFileDialog(hwnd, 0x0100 | (idm & 0xf), FDS_OPEN_DIALOG);
            return;
        case IDM_DETACHTAPE:
            tape_image_detach(1);
            return;
        case IDM_DETACH8:
        case IDM_DETACH9:
        case IDM_DETACH10:
        case IDM_DETACH11:
            file_system_detach_disk(idm & 0xf);
            return;
        case IDM_DETACHALL:
            file_system_detach_disk(8);
            file_system_detach_disk(9);
            file_system_detach_disk(10);
            file_system_detach_disk(11);
            return;
        case IDM_LOGWIN:
            WinActivateWindow(hwndLog, 1);
            return;

#ifdef __XVIC__
        case IDM_VIC20_CART_GENERIC:
            if (cartridge_attach_image(CARTRIDGE_VIC20_GENERIC, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_VIC20_CART_MEGACART:
            if (cartridge_attach_image(CARTRIDGE_VIC20_MEGACART, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_VIC20_CART_FINAL_EXP:
            if (cartridge_attach_image(CARTRIDGE_VIC20_FINAL_EXPANSION, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_VIC20_CART_VFP:
            if (cartridge_attach_image(CARTRIDGE_VIC20_FP, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_VIC20_CART_SMART_ATTACH:
            if (cartridge_attach_image(CARTRIDGE_VIC20_DETECT, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_VIC20_CART_4_8_16KB_2000:
            if (cartridge_attach_image(CARTRIDGE_VIC20_16KB_2000, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_VIC20_CART_4_8_16KB_4000:
            if (cartridge_attach_image(CARTRIDGE_VIC20_16KB_4000, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_VIC20_CART_4_8_16KB_6000:
            if (cartridge_attach_image(CARTRIDGE_VIC20_16KB_6000, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_VIC20_CART_4_8KB_A000:
            if (cartridge_attach_image(CARTRIDGE_VIC20_8KB_A000, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_VIC20_CART_4KB_B000:
            if (cartridge_attach_image(CARTRIDGE_VIC20_4KB_B000, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_VIC20_SET_DEFAULT_CART:
            cartridge_set_default();
            return;
        case IDM_VIC20_RESET_ON_CART_CHNG:
            toggle("CartridgeReset");
            return;
        case IDM_VIC20_DETACH_CART:
            cartridge_detach_image(-1);
            return;
#endif

#ifdef __XPLUS4__
        case IDM_PLUS4_CART_SMART_ATTACH:
            if (cartridge_attach_image(CARTRIDGE_PLUS4_DETECT, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_PLUS4_C0_LOW:
            if (cartridge_attach_image(CARTRIDGE_PLUS4_16KB_C0LO, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_PLUS4_C0_HIGH:
            if (cartridge_attach_image(CARTRIDGE_PLUS4_16KB_C0HI, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_PLUS4_C1_LOW:
            if (cartridge_attach_image(CARTRIDGE_PLUS4_16KB_C1LO, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_PLUS4_C1_HIGH:
            if (cartridge_attach_image(CARTRIDGE_PLUS4_16KB_C1HI, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_PLUS4_C2_LOW:
            if (cartridge_attach_image(CARTRIDGE_PLUS4_16KB_C2LO, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_PLUS4_C2_HIGH:
            if (cartridge_attach_image(CARTRIDGE_PLUS4_16KB_C2HI, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_PLUS4_RESET_ON_CART_CHNG:
            toggle("CartridgeReset");
            return;
        case IDM_PLUS4_DETACH_CART:
            cartridge_detach_image(-1);
            return;
#endif

#ifdef __XCBM__
        case IDM_CBM2_LOAD_1XXX:
            if (cartridge_attach_image(CARTRIDGE_CBM2_8KB_1000, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_CBM2_UNLOAD_1XXX:
            cartridge_detach_image(CARTRIDGE_CBM2_8KB_1000);
            return;
        case IDM_CBM2_LOAD_2_3XXX:
            if (cartridge_attach_image(CARTRIDGE_CBM2_8KB_2000, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_CBM2_UNLOAD_2_3XXX:
            cartridge_detach_image(CARTRIDGE_CBM2_8KB_2000);
            return;
        case IDM_CBM2_LOAD_4_5XXX:
            if (cartridge_attach_image(CARTRIDGE_CBM2_16KB_4000, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_CBM2_UNLOAD_4_5XXX:
            cartridge_detach_image(CARTRIDGE_CBM2_16KB_4000);
            return;
        case IDM_CBM2_LOAD_6_7XXX:
            if (cartridge_attach_image(CARTRIDGE_CBM2_16KB_6000, ViceFileSelect(hwnd, 1)) < 0) {
                ui_error("Cannot load cartridge image.");
            }
            return;
        case IDM_CBM2_UNLOAD_6_7XXX:
            cartridge_detach_image(CARTRIDGE_CBM2_16KB_6000);
            return;
        case IDM_CBM2_RESET_ON_CART_CHNG:
            toggle("CartridgeReset");
            return;
#endif

#if defined(__X64__) || defined(__X128__) || defined(__XCBM2__) || defined(__XSCPU64__)
        case IDM_CIA1_6526_OLD:
            resources_set_int("CIA1Model", 0);
            return;
        case IDM_CIA1_6526A_NEW:
            resources_set_int("CIA1Model", 1);
            return;
        case IDM_CIA2_6526_OLD:
            resources_set_int("CIA2Model", 0);
            return;
        case IDM_CIA2_6526A_NEW:
            resources_set_int("CIA2Model", 1);
            return;
#endif

#if defined(__X64__) || defined(__XSCPU64__)
        case IDM_C64PAL:
        case IDM_C64CPAL:
        case IDM_C64OLDPAL:
        case IDM_C64NTSC:
        case IDM_C64CNTSC:
        case IDM_C64OLDNTSC:
        case IDM_DREAN:
            c64model_set(C64MODEL_C64_PAL + idm - IDM_C64PAL);
            return;
        case IDM_C64SXPAL:
        case IDM_C64SXNTSC:
        case IDM_C64JAP:
        case IDM_C64GS:
        case IDM_PET64PAL:
        case IDM_PET64NTSC:
        case IDM_ULTIMAX:
            c64model_set(C64MODEL_C64SX_PAL + idm - IDM_C64SXPAL);
            return;
        case IDM_6569_PAL:
        case IDM_8565_PAL:
        case IDM_6569R1_OLD_PAL:
        case IDM_6567_NTSC:
        case IDM_8562_NTSC:
        case IDM_6567R56A_OLD_NTSC:
        case IDM_6572_PAL_N:
            resources_set_int("VICIIModel", VICII_MODEL_6569 + IDM_6569_PAL - idm);
            return;
        case IDM_VICII_NEW_LUMINANCES:
            toggle("VICIINewLuminances");
            return;
        case IDM_DISCRETE:
        case IDM_CUSTOM_IC:
            resources_set_int("GlueLogic", idm - IDM_DISCRETE);
            return;
#endif

#if defined(__X128__)
        case IDM_C128PAL:
        case IDM_C128DCRPAL:
        case IDM_C128NTSC:
        case IDM_C128DCRNTSC:
            c128model_set(C128MODEL_C128_PAL + idm - IDM_C128PAL);
            return;
#endif

#if defined(__XPLUS4__)
        case IDM_C16PAL:
        case IDM_C16NTSC:
        case IDM_PLUS4PAL:
        case IDM_PLUS4NTSC:
        case IDM_V364NTSC:
        case IDM_C232NTSC:
            plus4model_set(PLUS4MODEL_C16_PAL + idm - IDM_C16PAL);
            return;
#endif

#if defined(__XPET__)
        case IDM_PET2001:
        case IDM_PET3008:
        case IDM_PET3016:
        case IDM_PET3032:
        case IDM_PET3032B:
        case IDM_PET4016:
        case IDM_PET4032:
        case IDM_PET4032B:
        case IDM_PET8032:
        case IDM_PET8096:
        case IDM_PET8296:
        case IDM_PETSUPER:
            petmodel_set(PETMODEL_2001 + idm - IDM_PET2001);
            return;
#endif

#if defined(__XCBM__)
        case IDM_CBM510PAL:
        case IDM_CBM510NTSC:
        case IDM_CBM610PAL:
        case IDM_CBM610NTSC:
        case IDM_CBM620PAL:
        case IDM_CBM620NTSC:
        case IDM_CBM620PLUSPAL:
        case IDM_CBM620PLUSNTSC:
        case IDM_CBM710NTSC:
        case IDM_CBM720NTSC:
        case IDM_CBM720PLUSNTSC:
            cbm2model_set(CBM2MODEL_510_PAL + idm - IDM_CBM510PAL);
            return;
#endif

#if defined(__X64DTV__)
        case IDM_DTV2PAL:
        case IDM_DTV2NTSC:
        case IDM_DTV3PAL:
        case IDM_DTV3NTSC:
        case IDM_HUMMERNTSC:
            dtvmodel_set(DTVMODEL_V2_PAL + idm - IDM_DTV2PAL);
            return;
#endif

#if defined(__XVIC__)
        case IDM_VIC20PAL:
        case IDM_VIC20NTSC:
        case IDM_VIC21:
            vic20model_set(VIC20MODEL_VIC20_PAL + idm - IDM_VIC20PAL);
            return;
#endif

#if defined(__X64__) || defined(__X128__) || defined(__XSCPU64__)
        case IDM_CRTGEN:
        case IDM_CRTGEN8KB:
        case IDM_CRTGEN16KB:
        case IDM_CRTACTREPL:
        case IDM_CRTATOMPOW:
        case IDM_CRTEPYX:
        case IDM_CRTSSSHOT:
        case IDM_CRTSSSHOT5:
        case IDM_CRTWEST:
        case IDM_CRTIEEE:
        case IDM_CRTIDE64:
            ViceFileDialog(hwnd, 0x0b00 | (idm & 0xf), FDS_OPEN_DIALOG);
            return;
        case IDM_CARTRIDGEDET:
            cartridge_detach_image(-1);
            return;
        case IDM_CRTFREEZE:
            cartridge_trigger_freeze();
            return;
        case IDM_CRTSAVEIMG:
            ViceFileDialog(hwnd, 0x0601, FDS_ENABLEFILELB | FDS_SAVEAS_DIALOG);
            return;
#endif

        case IDM_FLIPNEXT8:
        case IDM_FLIPNEXT9:
        case IDM_FLIPPREV8:
        case IDM_FLIPPREV9:
            fliplist_attach_head((idm >> 4) & 0xf, idm & 1);
            return;
        case IDM_DEFAULTCONFIG:
            resources_set_defaults();
            return;
        case IDM_READCONFIG:
            if (resources_load(NULL) < 0) {
                WinMessageBox(HWND_DESKTOP, hwnd, "Cannot load default settings.", "Resources", 0, MB_OK);
            }
            return;
        case IDM_WRITECONFIG:
            WinMessageBox(HWND_DESKTOP, hwnd, resources_save(NULL) < 0 ? "Cannot save default settings." : "Settings written successfully.", "Resources", 0, MB_OK);
            return;
        case IDM_SNAPLOAD:
            interrupt_maincpu_trigger_trap(load_snapshot, (void*)hwnd);
            return;
        case IDM_SNAPSAVE:
            interrupt_maincpu_trigger_trap(save_snapshot, (void*)hwnd);
            return;
        case IDM_PRINTSCRN:
            save_screenshot(hwnd);
            return;
        case IDM_HARDRESET:
            hardreset_dialog(hwnd);
            return;
        case IDM_SOFTRESET:
            softreset_dialog(hwnd);
            return;

        case IDM_COLOR:
            color_dialog(hwnd, COLOR_GAMMA, COLOR_TINT, COLOR_SATURATION, COLOR_CONTRAST, COLOR_BRIGHTNESS);
            return;

        case IDM_CRT:
            crt_dialog(hwnd, PAL_SCANLINE_SHADE, PAL_BLUR, PAL_ODDLINE_PHASE, PAL_ODDLINE_OFFSET);
            return;

#ifdef __X128__
        case IDM_COLOR2:
            color_dialog(hwnd, "VDCColorGamma", "VDCColorTint", "VDCColorSaturation", "VDCColorContrast", "VDCColorBrightness");
            return;

        case IDM_CRT2:
            crt_dialog(hwnd, "VDCPALScanLineShade", "VDCPALBlur", "VDCPALOddLinePhase", "VDCPALOddLineOffset");
            return;
#endif

        case IDM_EXTERNALPAL:
            toggle(EXTERNAL_PALETTE);
            return;

        case IDM_DSIZE:
            interrupt_maincpu_trigger_trap(toggle_async, (resource_value_t*)DOUBLE_SIZE);
            return;
        case IDM_DSCAN:
            interrupt_maincpu_trigger_trap(toggle_async, (resource_value_t*)DOUBLE_SCAN);
            return;
        case IDM_EXTERNALPAL_FILE:
            resources_set_string(PALETTE_FILE, ViceFileSelect(hwnd, 1));
            return;
        case IDM_AUDIO_LEAK:
            toggle(AUDIO_LEAK);
            return;

#ifdef __X128__
        case IDM_VERTICAL_STRETCH:
            toggle(VERTICAL_STRETCH);
            return;
#endif

#if defined(__XPET__) || defined(__XCBM2__)
        case IDM_VERTICAL_STRETCH:
            toggle(VERTICAL_STRETCH);
            return;
#else
        case IDM_BORDER_NORMAL:
            resources_set_int(BORDER_MODE, NORMAL_BORDERS);
            return;
        case IDM_BORDER_FULL:
            resources_set_int(BORDER_MODE, FULL_BORDERS);
            return;
        case IDM_BORDER_DEBUG:
            resources_set_int(BORDER_MODE, DEBUG_BORDERS);
            return;
        case IDM_BORDER_NONE:
            resources_set_int(BORDER_MODE, NO_BORDERS);
            return;
#endif

#ifdef __X128__
        case IDM_VDC16K:
        case IDM_VDC64K:
            resources_set_int("VDC64KB", idm&1);
            return;
        case IDM_INTFUNCROM_NONE:
            resources_set_int("InternalFunctionROM", 0);
            return;
        case IDM_INTFUNCROM_ROM:
            resources_set_int("InternalFunctionROM", 1);
            return;
        case IDM_INTFUNCROM_RAM:
            resources_set_int("InternalFunctionROM", 2);
            return;
        case IDM_INTFUNCROM_RTC:
            resources_set_int("InternalFunctionROM", 3);
            return;
        case IDM_INTFUNCROM_RTC_SAVE:
            toggle("InternalFunctionROMRTCSave");
            return;
        case IDM_EXTFUNCROM_NONE:
            resources_set_int("ExternalFunctionROM", 0);
            return;
        case IDM_EXTFUNCROM_ROM:
            resources_set_int("ExternalFunctionROM", 1);
            return;
        case IDM_EXTFUNCROM_RAM:
            resources_set_int("ExternalFunctionROM", 2);
            return;
        case IDM_EXTFUNCROM_RTC:
            resources_set_int("ExternalFunctionROM", 3);
            return;
        case IDM_EXTFUNCROM_RTC_SAVE:
            toggle("ExternalFunctionROMRTCSave");
            return;
        case IDM_VDCDSIZE:
            interrupt_maincpu_trigger_trap(toggle_async, "VDCDoubleSize");
            return;
        case IDM_VDCDSCAN:
            interrupt_maincpu_trigger_trap(toggle_async, "VDCDoubleScan");
            return;
        case IDM_VDCEXTERNALPAL_FILE:
            resources_set_string("VDCPaletteFile", ViceFileSelect(hwnd, 1));
            return;
        case IDM_VDCAUDIO_LEAK:
            toggle("VDCAudioLeak");
            return;
#endif

#if defined(__X64__) || defined(__X128__) || defined(__XVIC__) || defined(__X64DTV__) || defined(__XSCPU64__)
        case IDM_PAL:
            resources_set_int("MachineVideoStandard", MACHINE_SYNC_PAL);
            return;
        case IDM_NTSC:
            resources_set_int("MachineVideoStandard", MACHINE_SYNC_NTSC);
            return;
#if defined(__X64__) || defined(__XSCPU64__)
        case IDM_NTSCOLD:
            resources_set_int("MachineVideoStandard", MACHINE_SYNC_NTSCOLD);
            return;
#endif // __X64__
#endif //  __X64__ || __X128__ || __XVIC__

#if defined(__X64__) || defined(__X128__) || defined(__X64DTV__) || defined(__XSCPU64__)
        case IDM_SBCOLL:
            toggle("VICIICheckSbColl");
            return;
        case IDM_SSCOLL:
            toggle("VICIICheckSsColl");
            return;
#endif

#if defined __XVIC__
        case IDM_VIC20_FE_WRITEBACK:
            toggle("FinalExpansionWriteBack");
            return;
        case IDM_VIC20_VFP_WRITEBACK:
            toggle("VicFlashPluginWriteBack");
            return;
        case IDM_VIC20_MC_WRITEBACK:
            toggle("MegaCartNvRAMWriteBack");
            return;
        case IDM_VIC20_MC_FILE:
            resources_set_string("MegaCartNvRAMfilename", ViceFileSelect(hwnd, 1));
            return;
#endif

#if defined(__X64__) || defined(__X128__) || defined(__XVIC__) || defined(__XSCPU64__)
        case IDM_GEORAM:
            toggle("GEORAM");
            return;
        case IDM_GEORAM64:
        case IDM_GEORAM128:
        case IDM_GEORAM256:
        case IDM_GEORAM512:
        case IDM_GEORAM1024:
        case IDM_GEORAM2048:
        case IDM_GEORAM4096:
            resources_set_int("GEORAMsize", (idm & 0xff) << 6);
            return;
        case IDM_SAVE_GEORAM:
            toggle("GEORAMImageWrite");
            return;
        case IDM_GEORAMFILE:
            resources_set_string("GEORAMfilename", ViceFileSelect(hwnd, 1));
            return;
        case IDM_SFX_SE:
            toggle("SFXSoundExpander");
            return;
        case IDM_SFX_SS:
            toggle("SFXSoundSampler");
            return;
        case IDM_SFX_SE_3526:
            resources_set_int("SFXSoundExpanderChip", 3526);
            return;
        case IDM_SFX_SE_3812:
            resources_set_int("SFXSoundExpanderChip", 3812);
            return;
        case IDM_DIGIMAX:
            toggle("DIGIMAX");
            return;
        case IDM_DS12C887RTC:
            toggle("DS12C887RTC");
            return;
        case IDM_DS12C887RTC_SAVE:
            toggle("DS12C887RTCSave");
            return;
#ifdef HAVE_TFE
        case IDM_TFE:
            toggle("ETHERNET_ACTIVE");
            return;
#endif
#endif

#ifndef __XVIC__
        case IDM_DIGIMAXDE00:
        case IDM_DIGIMAXDE20:
        case IDM_DIGIMAXDE40:
        case IDM_DIGIMAXDE60:
        case IDM_DIGIMAXDE80:
        case IDM_DIGIMAXDEA0:
        case IDM_DIGIMAXDEC0:
        case IDM_DIGIMAXDEE0:
        case IDM_DIGIMAXDF00:
        case IDM_DIGIMAXDF20:
        case IDM_DIGIMAXDF40:
        case IDM_DIGIMAXDF60:
        case IDM_DIGIMAXDF80:
        case IDM_DIGIMAXDFA0:
        case IDM_DIGIMAXDFC0:
        case IDM_DIGIMAXDFE0:
            resources_set_int("DIGIMAXbase", 0xde00 + ((idm & 0xf) * 0x20));
            return;
#else
        case IDM_DIGIMAXDE00:
        case IDM_DIGIMAXDE20:
        case IDM_DIGIMAXDE40:
        case IDM_DIGIMAXDE60:
        case IDM_DIGIMAXDE80:
        case IDM_DIGIMAXDEA0:
        case IDM_DIGIMAXDEC0:
        case IDM_DIGIMAXDEE0:
            resources_set_int("DIGIMAXbase", 0x9800 + ((idm & 0xf) * 0x20));
            return;
        case IDM_DIGIMAXDF00:
        case IDM_DIGIMAXDF20:
        case IDM_DIGIMAXDF40:
        case IDM_DIGIMAXDF60:
        case IDM_DIGIMAXDF80:
        case IDM_DIGIMAXDFA0:
        case IDM_DIGIMAXDFC0:
        case IDM_DIGIMAXDFE0:
            resources_set_int("DIGIMAXbase", 0x9b00 + ((idm & 0xf) * 0x20));
            return;
#endif

#ifndef __XVIC__
#ifndef __X128__
        case IDM_DS12C887RTCD500:
            resources_set_int("DS12C887RTCbase", 0xd500);
            return;
        case IDM_DS12C887RTCD600:
            resources_set_int("DS12C887RTCbase", 0xd600);
            return;
#endif
        case IDM_DS12C887RTCD700:
            resources_set_int("DS12C887RTCbase", 0xd700);
            return;
        case IDM_DS12C887RTCDE00:
            resources_set_int("DS12C887RTCbase", 0xde00);
            return;
        case IDM_DS12C887RTCDF00:
            resources_set_int("DS12C887RTCbase", 0xdf00);
            return;
#else
        case IDM_DS12C887RTC9800:
            resources_set_int("DS12C887RTCbase", 0x9800);
            return;
        case IDM_DS12C887RTC9C00:
            resources_set_int("DS12C887RTCbase", 0x9c00);
            return;
#endif

#ifdef __XVIC__
        case IDM_GEORAM_IO_SWAP:
            toggle("GEORAMIOSwap");
            return;
        case IDM_SFX_SE_IO_SWAP:
            toggle("SFXSoundExpanderIOSwap");
            return;
        case IDM_SFX_SS_IO_SWAP:
            toggle("SFXSoundSamplerIOSwap");
            return;
#ifdef HAVE_TFE
        case IDM_TFE_IO_SWAP:
            toggle("TFEIOSwap");
            return;
#endif
#endif

#if defined(__X64__) || defined(__X128__) || defined(__XSCPU64__)
        case IDM_REU:
            toggle("REU");
            return;
        case IDM_RAMCART:
            toggle("RAMCART");
            return;
        case IDM_MAGICVOICE:
            toggle("MagicVoiceCartridgeEnabled");
            return;
        case IDM_V364SPEECH:
            toggle("SpeechEnabled");
            return;
        case IDM_EASYFLASH_JUMPER:
            toggle("EasyFlashJumper");
            return;
        case IDM_EASYFLASH_AUTOSAVE:
            toggle("EasyFlashWriteCRT");
            return;
        case IDM_EASYFLASH_SAVE_NOW:
            if (cartridge_flush_image(CARTRIDGE_EASYFLASH) < 0) {
                ui_error("Can not save to EasyFlash .crt file");
            }
            return;
        case IDM_USERPORT_RTC:
            toggle("UserportRTC");
            return;
        case IDM_USERPORT_RTC_SAVE:
            toggle("UserportRTCSave");
            return;
        case IDM_DQBB:
            toggle("DQBB");
            return;
        case IDM_SAVE_DQBB:
            toggle("DQBBImageWrite");
            return;
        case IDM_ISEPIC:
            toggle("IsepicCartridgeEnabled");
            return;
        case IDM_ISEPIC_SWITCH:
            toggle("IsepicSwitch");
            return;
        case IDM_SAVE_ISEPIC:
            toggle("IsepicImageWrite");
            return;
        case IDM_EXPERT:
            toggle("ExpertCartridgeEnabled");
            return;
        case IDM_SAVE_EXPERT:
            toggle("ExpertImageWrite");
            return;

#if defined(__X64__) || defined(__XSCPU64__)
        case IDM_BURST_NONE:
            resources_set_int("BurstMod", 0);
            return;
        case IDM_BURST_CIA1:
            resources_set_int("BurstMod", 1);
            return;
        case IDM_BURST_CIA2:
            resources_set_int("BurstMod", 2);
            return;
#endif

#ifdef __XSCPU64__
        case IDM_SCPU64_SIMM_0:
            resources_set_int("SIMMSize", 0);
            return;
        case IDM_SCPU64_SIMM_1:
            resources_set_int("SIMMSize", 1);
            return;
        case IDM_SCPU64_SIMM_4:
            resources_set_int("SIMMSize", 4);
            return;
        case IDM_SCPU64_SIMM_8:
            resources_set_int("SIMMSize", 8);
            return;
        case IDM_SCPU64_SIMM_16:
            resources_set_int("SIMMSize", 8);
            return;
        case IDM_SCPU64_JIFFY_SWITCH:
            toggle("JiffySwitch");
            return;
        case IDM_SCPU64_SPEED_SWITCH:
            toggle("SpeedSwitch");
            return;
#endif

        case IDM_REU128:
        case IDM_REU256:
        case IDM_REU512:
        case IDM_REU1024:
        case IDM_REU2048:
        case IDM_REU4096:
        case IDM_REU8192:
        case IDM_REU16384:
            resources_set_int("REUsize", (idm & 0xff) << 7);
            return;
        case IDM_SAVE_REU:
            toggle("REUImageWrite");
            return;
        case IDM_REUFILE:
            resources_set_string("REUfilename", ViceFileSelect(hwnd, 1));
            return;
        case IDM_RAMCART64:
        case IDM_RAMCART128:
            resources_set_int("RAMCARTsize", (idm & 0xf) << 6);
            return;
        case IDM_SAVE_RAMCART:
            toggle("RAMCARTImageWrite");
            return;
        case IDM_RAMCARTFILE:
            resources_set_string("RAMCARTfilename", ViceFileSelect(hwnd, 1));
            return;
        case IDM_MAGICVOICEFILE:
            resources_set_string("MagicVoiceImage", ViceFileSelect(hwnd, 1));
            return;
        case IDM_V364SPEECHFILE:
            resources_set_string("SpeechImage", ViceFileSelect(hwnd, 1));
            return;
        case IDM_DQBBFILE:
            resources_set_string("DQBBfilename", ViceFileSelect(hwnd, 1));
            return;
        case IDM_ISEPICFILE:
            resources_set_string("Isepicfilename", ViceFileSelect(hwnd, 1));
            return;
        case IDM_EXPERTFILE:
            resources_set_string("Expertfilename", ViceFileSelect(hwnd, 1));
            return;
        case IDM_EXPERT_MODE_OFF:
        case IDM_EXPERT_MODE_PRG:
        case IDM_EXPERT_MODE_ON:
            resources_set_int("ExpertCartridgeMode", idm - IDM_EXPERT_MODE_OFF);
            return;
        case IDM_DIGIMAXDD00:
            resources_set_int("DIGIMAXbase", 0xdd00);
            return;

        case IDM_IDE64_VER_3:
        case IDM_IDE64_VER_4:
            resources_set_int("IDE64version4", idm - IDM_IDE64_VER_3);
            return;
        case IDM_IDE64_RTC_SAVE:
            toggle("IDE64RTCSave");
            return;
        case IDM_IDE64_IMAGE1:
            resources_set_string("IDE64Image1", ViceFileSelect(hwnd, 1));
            return;
        case IDM_IDE64_IMAGE2:
            resources_set_string("IDE64Image2", ViceFileSelect(hwnd, 1));
            return;
        case IDM_IDE64_IMAGE3:
            resources_set_string("IDE64Image3", ViceFileSelect(hwnd, 1));
            return;
        case IDM_IDE64_IMAGE4:
            resources_set_string("IDE64Image4", ViceFileSelect(hwnd, 1));
            return;
        case IDM_IDE64_AUTO1:
            toggle("IDE64AutodetectSize1");
            return;
        case IDM_IDE64_AUTO2:
            toggle("IDE64AutodetectSize2");
            return;
        case IDM_IDE64_AUTO3:
            toggle("IDE64AutodetectSize3");
            return;
        case IDM_IDE64_AUTO4:
            toggle("IDE64AutodetectSize4");
            return;
        case IDM_IDE64_GEO1:
            ide64_dialog(hwnd, 1);
            return;
        case IDM_IDE64_GEO2:
            ide64_dialog(hwnd, 2);
            return;
        case IDM_IDE64_GEO3:
            ide64_dialog(hwnd, 3);
            return;
        case IDM_IDE64_GEO4:
            ide64_dialog(hwnd, 4);
            return;

        case IDM_MMC64:
            toggle("MMC64");
            return;
        case IDM_MMC64_REV_A:
        case IDM_MMC64_REV_B:
            resources_set_int("MMC64_revision", idm - IDM_MMC64_REV_A);
            return;
        case IDM_MMC64_FLASH_JUMPER:
            toggle("MMC64_flashjumper");
            return;
        case IDM_MMC64_BIOS_WRITE:
            toggle("MMC64_bios_write");
            return;
        case IDM_MMC64_BIOS_FILE:
            resources_set_string("MMC64BIOSfilename", ViceFileSelect(hwnd, 1));
            return;
        case IDM_MMC64_SD_MMC_READONLY:
            toggle("MMC64_RO");
            return;
        case IDM_MMC64_SD_MMC_FILE:
            resources_set_string("MMC64imagefilename", ViceFileSelect(hwnd, 1));
            return;
        case IDM_MMC64_SD_TYPE_AUTO:
        case IDM_MMC64_SD_TYPE_MMC:
        case IDM_MMC64_SD_TYPE_SD:
        case IDM_MMC64_SD_TYPE_SDHC:
            resources_set_int("MMC64_sd_type", idm - IDM_MMC64_SD_TYPE_AUTO);
            return;

        case IDM_MMCR_RESCUE_MODE:
            toggle("MMCRRescueMode");
            return;
        case IDM_MMCR_EEPROM_WRITE:
            toggle("MMCRImageWrite");
            return;
        case IDM_MMCR_EEPROM_FILE:
            resources_set_string("MMCREEPROMImage", ViceFileSelect(hwnd, 1));
            return;
        case IDM_MMCR_EEPROM_READWRITE:
            toggle("MMCREEPROMRW");
            return;
        case IDM_MMCR_SD_MMC_READWRITE:
            toggle("MMCRCardRW");
            return;
        case IDM_MMCR_SD_MMC_FILE:
            resources_set_string("MMCRCardImage", ViceFileSelect(hwnd, 1));
            return;
        case IDM_MMCR_SD_TYPE_AUTO:
        case IDM_MMCR_SD_TYPE_MMC:
        case IDM_MMCR_SD_TYPE_SD:
        case IDM_MMCR_SD_TYPE_SDHC:
            resources_set_int("MMCRSDType", idm - IDM_MMCR_SD_TYPE_AUTO);
            return;

        case IDM_RR_FLASH_WRITE:
            toggle("RRBiosWrite");
            return;
        case IDM_RR_FLASH_JUMPER:
            toggle("RRFlashJumper");
            return;
        case IDM_RR_BANK_JUMPER:
            toggle("RRBankJumper");
            return;
        case IDM_RR_REV_RR:
            resources_set_int("RRrevision", 0);
            return;
        case IDM_RR_REV_NR:
            resources_set_int("RRrevision", 1);
            return;

#ifdef __X64__
        case IDM_PLUS60KD040:
            resources_set_int("PLUS60Kbase", 0xd040);
            return;
        case IDM_PLUS60KD100:
            resources_set_int("PLUS60Kbase", 0xd100);
            return;
        case IDM_PLUS60KFILE:
            resources_set_string("PLUS60Kfilename", ViceFileSelect(hwnd, 1));
            return;
        case IDM_PLUS256KFILE:
            resources_set_string("PLUS256Kfilename", ViceFileSelect(hwnd, 1));
            return;
        case IDM_C64_256K_BASEDE00:
        case IDM_C64_256K_BASEDE80:
        case IDM_C64_256K_BASEDF00:
        case IDM_C64_256K_BASEDF80:
            resources_set_int("C64_256Kbase", ((idm & 3) * 80) + 0xde00);
            return;
        case IDM_MEMORY_HACK_NONE:
            resources_set_int("MemoryHack", MEMORY_HACK_NONE);
            return;
        case IDM_MEMORY_HACK_C64_256K:
            resources_set_int("MemoryHack", MEMORY_HACK_C64_256K);
            return;
        case IDM_MEMORY_HACK_PLUS60K:
            resources_set_int("MemoryHack", MEMORY_HACK_PLUS60K);
            return;
        case IDM_MEMORY_HACK_PLUS256K:
            resources_set_int("MemoryHack", MEMORY_HACK_PLUS256K);
            return;
        case IDM_C64_256KFILE:
            resources_set_string("C64_256Kfilename", ViceFileSelect(hwnd, 1));
            return;
#endif
#endif // __X64__ || __X128__

#ifdef __X64DTV__
        case IDM_C64DTV_FLASHROM_ATTACH:
            resources_set_string("c64dtvromfilename", ViceFileSelect(hwnd, 0));
            return;
        case IDM_C64DTV_FLASHROM_DETACH:
            resources_set_string("c64dtvromfilename", "");
            return;
        case IDM_C64DTV_FLASHROM_RW:
            toggle("c64dtvromrw");
            return;
        case IDM_DTV2:
            resources_set_int("DtvRevision", 2);
            return;
       case IDM_DTV3:
            resources_set_int("DtvRevision", 3);
            return;
        case IDM_C64DTV_HUMMER_ADC:
            toggle("HummerADC");
            return;
#endif

        case IDM_AUTOSTART_WARP:
            toggle("AutostartWarp");
            return;
        case IDM_RUN_WITH_COLON:
            toggle("AutostartRunWithColon");
            return;
        case IDM_LOAD_TO_BASIC_START:
            toggle("AutostartBasicLoad");
            return;
        case IDM_AUTOSTART_RANDOM_DELAY:
            toggle("AutostartDelayRandom");
            return;
        case IDM_AUTOSTART_PRG_VIRTUAL_FS:
            resources_set_int("AutostartPrgMode", 0);
            return;
        case IDM_AUTOSTART_PRG_INJECT:
            resources_set_int("AutostartPrgMode", 1);
            return;
        case IDM_AUTOSTART_PRG_DISK_IMAGE:
            resources_set_int("AutostartPrgMode", 2);
            return;
        case IDM_AUTOSTART_DISK_FILE:
            resources_set_string("AutostartPrgDiskImage", ViceFileSelect(hwnd, 1));
            return;

#ifdef __XPET__
        case IDM_PETREU:
            toggle("PETREU");
            return;
        case IDM_PETREU128:
            resources_set_int("PETREUSize", 128);
            return;
        case IDM_PETREU512:
            resources_set_int("PETREUSize", 512);
            return;
        case IDM_PETREU1024:
            resources_set_int("PETREUSize", 1024);
            return;
        case IDM_PETREU2048:
            resources_set_int("PETREUSize", 2048);
            return;

        case IDM_PETREUFILE:
            resources_set_string("PETREUfilename", ViceFileSelect(hwnd, 1));
            return;

        case IDM_PETDWW:
            toggle("PETDWW");
            return;

        case IDM_PETDWWFILE:
            resources_set_string("PETDWWfilename", ViceFileSelect(hwnd, 1));
            return;

        case IDM_PETHRE:
            toggle("PETHRE");
            return;

        case IDM_PET_USERPORT_DAC:
            toggle("UserportDAC");
            return;
#endif

#ifdef HAVE_MOUSE
        case IDM_MOUSE:
            toggle("Mouse");
            return;
        case IDM_HIDEMOUSE:
            toggle("HideMousePtr");
            return;
        case IDM_SMART_MOUSE_RTC_SAVE:
            toggle("SmartMouseRTCSave");
            return;

#if defined(__X128__) || defined(__X64__) || defined(__XSCPU64__)
        case IDM_MOUSE_TYPE_1351:
            resources_set_int("Mousetype", MOUSE_TYPE_1351);
            return;
        case IDM_MOUSE_TYPE_NEOS:
            resources_set_int("Mousetype", MOUSE_TYPE_NEOS);
            return;
        case IDM_MOUSE_TYPE_AMIGA:
            resources_set_int("Mousetype", MOUSE_TYPE_AMIGA);
            return;
        case IDM_MOUSE_TYPE_PADDLE:
            resources_set_int("Mousetype", MOUSE_TYPE_PADDLE);
            return;
        case IDM_MOUSE_TYPE_CX22:
            resources_set_int("Mousetype", MOUSE_TYPE_CX22);
            return;
        case IDM_MOUSE_TYPE_ST:
            resources_set_int("Mousetype", MOUSE_TYPE_ST);
            return;
        case IDM_MOUSE_TYPE_SMART:
            resources_set_int("Mousetype", MOUSE_TYPE_SMART);
            return;
        case IDM_MOUSE_TYPE_MICROMYS:
            resources_set_int("Mousetype", MOUSE_TYPE_MICROMYS);
            return;
        case IDM_MOUSE_TYPE_KOALAPAD:
            resources_set_int("Mousetype", MOUSE_TYPE_KOALAPAD);
            return;
        case IDM_MOUSE_PORT_1:
            resources_set_int("Mouseport", 1);
            return;
        case IDM_MOUSE_PORT_2:
            resources_set_int("Mouseport", 2);
            return;
#endif
#endif // HAVE_MOUSE

        case IDM_PRT4IEC:
        case IDM_PRT5IEC:
        case IDM_PRT6IEC:
        case IDM_PRTUP:
            {
                char *res = printer_res("Printer%s", (idm >> 4) & 0xf);
                toggle(res);
                lib_free(res);
            }
            return;
        case IDM_PRT4ASCII:
        case IDM_PRT5ASCII:
        case IDM_PRTUPASCII:
            set_printer_res("Printer%sDriver", (idm >> 4) & 0xf, (resource_value_t*)"ascii");
            return;
        case IDM_PRT6_1520:
            set_printer_res("Printer%sDriver", (idm >> 4) & 0xf, (resource_value_t*)"1520");
            return;
        case IDM_PRT4MPS803:
        case IDM_PRT5MPS803:
        case IDM_PRTUPMPS803:
            set_printer_res("Printer%sDriver", (idm >> 4) & 0xf, (resource_value_t*)"mps803");
            return;
        case IDM_PRT4NL10:
        case IDM_PRT5NL10:
        case IDM_PRTUPNL10:
            set_printer_res("Printer%sDriver", (idm >> 4) & 0xf, (resource_value_t*)"nl10");
            return;
        case IDM_PRT4RAW:
        case IDM_PRT5RAW:
        case IDM_PRT6RAW:
        case IDM_PRTUPRAW:
            set_printer_res("Printer%sDriver", idm - IDM_PRT4RAW, (resource_value_t*)"raw");
            return;
        case IDM_PRT4TXT:
        case IDM_PRT5TXT:
        case IDM_PRT6TXT:
        case IDM_PRTUPTXT:
            set_printer_res("Printer%sOutput", (idm >> 4) & 0xf, (resource_value_t*)"text");
            return;
        case IDM_PRT4GFX:
        case IDM_PRT5GFX:
        case IDM_PRT6GFX:
        case IDM_PRTUPGFX:
            set_printer_res("Printer%sOutput", (idm >> 4) & 0xf, (resource_value_t*)"graphics");
            return;

#if defined __X128__ || defined __XVIC__
        case IDM_IEEE:
            toggle("IEEE488");
            return;
#endif // __X128__ || __XVIC__

        case IDM_VCACHE:
            toggle(VIDEO_CACHE);
            return;

#ifdef __X128__
        case IDM_C128INT:
        case IDM_C128FI:
        case IDM_C128FR:
        case IDM_C128GE:
        case IDM_C128IT:
        case IDM_C128NO:
        case IDM_C128SW:
            resources_set_int("MachineType", idm & 0xf);
            return;
        case IDM_VDCVCACHE:
            toggle("VDCVideoCache");
            return;
        case IDM_C128FULLBANKS:
            toggle("C128FullBanks");
            return;
#endif

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
            resources_set_int("RefreshRate", idm & 0xf);
            return;
        case IDM_MONITOR:
            {
                //
                // open monitor dialog
                //
                //int state = !WinIsWindowVisible(hwndMonitor);

                WinActivateWindow(hwndMonitor, 1);
                //
                // make sure that the build in monitor can be invoked
                //
                emulator_resume();

                //
                // trigger invokation of the build in monitor
                //
                monitor_startup_trap();
            }
            return;
        case IDM_DATASETTE:
            datasette_dialog(hwnd);
            return;
        case IDM_DRIVE:
            drive_dialog(hwnd);
            return;
        case IDM_SOUNDON:
            toggle("Sound");
            return;

        case IDM_SOUND_OUTPUT_SYSTEM:
            resources_set_int("SoundOutput", SOUND_OUTPUT_SYSTEM);
            return;
        case IDM_SOUND_OUTPUT_MONO:
            resources_set_int("SoundOutput", SOUND_OUTPUT_MONO);
            return;
        case IDM_SOUND_OUTPUT_STEREO:
            resources_set_int("SoundOutput", SOUND_OUTPUT_STEREO);
            return;
        case IDM_DRIVE_SOUND:
            toggle("DriveSoundEmulation");
            return;

#ifdef HAVE_RESID
        case IDM_RESID:
            toggle("SidEngine");
            return;
        case IDM_RESIDFAST:
        case IDM_RESIDINTERPOL:
        case IDM_RESIDRESAMPLE:
        case IDM_RESIDFASTRES:
            resources_set_int("SidResidSampling", idm - IDM_RESIDFAST);
            return;
        case IDM_RESIDPASS0:
        case IDM_RESIDPASS10:
        case IDM_RESIDPASS20:
        case IDM_RESIDPASS30:
        case IDM_RESIDPASS40:
        case IDM_RESIDPASS50:
        case IDM_RESIDPASS60:
        case IDM_RESIDPASS70:
        case IDM_RESIDPASS80:
        case IDM_RESIDPASS90:
            resources_set_int("SidResidPassband", (idm & 0xf) * 10);
            return;
#endif // HAVE_RESID

#if defined(__X64__) || defined(__X128__) || defined(__XCBM__) || defined(__X64DTV__) || defined(__XSCPU64__)
        case IDM_SIDFILTER:
            toggle("SidFilters");
            return;

#ifndef __X64DTV__
        case IDM_NO_EXTRA_SID:
            resources_set_int("SidStereo", 0);
            return;
        case IDM_STEREO:
            resources_set_int("SidStereo", 1);
            return;
        case IDM_TRIPLE:
            resources_set_int("SidStereo", 2);
            return;
#endif

        case IDM_SC6581:
        case IDM_SC8580:

#ifndef __X64DTV__
        case IDM_SC8580DB:
            resources_set_int("SidModel", idm - IDM_SC6581);
            return;
#else
        case IDM_SCDTV:
            resources_set_int("SidModel", 4);
            return;
#endif

#endif // __X64__ || __X128__ || __XCBM__ || __X64DTV__

        case IDM_SYNCFLEX:
        case IDM_SYNCADJUST:
        case IDM_SYNCEXACT:
            resources_set_int("SoundSpeedAdjustment", idm - IDM_SYNCFLEX);
            return;
        case IDM_FRAGVERYSMALL:
            resources_set_int("SoundFragmentSize", SOUND_FRAGMENT_VERY_SMALL);
            return;
        case IDM_FRAGSMALL:
            resources_set_int("SoundFragmentSize", SOUND_FRAGMENT_SMALL);
            return;
        case IDM_FRAGMEDIUM:
            resources_set_int("SoundFragmentSize", SOUND_FRAGMENT_MEDIUM);
            return;
        case IDM_FRAGLARGE:
            resources_set_int("SoundFragmentSize", SOUND_FRAGMENT_LARGE);
            return;
        case IDM_FRAGVERYLARGE:
            resources_set_int("SoundFragmentSize", SOUND_FRAGMENT_VERY_LARGE);
            return;
        case IDM_SR8000:
            resources_set_int("SoundSampleRate", 8000);
            return;
        case IDM_SR11025:
            resources_set_int("SoundSampleRate", 11025);
            return;
        case IDM_SR22050:
            resources_set_int("SoundSampleRate", 22050);
            return;
        case IDM_SR44100:
            resources_set_int("SoundSampleRate", 44100);
            return;
        case IDM_DEVDART:
            resources_set_string("SoundDeviceName", "dart");
            return;
        case IDM_DEVDUMMY:
            resources_set_string("SoundDeviceName", "dummy");
            return;
        case IDM_DEVSID: // no sampleparams
            resources_set_string("SoundDeviceName", "dump");
            return;
        case IDM_DEVWAV: // speed
            resources_set_string("SoundDeviceName", "wav");
            return;
        case IDM_DEVRAW: // no samplepar
            resources_set_string("SoundDeviceName", "fs");
            return;
        case IDM_BUF010:
        case IDM_BUF025:
        case IDM_BUF040:
        case IDM_BUF055:
        case IDM_BUF070:
        case IDM_BUF085:
        case IDM_BUF100:
            resources_set_int("SoundBufferSize", (idm & 0xff) * 50);
            return;
        case IDM_VOL100:
        case IDM_VOL90:
        case IDM_VOL80:
        case IDM_VOL70:
        case IDM_VOL60:
        case IDM_VOL50:
        case IDM_VOL40:
        case IDM_VOL30:
        case IDM_VOL20:
        case IDM_VOL10:
            set_volume(10 * (idm & 0xf));
            return;
        case IDM_MENUBAR:
            toggle("Menubar");
            return;
        case IDM_STRETCH1:
        case IDM_STRETCH2:
        case IDM_STRETCH3:
            resources_set_int("WindowStretchFactor", idm & 0xf);
            return;

#ifdef __XCBM__
        case IDM_MODEL750:
        case IDM_MODEL660:
        case IDM_MODEL650:
            resources_set_int("ModelLine", idm & 0xf);
            return;
        case IDM_RAM64:
        case IDM_RAM128:
        case IDM_RAM256:
        case IDM_RAM512:
        case IDM_RAM1024:
            resources_set_int("RamSize", (idm & 0xf) << 6);
            return;
        case IDM_RAM08:
            toggle("Ram08");
            return;
        case IDM_RAM10:
            toggle("Ram1");
            return;
        case IDM_RAM20:
            toggle("Ram2");
            return;
        case IDM_RAM40:
            toggle("Ram4");
            return;
        case IDM_RAM60:
            toggle("Ram6");
            return;
        case IDM_RAMC0:
            toggle("RamC");
            return;
#endif // __XCBM__

#ifdef __XPET__
        case IDM_CHARSET:
            toggle("Basic1Chars");
            return;
        case IDM_EOI:
            toggle("EoiBlank");
            return;
        case IDM_ROMPATCH:
            toggle("Basic1");
            return;
        case IDM_DIAGPIN:
            toggle("DiagPin");
            return;
        case IDM_SUPERPET:
            toggle("SuperPET");
            return;
        case IDM_MAP9:
            toggle("Ram9");
            return;
        case IDM_MAPA:
            toggle("RamA");
            return;
        case IDM_PETRAM4:
        case IDM_PETRAM8:
        case IDM_PETRAM16:
        case IDM_PETRAM32:
        case IDM_PETRAM96:
        case IDM_PETRAM128:
            resources_set_int("RamSize", idm & 0xff);
            return;
        case IDM_IOMEM256:
            resources_set_int("IOSize", 0x100);
            return;
        case IDM_IOMEM2K:
            resources_set_int("IOSize", 0x800);
            return;
        case IDM_CRTC:
            toggle("Crtc");
            return;
        case IDM_VSDETECT:
            resources_set_int("VideoSize", 0);
            return;
        case IDM_VS40:
            resources_set_int("VideoSize", 40);
            return;
        case IDM_VS80:
            resources_set_int("VideoSize", 80);
            return;
#endif // __XPET__

        case IDM_PAUSE:
            if (isEmulatorPaused()) {
                emulator_resume();
            } else {
                emulator_pause();
            }
            return;
        case IDM_TRUEDRIVE: // Switch True Drive Emulatin on/off
            toggle("DriveTrueEmulation");
            return;

#if defined(__XPET__) || defined(__XPLUS4__) || defined(__XVIC__)
        case IDM_SIDCART_ENABLE:
            toggle("SidCart");
            return;
        case IDM_SIDCART_6581:
            sid_set_engine_model(0, 0);
            return;
        case IDM_SIDCART_8580:
            sid_set_engine_model(0, 1);
            return;
        case IDM_SIDCART_FILTERS:
            toggle("SidFilters");
            return;
        case IDM_SIDCART_ADDRESS_1:
            resources_set_int("SidAddress", sidaddr1);
            return;
        case IDM_SIDCART_ADDRESS_2:
            resources_set_int("SidAddress", sidaddr2);
            return;
        case IDM_SIDCART_CLOCK_C64:
            resources_set_int("SidClock", 0);
            return;
        case IDM_SIDCART_CLOCK_NATIVE:
            resources_set_int("SidClock", 1);
            return;
        case IDM_DIGIBLASTER:
            toggle("DIGIBLASTER");
            return;
#endif

#ifdef HAS_JOYSTICK
        case IDM_JOYSTICK:
            joystick_dialog(hwnd);
            return;

#ifdef __XPLUS4__
        case IDM_SIDCART_JOY:
            toggle("SIDCartJoy");
            return;
        case IDM_USERPORT_JOY_SETUP:
            joystick_extra_dialog(hwnd);
            return;
#endif

#if !defined(__XPLUS4__) && !defined(__XCBM5X0__)
        case IDM_USERPORT_JOY:
            toggle("UserportJoy");
            return;
        case IDM_USERPORT_JOY_CGA:
            resources_set_int("UserportJoyType", USERPORT_JOYSTICK_CGA);
            return;
        case IDM_USERPORT_JOY_PET:
            resources_set_int("UserportJoyType", USERPORT_JOYSTICK_PET);
            return;
        case IDM_USERPORT_JOY_HUMMER:
            resources_set_int("UserportJoyType", USERPORT_JOYSTICK_HUMMER);
            return;
        case IDM_USERPORT_JOY_OEM:
            resources_set_int("UserportJoyType", USERPORT_JOYSTICK_OEM);
            return;
#if defined(__X64__) || defined(__X128__) || defined(__XSCPU64__)
        case IDM_USERPORT_JOY_HIT:
            resources_set_int("UserportJoyType", USERPORT_JOYSTICK_HIT);
            return;
        case IDM_USERPORT_JOY_KINGSOFT:
            resources_set_int("UserportJoyType", USERPORT_JOYSTICK_KINGSOFT);
            return;
        case IDM_USERPORT_JOY_STARBYTE:
            resources_set_int("UserportJoyType", USERPORT_JOYSTICK_STARBYTE);
            return;
#endif
        case IDM_USERPORT_JOY_SETUP:
            joystick_extra_dialog(hwnd);
            return;
#endif

        case IDM_ALLOW_OPPOSITE_JOY:
            toggle("JoyOpposite");
            return;
#endif

        case IDM_HLPINDEX:
            WinSendMsg(WinQueryHelpInstance(hwnd), HM_HELP_INDEX, 0, 0);
            return;
        case IDM_HLPGENERAL:
            WinSendMsg(WinQueryHelpInstance(hwnd), HM_DISPLAY_HELP, 0, 0);
            return;
        case IDM_CMDLINE:
            cmdline_show_help((void*)hwnd);
            return;
        case IDM_FSMODES:
            video_show_modes(hwnd);
            return;
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
    resources_get_int(name, &val);
    WinCheckMenuItem(hwnd, id, val);
}

void menu_select(HWND hwnd, USHORT item)
{
    int val;

    switch (item) {

#ifdef __XVIC__
        case IDM_VIC20_ATTACH_CARTRIDGE:
            resources_get_int("CartridgeReset", &val);
            WinCheckMenuItem(hwnd, IDM_VIC20_RESET_ON_CART_CHNG, val == 1);
            return;
#endif

#ifdef __XPLUS4__
        case IDM_PLUS4_ATTACH_CARTRIDGE:
            resources_get_int("CartridgeReset", &val);
            WinCheckMenuItem(hwnd, IDM_PLUS4_RESET_ON_CART_CHNG, val == 1);
            return;
#endif

#ifdef __XCBM__
        case IDM_CBM2_ATTACH_CARTRIDGE:
            resources_get_int("CartridgeReset", &val);
            WinCheckMenuItem(hwnd, IDM_CBM2_RESET_ON_CART_CHNG, val == 1);
            return;
#endif

#if defined(__X64__) || defined(__X128__) || defined(__XCBM2__) || defined(__XSCPU64__)
        case IDM_CIA1_MODEL:
            resources_get_int("CIA1Model", &val);
            WinCheckMenuItem(hwnd, IDM_CIA1_6526_OLD, val == 0);
            WinCheckMenuItem(hwnd, IDM_CIA1_6526A_NEW, val == 1);
            return;
        case IDM_CIA2_MODEL:
            resources_get_int("CIA2Model", &val);
            WinCheckMenuItem(hwnd, IDM_CIA2_6526_OLD, val == 0);
            WinCheckMenuItem(hwnd, IDM_CIA2_6526A_NEW, val == 1);
            return;
#endif

#if defined(__X64__) || defined(__XSCPU64__)
        case IDM_CUSTOM_C64_MODEL:
            resources_get_int("VICIINewLuminances", &val);
            WinCheckMenuItem(hwnd, IDM_VICII_NEW_LUMINANCES, val);
            return;
        case IDM_VICII_MODEL:
            resources_get_int("VICIIModel", &val);
            WinCheckMenuItem(hwnd, IDM_6569_PAL,          val == VICII_MODEL_6569);
            WinCheckMenuItem(hwnd, IDM_8565_PAL,          val == VICII_MODEL_8565);
            WinCheckMenuItem(hwnd, IDM_6569R1_OLD_PAL,    val == VICII_MODEL_6569R1);
            WinCheckMenuItem(hwnd, IDM_6567_NTSC,         val == VICII_MODEL_6567);
            WinCheckMenuItem(hwnd, IDM_8562_NTSC,         val == VICII_MODEL_8562);
            WinCheckMenuItem(hwnd, IDM_6567R56A_OLD_NTSC, val == VICII_MODEL_6567R56A);
            WinCheckMenuItem(hwnd, IDM_6572_PAL_N,        val == VICII_MODEL_6572);
            return;
        case IDM_GLUE_LOGIC:
            resources_get_int("GlueLogic", &val);
            WinCheckMenuItem(hwnd, IDM_DISCRETE, val == 0);
            WinCheckMenuItem(hwnd, IDM_CUSTOM_IC, val == 1);
            return;
#endif

#if defined(__X64__) || defined(__X128__) || defined(__XSCPU64__)
        case IDM_IDE64_SETTINGS:
            WinCheckRes(hwnd, IDM_IDE64_RTC_SAVE, "IDE64RTCSave");
            return;
        case IDM_IDE64_REVISION:
            resources_get_int("IDE64version4", &val);
            WinCheckMenuItem(hwnd, IDM_IDE64_VER_3, val == 0);
            WinCheckMenuItem(hwnd, IDM_IDE64_VER_4, val == 1);
            return;
        case IDM_IDE64_HD1:
            WinCheckRes(hwnd, IDM_IDE64_AUTO1, "IDE64AutodetectSize1");
            return;
        case IDM_IDE64_HD2:
            WinCheckRes(hwnd, IDM_IDE64_AUTO2, "IDE64AutodetectSize2");
            return;
        case IDM_IDE64_HD3:
            WinCheckRes(hwnd, IDM_IDE64_AUTO3, "IDE64AutodetectSize3");
            return;
        case IDM_IDE64_HD4:
            WinCheckRes(hwnd, IDM_IDE64_AUTO4, "IDE64AutodetectSize4");
            return;

        case IDM_MMC64_SETTINGS:
            WinCheckRes(hwnd, IDM_MMC64, "MMC64");
            WinCheckRes(hwnd, IDM_MMC64_FLASH_JUMPER, "MMC64_flashjumper");
            WinCheckRes(hwnd, IDM_MMC64_BIOS_WRITE, "MMC64_bios_write");
            WinCheckRes(hwnd, IDM_MMC64_SD_MMC_READONLY, "MMC64_RO");
            return;
        case IDM_MMC64_REVISION:
            resources_get_int("MMC64_revision", &val);
            WinCheckMenuItem(hwnd, IDM_MMC64_REV_A, val == 0);
            WinCheckMenuItem(hwnd, IDM_MMC64_REV_B, val == 1);
            return;
        case IDM_MMC64_SD_TYPE:
            resources_get_int("MMC64_sd_type", &val);
            WinCheckMenuItem(hwnd, IDM_MMC64_SD_TYPE_AUTO, val == 0);
            WinCheckMenuItem(hwnd, IDM_MMC64_SD_TYPE_MMC, val == 1);
            WinCheckMenuItem(hwnd, IDM_MMC64_SD_TYPE_SD, val == 2);
            WinCheckMenuItem(hwnd, IDM_MMC64_SD_TYPE_SDHC, val == 3);
            return;

        case IDM_RR_SETTINGS:
            WinCheckRes(hwnd, IDM_RR_FLASH_WRITE, "RRBiosWrite");
            WinCheckRes(hwnd, IDM_RR_FLASH_JUMPER, "RRFlashJumper");
            WinCheckRes(hwnd, IDM_RR_BANK_JUMPER, "RRBankJumper");
            resources_get_int("RRrevision", &val);
            WinCheckMenuItem(hwnd, IDM_RR_REV_RR, val == 0);
            WinCheckMenuItem(hwnd, IDM_RR_REV_NR, val == 1);
            return;

        case IDM_MMCR_SETTINGS:
            WinCheckRes(hwnd, IDM_MMCR_RESCUE_MODE, "MMCRRescueMode");
            WinCheckRes(hwnd, IDM_MMCR_EEPROM_WRITE, "MMCRImageWrite");
            WinCheckRes(hwnd, IDM_MMCR_EEPROM_READWRITE, "MMCREEPROMRW");
            WinCheckRes(hwnd, IDM_MMCR_SD_MMC_READWRITE, "MMCRCardRW");
            return;
        case IDM_MMCR_SD_TYPE:
            resources_get_int("MMCRSDType", &val);
            WinCheckMenuItem(hwnd, IDM_MMCR_SD_TYPE_AUTO, val == 0);
            WinCheckMenuItem(hwnd, IDM_MMCR_SD_TYPE_MMC, val == 1);
            WinCheckMenuItem(hwnd, IDM_MMCR_SD_TYPE_SD, val == 2);
            WinCheckMenuItem(hwnd, IDM_MMCR_SD_TYPE_SDHC, val == 3);
            return;

        case IDM_FILE:
            resources_get_int("CartridgeType", &val);
            WinEnableMenuItem(hwnd, IDM_CRTFREEZE, val == CARTRIDGE_ACTION_REPLAY ||
                                                   val == CARTRIDGE_KCS_POWER ||
                                                   val == CARTRIDGE_FINAL_III ||
                                                   val == CARTRIDGE_SUPER_SNAPSHOT ||
                                                   val == CARTRIDGE_SUPER_SNAPSHOT_V5 ||
                                                   val == CARTRIDGE_ATOMIC_POWER ||
                                                   val == CARTRIDGE_FINAL_I);
            WinEnableMenuItem(hwnd, IDM_CRTSAVEIMG, val == CARTRIDGE_EXPERT);
            return;
        case IDM_CARTRIDGE:
            resources_get_int("CartridgeType", &val);
            WinCheckMenuItem(hwnd, IDM_CRTGEN, val == CARTRIDGE_CRT);
            WinCheckMenuItem(hwnd, IDM_CRTGEN8KB, val == CARTRIDGE_GENERIC_8KB);
            WinCheckMenuItem(hwnd, IDM_CRTGEN16KB, val == CARTRIDGE_GENERIC_16KB);
            WinCheckMenuItem(hwnd, IDM_CRTACTREPL, val == CARTRIDGE_ACTION_REPLAY);
            WinCheckMenuItem(hwnd, IDM_CRTATOMPOW, val == CARTRIDGE_ATOMIC_POWER);
            WinCheckMenuItem(hwnd, IDM_CRTEPYX, val == CARTRIDGE_EPYX_FASTLOAD);
            WinCheckMenuItem(hwnd, IDM_CRTSSSHOT, val == CARTRIDGE_SUPER_SNAPSHOT);
            WinCheckMenuItem(hwnd, IDM_CRTSSSHOT5, val == CARTRIDGE_SUPER_SNAPSHOT_V5);
            WinCheckMenuItem(hwnd, IDM_CRTWEST, val == CARTRIDGE_WESTERMANN);
            WinCheckMenuItem(hwnd, IDM_CRTIEEE, val == CARTRIDGE_IEEE488);
            WinCheckMenuItem(hwnd, IDM_CRTIDE64, val == CARTRIDGE_IDE64);
            return;
#endif

        case IDM_DETACH:

#if defined(__X64__) || defined(__X128__) || defined(__XSCPU64__)
            resources_get_int("CartridgeType", &val);
            WinEnableMenuItem(hwnd, IDM_CARTRIDGEDET, val != CARTRIDGE_NONE);
#endif

            WinEnableMenuItem(hwnd, IDM_DETACHTAPE, tape_get_file_name());
            WinEnableMenuItem(hwnd, IDM_DETACH8, file_system_get_disk_name(8));
            WinEnableMenuItem(hwnd, IDM_DETACH9, file_system_get_disk_name(9));
            WinEnableMenuItem(hwnd, IDM_DETACH10, file_system_get_disk_name(10));
            WinEnableMenuItem(hwnd, IDM_DETACH11, file_system_get_disk_name(11));
            WinEnableMenuItem(hwnd, IDM_DETACHALL, file_system_get_disk_name(8) || file_system_get_disk_name(9) || file_system_get_disk_name(10) || file_system_get_disk_name(11));
            return;
        case IDM_VIEW:
            WinEnableMenuItem(hwnd, IDM_LOGWIN, hwndLog != NULLHANDLE);
            WinEnableMenuItem(hwnd, IDM_MONITOR, hwndMonitor != NULLHANDLE);

#if defined(__XPET__) || defined(__XPLUS4__) || defined(__XVIC__)
            WinCheckRes(hwnd, IDM_SIDCART_ENABLE, "SidCart");
            resources_get_int("SidCart", &val);
#ifdef __XPLUS4__
            WinEnableMenuItem(hwnd, IDM_SIDCART_JOY, val);
#endif
            WinEnableMenuItem(hwnd, IDM_SIDCART_MODEL, val);
            WinEnableMenuItem(hwnd, IDM_SIDCART_FILTERS, val);
            WinEnableMenuItem(hwnd, IDM_SIDCART_ADDRESS, val);
            WinEnableMenuItem(hwnd, IDM_SIDCART_CLOCK, val);
            WinEnableMenuItem(hwnd, IDM_DIGIBLASTER, val);
            resources_get_int("SidModel", &val);
            WinCheckMenuItem(hwnd, IDM_SIDCART_6581, val == 0);
            WinCheckMenuItem(hwnd, IDM_SIDCART_8580, val == 1);
            WinCheckRes(hwnd, IDM_SIDCART_FILTERS, "SidFilters");
            resources_get_int("SidAddress", &val);
            WinCheckMenuItem(hwnd, IDM_SIDCART_ADDRESS_1, (val == sidaddr1));
            WinCheckMenuItem(hwnd, IDM_SIDCART_ADDRESS_2, (val == sidaddr2));
            resources_get_int("SidClock", &val);
            WinCheckMenuItem(hwnd, IDM_SIDCART_CLOCK_C64, !val);
            WinCheckMenuItem(hwnd, IDM_SIDCART_CLOCK_NATIVE, val);
            WinCheckRes(hwnd, IDM_DIGIBLASTER, "DIGIBLASTER");
#endif

#ifdef HAS_JOYSTICK
#ifdef __XPLUS4__
            WinCheckRes(hwnd, IDM_USERPORT_JOY, "SIDCartJoy");
            resources_get_int("SIDCartJoy", &val);
            WinEnableMenuItem(hwnd, IDM_USERPORT_JOY_SETUP, val);
#endif

#if !defined(__XPLUS4__) && !defined(__XCBM5X0__)
            WinCheckRes(hwnd, IDM_USERPORT_JOY, "UserportJoy");
            resources_get_int("UserportJoy", &val);
            WinEnableMenuItem(hwnd, IDM_USERPORT_JOY_SETUP, val);
            WinEnableMenuItem(hwnd, IDM_USERPORT_JOY_TYPE, val);
            resources_get_int("UserportJoyType", &val);
            WinCheckMenuItem(hwnd, IDM_USERPORT_JOY_CGA, val == USERPORT_JOYSTICK_CGA);
            WinCheckMenuItem(hwnd, IDM_USERPORT_JOY_PET, val == USERPORT_JOYSTICK_PET);
            WinCheckMenuItem(hwnd, IDM_USERPORT_JOY_HUMMER, val == USERPORT_JOYSTICK_HUMMER);
            WinCheckMenuItem(hwnd, IDM_USERPORT_JOY_OEM, val == USERPORT_JOYSTICK_OEM);
#if defined(__X64__) || defined(__X128__) || defined(__XSCPU64__)
            WinCheckMenuItem(hwnd, IDM_USERPORT_JOY_HIT, val == USERPORT_JOYSTICK_HIT);
            WinCheckMenuItem(hwnd, IDM_USERPORT_JOY_KINGSOFT, val == USERPORT_JOYSTICK_KINGSOFT);
            WinCheckMenuItem(hwnd, IDM_USERPORT_JOY_STARBYTE, val == USERPORT_JOYSTICK_STARBYTE);
#endif
#endif

            resources_get_int("JoyOpposite", &val);
            WinCheckMenuItem(hwnd, IDM_ALLOW_OPPOSITE_JOY, val);
#endif
            return;

#if defined(__X64__) || defined(__X128__) || defined(__XVIC__) || defined(__X64DTV__) || defined(__XSCPU64__)
        case IDM_VIDEOSTD:
            resources_get_int("MachineVideoStandard", &val);
            WinCheckMenuItem(hwnd, IDM_PAL, val == MACHINE_SYNC_PAL);
            WinCheckMenuItem(hwnd, IDM_NTSC, val == MACHINE_SYNC_NTSC);

#if defined(__X64__) || defined(__XSCPU64__)
            WinCheckMenuItem(hwnd, IDM_NTSCOLD, val == MACHINE_SYNC_NTSCOLD);
#endif

            return;
#endif

        case IDM_SETUP:

#ifdef HAVE_MOUSE
            WinCheckRes(hwnd, IDM_MOUSE, "Mouse");
            WinCheckRes(hwnd, IDM_HIDEMOUSE, "HideMousePtr");
            WinCheckRes(hwnd, IDM_SMART_MOUSE_RTC_SAVE, "SmartMouseRTCSave");

#if defined(__X128__) || defined(__X64__) || defined(__XSCPU64__)
            WinEnableMenuItem(hwnd, IDM_MOUSE_TYPE, 1);
            WinEnableMenuItem(hwnd, IDM_MOUSE_PORT, 1);
#endif
#endif // HAVE_MOUSE

#ifdef __X128__
            WinCheckRes(hwnd, IDM_C128FULLBANKS, "C128FullBanks");
#endif

            WinCheckMenuItem(hwnd, IDM_PAUSE, isEmulatorPaused());
            WinCheckRes(hwnd, IDM_MENUBAR, "Menubar");

#if defined __X128__ || defined __XVIC__
            WinCheckRes(hwnd, IDM_IEEE, "IEEE488");
#endif // __X128__ || __XVIC__

#if defined(__X64__) || defined(__X128__) || defined(__XVIC__) || defined(__XSCPU64__)
            resources_get_int("GEORAM", &val);
            WinCheckMenuItem(hwnd, IDM_GEORAM, val);
            WinEnableMenuItem(hwnd, IDM_GEORAMSIZE, val);
            WinEnableMenuItem(hwnd, IDM_SAVE_GEORAM, val);
            WinCheckRes(hwnd, IDM_SAVE_GEORAM, "GEORAMImageWrite");
            WinEnableMenuItem(hwnd, IDM_GEORAMFILE, val);
#ifdef __XVIC__
            WinEnableMenuItem(hwnd, IDM_GEORAM_IO_SWAP, val);
            WinCheckRes(hwnd, IDM_GEORAM_IO_SWAP, "GEORAMIOSwap");
#endif
            resources_get_int("SFXSoundExpander", &val);
            WinCheckMenuItem(hwnd, IDM_SFX_SE, val);
            WinEnableMenuItem(hwnd, IDM_SFX_SE_TYPE, val);
#ifdef __XVIC__
            WinEnableMenuItem(hwnd, IDM_SFX_SE_IO_SWAP, val);
            WinCheckRes(hwnd, IDM_SFX_SE_IO_SWAP, "SFXSoundExpanderIOSwap");
#endif
            resources_get_int("SFXSoundSampler", &val);
            WinCheckMenuItem(hwnd, IDM_SFX_SS, val);
#ifdef __XVIC__
            WinEnableMenuItem(hwnd, IDM_SFX_SS_IO_SWAP, val);
            WinCheckRes(hwnd, IDM_SFX_SS_IO_SWAP, "SFXSoundSamplerIOSwap");
#endif
            resources_get_int("DIGIMAX", &val);
            WinCheckMenuItem(hwnd, IDM_DIGIMAX, val);
            WinEnableMenuItem(hwnd, IDM_DIGIMAXBASE, val);
            resources_get_int("DS12C887RTC", &val);
            WinCheckMenuItem(hwnd, IDM_DS12C887RTC, val);
            WinEnableMenuItem(hwnd, IDM_DS12C887RTCBASE, val);
            WinEnableMenuItem(hwnd, IDM_DS12C887RTC_SAVE, val);
#ifdef HAVE_TFE
            resources_get_int("ETHERNET_ACTIVE", &val);
            WinCheckMenuItem(hwnd, IDM_TFE, val);
#ifdef __XVIC__
            WinEnableMenuItem(hwnd, IDM_TFE_IO_SWAP, val);
            WinCheckRes(hwnd, IDM_TFE_IO_SWAP, "TFEIOSwap");
#endif
#endif
#endif

#if defined(__X64__) || defined(__XSCPU64__)
            resources_get_int("BurstMod", &val);
            WinCheckMenuItem(hwnd, IDM_BURST_NONE, val == 0);
            WinCheckMenuItem(hwnd, IDM_BURST_CIA1, val == 1);
            WinCheckMenuItem(hwnd, IDM_BURST_CIA2, val == 2);
#endif

#ifdef __XSCPU64__
            resources_get_int("SIMMSize", &val);
            WinCheckMenuItem(hwnd, IDM_SCPU64_SIMM_0, val == 0);
            WinCheckMenuItem(hwnd, IDM_SCPU64_SIMM_1, val == 1);
            WinCheckMenuItem(hwnd, IDM_SCPU64_SIMM_4, val == 4);
            WinCheckMenuItem(hwnd, IDM_SCPU64_SIMM_8, val == 8);
            WinCheckMenuItem(hwnd, IDM_SCPU64_SIMM_16, val == 16);
            WinCheckRes(hwnd, IDM_SCPU64_JIFFY_SWITCH, "JiffySwitch");
            WinCheckRes(hwnd, IDM_SCPU64_SPEED_SWITCH, "SpeedSwitch");
#endif

#if defined(__X64__) || defined(__X128__) || defined(__XSCPU64__)
            resources_get_int("REU", &val);
            WinCheckMenuItem(hwnd, IDM_REU, val);
            WinEnableMenuItem(hwnd, IDM_REUSIZE, val);
            WinEnableMenuItem(hwnd, IDM_SAVE_REU, val);
            WinCheckRes(hwnd, IDM_SAVE_REU, "REUImageWrite");
            WinEnableMenuItem(hwnd, IDM_REUFILE, val);
            resources_get_int("RAMCART", &val);
            WinCheckMenuItem(hwnd, IDM_RAMCART, val);
            WinEnableMenuItem(hwnd, IDM_RAMCARTSIZE, val);
            WinEnableMenuItem(hwnd, IDM_SAVE_RAMCART, val);
            WinCheckRes(hwnd, IDM_SAVE_RAMCART, "RAMCARTImageWrite");
            WinEnableMenuItem(hwnd, IDM_RAMCARTFILE, val);
            resources_get_int("MagicVoiceCartridgeEnabled", &val);
            WinCheckMenuItem(hwnd, IDM_MAGICVOICE, val);
            resources_get_int("EasyFlashJumper", &val);
            WinCheckMenuItem(hwnd, IDM_EASYFLASH_JUMPER, val);
            resources_get_int("EasyFlashWriteCRT", &val);
            WinCheckMenuItem(hwnd, IDM_EASYFLASH_AUTOSAVE, val);
            resources_get_int("UserportRTC", &val);
            WinCheckMenuItem(hwnd, IDM_USERPORT_RTC, val);
            resources_get_int("UserportRTCSave", &val);
            WinCheckMenuItem(hwnd, IDM_USERPORT_RTC_SAVE, val);
#endif

#if defined(__X64__) || defined(__XSCPU64__)
            resources_get_int("DQBB", &val);
            WinCheckMenuItem(hwnd, IDM_DQBB, val);
            WinEnableMenuItem(hwnd, IDM_SAVE_DQBB, val);
            WinCheckRes(hwnd, IDM_SAVE_DQBB, "DQBBImageWrite");
            WinEnableMenuItem(hwnd, IDM_DQBBFILE, val);
            resources_get_int("IsepicCartridgeEnabled", &val);
            WinCheckMenuItem(hwnd, IDM_ISEPIC, val);
            resources_get_int("IsepicSwitch", &val);
            WinCheckMenuItem(hwnd, IDM_ISEPIC_SWITCH, val);
            WinEnableMenuItem(hwnd, IDM_SAVE_ISEPIC, val);
            WinCheckRes(hwnd, IDM_SAVE_ISEPIC, "IsepicImageWrite");
            WinEnableMenuItem(hwnd, IDM_ISEPICFILE, val);
            resources_get_int("ExpertCartridgeEnabled", &val);
            WinCheckMenuItem(hwnd, IDM_EXPERT, val);
            WinEnableMenuItem(hwnd, IDM_EXPERT_MODE, val);
            WinEnableMenuItem(hwnd, IDM_SAVE_EXPERT, val);
            WinCheckRes(hwnd, IDM_SAVE_EXPERT, "ExpertImageWrite");
            WinEnableMenuItem(hwnd, IDM_EXPERTFILE, val);
#ifdef __X64__
            resources_get_int("MemoryHack", &val);
            WinEnableMenuItem(hwnd, IDM_C64_256K_BASE, val == MEMORY_HACK_C64_256K);
            WinEnableMenuItem(hwnd, IDM_C64_256KFILE, val == MEMORY_HACK_C64_256K);
            WinEnableMenuItem(hwnd, IDM_PLUS60KBASE, val == MEMORY_HACK_PLUS60K);
            WinEnableMenuItem(hwnd, IDM_PLUS60KFILE, val == MEMORY_HACK_PLUS60K);
            WinEnableMenuItem(hwnd, IDM_PLUS256KFILE, val == MEMORY_HACK_PLUS256K);
#endif

#ifdef __X64SC__
            val = c64model_get();
            WinCheckMenuItem(hwnd, IDM_C64PAL, val == C64MODEL_C64_PAL);
            WinCheckMenuItem(hwnd, IDM_C64CPAL, val == C64MODEL_C64C_PAL);
            WinCheckMenuItem(hwnd, IDM_C64OLDPAL, val == C64MODEL_C64_OLD_PAL);
            WinCheckMenuItem(hwnd, IDM_C64NTSC, val == C64MODEL_C64_NTSC);
            WinCheckMenuItem(hwnd, IDM_C64CNTSC, val == C64MODEL_C64C_NTSC);
            WinCheckMenuItem(hwnd, IDM_C64OLDNTSC, val == C64MODEL_C64_OLD_NTSC);
            WinCheckMenuItem(hwnd, IDM_DREAN, val == C64MODEL_C64_PAL_N);
            WinCheckMenuItem(hwnd, IDM_C64SXPAL, val == C64MODEL_C64SX_PAL);
            WinCheckMenuItem(hwnd, IDM_C64SXNTSC, val == C64MODEL_C64SX_NTSC);
            WinCheckMenuItem(hwnd, IDM_C64JAP, val == C64MODEL_C64_JAP);
            WinCheckMenuItem(hwnd, IDM_C64GS, val == C64MODEL_C64_GS);
            WinCheckMenuItem(hwnd, IDM_PET64PAL, val == C64MODEL_PET64_PAL);
            WinCheckMenuItem(hwnd, IDM_PET64NTSC, val == C64MODEL_PET64_NTSC);
            WinCheckMenuItem(hwnd, IDM_ULTIMAX, val == C64MODEL_ULTIMAX);
            resources_get_int("VICIIModel", &val);
            WinCheckMenuItem(hwnd, IDM_6569_PAL, val == VICII_MODEL_6569);
            WinCheckMenuItem(hwnd, IDM_8565_PAL, val == VICII_MODEL_8565);
            WinCheckMenuItem(hwnd, IDM_6569R1_OLD_PAL, val == VICII_MODEL_6569R1);
            WinCheckMenuItem(hwnd, IDM_6567_NTSC, val == VICII_MODEL_6567);
            WinCheckMenuItem(hwnd, IDM_8562_NTSC, val == VICII_MODEL_8562);
            WinCheckMenuItem(hwnd, IDM_6567R56A_OLD_NTSC, val == VICII_MODEL_6567R56A);
            WinCheckMenuItem(hwnd, IDM_6572_PAL_N, val == VICII_MODEL_6572);
#endif
#endif

#ifdef __X128__
            val = c128model_get();
            WinCheckMenuItem(hwnd, IDM_C128PAL, val == C128MODEL_C128_PAL);
            WinCheckMenuItem(hwnd, IDM_C128DCRPAL, val == C128MODEL_C128DCR_PAL);
            WinCheckMenuItem(hwnd, IDM_C128NTSC, val == C128MODEL_C128_NTSC);
            WinCheckMenuItem(hwnd, IDM_C128DCRNTSC, val == C128MODEL_C128DCR_NTSC);
#endif

#ifdef __XPLUS4__
            resources_get_int("SpeechEnabled", &val);
            WinCheckMenuItem(hwnd, IDM_V364SPEECH, val);
            val = plus4model_get();
            WinCheckMenuItem(hwnd, IDM_C16PAL, val == PLUS4MODEL_C16_PAL);
            WinCheckMenuItem(hwnd, IDM_C16NTSC, val == PLUS4MODEL_C16_NTSC);
            WinCheckMenuItem(hwnd, IDM_PLUS4PAL, val == PLUS4MODEL_PLUS4_PAL);
            WinCheckMenuItem(hwnd, IDM_PLUS4NTSC, val == PLUS4MODEL_PLUS4_NTSC);
            WinCheckMenuItem(hwnd, IDM_V364NTSC, val == PLUS4MODEL_V364_NTSC);
            WinCheckMenuItem(hwnd, IDM_C232NTSC, val == PLUS4MODEL_232_NTSC);
#endif

#ifdef __XPET__
            val = petmodel_get();
            WinCheckMenuItem(hwnd, IDM_PET2001, val == PETMODEL_2001);
            WinCheckMenuItem(hwnd, IDM_PET3008, val == PETMODEL_3008);
            WinCheckMenuItem(hwnd, IDM_PET3016, val == PETMODEL_3016);
            WinCheckMenuItem(hwnd, IDM_PET3032, val == PETMODEL_3032);
            WinCheckMenuItem(hwnd, IDM_PET3032B, val == PETMODEL_3032B);
            WinCheckMenuItem(hwnd, IDM_PET4016, val == PETMODEL_4016);
            WinCheckMenuItem(hwnd, IDM_PET4032, val == PETMODEL_4032);
            WinCheckMenuItem(hwnd, IDM_PET4032B, val == PETMODEL_4032B);
            WinCheckMenuItem(hwnd, IDM_PET8032, val == PETMODEL_8032);
            WinCheckMenuItem(hwnd, IDM_PET8096, val == PETMODEL_8096);
            WinCheckMenuItem(hwnd, IDM_PET8296, val == PETMODEL_8296);
            WinCheckMenuItem(hwnd, IDM_PETSUPER, val == PETMODEL_SUPERPET);
#endif

#ifdef __XCBM__
            val = cbm2model_get();
            WinCheckMenuItem(hwnd, IDM_CBM510PAL, val == CBM2MODEL_510_PAL);
            WinCheckMenuItem(hwnd, IDM_CBM510NTSC, val == CBM2MODEL_510_NTSC);
            WinCheckMenuItem(hwnd, IDM_CBM610PAL, val == CBM2MODEL_610_PAL);
            WinCheckMenuItem(hwnd, IDM_CBM610NTSC, val == CBM2MODEL_610_NTSC);
            WinCheckMenuItem(hwnd, IDM_CBM620PAL, val == CBM2MODEL_620_PAL);
            WinCheckMenuItem(hwnd, IDM_CBM620NTSC, val == CBM2MODEL_620_NTSC);
            WinCheckMenuItem(hwnd, IDM_CBM620PLUSPAL, val == CBM2MODEL_620PLUS_PAL);
            WinCheckMenuItem(hwnd, IDM_CBM620PLUSNTSC, val == CBM2MODEL_620PLUS_NTSC);
            WinCheckMenuItem(hwnd, IDM_CBM710NTSC, val == CBM2MODEL_710_NTSC);
            WinCheckMenuItem(hwnd, IDM_CBM720NTSC, val == CBM2MODEL_720_NTSC);
            WinCheckMenuItem(hwnd, IDM_CBM720PLUSNTSC, val == CBM2MODEL_720PLUS_NTSC);
#endif

#ifdef __X64DTV__
            val = dtvmodel_get();
            WinCheckMenuItem(hwnd, IDM_DTV2PAL, val == DTVMODEL_V2_PAL);
            WinCheckMenuItem(hwnd, IDM_DTV2NTSC, val == DTVMODEL_V2_NTSC);
            WinCheckMenuItem(hwnd, IDM_DTV3PAL, val == DTVMODEL_V3_PAL);
            WinCheckMenuItem(hwnd, IDM_DTV3NTSC, val == DTVMODEL_V3_NTSC);
            WinCheckMenuItem(hwnd, IDM_HUMMERNTSC, val == DTVMODEL_HUMMER_NTSC);

            resources_get_int("c64dtvromrw", &val);
            WinCheckMenuItem(hwnd, IDM_C64DTV_FLASHROM_RW, val);
#endif

#ifdef __XVIC__
            val = vic20model_get();
            WinCheckMenuItem(hwnd, IDM_VIC20PAL, val == VIC20MODEL_VIC20_PAL);
            WinCheckMenuItem(hwnd, IDM_VIC20NTSC, val == VIC20MODEL_VIC20_NTSC);
            WinCheckMenuItem(hwnd, IDM_VIC21, val == VIC20MODEL_VIC21);
#endif

#ifdef __XPET__
            resources_get_int("PETREU", &val);
            WinCheckMenuItem(hwnd, IDM_PETREU, val);
            WinEnableMenuItem(hwnd, IDM_PETREUSIZE, val);
            WinEnableMenuItem(hwnd, IDM_PETREUFILE, val);
            resources_get_int("PETDWW", &val);
            WinCheckMenuItem(hwnd, IDM_PETDWW, val);
            WinEnableMenuItem(hwnd, IDM_PETDWWFILE, val);
            resources_get_int("PETHRE", &val);
            WinCheckMenuItem(hwnd, IDM_PETHRE, val);
            WinCheckRes(hwnd, IDM_CHARSET, "Basic1Chars");
            WinCheckRes(hwnd, IDM_EOI, "EoiBlank");
            WinCheckRes(hwnd, IDM_ROMPATCH, "Basic1");
            WinCheckRes(hwnd, IDM_DIAGPIN, "DiagPin");
            WinCheckRes(hwnd, IDM_SUPERPET, "SuperPET");
            WinCheckRes(hwnd, IDM_CRTC, "Crtc");
            resources_get_int("UserportDAC", &val);
            WinCheckMenuItem(hwnd, IDM_PET_USERPORT_DAC, val);
#endif // __XPET__

#ifdef __XVIC__
            WinCheckRes(hwnd, IDM_VIC20_FE_WRITEBACK, "FinalExpansionWriteBack");
            WinCheckRes(hwnd, IDM_VIC20_VFP_WRITEBACK, "VicFlashPluginWriteBack");
            WinCheckRes(hwnd, IDM_VIC20_MC_WRITEBACK, "MegaCartNvRAMWriteBack");
#endif

            WinCheckRes(hwnd, IDM_AUTOSTART_WARP, "AutostartWarp");
            WinCheckRes(hwnd, IDM_RUN_WITH_COLON, "AutostartRunWithColon");
            WinCheckRes(hwnd, IDM_LOAD_TO_BASIC_START, "AutostartBasicLoad");
            WinCheckRes(hwnd, IDM_AUTOSTART_RANDOM_DELAY, "AutostartDelayRandom");

            resources_get_int("AutostartPrgMode", &val);
            WinCheckMenuItem(hwnd, IDM_AUTOSTART_PRG_VIRTUAL_FS, val == 0);
            WinCheckMenuItem(hwnd, IDM_AUTOSTART_PRG_INJECT, val == 1);
            WinCheckMenuItem(hwnd, IDM_AUTOSTART_PRG_DISK_IMAGE, val == 2);
            return;

#if defined(HAVE_MOUSE) && (defined(__X64__) || defined(__X128__) || defined(__XSCPU64__))
        case IDM_MOUSE_TYPE:
            resources_get_int("Mousetype", &val);
            WinCheckMenuItem(hwnd, IDM_MOUSE_TYPE_1351, val == MOUSE_TYPE_1351);
            WinCheckMenuItem(hwnd, IDM_MOUSE_TYPE_NEOS, val == MOUSE_TYPE_NEOS);
            WinCheckMenuItem(hwnd, IDM_MOUSE_TYPE_AMIGA, val == MOUSE_TYPE_AMIGA);
            WinCheckMenuItem(hwnd, IDM_MOUSE_TYPE_PADDLE, val == MOUSE_TYPE_PADDLE);
            WinCheckMenuItem(hwnd, IDM_MOUSE_TYPE_CX22, val == MOUSE_TYPE_CX22);
            WinCheckMenuItem(hwnd, IDM_MOUSE_TYPE_ST, val == MOUSE_TYPE_ST);
            WinCheckMenuItem(hwnd, IDM_MOUSE_TYPE_SMART, val == MOUSE_TYPE_SMART);
            WinCheckMenuItem(hwnd, IDM_MOUSE_TYPE_MICROMYS, val == MOUSE_TYPE_MICROMYS);
            WinCheckMenuItem(hwnd, IDM_MOUSE_TYPE_KOALAPAD, val == MOUSE_TYPE_KOALAPAD);
            return;
        case IDM_MOUSE_PORT:
            resources_get_int("Mouseport", &val);
            WinCheckMenuItem(hwnd, IDM_MOUSE_PORT_1, val == 1);
            WinCheckMenuItem(hwnd, IDM_MOUSE_PORT_2, val == 2);
            return;
#endif

        case IDM_PRINTER4:
        case IDM_PRINTER5:
        case IDM_PRINTER6:
        case IDM_PRINTERUP:
            {
                const int num = (item >> 4) & 0xf;

                get_printer_res("Printer%s", num, (resource_value_t*)&val);

                WinCheckMenuItem(hwnd, IDM_PRT4IEC | (num << 4), val);
                WinEnableMenuItem(hwnd, IDM_PRT4DRV | (num << 4), val);
                WinEnableMenuItem(hwnd, IDM_PRT4OUT | (num << 4), val);
            }
            return;
        case IDM_PRT4DRV:
        case IDM_PRT5DRV:
        case IDM_PRT6DRV:
        case IDM_PRTUPDRV:
            {
                const int num = (item >> 4) & 0xf;
                char *txt;

                get_printer_res("Printer%sDriver", num, (resource_value_t*)&txt);
                WinCheckMenuItem(hwnd, IDM_PRT4ASCII | (num << 4), !strcasecmp(txt, "ascii"));
                WinCheckMenuItem(hwnd, IDM_PRT4MPS803 | (num << 4), !strcasecmp(txt, "mps803"));
                WinCheckMenuItem(hwnd, IDM_PRT4NL10 | (num << 4), !strcasecmp(txt, "nl10"));
                WinCheckMenuItem(hwnd, IDM_PRT6_1520, !strcasecmp(txt, "1520"));
            }
        case IDM_PRT4OUT:
        case IDM_PRT5OUT:
        case IDM_PRT6OUT:
        case IDM_PRTUPOUT:
            {
                const int num = (item >> 4) & 0xf;
                char *txt;

                get_printer_res("Printer%sOutput", num, (resource_value_t*)&txt);
                WinCheckMenuItem(hwnd, IDM_PRT4TXT | (num << 4), !strcasecmp(txt, "text"));
                WinCheckMenuItem(hwnd, IDM_PRT4GFX | (num << 4), !strcasecmp(txt, "graphics"));

                get_printer_res("Printer%sDriver", num, (resource_value_t*)&txt);
                WinEnableMenuItem(hwnd, IDM_PRT4GFX | (num << 4), strcasecmp(txt, "ascii"));
            }

#ifdef __XPET__
        case IDM_PETREUSIZE:
            resources_get_int("PETREUSize", &val);
            WinCheckMenuItem(hwnd, IDM_PETREU128, val == 128);
            WinCheckMenuItem(hwnd, IDM_PETREU512, val == 512);
            WinCheckMenuItem(hwnd, IDM_PETREU1024, val == 1024);
            WinCheckMenuItem(hwnd, IDM_PETREU2048, val == 2048);
            return;
#endif

#ifdef __X64DTV__
        case IDM_C64DTV_REVISION:
            resources_get_int("DtvRevision", &val);
            WinCheckMenuItem(hwnd, IDM_DTV2, val == 2);
            WinCheckMenuItem(hwnd, IDM_DTV3, val == 3);
            return;
#endif

#if defined(__X64__) || defined(__X128__) || defined(__XVIC__) || defined(__XSCPU64__)
        case IDM_GEORAMSIZE:
            resources_get_int("GEORAMsize", &val);
            WinCheckMenuItem(hwnd, IDM_GEORAM64, val == 64);
            WinCheckMenuItem(hwnd, IDM_GEORAM128, val == 128);
            WinCheckMenuItem(hwnd, IDM_GEORAM256, val == 256);
            WinCheckMenuItem(hwnd, IDM_GEORAM512, val == 512);
            WinCheckMenuItem(hwnd, IDM_GEORAM1024, val == 1024);
            WinCheckMenuItem(hwnd, IDM_GEORAM2048, val == 2048);
            WinCheckMenuItem(hwnd, IDM_GEORAM4096, val == 4096);
            return;
        case IDM_SFX_SE_TYPE:
            resources_get_int("SFXSoundExpanderChip", &val);
            WinCheckMenuItem(hwnd, IDM_SFX_SE_3526, val == 3526);
            WinCheckMenuItem(hwnd, IDM_SFX_SE_3812, val == 3812);
            return;
        case IDM_DS12C887RTC_SETTINGS:
            resources_get_int("DS12C887RTCSave", &val);
            WinCheckMenuItem(hwnd, IDM_DS12C887RTC_SETTINGS, val);
            return;
#ifndef __XVIC__
        case IDM_DIGIMAXBASE:
            resources_get_int("DIGIMAXbase", &val);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDD00, val == 0xdd00);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDE00, val == 0xde00);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDE20, val == 0xde20);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDE40, val == 0xde40);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDE60, val == 0xde60);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDE80, val == 0xde80);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDEA0, val == 0xdea0);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDEC0, val == 0xdec0);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDEE0, val == 0xdee0);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDF00, val == 0xdf00);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDF20, val == 0xdf20);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDF40, val == 0xdf40);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDF60, val == 0xdf60);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDF80, val == 0xdf80);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDFA0, val == 0xdfa0);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDFC0, val == 0xdfc0);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDFE0, val == 0xdfe0);
            return;
#else
        case IDM_DIGIMAXBASE:
            resources_get_int("DIGIMAXbase", &val);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDE00, val == 0x9800);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDE20, val == 0x9820);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDE40, val == 0x9840);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDE60, val == 0x9860);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDE80, val == 0x9880);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDEA0, val == 0x98a0);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDEC0, val == 0x98c0);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDEE0, val == 0x98e0);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDF00, val == 0x9c00);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDF20, val == 0x9c20);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDF40, val == 0x9c40);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDF60, val == 0x9c60);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDF80, val == 0x9c80);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDFA0, val == 0x9ca0);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDFC0, val == 0x9cc0);
            WinCheckMenuItem(hwnd, IDM_DIGIMAXDFE0, val == 0x9ce0);
            return;
#endif
#ifndef __XVIC__
        case IDM_DS12C887RTCBASE:
            resources_get_int("DS12C887RTCbase", &val);
#ifndef __X128__
            WinCheckMenuItem(hwnd, IDM_DS12C887RTCD500, val == 0xd500);
            WinCheckMenuItem(hwnd, IDM_DS12C887RTCD600, val == 0xd600);
#endif
            WinCheckMenuItem(hwnd, IDM_DS12C887RTCD700, val == 0xd700);
            WinCheckMenuItem(hwnd, IDM_DS12C887RTCDE00, val == 0xde00);
            WinCheckMenuItem(hwnd, IDM_DS12C887RTCDF00, val == 0xdf00);
            return;
#else
        case IDM_DS12C887RTCBASE:
            resources_get_int("DS12C887RTCbase", &val);
            WinCheckMenuItem(hwnd, IDM_DS12C887RTC9800, val == 0x9800);
            WinCheckMenuItem(hwnd, IDM_DS12C887RTC9C00, val == 0x9c00);
            return;
#endif
#endif

#if defined(__X64__) || defined(__X128__) || defined(__XSCPU64__)
        case IDM_REUSIZE:
            resources_get_int("REUsize", &val);
            WinCheckMenuItem(hwnd, IDM_REU128, val == 128);
            WinCheckMenuItem(hwnd, IDM_REU256, val == 256);
            WinCheckMenuItem(hwnd, IDM_REU512, val == 512);
            WinCheckMenuItem(hwnd, IDM_REU1024, val == 1024);
            WinCheckMenuItem(hwnd, IDM_REU2048, val == 2048);
            WinCheckMenuItem(hwnd, IDM_REU4096, val == 4096);
            WinCheckMenuItem(hwnd, IDM_REU8192, val == 8192);
            WinCheckMenuItem(hwnd, IDM_REU16384, val == 16384);
            return;
        case IDM_RAMCARTSIZE:
            resources_get_int("RAMCARTsize", &val);
            WinCheckMenuItem(hwnd, IDM_RAMCART64, val == 64);
            WinCheckMenuItem(hwnd, IDM_RAMCART128, val == 128);
            return;

#if defined(__X64__) || defined(__XSCPU64__)
        case IDM_EXPERT_MODE:
            resources_get_int("ExpertCartridgeMode", &val);
            WinCheckMenuItem(hwnd, IDM_EXPERT_MODE_OFF, val == 0);
            WinCheckMenuItem(hwnd, IDM_EXPERT_MODE_PRG, val == 1);
            WinCheckMenuItem(hwnd, IDM_EXPERT_MODE_ON, val == 2);
            return;
#ifdef __X64__
        case IDM_PLUS60KBASE:
            resources_get_int("PLUS60Kbase", &val);
            WinCheckMenuItem(hwnd, IDM_PLUS60KD040, val == 0xd040);
            WinCheckMenuItem(hwnd, IDM_PLUS60KD100, val == 0xd100);
            return;
        case IDM_C64_256K_BASE:
            resources_get_int("C64_256K_Base", &val);
            WinCheckMenuItem(hwnd, IDM_C64_256K_BASEDE00, val == 0xde00);
            WinCheckMenuItem(hwnd, IDM_C64_256K_BASEDE80, val == 0xde80);
            WinCheckMenuItem(hwnd, IDM_C64_256K_BASEDF00, val == 0xdf00);
            WinCheckMenuItem(hwnd, IDM_C64_256K_BASEDF80, val == 0xdf80);
            return;
        case IDM_C64_MEMORY_HACK_DEVICE:
            resources_get_int("MemoryHack", &val);
            WinCheckMenuItem(hwnd, IDM_MEMORY_HACK_NONE, val == MEMORY_HACK_NONE);
            WinCheckMenuItem(hwnd, IDM_MEMORY_HACK_C64_256K, val == MEMORY_HACK_C64_256K);
            WinCheckMenuItem(hwnd, IDM_MEMORY_HACK_PLUS60K, val == MEMORY_HACK_PLUS60K);
            WinCheckMenuItem(hwnd, IDM_MEMORY_HACK_PLUS256K, val == MEMORY_HACK_PLUS256K);
            return;
#endif
#endif
#endif

#ifdef __X128__
        case IDM_C128TYPE:
            {
                int i;

                resources_get_int("MachineType", &val);
                for (i = 0; i < 7; i++) {
                    WinCheckMenuItem(hwnd, IDM_C128INT | i, val == i);
                }
            }
            return;
#endif

        case IDM_COLLISION:
            WinCheckRes(hwnd, IDM_SBCOLL, "VICIICheckSbColl");
            WinCheckRes(hwnd, IDM_SSCOLL, "VICIICheckSsColl");
            return;
        case IDM_REFRATE:
            resources_get_int("RefreshRate", &val);
            {
                int i;

                for (i = 0x0; i < 0xb; i++) {
                    WinCheckMenuItem(hwnd, IDM_REFRATEAUTO|i, i == val);
                }
            }
            return;
        case IDM_SOUNDDEV:
            {
                const char *dev;

                resources_get_string("SoundDeviceName", &dev);
                if (!dev || !dev[0]) {
                    dev = "dart";
                }
                WinCheckMenuItem(hwnd, IDM_DEVDART, !strcasecmp(dev, "dart"));
                WinCheckMenuItem(hwnd, IDM_DEVSID, !strcasecmp(dev, "dump"));
                WinCheckMenuItem(hwnd, IDM_DEVDUMMY, !strcasecmp(dev, "dummy"));
                WinCheckMenuItem(hwnd, IDM_DEVWAV, !strcasecmp(dev, "wav"));
                WinCheckMenuItem(hwnd, IDM_DEVRAW, !strcasecmp(dev, "fs"));
            }
            return;
        case IDM_SOUND:
            WinCheckRes(hwnd, IDM_SOUNDON, "Sound");
            resources_get_int("SoundOutput", &val);
            WinCheckMenuItem(hwnd, IDM_SOUND_OUTPUT_SYSTEM, val == SOUND_OUTPUT_SYSTEM);
            WinCheckMenuItem(hwnd, IDM_SOUND_OUTPUT_MONO, val == SOUND_OUTPUT_MONO);
            WinCheckMenuItem(hwnd, IDM_SOUND_OUTPUT_STEREO, val == SOUND_OUTPUT_STEREO);
            WinCheckRes(hwnd, IDM_DRIVE_SOUND, "DriveSoundEmulation");

#ifdef HAVE_RESID
            resources_get_int("SidEngine", &val);
            WinCheckMenuItem (hwnd, IDM_RESID, val);
            WinEnableMenuItem(hwnd, IDM_RESIDMETHOD, val);
            WinEnableMenuItem(hwnd, IDM_SOUNDSYNC, !val);
#endif // HAVE_RESID

#if defined(__X64__) || defined(__X128__) || defined(__XCBM__) || defined(__X64DTV__) || defined(__XSCPU64__)
            WinCheckRes(hwnd, IDM_SIDFILTER, "SidFilters");

#ifndef __X64DTV__
            resources_get_int("SidStereo", &val);
            WinCheckMenuItem(hwnd, IDM_NO_EXTRA_SID, val == 0);
            WinCheckMenuItem(hwnd, IDM_STEREO, val == 1);
            WinCheckMenuItem(hwnd, IDM_TRIPLE, val == 2);
#endif
#endif // __X64__ || __X128__ || __XCBM__

            return;

#ifdef HAVE_RESID
        case IDM_RESIDMETHOD:
            resources_get_int("SidResidSampling", &val);
            WinCheckMenuItem (hwnd, IDM_RESIDFAST, val == 0);
            WinCheckMenuItem (hwnd, IDM_RESIDINTERPOL, val == 1);
            WinCheckMenuItem (hwnd, IDM_RESIDRESAMPLE, val == 2);
            WinCheckMenuItem (hwnd, IDM_RESIDFASTRES, val == 3);
            WinEnableMenuItem(hwnd, IDM_RESIDBAND, val == 2);
            return;
        case IDM_RESIDBAND:
            resources_get_int("SidResidPassband", &val);
            {
                int i;

                for (i = 0; i < 10; i++) {
                    WinCheckMenuItem(hwnd, IDM_RESIDPASS0|i, i * 10 == val);
                }
            }
            return;
#endif // HAVE_RESID

#if defined(__X64__) || defined(__X128__) || defined(__XCBM__) || defined(__X64DTV__) || defined(__XSCPU64__)
        case IDM_SIDCHIP:
            resources_get_int("SidModel", &val);
            WinCheckMenuItem(hwnd, IDM_SC6581, val == 0);
            WinCheckMenuItem(hwnd, IDM_SC8580, val == 1);

#ifndef __X64DTV__
            WinCheckMenuItem(hwnd, IDM_SC8580DB, val == 2);
#endif

#if defined(HAVE_RESID) && defined(__X64DTV__)
            WinCheckMenuItem(hwnd, IDM_SCDTV, val == 4);
#endif

            return;
#endif // __X64__ || __X128__ || __XCBM__

        case IDM_SOUNDSYNC:
            resources_get_int("SoundSpeedAdjustment", &val);
            WinCheckMenuItem(hwnd, IDM_SYNCFLEX, val == SOUND_ADJUST_FLEXIBLE);
            WinCheckMenuItem(hwnd, IDM_SYNCADJUST, val == SOUND_ADJUST_ADJUSTING);
            WinCheckMenuItem(hwnd, IDM_SYNCEXACT,  val == SOUND_ADJUST_EXACT);
            return;
        case IDM_SOUNDFRAG:
            resources_get_int("SoundFragmentSize", &val);
            WinCheckMenuItem(hwnd, IDM_FRAGVERYSMALL, val == SOUND_FRAGMENT_VERY_SMALL);
            WinCheckMenuItem(hwnd, IDM_FRAGSMALL, val == SOUND_FRAGMENT_SMALL);
            WinCheckMenuItem(hwnd, IDM_FRAGMEDIUM, val == SOUND_FRAGMENT_MEDIUM);
            WinCheckMenuItem(hwnd, IDM_FRAGLARGE,  val == SOUND_FRAGMENT_LARGE);
            WinCheckMenuItem(hwnd, IDM_FRAGVERYLARGE,  val == SOUND_FRAGMENT_VERY_LARGE);
            return;
        case IDM_SAMPLINGRATE:
            resources_get_int("SoundSampleRate", &val);
            WinCheckMenuItem(hwnd, IDM_SR8000, val == 8000);
            WinCheckMenuItem(hwnd, IDM_SR11025, val == 11025);
            WinCheckMenuItem(hwnd, IDM_SR22050, val == 22050);
            WinCheckMenuItem(hwnd, IDM_SR44100, val == 44100);
            return;
        case IDM_BUFFER:
            resources_get_int("SoundBufferSize", &val);
            WinCheckMenuItem(hwnd, IDM_BUF010, val == 100);
            WinCheckMenuItem(hwnd, IDM_BUF025, val == 250);
            WinCheckMenuItem(hwnd, IDM_BUF040, val == 400);
            WinCheckMenuItem(hwnd, IDM_BUF055, val == 550);
            WinCheckMenuItem(hwnd, IDM_BUF070, val == 700);
            WinCheckMenuItem(hwnd, IDM_BUF085, val == 850);
            WinCheckMenuItem(hwnd, IDM_BUF100, val == 1000);
            return;
        case IDM_VOLUME:
            val = get_volume();
            WinCheckMenuItem(hwnd, IDM_VOL100, val == 100);
            WinCheckMenuItem(hwnd, IDM_VOL90, val == 90);
            WinCheckMenuItem(hwnd, IDM_VOL80, val == 80);
            WinCheckMenuItem(hwnd, IDM_VOL70, val == 70);
            WinCheckMenuItem(hwnd, IDM_VOL60, val == 60);
            WinCheckMenuItem(hwnd, IDM_VOL50, val == 50);
            WinCheckMenuItem(hwnd, IDM_VOL40, val == 40);
            WinCheckMenuItem(hwnd, IDM_VOL30, val == 30);
            WinCheckMenuItem(hwnd, IDM_VOL20, val == 20);
            WinCheckMenuItem(hwnd, IDM_VOL10, val == 10);
            return;

        case IDM_VIDEO_SETTINGS:
            resources_get_int(EXTERNAL_PALETTE, &val);
            WinCheckMenuItem(hwnd, IDM_EXTERNALPAL, val);
            WinEnableMenuItem(hwnd, IDM_EXTERNALPAL_FILE, val);
            WinCheckRes(hwnd, IDM_AUDIO_LEAK, AUDIO_LEAK);
            WinCheckRes(hwnd, IDM_VCACHE, VIDEO_CACHE);
            resources_get_int(DOUBLE_SIZE, &val);
            WinEnableMenuItem(hwnd, IDM_DSCAN, val);
            WinCheckMenuItem(hwnd, IDM_DSIZE, val);
            WinCheckRes(hwnd, IDM_DSCAN, DOUBLE_SCAN);
#if defined(__XPET__) || defined(__XCBM2__)
            WinCheckRes(hwnd, IDM_VERTICAL_STRETCH, VERTICAL_STRETCH);
#else
            resources_get_int(BORDER_MODE, &val);
            WinCheckMenuItem(hwnd, IDM_BORDER_NORMAL, val == NORMAL_BORDERS);
            WinCheckMenuItem(hwnd, IDM_BORDER_FULL, val == FULL_BORDERS);
            WinCheckMenuItem(hwnd, IDM_BORDER_DEBUG, val == DEBUG_BORDERS);
            WinCheckMenuItem(hwnd, IDM_BORDER_NONE, val == NO_BORDERS);
#endif

#ifdef __X128__
        case IDM_VDC_VIDEO_SETTINGS:
            resources_get_int("VICIIExternalPalette", &val);
            WinCheckMenuItem(hwnd, IDM_VDCEXTERNALPAL, val);
            WinEnableMenuItem(hwnd, IDM_VDCEXTERNALPAL_FILE, val);
            WinCheckRes(hwnd, IDM_VDCAUDIO_LEAK, "VDCAudioLeak");
            WinCheckRes(hwnd, IDM_VDCVCACHE, "VDCVideoCache");
            resources_get_int("VDCDoubleSize", &val);
            WinEnableMenuItem(hwnd, IDM_VDCDSCAN, val);
            WinCheckMenuItem(hwnd, IDM_VDCDSIZE, val);
            WinCheckRes(hwnd, IDM_VDCDSCAN, "VICIIDoubleScan");
            WinCheckRes(hwnd, IDM_VERTICAL_STRETCH, VERTICAL_STRETCH);
#endif

#ifdef HAVE_VDC
        case IDM_VDCMEMORY:
            resources_get_int("VDC64KB", &val);
            WinCheckMenuItem(hwnd, IDM_VDC16K, val == 0);
            WinCheckMenuItem(hwnd, IDM_VDC64K, val == 1);
            return;

        case IDM_FUNCROM:
            resources_get_int("InternalFunctionROM", &val);
            WinCheckMenuItem(hwnd, IDM_INTFUNCROM_NONE, val == 0);
            WinCheckMenuItem(hwnd, IDM_INTFUNCROM_ROM, val == 1);
            WinCheckMenuItem(hwnd, IDM_INTFUNCROM_RAM, val == 2);
            WinCheckMenuItem(hwnd, IDM_INTFUNCROM_RTC, val == 3);
            resources_get_int("InternalFunctionROMRTCSave", &val);
            WinCheckMenuItem(hwnd, IDM_INTFUNCROM_RTC_SAVE, val);
            resources_get_int("ExternalFunctionROM", &val);
            WinCheckMenuItem(hwnd, IDM_EXTFUNCROM_NONE, val == 0);
            WinCheckMenuItem(hwnd, IDM_EXTFUNCROM_ROM, val == 1);
            WinCheckMenuItem(hwnd, IDM_EXTFUNCROM_RAM, val == 2);
            WinCheckMenuItem(hwnd, IDM_EXTFUNCROM_RTC, val == 3);
            resources_get_int("ExternalFunctionROMRTCSave", &val);
            WinCheckMenuItem(hwnd, IDM_EXTFUNCROM_RTC_SAVE, val);
            return;
#endif // __X128__

        case IDM_STRETCH:
            resources_get_int("WindowStretchFactor", &val);
            WinCheckMenuItem(hwnd, IDM_STRETCH1, val == 1);
            WinCheckMenuItem(hwnd, IDM_STRETCH2, val == 2);
            WinCheckMenuItem(hwnd, IDM_STRETCH3, val == 3);
            return;

#ifdef __XCBM__
        case IDM_MODELLINE:
            resources_get_int("ModelLine", &val);
            WinCheckMenuItem(hwnd, IDM_MODEL750, val == 0);
            WinCheckMenuItem(hwnd, IDM_MODEL660, val == 1);
            WinCheckMenuItem(hwnd, IDM_MODEL650, val == 2);
           return;
        case IDM_RAMSIZE:
            resources_get_int("RamSize", &val);
            WinCheckMenuItem(hwnd, IDM_RAM64, val == 0x040); /*   64 */
            WinCheckMenuItem(hwnd, IDM_RAM128, val == 0x080); /*  128 */
            WinCheckMenuItem(hwnd, IDM_RAM256, val == 0x100); /*  256 */
            WinCheckMenuItem(hwnd, IDM_RAM512, val == 0x200); /*  512 */
            WinCheckMenuItem(hwnd, IDM_RAM1024, val == 0x400); /* 1024 */
            return;
        case IDM_RAMMAPPING:
            WinCheckRes(hwnd, IDM_RAM08, "Ram08");
            WinCheckRes(hwnd, IDM_RAM10, "Ram1");
            WinCheckRes(hwnd, IDM_RAM20, "Ram2");
            WinCheckRes(hwnd, IDM_RAM40, "Ram4");
            WinCheckRes(hwnd, IDM_RAM60, "Ram6");
            WinCheckRes(hwnd, IDM_RAMC0, "RamC");
            return;
#endif // __XCBM__

#ifdef __XPET__
        case IDM_IOMEM:
            resources_get_int("IOSize", &val);
            WinCheckMenuItem(hwnd, IDM_IOMEM256, val == 0x100);
            WinCheckMenuItem(hwnd, IDM_IOMEM2K, val == 0x800);
            return;
        case IDM_RAMMAPPING:
            WinCheckRes(hwnd, IDM_MAP9, "Ram9");
            WinCheckRes(hwnd, IDM_MAPA, "RamA");
            return;
        case IDM_RAMSIZE:
            resources_get_int("RamSize", &val);
            WinCheckMenuItem(hwnd, IDM_PETRAM4, val == 0x04); /*   4 */
            WinCheckMenuItem(hwnd, IDM_PETRAM8, val == 0x08); /*   8 */
            WinCheckMenuItem(hwnd, IDM_PETRAM16, val == 0x10); /*  16 */
            WinCheckMenuItem(hwnd, IDM_PETRAM32, val == 0x20); /*  32 */
            WinCheckMenuItem(hwnd, IDM_PETRAM96, val == 0x60); /*  96 */
            WinCheckMenuItem(hwnd, IDM_PETRAM128, val == 0x80); /* 128 */
           return;
        case IDM_VIDEOSIZE:
            resources_get_int("VideoSize", &val);
            WinCheckMenuItem(hwnd, IDM_VSDETECT, val == 0);
            WinCheckMenuItem(hwnd, IDM_VS40, val == 40);
            WinCheckMenuItem(hwnd, IDM_VS80, val == 80);
            return;
#endif // __XPET__
    }
}

// --------------------------------------------------------------------------
