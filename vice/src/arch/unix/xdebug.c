/*
 * $Id: xdebug.c,v 2.0 1997/05/23 07:29:55 ettore Exp ettore $
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice
 *
 * X11-based 65xx tracer/debugger interface.
 *
 *
 * Written by
 *   Jarkko Sonninen  (sonninen@lut.fi)
 *
 * Patches by
 *   Jouko Valta      (jopi@stekt.oulu.fi)
 *   Andre Fachat     (a.fachat@physik.tu-chemnitz.de)
 *   Ettore Perazzoli (ettore@comm2000.it)
 *   Teemu Rantanen   (tvr@cs.hut.fi)
 *
 * $Log: xdebug.c,v $
 * Revision 2.0  1997/05/23 07:29:55  ettore
 * Code cleaned up and popup menu removed.
 * Adapted to the new CPU emulation.
 *
 * Revision 1.7  1996/07/30 08:26:20  fachat
 * Miscellaneous bugs fixed.
 * Improved Trace Mode.
 * floppy9Name and tapeName resources, with `-9' and `-1' options.
 * Experimental popup menu.
 * hjoyPort resource for the Linux joystick.
 *
 * Revision 1.6  1996/04/01  09:01:41  jopi
 * New options
 * syntax() rerarranged
 * New debugger functions for Next
 *
 * Revision 1.5  1995/11/07  16:51:00  jopi
 * *** empty log message ***
 *
 * Revision 1.4  1995/04/01  07:54:09  jopi
 * X64 0.3 PL 0
 * Added new options: -notraps -brevision.
 * Debugger status display added.
 *
 * Revision 1.3  1994/12/12  16:59:44  jopi
 * Added display of Memory Address, Memory Data, and Clock Cycles.
 *
 * Revision 1.2  1994/06/16  17:19:26  jopi
 * Xrm replaced with XtResources
 *
 * Revision 1.1  1994/02/18  18:26:14  jopi
 * Initial revision
 *
 *
 */

/* 1996/04/11 a.fachat
 *
 * We normally start in run mode. From "run" mode one can get to
 * "stop" or "trace" mode. From "stop", we should be able to execute
 * one opcode with "step". We can also get to "run" or "trace".
 * From "trace" we can get to "stop" or "run" mode.
 * -> always have three buttons named "run", "trace" and "stop".
 * -> add a button "step" for stop mode.
 */

#ifdef __hpux
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#endif

#include "vice.h"

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>     /* Get standard string definitions. */
#include <X11/cursorfont.h>

/*
 * And then the same again by hand...
 * If you are unfortunate enough not to have XPointer etc defined by your
 * system, you have to define by hand all types that are missing:
 */

#ifdef X_NOT_STDC_ENV
typedef char * XPointer;
#define XtOffsetOf(s_type,field) XtOffset(s_type*,field)
#endif

#ifndef XtNjumpProc
#define XtNjumpProc "jumpProc"
#endif


/*** String definitions. ***/
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Cardinals.h>

/*** Simple Widgets ***/
#include <X11/Xaw/Grip.h>
#include <X11/Xaw/List.h>
/* #include <X11/Xaw/Panner.h> */
/* #include <X11/Xaw/Repeater.h> */
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/StripChart.h>
#include <X11/Xaw/Toggle.h>

/*** Menus ***/
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>

/*** Text Widgets ***/
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Paned.h>

#include "vmachine.h"
#include "misc.h"
#include "video.h"
#include "interrupt.h"
#include "serial.h"
#include "mshell.h"
#include "sid.h"
#include "maincpu.h"
#include "ui.h"
#include "vsync.h"
#include "asm.h"

/*
 * Variables
 */


#ifdef LED_ARRAY
static Window ledwin = 0;
#endif

int  ledflg = 0;

int      	halt = 0;
static int      trace = 0, next = 0, step = 0;

static ADDRESS  current_point;
static char     buf[256];

static Display *debug_display;

/* Alpha #defines `R_SP' for its own purposes.  */
#ifdef R_SP
#undef R_SP
#endif

enum Register {
    R_AC, R_XR, R_YR, R_SP, R_PC, R_SR, R_EFF, R_DL, R_CLK, NO_REGISTERS
};

static char *registerLabels[] = {
    "AC:", "XR:", "YR:", "SP:", "PC:", "SR:", "EA>", "DL>", "Clk"
};


