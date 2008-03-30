/*
 * mousedrv.c
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

#include "private.h"

#include <stdio.h>
#include "mousedrv.h"
#include "mouse.h"
#include "pointer.h"

/* FIXME: I should probably semaphore protect these */
static int mx = 0, my = 0, mb = 0;

#include <devices/input.h>
#include <devices/inputevent.h>

#include <proto/exec.h>

#define MB_LEFT (1 << 0)
#define MB_RIGHT (1 << 1)

struct InputEvent *MyInputHandler(struct InputEvent *event)
{
  if (event->ie_Class == IECLASS_RAWMOUSE) {
    switch(event->ie_Code) {
      case (IECODE_LBUTTON):
        mb |= MB_LEFT;
        mx += event->ie_position.ie_xy.ie_x;
        my += event->ie_position.ie_xy.ie_y;
        return NULL; /* remove event */
        break;

      case (IECODE_LBUTTON | IECODE_UP_PREFIX):
        mb &= ~MB_LEFT;
        mx += event->ie_position.ie_xy.ie_x;
        my += event->ie_position.ie_xy.ie_y;
        return NULL; /* remove event */
        break;

      case (IECODE_RBUTTON):
        mb |= MB_RIGHT;
        mx += event->ie_position.ie_xy.ie_x;
        my += event->ie_position.ie_xy.ie_y;
        return NULL; /* remove event */
        break;

      case (IECODE_RBUTTON | IECODE_UP_PREFIX):
        mb &= ~MB_RIGHT;
        mx += event->ie_position.ie_xy.ie_x;
        my += event->ie_position.ie_xy.ie_y;
        return NULL; /* remove event */
        break;

      case (IECODE_NOBUTTON):
        mx += event->ie_position.ie_xy.ie_x;
        my += event->ie_position.ie_xy.ie_y;
        return NULL; /* remove event */
        break;

      default:
        break;
    }
  }

  return event; /* let someone else process the event */
}

static struct IOStdReq *inputReqBlk = NULL;
static struct MsgPort *inputPort = NULL;
static struct Interrupt *inputHandler = NULL;
static char HandlerName[] = __FILE__ " input handler";
static int input_error = -1;

void rem_inputhandler(void)
{
  if (!input_error) {
    inputReqBlk->io_Data=(APTR)inputHandler;
    inputReqBlk->io_Command=IND_REMHANDLER;
    DoIO((struct IORequest *)inputReqBlk);
    CloseDevice((struct IORequest *)inputReqBlk);
    input_error = -1;
  }

  if (inputReqBlk) {
    DeleteIORequest((struct IORequest *)inputReqBlk);
    inputReqBlk = NULL;
  }

  if (inputHandler) {
    FreeMem(inputHandler, sizeof(struct Interrupt));
    inputHandler = NULL;
  }

  if (inputPort) {
    DeleteMsgPort(inputPort);
    inputPort = NULL;
  }
}

int add_inputhandler(void)
{
  if ((inputPort = CreateMsgPort())) {
    if ((inputHandler = (struct Interrupt *)AllocMem(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR))) {
      if ((inputReqBlk = (struct IOStdReq *)CreateIORequest(inputPort, sizeof(struct IOStdReq)))) {
        if (!(input_error = OpenDevice("input.device", 0, (struct IORequest *)inputReqBlk, 0))) {
          inputHandler->is_Code         = (void *)MyInputHandler;
          inputHandler->is_Data         = NULL;
          inputHandler->is_Node.ln_Pri  = 100;
          inputHandler->is_Node.ln_Name = HandlerName;
          inputReqBlk->io_Data    = (APTR)inputHandler;
          inputReqBlk->io_Command = IND_ADDHANDLER;
          DoIO((struct IORequest *)inputReqBlk);
        }
      }
    }
  }

  if (!input_error) {
    return 0;
  } else {
    rem_inputhandler();
    return -1;
  }
}

static int mouse_acquired = 0;

int mousedrv_resources_init(void)
{
  return 0;
}

int mousedrv_cmdline_options_init(void)
{
  return 0;
}

void mousedrv_init(void)
{
}

void mousedrv_mouse_changed(void)
{
  if (_mouse_enabled) {
    if (add_inputhandler() == 0) {
      pointer_hide();
      mouse_acquired = 1;
    }
  } else {
    if (mouse_acquired) {
      pointer_to_default();
      mouse_acquired = 0;
      rem_inputhandler();
    }
  }
}

BYTE mousedrv_get_x(void)
{
  if (!_mouse_enabled)
    return 0xff;
  return (BYTE)(mx >> 1) & 0x7e;
}

BYTE mousedrv_get_y(void)
{
  if (!_mouse_enabled)
    return 0xff;
  return (BYTE)(~my >> 1) & 0x7e;
}

void mousedrv_sync(void)
{
  if (mouse_acquired) {
    mouse_button_left(mb & MB_LEFT);
    mouse_button_right(mb & MB_RIGHT);
  }
}

