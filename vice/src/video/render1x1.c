/*
 * render1x1.c - 1x1 renderers
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

#include "render1x1.h"
#include "types.h"


/* 16 color 1x1 renderers */

void render_08_1x1_04(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	BYTE *tmpsrc;
	BYTE *tmptrg;
	int x,y,wstart,wfast,wend;

	src += pitchs*ys + xs;
	trg += pitcht*yt + xt;
	if (width < 4)
	{
		wstart=width;
		wfast=0;
		wend=0;
	}
	else
	{
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=trg;
		for (x=0;x<wstart;x++)
		{
			*tmptrg++=(BYTE)colortab[*tmpsrc++];
		}
		for (x=0;x<wfast;x++)
        {
			tmptrg[0]=(BYTE)colortab[tmpsrc[0]];
			tmptrg[1]=(BYTE)colortab[tmpsrc[1]];
			tmptrg[2]=(BYTE)colortab[tmpsrc[2]];
			tmptrg[3]=(BYTE)colortab[tmpsrc[3]];
			tmptrg[4]=(BYTE)colortab[tmpsrc[4]];
			tmptrg[5]=(BYTE)colortab[tmpsrc[5]];
			tmptrg[6]=(BYTE)colortab[tmpsrc[6]];
			tmptrg[7]=(BYTE)colortab[tmpsrc[7]];
			tmpsrc += 8;
			tmptrg += 8;
		}
		for (x=0;x<wend;x++)
		{
			*tmptrg++=(BYTE)colortab[*tmpsrc++];
		}
		src += pitchs;
		trg += pitcht;
	}
}

void render_16_1x1_04(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	BYTE *tmpsrc;
	WORD *tmptrg;
	int x,y,wstart,wfast,wend;

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
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=(WORD *)trg;
		for (x=0;x<wstart;x++)
		{
			*tmptrg++=(WORD)colortab[*tmpsrc++];
		}
		for (x=0;x<wfast;x++)
		{
			tmptrg[0]=(WORD)colortab[tmpsrc[0]];
			tmptrg[1]=(WORD)colortab[tmpsrc[1]];
			tmptrg[2]=(WORD)colortab[tmpsrc[2]];
			tmptrg[3]=(WORD)colortab[tmpsrc[3]];
			tmptrg[4]=(WORD)colortab[tmpsrc[4]];
			tmptrg[5]=(WORD)colortab[tmpsrc[5]];
			tmptrg[6]=(WORD)colortab[tmpsrc[6]];
			tmptrg[7]=(WORD)colortab[tmpsrc[7]];
			tmpsrc += 8;
			tmptrg += 8;
		}
		for (x=0;x<wend;x++)
		{
			*tmptrg++=(WORD)colortab[*tmpsrc++];
		}
		src += pitchs;
		trg += pitcht;
	}
}

void render_24_1x1_04(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	BYTE *tmpsrc;
	BYTE *tmptrg;
	int x,y,wstart,wfast,wend;
	register DWORD color;

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
		wstart=4-((int)trg & 3);			/* alignment: 4 pixels*/
		wfast =(width - wstart) >> 2;		/* fast loop for 4 pixel segments*/
		wend  =(width - wstart) & 0x03;		/* do not forget the rest*/
	}
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=trg;
		for (x=0;x<wstart;x++)
		{
			color=colortab[*tmpsrc++];
			tmptrg[0]=(BYTE)color;
			color >>= 8;
			tmptrg[1]=(BYTE)color;
			color >>= 8;
			tmptrg[2]=(BYTE)color;
			tmptrg += 3;
		}
		for (x=0;x<wfast;x++)
		{
			color=colortab[tmpsrc[0]];
			tmptrg[0]=(BYTE)color;
			color >>= 8;
			tmptrg[1]=(BYTE)color;
			color >>= 8;
			tmptrg[2]=(BYTE)color;
			color=colortab[tmpsrc[1]];
			tmptrg[3]=(BYTE)color;
			color >>= 8;
			tmptrg[4]=(BYTE)color;
			color >>= 8;
			tmptrg[5]=(BYTE)color;
			color=colortab[tmpsrc[2]];
			tmptrg[6]=(BYTE)color;
			color >>= 8;
			tmptrg[7]=(BYTE)color;
			color >>= 8;
			tmptrg[8]=(BYTE)color;
			color=colortab[tmpsrc[3]];
			tmptrg[9]=(BYTE)color;
			color >>= 8;
			tmptrg[10]=(BYTE)color;
			color >>= 8;
			tmptrg[11]=(BYTE)color;
			tmpsrc += 4;
			tmptrg += 12;
		}
		for (x=0;x<wend;x++)
		{
			color=colortab[*tmpsrc++];
			tmptrg[0]=(BYTE)color;
			color >>= 8;
			tmptrg[1]=(BYTE)color;
			color >>= 8;
			tmptrg[2]=(BYTE)color;
			tmptrg += 3;
		}
		src += pitchs;
		trg += pitcht;
	}
}

