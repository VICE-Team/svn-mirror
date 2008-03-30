/*
 * uimediafile.c - Implementation of the mediafile save dialog.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <commdlg.h>

#include "gfxoutputdrv/ffmpegdrv.h"
#include "gfxoutput.h"
#include "lib.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "screenshot.h"
#include "sound.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uilib.h"
#include "winmain.h"
#include "util.h"
#include "videoarch.h"
#include "vsync.h"


/* Mingw & pre VC 6 headers doesn't have this definition */
#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING    0x00800000
#endif

#define MAXSCRNDRVLEN 256
static char screendrivername[MAXSCRNDRVLEN];
static OPENFILENAME ofn;
static gfxoutputdrv_t *selected_driver;


static void enable_ffmpeg_settings(HWND hwnd, int enable)
{
    EnableWindow(GetDlgItem(hwnd,IDC_SCREENSHOT_FFMPEGFORMAT), enable);
    EnableWindow(GetDlgItem(hwnd,IDC_SCREENSHOT_FFMPEGAUDIOCODEC), enable);
    EnableWindow(GetDlgItem(hwnd,IDC_SCREENSHOT_FFMPEGVIDEOCODEC), enable);
    EnableWindow(GetDlgItem(hwnd,IDC_SCREENSHOT_FFMPEGAUDIOBITRATE), enable);
    EnableWindow(GetDlgItem(hwnd,IDC_SCREENSHOT_FFMPEGVIDEOBITRATE), enable);
}

static void update_ffmpeg_codecs(HWND hwnd)
{
    HWND audio_codec_combo, video_codec_combo;
    ffmpegdrv_format_t *current_format = NULL;
    TCHAR st_selection[MAXSCRNDRVLEN];
    int ac, vc, i;

    resources_get_value("FFMPEGAudioCodec", (void *)&ac);
    resources_get_value("FFMPEGVideoCodec", (void *)&vc);
    GetDlgItemText(hwnd,IDC_SCREENSHOT_FFMPEGFORMAT,
                   st_selection, MAXSCRNDRVLEN);
    for (i = 0; ffmpegdrv_formatlist[i].name != NULL; i++)
        if (strcmp(ffmpegdrv_formatlist[i].name, st_selection) == 0)
                current_format = &ffmpegdrv_formatlist[i];
    if (current_format != NULL) {
        audio_codec_combo = GetDlgItem(hwnd,IDC_SCREENSHOT_FFMPEGAUDIOCODEC);
        SendMessage(audio_codec_combo,CB_RESETCONTENT, 0, 0);
        if (current_format->audio_codecs != NULL) {
            for (i = 0; current_format->audio_codecs[i].name != NULL; i++) {
                SendMessage(audio_codec_combo,CB_ADDSTRING, 0,
                            (LPARAM)current_format->audio_codecs[i].name);
                if (current_format->audio_codecs[i].id == ac)
                    SendMessage(audio_codec_combo,CB_SETCURSEL, i, 0);
            }
            EnableWindow(audio_codec_combo, 1);
        } else {
            SendMessage(audio_codec_combo,CB_ADDSTRING, 0,
                        (LPARAM)"(default)");
            SendMessage(audio_codec_combo,CB_SETCURSEL,0 , 0);
            EnableWindow(audio_codec_combo, 0);
        }

        video_codec_combo = GetDlgItem(hwnd,IDC_SCREENSHOT_FFMPEGVIDEOCODEC);
        SendMessage(video_codec_combo,CB_RESETCONTENT, 0, 0);
        if (current_format->video_codecs != NULL) {
            for (i = 0; current_format->video_codecs[i].name != NULL; i++) {
                SendMessage(video_codec_combo,CB_ADDSTRING, 0,
                            (LPARAM)current_format->video_codecs[i].name);
                if (current_format->video_codecs[i].id == vc)
                    SendMessage(video_codec_combo,CB_SETCURSEL, i, 0);
            }
            EnableWindow(video_codec_combo, 1);
        } else {
            SendMessage(video_codec_combo,CB_ADDSTRING, 0,
                        (LPARAM)"(default)");
            SendMessage(video_codec_combo,CB_SETCURSEL,0 , 0);
            EnableWindow(video_codec_combo, 0);
        }
    }
}

