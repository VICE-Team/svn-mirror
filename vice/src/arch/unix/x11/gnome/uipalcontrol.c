/*
 * uipalcontrol.c - UI controls for PAL
 *
 * Written by
 *  Martin Pottendorfer (Martin.Pottendorfer@alcatel.at)
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
#include <gnome.h>

#include "ui.h"
#include "video.h"
#include "resources.h"
#include "videoarch.h"
#include "x11ui.h"

typedef struct pal_res_s {
    char *label;
    char *res;
    char *xv_name;
    Atom xv_atom;
    int xv_min;
    int xv_max;
    GtkObject *adj;
} pal_res_t;

static int xv_port;

static pal_res_t ctrls[] =
{
  { N_("Saturation"), "ColorSaturation", "XV_SATURATION", 0, 0, 0, NULL },
  { N_("Contrast"), "ColorContrast", "XV_CONTRAST", 0, 0, 0, NULL },
  { N_("Brightness"), "ColorBrightness", "XV_BRIGHTNESS", 0, 0, 0, NULL },
  { N_("Gamma"), "ColorGamma", "XV_GAMMA", 0, 0, 0, NULL },
};

static void upd_xv (int v_val, pal_res_t *p)
{
#ifdef HAVE_XVIDEO
    if (use_xvideo && p->xv_atom) {
        Display *dpy = x11ui_get_display_ptr();

	/* Map from VICE [0,2000] to XVideo [xv_min, xv_max]. */
	int v_min = 0, v_max = 2000;
	int v_zero = (v_min + v_max)/2;
	int v_range = v_max - v_min;

	int xv_zero = (p->xv_min + p->xv_max)/2;
	int xv_range = p->xv_max - p->xv_min;

	int xv_val = (v_val - v_zero)*xv_range/v_range + xv_zero;

	XvSetPortAttribute(dpy, xv_port, p->xv_atom, xv_val);
    }
#endif
}

static void upd_sb (GtkAdjustment *adj, gpointer data)
{
    pal_res_t *p = (pal_res_t *) data;
    resources_set_value(p->res, (resource_value_t) (int) adj->value);
    upd_xv(adj->value, p);
}

static void pal_ctrl_reset (GtkWidget *w, gpointer data)
{
    int i, tmp;

    for (i = 0; i < sizeof(ctrls)/sizeof(ctrls[0]); i++)
    {
        resources_get_default_value(ctrls[i].res, (resource_value_t *) &tmp);
	resources_set_value(ctrls[i].res, (resource_value_t) tmp);
	if (ctrls[i].adj) {
	    gtk_adjustment_set_value(GTK_ADJUSTMENT(ctrls[i].adj),
				     (gfloat) tmp);
	    upd_xv(tmp, &ctrls[i]);
	}
    }      
}

GtkWidget *build_pal_ctrl_widget(video_canvas_t *canvas)
{
    GtkWidget *b, *hb;
    GtkObject *adj;
    GtkWidget *sb;
    GtkWidget *f;
    GtkWidget *l, *c;
    GtkWidget *rb;
    int i, v;

#ifdef HAVE_XVIDEO
    int j, numattr;
    Display *dpy = NULL;
    XvAttribute *attr = NULL;

    if (use_xvideo && canvas->xv_image) {
        dpy = x11ui_get_display_ptr();
        attr = XvQueryPortAttributes(dpy, canvas->xv_port, &numattr);
	xv_port = canvas->xv_port;
    }
#endif
    
    f = gtk_frame_new(_("PAL Settings"));
    
    b = gtk_vbox_new(FALSE, 5);

    for (i = 0; i < sizeof(ctrls)/sizeof(ctrls[0]); i++)
    {
#ifdef HAVE_XVIDEO
        ctrls[i].xv_atom = 0;

        if (use_xvideo && canvas->xv_image) {
	    for (j = 0; j < numattr; j++) {
	        if (strcmp(ctrls[i].xv_name, attr[j].name) == 0) {
		    ctrls[i].xv_atom = XInternAtom(dpy, ctrls[i].xv_name, False);
		    ctrls[i].xv_min = attr[j].min_value;
		    ctrls[i].xv_max = attr[j].max_value;
		    break;
		}
	    }

	    /* Don't create useless scrollbars. */
	    if (!ctrls[i].xv_atom) {
	        continue;
	    }
	}
#endif
    
	hb = gtk_hbox_new(FALSE, 0);

	c = gtk_hbox_new(FALSE, 0);
	gtk_widget_set_usize(GTK_WIDGET(c), 80, 10);
	
	l = gtk_label_new(_(ctrls[i].label));
	gtk_container_add(GTK_CONTAINER(c), l);
	gtk_widget_show(l);
	
	gtk_box_pack_start(GTK_BOX(hb), c, FALSE, FALSE, 5);
	gtk_widget_show(c);
	
	ctrls[i].adj = adj = gtk_adjustment_new(0, 0, 2100, 1, 100, 100);
	
	resources_get_value(ctrls[i].res, (resource_value_t *) &v);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(adj), (gfloat) v);
	upd_xv(v, &ctrls[i]);
	sb = gtk_hscrollbar_new(GTK_ADJUSTMENT(adj));
	gtk_range_set_update_policy(GTK_RANGE(sb),
				    GTK_UPDATE_CONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hb), sb, TRUE, TRUE, 0);
	
	gtk_signal_connect (GTK_OBJECT(adj), "value_changed",
			    GTK_SIGNAL_FUNC (upd_sb), 
			    &ctrls[i]);
	
	gtk_widget_show(sb);
	gtk_box_pack_start(GTK_BOX(b), hb, TRUE, TRUE, 0);
	gtk_widget_show(hb);
    }

#ifdef HAVE_XVIDEO
    if (attr) {
        XFree(attr);
    }
#endif
    
    rb = gtk_button_new_with_label(_("Reset PAL Settings"));
    gtk_box_pack_start(GTK_BOX(b), rb, FALSE, FALSE, 5);
    gtk_signal_connect(GTK_OBJECT(rb), "clicked",
		       GTK_SIGNAL_FUNC(pal_ctrl_reset),
		       rb);
    GTK_WIDGET_UNSET_FLAGS (rb, GTK_CAN_FOCUS);
    gtk_widget_show(rb);

    gtk_widget_show(b);
    gtk_container_add(GTK_CONTAINER(f), b);
    gtk_widget_show(f);
    
    return f;
}
