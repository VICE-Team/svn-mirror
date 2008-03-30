/*
 * crtc-mem.c - A line-based CRTC emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *
 * 16/24bpp support added by
 *  Steven Tieu (stieu@physics.ubc.ca)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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


/* -------------------------------------------------------------------------- */

/* CRTC interface functions.
   FIXME: Several registers are not implemented.  */

void REGPARM2 store_crtc(ADDRESS addr, BYTE value)
{
    crtc[addr] = value;

    switch (addr) {
      case 0:			/* R00  Horizontal total (characters + 1) */
	new_crtc_cycles_per_line = crtc[0] + 1;

      case 1:			/* R01  Horizontal characters displayed */
	if (!crtc[1])
	    return;
	new_memptr_inc = crtc[1];
        if (hw_double_cols) {
            new_memptr_inc *= 2;
        }
	/* catch screens to large for our text cache */
	new_memptr_inc = crtc_min( SCREEN_MAX_TEXTCOLS, new_memptr_inc );
        break;

      case 2:			/* R02  Horizontal Sync Position */
	break;

      case 3:			/* R03  Horizontal/Vertical Sync widths */
	break;

      case 7:			/* R07  Vertical sync position */
	break;

      case 8:			/* R08  unused: Interlace and Skew */
	break;

      case 6:			/* R06  Number of display lines on screen */
        new_crtc_screen_textlines = crtc[6] & 0x7f;
        break;

      case 9:			/* R09  Rasters between two display lines */
	new_screen_charheight = crtc_min(16, crtc[9] + 1);
	break;

      case 4:			/* R04  Vertical total (character) rows */
        new_crtc_vertical_total = crtc[4] + 1;
	break;

      case 5:			/* R05  Vertical total line adjust */
        new_crtc_vertical_adjust = crtc[5];
	break;

      case 10:			/* R10  Cursor (not implemented on the PET) */
        crsrstart = value & 0x1f;
        value = ((value >> 5) & 0x03) ^ 0x01;
        if (crsr_enable && (crsrmode != value)) {
          crsrmode = value;
          crsrstate = 1;
          crsrcnt = 16;
	  crsr_set_dirty();
        }
	break;

      case 11:			/* R11  Cursor (not implemented on the PET) */
	crsr_set_dirty();
        crsrend = value & 0x1f;
	crsr_set_dirty();
	break;

      case 12:			/* R12  Control register */

        /* This is actually the upper 6 video RAM address bits.
	 * But CBM decided that the two uppermost bits should be used
	 * for control. 
	 * The usage here is from the 8032 schematics on funet.
	 *
	 * Bit 0: 1=add 256 to screen start address ( 512 for 80-columns)
	 * Bit 1: 1=add 512 to screen start address (1024 for 80-columns)
	 * Bit 2: no connection
	 * Bit 3: no connection
	 * Bit 4: invert video signal
	 * Bit 5: use top half of 4K character generator
	 * Bit 6: (no pin on the CRTC, video address is 14 bit only)
	 * Bit 7: (no pin on the CRTC, video address is 14 bit only)
	 */

	crsr_set_dirty();
        crtc_update_memory_ptrs();
        scrpos = ((scrpos & 0x00ff) | ((value << 8) & 0x3f00)) & addr_mask;
        crsrrel = crsrpos - scrpos;
	crsr_set_dirty();
	break;

      case 13:			/* R13  Address of first character */
	/* Value + 32786 (8000) */
	crsr_set_dirty();
	crtc_update_memory_ptrs();
        scrpos = ((scrpos & 0x3f00) | (value & 0xff)) & addr_mask;
        crsrrel = crsrpos - scrpos;
	crsr_set_dirty();
	break;

      case 14:
	crsr_set_dirty();
        crsrpos = ((crsrpos & 0x00ff) | ((value << 8) & 0x3f00)) & addr_mask;
        crsrrel = crsrpos - scrpos;
	crsr_set_dirty();
	break;

      case 15:			/* R14-5 Cursor location HI/LO -- unused */
	crsr_set_dirty();
        crsrpos = ((crsrpos & 0x3f00) | (value & 0xff)) & addr_mask;
        crsrrel = crsrpos - scrpos;
	crsr_set_dirty();
	break;

      case 16:
      case 17:			/* R16-7 Light Pen HI/LO -- read only */
	break;

      case 18:
      case 19:			/* R18-9 Update address HI/LO (only 6545)  */
	break;
    }
}

BYTE REGPARM1 read_crtc(ADDRESS addr)
{
    switch (addr) {
      case 14:
      case 15:			/* Cursor location HI/LO */
	return crtc[addr];

      case 16:
      case 17:			/* Light Pen X,Y */
	return 0xff;

      default:
	return 0;		/* All the rest are write-only registers */
    }
}

BYTE REGPARM1 peek_crtc(ADDRESS addr)
{
    return read_crtc(addr);
}

#if 0
void store_colorram(ADDRESS addr, BYTE value)
{
    /* No color RAM. */
}

BYTE read_colorram(ADDRESS addr)
{
    /* Bogus. */
    return 0;
}
#endif

void crtc_set_char(int crom)
{
    chargen_rel = (chargen_rel & ~0x800) | (crom ? 0x800 : 0);
    /* chargen_rel is computed for 8bytes/char, but charom is 16bytes/char */
    chargen_ptr = chargen_rom + (chargen_rel << 1);
}


static void crtc_update_memory_ptrs(void)
{
    scraddr = crtc[13] + ((crtc[12] & 0x3f) << 8);

    /* depends on machine */
    do_update_memory_ptrs();

    /* chargen_rel is computed for 8bytes/char, but charom is 16bytes/char */
    chargen_ptr = chargen_rom + (chargen_rel << 1);

    scraddr &= addr_mask;
}

