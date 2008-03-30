/*
 * uimsgwin.c - all windows for displaying / editing text.
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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

#include "wimp.h"
#include "textwin.h"

#include "archdep.h"
#include "lib.h"
#include "monitor.h"
#include "videoarch.h"
#include "uimsgwin.h"
#include "ui.h"




#define MSGWIN_FLAG_OPEN	1
#define MSGWIN_FLAG_BUSY	2
#define MSGWIN_FLAG_PENDCLOSE	4


extern void console_raise_escape(void);


typedef struct message_window_s {
  text_window_t *tw;
  RO_Window *win;
  int StringWidth;
  int Flags;
} message_window_t;


static void ui_monitor_enter(void *context, int line, const char *str);

static message_window_t MsgWindows[msg_win_NUMBER];

static const char *LastMonitorCommand = NULL;


static text_window_t MsgWinDescs[msg_win_NUMBER] = {

  /* monitor window */
  {
    "corpus.medium", TWIN_FLAG_LINEEDIT, 10, 10, 16, 0xeeeeee00, 0x00000000, 1500, 500, 256, 1536, 256, 4096, {&(MsgWinDescs[msg_win_monitor]), ui_monitor_enter}
  },

  /* license window */
  {
    "corpus.medium", TWIN_FLAG_READONLY, 10, 10, 16, 0xeeeeee00, 0x00000000, 1500, 500, 256, 1536, 256, 0
  },

  /* warranty window */
  {
    "corpus.medium", TWIN_FLAG_READONLY, 10, 10, 16, 0xeeeeee00, 0x00000000, 1500, 500, 256, 1536, 256, 0
  },

  /* contributor window */
  {
    "corpus.medium", TWIN_FLAG_READONLY, 10, 10, 16, 0xeeeeee00, 0x00000000, 1500, 500, 256, 1536, 256, 0
  },

  /* log window */
  {
    "corpus.medium", TWIN_FLAG_READONLY, 10, 10, 16, 0xeeeeee00, 0x00000000, 1500, 500, 256, 1536, 256, 1024
  }
};





void msgwin_monitor_close(void)
{
  textwin_free(MsgWindows[msg_win_monitor].tw);
  MsgWindows[msg_win_monitor].tw = NULL;
  MsgWindows[msg_win_monitor].Flags = 0;
  if (LastCaret.WHandle != -1)
  {
    Wimp_SetCaretPosition(LastCaret.WHandle, LastCaret.IHandle, LastCaret.offx, LastCaret.offy, LastCaret.height, LastCaret.index);
    LastCaret.WHandle = -1;
  }
}


static void mon_close_request(void)
{
  console_raise_escape();
  MsgWindows[msg_win_monitor].Flags |= MSGWIN_FLAG_PENDCLOSE;
}


/* monitor window close event handler */
static int mon_close_event(text_window_t *tw, int *block)
{
  /* closing the monitor window also destroys it, but only if the monitor isn't busy */
  if (block[WindowB_Handle] == MsgWindows[msg_win_monitor].win->Handle)
  {
    mon_close_request();

    return 1;
  }
  return 0;
}


static int msgwin_close_generic(message_window_e mwin, int *block)
{
  if (block[WindowB_Handle] == MsgWindows[mwin].win->Handle)
  {
    textwin_close(MsgWindows[mwin].tw);
    MsgWindows[mwin].Flags = 0;
    return 1;
  }
  return 0;
}

static int msgwin_close_license(text_window_t *tw, int *block)
{
  return msgwin_close_generic(msg_win_license, block);
}

static int msgwin_close_warranty(text_window_t *tw, int *block)
{
  return msgwin_close_generic(msg_win_warranty, block);
}

static int msgwin_close_contrib(text_window_t *tw, int *block)
{
  return msgwin_close_generic(msg_win_contrib, block);
}

