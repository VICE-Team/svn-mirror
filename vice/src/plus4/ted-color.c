/*
 * vicii-color.c - Colors for the MOS 6569 (VIC-II) emulation.
 *
 * Written by
 *  John Selck <graham@cruise.de>
 *
 * Research about the YUV values by
 *  Philip Timmermann <pepto@pepto.de>
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
   The 16 C64 colors in YUV format

   Note: There are two different luminance values (Y) for
   each color. The old values are for VERY old VIC-II
   revisions, the new values are for all others. Beware:
   the old luminances make the colors look very ugly! I only
   included these values to have the colors complete.
   Commodore improved the Y values very fast after the
   release of the C64, so only very few machines actually
   have these colors. A wise decision, the palette gets
   much more usable with the new luminances.

   Formulas for color space conversions:

   Y = 0.299*R + 0.587*G + 0.114*B
   U = 0.493*(B - Y)
   V = 0.877*(R - Y)

   Please note that the factors on U and V have been removed,
   so to be exact, the color space is not YUV but YCbCr here.
*/

#include <math.h>	/* needed for pow function */
#include "ted-color.h"

static unsigned char ColorLookup[256];	/* simple lookup table: mixed color -> palette index */
static unsigned char GunData[256*9];	/* lookup table to avoid over/underflows in the 8 bit per RGB value + gamma correction   */
static   signed int IntColData[16*3];	/* integer YUV with saturation, brightness and contrast already calculated (for pal emu) */

/* new luminances */

#define LUMN0	  0.0f
#define LUMN1	 56.0f
#define LUMN2	 74.0f
#define LUMN3	 92.0f
#define LUMN4	117.0f
#define LUMN5	128.0f
#define LUMN6	163.0f
#define LUMN7	199.0f
#define LUMN8	256.0f

/* old luminances */

#define LUMO0	  0.0f
#define LUMO1	 56.0f
#define LUMO2	128.0f
#define LUMO3	191.0f
#define LUMO4	256.0f

/* color vectors */

/* grey */
#define GRYU	(  0.00f)
#define GRYV	(  0.00f)

/* red / -cyan */
#define REDU	(-25.07f)
#define REDV	( 43.38f)

/* -green / purple  INVERTED! */
#define GRNU	( 61.70f)
#define GRNV	( 29.62f)

/* blue / -yellow */
#define BLUU	( 80.89f)
#define BLUV	( -3.58f)

/* orange */
#define ORNU	(-52.69f)
#define ORNV	( 34.68f)

/* -brown */
#define BRNU	( 72.29f)
#define BRNV	(-20.71f)

static float C64Colors[64]=
{
/*new Y  old Y   U     V                       */
  LUMN0, LUMO0, GRYU, GRYV, /* 0 : black       */
  LUMN8, LUMO4,-GRYU,-GRYV, /* 1 : white       */
  LUMN2, LUMO1, REDU, REDV, /* 2 : red         */
  LUMN6, LUMO3,-REDU,-REDV, /* 3 : cyan        */
  LUMN3, LUMO2, GRNU, GRNV, /* 4 : purple      */
  LUMN5, LUMO2,-GRNU,-GRNV, /* 5 : green       */
  LUMN1, LUMO1, BLUU, BLUV, /* 6 : blue        */
  LUMN7, LUMO3,-BLUU,-BLUV, /* 7 : yellow      */
  LUMN3, LUMO2, ORNU, ORNV, /* 8 : orange      */
  LUMN1, LUMO1,-BRNU,-BRNV, /* 9 : brown       */
  LUMN5, LUMO2, REDU, REDV, /* A : light red   */
  LUMN2, LUMO1,-GRYU,-GRYV, /* B : dark grey   */
  LUMN4, LUMO2, GRYU, GRYV, /* C : grey        */
  LUMN7, LUMO3,-GRNU,-GRNV, /* D : light green */
  LUMN4, LUMO2, BLUU, BLUV, /* E : light blue  */
  LUMN6, LUMO3,-GRYU,-GRYV  /* F : light grey  */
};

/* default dithering */

static char dither[16]=
{
	0x00,0x0E,0x04,0x0C,
	0x08,0x04,0x04,0x0C,
	0x04,0x04,0x08,0x04,
	0x08,0x08,0x08,0x0C
};

/*
   Table of all unique chroma values in the 16 C64 colors

   The C64 colors have 9 different chromas (color tones):

   Grey   (0,B,C,F,1)
   Red    (2,A)
   Green  (5,D)
   Blue   (6,E)
   Cyan   (3)
   Purple (4)
   Yellow (7)
   Orange (8)
   Brown  (9)

   For palette based delay loop emulation it might be useful
   to have that information to save some colors in the palette.
*/

static const char unique_chroma[9]=
{
	0x00,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09
};

/* Lookup: what color has what index in unique_chroma */

