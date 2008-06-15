/*
 * render2x2pal.c - 2x2 PAL renderers
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

#include <stdio.h>

#include "render2x2.h"
#include "render2x2pal.h"
#include "types.h"
#include "video-resources.h"

extern DWORD gamma_red[256 * 3];
extern DWORD gamma_grn[256 * 3];
extern DWORD gamma_blu[256 * 3];

extern DWORD gamma_red_fac[256 * 3];
extern DWORD gamma_grn_fac[256 * 3];
extern DWORD gamma_blu_fac[256 * 3];

/* PAL 2x2 renderers */

void render_16_2x2_pal(video_render_color_tables_t *color_tab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
                       const unsigned int doublescan)
{
    const SDWORD *cbtable = color_tab->cbtable;
    const SDWORD *crtable = color_tab->crtable;
    const SDWORD *ytablel = color_tab->ytablel;
    const SDWORD *ytableh = color_tab->ytableh;
    const DWORD *colortab = color_tab->physical_colors;
    const BYTE *tmpsrc;
    WORD *tmptrg;
    SDWORD *lineptr0;
    SDWORD *lineptr1;
    SDWORD *line;
    SDWORD *linepre;
    unsigned int x, y, wfirst, wstart, wfast, wend, wlast, wint, yys;
    WORD color;
    SDWORD l, u, v;
    DWORD red, grn, blu, red2, grn2, blu2;

    src = src + pitchs * ys + xs - 2;
    trg = trg + pitcht * yt + (xt << 1);
    yys = (ys << 1) | (yt & 1);
    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        /* alignment: 8 pixels*/
        wstart = (unsigned int)(8 - ((unsigned long)trg & 7));
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + wfirst + wlast + 5;
    lineptr0 = color_tab->line_yuv_0;
    lineptr1 = color_tab->line_yuv_1;

    tmpsrc = src - (ys ? pitchs : 0);
    line = lineptr0;
    for (x = 0; x < wint; x++) {
        register DWORD cl0, cl1, cl2, cl3;

        cl0 = tmpsrc[0];
        cl1 = tmpsrc[1];
        cl2 = tmpsrc[2];
        cl3 = tmpsrc[3];
        line[0] = 0;
        line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
        line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];
        tmpsrc++;
        line += 3;
    }

    if (yys & 1) {
        line = lineptr0;
        lineptr0 = lineptr1;
        lineptr1 = line;

        tmpsrc = src;
        line = lineptr0;
        for (x = 0;x < wint; x++) {
            register DWORD cl0, cl1, cl2, cl3;

            cl0 = tmpsrc[0];
            cl1 = tmpsrc[1];
            cl2 = tmpsrc[2];
            cl3 = tmpsrc[3];
            line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
            line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                      + cbtable[cl3];
            line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                      + crtable[cl3];
            tmpsrc++;
            line += 3;
        }
    }
    line = lineptr1;

    for (y = yys; y < (yys + height); y++) {
        tmpsrc = src;
        tmptrg = (WORD *)trg;
        if ((!(y & 1)) || doublescan) {
            if (!(y & 1)) {
                line = lineptr0;
                lineptr0 = lineptr1;
                lineptr1 = line;

                tmpsrc = src;
                line = lineptr0;
                for (x = 0; x < wint; x++) {
                    register DWORD cl0, cl1, cl2, cl3;

                    cl0 = tmpsrc[0];
                    cl1 = tmpsrc[1];
                    cl2 = tmpsrc[2];
                    cl3 = tmpsrc[3];
                    line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
                    line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                            + cbtable[cl3];
                    line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                            + crtable[cl3];
                    tmpsrc++;
                    line += 3;
                }

            }

            line = lineptr0;
            linepre = lineptr1;

            l = line[0];
            u = (line[1] + linepre[1]) >> 3;
            v = (line[2] + linepre[2]) >> 3;
            line += 3;
            linepre += 3;

            red = ((v + l) >> 8) + 256;
            blu = ((u + l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

            if (!(y & 1)) {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red[(red+red2) >> 1]
                                | gamma_grn[(grn+grn2) >> 1]
                                | gamma_blu[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu]);
                    *tmptrg++ = (WORD)(gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = (WORD)(gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu]);
                }
            } else {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v+l) >> 8) + 256;
                    blu2 = ((u+l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x <(wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red_fac[red] | gamma_grn_fac[grn]
                              | gamma_blu_fac[blu]);
                    *tmptrg++ = (WORD)(gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = (WORD)(gamma_red_fac[red] | gamma_grn_fac[grn]
                            | gamma_blu_fac[blu]);
                }
            }
            if (y & 1)
                src += pitchs;
        } else {
            color = (WORD)colortab[0];
            if (wfirst) {
                *tmptrg++ = color;
            }
            for (x = 0; x < wstart; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            for (x = 0; x < wfast; x++) {
                tmptrg[0] = color;
                tmptrg[1] = color;
                tmptrg[2] = color;
                tmptrg[3] = color;
                tmptrg[4] = color;
                tmptrg[5] = color;
                tmptrg[6] = color;
                tmptrg[7] = color;
                tmptrg[8] = color;
                tmptrg[9] = color;
                tmptrg[10] = color;
                tmptrg[11] = color;
                tmptrg[12] = color;
                tmptrg[13] = color;
                tmptrg[14] = color;
                tmptrg[15] = color;
                tmptrg += 16;
            }
            for (x = 0;x < wend; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            if (wlast) {
                *tmptrg = color;
            }
            if (y & 1)
                src += pitchs;
        }
        trg += pitcht;
    }
}

/*
	render shaded scanlines in 32bit mode
	
	the "simple scanlines" filter uses only the previous scanline to calculate the color of
	the "empty" scanlines. 

        the other filter is more correct in that it takes the previous and next scanline and 
	interpolates between them. this however doesnt work correctly (yet?) - need to investigate
	the whacked up video caching some more.
	
	note that renderers which call this one afterwards do not have to draw the black odd lines
	by themselves.
*/
#define SIMPLE_SCANLINES
void render_32_2x2_pal_scanlines(video_render_color_tables_t *color_tab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
                       const unsigned int doublescan)
{
    const DWORD *colortab = color_tab->physical_colors;
    register DWORD color;

    unsigned int x, y, wfirst, wstart, wfast, wend, wlast, wint, yys;

    DWORD *tmptrgw;
    DWORD *tmptrg,*tmpsrc;
    DWORD red, grn, blu;
#ifdef SIMPLE_SCANLINES
    float shade=((float)(video_resources.pal_scanlineshade)) / 1000.0f;
#else
    DWORD *tmpsrc2,*tmpsrc3;
    DWORD  red2, grn2, blu2;
    float shade=((float)(video_resources.pal_scanlineshade)) / 2000.0f;
#endif

    trg = trg + pitcht * yt + (xt << 2);
    yys = (ys << 1) | (yt & 1);

    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        /* alignment: 8 pixels*/
        wstart = (unsigned int)(8 - ((unsigned long)trg & 7));
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + wfirst + wlast + 5;
    
    /* loop over all lines */
    if(doublescan)
    {
	tmpsrc = NULL;
	tmptrg = NULL;
	
#ifdef SIMPLE_SCANLINES
/*	for (y = (yt); y < (yt + (height)); y++) { */
        for (y = (yys); y < (yys + (height)); y++) {
		if((y&1)==0)
		{
			tmpsrc=(DWORD*)trg;
		}
		else
		{
			tmptrg=(DWORD*)trg; /* draw pointer */
			
			/* draw a line */
			
			if(tmpsrc!=NULL)
			{
				if (wfirst) {
					red = (*tmpsrc>>16)&0xff;
					grn = (*tmpsrc>>8)&0xff;
					blu = (*tmpsrc>>0)&0xff;
					tmpsrc++;
	 
					red = (DWORD)((red)*shade)&0xff;
					blu = (DWORD)((blu)*shade)&0xff;
					grn = (DWORD)((grn)*shade)&0xff;
					
					*tmptrg++ = (red<<16)|(grn<<8)|(blu);
				}
				
				for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
					
					red = (*tmpsrc>>16)&0xff;
					grn = (*tmpsrc>>8)&0xff;
					blu = (*tmpsrc>>0)&0xff;
					tmpsrc++;
	 
					red = (DWORD)((red)*shade)&0xff;
					blu = (DWORD)((blu)*shade)&0xff;
					grn = (DWORD)((grn)*shade)&0xff;
					
					*tmptrg++ = (red<<16)|(grn<<8)|(blu);
				}
				if (wlast) {
					red = (*tmpsrc>>16)&0xff;
					grn = (*tmpsrc>>8)&0xff;
					blu = (*tmpsrc>>0)&0xff;
					tmpsrc++;
	 
					red = (DWORD)((red)*shade)&0xff;
					blu = (DWORD)((blu)*shade)&0xff;
					grn = (DWORD)((grn)*shade)&0xff;
					
					*tmptrg = (red<<16)|(grn<<8)|(blu);
				}
			}
			else
			{
  			        tmpsrc=(DWORD*)&colortab[0];

				if (wfirst) {
				    red = (*tmpsrc>>16)&0xff;
				    grn = (*tmpsrc>>8)&0xff;
				    blu = (*tmpsrc>>0)&0xff;
				    
				    red = (DWORD)((red)*shade)&0xff;
				    blu = (DWORD)((blu)*shade)&0xff;
				    grn = (DWORD)((grn)*shade)&0xff;
			    
				    *tmptrg++ = (red<<16)|(grn<<8)|(blu);
				}
				
				for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
					
				    red = (*tmpsrc>>16)&0xff;
				    grn = (*tmpsrc>>8)&0xff;
				    blu = (*tmpsrc>>0)&0xff;
				    
				    red = (DWORD)((red)*shade)&0xff;
				    blu = (DWORD)((blu)*shade)&0xff;
				    grn = (DWORD)((grn)*shade)&0xff;
			    
				    *tmptrg++ = (red<<16)|(grn<<8)|(blu);
				}
				
				if (wlast) {
				    red = (*tmpsrc>>16)&0xff;
				    grn = (*tmpsrc>>8)&0xff;
				    blu = (*tmpsrc>>0)&0xff;
				    
				    red = (DWORD)((red)*shade)&0xff;
				    blu = (DWORD)((blu)*shade)&0xff;
				    grn = (DWORD)((grn)*shade)&0xff;
			    
				    *tmptrg = (red<<16)|(grn<<8)|(blu);
				}
			}
			
			tmptrg = NULL;
		}
		trg += (pitcht);
	}
	    
#else
	tmptrg = NULL;
	tmpsrc = NULL;
	tmpsrc2 = NULL;
	tmpsrc3 = NULL;
	    
/*	for (y = (yt); y < (yt + (height)); y++) { */
        for (y = (yys); y < (yys + (height)); y++) {

		if((y&1)==0)
		{
			/* even lines */
			if(tmptrg!=NULL)
			{
				tmpsrc2 = (DWORD*)trg;
				tmpsrc3 = (DWORD*)trg;
				
				if(tmpsrc!=NULL)
				{
					if (wfirst) {
					    red = (*tmpsrc>>16)&0xff;
					    grn = (*tmpsrc>>8)&0xff;
					    blu = (*tmpsrc>>0)&0xff;
					    tmpsrc++;
					    
					    red2 = (*tmpsrc2>>16)&0xff;
					    grn2 = (*tmpsrc2>>8)&0xff;
					    blu2 = (*tmpsrc2>>0)&0xff;
					    tmpsrc2++;
					    
					    red = (DWORD)((red2+red)*shade)&0xff;
					    blu = (DWORD)((blu2+blu)*shade)&0xff;
					    grn = (DWORD)((grn2+grn)*shade)&0xff;
					    
					    *tmptrg++ = (red<<16)|(grn<<8)|(blu);
					}
					
	/*			      	for (x = 0; x < (width); x++) { */
					for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
						
					    red = (*tmpsrc>>16)&0xff;
					    grn = (*tmpsrc>>8)&0xff;
					    blu = (*tmpsrc>>0)&0xff;
					    tmpsrc++;
					    
					    red2 = (*tmpsrc2>>16)&0xff;
					    grn2 = (*tmpsrc2>>8)&0xff;
					    blu2 = (*tmpsrc2>>0)&0xff;
					    tmpsrc2++;
					    
					    red = (DWORD)((red2+red)*shade)&0xff;
					    blu = (DWORD)((blu2+blu)*shade)&0xff;
					    grn = (DWORD)((grn2+grn)*shade)&0xff;
					    
					    *tmptrg++ = (red<<16)|(grn<<8)|(blu);
					}
					if (wlast) {
					    red = (*tmpsrc>>16)&0xff;
					    grn = (*tmpsrc>>8)&0xff;
					    blu = (*tmpsrc>>0)&0xff;
					    tmpsrc++;
					    
					    red2 = (*tmpsrc2>>16)&0xff;
					    grn2 = (*tmpsrc2>>8)&0xff;
					    blu2 = (*tmpsrc2>>0)&0xff;
					    tmpsrc2++;
					    
					    red = (DWORD)((red2+red)*shade)&0xff;
					    blu = (DWORD)((blu2+blu)*shade)&0xff;
					    grn = (DWORD)((grn2+grn)*shade)&0xff;
					    
					    *tmptrg = (red<<16)|(grn<<8)|(blu);
					}
				}
				else
				{
					if (wfirst) {
					    red2 = (*tmpsrc2>>16)&0xff;
					    grn2 = (*tmpsrc2>>8)&0xff;
					    blu2 = (*tmpsrc2>>0)&0xff;
					    tmpsrc2++;
					    
					    red = (DWORD)((red2<<1)*shade)&0xff;
					    blu = (DWORD)((blu2<<1)*shade)&0xff;
					    grn = (DWORD)((grn2<<1)*shade)&0xff;
					    
					    *tmptrg++ = (red<<16)|(grn<<8)|(blu);
					}
					
	/*			      	for (x = 0; x < (width); x++) { */
					for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
						
					    red2 = (*tmpsrc2>>16)&0xff;
					    grn2 = (*tmpsrc2>>8)&0xff;
					    blu2 = (*tmpsrc2>>0)&0xff;
					    tmpsrc2++;
					    
					    red = (DWORD)((red2<<1)*shade)&0xff;
					    blu = (DWORD)((blu2<<1)*shade)&0xff;
					    grn = (DWORD)((grn2<<1)*shade)&0xff;
					    
					    *tmptrg++ = (red<<16)|(grn<<8)|(blu);
					}
				        if (wlast) {
					    red2 = (*tmpsrc2>>16)&0xff;
					    grn2 = (*tmpsrc2>>8)&0xff;
					    blu2 = (*tmpsrc2>>0)&0xff;
					    tmpsrc2++;
					    
					    red = (DWORD)((red2<<1)*shade)&0xff;
					    blu = (DWORD)((blu2<<1)*shade)&0xff;
					    grn = (DWORD)((grn2<<1)*shade)&0xff;
					    
					    *tmptrg = (red<<16)|(grn<<8)|(blu);
					}
				}
				tmptrg = NULL;
				
			}
			else
			{
				tmpsrc = (DWORD*)trg;
			}
		}
		else
		{
			/* odd line */
			tmptrg=(DWORD*)trg; /* draw pointer */
			if(tmpsrc3)
			{
				tmpsrc=tmpsrc3;
				tmpsrc3=NULL;
			}
		}
		trg += (pitcht);
	}
	    
	/* draw last line */
	if(tmptrg!=NULL)
	{
		if(tmpsrc!=NULL)
		{
		    if (wfirst) {
			    red = (*tmpsrc>>16)&0xff;
			    grn = (*tmpsrc>>8)&0xff;
			    blu = (*tmpsrc>>0)&0xff;
			    tmpsrc++;
			    
			    red = (DWORD)((red<<1)*shade)&0xff;
			    blu = (DWORD)((blu<<1)*shade)&0xff;
			    grn = (DWORD)((grn<<1)*shade)&0xff;
		    
			    *tmptrg++ = (red<<16)|(grn<<8)|(blu);
		    }
		    for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
			    
			    red = (*tmpsrc>>16)&0xff;
			    grn = (*tmpsrc>>8)&0xff;
			    blu = (*tmpsrc>>0)&0xff;
			    tmpsrc++;
			    
			    red = (DWORD)((red<<1)*shade)&0xff;
			    blu = (DWORD)((blu<<1)*shade)&0xff;
			    grn = (DWORD)((grn<<1)*shade)&0xff;
		    
			    *tmptrg++ = (red<<16)|(grn<<8)|(blu);
		    }
		    if (wlast) {
			    red = (*tmpsrc>>16)&0xff;
			    grn = (*tmpsrc>>8)&0xff;
			    blu = (*tmpsrc>>0)&0xff;
			    tmpsrc++;
			    
			    red = (DWORD)((red<<1)*shade)&0xff;
			    blu = (DWORD)((blu<<1)*shade)&0xff;
			    grn = (DWORD)((grn<<1)*shade)&0xff;
		    
			    *tmptrg = (red<<16)|(grn<<8)|(blu);
		    }
		}
		else
		{
		    tmpsrc=(DWORD*)&colortab[0];
		    
		    if (wfirst) {
			    red = (*tmpsrc>>16)&0xff;
			    grn = (*tmpsrc>>8)&0xff;
			    blu = (*tmpsrc>>0)&0xff;
			    
			    red = (DWORD)((red<<1)*shade)&0xff;
			    blu = (DWORD)((blu<<1)*shade)&0xff;
			    grn = (DWORD)((grn<<1)*shade)&0xff;
		    
			    *tmptrg++ = (red<<16)|(grn<<8)|(blu);
		    }
		    for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
			    red = (*tmpsrc>>16)&0xff;
			    grn = (*tmpsrc>>8)&0xff;
			    blu = (*tmpsrc>>0)&0xff;
			    
			    red = (DWORD)((red<<1)*shade)&0xff;
			    blu = (DWORD)((blu<<1)*shade)&0xff;
			    grn = (DWORD)((grn<<1)*shade)&0xff;
		    
			    *tmptrg++ = (red<<16)|(grn<<8)|(blu);
		    }
		    
		    if (wlast) {
			    red = (*tmpsrc>>16)&0xff;
			    grn = (*tmpsrc>>8)&0xff;
			    blu = (*tmpsrc>>0)&0xff;
			    
			    red = (DWORD)((red<<1)*shade)&0xff;
			    blu = (DWORD)((blu<<1)*shade)&0xff;
			    grn = (DWORD)((grn<<1)*shade)&0xff;
		    
			    *tmptrg = (red<<16)|(grn<<8)|(blu);
		    }
		    
		}
	}
