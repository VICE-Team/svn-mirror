/*
 * petkbd.c -- PET keyboard handling for MS-DOS.
 *
 * Written by
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

#include "vice.h"

#include <stdio.h>

#include "kbd.h"

/* PET 3032/4032 keyboard map.  */
static keyconv pet40_keyboard[256] = {
    { -1, -1, 0 },		/*           (no key)           */
    { -1, -1, 0 },		/*          ESC -> (no key)     */
    { 0, 0, 0 },		/*            1 -> !            */
    { 1, 0, 0 },		/*            2 -> "            */
    { 0, 1, 0 },		/*            3 -> #            */
    { 1, 1, 0 },		/*            4 -> $            */
    { 0, 2, 0 },		/*            5 -> %            */
    { 1, 2, 0 },		/*            6 -> '            */
    { 0, 3, 0 },		/*            7 -> &            */
    { 1, 3, 0 },		/*            8 -> \            */
    { 0, 4, 0 },		/*            9 -> (            */
    { 1, 4, 0 },		/*            0 -> )            */
    { 0, 5, 0 },		/*        Minus -> Left arrow   */
    { -1, -1, 0 },		/*        Equal -> (no key)     */
    { 1, 7, 0 },		/*    Backspace -> Del          */
    { -1, -1, 0 },		/*          TAB -> (no key)     */
    { 2, 0, 0 },		/*            Q -> Q            */
    { 3, 0, 0 },		/*            W -> W            */
    { 2, 1, 0 },		/*            E -> E            */
    { 3, 1, 0 },		/*            R -> R            */
    { 2, 2, 0 },		/*            T -> T            */
    { 3, 2, 0 },		/*            Y -> Y            */
    { 2, 3, 0 },		/*            U -> U            */
    { 3, 3, 0 },		/*            I -> I            */
    { 2, 4, 0 },		/*            O -> O            */
    { 3, 4, 0 },		/*            P -> P            */
    { 2, 5, 0 },		/*            [ -> Up arrow     */
    { -1, -1, 0 },		/*            ] -> (no key)     */
    { 6, 5, 0 },		/*       Return -> Return       */
    { 9, 0, 0 },		/*    Left Ctrl -> RVS ON/OFF   */
    { 4, 0, 0 },		/*            A -> A            */
    { 5, 0, 0 },		/*            S -> S            */
    { 4, 1, 0 },		/*            D -> D            */
    { 5, 1, 0 },		/*            F -> F            */
    { 4, 2, 0 },		/*            G -> G            */
    { 5, 2, 0 },		/*            H -> H            */
    { 4, 3, 0 },		/*            J -> J            */
    { 5, 3, 0 },		/*            K -> K            */
    { 4, 4, 0 },		/*            L -> L            */
    { 5, 4, 0 },		/*            ; -> :            */
    { -1, -1, 0 },		/*            ' -> (no key)     */
    { -1, -1, 0 },		/*	      ` -> (no key)     */
    { 8, 0, 1 },		/*   Left Shift -> Left Shift   */
    { -1, -1, 0 },		/*            \ -> (no key)     */
    { 6, 0, 0 },		/*            Z -> Z            */
    { 7, 0, 0 },		/*            X -> X            */
    { 6, 1, 0 },		/*            C -> C            */
    { 7, 1, 0 },		/*            V -> V            */
    { 6, 2, 0 },		/*            B -> B            */
    { 7, 2, 0 },		/*            N -> N            */
    { 6, 3, 0 },		/*            M -> M            */
    { 7, 3, 0 },		/*            , -> ,            */
    { 6, 4, 0 },		/*            . -> ;            */
    { 7, 4, 0 },		/*            / -> ?            */
    { 8, 5, 0 },		/*  Right Shift -> Right Shift  */
    { 5, 7, 0 },		/*       Grey * -> *            */
    { -1, -1, 0 },		/*     Left Alt -> (no key)     */
    { 9, 2, 0 },		/*        Space -> Space        */
    { 9, 4, 0 },		/*    Caps Lock -> RUN/STOP	*/
    { -1, -1, 0 },		/*           F1 -> (no key)     */
    { -1, -1, 0 },		/*           F2 -> (no key)     */
    { -1, -1, 0 },		/*           F3 -> (no key)     */
    { -1, -1, 0 },		/*           F4 -> (no key)     */
    { -1, -1, 0 },		/*           F5 -> (no key)     */
    { -1, -1, 0 },		/*           F6 -> (no key)     */
    { -1, -1, 0 },		/*           F7 -> (no key)     */
    { -1, -1, 0 },		/*           F8 -> (no key)     */
    { -1, -1, 0 },		/*           F9 -> (no key)     */
    { -1, -1, 0 },		/*          F10 -> (no key)	*/
    { 0, 6, 0 },		/*     Num Lock -> CLR/HOME	*/
    { -1, -1, 0 },		/*  Scroll Lock -> (no key)	*/
    { 2, 6, 0 },		/*     Numpad 7 -> 7	 	*/
    { 3, 6, 0 },		/*     Numpad 8 -> 8		*/
    { 2, 7, 0 },		/*     Numpad 9 -> 9		*/
    { 8, 7, 0 },		/*     Numpad - -> -		*/
    { 4, 6, 0 },		/*     Numpad 4 -> 4		*/
    { 5, 6, 0 },		/*     Numpad 5 -> 5		*/
    { 4, 7, 0 },		/*     Numpad 6 -> 6	 	*/
    { 7, 7, 0 },		/*     Numpad + -> +		*/
    { 6, 6, 0 },		/*     Numpad 1 -> 1		*/
    { 7, 6, 0 },		/*     Numpad 2 -> 2	 	*/
    { 6, 7, 0 },		/*     Numpad 3 -> 3		*/
    { 8, 6, 0 },		/*     Numpad 0 -> 0		*/
    { 9, 6, 0 },		/*     Numpad . -> .		*/
    { -1, -1, 0 },		/*       SysReq -> (no key) 	*/
    { -1, -1, 0 },		/*           85 -> (no key) 	*/
    { -1, -1, 0 },		/*           86 -> (no key) 	*/
    { -1, -1, 0 },		/*          F11 -> (no key) 	*/
    { -1, -1, 0 },		/*          F12 -> (no key) 	*/
    { 9, 1, 0 },		/*         Home -> [	 	*/
    { 1, 6, 1 },		/*           Up -> CRSR UP      */
    { 8, 2, 0 },		/*         PgUp -> ]	 	*/
    { 0, 7, 1 },		/*         Left -> CRSR LEFT	*/
    { 0, 7, 0 },		/*        Right -> CRSR RIGHT 	*/
    { 9, 3, 0 },		/*          End -> <	 	*/
    { 1, 6, 0 },		/*         Down -> CRSR DOWN 	*/
    { 8, 4, 0 },		/*       PgDown -> >	 	*/
    { 8, 1, 0 },		/*          Ins -> @	 	*/
    { -1, -1, 0 },		/*          Del -> (no key) 	*/
    { 9, 7, 0 },		/* Numpad Enter -> =	 	*/
    { 9, 4, 0 },		/*   Right Ctrl -> RUN/STOP 	*/
    { -1, -1, 0 },		/*        Pause -> (no key) 	*/
    { -1, -1, 0 },		/*       PrtScr -> (no key) 	*/
    { 3, 7, 0 },		/*     Numpad / -> /        	*/
    { -1, -1, 0 },		/*    Right Alt -> (no key) 	*/
    { -1, -1, 0 },		/*        Break -> (no key) 	*/
    { -1, -1, 0 },		/*   Left Win95 -> (no key) 	*/
    { -1, -1, 0 },		/*  Right Win95 -> (no key) 	*/
};