static int msgwin_close_log(text_window_t *tw, int *block)
{
  /* just close, but don't delete */
  Wimp_CloseWindow(block);
  MsgWindows[msg_win_log].Flags &= ~MSGWIN_FLAG_OPEN;
  return 0;
}


static void ui_monitor_enter(void *context, int line, const char *str)
{
  /* just make a note of the command, this will be read by the console later */
  LastMonitorCommand = str;
}


static void ui_msgwin_set_dimensions(message_window_t *mw, int cols, int rows)
{
  if (mw->StringWidth < 0)
  {
    char strdummy[101];

    memset(strdummy, '0', 100);
    strdummy[100] = '\0';
    mw->StringWidth = textwin_font_string_width(mw->tw, strdummy);
  }
  if (cols > 0)
    mw->tw->MaxWidth = 3 * mw->tw->WindowBorder + (cols * mw->StringWidth) / 100;
  if (rows > 0)
    mw->tw->MinHeight = 2 * mw->tw->WindowBorder + rows * mw->tw->LineHeight;
}


static void ui_msgwin_poll_logfile(void)
{
  FILE *logfp = archdep_get_default_log_file();

  if (logfp != NULL)
  {
    size_t curPos = ftell(logfp);

    if (curPos != 0)
    {
      text_window_t *tw = MsgWindows[msg_win_log].tw;

      /* safeguard in case the message window hasn't been created yet for some reason */
      if (tw != NULL)
      {
        char *text = (char*)lib_malloc(curPos + 1);

        if (text != NULL)
        {
          fseek(logfp, 0, SEEK_SET);
          fread(text, 1, curPos, logfp);
          text[curPos] = '\0';
          textwin_add_text(tw, text);
          if (FullScreenMode == 0)
          {
            int block[WindowB_WFlags+1];
            block[WindowB_Handle] = MsgWindows[msg_win_log].win->Handle;
            Wimp_GetWindowState(block);
            if ((block[WindowB_WFlags] & (1<<16)) != 0)
              textwin_caret_to_end(tw);
          }
          lib_free(text);
        }
      }
      fseek(logfp, 0, SEEK_SET);
    }
  }
}


static int ui_message_window_change_flag(message_window_e mwin, int flag, int set)
{
  switch (mwin)
  {
    case msg_win_monitor:
    case msg_win_license:
    case msg_win_warranty:
    case msg_win_contrib:
    case msg_win_log:
      if (set == 0)
        MsgWindows[mwin].Flags &= ~flag;
      else
        MsgWindows[mwin].Flags |= flag;
      return 0;
    default:
      return -1;
  }
}


static int ui_message_window_is_flag(message_window_e mwin, int flag)
{
  switch (mwin)
  {
    case msg_win_monitor:
    case msg_win_license:
    case msg_win_warranty:
    case msg_win_contrib:
    case msg_win_log:
      return ((MsgWindows[mwin].Flags & flag) != 0);
    default:
      return 0;
  }
}


static textwin_event_handler_func MsgCloseEvents[msg_win_NUMBER] = {
  mon_close_event,
  msgwin_close_license,
  msgwin_close_warranty,
  msgwin_close_contrib,
  msgwin_close_log
};




void ui_message_get_dimensions(const char *msg, int *cols, int *rows)
{
  const char *base, *b;
  int c, r;

  c = 0; r = 0;
  base = msg;
  while (*base != '\0')
  {
    int width;

    b = base;
    while ((*b != '\0') && (*b != '\n')) b++;
    width = (b - base);
    if (c < width)
      c = width;
    base = (*b == '\0') ? b : b+1;
    r++;
  }
  if (cols != NULL)
    *cols = c;
  if (rows != NULL)
    *rows = r;
}