#endif /* SIMPLE_SCANLINES */
    }
    else
    {
	    /* no doublescan / scanlines black */
	    for (y = (yys); y < (yys + (height)); y++) {

		if(y&1)
		{
            tmptrgw = (DWORD *)trg;
            color = colortab[0];
            if (wfirst) {
                *tmptrgw++ = color;
            }
            for (x = 0; x < wstart; x++) {
                *tmptrgw++ = color;
                *tmptrgw++ = color;
            }
            for (x = 0; x < wfast; x++) {
                tmptrgw[0] = color;
                tmptrgw[1] = color;
                tmptrgw[2] = color;
                tmptrgw[3] = color;
                tmptrgw[4] = color;
                tmptrgw[5] = color;
                tmptrgw[6] = color;
                tmptrgw[7] = color;
                tmptrgw[8] = color;
                tmptrgw[9] = color;
                tmptrgw[10] = color;
                tmptrgw[11] = color;
                tmptrgw[12] = color;
                tmptrgw[13] = color;
                tmptrgw[14] = color;
                tmptrgw[15] = color;
                tmptrgw += 16;
            }
            for (x = 0; x < wend; x++) {
                *tmptrgw++ = color;
                *tmptrgw++ = color;
            }
            if (wlast) {
                *tmptrgw = color;
            }
		}
	    trg += (pitcht);
        }
	    
    }	
    
}

