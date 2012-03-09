/*
 * uipalcontrol.c - Xaw only, UI controls for CRT emu
 *
 * Written by
 *  Olaf Seibert <rhialto@falu.nl>
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
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Toggle.h>

#include "lib.h"
#include "util.h"
#include "resources.h"
#include "videoarch.h"

void destroy_pal_ctrl_widget(Widget w, XtPointer client_data, XtPointer call_data);

typedef struct pal_res_s {
    char *label;        /* Label of Adjustmentbar */
    char *res;          /* Associated resource */
    int scale;          /* Value ranges 0...scale */
    Widget labelwidget; /* widget holding the label */
    Widget scrollbar;   /* pointer to scrollbar */
} pal_res_t;

static pal_res_t ctrls[] = {
    { N_("Blur"), "PALBlur", 1000, },
    { N_("Scanline shade"), "PALScanLineShade", 1000, },
    { N_("Saturation"), "ColorSaturation", 2000, },
    { N_("Contrast"), "ColorContrast", 2000, },
    { N_("Brightness"), "ColorBrightness", 2000, },
    { N_("Gamma"), "ColorGamma", 4000, },
    { N_("Tint"), "ColorTint", 2000, },
    { N_("Odd lines phase"), "PALOddLinePhase", 2000, },
    { N_("Odd lines offset"), "PALOddLineOffset", 2000, },
};

typedef struct {
    Widget shell;
    video_canvas_t *cached_canvas;
    pal_res_t ctrls[0];
} cleanup_data_t;

#define THUMB_SIZE      ((float) 0.05)

static void ScrollbarSetThumb(Widget scrollbar, float position)
{
    if (position > 1.0 - THUMB_SIZE) {
        position = 1.0 - THUMB_SIZE;
    }
    XawScrollbarSetThumb(scrollbar, position, THUMB_SIZE);
}

static void JumpProc(Widget scrollbar, XtPointer client_data, XtPointer percent_ptr)
{
    float fraction;
    int value;
    pal_res_t *p = (pal_res_t *)client_data;

    fraction = *(float *)percent_ptr;
    if (fraction > 1.0 - THUMB_SIZE) {
        fraction = 1.0 - THUMB_SIZE;
    }
    value = fraction * p->scale;

    resources_set_int(p->res, value);
}

static void ScrollProc(Widget scrollbar, XtPointer client_data, XtPointer positionptr /* int */)
{
    float delta;
    int length;
    float oldposition, sizeOfThumb;
    long position = (long)positionptr;
    pal_res_t *p = (pal_res_t *)client_data;

    XtVaGetValues(scrollbar,
                  XtNlength, &length,
                  XtNtopOfThumb, &oldposition,
                  XtNshown, &sizeOfThumb,
                  NULL);
    if (position < 0) { /* right button, strangely enough */
        delta = +THUMB_SIZE;
    } else {
        delta = -THUMB_SIZE;
    }

    oldposition += delta;
    ScrollbarSetThumb(scrollbar, oldposition);
    resources_set_int(p->res, oldposition * p->scale);
}

static void GetWH(Widget widget, int *w, int *h)
{
    XtVaGetValues(widget,
                  XtNwidth, w,
                  XtNheight, h,
                  NULL);
}

static void ResetProc(Widget w, XtPointer client_data, XtPointer dummy)
{
    cleanup_data_t *p = (cleanup_data_t *)client_data;
    unsigned int i;
    int tmp;
    float fraction;

    for (i = 0; i < util_arraysize(ctrls); i++) {
        resources_get_default_value(p->ctrls[i].res, (void *)&tmp);
        resources_set_int(p->ctrls[i].res, tmp);
        fraction = (float)tmp / p->ctrls[i].scale;
        if (p->ctrls[i].scrollbar) {
            ScrollbarSetThumb(p->ctrls[i].scrollbar, fraction);
        }
    }

    video_canvas_refresh_all(p->cached_canvas);
}