enum DState { DS_Run, DS_Halt, DS_Trace, DS_Step, DS_Break, DS_Mon };
enum DState  state;

static char *StatusLabels[] = {
    "Running.", "Halted.", "Trace.", "Execute.", "Break.", "Monitor Break."
};



/*
 * Functions
 */

extern void   end64 ( void );
extern Widget xdebug_create ( Widget parent );
extern void   toggle_cntl_win ( void );

extern void   handle_mainwin_event ( XEvent * );

static void   StopTrap(ADDRESS addr);

static void   ThumbProc ( Widget bar, XtPointer client, XtPointer ptr );
static void   ScrollProc ( Widget bar, XtPointer client, XtPointer ptr );
static void   RegisterBar ( Widget parent );

static void   UpdateRegisters ( void  );
static void   UpdateStatus ( enum DState );

static void   StopCb  ( Widget w, XtPointer clientData, XtPointer callData );
static void   RunCb   ( Widget w, XtPointer clientData, XtPointer callData );
static void   TraceCb ( Widget w, XtPointer clientData, XtPointer callData );
static void   NextCb  ( Widget w, XtPointer clientData, XtPointer callData );

#ifdef LED_ARRAY
static void   actLedBar	(Widget w, XtPointer clientData, XtPointer callData);
#endif

int  check_next ( void );
void loop_test ( int cc, BYTE src, int set );

static void  set_asm ( ADDRESS );
static void  draw_asm ( ADDRESS );

#ifdef LED_ARRAY

static void LedCb  ( Widget w, XtPointer clientData, XtPointer callData );


static void open_led_win ( void );
static int  toggle_ledbar ( void );
static void draw_ledbar ( void );
static void update_ledbar ( void );

#endif  /* LED_ARRAY */


/*
 * Widgets
 */

static Widget registers[NO_REGISTERS];
static Widget runButton, stopButton, traceButton, nextButton;
static Widget skipButton, stepButton;
static Widget view;
static Widget ASMbar;
static Widget status1;
static Widget XDebugger;


/* ------------------------------------------------------------------------- */


static void XDebugEnableTrap(ADDRESS addr)
{
    XtRealizeWidget(XDebugger);
    XtUnrealizeWidget(nextButton);
    XtUnrealizeWidget(stepButton);
    XtUnrealizeWidget(skipButton);
    UpdateRegisters();
    set_asm(maincpu_regs.pc);
    UpdateStatus(DS_Halt);
    maincpu_trigger_trap(StopTrap);
}

void    xdebug_enable (void)
{
    XDebugEnableTrap(0);
    /* maincpu_trigger_trap(XDebugEnableTrap); */
}

void    xdebug_disable (void)
{
    /*XtUnrealizeWidget (XDebugger);*/
    UpdateStatus(DS_Run);
    UiDisplayPaused(0);
}


static void UpdateRegisters (void)
{
    int eff;

    sprintf (buf, "$%02X", maincpu_regs.a);
    XtVaSetValues (registers [R_AC],
		   XtNstring, buf,
		   NULL);

    sprintf (buf, "$%02X", maincpu_regs.x);
    XtVaSetValues (registers [R_XR],
                   XtNstring, buf,
		   NULL);

    sprintf (buf, "$%02X", maincpu_regs.y);
    XtVaSetValues (registers [R_YR],
                   XtNstring, buf,
		   NULL);

    sprintf (buf, "$%02X", maincpu_regs.sp);
    XtVaSetValues (registers [R_SP],
                   XtNstring, buf,
		   NULL);

    sprintf (buf, "$%04X", maincpu_regs.pc);
    XtVaSetValues (registers [R_PC],
                   XtNstring, buf,
		   NULL);

#if 0
    /* FIXME!  */
    sprintf (buf, "$%02X", GET_SR());
    XtVaSetValues (registers [R_SR],
                   XtNstring, buf,
		   NULL);
#endif

    /* Memory */

    eff = eff_address(maincpu_regs.pc, 1);
    if (eff >= 0)
	sprintf (buf, "$%04X", eff);
    else
	sprintf (buf, "----");

    XtVaSetValues (registers [R_EFF],
                   XtNstring, buf,
		   NULL);

    if (eff >= 0)
	sprintf (buf, "$%02X", LOAD(eff));
    else
	sprintf (buf, "--");

    XtVaSetValues (registers [R_DL],
		   XtNstring, buf,
		   NULL);

    /* Clock */

    sprintf (buf, "%d", clk);
    XtVaSetValues (registers [R_CLK],
		   XtNstring, buf,
		   NULL);
}