int ui_message_window_open(message_window_e mwin, const char *title, const char *message, int cols, int rows)
{
  text_window_t *tw;
  message_window_t *mw;

  switch (mwin)
  {
    case msg_win_monitor:
      Wimp_GetCaretPosition(&LastCaret);
      tw = MsgWinDescs;
      mw = MsgWindows;
      if (mw->win == NULL)
      {
        mw->win = wimp_window_clone(MessageWindow);
        wimp_window_create((int*)(mw->win), title);
      }
      if (mw->tw == NULL)
      {
        mw->tw = tw;
        ui_msgwin_set_dimensions(mw, cols, rows);
        textwin_init(tw, mw->win, message, NULL);
        (tw->Events)[WimpEvt_CloseWin] = MsgCloseEvents[msg_win_monitor];
      }
      textwin_open_centered(tw, tw->MaxWidth, 1024, ScreenMode.resx, ScreenMode.resy);
      textwin_set_caret(tw, 0, 0);
      MsgWindows[mwin].Flags |= MSGWIN_FLAG_OPEN;
      break;
    case msg_win_license:
    case msg_win_warranty:
    case msg_win_contrib:
    case msg_win_log:
      tw = MsgWinDescs + mwin;
      mw = MsgWindows + mwin;
      if (mw->win == NULL)
      {
        mw->win = wimp_window_clone(MessageWindow);
        wimp_window_create((int*)(mw->win), title);
      }
      if (mw->tw == NULL)
      {
        mw->tw = tw;
        ui_msgwin_set_dimensions(mw, cols, rows);
        textwin_init(tw, mw->win, message, NULL);
        (tw->Events)[WimpEvt_CloseWin] = MsgCloseEvents[mwin];
      }

      if (mwin == msg_win_log)
      {
        int block[WindowB_WFlags+1];

        /* if already opened then raise, otherwise open in bottom left corner */
        block[WindowB_Handle] = mw->win->Handle;
        Wimp_GetWindowState(block);
        block[WindowB_Stackpos] = -1;
        if ((block[WindowB_WFlags] & (1<<16)) == 0)
        {
          /* open in the bottom left corner, but leave room below for the icon bar */
          textwin_open(tw, tw->MaxWidth, tw->MinHeight, 0, 160 + tw->MinHeight);
        }
        else
        {
          /* just raise */
          Wimp_OpenWindow(block);
        }
        /* always set caret at end */
        textwin_caret_to_end(tw);
      }
      else
      {
        textwin_open_centered(tw, tw->MaxWidth, 1024, ScreenMode.resx, ScreenMode.resy);
      }

      MsgWindows[mwin].Flags |= MSGWIN_FLAG_OPEN;
      break;
    default:
      return -1;
  }
  return 0;
}



int ui_message_window_close(message_window_e mwin)
{
  switch (mwin)
  {
    case msg_win_monitor:
      /* This function should only be called from the console (i.e. monitor-internal).
         External functions should close by calling ui_message_window_close_request(). */
      msgwin_monitor_close();
      break;
    case msg_win_license:
    case msg_win_warranty:
    case msg_win_contrib:
      if (MsgWindows[mwin].tw != NULL)
      {
        textwin_close(MsgWindows[mwin].tw);
        MsgWindows[mwin].Flags &= ~MSGWIN_FLAG_OPEN;
      }
      break;
    case msg_win_log:
      if (MsgWindows[mwin].win != NULL)
      {
        Wimp_CloseWindow((int*)(MsgWindows[mwin].win));
        MsgWindows[mwin].Flags &= ~MSGWIN_FLAG_OPEN;
      }
      break;
    default:
      return -1;
  }
  return 0;
}


int ui_message_window_close_request(message_window_e mwin)
{
  if (mwin == msg_win_monitor)
  {
    mon_close_request();
    return 0;
  }
  return ui_message_window_close(mwin);
}


int ui_message_window_destroy(message_window_e mwin)
{
  switch (mwin)
  {
    case msg_win_monitor:
    case msg_win_license:
    case msg_win_warranty:
    case msg_win_contrib:
    case msg_win_log:
      if (MsgWindows[mwin].tw != NULL)
      {
        textwin_free(MsgWindows[mwin].tw);
        MsgWindows[mwin].tw = NULL;
      }
    default:
      return -1;
  }
  return 0;
}


