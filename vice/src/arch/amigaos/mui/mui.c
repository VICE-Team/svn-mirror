/*
 * mui.c
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

#include "mui.h"

ui_to_from_t *ui_find_resource(ui_to_from_t *data, char *resource)
{
  while (data->resource != NULL) {
    if (strcmp(data->resource, resource) == 0) {
      return data;
    }
    data++;
  }
  return NULL;
}

static void radio_get_to_ui(ui_to_from_t *data)
{
  int n, val = 0;
  resources_get_value(data->resource, (void *)&val);
  for (n=0; data->values[n] != -1; n++) {
    if (data->values[n] == val) {
      set(data->object, MUIA_Radio_Active, n);
      break;
    }
  }
}

static void radio_get_from_ui(ui_to_from_t *data)
{
  int n, val = 0;
  get(data->object, MUIA_Radio_Active, (APTR)&n);
  val = data->values[n];
  resources_set_value(data->resource, (resource_value_t *)val);
}

static void check_get_to_ui(ui_to_from_t *data)
{
  int val = 0;
  resources_get_value(data->resource, (void *)&val);
  set(data->object, MUIA_Selected, val);
}

static void check_get_from_ui(ui_to_from_t *data)
{
  int val = 0;
  get(data->object, MUIA_Selected, (APTR)&val);
  resources_set_value(data->resource, (resource_value_t *)val);
}

static void cycle_get_to_ui(ui_to_from_t *data)
{
  int n, val = 0;
  resources_get_value(data->resource, (void *)&val);
  for (n=0; data->values[n] != -1; n++) {
    if (data->values[n] == val) {
      set(data->object, MUIA_Cycle_Active, n);
      break;
    }
  }
}

static void cycle_get_from_ui(ui_to_from_t *data)
{
  int n, val = 0;
  get(data->object, MUIA_Cycle_Active, (APTR)&n);
  val = data->values[n];
  resources_set_value(data->resource, (resource_value_t *)val);
}

static void integer_get_to_ui(ui_to_from_t *data)
{
  char str[32];
  int val = 0;
  resources_get_value(data->resource, (void *)&val);
  sprintf(str, "%d", val);
  set(data->object, MUIA_String_Contents, str);
}

static void integer_get_from_ui(ui_to_from_t *data)
{
  int val = 0;
  char *str;
  get(data->object, MUIA_String_Contents, (APTR)&str);
  val = atoi(str);
  resources_set_value(data->resource, (resource_value_t *)val);
}

static void text_get_to_ui(ui_to_from_t *data)
{
  char *str;
  resources_get_value(data->resource, (void *)&str);
  set(data->object, MUIA_String_Contents, str);
}

static void text_get_from_ui(ui_to_from_t *data)
{
  char *str;
  get(data->object, MUIA_String_Contents, (APTR)&str);
  resources_set_value(data->resource, (resource_value_t *)str);
}

/* static */ void ui_get_from(ui_to_from_t *data)
{
  if (data == NULL) {
    return;
  }
  while (data->resource != NULL) {
    if (data->object != NULL) {
      switch (data->type) {
        case MUI_TYPE_RADIO:
          radio_get_from_ui(data);
          break;

        case MUI_TYPE_CHECK:
          check_get_from_ui(data);
          break;

        case MUI_TYPE_CYCLE:
          cycle_get_from_ui(data);
          break;

        case MUI_TYPE_INTEGER:
          integer_get_from_ui(data);
          break;

        case MUI_TYPE_TEXT:
          text_get_from_ui(data);
          break;

        default:
          break;
      }
    }
    data++;
  }
}

/* static */ void ui_get_to(ui_to_from_t *data)
{
  if (data == NULL) {
    return;
  }
  while (data->resource != NULL) {
    if (data->object != NULL) {
      switch (data->type) {
        case MUI_TYPE_RADIO:
          radio_get_to_ui(data);
          break;

        case MUI_TYPE_CHECK:
          check_get_to_ui(data);
          break;

        case MUI_TYPE_CYCLE:
          cycle_get_to_ui(data);
          break;

        case MUI_TYPE_INTEGER:
          integer_get_to_ui(data);
          break;

        case MUI_TYPE_TEXT:
          text_get_to_ui(data);
          break;

        default:
          break;
      }
    }
    data++;
  }
}

int mui_show_dialog(APTR gui, char *title, ui_to_from_t *data)
{
  APTR window = mui_make_ok_cancel_window(gui, title);

  if (window != NULL) {
    mui_add_window(window);
    ui_get_to(data);
    set(window, MUIA_Window_Open, TRUE);
    if (mui_run() == BTN_OK) {
      ui_get_from(data);
    }
    set(window, MUIA_Window_Open, FALSE);
    mui_rem_window(window);
    MUI_DisposeObject(window);
  }

  return 0;
}

#define List(ftxt) \
  ListviewObject, MUIA_Weight, 50, MUIA_Listview_Input, FALSE, MUIA_Listview_List, \
    FloattextObject, MUIA_Frame, MUIV_Frame_ReadList, MUIA_Background, MUII_ReadListBack, \
                     MUIA_Floattext_Text, ftxt, MUIA_Floattext_TabSize, 4, MUIA_Floattext_Justify, TRUE, \
    End, \
  End

void ui_show_text(const char *title, const char *description, const char *text)
{
  APTR gui =
    GroupObject,
      Child, LLabel(description),
      Child, List(text),
    End;
  mui_show_dialog(gui, (char *)title, NULL);
}