Widget build_pal_ctrl_widget_sliders(video_canvas_t *canvas, Widget parent, cleanup_data_t **cleanup_p)
{
    unsigned int i;
    int v;
    char *chip;
    pal_res_t *ctrldata;
    Widget widestlabel = 0;
    int widestsize = -1, labelheight;
    Widget form, toplabel, reset;
    Widget fromVert;
    cleanup_data_t *cleanupdata;

    cleanupdata = lib_malloc(sizeof(cleanup_data_t) + sizeof(ctrls));
    cleanupdata->cached_canvas = canvas;
    ctrldata = (pal_res_t *)(cleanupdata + 1);
    memcpy(ctrldata, ctrls, sizeof(ctrls));
    *cleanup_p = cleanupdata;

    form = XtVaCreateManagedWidget("palControlsForm",
                                    formWidgetClass, parent,
                                    XtNtop, XawChainBottom,
                                    XtNbottom, XawChainBottom,
                                    XtNleft, XawChainLeft,
                                    XtNright, XawChainRight,
                                    NULL);

    toplabel = XtVaCreateManagedWidget("topLabel",
                                       labelWidgetClass, form,
                                       XtNjustify, XtJustifyLeft,
                                       XtNlabel, _("CRT emulation settings"),
                                       XtNborderWidth, 0,
                                       XtNtop, XawChainTop,
                                       XtNbottom, XawChainTop,
                                       NULL);

    reset = XtVaCreateManagedWidget("reset",
                                    commandWidgetClass, form,
                                    XtNlabel, _("Reset"),
                                    XtNwidth, 50,
                                    XtNheight, 15,
                                    XtNfromHoriz, toplabel,
                                    XtNtop, XawChainTop,
                                    XtNbottom, XawChainTop,
                                    NULL);
    XtAddCallback(reset, XtNcallback, ResetProc, (XtPointer)cleanupdata);

    chip = canvas->videoconfig->chip_name;
    fromVert = toplabel;

    /* First place the labels, from top to bottom */
    for (i = 0; i < util_arraysize(ctrls); i++) {
        char *resname;
        char *labelname;
        Widget label;
        int width;

        resname = util_concat(chip, ctrls[i].res, NULL);
        ctrldata[i].res = resname;

        labelname = ctrldata[i].label;
        label = XtVaCreateManagedWidget(labelname,
                                        labelWidgetClass, form,
                                        XtNjustify, XtJustifyLeft,
                                        XtNlabel, _(labelname),
                                        XtNborderWidth, 0,
                                        XtNfromVert, fromVert,
                                        XtNleft, XawChainLeft,
                                        XtNright, XawChainLeft,
                                        NULL);

        fromVert = label;
        ctrldata[i].labelwidget = label;

        /* Determine which of the labels is the widest */
        width = 0;
        GetWH(label, &width, &labelheight);
        if (width > widestsize) {
            widestlabel = label;
            widestsize = width;
        }
    }

    /* Then place the scollbars, to the right of the widest label */
    fromVert = toplabel;
    for (i = 0; i < util_arraysize(ctrls); i++) {
        char *resname = ctrldata[i].res;
        char *scrollbarname;
        Widget scroll;
        int borderwidth = 1;

        scrollbarname = resname;
        scroll = XtVaCreateManagedWidget(scrollbarname,
                                        scrollbarWidgetClass, form,
                                        XtNorientation, XtorientHorizontal,
                                        XtNfromHoriz, widestlabel,
                                        XtNfromVert, fromVert,
                                        XtNheight, labelheight - 2 * borderwidth,
                                        XtNwidth, 200,
                                        XtNborderWidth, borderwidth,
                                        XtNleft, XawChainLeft,
                                        XtNright, XawChainRight,
                                        NULL);
        XtAddCallback(scroll, XtNscrollProc, ScrollProc, (XtPointer)&ctrldata[i]);
        XtAddCallback(scroll, XtNjumpProc, JumpProc, (XtPointer)&ctrldata[i]);

        ctrldata[i].scrollbar = scroll;
        fromVert = scroll;

        ctrldata[i].scale /= (1.0 - THUMB_SIZE);
        resources_get_int(resname, &v);
        ScrollbarSetThumb(scroll, (float)((float)v / ctrldata[i].scale));
    }

    return form;
}

/* This is needed to catch the `Close' command from the Window Manager. */
/*static Atom wm_delete_window;*/

void ToggleProc(Widget w, XtPointer client_data, XtPointer toggle)
{
    Widget showhide = (Widget)client_data;

    if (toggle) {
        XtPopup(showhide, XtGrabNone);
	/*XSetWMProtocols(display, XtWindow(showhide), &wm_delete_window, 1);*/
    } else {
        XtPopdown(showhide);
    }
}

/*
 * Create a toggle button, to be placed on the UI window.
 * If clicked, it toggles a transient shell, which contains
 * the controls to change the CRT settings.
 *
 * FIXME: clicking the close box of the popup window confuses
 * X and terminates VICE:
 * XIO:  fatal IO error 11 (Resource temporarily unavailable) on X server ":0.0"
 */
Widget build_pal_ctrl_widget(video_canvas_t *canvas, Widget parent)
{
    Widget sliders;
    Widget toggle;
    Widget shell;
    cleanup_data_t *cleanupdata;

    toggle = XtVaCreateManagedWidget("toggle",
                                     toggleWidgetClass, parent,
                                     XtNlabel, _("CRT Controls"),
                                     XtNwidth, 100,
                                     XtNheight, 18,
                                     XtNresizable, 0,
                                     NULL);

    /* popup window */
    shell = ui_create_transient_shell(parent, "PAL CRT Controls");
    /* put in the sliders */
    sliders = build_pal_ctrl_widget_sliders(canvas, shell, &cleanupdata);
    (void)sliders;
    cleanupdata->shell = shell;

    XtAddCallback(toggle, XtNcallback, ToggleProc, (XtPointer)shell);
    XtAddCallback(toggle, XtNdestroyCallback, destroy_pal_ctrl_widget, cleanupdata);

    /*wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);*/

    return toggle;
}

void destroy_pal_ctrl_widget(Widget w, XtPointer client_data, XtPointer call_data)
{
    cleanup_data_t *clean = (cleanup_data_t *)client_data;
    int i;

    if (clean->shell) {
        XtDestroyWidget(clean->shell);
    }
    for (i = 0; i < util_arraysize(ctrls); i++) {
        lib_free(clean->ctrls[i].res);
    }
    lib_free(clean);
}