/* Debugger Status */

static void UpdateStatus (enum DState dst)
{
    static int s = -1;
    int show = 0, hide = 0;

    state = dst;

    if (s == (int)dst)
	return;

    switch (dst)
    {
    case DS_Run:
	step = halt = trace = next = 0;
	hide = 1;
	break;
    case DS_Halt:
	halt = 1;
	trace = next = step = 0;
	show = 1;
	break;
    case DS_Trace:
	trace = 1;
	step = halt = next = 0;
	hide = 1;
	break;
    case DS_Step:
	step = 1;
	trace = next = halt = 0;
	break;

#if 0
    case DS_Next:
	next = 0;
	trace = step = halt = 0;
	break;
#endif

    default:
        /* Not handled. */
        break;
    }

    if (hide)
    {
	XtUnrealizeWidget(nextButton);
	XtUnrealizeWidget(stepButton);
	XtUnrealizeWidget(skipButton);
    }
    if (show)
    {
#if 0
	XtRealizeWidget(nextButton);
	XtManageChild(nextButton);
#endif
	XtRealizeWidget(stepButton);
	XtManageChild(stepButton);
	XtRealizeWidget(skipButton);
	XtManageChild(skipButton);
    }
    if (show || hide)
	XSync(debug_display, False);

    XtVaSetValues (status1,
		   XtNlabel, StatusLabels[dst],
		   NULL);
    s = dst;
}

/* ------------------------------------------------------------------------- */

static void RunTrap(ADDRESS a)
{
    UpdateRegisters();
    set_asm(maincpu_regs.pc);
}

static void RunCb(Widget w, XtPointer clientData, XtPointer callData)
{
    UpdateStatus(DS_Run);
    maincpu_trigger_trap(RunTrap);
    UiDisplayPaused(0);
}

static void StopTrap(ADDRESS addr)
{
    UpdateStatus(DS_Halt);
    UpdateRegisters();
    set_asm(maincpu_regs.pc);

    UiDisplayPaused(1);
    suspend_speed_eval();

    while (halt && !step)
	UiDispatchEvents();

    UiDisplaySpeed(0.0, 0.0);

    if (step)
	maincpu_trigger_trap(StopTrap);
}

static void StopCb(Widget w, XtPointer clientData, XtPointer callData)
{
    UpdateStatus(DS_Halt);
    maincpu_trigger_trap(StopTrap);
}


/* FIXME: Instead of asm(), Trace should display the branches encountered. */

static void TraceTrap(ADDRESS addr)
{
    UpdateRegisters();
    set_asm(maincpu_regs.pc);
    UiDispatchEvents();
    if (trace) {
	maincpu_trigger_trap(TraceTrap);
	XSync(debug_display, False);
    }
}

static void TraceCb(Widget w, XtPointer clientData, XtPointer callData)
{
    if (!trace) {
	UpdateStatus(DS_Trace);
	maincpu_trigger_trap(TraceTrap);
    } else {
	UpdateStatus(DS_Run);
    }
}

static void SkipCb(Widget w, XtPointer clientData, XtPointer callData)
{
    int addr_mode;
    addr_mode= lookup[LOAD(maincpu_regs.pc)].addr_mode;

    switch(addr_mode) {
      case IMPLIED: 	maincpu_regs.pc+=1; break;
      case IMMEDIATE: 	maincpu_regs.pc+=2; break;
      case ZERO_PAGE: 	maincpu_regs.pc+=2; break;
      case ZERO_PAGE_X:	maincpu_regs.pc+=2; break;
      case ZERO_PAGE_Y:	maincpu_regs.pc+=2; break;
      case ABSOLUTE:	maincpu_regs.pc+=3; break;
      case ABSOLUTE_X:	maincpu_regs.pc+=3; break;
      case ABSOLUTE_Y:	maincpu_regs.pc+=3; break;
      case INDIRECT_X:	maincpu_regs.pc+=2; break;
      case INDIRECT_Y:	maincpu_regs.pc+=2; break;
      case ABS_INDIRECT:maincpu_regs.pc+=3; break;
      case RELATIVE:	maincpu_regs.pc+=2; break;
    }
    UpdateRegisters();
    set_asm(maincpu_regs.pc);
    UpdateStatus(DS_Halt);

}

