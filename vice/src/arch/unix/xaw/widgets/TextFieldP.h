/*-----------------------------------------------------------------------------
 * TextField	A single line text entry widget
 *
 * Copyright (c) 1995 Robert W. McMullen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Author: Rob McMullen <rwmcm@orion.ae.utexas.edu>
 *         http://www.ae.utexas.edu/~rwmcm
 */

#ifndef _TextFieldP_H
#define _TextFieldP_H

#include <X11/Core.h>

#include "TextField.h"

#define TEXTFIELD_ALLOC_SIZE	256

typedef struct {
  int dummy;			/* keep compiler happy with dummy field */
} TextFieldClassPart;

typedef struct _TextFieldClassRec {
  CoreClassPart core_class;
  TextFieldClassPart TextField_class;
} TextFieldClassRec;

extern TextFieldClassRec textfieldClassRec;

typedef struct {
  /* Public stuff ... */
  long foreground_pixel;	/* data storage for resources ... */
  long cursor_pixel;
  XFontStruct *font;
  Dimension Margin;
  int TextMaxLen;
  Boolean Echo;
  Boolean Editable;
  Boolean DisplayCursor;
  Boolean AllowSelection;
  Boolean PendingDelete;
  char *DefaultString;
  XtCallbackList ActivateCallback;

  /* Private stuff ... */
  GC drawGC;			/* GC for drawing and copying */
  GC highlightGC;		/* GC for highlighting text */
  GC cursorGC;			/* GC for cursor (not clipped like drawGC) */
  GC dashGC;			/* GC for cursor when we don't have focus */
  GC eraseGC;			/* GC for erasing (not clipped) */

  int CursorPos;		/* text position of cursor */
  int OldCursorPos;		/* previous position */
  int OldCursorX;		/* previous pixel pos of cursor */
  int HighlightStart;		/* text pos of leftmost highlight pos */
  int HighlightEnd;		/* text pos of rightmost highlight pos */
  int HighlightPivotStart;	/* left pivot pos for ExtendHighlight */
  int HighlightPivotEnd;	/* right ... */
  int OldHighlightStart;	/* save data */
  int OldHighlightEnd;

  char *Text;			/* pointer to the text */
  int TextAlloc;		/* number of bytes allocated for the text */
  int TextLen;			/* current length of text */

  char *SelectionText;		/* pointer to text selection, when needed */
  int SelectionLen;		/* length */

  int FastInsertCursorStart;	/* data storage for some text optimization */
  int FastInsertTextLen;

  Dimension ViewWidth;		/* visible width of widget */
  int XOffset;			/* offset from x=0 to start of text string */
  int OldXOffset;
  int YOffset;			/* y pixel offset to baseline of font */
  int TextWidth;		/* char width of text */
  int OldTextWidth;

  XtIntervalId timer_id;	/* timer for double click test */
  int timer_x;			/* save event x pos */
  int highlight_time;		/* time delay for scrolling */
  int multi_click_time;		/* local storage for XtGetMultiClickTime */
} TextFieldPart;

typedef struct _TextFieldRec {
  CorePart core;
  TextFieldPart text;
} TextFieldRec;


#endif /* _TextFieldP_H */
