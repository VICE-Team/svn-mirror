/*
 * $Id: xdebug.h,v 1.1 1997/05/22 21:46:43 ettore Exp ettore $
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 * Header file for the X11-based 65xx tracer/debugger by
 * Jarkko Sonninen.
 *
 * Written by
 *   Ettore Perazzoli	(ettore@comm2000.it)
 *
 * $Log: xdebug.h,v $
 * Revision 1.1  1997/05/22 21:46:43  ettore
 * Initial revision
 *
 */


#ifndef _XDEBUG_H
#define _XDEBUG_H

#define XDEBUG_FALLBACK_RESOURCES			\
    "*XDebug.vbox.register.width:        40",		\
    "*XDebug.vbox.value*font:            fixed",	\
    "*XDebug.vbox.run.Label:             Run",		\
    "*XDebug.vbox.run.width:             150",		\
    "*XDebug.vbox.stop.Label:            Stop",		\
    "*XDebug.vbox.stop.width:            150",		\
    "*XDebug.vbox.trace.Label:           Trace",	\
    "*XDebug.vbox.trace.width:           150",		\
    "*XDebug.vbox.next.Label:            Next",		\
    "*XDebug.vbox.next.width:            150",		\
    "*XDebug.vbox.step.Label:            Step",		\
    "*XDebug.vbox.step.width:            150",		\
    "*XDebug.vbox.skip.Label:            Skip",		\
    "*XDebug.vbox.skip.width:            150",		\
    "*XDebug.view.allowVert:		 True",		\
    "*XDebug.view.forceBars:		 True",		\
    "*XDebug.view.width:		 200",		\
    "*XDebug.view.height:		 360", 		\
    "*XDebug.view*font:			 fixed",	\
    "*XDebug.hbox.orientation:		 Horizontal",	\
    "*XDebug.hbox.borderWidth:		 0",		\
    "*XDebug.hbox.min:			 28",		\
    "*XDebug.hbox.max:			 28",		\
    "*XDebug.vbox.orientation:		 Vertical",	\
    "*XDebug.vbox.borderWidth:		 0",		\
    "*XDebug*Label*borderWidth:		 0",		\
    "*XDebug.bar.orientation:		 Vertical",	\
    "*XDebug.bar.length:		 360",		\
    "*XDebug.bar.shown:			 0.02",		\
    "*XDebug*foreground:		 Black",	\
    "*XDebug*background:		 Light Gray",	\
    "*XDebug*Label.foreground		 black"

Widget xdebug_create(Widget parent);
void xdebug_enable(void);
void xdebug_disable(void);

#endif /* ndef _XDEBUG_H */