int ui_message_window_is_open(message_window_e mwin)
{
  return ui_message_window_is_flag(mwin, MSGWIN_FLAG_OPEN);
}


int ui_message_window_busy(message_window_e mwin, int busy)
{
  if (ui_message_window_change_flag(mwin, MSGWIN_FLAG_BUSY, busy) == 0)
  {
    if (mwin == msg_win_monitor)
    {
      text_window_t *tw = MsgWinDescs + msg_win_monitor;
      if (busy == 0)
      {
        textwin_add_flush(tw);
        textwin_mark_prompt(tw);
        textwin_flush_input(tw);
      }
      else
      {
        textwin_buffer_input(tw);
      }
    }
  }
  return -1;
}

int ui_message_window_is_busy(message_window_e mwin)
{
  return ui_message_window_is_flag(mwin, MSGWIN_FLAG_BUSY);
}


const char *ui_message_window_get_last_command(message_window_e mwin)
{
  const char *cmd;

  if (mwin != msg_win_monitor)
    return NULL;

  /* if there's a pending close flag, return "x" */
  if ((MsgWindows[msg_win_monitor].Flags & MSGWIN_FLAG_PENDCLOSE) != 0)
    cmd = "x";
  else
    cmd = LastMonitorCommand;

  LastMonitorCommand = NULL;

  return cmd;
}



void ui_message_init(void)
{
  int i;

  for (i=0; i<msg_win_NUMBER; i++)
  {
    MsgWindows[i].tw = NULL;
    MsgWindows[i].win = NULL;
    MsgWindows[i].StringWidth = -1;
    MsgWindows[i].Flags = 0;
  }
}


void ui_message_exit(void)
{
  int i;

  for (i=0; i<msg_win_NUMBER; i++)
  {
    ui_message_window_destroy((message_window_e)i);
  }
}


message_window_e ui_message_window_for_handle(int handle)
{
  int i;

  for (i=0; i<msg_win_NUMBER; i++)
  {
    if (MsgWindows[i].win != NULL)
    {
      if (MsgWindows[i].win->Handle == handle)
        break;
    }
  }
  return (message_window_e)i;
}


text_window_t *ui_message_get_text_window(message_window_e mwin)
{
  switch (mwin)
  {
    case msg_win_monitor:
    case msg_win_license:
    case msg_win_warranty:
    case msg_win_contrib:
    case msg_win_log:
      return MsgWindows[mwin].tw;
    default:
      return NULL;
  }
}


int ui_message_need_null_event(void)
{
  int i;

  for (i=0; i<msg_win_NUMBER; i++)
  {
    if (MsgWindows[i].tw != NULL)
    {
      if (textwin_need_null(MsgWindows[i].tw))
        return 1;
    }
  }
  return 0;
}


int ui_message_process_event(int event, int *wimpblock)
{
  int i, status = 0;

  /* catch escape key in monitor window */
  if ((event == WimpEvt_KeyPress) && (MsgWindows[msg_win_monitor].win != NULL) &&
      (wimpblock[KeyPB_Window] == MsgWindows[msg_win_monitor].win->Handle) &&
      (wimpblock[KeyPB_Key] == 0x1b))
  {
      console_raise_escape();
      return 1;
  }

  for (i=0; i<msg_win_NUMBER; i++)
  {
    if (MsgWindows[i].tw != NULL)
    {
      status = textwin_process_event(MsgWindows[i].tw, event, wimpblock);

      if (status != 0)
        break;
    }
  }

  /* this may create new events; process afterwards */
  ui_msgwin_poll_logfile();

  return status;
}


void ui_message_init_messages(wimp_msg_desc *msg)
{
  textwin_init_messages(msg);
}


void ui_message_other_clicked(void)
{
  int i;

  for (i=0; i<msg_win_NUMBER; i++)
    textwin_other_clicked(MsgWindows[i].tw);
}
