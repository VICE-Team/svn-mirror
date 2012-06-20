/*
 * vsidui.h - Implementation of the VSID-specific part of the UI.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include <stdio.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>

#include "private.h"
#include "info.h"
#include "intl.h"
#include "ui.h"
#include "vsidui.h"

#define UI_VSID
#define UI_MENU_NAME vsid_ui_translation_menu
#define UI_TRANSLATED_MENU_NAME vsid_ui_menu

#include "vsiduires.h"

static struct Window *vsid_window = NULL;
static struct Menu *vsid_menu = NULL;

static int do_quit_vice = 0;

static struct NewMenu *machine_specific_menu = vsid_ui_menu;
static APTR VisualInfo;

static const ui_menu_toggle_t toggle_list[] = {
    { "Sound", IDM_TOGGLE_SOUND },
    { "WarpMode", IDM_TOGGLE_WARP_MODE },
    { "SaveResourcesOnExit", IDM_TOGGLE_SAVE_SETTINGS_ON_EXIT },
    { "ConfirmOnExit", IDM_TOGGLE_CONFIRM_ON_EXIT },
    { "PSIDKeepEnv", IDM_PSID_OVERRIDE },
    { NULL, 0 }
};

static const ui_res_possible_values_t RefreshRateValues[] = {
    { 0, IDM_REFRESH_RATE_AUTO },
    { 1, IDM_REFRESH_RATE_1 },
    { 2, IDM_REFRESH_RATE_2 },
    { 3, IDM_REFRESH_RATE_3 },
    { 4, IDM_REFRESH_RATE_4 },
    { 5, IDM_REFRESH_RATE_5 },
    { 6, IDM_REFRESH_RATE_6 },
    { 7, IDM_REFRESH_RATE_7 },
    { 8, IDM_REFRESH_RATE_8 },
    { 9, IDM_REFRESH_RATE_9 },
    { 10, IDM_REFRESH_RATE_10 },
    { -1, 0 }
};

static ui_res_possible_values_t SpeedValues[] = {
    { 0, IDM_MAXIMUM_SPEED_NO_LIMIT },
    { 10, IDM_MAXIMUM_SPEED_10 },
    { 20, IDM_MAXIMUM_SPEED_20 },
    { 50, IDM_MAXIMUM_SPEED_50 },
    { 100, IDM_MAXIMUM_SPEED_100 },
    { 200, IDM_MAXIMUM_SPEED_200 },
    { -1, 0 }
};

static const ui_res_value_list_t value_list[] = {
    { "RefreshRate", RefreshRateValues, 0 },
    { "Speed", SpeedValues, IDM_MAXIMUM_SPEED_CUSTOM },
    { NULL, NULL, 0 }
};

static char fname[1024] = "";

char *VSID_BrowseFile(char *select_text, char *pattern)
{
    struct FileRequester *request;

    request = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest, ASL_Hail, select_text, (struct TagItem *)TAG_DONE);

    if (AslRequestTags(request, ASLFR_Window, vsid_window,
                                ASLFR_InitialDrawer, "PROGDIR:",
                                ASLFR_InitialPattern, pattern,
                                ASLFR_PositiveText, select_text,
                                (struct TagItem *)TAG_DONE)) {
        fname[0]=0;
        strcat(fname,request->rf_Dir);
        if (fname[strlen(fname) - 1] != (UBYTE)58) {
            strcat(fname, "/");
        }
        strcat(fname, request->rf_File);
        if (fname[strlen(fname) - 1] != (UBYTE)58 && fname[strlen(fname) - 1] != '/') {
            if (request) {
                FreeAslRequest(request);
            }
            return fname;
        }
    }
    if (request) {
        FreeAslRequest(request);
    }

    return NULL;
}

static int vsid_menu_update(void)
{
    int i, j;
    int value;
    int result;

    for (i = 0; toggle_list[i].name != NULL; i++) {
        value = 0;
        resources_get_value(toggle_list[i].name, (void *)&value);
        toggle_menu_item(vsid_menu, toggle_list[i].idm, value);
    }

    for (i = 0; value_list[i].name != NULL; i++) {
        value = -1;
        result = resources_get_value(value_list[i].name, (void *)&value);
        if (result == 0) {
            unsigned int checked = 0;

            toggle_menu_item(vsid_menu, value_list[i].default_idm, 0);
            for (j = 0; value_list[i].vals[j].idm != 0; j++) {
                if (value == value_list[i].vals[j].value && !checked) {
                    toggle_menu_item(vsid_menu, value_list[i].vals[j].idm, 1);
                    checked = 1;
                } else {
                    toggle_menu_item(vsid_menu, value_list[i].vals[j].idm, 0);
                }
            }
            if (checked == 0 && value_list[i].default_idm > 0) {
                toggle_menu_item(vsid_menu, value_list[i].default_idm, 1);
            }
        }
    }

    ResetMenuStrip(vsid_window, vsid_menu);

    return 0;
}

static int vsid_menu_create(void)
{
    struct Screen* pubscreen = NULL;
    int i, j;

    pubscreen = LockPubScreen(NULL);

    if (pubscreen == NULL) {
        return -1;
    }

    if (!(VisualInfo = GetVisualInfo(pubscreen, TAG_DONE))) {
        return -1;
    }

    for (i = 0, j = 0; vsid_ui_translation_menu[i].nm_Type != NM_END; i++) {
        vsid_ui_menu[j].nm_Type = vsid_ui_translation_menu[i].nm_Type;
        vsid_ui_menu[j].nm_CommKey = vsid_ui_translation_menu[i].nm_CommKey;
        vsid_ui_menu[j].nm_Flags = vsid_ui_translation_menu[i].nm_Flags;
        vsid_ui_menu[j].nm_MutualExclude = vsid_ui_translation_menu[i].nm_MutualExclude;
        vsid_ui_menu[j].nm_UserData = vsid_ui_translation_menu[i].nm_UserData;
        switch (vsid_ui_translation_menu[i].nm_Label) {
            case 0:
                vsid_ui_menu[j++].nm_Label = (STRPTR)NM_BARLABEL;
                break;
            default:
                vsid_ui_menu[j++].nm_Label = translate_text(vsid_ui_translation_menu[i].nm_Label);
                break;
        }
    }
    vsid_ui_menu[i].nm_Type = NM_END;
    vsid_ui_menu[i].nm_CommKey = NULL;
    vsid_ui_menu[i].nm_Flags = 0;
    vsid_ui_menu[i].nm_MutualExclude = 0L;
    vsid_ui_menu[i].nm_UserData = NULL;
    vsid_ui_menu[i].nm_Label = NULL;

    if (!(vsid_menu = CreateMenus(vsid_ui_menu, GTMN_FrontPen, 0L, TAG_DONE))) {
        return -1;
    }

    LayoutMenus(vsid_menu, VisualInfo, GTMN_NewLookMenus, TRUE, TAG_DONE);

    SetMenuStrip(vsid_window, vsid_menu);

    vsid_menu_update();

    UnlockPubScreen(NULL, pubscreen);

    return 0;
}

static void vsid_menu_destroy(void)
{
    if (vsid_menu) {
        ClearMenuStrip(vsid_window);
        FreeMenus(vsid_menu);
        vsid_menu = NULL;
    }

    if (VisualInfo) {
        FreeVisualInfo(VisualInfo);
        VisualInfo = NULL;
    }
}

static void vsid_menu_rebuild(void)
{
    vsid_menu_destroy();
    vsid_menu_create();
}

static int vsid_menu_handle(int idm)
{
    char *fname = NULL;
    char *curlang;

    switch (idm) {
        case IDM_RESET_HARD:
            machine_trigger_reset(MACHINE_RESET_MODE_HARD);
            break;
        case IDM_RESET_SOFT:
            machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
            break;
        case IDM_EXIT:
            do_quit_vice = 1;
            break;
        case IDM_SETTINGS_SAVE_FILE:
            fname = VSID_BrowseFile(translate_text(IDS_CONFIG_FILENAME_SELECT), "#?");
            if (fname != NULL) {
                if (resources_save(fname) < 0) {
                    ui_error(translate_text(IDMES_CANNOT_SAVE_SETTINGS));
                } else {
                    ui_message(translate_text(IDMES_SETTINGS_SAVED_SUCCESS));
                }
            }
            break;
        case IDM_SETTINGS_LOAD_FILE:
            fname = VSID_BrowseFile(translate_text(IDS_CONFIG_FILENAME_SELECT), "#?");
            if (fname != NULL) {
                if (resources_load(fname) < 0) {
                    ui_error(translate_text(IDMES_CANNOT_LOAD_SETTINGS));
                } else {
                    ui_message(translate_text(IDMES_SETTINGS_LOAD_SUCCESS));
                }
            }
            break;
        case IDM_SETTINGS_SAVE:
            if (resources_save(NULL) < 0) {
                ui_error(translate_text(IDMES_CANNOT_SAVE_SETTINGS));
            } else {
                ui_message(translate_text(IDMES_SETTINGS_SAVED_SUCCESS));
            }
            break;
        case IDM_SETTINGS_LOAD:
            if (resources_load(NULL) < 0) {
                ui_error(translate_text(IDMES_CANNOT_LOAD_SETTINGS));
            } else {
                ui_message(translate_text(IDMES_SETTINGS_LOAD_SUCCESS));
            }
            break;
        case IDM_SETTINGS_DEFAULT:
            resources_set_defaults();
            ui_message(translate_text(IDMES_DFLT_SETTINGS_RESTORED));
            break;
        case IDM_ABOUT:
            ui_about();
            break;
        case IDM_CONTRIBUTORS:
            ui_show_text(translate_text(IDMES_VICE_CONTRIBUTORS), translate_text(IDMES_WHO_MADE_WHAT), info_contrib_text);
            break;
        case IDM_LICENSE:
            ui_show_text(translate_text(IDMS_LICENSE), "VICE license (GNU General Public License)", info_license_text);
            break;
        case IDM_WARRANTY:
            ui_show_text(translate_text(IDMS_NO_WARRANTY), translate_text(IDMES_VICE_DIST_NO_WARRANTY), info_warranty_text);
            break;
        case IDM_CMDLINE:
            {
                char *options;

                options = cmdline_options_string();
                ui_show_text(translate_text(IDMS_COMMAND_LINE_OPTIONS), translate_text(IDMES_WHICH_COMMANDS_AVAILABLE), options);
                lib_free(options);
            }
            break;
        case IDM_SOUND_SETTINGS:
            ui_sound_settings_dialog();
            break;
        case IDM_SOUND_RECORD_START:
            ui_sound_record_settings_vsid_dialog();
            break;
        case IDM_SOUND_RECORD_STOP:
            resources_set_string("SoundRecordDeviceName", "");
            ui_display_statustext(translate_text(IDS_SOUND_RECORDING_STOPPED), 1);
            break;
        case IDM_LANGUAGE_ENGLISH:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "en")) {
                resources_set_value("Language", (resource_value_t *)"en");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_DANISH:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "da")) {
                resources_set_value("Language", (resource_value_t *)"da");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_GERMAN:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "de")) {
                resources_set_value("Language", (resource_value_t *)"de");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_SPANISH:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "es")) {
                resources_set_value("Language", (resource_value_t *)"es");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_FRENCH:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "fr")) {
                resources_set_value("Language", (resource_value_t *)"fr");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_ITALIAN:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "it")) {
                resources_set_value("Language", (resource_value_t *)"it");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_KOREAN:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "ko")) {
                resources_set_value("Language", (resource_value_t *)"ko");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_DUTCH:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "nl")) {
                resources_set_value("Language", (resource_value_t *)"nl");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_POLISH:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "pl")) {
                resources_set_value("Language", (resource_value_t *)"pl");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_HUNGARIAN:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "hu")) {
                resources_set_value("Language", (resource_value_t *)"hu");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_RUSSIAN:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "ru")) {
                resources_set_value("Language", (resource_value_t *)"ru");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_SWEDISH:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "sv")) {
                resources_set_value("Language", (resource_value_t *)"sv");
                vsid_menu_rebuild();
            }
            break;
        case IDM_LANGUAGE_TURKISH:
            resources_get_value("Language", (void *)&curlang);
            if (strcasecmp(curlang, "tr")) {
                resources_set_value("Language", (resource_value_t *)"tr");
                vsid_menu_rebuild();
            }
            break;
        default:
            {
                int i, j, command_found = 0;

                for (i = 0; toggle_list[i].name != NULL && !command_found; i++) {
                    if (toggle_list[i].idm == idm) {
                        resources_toggle(toggle_list[i].name, NULL);
                        command_found = 1;
                    }
                }

                for (i = 0; value_list[i].name != NULL && !command_found; i++) {
                    for (j = 0; value_list[i].vals[j].idm != 0 && !command_found; j++) {
                        if (value_list[i].vals[j].idm == idm) {
                            resources_set_value(value_list[i].name, (resource_value_t) value_list[i].vals[j].value);
                            command_found = 1;
                        }
                    }
                }
            }
            break;
    }

    return 0;
}

static void vsid_event_handling(void)
{
    int done = 1;
    unsigned long imCode, imClass;
    struct IntuiMessage *imsg;

    /* Check for IDCMP messages */
    while ((imsg = (struct IntuiMessage *)GetMsg(vsid_window->UserPort))) {
        imClass = imsg->Class;
        imCode = imsg->Code;

        switch (imClass) {
            case IDCMP_MENUPICK:
                pointer_to_default();
                while (imCode != MENUNULL) {
                    struct MenuItem *n = ItemAddress(vsid_menu, imCode);
                    vsid_menu_handle((int)GTMENUITEM_USERDATA(n));
                    imCode = n->NextSelect;
                }
                vsid_menu_update();
                done = 1;
                break;
            default:
                break;
        }

        ReplyMsg((struct Message *)imsg);

        switch (imClass) {
            case IDCMP_CLOSEWINDOW:
                do_quit_vice = 1;
                break;
            default:
                break;
        }
    }

    if (do_quit_vice) {
        exit(0); // I think it's safe to quit here
    }
}

