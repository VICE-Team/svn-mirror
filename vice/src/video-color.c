/*
 * video-color.h - Video implementation of YUV, YCbCr and RGB colors
 *
 * Written by
 *  John Selck <graham@cruise.de>
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

/*

  formulas for color space conversion:

  RGB to YCbCr:

  Y  = 0.299*R + 0.587*G + 0.114*B
  Cb = B - Y
  Cr = R - Y

  YCbCr to RGB:

  G = Y - (0.114/0.587)*Cb - (0.299/0.587)*Cr
  B = Cb + Y
  R = Cr + Y

  YCbCr to YUV:

  U = 0.493111*Cb
  V = 0.877283*Cr

  YUV is the PAL colorspace. It's just a slightly modified YCbCr for
  better broadcasting.

*/

#include <stdlib.h>
#include <math.h>	/* needed for pow function */
#include "video-color.h"
#include "video-resources.h"
#include "machine.h"

#define MATH_PI		3.141592653589793238462643383279 // 5028841971 6939937510 5820974944 5923078164 0628620899 8628034825 3421170679

typedef struct video_ycbcr_color_s {
	float y;
	float cb;
	float cr;
} video_ycbcr_color_t;

/* variables needed for generating and activating a palette */

static video_cbm_palette_t *video_current_palette=NULL;
static raster_t *video_current_raster=NULL;

void video_set_palette(video_cbm_palette_t *palette)
{
	video_current_palette=palette;
}

void video_set_raster(raster_t *raster)
{
	video_current_raster=raster;
}

/* conversion of VIC/VIC-II/TED colors to YCbCr */

static void video_convert_cbm_to_ycbcr(video_cbm_color_t *src,float basesat,float phase,video_ycbcr_color_t *dst)
{
	dst->y = src->luminance;

	/* chrominance (U and V) of color */

	dst->cb=(float)(basesat*cos((src->angle+phase)*(MATH_PI/180.0)));
	dst->cr=(float)(basesat*sin((src->angle+phase)*(MATH_PI/180.0)));

	/* convert UV to CbCr */

	dst->cb /= 0.493111f;
	dst->cr /= 0.877283f;

	/* direction of color vector (-1 = inverted vector, 0 = grey vector) */

	if (src->direction == 0)
	{
		dst->cb = 0.0f;
		dst->cr = 0.0f;
	}
	if (src->direction < 0)
	{
		dst->cb = -dst->cb;
		dst->cr = -dst->cr;
	}

}

/* conversion of YCbCr to RGB */

static void video_convert_ycbcr_to_rgb(video_ycbcr_color_t *src,float sat,float bri,float con,float gam,palette_entry_t *dst)
{
	float rf,bf,gf;
	double factor;
	int r,g,b;

	/* apply saturation */

	src->cb *= sat;
	src->cr *= sat;

	/* convert YCbCr to RGB */

	bf = src->cb + src->y;
	rf = src->cr + src->y;
	gf = src->y - (0.114f/0.587f)*src->cb - (0.299f/0.587f)*src->cr;

	/* apply brightness and contrast */

	rf = (rf+bri) * con;
	gf = (gf+bri) * con;
	bf = (bf+bri) * con;

	/* apply gamma correction */

	factor=pow(255.0f,1.0f-gam);
	rf = (float)(factor * pow(rf, gam));
	gf = (float)(factor * pow(gf, gam));
	bf = (float)(factor * pow(bf, gam));

	/* convert to int and clip to 8 bit boundaries */

	r = (int)rf;
	g = (int)gf;
	b = (int)bf;

	if (r <   0) r =   0;
	if (r > 255) r = 255;
	if (g <   0) g =   0;
	if (g > 255) g = 255;
	if (b <   0) b =   0;
	if (b > 255) b = 255;

	dst->dither = 0;
	dst->red    = (BYTE)r;
	dst->green  = (BYTE)g;
	dst->blue   = (BYTE)b;
	dst->name   = NULL;
}

/* gammatable calculation */

extern BYTE gammatable[1024+256+1024];