static void StepCb(Widget w, XtPointer clientData, XtPointer callData)
{
    UpdateStatus(DS_Step);
}

static BYTE next_sp = 0xFF;


#if 0
static void NextTrap(void)
{
    BYTE  src  = LOAD(maincpu_regs.pc +1);


   /*
    * Halt, Trace: if "Next" is pressed when JSR is to be executed, tracing
    *   is turned off until execution returns back, or the entry is removed
    *   from stack.
    * Halt: If a conditional branch backwards is encountered, turns on Run
    *   mode until execution has passed the loop. Elsewhere, works like "Step".
    * Trace: Skip the rest of subroutine being traced.
    * This button has no use in Run mode.
    */

    if (halt) {					/* Halted */
	switch (LOAD(maincpu_regs.pc)) {
	  case 0x20:	/* JSR */
	    next_sp = SP;
	    next = 1;
	    break;


	    /* BRANCH instructions. */

	  case 0x10:	/* BPL */
	    loop_test (!IF_SIGN(), src, 1);
	    break;

	  case 0x30:	/* BMI */
	    loop_test (IF_SIGN(), src, 1);
	    break;

	  case 0x50:	/* BVC */
	    loop_test (!IF_OVERFLOW(), src, 1);
	    break;

	  case 0x70:	/* BVS */
	    loop_test (IF_OVERFLOW(), src, 1);
	    break;

	  case 0x90:	/* BCC */
	    loop_test (!IF_CARRY(), src, 1);
	    break;

	  case 0xB0:	/* BCS */
	    loop_test (IF_CARRY(), src, 1);
	    break;

	  case 0xD0:	/* BNE */
	    loop_test (!IF_ZERO(), src, 1);
	    break;

	  case 0xF0:	/* BEQ */
	    loop_test (IF_ZERO(), src, 1);
	    break;

	  default:
	    trace = 1;		/* Single Step */
#ifdef DEBUG
	    printf ("single-step\n");
#endif
	}  /* switch */

    }  /* halt */

    else {
	if (trace) {				/* Tracing */
	    /*trace = 0;*/
	}
    }

    UpdateRegisters();
    set_asm(maincpu_regs.pc);
    UiDispatchEvents();
    maincpu_trigger_trap(NextTrap);
}
#endif

static void NextCb(Widget w, XtPointer clientData, XtPointer callData)
{
#if 0
    UpdateStatus(DS_Next);
    maincpu_trigger_trap(NextTrap);
#endif
    printf("NextCb() broken\n");
}


/* ------------------------------------------------------------------------- */


/* Check if we've done the program structure
 * returns nonzero when the next instruction is reached.
 */

int  check_next (void)
{
    BYTE  src  = LOAD(maincpu_regs.pc +1);


    switch (LOAD(maincpu_regs.pc)) {

      case 0x40:	/* RTI */
      case 0x60:	/* RTS */
      case 0x68:	/* PLA */
	if (maincpu_regs.sp > next_sp)
	    next = 0;
	break;

      case 0x9A:	/* TXS */
	if (maincpu_regs.x > next_sp)
	    next = 0;
	break;


	/* BRANCH instructions. */

      case 0x10:	/* BPL */
	loop_test (!maincpu_regs.psp.n, src, 0);
	break;

      case 0x30:	/* BMI */
	loop_test (maincpu_regs.psp.n, src, 0);
	break;

      case 0x50:	/* BVC */
	loop_test (!maincpu_regs.psp.v, src, 0);
	break;

      case 0x70:	/* BVS */
	loop_test (maincpu_regs.psp.v, src, 0);
	break;

      case 0x90:	/* BCC */
	loop_test (!maincpu_regs.psp.c, src, 0);
	break;

      case 0xB0:	/* BCS */
	loop_test (maincpu_regs.psp.c, src, 0);
	break;

      case 0xD0:	/* BNE */
	loop_test (!maincpu_regs.psp.z, src, 0);
	break;

      case 0xF0:	/* BEQ */
	loop_test (maincpu_regs.psp.z, src, 0);

    }  /* switch */


    return (!next);
}