void render_32_1x1_04(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	BYTE *tmpsrc;
	DWORD *tmptrg;
	int x,y,wstart,wfast,wend;

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
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=(DWORD *)trg;
		for (x=0;x<wstart;x++)
		{
			*tmptrg++=colortab[*tmpsrc++];
		}
		for (x=0;x<wfast;x++)
		{
			tmptrg[0]=colortab[tmpsrc[0]];
			tmptrg[1]=colortab[tmpsrc[1]];
			tmptrg[2]=colortab[tmpsrc[2]];
			tmptrg[3]=colortab[tmpsrc[3]];
			tmptrg[4]=colortab[tmpsrc[4]];
			tmptrg[5]=colortab[tmpsrc[5]];
			tmptrg[6]=colortab[tmpsrc[6]];
			tmptrg[7]=colortab[tmpsrc[7]];
			tmpsrc += 8;
			tmptrg += 8;
		}
		for (x=0;x<wend;x++)
		{
			*tmptrg++=colortab[*tmpsrc++];
		}
		src += pitchs;
		trg += pitcht;
	}
}

/*****************************************************************************/
/*****************************************************************************/

/* 256 color 1x1 renderers */

void render_08_1x1_08(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	BYTE *tmppre;
	BYTE *tmpsrc;
	BYTE *tmptrg;
	int x,y,wstart,wfast,wend;
	register DWORD rcolor;

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
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	tmppre = src-pitchs-1;
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=trg;
		for (x=0;x<wstart;x++)
		{
			*tmptrg++=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
		}
		for (x=0;x<wfast;x++)
		{
			rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
			tmptrg[0]=(BYTE)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[1]=(BYTE)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[2]=(BYTE)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[3]=(BYTE)colortab[(BYTE)rcolor];
			rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
			tmptrg[4]=(BYTE)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[5]=(BYTE)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[6]=(BYTE)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[7]=(BYTE)colortab[(BYTE)rcolor];
			tmptrg += 8;
		}
		for (x=0;x<wend;x++)
		{
			*tmptrg++=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
		}
		tmppre = src-1;
		src += pitchs;
		trg += pitcht;
	}
}

void render_16_1x1_08(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	BYTE *tmppre;
	BYTE *tmpsrc;
	WORD *tmptrg;
	int x,y,wstart,wfast,wend;
	register DWORD rcolor;

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
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	tmppre = src-pitchs-1;
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=(WORD *)trg;
		for (x=0;x<wstart;x++)
		{
			*tmptrg++=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
		}
		for (x=0;x<wfast;x++)
		{
			rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
			tmptrg[0]=(WORD)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[1]=(WORD)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[2]=(WORD)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[3]=(WORD)colortab[(BYTE)rcolor];
			rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
			tmptrg[4]=(WORD)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[5]=(WORD)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[6]=(WORD)colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[7]=(WORD)colortab[(BYTE)rcolor];
			tmptrg += 8;
		}
		for (x=0;x<wend;x++)
		{
			*tmptrg++=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
		}
		tmppre = src-1;
		src += pitchs;
		trg += pitcht;
	}
}

