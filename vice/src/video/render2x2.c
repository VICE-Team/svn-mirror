/*
 * render2x2.c - 2x2 renderers
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

#include "render2x2.h"
#include "types.h"


/* 16 color 2x2 renderers */

void render_08_2x2_04(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                      int height, int xs, int ys, int xt, int yt, int pitchs,
                      int pitcht, int doublescan)
{
	BYTE *tmpsrc;
	BYTE *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast,oldwidth;
	register DWORD rcolor;
	register BYTE color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + xt;
	oldwidth=width;
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
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
	for (y=ys;y<(ys+height);y++)
	{
		tmpsrc=src;
		tmptrg=trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*tmptrg++=(BYTE)colortab[*tmpsrc++];
			}
			for (x=0;x<wstart;x++)
			{
				color=(BYTE)colortab[*tmpsrc++];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				rcolor=*((DWORD *)tmpsrc);
            tmpsrc += sizeof(DWORD);

				color=(BYTE)colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
				rcolor >>= 8;
				color=(BYTE)colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
				rcolor >>= 8;
				color=(BYTE)colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
				rcolor >>= 8;
				color=(BYTE)colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wend;x++)
			{
				color=(BYTE)colortab[*tmpsrc++];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			if (wlast)
			{
				*tmptrg++=(BYTE)colortab[*tmpsrc++];
			}
			if (y & 1) src += pitchs;
		}
		else
		{
			color=(BYTE)colortab[0];
			for (x=0;x<oldwidth;x++) *tmptrg++=color;
		}
		trg += pitcht;
	}
}

void render_16_2x2_04(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                      int height, int xs, int ys, int xt, int yt, int pitchs,
                      int pitcht, int doublescan)
{
	BYTE *tmpsrc;
	WORD *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast,oldwidth;
	register DWORD rcolor;
	register WORD color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt << 1);
	oldwidth=width;
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
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
	for (y=ys;y<(ys+height);y++)
	{
		tmpsrc=src;
		tmptrg=(WORD *)trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*tmptrg++=(WORD)colortab[*tmpsrc++];
			}
			for (x=0;x<wstart;x++)
			{
				color=(WORD)colortab[*tmpsrc++];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				rcolor=*((DWORD *)tmpsrc);
            tmpsrc += sizeof(DWORD);

				color=(WORD)colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
				rcolor >>= 8;
				color=(WORD)colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
				rcolor >>= 8;
				color=(WORD)colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
				rcolor >>= 8;
				color=(WORD)colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wend;x++)
			{
				color=(WORD)colortab[*tmpsrc++];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			if (wlast)
			{
				*tmptrg++=(WORD)colortab[*tmpsrc++];
			}
			if (y & 1) src += pitchs;
		}
		else
		{
			for (x=0;x<oldwidth;x++) *tmptrg++=0;
		}
		trg += pitcht;
	}
}

void render_24_2x2_04(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                      int height, int xs, int ys, int xt, int yt, int pitchs,
                      int pitcht, int doublescan)
{
	BYTE *tmpsrc;
	BYTE *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast,oldwidth;
	register DWORD color;
	register DWORD tcolor;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt * 3);
	oldwidth=width;
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
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
	for (y=ys;y<(ys+height);y++)
	{
		tmpsrc=src;
		tmptrg=trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				color=colortab[*tmpsrc++];
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
			}
			for (x=0;x<wstart;x++)
			{
				color=colortab[*tmpsrc++];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
			}
			for (x=0;x<wfast;x++)
			{
				color=colortab[*tmpsrc++];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				color=colortab[*tmpsrc++];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				color=colortab[*tmpsrc++];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				color=colortab[*tmpsrc++];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
			}
			for (x=0;x<wend;x++)
			{
				color=colortab[*tmpsrc++];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
			}
			if (wlast)
			{
				color=colortab[*tmpsrc++];
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
			}
			if (y & 1) src += pitchs;
		}
		else
		{
			for (x=0;x<oldwidth*3;x++) *tmptrg++=0;
		}
		trg += pitcht;
	}
}

void render_32_2x2_04(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                      int height, int xs, int ys, int xt, int yt, int pitchs,
                      int pitcht, int doublescan)
{
	BYTE *tmpsrc;
	DWORD *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast,oldwidth;
	register DWORD rcolor;
	register DWORD color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt << 2);
	oldwidth=width;
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
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
	for (y=ys;y<(ys+height);y++)
	{
		tmpsrc=src;
		tmptrg=(DWORD *)trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*tmptrg++=colortab[*tmpsrc++];
			}
			for (x=0;x<wstart;x++)
			{
				color=colortab[*tmpsrc++];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				rcolor=*((DWORD *)tmpsrc);
            tmpsrc += sizeof(DWORD);

				color=colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
				rcolor >>= 8;
				color=colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
				rcolor >>= 8;
				color=colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
				rcolor >>= 8;
				color=colortab[(BYTE)rcolor];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wend;x++)
			{
				color=colortab[*tmpsrc++];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			if (wlast)
			{
				*tmptrg++=colortab[*tmpsrc++];
			}
			if (y & 1) src += pitchs;
		}
		else
		{
			for (x=0;x<oldwidth;x++) *tmptrg++=0;
		}
		trg += pitcht;
	}
}