void loop_test (int cc, BYTE src, int set)
{
    static ADDRESS  next_begin = 0, next_end  = 0;
    static int      next_cnt = 0;


    if (src & 0x80) {

	if (set) {
	    if (cc) {
		next_begin = maincpu_regs.pc + (signed char)src;
		next_end   = maincpu_regs.pc;
		next_cnt = 0;
		++next;
#ifdef DEBUG
		printf ("pc %04x  cc %d  -- set %d loop %04x %04x  SET LOOP\n",
			maincpu_regs.pc, cc, set, next_begin, next_end);
#endif
	    }

	    trace = 1;			/* Excute instruction */
	}
	else if (maincpu_regs.pc == next_end) {	/* Don't set but test only */
	    if (cc) {
		++next_cnt;
	    }
	    else {
		printf ("Loop  %04x %04x  executed %d times.\n",
			next_begin, next_end, next_cnt);
		next_begin = 0;
		next_end   = 0;
		next_cnt = 0;
		next = 0;
	    }
	}
    }    /* offset */
    else
	trace = 1;			/* Do single-step instead */

}


/* ------------------------------------------------------------------------- */


/*
 * Assembly Window
 */

static void ThumbProc (Widget bar, XtPointer client, XtPointer ptr)
{
    float percent = *(float*) ptr;
    ADDRESS p = ((int) (percent * 0xffff)) & 0xffff;
    draw_asm (p);
    return;
}


static void ScrollProc (Widget bar, XtPointer client, XtPointer ptr)
{
    int position = (int) ptr;

    /*printf ("position %d\n", position);*/
    if (position > 0) {
	draw_asm (current_point + 0x1);
    } else {
	draw_asm (current_point - 0x1);
    }
    return;
}


static void set_asm (/*Widget bar,*/ ADDRESS p)
{
    float percent = (p & 0xffff) / 65535.0;

    XawScrollbarSetThumb (ASMbar, percent, 0.02);
    draw_asm (p);
}


static void draw_asm (ADDRESS p)
{
    Widget  sink;
    char    buf2[8192];
    short   lines, height;  /* NOT ints */
    int i;


    buf2[0] = '\0';
    current_point = p;

    XtVaGetValues (view,
	 XtNheight, &height,
	 XtNtextSink, &sink,
	 NULL);
    lines = XawTextSinkMaxLines (sink, height) + 1;

    for (i = 0; i <= lines; i++) {
	p &= 0xffff;
	sprintf (buf, "%04X %s  %-s\n", p,
		 sprint_ophex (p), sprint_opcode(p, 1));
	strcat (buf2, buf);
	p += clength[lookup[LOAD(p)].addr_mode];
    }

    XtVaSetValues (view,
                   XtNstring, buf2,
		   NULL);
}


/* ------------------------------------------------------------------------- */


static void RegisterBar (Widget parent)
{
    int i;
    Widget form;
    Widget label, lastlabel;

    form = parent;

    /*
     * Registers
     */

    for (lastlabel = NULL, i = 0; i < NO_REGISTERS; i++, lastlabel = label) {
	label = XtVaCreateManagedWidget (
	     "register", labelWidgetClass, form,
	     XtNlabel, registerLabels[i],
	     XtNfromVert, lastlabel,
	     NULL);
	registers[i] = XtVaCreateManagedWidget (
	     "value",asciiTextWidgetClass, form,
	     XtNtype,XawAsciiString,
	     XtNeditType, XawtextRead,
	     XtNfromHoriz, label,
	     XtNfromVert, lastlabel,
	     NULL);
    }

    /*
     * Debugger Status
     */

    status1 = XtVaCreateManagedWidget (
	"status", labelWidgetClass, form,
	XtNlabel, StatusLabels[DS_Run],
	XtNfromVert, lastlabel,
	NULL);

    /*
     * Buttons
     */

    runButton = XtVaCreateManagedWidget (
	"run", commandWidgetClass, form,
	XtNfromVert, status1,
	NULL);
    XtAddCallback(runButton, XtNcallback, RunCb, NULL);

    stopButton = XtVaCreateManagedWidget (
	"stop", commandWidgetClass, form,
	 XtNfromVert, runButton,
	 NULL);
    XtAddCallback(stopButton, XtNcallback, StopCb, NULL);

    traceButton = XtVaCreateManagedWidget (
	"trace", commandWidgetClass, form,
	 XtNfromVert, stopButton,
	 NULL);
    XtAddCallback(traceButton, XtNcallback, TraceCb, NULL);

    nextButton = XtVaCreateManagedWidget (
	"next", commandWidgetClass, form,
	XtNfromVert, traceButton,
	NULL);
    XtAddCallback(nextButton, XtNcallback, NextCb, NULL);

    stepButton = XtVaCreateManagedWidget (
	"step", commandWidgetClass, form,
	XtNfromVert, nextButton,
	NULL);
    XtAddCallback(stepButton, XtNcallback, StepCb, NULL);

    skipButton = XtVaCreateManagedWidget (
	"skip", commandWidgetClass, form,
	XtNfromVert, stepButton,
	NULL);
    XtAddCallback(skipButton, XtNcallback, SkipCb, NULL);
}