void render_24_1x1_08(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	BYTE *tmppre;
	BYTE *tmpsrc;
	BYTE *tmptrg;
	int x,y,wstart,wfast,wend;
	register DWORD rcolor;
	register DWORD color;

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
		wstart=4-((int)trg & 3);			/* alignment: 4 pixels*/
		wfast =(width - wstart) >> 2;		/* fast loop for 4 pixel segments*/
		wend  =(width - wstart) & 0x03;		/* do not forget the rest*/
	}
	tmppre = src-pitchs-1;
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=trg;
		for (x=0;x<wstart;x++)
		{
			color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
			*tmptrg++=(BYTE)color;
			color >>= 8;
			*tmptrg++=(BYTE)color;
			color >>= 8;
			*tmptrg++=(BYTE)color;
		}
		for (x=0;x<wfast;x++)
		{
			rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
			color=colortab[(BYTE)rcolor];
			tmptrg[0]=(BYTE)color;
			color >>= 8;
			tmptrg[1]=(BYTE)color;
			color >>= 8;
			tmptrg[2]=(BYTE)color;
			rcolor >>= 8;
			color=colortab[(BYTE)rcolor];
			tmptrg[3]=(BYTE)color;
			color >>= 8;
			tmptrg[4]=(BYTE)color;
			color >>= 8;
			tmptrg[5]=(BYTE)color;
			rcolor >>= 8;
			color=colortab[(BYTE)rcolor];
			tmptrg[6]=(BYTE)color;
			color >>= 8;
			tmptrg[7]=(BYTE)color;
			color >>= 8;
			tmptrg[8]=(BYTE)color;
			rcolor >>= 8;
			color=colortab[(BYTE)rcolor];
			tmptrg[9]=(BYTE)color;
			color >>= 8;
			tmptrg[10]=(BYTE)color;
			color >>= 8;
			tmptrg[11]=(BYTE)color;
			tmptrg += 12;
		}
		for (x=0;x<wend;x++)
		{
			color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
			*tmptrg++=(BYTE)color;
			color >>= 8;
			*tmptrg++=(BYTE)color;
			color >>= 8;
			*tmptrg++=(BYTE)color;
		}
		tmppre = src-1;
		src += pitchs;
		trg += pitcht;
	}
}

void render_32_1x1_08(DWORD *colortab,BYTE *src,BYTE *trg,int width,int height,int xs,int ys,int xt,int yt,int pitchs,int pitcht)
{
	BYTE *tmppre;
	BYTE *tmpsrc;
	DWORD *tmptrg;
	int x,y,wstart,wfast,wend;
	register DWORD rcolor;

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
		wstart=8-((int)trg & 7);			/* alignment: 8 pixels*/
		wfast =(width - wstart) >> 3;		/* fast loop for 8 pixel segments*/
		wend  =(width - wstart) & 0x07;		/* do not forget the rest*/
	}
	tmppre = src-pitchs-1;
	for (y=0;y<height;y++)
	{
		tmpsrc=src;
		tmptrg=(DWORD *)trg;
		for (x=0;x<wstart;x++)
		{
			*tmptrg++=colortab[*tmpsrc++ | (*tmppre++ << 4)];
		}
		for (x=0;x<wfast;x++)
		{
			rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
			tmptrg[0]=colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[1]=colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[2]=colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[3]=colortab[(BYTE)rcolor];
			rcolor = ((*((DWORD *)tmpsrc)++) | ((*((DWORD *)tmppre)++) << 4));
			tmptrg[4]=colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[5]=colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[6]=colortab[(BYTE)rcolor];
			rcolor >>= 8;
			tmptrg[7]=colortab[(BYTE)rcolor];
			tmptrg += 8;
		}
		for (x=0;x<wend;x++)
		{
			*tmptrg++=colortab[*tmpsrc++ | (*tmppre++ << 4)];
		}
		tmppre = src-1;
		src += pitchs;
		trg += pitcht;
	}
}