/*****************************************************************************/
/*****************************************************************************/

/* 16 color 2x2 renderers */

void render_08_2x2_08(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                      int height, int xs, int ys, int xt, int yt, int pitchs,
                      int pitcht, int doublescan)
{
	BYTE *pre;
	BYTE *tmppre;
	BYTE *tmpsrc;
	BYTE *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast,oldwidth;
	register BYTE color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + xt;
	oldwidth=width;
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
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
	pre = src-pitchs-1;
	for (y=ys;y<(ys+height);y++)
	{
		tmppre=pre;
		tmpsrc=src;
		tmptrg=trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*tmptrg++=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			for (x=0;x<wstart;x++)
			{
				color=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				color=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
				color=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
				color=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
				color=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wend;x++)
			{
				color=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			if (wlast)
			{
				*tmptrg++=(BYTE)colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			if (y & 1)
			{
				pre = src-1;
				src += pitchs;
			}
		}
		else
		{
			color=(BYTE)colortab[0];
			for (x=0;x<oldwidth;x++) *tmptrg++=color;
		}
		trg += pitcht;
	}
}

void render_16_2x2_08(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                      int height, int xs, int ys, int xt, int yt, int pitchs,
                      int pitcht, int doublescan)
{
	BYTE *pre;
	BYTE *tmppre;
	BYTE *tmpsrc;
	WORD *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast,oldwidth;
	register WORD color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt << 1);
	oldwidth=width;
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
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
	pre = src-pitchs-1;
	for (y=ys;y<(ys+height);y++)
	{
		tmppre=pre;
		tmpsrc=src;
		tmptrg=(WORD *)trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*tmptrg++=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			for (x=0;x<wstart;x++)
			{
				color=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				color=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
				color=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
				color=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
				color=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wend;x++)
			{
				color=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			if (wlast)
			{
				*tmptrg++=(WORD)colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			if (y & 1)
			{
				pre = src-1;
				src += pitchs;
			}
		}
		else
		{
			for (x=0;x<oldwidth;x++) *tmptrg++=0;
		}
		trg += pitcht;
	}
}

void render_24_2x2_08(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                      int height, int xs, int ys, int xt, int yt, int pitchs,
                      int pitcht, int doublescan)
{
	BYTE *pre;
	BYTE *tmppre;
	BYTE *tmpsrc;
	BYTE *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast,oldwidth;
	register DWORD color;
	register DWORD tcolor;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt * 3);
	oldwidth=width;
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
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
	pre = src-pitchs-1;
	for (y=ys;y<(ys+height);y++)
	{
		tmppre=pre;
		tmpsrc=src;
		tmptrg=trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
			}
			for (x=0;x<wstart;x++)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
			}
			for (x=0;x<wfast;x++)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
			}
			for (x=0;x<wend;x++)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				tcolor=color;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
				tcolor >>= 8;
				*tmptrg++=(BYTE)tcolor;
			}
			if (wlast)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
				color >>= 8;
				*tmptrg++=(BYTE)color;
			}
			if (y & 1)
			{
				pre = src-1;
				src += pitchs;
			}
		}
		else
		{
			for (x=0;x<oldwidth*3;x++) *tmptrg++=0;
		}
		trg += pitcht;
	}
}

void render_32_2x2_08(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                      int height, int xs, int ys, int xt, int yt, int pitchs,
                      int pitcht, int doublescan)
{
	BYTE *pre;
	BYTE *tmppre;
	BYTE *tmpsrc;
	DWORD *tmptrg;
	int x,y,wfirst,wstart,wfast,wend,wlast,oldwidth;
	register DWORD color;

	src=src + pitchs*(ys >> 1) + (xs >> 1);
	trg=trg + pitcht*yt + (xt << 2);
	oldwidth=width;
	wfirst = xs & 1;
	width -= wfirst;
	wlast = width & 1;
	width >>= 1;
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
	pre = src-pitchs-1;
	for (y=ys;y<(ys+height);y++)
	{
		tmppre=pre;
		tmpsrc=src;
		tmptrg=(DWORD *)trg;
		if ((y & 1) || doublescan)
		{
			if (wfirst)
			{
				*tmptrg++=colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			for (x=0;x<wstart;x++)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wfast;x++)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			for (x=0;x<wend;x++)
			{
				color=colortab[*tmpsrc++ | (*tmppre++ << 4)];
				*tmptrg++=color;
				*tmptrg++=color;
			}
			if (wlast)
			{
				*tmptrg++=colortab[*tmpsrc++ | (*tmppre++ << 4)];
			}
			if (y & 1)
			{
				pre = src-1;
				src += pitchs;
			}
		}
		else
		{
			for (x=0;x<oldwidth;x++) *tmptrg++=0;
		}
		trg += pitcht;
	}
}