void render_32_2x2_pal(video_render_color_tables_t *color_tab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
                       const unsigned int doublescan)
{
    const SDWORD *cbtable = color_tab->cbtable;
    const SDWORD *crtable = color_tab->crtable;
    const SDWORD *ytablel = color_tab->ytablel;
    const SDWORD *ytableh = color_tab->ytableh;
    const DWORD *colortab = color_tab->physical_colors;
    const BYTE *tmpsrc;
    DWORD *tmptrg;
    SDWORD *lineptr0;
    SDWORD *lineptr1;
    SDWORD *line;
    SDWORD *linepre;
    unsigned int x, y, wfirst, wstart, wfast, wend, wlast, wint, yys;
    register DWORD color;
    SDWORD l, u, v;
    DWORD red, grn, blu, red2, grn2, blu2;

    src = src + pitchs * ys + xs - 2;
    trg = trg + pitcht * yt + (xt << 2);
    yys = (ys << 1) | (yt & 1);
    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        /* alignment: 8 pixels*/
        wstart = (unsigned int)(8 - ((unsigned long)trg & 7));
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + wfirst + wlast + 5;
    lineptr0 = color_tab->line_yuv_0;
    lineptr1 = color_tab->line_yuv_1;

    tmpsrc = src - (ys ? pitchs : 0);
    line = lineptr0;
    for (x = 0; x < wint; x++) {
        register DWORD cl0, cl1, cl2, cl3;

        cl0 = tmpsrc[0];
        cl1 = tmpsrc[1];
        cl2 = tmpsrc[2];
        cl3 = tmpsrc[3];
        line[0] = 0;
        line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
        line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];
        tmpsrc++;
        line += 3;
    }

    if (yys & 1) {
        line = lineptr0;
        lineptr0 = lineptr1;
        lineptr1 = line;

        tmpsrc = src;
        line = lineptr0;
        for (x = 0; x <wint; x++) {
            register DWORD cl0, cl1, cl2, cl3;

            cl0 = tmpsrc[0];
            cl1 = tmpsrc[1];
            cl2 = tmpsrc[2];
            cl3 = tmpsrc[3];
            line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
            line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                    + cbtable[cl3];
            line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                    + crtable[cl3];
            tmpsrc++;
            line += 3;
        }
    }
    line = lineptr1;

    for (y = yys; y < (yys + height); y++) {
        tmpsrc = src;
        tmptrg = (DWORD *)trg;
        if ((!(y & 1)) || doublescan) {
            if (!(y & 1)) {
                line = lineptr0;
                lineptr0 = lineptr1;
                lineptr1 = line;

                tmpsrc = src;
                line = lineptr0;
                for (x = 0; x <wint; x++) {
                    register DWORD cl0, cl1, cl2, cl3;

                    cl0 = tmpsrc[0];
                    cl1 = tmpsrc[1];
                    cl2 = tmpsrc[2];
                    cl3 = tmpsrc[3];
                    line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
                    line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                            + cbtable[cl3];
                    line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                            + crtable[cl3];
                    tmpsrc++;
                    line += 3;
                }

            }

            line = lineptr0;
            linepre = lineptr1;

            l = line[0];
            u = (line[1] + linepre[1]) >> 3;
            v = (line[2] + linepre[2]) >> 3;
            line += 3;
            linepre += 3;

            red = ((v+l) >> 8) + 256;
            blu = ((u+l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

            if (!(y & 1)) {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu];
                    *tmptrg++ = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
                }
            } else {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;

                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red_fac[red] | gamma_grn_fac[grn]
                              | gamma_blu_fac[blu];
                    *tmptrg++ = gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = gamma_red_fac[red] | gamma_grn_fac[grn]
                            | gamma_blu_fac[blu];
                }
            }
            if (y & 1)
                src += pitchs;
        } else {
            color = colortab[0];
            if (wfirst) {
                *tmptrg++ = color;
            }
            for (x = 0; x < wstart; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            for (x = 0; x < wfast; x++) {
                tmptrg[0] = color;
                tmptrg[1] = color;
                tmptrg[2] = color;
                tmptrg[3] = color;
                tmptrg[4] = color;
                tmptrg[5] = color;
                tmptrg[6] = color;
                tmptrg[7] = color;
                tmptrg[8] = color;
                tmptrg[9] = color;
                tmptrg[10] = color;
                tmptrg[11] = color;
                tmptrg[12] = color;
                tmptrg[13] = color;
                tmptrg[14] = color;
                tmptrg[15] = color;
                tmptrg += 16;
            }
            for (x = 0; x < wend; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            if (wlast) {
                *tmptrg = color;
            }
            if (y & 1)
                src += pitchs;
        }
        trg += pitcht;
    }
}