static void init_mediafile_dialog(HWND hwnd)
{
    HWND combo;
    gfxoutputdrv_t *driver;
    char *ffmpeg_format;
    int i;
    int enable_ffmpeg = 0;
    int bitrate;
    TCHAR st[256];

    combo = GetDlgItem(hwnd,IDC_SCREENSHOT_DRIVER);
    driver = gfxoutput_drivers_iter_init();
    for (i = 0; i < gfxoutput_num_drivers(); i++) {
        SendMessage(combo,CB_ADDSTRING, 0,
                    (LPARAM)driver->displayname);
        if (driver == selected_driver) {
            SendMessage(combo,CB_SETCURSEL,(WPARAM)i, 0);
            if (strcmp(driver->name, "FFMPEG") == 0)
                enable_ffmpeg = 1;
        }
        driver = gfxoutput_drivers_iter_next();
    }

    resources_get_value("FFMPEGFormat", (void *)&ffmpeg_format);
    combo = GetDlgItem(hwnd,IDC_SCREENSHOT_FFMPEGFORMAT);
    for (i = 0; ffmpegdrv_formatlist[i].name != NULL; i++) {
        SendMessage(combo,CB_ADDSTRING, 0,
                    (LPARAM)ffmpegdrv_formatlist[i].name);
        if (strcmp(ffmpeg_format, ffmpegdrv_formatlist[i].name) == 0)
            SendMessage(combo,CB_SETCURSEL,(WPARAM)i, 0);
    }
    update_ffmpeg_codecs(hwnd);

    resources_get_value("FFMPEGAudioBitrate", (void *)&bitrate);
    _stprintf(st, TEXT("%d"), bitrate);
    SetDlgItemText(hwnd, IDC_SCREENSHOT_FFMPEGAUDIOBITRATE, st);

    resources_get_value("FFMPEGVideoBitrate", (void *)&bitrate);
    _stprintf(st, TEXT("%d"), bitrate);
    SetDlgItemText(hwnd, IDC_SCREENSHOT_FFMPEGVIDEOBITRATE, st);

    enable_ffmpeg_settings(hwnd, enable_ffmpeg);
}


static UINT APIENTRY hook_save_mediafile(HWND hwnd, UINT uimsg, WPARAM wparam,
                                        LPARAM lparam)
{
    TCHAR st_selection[MAXSCRNDRVLEN];
    char *ffmpeg_format;
    int i, j;

    switch (uimsg) {
      case WM_INITDIALOG:
        init_mediafile_dialog(hwnd);
        break;
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDC_SCREENSHOT_DRIVER:
            GetDlgItemText(hwnd, IDC_SCREENSHOT_DRIVER,
                           st_selection, MAXSCRNDRVLEN);
            system_wcstombs(screendrivername, st_selection, MAXSCRNDRVLEN);
            enable_ffmpeg_settings(hwnd, strcmp(screendrivername, 
                                                "FFMPEG") == 0 ? 1 : 0);
            break;
          case IDC_SCREENSHOT_FFMPEGFORMAT:
            GetDlgItemText(hwnd,IDC_SCREENSHOT_FFMPEGFORMAT,
                       st_selection, MAXSCRNDRVLEN);
            resources_get_value("FFMPEGFormat", (void *)&ffmpeg_format);
            if (strcmp(st_selection, ffmpeg_format) != 0) {
                resources_set_value("FFMPEGFormat", st_selection);
                update_ffmpeg_codecs(hwnd);
            }
            break;
          case IDC_SCREENSHOT_FFMPEGAUDIOCODEC:
            i = SendDlgItemMessage(hwnd, IDC_SCREENSHOT_FFMPEGFORMAT,
                        CB_GETCURSEL, 0, 0);
            j = SendDlgItemMessage(hwnd, IDC_SCREENSHOT_FFMPEGAUDIOCODEC,
                        CB_GETCURSEL, 0, 0);
            resources_set_value("FFMPEGAudioCodec", (resource_value_t)
                                ffmpegdrv_formatlist[i].audio_codecs[j].id);
            break;            
          case IDC_SCREENSHOT_FFMPEGVIDEOCODEC:
            i = SendDlgItemMessage(hwnd, IDC_SCREENSHOT_FFMPEGFORMAT,
                        CB_GETCURSEL, 0, 0);
            j = SendDlgItemMessage(hwnd, IDC_SCREENSHOT_FFMPEGVIDEOCODEC,
                        CB_GETCURSEL, 0, 0);
            resources_set_value("FFMPEGVideoCodec", (resource_value_t)
                                ffmpegdrv_formatlist[i].video_codecs[j].id);
            break;            
          case IDC_SCREENSHOT_FFMPEGAUDIOBITRATE:
            GetDlgItemText(hwnd, IDC_SCREENSHOT_FFMPEGAUDIOBITRATE,
                            st_selection, MAXSCRNDRVLEN);
            _stscanf(st_selection, TEXT("%d"), &i);
            resources_set_value("FFMPEGAudioBitrate", 
                                    (resource_value_t)i);
            break;
          case IDC_SCREENSHOT_FFMPEGVIDEOBITRATE:
            GetDlgItemText(hwnd, IDC_SCREENSHOT_FFMPEGVIDEOBITRATE,
                            st_selection, MAXSCRNDRVLEN);
            _stscanf(st_selection, TEXT("%d"), &i);
            resources_set_value("FFMPEGVideoBitrate", (resource_value_t)i);
            break;
        }
    }
    return 0;
}


