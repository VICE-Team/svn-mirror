/*
 * $Id: printer.h,v 1.3 1996/04/01 09:01:41 jopi Exp $
 *
 * Commodore compatible Matrix Printer Emulator.
 * See README for copyright notice.
 *
 * Restricted Epson compatible Serial printer emulator.
 *
 * Written by
 *   Jouko Valta  (jopi@stekt.oulu.fi)
 *
 *
 * $Log: printer.h,v $
 * Revision 1.3  1996/04/01 09:01:41  jopi
 * Text formatting and PostScript support decrarations.
 *
 * Revision 1.2  1995/11/07  16:51:00  jopi
 * printer font image structure
 * graphics support
 * control value declarations
 *
 * Revision 1.1  1995/04/01  07:50:52  jopi
 * Initial revision
 *
 *
 *
 */

#ifndef _CBM_PRINTER_H_
#define _CBM_PRINTER_H_


/* Output File Formats */

#define FT_RAW		0
#define FT_HEXDUMP	1
#define FT_PETSCII	2
#define FT_ASCII	3
#define FT_LATIN_1	4
#define FT_PS		5
#define FT_IMAGE	8	/* Flag */
#define FT_XBM		8


/* Printer Mode */

#define T_ASCIIMODE	0x10
#define T_GRAPHIC	0x8	/* data on flow */
#define T_CLEAN		0x2


/* State */

#define UPPERCASE	0x91	/* boundary is 0x80 */
#define LOWCASE		0x11

#define T_QUOTE		0x80

#define T_OLUPPC	0x8	/* one line */
#define T_OLLOWC	0x4	/* one line */
#define T_UPPC		0x2
#define T_LOWC		0x1

#define T_CASE		(T_OLUPPC | T_OLLOWC | T_UPPC | T_LOWC)


/* PS States */

#define PS_BLANK	0
#define PS_PROLOG	1	/* flags */
#define PS_PAGEHDR	2	/* formfeed if this is missing */
#define PS_SHOW		4
#define PS_DRAW		8	/* 7-bit graphics command underway */


/* Colours */

#define PC_BLACK	0
#define PC_RED		1
#define PC_BLUE		2
#define PC_VIOLET	3
#define PC_YELLOW	4
#define PC_ORANGE	5
#define PC_GREEN	6
#define P_MAX_COLOUR	PC_GREEN


#define MAX_HTABS	40
#define GR_BUFSIZE	960


typedef struct {
    int    type;

    int    of_format;
    int    ps_state;		/* PS generator mode */
    int    page_h;		/* PS page length */
    int    page_w;		/* PS page width */

    FILE  *FileDs;
    char   ActiveName[256];
    char  *lpCommand;
    char  *lpName;

    /* Printer */

    int    mode;		/* operating mode */
    int    colour;
    int    lang;

    int    graphics;		/* graphics mode  */
    int    gr_pitch;		/* graphics pitch */
    short  gr_cnt;		/* gr_buf counter */
    BYTE   gr_buf[GR_BUFSIZE];	/* graphics image data */

    /* Text processing */

    BYTE  *inbuf;
    short  inptr;

    int    state;		/* uppercase */
    int    reverse;

    int    size;
    int    pitch;	/* text pitch */
    int    lf_advance;	/* pixels (1/216 inch (1 pin = 3/216 = 1/72)) */

    /* Page */

    int    mgn_top;
    int    mgn_bottom;
    int    mgn_left;
    int    mgn_right;
    int    line;	/* pixels (1/216 inch) */
    int    column;
} PRINTER;


/*
 * Commodore MPS 803 Business Mode character definition structure.
 */

typedef struct {
    char descender;
    char data[7];
} printerfont7;


#endif  /* _CBM_PRINTER_H_ */

