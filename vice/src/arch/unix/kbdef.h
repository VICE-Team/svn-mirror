/*
 * kbdef.h - X11 keyboard definitions.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
 *  Ettore Perazzoli (ettore@comm2000.it)
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

/* Keyboard definitions for the X11 keyboard driver. */

#ifndef _KBDEF_X_H
#define _KBDEF_X_H

#define	NO_SHIFT	(0)		/* Key is not shifted. */
#define	VIRTUAL_SHIFT	(1 << 0)	/* The key needs a shift on the real machine. */
#define	LEFT_SHIFT	(1 << 1)	/* Key is left shift. */
#define	RIGHT_SHIFT	(1 << 2)	/* Key is right shift. */
#define	ALLOW_SHIFT	(1 << 3)	/* Allow key to be shifted. */

typedef struct {
    KeySym sym;
    int row;
    int column;
    int shift;
} keyconv;

#ifdef VIC20
#define ROW0	7
#define ROW7	0
#define COL3	7
#define COL7	3
#else
#define ROW0	0
#define ROW7	7
#define COL3	3
#define COL7	7
#endif

/* VIC20/C64/C128 keyboard maps. */

#ifndef PET
static keyconv default_keyconvmap[] =
{
    { XK_Delete, ROW0, 0, ALLOW_SHIFT },	/* del */
    { XK_Return, ROW0, 1, ALLOW_SHIFT },
    { XK_Right, ROW0, 2, ALLOW_SHIFT },
    { XK_F7, ROW0, COL3, ALLOW_SHIFT },
    { XK_F1, ROW0, 4, ALLOW_SHIFT },
    { XK_F3, ROW0, 5, ALLOW_SHIFT },
    { XK_F5, ROW0, 6, ALLOW_SHIFT },
    { XK_Down, ROW0, COL7, ALLOW_SHIFT },

    { XK_3, 1, 0, NO_SHIFT },
    { XK_w, 1, 1, ALLOW_SHIFT },
    { XK_a, 1, 2, ALLOW_SHIFT },
    { XK_4, 1, COL3, NO_SHIFT },
    { XK_z, 1, 4, ALLOW_SHIFT },
    { XK_s, 1, 5, ALLOW_SHIFT },
    { XK_e, 1, 6, ALLOW_SHIFT },
    { XK_Shift_L, 1, COL7, LEFT_SHIFT },

    { XK_5, 2, 0, NO_SHIFT },
    { XK_r, 2, 1, ALLOW_SHIFT },
    { XK_d, 2, 2, ALLOW_SHIFT },
    { XK_6, 2, COL3, NO_SHIFT },
    { XK_c, 2, 4, ALLOW_SHIFT },
    { XK_f, 2, 5, ALLOW_SHIFT },
    { XK_t, 2, 6, ALLOW_SHIFT },
    { XK_x, 2, COL7, ALLOW_SHIFT },

    { XK_7, 3, 0, NO_SHIFT },
    { XK_y, 3, 1, ALLOW_SHIFT },
    { XK_g, 3, 2, ALLOW_SHIFT },
    { XK_8, 3, COL3, NO_SHIFT },
    { XK_b, 3, 4, ALLOW_SHIFT },
    { XK_h, 3, 5, ALLOW_SHIFT },
    { XK_u, 3, 6, ALLOW_SHIFT },
    { XK_v, 3, COL7, ALLOW_SHIFT },

    { XK_9, 4, 0, NO_SHIFT },
    { XK_i, 4, 1, ALLOW_SHIFT },
    { XK_j, 4, 2, ALLOW_SHIFT },
    { XK_0, 4, COL3, NO_SHIFT },
    { XK_m, 4, 4, ALLOW_SHIFT },
    { XK_k, 4, 5, ALLOW_SHIFT },
    { XK_o, 4, 6, ALLOW_SHIFT },
    { XK_n, 4, COL7, ALLOW_SHIFT },

    { XK_plus, 5, 0, NO_SHIFT },
    { XK_p, 5, 1, ALLOW_SHIFT },
    { XK_l, 5, 2, ALLOW_SHIFT },
    { XK_minus, 5, COL3, NO_SHIFT },
    { XK_period, 5, 4, NO_SHIFT },
    { XK_colon, 5, 5, NO_SHIFT },
    { XK_at, 5, 6, NO_SHIFT },		/* shifted @ gives 'grave' */
    { XK_comma, 5, COL7, NO_SHIFT },

    { XK_sterling, 6, 0, NO_SHIFT },	/* Sterling Pound sign */
    { XK_bar, 6, 0, ALLOW_SHIFT },	/* Sterling Pound sign */
    { XK_asterisk, 6, 1, NO_SHIFT },
    { XK_semicolon, 6, 2, NO_SHIFT },
    { XK_Home, 6, COL3, ALLOW_SHIFT },

    { XK_Shift_R, 6, 4, NO_SHIFT },

    { XK_equal, 6, 5, NO_SHIFT },
    
    { XK_asciicircum, 6, 6, NO_SHIFT }, /* Up arrow (US kbd) */
    { XK_asciitilde, 6, 6, NO_SHIFT },	/* Up arrow (scandinavian kbd) */

    { XK_slash, 6, COL7, NO_SHIFT },

    { XK_1, ROW7, 0, NO_SHIFT },
    { XK_underscore, ROW7, 1, NO_SHIFT }, /* Left arrow */
    { XK_Control_L, ROW7, 2, ALLOW_SHIFT }, /* CNTRL */
    { XK_2, ROW7, COL3, NO_SHIFT },
    { XK_space, ROW7, 4, ALLOW_SHIFT },
    { XK_Alt_L, ROW7, 5, ALLOW_SHIFT },	/* CBM */
    { XK_Meta_L, ROW7, 5, ALLOW_SHIFT },	/* CBM */
    { XK_q, ROW7, 6, ALLOW_SHIFT },
    { XK_Escape, ROW7, COL7, ALLOW_SHIFT }, /* STOP */

    { XK_BackSpace, ROW0, 0, ALLOW_SHIFT }, /* Del */

  /*
   * Some keys for DECstations
   */
    { XK_KP_F1, ROW0, 4, ALLOW_SHIFT },	/* F1 */
    { XK_KP_F2, ROW0, 5, ALLOW_SHIFT },	/* F3 */
    { XK_KP_F3, ROW0, 6, ALLOW_SHIFT },	/* F5 */
    { XK_KP_F4, ROW0, 2, ALLOW_SHIFT },	/* F7 */
    { XK_Insert, ROW0, 0, 16 },	/* Inst */
    { XK_Find, 6, COL3, ALLOW_SHIFT },	/* Home */

#ifdef DECMAP
    { DXK_Remove, ROW0, 0, ALLOW_SHIFT },	/* del (DECkeysym.h) */
    { XK_Help, ROW7, COL7, ALLOW_SHIFT },	/* because no real ESC-key on the keyboard /RH/ */
#endif

  /*
   * C128 Keyboard Extension
   * C128 Special keys, rows K1, K2 and K3 from the VIC-IIe
   *
   * Kx d02f    dc01:
   * 08  fe     KP_1    KP_7    KP_4    KP_2    Tab     KP_5    KP_8    Help
   * 09  fd     KP_3    KP_9    KP_6    KP_Entr LineFd KP_Minus KP_Plus Esc 
   * 10  fb  NoScroll   Right   Left    Down    Up      KP_Dot  KP_0    Alt
   *
   */

#ifdef C128
    { XK_KP_1, 8, 7, ALLOW_SHIFT },	/* Numpad 1 */
    { XK_KP_2, 8, 4, ALLOW_SHIFT },	/* Numpad 2 */
    { XK_KP_3, 9, 7, ALLOW_SHIFT },	/* Numpad 3 */
    { XK_KP_4, 8, 5, ALLOW_SHIFT },	/* Numpad 4 */
    { XK_KP_5, 8, 2, ALLOW_SHIFT },	/* Numpad 5 */
    { XK_KP_6, 9, 5, ALLOW_SHIFT },	/* Numpad 6 */
    { XK_KP_7, 8, 6, ALLOW_SHIFT },	/* Numpad 7 */
    { XK_KP_8, 8, 1, ALLOW_SHIFT },	/* Numpad 8 */
    { XK_KP_9, 9, 6, ALLOW_SHIFT },	/* Numpad 9 */
    { XK_KP_0, 10, 1, ALLOW_SHIFT },	/* Numpad 0 */
    { XK_KP_Subtract, 9, 2, ALLOW_SHIFT }, /* Numpad - */
    { XK_KP_Add, 0, 1, ALLOW_SHIFT },	/* Numpad + */
    { XK_KP_Separator, 10, 2, ALLOW_SHIFT }, /* Numpad . */
#endif				/* C128 */

  /*
   * Shifted C64/128 keys
   */

    { XK_Linefeed, ROW0, 1, VIRTUAL_SHIFT },
    { XK_Left, ROW0, 2, VIRTUAL_SHIFT },
    { XK_F8, ROW0, COL3, VIRTUAL_SHIFT },
    { XK_F2, ROW0, 4, VIRTUAL_SHIFT },
    { XK_F4, ROW0, 5, VIRTUAL_SHIFT },
    { XK_F6, ROW0, 6, VIRTUAL_SHIFT },
    { XK_Up, ROW0, COL7, VIRTUAL_SHIFT },

    { XK_numbersign, 1, 0, VIRTUAL_SHIFT },
    { XK_W, 1, 1, VIRTUAL_SHIFT },
    { XK_A, 1, 2, VIRTUAL_SHIFT },
    { XK_dollar, 1, COL3, VIRTUAL_SHIFT },
    { XK_Z, 1, 4, VIRTUAL_SHIFT },
    { XK_S, 1, 5, VIRTUAL_SHIFT },
    { XK_E, 1, 6, VIRTUAL_SHIFT },

    { XK_percent, 2, 0, VIRTUAL_SHIFT },
    { XK_R, 2, 1, VIRTUAL_SHIFT },
    { XK_D, 2, 2, VIRTUAL_SHIFT },
    { XK_ampersand, 2, COL3, VIRTUAL_SHIFT },
    { XK_C, 2, 4, VIRTUAL_SHIFT },
    { XK_F, 2, 5, VIRTUAL_SHIFT },
    { XK_T, 2, 6, VIRTUAL_SHIFT },
    { XK_X, 2, COL7, VIRTUAL_SHIFT },

    { XK_quoteright, 3, 0, VIRTUAL_SHIFT },	/* apostrophe */
    { XK_Y, 3, 1, VIRTUAL_SHIFT },
    { XK_G, 3, 2, VIRTUAL_SHIFT },
    { XK_parenleft, 3, COL3, VIRTUAL_SHIFT },
    { XK_B, 3, 4, VIRTUAL_SHIFT },
    { XK_H, 3, 5, VIRTUAL_SHIFT },
    { XK_U, 3, 6, VIRTUAL_SHIFT },
    { XK_V, 3, COL7, VIRTUAL_SHIFT },

    { XK_parenright, 4, 0, VIRTUAL_SHIFT },
    { XK_I, 4, 1, VIRTUAL_SHIFT },
    { XK_J, 4, 2, VIRTUAL_SHIFT },
    { XK_M, 4, 4, VIRTUAL_SHIFT },
    { XK_K, 4, 5, VIRTUAL_SHIFT },
    { XK_O, 4, 6, VIRTUAL_SHIFT },
    { XK_N, 4, COL7, VIRTUAL_SHIFT },

    { XK_P, 5, 1, VIRTUAL_SHIFT },
    { XK_L, 5, 2, VIRTUAL_SHIFT },
    { XK_greater, 5, 4, VIRTUAL_SHIFT },
    { XK_bracketleft, 5, 5, VIRTUAL_SHIFT },
    { XK_grave, 5, 6, VIRTUAL_SHIFT },	/* shifted @ */
    { XK_less, 5, COL7, VIRTUAL_SHIFT },

    { XK_backslash, 6, 0, VIRTUAL_SHIFT },
    { XK_bracketright, 6, 2, VIRTUAL_SHIFT },
#ifdef US_KBD
    { XK_asciitilde, 6, 6, NO_SHIFT },	/* shift up arrow (US kbd) */
#else
    { XK_asciicircum, 6, 6, ALLOW_SHIFT }, /* shift up arrow (scandinavian kbd) */
#endif
    { XK_question, 6, COL7, VIRTUAL_SHIFT },

    { XK_exclam, ROW7, 0, VIRTUAL_SHIFT },
    { XK_quotedbl, ROW7, COL3, VIRTUAL_SHIFT },
    { XK_Q, ROW7, 6, VIRTUAL_SHIFT },
    { 0, 0, 0, 0}
};