int vsid_ui_init(void)
{
    struct IntuiText vsid_text;

    vsid_window = (struct Window *)OpenWindowTags(NULL,
                                                  WA_Left, 20,
                                                  WA_Top, 20,
                                                  WA_Width, 300,
                                                  WA_Height, 100,
                                                  WA_Title, (ULONG)"VSID: The VICE SID player",
                                                  WA_DepthGadget, TRUE,
                                                  WA_CloseGadget, TRUE,
                                                  WA_SizeGadget, TRUE,
                                                  WA_DragBar, TRUE,
                                                  WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_MENUPICK | IDCMP_MENUVERIFY,
                                                  TAG_END);
    if (vsid_window == NULL) {
        return -1;
    }

    ui_event_handling = vsid_event_handling;

    if (vsid_menu_create() != 0) {
        return -1;
    }

    vsid_text.LeftEdge = 10;
    vsid_text.TopEdge = 20;
    vsid_text.IText = "Test";
    vsid_text.ITextFont = NULL;
    vsid_text.DrawMode = JAM1;
    vsid_text.FrontPen = 1;
    vsid_text.NextText = NULL;

    PrintIText(vsid_window->RPort, &vsid_text, 0, 0);

    return 0;
}

void vsid_ui_close(void)
{
    vsid_menu_destroy();

    if (vsid_window) {
        CloseWindow(vsid_window);
    }
}

void vsid_ui_display_name(const char *name)
{
}

void vsid_ui_display_author(const char *author)
{
}

void vsid_ui_display_copyright(const char *copyright)
{
}

void vsid_ui_display_sync(int sync)
{
}

void vsid_ui_display_sid_model(int model)
{
}

void vsid_ui_display_tune_nr(int nr)
{
}

void vsid_ui_display_nr_of_tunes(int count)
{
}

void vsid_ui_set_default_tune(int nr)
{
}

void vsid_ui_display_time(unsigned int sec)
{
}

void vsid_ui_display_irqtype(const char *irq)
{
}

void vsid_ui_setdrv(char* driver_info_text)
{
}
