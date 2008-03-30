/*
 * ui_xaw.c - Simple Xaw-based graphical user interface.  It uses widgets
 * from the Free Widget Foundation and Robert W. McMullen.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _UI_XAW_H
#define _UI_XAW_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "types.h"
#include "xdebug.h"

typedef Widget UiWindow;
typedef XtCallbackProc UiMenuCallback;
typedef void (*UiExposureHandler) (unsigned int width, unsigned int height);
typedef enum { Button_None, Button_Close, Button_Ok, Button_Cancel,
	       Button_Yes, Button_No, Button_Reset, Button_Mon,
	       Button_Debug, Button_Contents, Button_Autostart } UiButton;
typedef struct { unsigned short red, green, blue; BYTE dither; } UiColorDef;
typedef XtPointer UiCallbackDataPtr;

extern int ui_init_resources(void);
extern int ui_init_cmdline_options(void);

extern int UiInit(int *argc, char **argv);
extern int UiInitFinish(void);
extern UiWindow UiOpenCanvasWindow(const char *title, int width, int height,
				   int no_autorepeat,
				   UiExposureHandler exposure_proc,
				   int num_colors,
				   const UiColorDef color_defs[],
				   PIXEL pixel_return[]);
extern void UiResizeCanvasWindow(UiWindow w, int height, int width);
extern void UiMapCanvasWindow(UiWindow w);
extern void UiUnmapCanvasWindow(UiWindow w);
extern Window UiCanvasDrawable(UiWindow w);
extern void UiDisplaySpeed(float percent, float framerate);
extern void UiToggleDriveStatus(int state);
extern void UiDisplayDriveTrack(double track_number);
extern void UiDisplayDriveLed(int status);
extern void UiDisplayPaused(int flag);
extern void UiDispatchEvents(void);
extern void UiError(const char *format,...);
extern int UiJamDialog(const char *format,...);
extern void UiMessage(const char *format,...);
extern void UiShowText(const char *title, const char *text, int width,
		       int height);
extern char *UiFileSelect(const char *title,
			  char *(*read_contents_func)(const char *),
			  int allow_autostart,
			  UiButton *button_return);
extern UiButton UiInputString(const char *title, const char *prompt, char *buf,
			      unsigned int buflen);
extern UiButton UiAskConfirmation(const char *title, const char *text);
extern void UiAutoRepeatOn(void);
extern void UiAutoRepeatOff(void);
extern void UiUpdateMenus(void);
extern int UiExtendImageDialog(void);

#endif /* !defined (_UI_XAW_H) */