void render_32_2x2_pal_new(video_render_color_tables_t *color_tab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
                       const unsigned int doublescan)
{
    const SDWORD *cbtable = color_tab->cbtable;
    const SDWORD *crtable = color_tab->crtable;
    const SDWORD *ytablel = color_tab->ytablel;
    const SDWORD *ytableh = color_tab->ytableh;
    const BYTE *tmpsrc;
    DWORD *tmptrg;
    SDWORD *lineptr0;
    SDWORD *lineptr1;
    SDWORD *line;
    SDWORD *linepre;
    unsigned int x, y, wfirst, wstart, wfast, wend, wlast, wint, yys;
    float off;
    SDWORD l, u, v;
    DWORD red, grn, blu, red2, grn2, blu2;

    src = src + pitchs * ys + xs - 2;
    trg = trg + pitcht * yt + (xt << 2);
    yys = (ys << 1) | (yt & 1);
    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        /* alignment: 8 pixels*/
        wstart = (unsigned int)(8 - ((unsigned long)trg & 7));
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + wfirst + wlast + 5;
    lineptr0 = color_tab->line_yuv_0;
    lineptr1 = color_tab->line_yuv_1;

    /* get previous line into buffer */
    tmpsrc = src - (ys ? pitchs : 0);
    line = lineptr0;
    
    if((yys&2)==2)
    {
	    cbtable = color_tab->cbtable;
	    crtable = color_tab->crtable;
    }
    else
    {
	    cbtable = color_tab->cbtable_odd;
	    crtable = color_tab->crtable_odd;
    }
    
    for (x = 0; x < wint; x++) {
        register DWORD cl0, cl1, cl2, cl3;

        cl0 = tmpsrc[0];
        cl1 = tmpsrc[1];
        cl2 = tmpsrc[2];
        cl3 = tmpsrc[3];
        line[0] = 0;
        line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
        line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];
        tmpsrc++;
        line += 3;
    }

    if (yys & 1) {
        line = lineptr0;
        lineptr0 = lineptr1;
        lineptr1 = line;

        tmpsrc = src;
        line = lineptr0;
	
	if((yys&2)==0)
	{
	    cbtable = color_tab->cbtable;
	    crtable = color_tab->crtable;
	}
	else
	{
	    cbtable = color_tab->cbtable_odd;
	    crtable = color_tab->crtable_odd;
	}
	
        for (x = 0; x <wint; x++) {
            register DWORD cl0, cl1, cl2, cl3;

            cl0 = tmpsrc[0];
            cl1 = tmpsrc[1];
            cl2 = tmpsrc[2];
            cl3 = tmpsrc[3];
            line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
            line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                    + cbtable[cl3];
            line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                    + crtable[cl3];
            tmpsrc++;
            line += 3;
        }
    }
    line = lineptr1;

    off=(((float)(video_resources.pal_oddlines_offset)) * (1.5f / 2000.0f))-((1.5f/2.0f)-1.0f);
    
    for (y = yys; y < (yys + height); y+=2) {
        tmpsrc = src;
        tmptrg = (DWORD *)trg;
	
	if(!(y&2)) { /* even sourceline */
		
		/* swap the line buffers */
                line = lineptr0;
                lineptr0 = lineptr1;
                lineptr1 = line;

                tmpsrc = src;
                line = lineptr0;
		
		cbtable = color_tab->cbtable;
		crtable = color_tab->crtable;
		ytablel = color_tab->ytablel;
		ytableh = color_tab->ytableh;
		
                for (x = 0; x <wint; x++) {
                    register DWORD cl0, cl1, cl2, cl3;

                    cl0 = tmpsrc[0];
                    cl1 = tmpsrc[1];
                    cl2 = tmpsrc[2];
                    cl3 = tmpsrc[3];
                    line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
                    line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                            + cbtable[cl3];
                    line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                            + crtable[cl3];
                    tmpsrc++;
                    line += 3;
                }

            line = lineptr0;
            linepre = lineptr1;

            l = line[0];
		/* current line even (u) , previous line odd (-u) */
		u = (line[1] - linepre[1]) >> 3;
		v = (line[2] - linepre[2]) >> 3;
	    
            line += 3;
            linepre += 3;

            red = ((v+l) >> 8) + 256;
            blu = ((u+l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

		/* actual line */
                if (wfirst) {
                    l = line[0];
		    
		    /* current line even (u) , previous line odd (-u) */
		    u = (line[1] - linepre[1]) >> 3;
		    v = (line[2] - linepre[2]) >> 3;
		    
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
		    
			/* current line even (u) , previous line odd (-u) */
			u = (line[1] - linepre[1]) >> 3;
			v = (line[2] - linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu];
                    *tmptrg++ = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
                }
		
	} else { /* odd sourceline */
		
		/* swap the line buffers */
                line = lineptr0;
                lineptr0 = lineptr1;
                lineptr1 = line;

                tmpsrc = src;
                line = lineptr0;
		
		cbtable = color_tab->cbtable_odd;
		crtable = color_tab->crtable_odd;
		ytablel = color_tab->ytablel;
		ytableh = color_tab->ytableh;
		    
                for (x = 0; x <wint; x++) {
                    register DWORD cl0, cl1, cl2, cl3;

                    cl0 = tmpsrc[0];
                    cl1 = tmpsrc[1];
                    cl2 = tmpsrc[2];
                    cl3 = tmpsrc[3];
		    
		    
                    line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
                    line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                            + cbtable[cl3];
                    line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                            + crtable[cl3];
		    
                    tmpsrc++;
                    line += 3;
                }
		
            line = lineptr0;
            linepre = lineptr1;

            l = line[0];
	    /* current line odd (-u) , previous line even (u) */
	    u = (linepre[1] - line[1]) >> 3;
	    v = (linepre[2] - line[2]) >> 3;
	    u = (int)((float)u * off);
	    v = (int)((float)v * off);
	    
            line += 3;
            linepre += 3;

            red = ((v+l) >> 8) + 256;
            blu = ((u+l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;
		/* actual line */
                if (wfirst) {
                    l = line[0];
			    /* current line odd (-u) , previous line even (u) */
			    u = (linepre[1] - line[1]) >> 3;
			    v = (linepre[2] - line[2]) >> 3;
			u = (int)((float)u * off);
			v = (int)((float)v * off);
		    
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;

                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
		    
		        /* current line odd (-u) , previous line even (u) */
		        u = (linepre[1] - line[1]) >> 3;
		        v = (linepre[2] - line[2]) >> 3;
			u = (int)((float)u * off);
			v = (int)((float)v * off);
			    
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu];
                    *tmptrg++ = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
            }
            }
	
                src += pitchs;
        trg += (pitcht*2);
    }
}

/*
	render shaded scanlines in 24bit mode
	
	the "simple scanlines" filter uses only the previous scanline to calculate the color of
	the "empty" scanlines. 

        the other filter is more correct in that it takes the previous and next scanline and 
	interpolates between them. this however doesnt work correctly (yet?) - need to investigate
	the whacked up video caching some more.
	
	note that renderers which call this one afterwards do not have to draw the black odd lines
	by themselves.
*/
#define SIMPLE_SCANLINES_24
void render_24_2x2_pal_scanlines(video_render_color_tables_t *color_tab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
                       const unsigned int doublescan)
{
    const DWORD *colortab = color_tab->physical_colors;
    register BYTE color_r,color_g,color_b;

    unsigned int x, y, wfirst, wstart, wfast, wend, wlast, wint, yys;

    BYTE *tmptrgw;
    BYTE *tmptrg,*tmpsrc;
#ifdef SIMPLE_SCANLINES_24
    float shade=((float)(video_resources.pal_scanlineshade)) / 1000.0f;
#else
    BYTE *tmpsrc2,*tmpsrc3;
    DWORD red, grn, blu;
    DWORD  red2, grn2, blu2;
    float shade=((float)(video_resources.pal_scanlineshade)) / 2000.0f;
#endif

    trg = trg + pitcht * yt + (xt *3);
    yys = (ys << 1) | (yt & 1);

    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        /* alignment: 8 pixels*/
        wstart = (unsigned int)(8 - ((unsigned long)trg & 7));
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + wfirst + wlast + 5;
    
    /* loop over all lines */
    if(doublescan)
    {
	tmpsrc = NULL;
	tmptrg = NULL;
	
#ifdef SIMPLE_SCANLINES_24
/*	for (y = (yt); y < (yt + (height)); y++) { */
        for (y = (yys); y < (yys + (height)); y++) {
		if((y&1)==0)
		{
			tmpsrc=(BYTE*)trg;
		}
		else
		{
			tmptrg=(BYTE*)trg; /* draw pointer */
			
			/* draw a line */
			
			if(tmpsrc!=NULL)
			{
				if (wfirst) {
					
					*tmptrg++ = (BYTE)((*tmpsrc++)*shade)&0xff;
					*tmptrg++ = (BYTE)((*tmpsrc++)*shade)&0xff;
					*tmptrg++ = (BYTE)((*tmpsrc++)*shade)&0xff;
				}
				
				for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
					
					*tmptrg++ = (BYTE)((*tmpsrc++)*shade)&0xff;
					*tmptrg++ = (BYTE)((*tmpsrc++)*shade)&0xff;
					*tmptrg++ = (BYTE)((*tmpsrc++)*shade)&0xff;
				}
				if (wlast) {
					
					*tmptrg++ = (BYTE)((*tmpsrc++)*shade)&0xff;
					*tmptrg++ = (BYTE)((*tmpsrc++)*shade)&0xff;
					*tmptrg++ = (BYTE)((*tmpsrc++)*shade)&0xff;
				}
			}
			else
			{
				    color_r = (BYTE)(colortab[0]>>0)&0xff;
				    color_g = (BYTE)(colortab[0]>>8)&0xff;
				    color_b = (BYTE)(colortab[0]>>16)&0xff;

				if (wfirst) {
			    
					*tmptrg++ = (BYTE)((color_r)*shade)&0xff;
					*tmptrg++ = (BYTE)((color_g)*shade)&0xff;
					*tmptrg++ = (BYTE)((color_b)*shade)&0xff;
				}
				
				for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
					
					*tmptrg++ = (BYTE)((color_r)*shade)&0xff;
					*tmptrg++ = (BYTE)((color_g)*shade)&0xff;
					*tmptrg++ = (BYTE)((color_b)*shade)&0xff;
				}
				
				if (wlast) {
			    
					*tmptrg++ = (BYTE)((color_r)*shade)&0xff;
					*tmptrg++ = (BYTE)((color_g)*shade)&0xff;
					*tmptrg++ = (BYTE)((color_b)*shade)&0xff;
				}
			}
			
			tmptrg = NULL;
		}
		trg += (pitcht);
	}
	    
#else
	tmptrg = NULL;
	tmpsrc = NULL;
	tmpsrc2 = NULL;
	tmpsrc3 = NULL;
	    
/*	for (y = (yt); y < (yt + (height)); y++) { */
        for (y = (yys); y < (yys + (height)); y++) {

		if((y&1)==0)
		{
			/* even lines */
			if(tmptrg!=NULL)
			{
				tmpsrc2 = (BYTE*)trg;
				tmpsrc3 = (BYTE*)trg;
				
				if(tmpsrc!=NULL)
				{
					if (wfirst) {
					    red = (*tmpsrc++)&0xff;
					    grn = (*tmpsrc++)&0xff;
					    blu = (*tmpsrc++)&0xff;
					    
					    red2 = (*tmpsrc2++)&0xff;
					    grn2 = (*tmpsrc2++)&0xff;
					    blu2 = (*tmpsrc2++)&0xff;
					    
					    *tmptrg++ = (BYTE)((red2+red)*shade)&0xff;
					    *tmptrg++ = (BYTE)((grn2+grn)*shade)&0xff;
					    *tmptrg++ = (BYTE)((blu2+blu)*shade)&0xff;
					    
					}
					
	/*			      	for (x = 0; x < (width); x++) { */
					for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
						
					    red = (*tmpsrc++)&0xff;
					    grn = (*tmpsrc++)&0xff;
					    blu = (*tmpsrc++)&0xff;
					    
					    red2 = (*tmpsrc2++)&0xff;
					    grn2 = (*tmpsrc2++)&0xff;
					    blu2 = (*tmpsrc2++)&0xff;
					    
					    *tmptrg++ = (BYTE)((red2+red)*shade)&0xff;
					    *tmptrg++ = (BYTE)((grn2+grn)*shade)&0xff;
					    *tmptrg++ = (BYTE)((blu2+blu)*shade)&0xff;
					}
					if (wlast) {
					    red = (*tmpsrc++)&0xff;
					    grn = (*tmpsrc++)&0xff;
					    blu = (*tmpsrc++)&0xff;
					    
					    red2 = (*tmpsrc2++)&0xff;
					    grn2 = (*tmpsrc2++)&0xff;
					    blu2 = (*tmpsrc2++)&0xff;
					    
					    *tmptrg++ = (BYTE)((red2+red)*shade)&0xff;
					    *tmptrg++ = (BYTE)((grn2+grn)*shade)&0xff;
					    *tmptrg++ = (BYTE)((blu2+blu)*shade)&0xff;
					}
				}
				else
				{
					if (wfirst) {
					    red2 = (*tmpsrc2++)&0xff;
					    grn2 = (*tmpsrc2++)&0xff;
					    blu2 = (*tmpsrc2++)&0xff;
					    
					    *tmptrg++ = (BYTE)((red2<<1)*shade)&0xff;
					    *tmptrg++ = (BYTE)((grn2<<1)*shade)&0xff;
					    *tmptrg++ = (BYTE)((blu2<<1)*shade)&0xff;
					    
					}
					
	/*			      	for (x = 0; x < (width); x++) { */
					for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
						
					    red2 = (*tmpsrc2++)&0xff;
					    grn2 = (*tmpsrc2++)&0xff;
					    blu2 = (*tmpsrc2++)&0xff;
					    
					    *tmptrg++ = (BYTE)((red2<<1)*shade)&0xff;
					    *tmptrg++ = (BYTE)((grn2<<1)*shade)&0xff;
					    *tmptrg++ = (BYTE)((blu2<<1)*shade)&0xff;
					}
				        if (wlast) {
					    red2 = (*tmpsrc2++)&0xff;
					    grn2 = (*tmpsrc2++)&0xff;
					    blu2 = (*tmpsrc2++)&0xff;
					    
					    *tmptrg++ = (BYTE)((red2<<1)*shade)&0xff;
					    *tmptrg++ = (BYTE)((grn2<<1)*shade)&0xff;
					    *tmptrg++ = (BYTE)((blu2<<1)*shade)&0xff;
					}
				}
				tmptrg = NULL;
				
			}
			else
			{
				tmpsrc = (BYTE*)trg;
			}
		}
		else
		{
			/* odd line */
			tmptrg=(BYTE*)trg; /* draw pointer */
			if(tmpsrc3)
			{
				tmpsrc=tmpsrc3;
				tmpsrc3=NULL;
			}
		}
		trg += (pitcht);
	}
	    
	/* draw last line */
	if(tmptrg!=NULL)
	{
		if(tmpsrc!=NULL)
		{
		    if (wfirst) {
			    red = (*tmpsrc++)&0xff;
			    grn = (*tmpsrc++)&0xff;
			    blu = (*tmpsrc++)&0xff;
			    
			    *tmptrg++ = (BYTE)((red<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((grn<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((blu<<1)*shade)&0xff;
		    }
		    for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
			    
			    red = (*tmpsrc++)&0xff;
			    grn = (*tmpsrc++)&0xff;
			    blu = (*tmpsrc++)&0xff;
			    
			    *tmptrg++ = (BYTE)((red<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((grn<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((blu<<1)*shade)&0xff;
		    }
		    if (wlast) {
			    red = (*tmpsrc++)&0xff;
			    grn = (*tmpsrc++)&0xff;
			    blu = (*tmpsrc++)&0xff;
			    
			    *tmptrg++ = (BYTE)((red<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((grn<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((blu<<1)*shade)&0xff;
		    }
		}
		else
		{
		    tmpsrc=(BYTE*)&colortab[0];
		    
		    if (wfirst) {
			    red = (*tmpsrc++)&0xff;
			    grn = (*tmpsrc++)&0xff;
			    blu = (*tmpsrc++)&0xff;
			    
			    *tmptrg++ = (BYTE)((red<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((grn<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((blu<<1)*shade)&0xff;
		    }
		    for (x = 0; x < ((wfast << 3) + wstart + wend)<<1; x++) {
			    red = (*tmpsrc++)&0xff;
			    grn = (*tmpsrc++)&0xff;
			    blu = (*tmpsrc++)&0xff;
			    
			    *tmptrg++ = (BYTE)((red<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((grn<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((blu<<1)*shade)&0xff;
		    }
		    
		    if (wlast) {
			    red = (*tmpsrc++)&0xff;
			    grn = (*tmpsrc++)&0xff;
			    blu = (*tmpsrc++)&0xff;
			    
			    *tmptrg++ = (BYTE)((red<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((grn<<1)*shade)&0xff;
			    *tmptrg++ = (BYTE)((blu<<1)*shade)&0xff;
		    }
		    
		}
	}
#endif /* SIMPLE_SCANLINES */
    }
    else
    {
	    /* no doublescan / scanlines black */
	    for (y = (yys); y < (yys + (height)); y++) {

		if(y&1)
		{
            tmptrgw = (BYTE *)trg;
            color_r = (BYTE)(colortab[0]>>0)&0xff;
            color_g = (BYTE)(colortab[0]>>8)&0xff;
            color_b = (BYTE)(colortab[0]>>16)&0xff;
	    
            if (wfirst) {
                *tmptrgw++ = color_r;
                *tmptrgw++ = color_g;
                *tmptrgw++ = color_b;
            }
            for (x = 0; x < wstart; x++) {
                *tmptrgw++ = color_r;
                *tmptrgw++ = color_g;
                *tmptrgw++ = color_b;
		
                *tmptrgw++ = color_r;
                *tmptrgw++ = color_g;
                *tmptrgw++ = color_b;
            }
            for (x = 0; x < wfast; x++) {
                tmptrgw[0] = color_r;
                tmptrgw[1] = color_g;
                tmptrgw[2] = color_b;
                tmptrgw[3] = color_r;
                tmptrgw[4] = color_g;
                tmptrgw[5] = color_b;
                tmptrgw[6] = color_r;
                tmptrgw[7] = color_g;
                tmptrgw[8] = color_b;
                tmptrgw[9] = color_r;
                tmptrgw[10] = color_g;
                tmptrgw[11] = color_b;
                tmptrgw[12] = color_r;
                tmptrgw[13] = color_g;
                tmptrgw[14] = color_b;
                tmptrgw[15] = color_r;
                tmptrgw[16] = color_g;
                tmptrgw[17] = color_b;
                tmptrgw[18] = color_r;
                tmptrgw[19] = color_g;
                tmptrgw[20] = color_b;
                tmptrgw[21] = color_r;
                tmptrgw[22] = color_g;
                tmptrgw[23] = color_b;
                tmptrgw[24] = color_r;
                tmptrgw[25] = color_g;
                tmptrgw[26] = color_b;
                tmptrgw[27] = color_r;
                tmptrgw[28] = color_g;
                tmptrgw[29] = color_b;
                tmptrgw[30] = color_r;
                tmptrgw[31] = color_g;
                tmptrgw[32] = color_b;
                tmptrgw[33] = color_r;
                tmptrgw[34] = color_g;
                tmptrgw[35] = color_b;
                tmptrgw[36] = color_r;
                tmptrgw[37] = color_g;
                tmptrgw[38] = color_b;
                tmptrgw[39] = color_r;
                tmptrgw[40] = color_g;
                tmptrgw[41] = color_b;
                tmptrgw[42] = color_r;
                tmptrgw[43] = color_g;
                tmptrgw[44] = color_b;
                tmptrgw[45] = color_r;
                tmptrgw[46] = color_g;
                tmptrgw[47] = color_b;
                tmptrgw += 16*3;
            }
            for (x = 0; x < wend; x++) {
                *tmptrgw++ = color_r;
                *tmptrgw++ = color_g;
                *tmptrgw++ = color_b;
		
                *tmptrgw++ = color_r;
                *tmptrgw++ = color_g;
                *tmptrgw++ = color_b;
            }
            if (wlast) {
                *tmptrgw++ = color_r;
                *tmptrgw++ = color_g;
                *tmptrgw++ = color_b;
            }
		}
	    trg += (pitcht);
        }
	    
    }	
    
}

void render_24_2x2_pal_new(video_render_color_tables_t *color_tab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
                       const unsigned int doublescan)
{
    const SDWORD *cbtable = color_tab->cbtable;
    const SDWORD *crtable = color_tab->crtable;
    const SDWORD *ytablel = color_tab->ytablel;
    const SDWORD *ytableh = color_tab->ytableh;
    const BYTE *tmpsrc;
    BYTE *tmptrg;
    DWORD tmpcol;
    
    SDWORD *lineptr0;
    SDWORD *lineptr1;
    SDWORD *line;
    SDWORD *linepre;
    unsigned int x, y, wfirst, wstart, wfast, wend, wlast, wint, yys;
    float off;
    SDWORD l, u, v;
    DWORD red, grn, blu, red2, grn2, blu2;

    src = src + pitchs * ys + xs - 2;
    trg = trg + pitcht * yt + (xt * 3);
    yys = (ys << 1) | (yt & 1);
    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        /* alignment: 8 pixels*/
        wstart = (unsigned int)(8 - ((unsigned long)trg & 7));
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + wfirst + wlast + 5;
    lineptr0 = color_tab->line_yuv_0;
    lineptr1 = color_tab->line_yuv_1;

    /* get previous line into buffer */
    tmpsrc = src - (ys ? pitchs : 0);
    line = lineptr0;
    
    if((yys&2)==2)
    {
	    cbtable = color_tab->cbtable;
	    crtable = color_tab->crtable;
    }
    else
    {
	    cbtable = color_tab->cbtable_odd;
	    crtable = color_tab->crtable_odd;
    }
    
    for (x = 0; x < wint; x++) {
        register DWORD cl0, cl1, cl2, cl3;

        cl0 = tmpsrc[0];
        cl1 = tmpsrc[1];
        cl2 = tmpsrc[2];
        cl3 = tmpsrc[3];
        line[0] = 0;
        line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
        line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];
        tmpsrc++;
        line += 3;
    }

    if (yys & 1) {
        line = lineptr0;
        lineptr0 = lineptr1;
        lineptr1 = line;

        tmpsrc = src;
        line = lineptr0;
	
	if((yys&2)==0)
	{
	    cbtable = color_tab->cbtable;
	    crtable = color_tab->crtable;
	}
	else
	{
	    cbtable = color_tab->cbtable_odd;
	    crtable = color_tab->crtable_odd;
	}
	
        for (x = 0; x <wint; x++) {
            register DWORD cl0, cl1, cl2, cl3;

            cl0 = tmpsrc[0];
            cl1 = tmpsrc[1];
            cl2 = tmpsrc[2];
            cl3 = tmpsrc[3];
            line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
            line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                    + cbtable[cl3];
            line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                    + crtable[cl3];
            tmpsrc++;
            line += 3;
        }
    }
    line = lineptr1;

    off=(((float)(video_resources.pal_oddlines_offset)) * (1.5f / 2000.0f))-((1.5f/2.0f)-1.0f);
    
    for (y = yys; y < (yys + height); y+=2) {
        tmpsrc = src;
        tmptrg = (BYTE *)trg;
	
	if(!(y&2)) { /* even sourceline */
		
		/* swap the line buffers */
                line = lineptr0;
                lineptr0 = lineptr1;
                lineptr1 = line;

                tmpsrc = src;
                line = lineptr0;
		
		cbtable = color_tab->cbtable;
		crtable = color_tab->crtable;
		ytablel = color_tab->ytablel;
		ytableh = color_tab->ytableh;
		
                for (x = 0; x <wint; x++) {
                    register DWORD cl0, cl1, cl2, cl3;

                    cl0 = tmpsrc[0];
                    cl1 = tmpsrc[1];
                    cl2 = tmpsrc[2];
                    cl3 = tmpsrc[3];
                    line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
                    line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                            + cbtable[cl3];
                    line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                            + crtable[cl3];
                    tmpsrc++;
                    line += 3;
                }

            line = lineptr0;
            linepre = lineptr1;

            l = line[0];
		/* current line even (u) , previous line odd (-u) */
		u = (line[1] - linepre[1]) >> 3;
		v = (line[2] - linepre[2]) >> 3;
	    
            line += 3;
            linepre += 3;

            red = ((v+l) >> 8) + 256;
            blu = ((u+l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

		/* actual line */
                if (wfirst) {
                    l = line[0];
		    
		    /* current line even (u) , previous line odd (-u) */
		    u = (line[1] - linepre[1]) >> 3;
		    v = (line[2] - linepre[2]) >> 3;
		    
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    tmpcol   = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];
		    *tmptrg++ = (BYTE)(tmpcol>>0) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>8) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>16) & 0xff;

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
		    
			/* current line even (u) , previous line odd (-u) */
			u = (line[1] - linepre[1]) >> 3;
			v = (line[2] - linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    tmpcol = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
		    *tmptrg++ = (BYTE)(tmpcol>>0) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>8) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>16) & 0xff;
		    
                    tmpcol   = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];
		    *tmptrg++ = (BYTE)(tmpcol>>0) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>8) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>16) & 0xff;

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    tmpcol = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
		    *tmptrg++ = (BYTE)(tmpcol>>0) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>8) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>16) & 0xff;
                }
		
	} else { /* odd sourceline */
		
		/* swap the line buffers */
                line = lineptr0;
                lineptr0 = lineptr1;
                lineptr1 = line;

                tmpsrc = src;
                line = lineptr0;
		
		cbtable = color_tab->cbtable_odd;
		crtable = color_tab->crtable_odd;
		ytablel = color_tab->ytablel;
		ytableh = color_tab->ytableh;
		    
                for (x = 0; x <wint; x++) {
                    register DWORD cl0, cl1, cl2, cl3;

                    cl0 = tmpsrc[0];
                    cl1 = tmpsrc[1];
                    cl2 = tmpsrc[2];
                    cl3 = tmpsrc[3];
		    
		    
                    line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
                    line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                            + cbtable[cl3];
                    line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                            + crtable[cl3];
		    
                    tmpsrc++;
                    line += 3;
                }
		
            line = lineptr0;
            linepre = lineptr1;

            l = line[0];
	    /* current line odd (-u) , previous line even (u) */
	    u = (linepre[1] - line[1]) >> 3;
	    v = (linepre[2] - line[2]) >> 3;
	    u = (int)((float)u * off);
	    v = (int)((float)v * off);
	    
            line += 3;
            linepre += 3;

            red = ((v+l) >> 8) + 256;
            blu = ((u+l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;
		/* actual line */
                if (wfirst) {
                    l = line[0];
			    /* current line odd (-u) , previous line even (u) */
			    u = (linepre[1] - line[1]) >> 3;
			    v = (linepre[2] - line[2]) >> 3;
			u = (int)((float)u * off);
			v = (int)((float)v * off);
		    
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;

                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    tmpcol   = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];
		    *tmptrg++ = (BYTE)(tmpcol>>0) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>8) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>16) & 0xff;

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
		    
		        /* current line odd (-u) , previous line even (u) */
		        u = (linepre[1] - line[1]) >> 3;
		        v = (linepre[2] - line[2]) >> 3;
			u = (int)((float)u * off);
			v = (int)((float)v * off);
			    
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    tmpcol = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
		    *tmptrg++ = (BYTE)(tmpcol>>0) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>8) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>16) & 0xff;
		    
                    tmpcol   = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];
		    *tmptrg++ = (BYTE)(tmpcol>>0) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>8) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>16) & 0xff;

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
		    
                    tmpcol = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
		    *tmptrg++ = (BYTE)(tmpcol>>0) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>8) & 0xff;
		    *tmptrg++ = (BYTE)(tmpcol>>16) & 0xff;
		    
            }
            }
	
                src += pitchs;
        trg += (pitcht*2);
    }
}