void ui_about(void)
{
  static const char *authors[] = {
    "VICE",
    "",
    "Versatile Commodore Emulator",
    "",
    "Version " VERSION,
    "",
    "The VICE Team",
    "Copyright @ 1998-2005 Andreas Boose",
    "Copyright @ 1998-2005 Tibor Biczo",
    "Copyright @ 1999-2005 Andreas Dehmel",
    "Copyright @ 1999-2005 Thomas Bretz",
    "Copyright @ 1999-2005 Andreas Matthies",
    "Copyright @ 1999-2005 Martin Pottendorfer",
    "Copyright @ 1998-2005 Dag Lem",
    "Copyright @ 2000-2005 Spiro Trikaliotis",
    "Copyright @ 2003-2005 David Hansel",
    "Copyright @ 2000-2004 Markus Brenner",
    "",
    "Official VICE homepage:",
    "http://www.viceteam.org/",
    NULL};
  APTR gui =
    GroupObject,
    End;
  int i = 0;

  while (authors[i] != NULL) {
    if (i <= 5) { /* centered */
      DoMethod(gui, OM_ADDMEMBER, CLabel(authors[i]));
    } else {
      DoMethod(gui, OM_ADDMEMBER, LLabel(authors[i]));
    }
    i++;
  }

  mui_show_dialog(gui, "About", NULL);
}

/* ---------- */

static APTR app = NULL;

#if 0
#define ENTER printf("enter: %s @ %d in %s\n", __FUNCTION__, __LINE__, __FILE__);
#define EXIT  printf("exit:  %s @ %d in %s\n", __FUNCTION__, __LINE__, __FILE__);
#else
#define ENTER
#define EXIT
#endif

int mui_init(void)
{
  ENTER
  app = ApplicationObject,
    MUIA_Application_Author, "The VICE Team",
    MUIA_Application_Base, "VICE",
    MUIA_Application_Title, "VICE",
    MUIA_Application_Version, "$VER: VICE v" VERSION,
    MUIA_Application_Copyright, "The VICE Team",
    MUIA_Application_Description, "VICE",
  End;

  EXIT
  return ((app == NULL) ? -1 : 0);
}

APTR mui_get_app(void)
{
  return app;
}

APTR mui_make_simple_window(APTR gui, char *title)
{
  APTR window;

  ENTER
  if (app == NULL) {
    return NULL;
  }

  window = WindowObject,
    MUIA_Window_Title, title,
    MUIA_Window_ID, MAKE_ID(title[0], title[1], title[2], title[3]),
    MUIA_Window_Screen, canvaslist->os->screen,
    WindowContents, VGroup,
      Child, VGroup,
        Child, gui,
      End,
    End,
  End;

  if (window != NULL) {
    DoMethod(window,
      MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
      app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
  }

  EXIT
  return window;
}

APTR mui_make_ok_cancel_window(APTR gui, char *title)
{
  APTR window, ok, cancel;

  ENTER
  if (app == NULL) {
    return NULL;
  }

  window = WindowObject,
    MUIA_Window_Title, title,
    MUIA_Window_ID, MAKE_ID(title[0], title[1], title[2], title[3]),
    MUIA_Window_Screen, canvaslist->os->screen,
    WindowContents, VGroup,
      Child, VGroup,
        Child, gui,
      End,

      Child, HGroup,
        Child, ok = TextObject,
          ButtonFrame,
          MUIA_Background, MUII_ButtonBack,
          MUIA_Text_Contents, "Ok",
          MUIA_Text_PreParse, "\033c",
          MUIA_InputMode, MUIV_InputMode_RelVerify,
        End,
        Child, cancel = TextObject,
          ButtonFrame,
          MUIA_Background, MUII_ButtonBack,
          MUIA_Text_Contents, "Cancel",
          MUIA_Text_PreParse, "\033c",
          MUIA_InputMode, MUIV_InputMode_RelVerify,
        End,
      End,
    End,
  End;

  if (window != NULL) {
    DoMethod(window,
      MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
      app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(cancel,
      MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_Application_ReturnID, BTN_OK);
  }

  EXIT
  return window;
}

void mui_add_window(APTR window)
{
  ENTER
  if ((app != NULL) && (window != NULL)) {
    DoMethod(app, OM_ADDMEMBER, window);
  }
  EXIT
}

void mui_rem_window(APTR window)
{
  ENTER
  if ((app != NULL) && (window != NULL)) {
    DoMethod(app, OM_REMMEMBER, window);
  }
  EXIT
}

int mui_run(void)
{
  unsigned long retval = -1;
  BOOL running = TRUE;
  ULONG signals;

  ENTER
  if (app) {
    while (running) {
      retval = DoMethod(app, MUIM_Application_Input, &signals);
      switch (retval) {
        case MUIV_Application_ReturnID_Quit:
          running = FALSE;
          break;

        case BTN_OK:
          running = FALSE;
          break;

        default:
          if ((retval >= (256+0)) && (retval <= (256+255))) {
            running = FALSE;
          }
          break;
      }

      if (running && signals) {
        Wait(signals);
      }
    }
  }

  EXIT
  return retval;
}

void mui_exit(void)
{
  ENTER
  if (app != NULL) {
    MUI_DisposeObject(app);
  }
  EXIT
}  