/* PET 8032 keyboard map.  */
static keyconv pet80_keyboard[256] = {
    { -1, -1, 0 },		/*           (no key)           */
    { -1, -1, 0 },		/*          ESC -> (no key)     */
    { 1, 0, 0 },		/*            1 -> 1            */
    { 0, 0, 0 },		/*            2 -> 2            */
    { 9, 1, 0 },		/*            3 -> 3            */
    { 1, 1, 0 },		/*            4 -> 4            */
    { 0, 1, 0 },		/*            5 -> 5            */
    { 9, 2, 0 },		/*            6 -> 6            */
    { 1, 2, 0 },		/*            7 -> 7            */
    { 0, 2, 0 },		/*            8 -> 8            */
    { 9, 3, 0 },		/*            9 -> 9            */
    { 1, 3, 0 },		/*            0 -> 0            */
    { 9, 5, 0 },		/*            - -> :            */
    { 0, 3, 0 },		/*            = -> -            */
    { 4, 7, 0 },		/*    Backspace -> Del          */
    { 4, 0, 0 },		/*          Tab -> Tab		*/
    { 5, 0, 0 },		/*            Q -> Q            */
    { 4, 1, 0 },		/*            W -> W            */
    { 5, 1, 0 },		/*            E -> E            */
    { 4, 2, 0 },		/*            R -> R            */
    { 5, 2, 0 },		/*            T -> T            */
    { 4, 3, 0 },		/*            Y -> Y            */
    { 5, 3, 0 },		/*            U -> U            */
    { 4, 5, 0 },		/*            I -> I            */
    { 5, 5, 0 },		/*            O -> O            */
    { 4, 6, 0 },		/*            P -> P            */
    { 5, 6, 0 },		/*            [ -> [            */
    { 2, 4, 0 },		/*            ] -> ]		*/
    { 3, 4, 0 },		/*       Return -> Return       */
    { 8, 0, 0 },		/*    Left Ctrl -> RVS	        */
    { 3, 0, 0 },		/*            A -> A            */
    { 2, 1, 0 },		/*            S -> S            */
    { 3, 1, 0 },		/*            D -> D            */
    { 2, 2, 0 },		/*            F -> F            */
    { 3, 2, 0 },		/*            G -> G            */
    { 2, 3, 0 },		/*            H -> H            */
    { 3, 3, 0 },		/*            J -> J            */
    { 2, 5, 0 },		/*            K -> K            */
    { 3, 5, 0 },		/*            L -> L            */
    { 2, 6, 0 },		/*            ; -> ;            */
    { 3, 6, 0 },		/*            ' -> @            */
    { 9, 0, 0 },		/*            ` -> Left Arrow	*/
    { 6, 0, 1 },		/*   Left Shift -> Left Shift   */
    { 4, 4, 0 },		/*            \ -> \	        */
    { 7, 0, 0 },		/*            Z -> Z            */
    { 8, 1, 0 },		/*            X -> X            */
    { 6, 1, 0 },		/*            C -> C            */
    { 7, 1, 0 },		/*            V -> V            */
    { 6, 2, 0 },		/*            B -> B            */
    { 7, 2, 0 },		/*            N -> N            */
    { 8, 3, 0 },		/*            M -> M            */
    { 7, 3, 0 },		/*            , -> ,            */
    { 6, 3, 0 },		/*            . -> .            */
    { 8, 6, 0 },		/*            / -> /            */
    { 6, 6, 0 },		/*  Right Shift -> Right Shift  */
    { -1, -1, 0 },		/*       Grey * -> (no key)     */
    { -1, -1, 0 },		/*     Left Alt -> (no key)     */
    { 8, 2, 0 },		/*        Space -> Space        */
    { 2, 0, 0 },		/*    Caps Lock -> ESC	        */
    { -1, -1, 0 },		/*           F1 -> (no key)     */
    { -1, -1, 0 },		/*           F2 -> (no key)     */
    { -1, -1, 0 },		/*           F3 -> (no key)     */
    { -1, -1, 0 },		/*           F4 -> (no key)     */
    { -1, -1, 0 },		/*           F5 -> (no key)     */
    { -1, -1, 0 },		/*           F6 -> (no key)     */
    { -1, -1, 0 },		/*           F7 -> (no key)     */
    { -1, -1, 0 },		/*           F8 -> (no key)     */
    { -1, -1, 0 },		/*           F9 -> (no key)     */
    { -1, -1, 0 },		/*          F10 -> (no key)	*/
    { -1, -1, 0 },		/*     Num Lock -> (no key)	*/
    { -1, -1, 0 },		/*  Scroll Lock -> (no key)	*/
    { 1, 4, 0 },		/*     Numpad 7 -> Numpad 7 	*/
    { 0, 4, 0 },		/*     Numpad 8 -> Numpad 8	*/
    { 1, 7, 0 },		/*     Numpad 9 -> Numpad 9	*/
    { -1, -1, 0 },		/*     Numpad - -> (no key)	*/
    { 5, 7, 0 },		/*     Numpad 4 -> Numpad 4 	*/
    { 2, 7, 0 },		/*     Numpad 5 -> Numpad 5 	*/
    { 3, 7, 0 },		/*     Numpad 6 -> Numpad 6 	*/
    { -1, -1, 0 },		/*     Numpad + -> (no key)	*/
    { 8, 7, 0 },		/*     Numpad 1 -> Numpad 1 	*/
    { 7, 7, 0 },		/*     Numpad 2 -> Numpad 2 	*/
    { 6, 7, 0 },		/*     Numpad 3 -> Numpad 3 	*/
    { 7, 4, 0 },		/*     Numpad 0 -> Numpad 0 	*/
    { 6, 4, 0 },		/*     Numpad . -> .	 	*/
    { -1, -1, 0 },		/*       SysReq -> (no key) 	*/
    { -1, -1, 0 },		/*           85 -> (no key) 	*/
    { -1, -1, 0 },		/*           86 -> (no key) 	*/
    { -1, -1, 0 },		/*          F11 -> (no key) 	*/
    { -1, -1, 0 },		/*          F12 -> (no key) 	*/
    { 8, 4, 0 },		/*         Home -> CLR/HOME 	*/
    { 5, 4, 1 },		/*           Up -> CRSR UP  	*/
    { -1, -1, 0 },		/*         PgUp -> (no key) 	*/
    { 0, 5, 1 }, 		/*         Left -> CRSR LEFT	*/
    { 0, 5, 0 }, 		/*        Right -> CRSR RIGHT	*/
    { -1, -1, 0 },		/*          End -> (no key) 	*/
    { 5, 4, 0 },		/*         Down -> CRSR DOWN	*/
    { -1, -1, 0 },		/*       PgDown -> (no key) 	*/
    { 9, 4, 0 },		/*          Ins -> STOP 	*/
    { 1, 5, 0 },		/*          Del -> Up arrow 	*/
    { -1, -1, 0 },		/* Numpad Enter -> (no key) 	*/
    { 7, 6, 0 },		/*   Right Ctrl -> RPT	 	*/
    { -1, -1, 0 },		/*        Pause -> (no key) 	*/
    { -1, -1, 0 },		/*       PrtScr -> (no key) 	*/
    { -1, -1, 0 },		/*     Numpad / -> (no key) 	*/
    { -1, -1, 0 },		/*    Right Alt -> (no key) 	*/
    { -1, -1, 0 },		/*        Break -> (no key) 	*/
    { -1, -1, 0 },		/*   Left Win95 -> (no key) 	*/
    { -1, -1, 0 }		/*  Right Win95 -> (no key) 	*/
};

int pet_kbd_init(void)
{
    return kbd_init(2,
                    6, 0, pet80_keyboard, sizeof(pet80_keyboard),
                    8, 0, pet40_keyboard, sizeof(pet40_keyboard));
}

int pet_kbd_init_resources(void)
{
    return kbd_init_resources();
}

int pet_kbd_init_cmdline_options(void)
{
    return kbd_init_cmdline_options();
}