/* End of VIC20/C64/C128 keyboard */


#else

/* ------------------------------------------------------------------------- */

/* PET keyboard(s). */

/* PET 8032 Business Keyboard. */

static keyconv pet80map[] =
{ 
    { XK_0, 1, 3, NO_SHIFT },
    { XK_1, 1, 0, NO_SHIFT },
    { XK_2, 0, 0, NO_SHIFT },
    { XK_3, 9, 1, NO_SHIFT },
    { XK_4, 1, 1, NO_SHIFT },
    { XK_5, 0, 1, NO_SHIFT },
    { XK_6, 9, 2, NO_SHIFT },
    { XK_7, 1, 2, NO_SHIFT },
    { XK_8, 0, 2, NO_SHIFT },
    { XK_9, 9, 3, NO_SHIFT },

    { XK_Control_L, 9, 4, ALLOW_SHIFT },	/* left Ctrl maps to run/stop */

    { XK_Home, 8, 4, ALLOW_SHIFT },
    { XK_Right, 0, 5, ALLOW_SHIFT },
    { XK_Down, 5, 4, ALLOW_SHIFT },
    { XK_Delete, 4, 7, NO_SHIFT },	/* del */
    { XK_BackSpace, 4, 7, ALLOW_SHIFT },	/* inst/del */
    { XK_Shift_L, 6, 0, LEFT_SHIFT },
    { XK_Shift_R, 6, 6, RIGHT_SHIFT },
    { XK_Return, 3, 4, NO_SHIFT },
    { XK_Escape, 2, 0, NO_SHIFT },	/* Escape */

    { XK_Alt_L, 8, 0, ALLOW_SHIFT },		/* Left Alt -> RVS */
    { XK_Meta_L, 8, 0, ALLOW_SHIFT },		/* Left Meta -> RVS */

    { XK_Control_R, 7, 6, ALLOW_SHIFT },	/* Right Ctrl -> RPT */
    
    { XK_Tab, 4, 0, ALLOW_SHIFT },		/* Tab -> Tab */
#ifdef XK_ISO_Left_Tab
    { XK_ISO_Left_Tab, 4, 0, VIRTUAL_SHIFT },   /* Left Tab -> Shift-Tab */
#endif
#ifdef hpXK_BackTab
    { hpXK_BackTab, 4, 0, VIRTUAL_SHIFT },
#endif
#ifdef hpXK_KP_BackTab
    { hpXK_KP_BackTab, 4, 0, VIRTUAL_SHIFT },
#endif
    
    { XK_KP_0, 7, 4, ALLOW_SHIFT },	/* Numpad 0 */
    { XK_KP_1, 8, 7, ALLOW_SHIFT },	/* Numpad 1 */
    { XK_KP_2, 7, 7, ALLOW_SHIFT },	/* Numpad 2 */
    { XK_KP_3, 6, 7, ALLOW_SHIFT },	/* Numpad 3 */
    { XK_KP_4, 5, 7, ALLOW_SHIFT },	/* Numpad 4 */
    { XK_KP_5, 2, 7, ALLOW_SHIFT },	/* Numpad 5 */
    { XK_KP_6, 3, 7, ALLOW_SHIFT },	/* Numpad 6 */
    { XK_KP_7, 1, 4, ALLOW_SHIFT },	/* Numpad 7 */
    { XK_KP_8, 0, 4, ALLOW_SHIFT },	/* Numpad 8 */
    { XK_KP_9, 1, 7, ALLOW_SHIFT },	/* Numpad 9 */
    { XK_KP_Separator, 7, 3, NO_SHIFT },
    { XK_KP_Multiply, 9, 5, VIRTUAL_SHIFT },
    { XK_KP_Divide, 8, 6, NO_SHIFT },
    { XK_KP_Add, 2, 6, VIRTUAL_SHIFT },
    { XK_KP_Subtract, 0, 3, NO_SHIFT },
    { XK_KP_Enter, 3, 4, NO_SHIFT },
    { XK_KP_Decimal, 6, 4, NO_SHIFT },	/* Numpad . */

    { XK_period, 6, 4, NO_SHIFT },
    { XK_minus, 0, 3, NO_SHIFT },
    { XK_space, 8, 2, NO_SHIFT },
    { XK_colon, 9, 5, NO_SHIFT },
    { XK_underscore, 9, 0, NO_SHIFT },	/* Left arrow */
    { XK_comma, 7, 3, NO_SHIFT },

    { XK_slash, 8, 6, NO_SHIFT },
    { XK_semicolon, 2, 6, NO_SHIFT },

    { XK_at, 3, 6, NO_SHIFT },		/* shifted @ gives 'grave' */
    { XK_a, 3, 0, NO_SHIFT },
    { XK_b, 6, 2, NO_SHIFT },
    { XK_c, 6, 1, NO_SHIFT },
    { XK_d, 3, 1, NO_SHIFT },
    { XK_e, 5, 1, NO_SHIFT },
    { XK_f, 2, 2, NO_SHIFT },
    { XK_g, 3, 2, NO_SHIFT },
    { XK_h, 2, 3, NO_SHIFT },
    { XK_i, 4, 5, NO_SHIFT },
    { XK_j, 3, 3, NO_SHIFT },
    { XK_k, 2, 5, NO_SHIFT },
    { XK_l, 3, 5, NO_SHIFT },
    { XK_m, 8, 3, NO_SHIFT },
    { XK_n, 7, 2, NO_SHIFT },
    { XK_o, 5, 5, NO_SHIFT },
    { XK_p, 4, 6, NO_SHIFT },
    { XK_q, 5, 0, NO_SHIFT },
    { XK_r, 4, 2, NO_SHIFT },
    { XK_s, 2, 1, NO_SHIFT },
    { XK_t, 5, 2, NO_SHIFT },
    { XK_u, 5, 3, NO_SHIFT },
    { XK_v, 7, 1, NO_SHIFT },
    { XK_w, 4, 1, NO_SHIFT },
    { XK_x, 8, 1, NO_SHIFT },
    { XK_y, 4, 3, NO_SHIFT },
    { XK_z, 7, 0, NO_SHIFT },

    { XK_asciicircum, 1, 5, NO_SHIFT },	/* Up arrow (US kbd) */
    { XK_asciitilde, 1, 5, NO_SHIFT },	/* Up arrow (scandinavian kbd) */

    { XK_bracketright, 2, 5, NO_SHIFT },
    { XK_bracketleft, 5, 6, NO_SHIFT },

  /*
   * Shifted PET keys
   */

    { XK_Insert, 4, 7, VIRTUAL_SHIFT },
    { XK_Left, 0, 5, VIRTUAL_SHIFT },
    { XK_Up, 5, 4, VIRTUAL_SHIFT },

    { XK_A, 3, 0, VIRTUAL_SHIFT },
    { XK_B, 6, 2, VIRTUAL_SHIFT },
    { XK_C, 6, 1, VIRTUAL_SHIFT },
    { XK_D, 3, 1, VIRTUAL_SHIFT },
    { XK_E, 5, 1, VIRTUAL_SHIFT },
    { XK_F, 2, 2, VIRTUAL_SHIFT },
    { XK_G, 3, 2, VIRTUAL_SHIFT },
    { XK_H, 2, 3, VIRTUAL_SHIFT },
    { XK_I, 4, 5, VIRTUAL_SHIFT },
    { XK_J, 3, 3, VIRTUAL_SHIFT },
    { XK_K, 2, 5, VIRTUAL_SHIFT },
    { XK_L, 3, 5, VIRTUAL_SHIFT },
    { XK_M, 8, 3, VIRTUAL_SHIFT },
    { XK_N, 7, 2, VIRTUAL_SHIFT },
    { XK_O, 5, 5, VIRTUAL_SHIFT },
    { XK_P, 4, 6, VIRTUAL_SHIFT },
    { XK_Q, 5, 0, VIRTUAL_SHIFT },
    { XK_R, 4, 2, VIRTUAL_SHIFT },
    { XK_S, 2, 1, VIRTUAL_SHIFT },
    { XK_T, 5, 2, VIRTUAL_SHIFT },
    { XK_U, 5, 3, VIRTUAL_SHIFT },
    { XK_V, 7, 1, VIRTUAL_SHIFT },
    { XK_W, 4, 1, VIRTUAL_SHIFT },
    { XK_X, 8, 1, VIRTUAL_SHIFT },
    { XK_Y, 4, 3, VIRTUAL_SHIFT },
    { XK_Z, 7, 0, VIRTUAL_SHIFT },

    { XK_exclam, 1, 0, VIRTUAL_SHIFT },
    { XK_quotedbl, 0, 0, VIRTUAL_SHIFT },
    { XK_numbersign, 9, 1, VIRTUAL_SHIFT },
    { XK_dollar, 1, 1, VIRTUAL_SHIFT },
    { XK_percent, 0, 1, VIRTUAL_SHIFT },
    { XK_ampersand, 9, 2, VIRTUAL_SHIFT },
    { XK_quoteright, 1, 2, VIRTUAL_SHIFT },	/* apostrophe = shift 7 */
    { XK_parenleft, 0, 2, VIRTUAL_SHIFT },
    { XK_parenright, 9, 3, VIRTUAL_SHIFT },
    { XK_asterisk, 9, 5, VIRTUAL_SHIFT },
    { XK_plus, 2, 6, VIRTUAL_SHIFT },

    { XK_less, 7, 3, VIRTUAL_SHIFT },
    { XK_equal, 0, 3, VIRTUAL_SHIFT },
    { XK_greater, 6, 3, VIRTUAL_SHIFT },
    { XK_question, 8, 6, VIRTUAL_SHIFT },

    { 0, 0, 0, 0}
};