static void video_calc_gammatable(void)
{
	int i;
	float sat,bri,con,gam,v;
	double factor;

	sat=((float)(video_resources.color_saturation     ))/1000.0f;
	bri=((float)(video_resources.color_brightness-1000))*(128.0f/1000.0f);
	con=((float)(video_resources.color_contrast       ))/1000.0f;
	gam=((float)(video_resources.color_gamma          ))/1000.0f;

	factor=pow(255.0f,1.0f-gam);
	for (i=0;i<1024+256+1024;i++)
	{
		v=(float)(i-1024);
		v=(v + bri) * con;
		v=(float)(factor * pow(v, gam));
		if (v <   0.0f) v =   0.0f;
		if (v > 255.0f) v = 255.0f;
		gammatable[i]=(int)v;
	}
}

/* ycbcr table calculation */

extern SDWORD  ytable[128];
extern SDWORD cbtable[128];
extern SDWORD crtable[128];

static void video_calc_ycbcrtable(video_cbm_palette_t *p)
{
	video_ycbcr_color_t primary;
	unsigned int i;
	float sat;

	sat=((float)(video_resources.color_saturation     ))*(256.0f/1000.0f);
	for (i=0;i<p->num_entries;i++)
	{
		video_convert_cbm_to_ycbcr(&p->entries[i],p->saturation,p->phase,&primary);
		 ytable[i] = (SDWORD)(primary.y  * 256.0f);
		cbtable[i] = (SDWORD)(primary.cb * sat);
		crtable[i] = (SDWORD)(primary.cr * sat);
	}
}

/* calculate a RGB palette out of VIC/VIC-II/TED colors */

static palette_t *video_calc_palette(video_cbm_palette_t *p)
{
	palette_t *prgb;
	video_ycbcr_color_t primary;
	unsigned int i,j,index;
	float sat,bri,con,gam,cb,cr;

	sat=((float)(video_resources.color_saturation     ))/1000.0f;
	bri=((float)(video_resources.color_brightness-1000))*(128.0f/1000.0f);
	con=((float)(video_resources.color_contrast       ))/1000.0f;
	gam=((float)(video_resources.color_gamma          ))/1000.0f;
	if ((!video_resources.delayloop_emulation) || (p->num_entries > 16))
	{
		/* create RGB palette with the base colors of the video chip */

		prgb = palette_create(p->num_entries, NULL);
		if (prgb == NULL) return NULL;

		for (i=0;i<p->num_entries;i++)
		{
			video_convert_cbm_to_ycbcr(&p->entries[i],p->saturation,p->phase,&primary);
			video_convert_ycbcr_to_rgb(&primary,sat,bri,con,gam,&prgb->entries[i]);
		}
	}
	else
	{
		/* create RGB palette with the mixed base colors of the video chip */
		/* this is for the fake pal emu only, maximum 16 colors allowed */

		prgb = palette_create(p->num_entries * p->num_entries, NULL);
		if (prgb == NULL) return NULL;

		index=0;
		for (j=0;j<p->num_entries;j++)
		{
		  video_convert_cbm_to_ycbcr(&p->entries[j],p->saturation,p->phase,&primary);
		  cb = primary.cb;
		  cr = primary.cr;
		  for (i=0;i<p->num_entries;i++)
		  {
			video_convert_cbm_to_ycbcr(&p->entries[i],p->saturation,p->phase,&primary);
			primary.cb = (primary.cb + cb) * 0.5f;
			primary.cr = (primary.cr + cr) * 0.5f;
			video_convert_ycbcr_to_rgb(&primary,sat,bri,con,gam,&prgb->entries[index]);
			index++;
		  }
		}
	}
	return prgb;
}

/* load RGB palette */

static palette_t *video_load_palette(video_cbm_palette_t *p,const char *name)
{
	palette_t *palette;

	palette = palette_create(p->num_entries, NULL);
	if (palette == NULL) return NULL;

	if (!console_mode && !vsid_mode && palette_load(name, palette) < 0)
	{
/*	    log_message(vic_ii.log, "Cannot load palette file `%s'.", name); */
	    return NULL;
	}

	return palette;
}

/* calculate or load a palette, depending on configuration */

int video_update_palette()
{
	palette_t *palette;

	if (video_current_palette == NULL) return 0;
	if (video_current_raster  == NULL) return 0;

	video_calc_gammatable();
	video_calc_ycbcrtable(video_current_palette);
	if (video_resources.ext_palette) palette=video_load_palette(video_current_palette,video_resources.palette_file_name);
	else palette=video_calc_palette(video_current_palette);

	if (palette != NULL) return raster_set_palette(video_current_raster,palette);
	return -1;
}
