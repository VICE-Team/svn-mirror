/*
 * filereq.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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
#include "mui.h"

#include "lib.h"
#include "imagecontents.h"
#include "diskimage.h"
#include "util.h"
#include "ui.h"
#include "vdrive-internal.h"
#include "cbmimage.h"
#include "filereq.h"

struct ObjApp
{
	APTR	App;
	APTR	FILEREQ;
	APTR	PA_FILEREQ;
	APTR	STR_PA_FILEREQ;
	APTR	LV_FILELIST;
	APTR	LV_VOLUMELIST;
	APTR	BT_ATTACH;
	APTR	BT_PARENT;
	APTR	BT_CANCEL;
	APTR	LV_CONTENTS;
	APTR	CH_READONLY;
	APTR	STR_IMAGENAME;
	APTR	STR_IMAGEID;
	APTR	CY_IMAGETYPE;
	APTR	BT_CREATEIMAGE;
	APTR	BT_CREATETAPIMAGE;
};

static const char *image_type_name[] = {
  "d64",
  "d71",
  "d80",
  "d81",
  "d82",
  "g64",
  "x64",
  NULL
};

static const int image_type[] = {
    DISK_IMAGE_TYPE_D64,
    DISK_IMAGE_TYPE_D71,
    DISK_IMAGE_TYPE_D80,
    DISK_IMAGE_TYPE_D81,
    DISK_IMAGE_TYPE_D82,
    DISK_IMAGE_TYPE_G64,
    DISK_IMAGE_TYPE_X64,
    -1
};

static struct ObjApp * CreateApp(const char *title, int template, char *resource_readonly)
{
	struct ObjApp * Object;

	APTR	GROUP_ROOT, GR_FILEREQ, LA_FILEREQ, GR_FILEBROWSE, GR_BUTTONS, GR_MISC_1;
	APTR	GR_CONTENTS, GR_MISC_2, GR_READONLY, LA_READONLY, GR_NEWIMAGE, GR_MISC_3;
	APTR GR_NEWTAPIMAGE;

	if (!(Object = AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
		return(NULL);

	LA_FILEREQ = Label("File");

	Object->STR_PA_FILEREQ = String("", 1024);

	Object->PA_FILEREQ = PopButton(MUII_PopFile);

	Object->PA_FILEREQ = PopaslObject,
		MUIA_HelpNode, "PA_FILEREQ",
		MUIA_Popasl_Type, 0,
		MUIA_Popstring_String, Object->STR_PA_FILEREQ,
		MUIA_Popstring_Button, Object->PA_FILEREQ,
	End;

	GR_FILEREQ = GroupObject,
		MUIA_HelpNode, "GR_FILEREQ",
		MUIA_Group_Horiz, TRUE,
		Child, LA_FILEREQ,
		Child, Object->PA_FILEREQ,
	End;

	Object->LV_FILELIST = DirlistObject,
		MUIA_Background, MUII_ListBack,
		MUIA_Frame, MUIV_Frame_InputList,
		MUIA_Dirlist_Directory, "RAM:",
	End;

	Object->LV_FILELIST = ListviewObject,
		MUIA_HelpNode, "LV_FILELIST",
		MUIA_Listview_List, Object->LV_FILELIST,
	End;

	Object->LV_VOLUMELIST = VolumelistObject,
		MUIA_Frame, MUIV_Frame_InputList,
	End;

	Object->LV_VOLUMELIST = ListviewObject,
		MUIA_HelpNode, "LV_VOLUMELIST",
		MUIA_Weight, 30,
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_None,
		MUIA_Listview_List, Object->LV_VOLUMELIST,
	End;

	GR_FILEBROWSE = GroupObject,
		MUIA_HelpNode, "GR_FILEBROWSE",
		MUIA_Group_Horiz, TRUE,
		Child, Object->LV_FILELIST,
		Child, Object->LV_VOLUMELIST,
	End;

	Object->BT_ATTACH = SimpleButton("Attach");

	Object->BT_PARENT = SimpleButton("Parent");

	Object->BT_CANCEL = SimpleButton("Cancel");

	GR_BUTTONS = GroupObject,
		MUIA_HelpNode, "GR_BUTTONS",
		MUIA_Group_Horiz, TRUE,
		Child, Object->BT_ATTACH,
		Child, Object->BT_PARENT,
		Child, Object->BT_CANCEL,
	End;

	Object->LV_CONTENTS = ListObject,
		MUIA_Frame, MUIV_Frame_InputList,
		MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
		MUIA_List_DestructHook, MUIV_List_DestructHook_String,
	End;

	Object->LV_CONTENTS = ListviewObject,
		MUIA_HelpNode, "LV_CONTENTS",
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_None,
		MUIA_Listview_List, Object->LV_CONTENTS,
	End;

	GR_CONTENTS = GroupObject,
		MUIA_HelpNode, "GR_CONTENTS",
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Image Contents",
		Child, Object->LV_CONTENTS,
	End;

	Object->CH_READONLY = CheckMark(FALSE);

	LA_READONLY = Label("Attach read only");

	GR_READONLY = GroupObject,
		MUIA_HelpNode, "GR_READONLY",
		MUIA_Group_Horiz, TRUE,
		Child, Object->CH_READONLY,
		Child, LA_READONLY,
	End;

	Object->STR_IMAGENAME = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "Name",
		MUIA_HelpNode, "STR_IMAGENAME",
		MUIA_String_MaxLen, 17,
	End;

	Object->STR_IMAGEID = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "ID",
		MUIA_HelpNode, "STR_IMAGEID",
		MUIA_String_MaxLen, 5,
	End;

	Object->CY_IMAGETYPE = CycleObject,
		MUIA_HelpNode, "CY_IMAGETYPE",
		MUIA_Cycle_Entries, image_type_name,
	End;

	GR_MISC_3 = GroupObject,
		MUIA_HelpNode, "GR_MISC_3",
		MUIA_Group_Horiz, TRUE,
		Child, Object->STR_IMAGEID,
		Child, Object->CY_IMAGETYPE,
	End;

	Object->BT_CREATEIMAGE = SimpleButton("Create Image");

	GR_NEWIMAGE = GroupObject,
		MUIA_HelpNode, "GR_NEWIMAGE",
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "New Image",
		Child, Object->STR_IMAGENAME,
		Child, GR_MISC_3,
		Child, Object->BT_CREATEIMAGE,
	End;

	Object->BT_CREATETAPIMAGE = SimpleButton("Create Image");

	GR_NEWTAPIMAGE = GroupObject,
		MUIA_HelpNode, "GR_NEWTAPIMAGE",
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "New TAP Image",
		Child, Object->BT_CREATETAPIMAGE,
	End;

	GR_MISC_2 = GroupObject,
		MUIA_HelpNode, "GR_MISC_2",
		Child, GR_READONLY,
		Child, GR_NEWIMAGE,
		Child, GR_NEWTAPIMAGE,
	End;

	GR_MISC_1 = GroupObject,
		MUIA_HelpNode, "GR_MISC_1",
		MUIA_Group_Horiz, TRUE,
		Child, GR_CONTENTS,
		Child, GR_MISC_2,
	End;

	GROUP_ROOT = GroupObject,
		Child, GR_FILEREQ,
		Child, GR_FILEBROWSE,
		Child, GR_BUTTONS,
		Child, GR_MISC_1,
	End;

	Object->FILEREQ = WindowObject,
		MUIA_Window_Title, title,
		MUIA_Window_ID, MAKE_ID('F', 'R', 'E', 'Q'),
        MUIA_Window_Screen, canvaslist->os->screen,
		WindowContents, GROUP_ROOT,
	End;

	Object->App = ApplicationObject,
		MUIA_Application_Author, "NONE",
		MUIA_Application_Base, "NONE",
		MUIA_Application_Title, "NONE",
		MUIA_Application_Version, "$VER: NONE XX.XX (XX.XX.XX)",
		MUIA_Application_Copyright, "NOBODY",
		MUIA_Application_Description, "NONE",
		SubWindow, Object->FILEREQ,
	End;


	if (!Object->App)
	{
		FreeVec(Object);
		return(NULL);
	}

	DoMethod(Object->FILEREQ,
		MUIM_Window_SetCycleChain, Object->PA_FILEREQ,
		Object->LV_FILELIST,
		Object->LV_VOLUMELIST,
		Object->BT_ATTACH,
		Object->BT_PARENT,
		Object->BT_CANCEL,
		Object->LV_CONTENTS,
		Object->CH_READONLY,
		Object->STR_IMAGENAME,
		Object->STR_IMAGEID,
		Object->CY_IMAGETYPE,
		Object->BT_CREATEIMAGE,
		Object->BT_CREATETAPIMAGE,
		0
		);

	if (resource_readonly == NULL) {
		set(GR_READONLY, MUIA_ShowMe, FALSE);
	}

	if (template == IDD_OPENTAPE_TEMPLATE) {
		set(GR_READONLY, MUIA_ShowMe, FALSE);
		set(GR_NEWIMAGE, MUIA_ShowMe, FALSE);
	} else {
		set(GR_NEWTAPIMAGE, MUIA_ShowMe, FALSE);
	}

	set(Object->FILEREQ,
		MUIA_Window_Open, TRUE
		);

	return(Object);
}

static void DisposeApp(struct ObjApp * Object)
{
	MUI_DisposeObject(Object->App);
	FreeVec(Object);
}

static struct ObjApp *app = NULL;

static ULONG NewVolume( struct Hook *hook, Object *obj, APTR arg )
{
  char *buf;

  DoMethod(app->LV_VOLUMELIST, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &buf);
  set(app->LV_FILELIST, MUIA_Dirlist_Directory, buf);
  set(app->STR_PA_FILEREQ, MUIA_String_Contents, buf);

  return 0;
}

static void create_content_list(char *text, Object *list)
{
    char *start;
    char buffer[256];
    int index;

    DoMethod(list, MUIM_List_Clear);

    if (text == NULL)
        return;

    start = text;
    index = 0;
    while (1) {
        if (*start == 0x0a) {
            buffer[index] = 0;
            index = 0;
            DoMethod(list, MUIM_List_InsertSingle, buffer, MUIV_List_Insert_Bottom);
        } else if (*start == 0x0d) {
        } else if (*start == 0) {
            break;
        } else {
            buffer[index++] = *start;
        }
        start++;
    }
}

static char *(*read_content_func)(const char *) = NULL;

enum {
  RET_NONE = 0,
  RET_AUTOSTART_ATTACH,
  RET_ATTACH
};

static ULONG NewFile( struct Hook *hook, Object *obj, APTR arg )
{
  char *buf;

  get(app->LV_FILELIST, MUIA_Dirlist_Path, (APTR)&buf);
  if (buf != NULL) {
    struct FileInfoBlock *fib;
    DoMethod(app->LV_FILELIST, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &fib);
    if (fib->fib_DirEntryType > 0) {
    } else {
      set(app->STR_PA_FILEREQ, MUIA_String_Contents, buf);

      if (read_content_func != NULL) {
        char *contents;
        contents = read_content_func(buf);
        create_content_list(contents, app->LV_CONTENTS);
        lib_free(contents);
      }
    }
  }

  return 0;
}

static ULONG NewDir( struct Hook *hook, Object *obj, APTR arg )
{
  char *buf;

  get(app->LV_FILELIST, MUIA_Dirlist_Path, (APTR)&buf);
  if (buf != NULL) {
    struct FileInfoBlock *fib;

    DoMethod(app->LV_FILELIST, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &fib);
    if (fib->fib_DirEntryType > 0) {
      char newdir[1024];

      strcpy(newdir, buf);
      AddPart(newdir, "", 1024);

      set(app->LV_FILELIST, MUIA_Dirlist_Directory, newdir);
      set(app->STR_PA_FILEREQ, MUIA_String_Contents, newdir);
    } else {
      DoMethod(app->App, MUIM_Application_ReturnID, RET_ATTACH);
    }
  }

  return 0;
}

static ULONG NewParentDir( struct Hook *hook, Object *obj, APTR arg )
{
  char *buf;

  get(app->LV_FILELIST, MUIA_Dirlist_Directory, (APTR)&buf);
  if (buf != NULL) {
    char newdir[1024], *ptr = PathPart(buf);

    memcpy(newdir, buf, (int)(ptr - buf));
    newdir[(int)(ptr - buf)] = '\0';

    set(app->LV_FILELIST, MUIA_Dirlist_Directory, newdir);
    set(app->STR_PA_FILEREQ, MUIA_String_Contents, newdir);
  }

  return 0;
}

static ULONG NewPopFile( struct Hook *hook, Object *obj, APTR arg )
{
  char *buf;

  get(app->STR_PA_FILEREQ, MUIA_String_Contents, (APTR)&buf);
  if (buf != NULL) {
    char newdir[1024], *ptr = PathPart(buf);

    memcpy(newdir, buf, (int)(ptr - buf));
    newdir[(int)(ptr - buf)] = '\0';

    set(app->LV_FILELIST, MUIA_Dirlist_Directory, newdir);

    if (read_content_func != NULL) {
      char *contents;
      contents = read_content_func(buf);
      create_content_list(contents, app->LV_CONTENTS);
      lib_free(contents);
    }
  }

  return 0;
}

static ULONG NewCreateImage( struct Hook *hook, Object *obj, APTR arg )
{
  char name[1024], *ext, *format_name;
  char *filename = NULL, *disk_name = NULL, *disk_id = NULL;
  int imagetype = 0;

  get(app->STR_PA_FILEREQ, MUIA_String_Contents, (APTR)&filename);
  get(app->STR_IMAGENAME, MUIA_String_Contents, (APTR)&disk_name);
  get(app->STR_IMAGEID, MUIA_String_Contents, (APTR)&disk_id);
  get(app->CY_IMAGETYPE, MUIA_Cycle_Active, (APTR)&imagetype);

  /*  Find last dot in name */
  ext = strrchr(filename, '.');

  strcpy(name, filename);
  if ((ext == NULL) || (strcasecmp(ext + 1, image_type_name[imagetype]) != 0)) {
    strcat(name, ".");
    strcat(name, image_type_name[imagetype]);
  }

  if (util_file_exists(name)) {
    LONG result = MUI_RequestA(app->App, app->FILEREQ, 0, "VICE question", "_Yes|*_No", "Overwrite existing image?", NULL);
    if (result != 1) {
      return 0;
    }
  }

  format_name = lib_msprintf("%s,%s", disk_name, disk_id);
  if (vdrive_internal_create_format_disk_image(name, format_name, image_type[imagetype]) < 0) {
    ui_error("Cannot create image");
  } else {
    char newdir[1024], *ptr = PathPart(name);

    memcpy(newdir, name, (int)(ptr - name));
    newdir[(int)(ptr - name)] = '\0';

    set(app->LV_FILELIST, MUIA_Dirlist_Directory, newdir);
    DoMethod(app->LV_FILELIST, MUIM_Dirlist_ReRead);
    set(app->STR_PA_FILEREQ, MUIA_String_Contents, name);

    if (read_content_func != NULL) {
      char *contents;
      contents = read_content_func(name);
      create_content_list(contents, app->LV_CONTENTS);
      lib_free(contents);
    }
  }
  lib_free(format_name);

  return 0;
}

