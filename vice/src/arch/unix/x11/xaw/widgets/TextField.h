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

#ifndef _TextField_H
#define _TextField_H

#include <X11/Core.h>

#define _TextField_WIDGET_VERSION	1.0

#ifndef XtIsTextField
#define XtIsTextField(w) XtIsSubclass((Widget)w, textfieldWidgetClass)
#endif 

/* Athena style resource names */

#ifndef XtNecho
#define XtNecho			"echo"
#endif
#ifndef XtNpendingDelete
#define XtNpendingDelete	"pendingDelete"
#endif
#ifndef XtNlength
#define XtNlength		"length"
#endif
#ifndef XtNstring
#define XtNstring		"string"
#endif
#ifndef XtNinsertPosition
#define XtNinsertPosition	"insertPosition"
#endif
#ifndef XtNdisplayCaret
#define XtNdisplayCaret		"displayCaret"
#endif
#ifndef XtNeditable
#define XtNeditable		"editable"
#endif
#define XtNmargin		"margin"
#define XtNcursorWidth		"cursorWidth"
#define XtNallowSelection	"allowSelection"
#define XtNactivateCallback	"activateCallback"


/* Motif style resource names */

#ifndef XmNmaxLength
#define XmNmaxLength		XtNlength
#endif
#ifndef XmNvalue
#define XmNvalue		XtNvalue
#endif
#ifndef XmNcursorPosition
#define XmNcursorPosition	XtNinsertPosition
#endif
#ifndef XmNcursorPositionVisible
#define XmNcursorPositionVisible	XtNdisplayCaret
#endif
#ifndef XmNeditable
#define XmNeditable		XtNeditable
#endif
#ifndef XmNactivateCallback
#define XmNactivateCallback	XtNactivateCallback
#endif


extern WidgetClass textfieldWidgetClass;

typedef struct _TextFieldClassRec *TextFieldWidgetClass;
typedef struct _TextFieldRec      *TextFieldWidget;

typedef struct _TextFieldReturnStruct {
	int	reason;		/* Motif compatibility */
	XEvent	*event;
	char	*string;
} TextFieldReturnStruct;

/*
** Public function declarations
*/
#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* TextField.c */
Boolean TextFieldGetEditable P_((Widget aw));
int TextFieldGetInsertionPosition P_((Widget aw));
char *TextFieldGetString P_((Widget aw));
void TextFieldInsert P_((Widget aw, int pos, char *str));
void TextFieldReplace P_((Widget aw, int first, int last, char *str));
void TextFieldSetEditable P_((Widget aw, Boolean editable));
void TextFieldSetInsertionPosition P_((Widget aw, int pos));
void TextFieldSetSelection P_((Widget aw, int start, int end, Time time));
void TextFieldSetString P_((Widget aw, char *str));

#undef P_

#endif /* _TextField_H */