/* ------------------------------------------------------------------------- */


/* Build the XDebugger as a child of `parent'. */
Widget xdebug_create (Widget parent)
{
    Widget form;
    Widget vbox;

    XDebugger = form = XtVaCreateManagedWidget("XDebug",
					       formWidgetClass, parent, NULL);

    /*
     * Assembly listing
     */

    ASMbar = XtVaCreateManagedWidget("bar", scrollbarWidgetClass, form, NULL);
    XtAddCallback(ASMbar, XtNjumpProc, ThumbProc, NULL);
    XtAddCallback(ASMbar, XtNscrollProc, ScrollProc, NULL);

    view = XtVaCreateManagedWidget
	("view", asciiTextWidgetClass, form,
	 XtNfromHoriz, ASMbar,
	 NULL);
    vbox = XtVaCreateManagedWidget
	("vbox", formWidgetClass, form,
	 XtNfromHoriz, view,
	 NULL);
    RegisterBar (vbox);

    debug_display = XtDisplay (parent);

#ifdef LED_ARRAY
    /*
     * Bus Activity LEDs Window
     */

    open_led_win();
    /*led = XtCreateManagedWidget("led", panedWidgetClass,
			      toplevel, NULL, 0);*/
#endif

    return XDebugger;
}


/* ------------------------------------------------------------------------- */


/*
 *  LED Bar - a simple toy.
 */

#ifdef LED_ARRAY

#define NUM_LEDS	16
#define LED_DIM		2	/* cycles */
#define LED_HALF	(CYCLES_PER_SEC >> 5)
#define LED_BRIGHT	(CYCLES_PER_SEC >> 4)

#define LED_X		10
#define LED_Y		20
#define LEDX_SPC	15
#define XOFF		20
#define YOFF		20
#define LED_WID		(LEDX_SPC * NUM_LEDS + 2 * XOFF)
#define LED_HEI		(LED_Y + 40 + 2 * YOFF)


static GC      led_gc[4];
static GC      debuggc;


/* fonts */

static XFontStruct *font_info;	/* Structure containing font info */
static int     font_height;
static int     font_width;

static char   *font_list[] = {
    "-*-helvetica-bold-r-*-*-12-*",
    "-*-courier-bold-r-*-*-12-*",
    "-*-times-bold-r-*-*-12-*",
    "fixed",
    NULL,
};


static void LedCb(Widget w, XtPointer clientData, XtPointer callData)
{
    ledflg = toggle_ledbar();
}

/* ------------------------------------------------------------------------- */

