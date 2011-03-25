#ifndef __render1x1_dingoo_h__
#define __render1x1_dingoo_h__
static int init_color_tab = 1;

static DWORD colors[65536];

static void init_render_16_1x1_04(const video_render_color_tables_t *color_tab)
{
	const DWORD *colortab = color_tab->physical_colors;
	
	int i,j;
	for(i=0; i < 256; ++i)
	{
		for(j=0; j < 256; ++j)
		{
			colors[(i<<8)+j] = ((WORD)(colortab[i]) << 16) + ((WORD)colortab[j]);
		}
	}
	init_color_tab = 0;	
}

static void render_16_1x1_04_dingoo(const video_render_color_tables_t *color_tab, const BYTE *src, BYTE *trg,
                      unsigned int width, const unsigned int height,
                      unsigned int xs, const unsigned int ys,
                      unsigned int xt, const unsigned int yt,
                      const unsigned int pitchs, const unsigned int pitcht)
{
    const BYTE *tmpsrc;
    DWORD *tmptrg_d;
    unsigned int x, y, wstart, wfast, wend;

    if(xs & 1)
    {
    	xs &= 0xfffffffe;
    	xt &= 0xfffffffe;
	width++;
    }
    if(width&1)
    	++width;

    if(init_color_tab)
    	init_render_16_1x1_04(color_tab);
	
    src = src + pitchs * ys + xs;
    trg = trg + pitcht * yt + (xt << 1);
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        /* alignment: 8 pixels*/
        wstart = (unsigned int)(8 - (vice_ptr_to_uint(trg) & 7));
        wfast = (width - wstart) >> 3;  /* fast loop for 8 pixel segments*/
        wend = ((width - wstart) & 0x07); /* do not forget the rest*/
    }
    for (y = 0; y < height; y++) {
        tmpsrc = src;
	tmptrg_d = (DWORD *)trg;
        for (x = 0; x < wstart; x+=2) {
                *tmptrg_d++ = colors[*((WORD *)tmpsrc)];
		tmpsrc+=2;
        }
        for (x = 0; x< wfast; x++) {
	    tmptrg_d[0] = colors[((WORD *)tmpsrc)[0]];
	    tmptrg_d[1] = colors[((WORD *)tmpsrc)[1]];
	    tmptrg_d[2] = colors[((WORD *)tmpsrc)[2]];
	    tmptrg_d[3] = colors[((WORD *)tmpsrc)[3]];
            tmpsrc += 8;
            tmptrg_d += 4;
        }

        for (x = 0; x < wend; x+=2)
        {
                *tmptrg_d++ = colors[*((WORD *)tmpsrc)];
		tmpsrc+=2;
        }
        src += pitchs;
        trg += pitcht;
    }
}
#endif

