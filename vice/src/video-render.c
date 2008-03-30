/*
 * video-render.c - Implementation of framebuffer to physical screen copy
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

#include "vice.h"

#include "types.h"
#include "video-render.h"


/* definitions of low level memory types (to avoid win32 specific types) */

/* !!! Make this go way by using BYTE, WORD and DWORD. !!! */

#ifndef _U8_DEFINED
#define _U8_DEFINED
typedef unsigned char U8;
#endif

#ifndef _U16_DEFINED
#define _U16_DEFINED
typedef unsigned short U16;
#endif

#ifndef _U32_DEFINED
#define _U32_DEFINED
typedef unsigned long U32;
#endif

/*********************************************************************************/
/*********************************************************************************/

/* 16 color 1x1 renderers */

static void render_08_1x1_04(U8 *src,U8 *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	U32 *tmpsrc;
	U32 *tmptrg;
	int x,y,wstart,wfast,wend;

	src=src + pitchs*ys + xs;
	trg=trg + pitcht*yt + xt;
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=4-((int)trg & 3);			// alignment: 4 pixels
		wfast =(width - wstart) >> 3;		// fast loop for 8 pixel segments
		wend  =(width - wstart) & 0x07;		// do not forget the rest
	}
	for (y=0;y<height;y++)
	{
		tmpsrc=(U32 *)src;
		tmptrg=(U32 *)trg;
		for (x=0;x<wstart;x++)
		{
			*((U8 *)tmptrg)++=*((U8 *)tmpsrc)++;
		}
		for (x=0;x<wfast;x++)
		{
			*tmptrg++=*tmpsrc++;
			*tmptrg++=*tmpsrc++;
		}
		for (x=0;x<wend;x++)
		{
			*((U8 *)tmptrg)++=*((U8 *)tmpsrc)++;
		}
		src += pitchs;
		trg += pitcht;
	}
}

static void render_16_1x1_04(U32 *colortab,U8 *src,U8 *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	U8 *tmpsrc;
	U16 *tmptrg;
	int x,y,wstart,wfast,wend;
	register U32 rcolor;

	src=src + pitchs*ys + xs;
	trg=trg + pitcht*yt + (xt << 1);
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=4-((int)trg & 3);			// alignment: 4 pixels
		wfast =(width - wstart) >> 2;		// fast loop for 4 pixel segments
		wend  =(width - wstart) & 0x03;		// do not forget the rest
	}
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=(U16 *)trg;
		for (x=0;x<wstart;x++)
		{
			*tmptrg++=(U16)colortab[*tmpsrc++];
		}
		for (x=0;x<wfast;x++)
		{
			rcolor=*((U32 *)tmpsrc)++;
			*tmptrg++=(U16)colortab[(U8)rcolor];
			rcolor >>= 8;
			*tmptrg++=(U16)colortab[(U8)rcolor];
			rcolor >>= 8;
			*tmptrg++=(U16)colortab[(U8)rcolor];
			rcolor >>= 8;
			*tmptrg++=(U16)colortab[(U8)rcolor];
		}
		for (x=0;x<wend;x++)
		{
			*tmptrg++=(U16)colortab[*tmpsrc++];
		}
		src += pitchs;
		trg += pitcht;
	}
}

static void render_24_1x1_04(U32 *colortab,U8 *src,U8 *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	U8 *tmpsrc;
	U8 *tmptrg;
	int x,y,wstart,wfast,wend;
	register U32 color;

	src=src + pitchs*ys + xs;
	trg=trg + pitcht*yt + (xt * 3);
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=4-((int)trg & 3);			// alignment: 4 pixels
		wfast =(width - wstart) >> 2;		// fast loop for 4 pixel segments
		wend  =(width - wstart) & 0x03;		// do not forget the rest
	}
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=trg;
		for (x=0;x<wstart;x++)
		{
			color=colortab[*tmpsrc++];
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
		}
		for (x=0;x<wfast;x++)
		{
			color=colortab[*tmpsrc++];
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
			color=colortab[*tmpsrc++];
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
			color=colortab[*tmpsrc++];
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
			color=colortab[*tmpsrc++];
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
		}
		for (x=0;x<wend;x++)
		{
			color=colortab[*tmpsrc++];
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
			color >>= 8;
			*tmptrg++=(U8)color;
		}
		src += pitchs;
		trg += pitcht;
	}
}

static void render_32_1x1_04(U32 *colortab,U8 *src,U8 *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	U8 *tmpsrc;
	U32 *tmptrg;
	int x,y,wstart,wfast,wend;
	register U32 rcolor;

	src=src + pitchs*ys + xs;
	trg=trg + pitcht*yt + (xt << 2);
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=4-((int)trg & 3);			// alignment: 4 pixels
		wfast =(width - wstart) >> 2;		// fast loop for 4 pixel segments
		wend  =(width - wstart) & 0x03;		// do not forget the rest
	}
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=(U32 *)trg;
		for (x=0;x<wstart;x++)
		{
			*tmptrg++=colortab[*tmpsrc++];
		}
		for (x=0;x<wfast;x++)
		{
			rcolor=*((U32 *)tmpsrc)++;
			*tmptrg++=(U8)rcolor;
			rcolor >>= 8;
			*tmptrg++=(U8)rcolor;
			rcolor >>= 8;
			*tmptrg++=(U8)rcolor;
			rcolor >>= 8;
			*tmptrg++=(U8)rcolor;
		}
		for (x=0;x<wend;x++)
		{
			*tmptrg++=colortab[*tmpsrc++];
		}
		src += pitchs;
		trg += pitcht;
	}
}

/*********************************************************************************/
/*********************************************************************************/

/* this function is the interface to the outer world */

void video_render_main(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                       int height, int xs, int ys, int xt, int yt, int pitchs,
                       int pitcht, int depth)
{
	switch (depth)
	{
	case 8:
		render_08_1x1_04(src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
		break;
	case 16:
		render_16_1x1_04((U32 *)colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
		break;
	case 24:
		render_24_1x1_04((U32 *)colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
		break;
	case 32:
		render_32_1x1_04((U32 *)colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
		break;
	}
}

/*********************************************************************************/
/*********************************************************************************/
