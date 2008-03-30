/*
 * vicii-color.c - Colors for the MOS 6569 (VIC-II) emulation.
 *
 * Written by
 *  John Selck <graham@cruise.de>
 *
 * Research about the YUV values by
 *  Philip Thimerman <pepto@pepto.de>
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

/* the 16 c64 colors in YUV format */
/*
   note: there are two different luminance values (Y) for
   each color. the old values are for VERY old vic-ii
   revisions, the new values are for all others. Beware:
   the old luminances make the colors look very ugly! i only
   included these values to have the colors complete.
*/

#include <math.h>	/* needed for pow function */
#include "palette.h"

static float C64Colors[64]=
{
/*   new Y   old Y       U           V */
	  0.0f,	  0.0f,	  0.00f,	  0.00f,	// 0
	256.0f,	256.0f,	  0.00f,	  0.00f,	// 1
	 74.0f,	 56.0f,	-31.05f,	 42.14f,	// 2
	163.0f,	191.0f,	 31.05f,	-42.14f,	// 3
	 92.0f,	128.0f,	 57.37f,	 32.25f,	// 4
	128.0f,	128.0f,	-57.37f,	-32.25f,	// 5
	 56.0f,	 56.0f,	 81.14f,	  0.00f,	// 6
	199.0f,	191.0f,	-81.14f,	  0.00f,	// 7
	 92.0f,	128.0f,	-57.37f,	 32.25f,	// 8
	 56.0f,	 56.0f,	-74.96f,	 17.45f,	// 9
	128.0f,	128.0f,	-31.05f,	 42.14f,	// a
	 74.0f,	 56.0f,	  0.00f,	  0.00f,	// b
	117.0f,	128.0f,	  0.00f,	  0.00f,	// c
	199.0f,	191.0f,	-57.37f,	-32.25f,	// d
	117.0f,	128.0f,	 81.14f,	  0.00f,	// e
	163.0f,	191.0f,	  0.00f,	  0.00f,	// f
};

static char dither[16]=
{
	0x00,0x0E,0x04,0x0C,
	0x08,0x04,0x04,0x0C,
	0x04,0x04,0x08,0x04,
	0x08,0x08,0x08,0x0C
};

static unsigned char GunData[256*9];	// lookup table to avoid over/underflows in the 8 bit per RGB value + gamma correction
static   signed int IntColData[16*3];	// integer YUV with saturation, brightness and contrast already calculated (for pal emu)
static palette_entry_t RawColData[16];		// RGB palette of IntColData, for plain 16 color modes
static palette_entry_t PseudoColData[256];	// RGB palette of mixed colors, for fake pal emulation

void vic_ii_pal_initpalettes(float saturation,float brightness,float contrast,int newlum)
{
	int i,i4,k,k4,r,g,b,yi,ui,vi,lum;
	float y,u,v,con;
	palette_entry_t *entry;

	if (newlum) lum=0;
	else lum=1;

	con=256.0f*contrast;
	for (i=0;i<16;i++)
	{
		i4=i<<2;
		y=C64Colors[i4+lum];
		u=C64Colors[i4+2];
		v=C64Colors[i4+3];

		yi=(int)(con*(y+brightness*128.0f));
		ui=(int)(con*(u*saturation));
		vi=(int)(con*(v*saturation));

		IntColData[i]=yi;
		IntColData[i+16]=(ui*3) >> 3;
		IntColData[i+32]=(vi*3) >> 3;

		b=ui+yi;
		r=vi+yi;
		g=436*yi-130*r-50*b;

		r>>=8;
		g>>=16;
		b>>=8;

		entry=&RawColData[i];
		entry->red=GunData[r+1024];
		entry->green=GunData[g+1024];
		entry->blue=GunData[b+1024];
		entry->dither=dither[i];

		for (k=0;k<16;k++)
		{
			k4=k<<2;
			y=C64Colors[i4+lum];
			u=(C64Colors[i4+2]+C64Colors[k4+2])*0.5f;
			v=(C64Colors[i4+3]+C64Colors[k4+3])*0.5f;

			yi=(int)(con*(y+brightness*128.0f));
			ui=(int)(con*(u*saturation));
			vi=(int)(con*(v*saturation));

			b=ui+yi;
			r=vi+yi;
			g=436*yi-130*r-50*b;

			r>>=8;
			g>>=16;
			b>>=8;

			entry=&PseudoColData[i+(k<<4)];
			entry->red=GunData[r+1024];
			entry->green=GunData[g+1024];
			entry->blue=GunData[b+1024];
			entry->dither=dither[i];
		}
	}
}

void vic_ii_pal_initfilter(int saturation,int contrast,int brightness,int gamma,int newlum)
{
	int x;
	float xf,af,gam;

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
		xf=af*(float)pow((float)x,gam);
		GunData[x+1024]=(unsigned char)((int)xf);
	}

	/* calculate the vic ii palettes: */

	vic_ii_pal_initpalettes(((float)saturation)/1000.0f,((float)(brightness-1000))/1000.0f,((float)contrast)/1000.0f,newlum);
}

void vic_ii_pal_fillpalette(palette_t *palette)
{
	int i;
	palette_entry_t *src;
	palette_entry_t *dst;

	if (palette)
	{
		for (i=0;i<(int)palette->num_entries;i++)
		{
			src=&RawColData[i];
			dst=&palette->entries[i];
			dst->red=src->red;
			dst->green=src->green;
			dst->blue=src->blue;
			dst->dither=src->dither;
		}
	}
}