static const char unique_chroma_lookup[16]=
{
	0,0,1,2,3,4,5,6,
	7,8,1,0,0,4,5,0
};

/* Names for C64 colors. (Only used on palettes with 16 entries) */

const char *vic_ii_color_names[16] =
{
  "Black", "White", "Red", "Cyan", "Purple", "Green", "Blue",
  "Yellow", "Orange", "Brown", "Light Red", "Dark Gray", "Medium Gray",
  "Light Green", "Light Blue", "Light Gray"
};

static void vic_ii_color_makecolor(float saturation,float contrast,float brightness,int newlum,unsigned char primary,unsigned char secondary,palette_entry_t *entry)
{
	short i4,k4,lum;
	float y,u,v,r,g,b,con;

	if (newlum) lum=0;
	else lum=1;

	i4=primary << 2;
	k4=secondary << 2;

	y=(C64Colors[i4+lum]);
	u=(C64Colors[i4+2]+C64Colors[k4+2])*0.5f;
	v=(C64Colors[i4+3]+C64Colors[k4+3])*0.5f;

	con=256.0f*contrast;
	y=(con*(y+brightness*128.0f));
	u=(con*(u*saturation));
	v=(con*(v*saturation));

	b=u+y;
	r=v+y;
	g=436.116f*y-130.399f*r-49.717f*b;

	entry->red   =GunData[(((int)r) >>  8)+1024];
	entry->green =GunData[(((int)g) >> 16)+1024];
	entry->blue  =GunData[(((int)b) >>  8)+1024];
	entry->dither=dither[primary];
}

/* Calculate one of the 16 C64 colors */

static void vic_ii_color_makebasecolor(float sat,float con,float bri,int lum,unsigned char color,palette_entry_t *entry)
{
  vic_ii_color_makecolor(sat,con,bri,lum,color,color,entry);
}

/* Calculated a mixed color out of 2 C64 colors, the primary color gives the brightness */

static void vic_ii_color_makemixedcolor(float sat,float con,float bri,int lum,unsigned char color,palette_entry_t *entry)
{
  vic_ii_color_makecolor(sat,con,bri,lum,(unsigned char)(color & 0x0F),(unsigned char)(color >> 4),entry);
}

/* Initialize the gamma table */

static void vic_ii_color_initgamma(int gamma)
{
	int x,xi;
	float af,gam;

	/* first avoid overflows/underflows for the 8 bits per color gun: */

	for (x=0;x<1024;x++)
	{
		GunData[x]=0;
		GunData[x+(1024+256)]=255;
	}

	/* calculate gamma curve for the valid 8 bit range: */

	gam=((float)gamma)/1000.0f;
	af=(float)pow(255.0f,1.0f-gam);
	for (x=0;x<256;x++)
	{
		xi=(int)(af*(float)pow((float)x,gam));
		if (xi < 0) xi = 0;		/* security (should never happen) */
		if (xi > 255) xi = 255; /* security (should never happen) */
		GunData[x+1024]=(unsigned char)xi;
	}
}

/* calculated a palette */

palette_t *vic_ii_color_calcpalette(int type,int saturation,int contrast,int brightness,int gamma,int newlum)
{
	int i;
	palette_t *palette;
	float sat,con,bri,gam;

	sat=((float)saturation)/1000.0f;
	con=((float)contrast)/1000.0f;
	bri=((float)(brightness-1000))/1000.0f;
	gam=((float)gamma)/1000.0f;
	vic_ii_color_initgamma(gamma);

	palette = 0;
	switch (type)
	{
	case VIC_II_COLOR_PALETTE_16:
		palette = palette_create(16, vic_ii_color_names);
		for (i=0;i<256;i++)
			ColorLookup[i]=(unsigned char)(i & 0x0F);
		for (i=0;i<(int)palette->num_entries;i++)
			vic_ii_color_makebasecolor(sat,con,bri,newlum,(unsigned char)i,&palette->entries[i]);
		break;
	case VIC_II_COLOR_PALETTE_256:
		palette = palette_create(16*16, 0);
		for (i=0;i<256;i++)
			ColorLookup[i]=(unsigned char)i;
		for (i=0;i<(int)palette->num_entries;i++)
			vic_ii_color_makemixedcolor(sat,con,bri,newlum,(unsigned char)i,&palette->entries[i]);
		break;
	case VIC_II_COLOR_PALETTE_LOOKUP:
		palette = palette_create(16*9, 0);
		for (i=0;i<256;i++)
			ColorLookup[i]=(unsigned char)((i & 0x0F) | (unique_chroma_lookup[i >> 4] << 4));
		for (i=0;i<(int)palette->num_entries;i++)
			vic_ii_color_makemixedcolor(sat,con,bri,newlum,(unsigned char)((i & 0x0F) | (unique_chroma[i >> 4] << 4)),&palette->entries[i]);
		break;
	}
	return palette;
}