static ULONG NewCreateTAPImage( struct Hook *hook, Object *obj, APTR arg )
{
  char name[1024], *ext, *filename = NULL;

  get(app->STR_PA_FILEREQ, MUIA_String_Contents, (APTR)&filename);

  /*  Find last dot in name */
  ext = strrchr(filename, '.');

  strcpy(name, filename);
  if ((ext == NULL) || (strcasecmp(ext + 1, "tap") != 0)) {
    strcat(name, ".");
    strcat(name, "tap");
  }

  if (util_file_exists(name)) {
    LONG result = MUI_RequestA(app->App, app->FILEREQ, 0, "VICE question", "_Yes|*_No", "Overwrite existing image?", NULL);
    if (result != 1) {
      return 0;
    }
  }

  if (cbmimage_create_image(name, DISK_IMAGE_TYPE_TAP)) {
    ui_error("Cannot create image");
  } else {
    char newdir[1024], *ptr = PathPart(name);

    memcpy(newdir, name, (int)(ptr - name));
    newdir[(int)(ptr - name)] = '\0';

    set(app->LV_FILELIST, MUIA_Dirlist_Directory, newdir);
    DoMethod(app->LV_FILELIST, MUIM_Dirlist_ReRead);
    set(app->STR_PA_FILEREQ, MUIA_String_Contents, name);
  }

  return 0;
}