static void open_led_win (void)
{
    Window    rootw;
    XSizeHints size_hints;
    XTextProperty windowName, iconName;
    XWMHints  wm_hints;
    XClassHint class_hints;
    XGCValues gcval;
    XColor    exact_def, rgb_def;
    Colormap  def_cmap = DefaultColormap(debug_display, screen);
    int       i;

    char  **p = font_list;
    char   *win_name = "Bus Activity";
    char   *icon_name = "LEDs";


    /* win */

    size_hints.min_width = size_hints.width = LED_WID;
    size_hints.min_height = size_hints.height = LED_HEI;
    size_hints.flags = PSize | PMinSize;

    rootw = RootWindow(debug_display, screen);
    ledwin = XCreateSimpleWindow(debug_display, rootw, 0, 0,
	size_hints.width, size_hints.height, 1,
	BlackPixel(debug_display, screen),
	WhitePixel(debug_display, screen));

    if (debugflg)
	printf("LED Window open\n");

    XSetStandardProperties(debug_display, ledwin, win_name, icon_name,
	None, NULL, 0, &size_hints);

    /* Select event types wanted */
    XSelectInput(debug_display, ledwin,
	ExposureMask | StructureNotifyMask | FocusChangeMask);

    if (XStringListToTextProperty(&win_name, 1, &windowName) == 0) {
	(void) fprintf(stderr,
	    "%s: structure allocation for windowName failed.\n",
	    progname);
	exit(-1);
    }
    if (XStringListToTextProperty(&icon_name, 1, &iconName) == 0) {
	(void) fprintf(stderr,
	    "%s: structure allocation for iconName failed.\n",
	    progname);
	exit(-1);
    }

    wm_hints.initial_state = NormalState;
    wm_hints.input = True;
    wm_hints.flags = StateHint | InputHint;

    class_hints.res_name = progname;
    class_hints.res_class = APPLICATION_CLASS;

    XSetWMProperties(debug_display, ledwin, &windowName, &iconName, NULL, 0,
	&size_hints, &wm_hints, &class_hints);


    /* Allocate colors for images on screen */

    gcval.foreground = BlackPixel(debug_display, screen);
    gcval.background = WhitePixel(debug_display, screen);
    debuggc = XCreateGC(debug_display, rootw, GCForeground | GCBackground,
			&gcval);

    if (!led_gc[0]) {
	char *lc_names[] = { "red4", "red3", "red2", "red" };
	/*char *lc_names[] = { "blue", "red4", "yellow", "green" };*/

	if (DefaultDepth(debug_display, screen) < 4) {
	    fprintf (stderr, "Cannot create LED bar.\n");
	    return;
	}

	/* Allocate read only colors for each LED brightness */

	for (i = 0; i < 4; ++i) {
	    if (!XAllocNamedColor(debug_display, def_cmap,
				  lc_names[i], &exact_def, &rgb_def)) {
		fprintf(stderr, "Cannot allocate color %s\n", lc_names[i]);
		return;
	    }

	    gcval.foreground = exact_def.pixel;
	    gcval.background = WhitePixel(debug_display, screen);
	    led_gc[i] =
		XCreateGC(debug_display, rootw, GCForeground | GCBackground,
			  &gcval);
	}  /* for */
    }


    /* font */

    while (p && *p && !(font_info = XLoadQueryFont(debug_display, *p))) ++p;
    if (!p) {
	(void) fprintf(stderr, "%s: Fatal: No fonts available.\n", progname);
	exit(-1);
    }
    font_height = font_info -> ascent + font_info -> descent;
    font_width = font_info -> max_bounds.width;


    XSetFont(debug_display, debuggc, font_info -> fid);
}


static int  toggle_ledbar (void)
{
    static int  st = 0;

    if (!st) XMapWindow (debug_display, ledwin);
    else XUnmapWindow(debug_display, ledwin);

    st = !st;
    return (st);
}


static void draw_ledbar (void)
{
    int  i;


    for (i = 0; i < NUM_LEDS; ++i)
	XDrawImageString (debug_display, ledwin, debuggc,
			  XOFF + i * LEDX_SPC + 2,
			  YOFF + LED_Y + 15 /*font->ascent*/ +2,
			  "0123456789ABCDEF" + i, 1);

    XDrawImageString (debug_display, ledwin, debuggc,
		      XOFF * 2, YOFF + LED_Y + 40 /*font->ascent*/ +2,
		      "VIC-74154 Virtual Bus Tester", 30);

    update_ledbar ();
}


static void update_ledbar (void)
{
    GC   gcl;
    int  i, x = XOFF;
    int *r;


    if (!ledwin)
	return;


    for (i = 0, r = &ledarr[0]; i < NUM_LEDS; ++i, ++r) {

	if (*r < LED_DIM)
	    gcl = led_gc[0];
	else if (*r < LED_HALF)
	    gcl = led_gc[1];
	else if (*r < LED_BRIGHT)
	    gcl = led_gc[2];
	else
	    gcl = led_gc[3];

	XFillRectangle (debug_display, ledwin, gcl, x, YOFF, LED_X, LED_Y);

	x += LEDX_SPC;
	*r = 0;

    }  /* for */
}

#endif  /* LED_ARRAY */
