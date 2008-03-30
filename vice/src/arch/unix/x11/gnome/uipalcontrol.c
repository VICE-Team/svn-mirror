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
#include "resources.h"

typedef struct pal_res_s {
    char *label;
    char *res;
} pal_res_t;

pal_res_t ctrls[] = { { "Saturation", "ColorSaturation" },
		      { "Contrast", "ColorContrast" },
		      { "Brightness", "ColorBrightness" },
		      { "Gamma", "ColorGamma" },
		      { NULL, NULL } };

void upd_sb (GtkAdjustment *adj, gpointer data)
{
    pal_res_t *p = (pal_res_t *) data;
    resources_set_value(p->res, (resource_value_t) (int) adj->value);
}

GtkWidget *build_pal_ctrl_widget(void)
{
    GtkWidget *b, *hb;
    GtkObject *adj;
    GtkWidget *sb;
    GtkWidget *f;
    GtkWidget *l;
    int i, v;
    
    f = gtk_frame_new(_("PAL Settings"));
    
    b = gtk_vbox_new(FALSE, 10);

    for (i = 0; ctrls[i].label; i++)
    {
    
	hb = gtk_hbox_new(FALSE, 0);

	l = gtk_label_new(_(ctrls[i].label));
	gtk_box_pack_start(GTK_BOX(hb), l, FALSE, FALSE, 5);
	gtk_widget_show(l);
	
	adj = gtk_adjustment_new(0, 0, 2100, 1, 100, 100);
	
	resources_get_value(ctrls[i].res, (resource_value_t *) &v);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(adj), (gfloat) v);
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
    
    gtk_widget_show(b);
    gtk_container_add(GTK_CONTAINER(f), b);
    gtk_widget_show(f);
    
    return f;
}