static char *request_file(const char *title, char *initialdir, char *initialfile)
{
  struct FileRequester *FileReq = NULL;
  static char file_name[1024];

  FileReq = AllocAslRequestTags(ASL_FileRequest, TAG_END);

  file_name[0] = '\0';
  if (FileReq == NULL) {
    return NULL;
  } else {
    if (AslRequestTags(FileReq,
        ASLFR_TitleText, title,
        ASLFR_InitialDrawer, initialdir,
        ASLFR_InitialFile, initialfile,
        TAG_END)) {
      strcpy(file_name, FileReq->fr_Drawer);
      AddPart(file_name, FileReq->fr_File, 1024);
    }
  }

  FreeAslRequest(FileReq);

  if (file_name[0] == '\0') {
    return NULL;
  }

  return file_name;
}

char *ui_filereq(const char *title, int template,
                 char *initialdir, char *initialfile,
                 char *(*read_content)(const char *),
                 int *autostart, char *resource_readonly)
{
  static char filename[1024];

  static const struct Hook NewVolumeHook = { { NULL,NULL },(VOID *)NewVolume,NULL,NULL };
  static const struct Hook NewFileHook = { { NULL,NULL },(VOID *)NewFile,NULL,NULL };
  static const struct Hook NewDirHook = { { NULL,NULL },(VOID *)NewDir,NULL,NULL };
  static const struct Hook NewParentDirHook = { { NULL,NULL },(VOID *)NewParentDir,NULL,NULL };
  static const struct Hook NewPopFileHook = { { NULL,NULL },(VOID *)NewPopFile,NULL,NULL };
  static const struct Hook NewCreateImageHook = { { NULL,NULL },(VOID *)NewCreateImage,NULL,NULL };
  static const struct Hook NewCreateTAPImageHook = { { NULL,NULL },(VOID *)NewCreateTAPImage,NULL,NULL };

  BOOL running = TRUE;
  ULONG signals;
  char *ret = NULL;
  int readonly = 0;

  read_content_func = read_content;
  if (initialdir == NULL) {
    initialdir = "";
  }
  if (initialfile == NULL) {
    initialfile = "";
  }

  if (template == IDD_NONE) { /* simple requester */
    return request_file(title, initialdir, initialfile);
  }

    if (resource_readonly != NULL) {
        resources_get_value(resource_readonly, (void *)&readonly);
    }

      app = CreateApp(title, template, resource_readonly);
      if (app) {
	DoMethod(app->BT_ATTACH,
	MUIM_Notify, MUIA_Pressed, FALSE,
	app->App,
	2,
	MUIM_Application_ReturnID, RET_ATTACH
	);

	DoMethod(app->BT_CANCEL,
	MUIM_Notify, MUIA_Pressed, FALSE,
	app->App,
	2,
	MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
	);

	DoMethod(app->FILEREQ,
	MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	app->FILEREQ,
	3,
	MUIM_Set, MUIA_Window_Open, FALSE
	);

	DoMethod(app->FILEREQ,
	MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	app->App,
	2,
	MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
	);

	DoMethod(app->LV_VOLUMELIST,
	MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
	app->LV_VOLUMELIST,
	2,
	MUIM_CallHook, &NewVolumeHook
	);

	DoMethod(app->LV_FILELIST,
	MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
	app->LV_FILELIST,
	2,
	MUIM_CallHook, &NewFileHook
	);

	DoMethod(app->LV_FILELIST,
	MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
	app->LV_FILELIST,
	2,
	MUIM_CallHook, &NewDirHook
	);

	DoMethod(app->BT_PARENT,
	MUIM_Notify, MUIA_Pressed, FALSE,
	app->App,
	2,
	MUIM_CallHook, &NewParentDirHook
	);

	DoMethod(app->LV_CONTENTS,
	MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
	app->App,
	2,
	MUIM_Application_ReturnID, RET_AUTOSTART_ATTACH
	);

	DoMethod(app->STR_PA_FILEREQ,
	MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
	app->App,
	2,
	MUIM_CallHook, &NewPopFileHook
	);

	DoMethod(app->BT_CREATEIMAGE,
	MUIM_Notify, MUIA_Pressed, FALSE,
	app->App,
	2,
	MUIM_CallHook, &NewCreateImageHook
	);

	DoMethod(app->BT_CREATETAPIMAGE,
	MUIM_Notify, MUIA_Pressed, FALSE,
	app->App,
	2,
	MUIM_CallHook, &NewCreateTAPImageHook
	);

	/* Initialize */

	set(app->CH_READONLY, MUIA_Selected, readonly);
	set(app->LV_FILELIST, MUIA_Dirlist_Directory, initialdir);

	strcpy(filename, initialdir);
	AddPart(filename, initialfile, 1024);
	set(app->STR_PA_FILEREQ, MUIA_String_Contents, filename);

        while (running) {
          unsigned long retval = DoMethod(app->App,MUIM_Application_Input,&signals);
          switch (retval) {
            case MUIV_Application_ReturnID_Quit:
              running = FALSE;
              break;

            case RET_AUTOSTART_ATTACH:
              get(app->LV_CONTENTS, MUIA_List_Active, (APTR)autostart);
              /* fall through */

            case RET_ATTACH: {
              char *path;
              get(app->STR_PA_FILEREQ, MUIA_String_Contents, (APTR)&path);
              strcpy(filename, path);
              get(app->CH_READONLY, MUIA_Selected, (APTR)&readonly);
              ret = filename;
              running = FALSE;
              } break;

            default:
              break;
          }
          if (running && signals) {
            Wait(signals);
          }
        }
        DisposeApp(app);
      }  

    if (resource_readonly != NULL) {
        resources_set_value(resource_readonly, (resource_value_t)readonly);
    }

  return ret;
}