static char *ui_save_mediafile(const TCHAR *title, const TCHAR *filter, 
                       HWND hwnd, int dialog_template)
{
    TCHAR name[1024] = TEXT("");
    char *ret = NULL;

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = winmain_instance;
    ofn.lpstrFilter = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = name;
    ofn.nMaxFile = sizeof(name);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = title;
    ofn.Flags = (OFN_EXPLORER
                 | OFN_HIDEREADONLY
                 | OFN_NOTESTFILECREATE
                 | OFN_FILEMUSTEXIST
                 | OFN_SHAREAWARE);
    if (dialog_template) {
        ofn.lpfnHook = hook_save_mediafile;
        ofn.lpTemplateName = MAKEINTRESOURCE(dialog_template);
        ofn.Flags = (ofn.Flags
                 | OFN_ENABLEHOOK
                 | OFN_ENABLETEMPLATE);
    }
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    vsync_suspend_speed_eval();

    if (GetSaveFileName(&ofn))
        ret = system_wcstombs_alloc(name);

    return ret;
}



void ui_mediafile_save_dialog(HWND hwnd)
{
    int filter_len,mask_len;
    char *s;
    char *filter;
    char mask[]="*.bmp;*.gif;*.iff;*.jpg;*.pcx;*.png;*.ppm;*.wav;*.mp3;*.avi;*.mpg";

    if (screenshot_is_recording()) {
        /* the recording is active; stop it  */
        screenshot_stop_recording();
        ui_display_statustext("", 0);
        return;
    }
    s=translate_text(IDS_MEDIA_FILES_FILTER);
    filter_len=strlen(s);
    mask_len=strlen(mask);
    filter = util_concat(s, "0", mask, "0", NULL);
    filter[filter_len]='\0';
    filter[filter_len+mask_len+1]='\0';
    s = ui_save_mediafile(translate_text(IDS_SAVE_MEDIA_IMAGE),
        filter,
        hwnd,
        translate_res(IDD_MEDIAFILE_DIALOG));

    lib_free(filter);

    if (s != NULL) {
        selected_driver = gfxoutput_get_driver(screendrivername);
        if (!selected_driver) {
            ui_error(translate_text(IDS_NO_DRIVER_SELECT_SUPPORT));
            return;
        }
        util_add_extension(&s, selected_driver->default_extension);

        if (screenshot_save(selected_driver->name, s,
            video_canvas_for_hwnd(hwnd)) < 0)
            ui_error(translate_text(IDS_CANT_WRITE_SCREENSHOT_S), s);
        lib_free(s);
    }
}

