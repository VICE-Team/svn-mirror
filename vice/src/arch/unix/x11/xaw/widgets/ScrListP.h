/****************************************************************************

	ScrolledListP.h

	This file is the private definition file for the File Selector
	Widget.

 ****************************************************************************/

/*
 * Author:
 * 	Brian Totty
 * 	Department of Computer Science
 * 	University Of Illinois at Urbana-Champaign
 *	1304 West Springfield Avenue
 * 	Urbana, IL 61801
 * 
 * 	totty@cs.uiuc.edu
 * 	
 */ 

#ifndef	_SCROLLEDLISTP_H_
#define _SCROLLEDLISTP_H_

typedef struct _XfwfScrolledListClassPart
{
	int    empty;
} XfwfScrolledListClassPart;

typedef struct
{
	Widget				list;
	Widget				viewport;
	char				**item_array;
	int				item_count;
	Boolean				*sensitive_array;
	XtCallbackList			callback;
} XfwfScrolledListPart;

typedef struct _XfwfScrolledListClassRec
{
	CoreClassPart			core_class;
	CompositeClassPart		composite_class;
	XfwfScrolledListClassPart	scrolledList_class;
} XfwfScrolledListClassRec;

typedef struct _XfwfScrolledListRec
{
	CorePart			core;
	CompositePart			composite;
	XfwfScrolledListPart		scrolledList;
} XfwfScrolledListRec;

extern XfwfScrolledListClassRec xfwfScrolledListClassRec;

#endif