/* PET 3032/4032 Graphics Keyboard. */

keyconv pet40map[] =
{ 
    { XK_Return, 6, 5, NO_SHIFT },
    { XK_Down, 1, 6, ALLOW_SHIFT },
    { XK_Right, 0, 7, ALLOW_SHIFT },
    { XK_Home, 0, 6, ALLOW_SHIFT },
    { XK_BackSpace, 1, 7, ALLOW_SHIFT },
    { XK_Delete, 1, 7, NO_SHIFT },

    { XK_Tab, 9, 0, ALLOW_SHIFT },		/* Tab -> rev/off */
    { XK_KP_Tab, 9, 0, ALLOW_SHIFT },
#ifdef XK_ISO_Left_Tab
    { XK_ISO_Left_Tab, 9, 0, VIRTUAL_SHIFT },   /* Shift-Tab */
#endif
#ifdef hpXK_BackTab
    { hpXK_BackTab, 9, 0, VIRTUAL_SHIFT },	/* Tab -> rev/off */
#endif
#ifdef hpXK_KP_BackTab
    { hpXK_KP_BackTab, 9, 0, VIRTUAL_SHIFT },	/* Tab -> rev/off */
#endif
    
    { XK_Alt_L, 9, 0, ALLOW_SHIFT },		/* Left Alt -> rev/off */
    { XK_Meta_L, 9, 0, ALLOW_SHIFT },		/* Left Meta -> rev/off */

    { XK_Escape, 9, 4, ALLOW_SHIFT },		/* Escape -> Run/Stop */
    { XK_Control_L, 9, 4, ALLOW_SHIFT },	/* Left Ctrl -> Run/Stop */
    
    { XK_Find, 0, 6, NO_SHIFT },		/* Home */

    { XK_Shift_L, 8, 0, LEFT_SHIFT },
    { XK_Shift_R, 8, 5, RIGHT_SHIFT },

#ifdef DECMAP
    { DXK_Remove, 1, 7, NO_SHIFT },	/* del (DECkeysym.h) */
    { XK_Help, 9, 4, ALLOW_SHIFT },	/* because there is no real ESC-key
					   on the keyboard /RH/ */
#endif				/* DECMAP */

    { XK_a, 4, 0, NO_SHIFT },
    { XK_b, 6, 2, NO_SHIFT },
    { XK_c, 6, 1, NO_SHIFT },
    { XK_d, 4, 1, NO_SHIFT },
    { XK_e, 2, 1, NO_SHIFT },
    { XK_f, 5, 1, NO_SHIFT },
    { XK_g, 4, 2, NO_SHIFT },
    { XK_h, 5, 2, NO_SHIFT },
    { XK_i, 3, 3, NO_SHIFT },
    { XK_j, 4, 3, NO_SHIFT },
    { XK_k, 5, 3, NO_SHIFT },
    { XK_l, 4, 4, NO_SHIFT },
    { XK_m, 6, 3, NO_SHIFT },
    { XK_n, 7, 2, NO_SHIFT },
    { XK_o, 2, 4, NO_SHIFT },
    { XK_p, 3, 4, NO_SHIFT },
    { XK_q, 2, 0, NO_SHIFT },
    { XK_r, 3, 1, NO_SHIFT },
    { XK_s, 5, 0, NO_SHIFT },
    { XK_t, 2, 2, NO_SHIFT },
    { XK_u, 2, 3, NO_SHIFT },
    { XK_v, 7, 1, NO_SHIFT },
    { XK_w, 3, 0, NO_SHIFT },
    { XK_x, 7, 0, NO_SHIFT },
    { XK_y, 3, 2, NO_SHIFT },
    { XK_z, 6, 0, NO_SHIFT },

    { XK_KP_0, 8, 6, ALLOW_SHIFT },
    { XK_KP_1, 6, 6, ALLOW_SHIFT },
    { XK_KP_2, 7, 6, ALLOW_SHIFT },
    { XK_KP_3, 6, 7, ALLOW_SHIFT },
    { XK_KP_4, 4, 6, ALLOW_SHIFT },
    { XK_KP_5, 5, 6, ALLOW_SHIFT },
    { XK_KP_6, 4, 7, ALLOW_SHIFT },
    { XK_KP_7, 2, 6, ALLOW_SHIFT },
    { XK_KP_8, 3, 6, ALLOW_SHIFT },
    { XK_KP_9, 2, 7, ALLOW_SHIFT },

    { XK_KP_Separator, 7, 3, ALLOW_SHIFT },
    { XK_KP_Multiply, 5, 7, ALLOW_SHIFT },
    { XK_KP_Divide, 3, 7, ALLOW_SHIFT },
    { XK_KP_Add, 7, 7, ALLOW_SHIFT },
    { XK_KP_Subtract, 8, 7, ALLOW_SHIFT },
    { XK_KP_Enter, 6, 5, ALLOW_SHIFT },
    { XK_KP_Decimal, 9, 6, ALLOW_SHIFT },	/* Numpad . */

    { XK_0, 8, 6, NO_SHIFT },
    { XK_1, 6, 6, NO_SHIFT },
    { XK_2, 7, 6, NO_SHIFT },
    { XK_3, 6, 7, NO_SHIFT },
    { XK_4, 4, 6, NO_SHIFT },
    { XK_5, 5, 6, NO_SHIFT },
    { XK_6, 4, 7, NO_SHIFT },
    { XK_7, 2, 6, NO_SHIFT },
    { XK_8, 3, 6, NO_SHIFT },
    { XK_9, 2, 7, NO_SHIFT },

    { XK_slash, 3, 7, NO_SHIFT },
    { XK_bracketright, 1, 4, NO_SHIFT },
    { XK_bracketleft, 0, 4, NO_SHIFT },
    { XK_parenright, 1, 4, NO_SHIFT },
    { XK_parenleft, 0, 4, NO_SHIFT },
    { XK_question, 7, 4, NO_SHIFT },
    { XK_dollar, 1, 1, NO_SHIFT },
    { XK_asterisk, 5, 7, NO_SHIFT },
    { XK_colon, 5, 4, NO_SHIFT },
    { XK_comma, 7, 3, NO_SHIFT },
    { XK_quotedbl, 1, 0, NO_SHIFT },
    { XK_sterling, 1, 3, NO_SHIFT },
    { XK_backslash, 1, 3, NO_SHIFT },
    { XK_bar, 1, 3, NO_SHIFT },
    { XK_numbersign, 0, 1, NO_SHIFT },
    { XK_exclam, 0, 0, NO_SHIFT },
    { XK_percent, 0,  2, NO_SHIFT },
    { XK_plus, 7, 7, NO_SHIFT },
    { XK_underscore, 0, 5, NO_SHIFT },
    { XK_ampersand, 0, 3, NO_SHIFT },
    { XK_semicolon, 6, 4, NO_SHIFT },
    { XK_minus, 8, 7, NO_SHIFT },
    { XK_equal, 9, 7, NO_SHIFT },
    { XK_space, 9, 2, NO_SHIFT },
    { XK_greater, 8, 4, NO_SHIFT },
    { XK_less, 9, 3, NO_SHIFT },
    { XK_period, 9, 6, NO_SHIFT },
    { XK_at, 8, 1, NO_SHIFT },
    { XK_asciicircum, 2, 5, NO_SHIFT },	/* up arrow (US kbd) */
    { XK_asciitilde, 2, 5, NO_SHIFT },	/* up arrow (scandinavian kbd) */

    /* shifted keys */
    { XK_Left, 0, 7, VIRTUAL_SHIFT },
    { XK_Up, 1, 6, VIRTUAL_SHIFT },
    { XK_Insert, 1, 7, VIRTUAL_SHIFT },
    { XK_Clear, 0, 6, VIRTUAL_SHIFT },

    { XK_grave, 8, 1, VIRTUAL_SHIFT },	/* shifted @ */

    { XK_W, 3, 0, VIRTUAL_SHIFT },
    { XK_A, 4, 0, VIRTUAL_SHIFT },
    { XK_Z, 6, 0, VIRTUAL_SHIFT },
    { XK_S, 5, 0, VIRTUAL_SHIFT },
    { XK_E, 2, 1, VIRTUAL_SHIFT },
    { XK_R, 3, 1, VIRTUAL_SHIFT },
    { XK_D, 4, 1, VIRTUAL_SHIFT },
    { XK_C, 6, 1, VIRTUAL_SHIFT },
    { XK_F, 5, 1, VIRTUAL_SHIFT },
    { XK_T, 2, 2, VIRTUAL_SHIFT },
    { XK_X, 7, 0, VIRTUAL_SHIFT },
    { XK_Y, 3, 2, VIRTUAL_SHIFT },
    { XK_G, 4, 2, VIRTUAL_SHIFT },
    { XK_B, 6, 2, VIRTUAL_SHIFT },
    { XK_H, 5, 2, VIRTUAL_SHIFT },
    { XK_U, 2, 3, VIRTUAL_SHIFT },
    { XK_V, 7, 1, VIRTUAL_SHIFT },
    { XK_I, 3, 3, VIRTUAL_SHIFT },
    { XK_J, 4, 3, VIRTUAL_SHIFT },
    { XK_M, 6, 3, VIRTUAL_SHIFT },
    { XK_K, 5, 3, VIRTUAL_SHIFT },
    { XK_O, 2, 4, VIRTUAL_SHIFT },
    { XK_N, 7, 2, VIRTUAL_SHIFT },
    { XK_P, 3, 4, VIRTUAL_SHIFT },
    { XK_L, 4, 4, VIRTUAL_SHIFT },
    { XK_Q, 2, 0, VIRTUAL_SHIFT },

    { 0, 0, 0, 0 }
};

/* static keyconv *keyconvmap = pet80map; */
/* static int CONV_KEYS = sizeof(pet80map) / sizeof(keyconv); */
#endif				/* PET */

#ifndef PET
/* #define CONV_KEYS (sizeof(keyconvmap)/sizeof(keyconv)) */
#endif

static keyconv *keyconvmap = NULL;

#endif /* !_KBDEF_X_H */
