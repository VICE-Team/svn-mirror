/*
 * c128kbd.c -- C128 keyboard.
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

#include "c128kbd.h"
#include "cartridge.h"
#include "kbd.h"

static keyconv c128_keyboard[256] = {
    { -1, -1, 0 },              /*           (no key)           */
    { -1, -1, 0 },              /*          ESC -> (no key)     */
    { 7, 0, 0 },                /*            1 -> 1            */
    { 7, 3, 0 },                /*            2 -> 2            */
    { 1, 0, 0 },                /*            3 -> 3            */
    { 1, 3, 0 },                /*            4 -> 4            */
    { 2, 0, 0 },                /*            5 -> 5            */
    { 2, 3, 0 },                /*            6 -> 6            */
    { 3, 0, 0 },                /*            7 -> 7            */
    { 3, 3, 0 },                /*            8 -> 8            */
    { 4, 0, 0 },                /*            9 -> 9            */
    { 4, 3, 0 },                /*            0 -> 0            */
    { 5, 0, 0 },                /*        Minus -> Plus         */
    { 5, 3, 0 },                /*        Equal -> Minus        */
    { 0, 0, 0 },                /*    Backspace -> Del          */
    { 7, 2, 0 },                /*          TAB -> Ctrl         */
    { 7, 6, 0 },                /*            Q -> Q            */
    { 1, 1, 0 },                /*            W -> W            */
    { 1, 6, 0 },                /*            E -> E            */
    { 2, 1, 0 },                /*            R -> R            */
    { 2, 6, 0 },                /*            T -> T            */
    { 3, 1, 0 },                /*            Y -> Y            */
    { 3, 6, 0 },                /*            U -> U            */
    { 4, 1, 0 },                /*            I -> I            */
    { 4, 6, 0 },                /*            O -> O            */
    { 5, 1, 0 },                /*            p -> P            */
    { 5, 6, 0 },                /*            [ -> @            */
    { 6, 1, 0 },                /*            ] -> *            */
    { 0, 1, 0 },                /*       Return -> Return       */
    { 7, 5, 0 },                /*    Left Ctrl -> CBM          */
    { 1, 2, 0 },                /*            A -> A            */
    { 1, 5, 0 },                /*            S -> S            */
    { 2, 2, 0 },                /*            D -> D            */
    { 2, 5, 0 },                /*            F -> F            */
    { 3, 2, 0 },                /*            G -> G            */
    { 3, 5, 0 },                /*            H -> H            */
    { 4, 2, 0 },                /*            J -> J            */
    { 4, 5, 0 },                /*            K -> K            */
    { 5, 2, 0 },                /*            L -> L            */
    { 5, 5, 0 },                /*            ; -> :            */
    { 6, 2, 0 },                /*            ' -> ;            */
    { 7, 1, 0 },                /*            ` -> Left Arrow   */
    { 1, 7, 1 },                /*   Left Shift -> Left Shift   */
    { 6, 5, 0 },                /*            \ -> =            */
    { 1, 4, 0 },                /*            Z -> Z            */
    { 2, 7, 0 },                /*            X -> X            */
    { 2, 4, 0 },                /*            C -> C            */
    { 3, 7, 0 },                /*            V -> V            */
    { 3, 4, 0 },                /*            B -> B            */
    { 4, 7, 0 },                /*            N -> N            */
    { 4, 4, 0 },                /*            M -> M            */
    { 5, 7, 0 },                /*            , -> ,            */
    { 5, 4, 0 },                /*            . -> .            */
    { 6, 7, 0 },                /*            / -> /            */
    { 6, 4, 0 },                /*  Right Shift -> Right Shift  */
    { 6, 1, 0 },                /*       Grey * -> *            */
    { -1, -1, 0 },              /*     Left Alt -> (no key)     */
    { 7, 4, 0 },                /*        Space -> Space        */
    { 7, 7, 0 },                /*    Caps Lock -> Run/Stop     */
#if 0
    { 0, 4, 0 },                /*           F1 -> F1           */
    { 0, 4, 1 },                /*           F2 -> F2           */
    { 0, 5, 0 },                /*           F3 -> F3           */
    { 0, 5, 1 },                /*           F4 -> F4           */
    { 0, 6, 0 },                /*           F5 -> F5           */
    { 0, 6, 1 },                /*           F6 -> F6           */
    { 0, 3, 0 },                /*           F7 -> F7           */
    { 0, 3, 1 },                /*           F8 -> F8           */
#else
    { 9, 0, 0 },                /*           F1 -> ESC          */
    { 8, 3, 0 },                /*           F2 -> TAB          */
    { 10, 0, 0 },               /*           F3 -> Alt          */
    { -1, -1, 0 },              /*           F4 -> (no key)     */
    { 8, 0, 0 },                /*           F5 -> HELP         */
    { 9, 3, 0 },                /*           F6 -> LINE FEED    */
    { -1, -1, 0 },              /*           F7 -> (no key)     */
    { 10, 7, 0 },               /*           F8 -> NO SCROLL    */
#endif
    { 0, 4, 0 },                /*           F9 -> F1           */
    { 0, 5, 0 },                /*          F10 -> F3           */
    { -1, -1, 0 },              /*     Num Lock -> (no key)     */
    { -1, -1, 0 },              /*  Scroll Lock -> (no key)     */
    { 8, 6, 0 },                /*     Numpad 7 -> Numpad 7     */
    { 8, 1, 0 },                /*     Numpad 8 -> Numpad 8     */
    { 9, 6, 0 },                /*     Numpad 9 -> Numpad 9     */
    { 9, 1, 0 },                /*     Numpad - -> Numpad +     */
    { 8, 5, 0 },                /*     Numpad 4 -> Numpad 4     */
    { 8, 2, 0 },                /*     Numpad 5 -> Numpad 5     */
    { 9, 5, 0 },                /*     Numpad 6 -> Numpad 6     */
    { 9, 2, 0 },                /*     Numpad + -> Numpad -     */
    { 8, 7, 0 },                /*     Numpad 1 -> Numpad 1     */
    { 8, 4, 0 },                /*     Numpad 2 -> Numpad 2     */
    { 9, 7, 0 },                /*     Numpad 3 -> Numpad 3     */
    { 10, 1, 0 },               /*     Numpad 0 -> Numpad 0     */
    { 10, 2, 0 },               /*     Numpad . -> Numpad .     */
    { -1, -1, 0 },              /*       SysReq -> (no key)     */
    { -1, -1, 0 },              /*           85 -> (no key)     */
    { -1, -1, 0 },              /*           86 -> (no key)     */
    { 0, 6, 0 },                /*          F11 -> F5           */
    { 0, 3, 0 },                /*          F12 -> F7           */
    { 6, 3, 0 },                /*         Home -> CLR/HOME     */
    { 10, 3, 0 },               /*           Up -> Up           */
    { -1, -1, 0 },              /*         PgUp -> (no key)     */
    { 10, 5, 0 },               /*         Left -> Left         */
    { 10, 6, 0 },               /*        Right -> Right        */
    { 0, 7, 0 },                /*          End -> CRSR DOWN    */
    { 10, 4, 0 },               /*         Down -> Down         */
    { 0, 2, 0 },                /*       PgDown -> CRSR RIGHT   */
    { 6, 0, 0 },                /*          Ins -> Pound        */
    { 6, 6, 0 },                /*          Del -> Up Arrow     */
    { -1, -1, 0 },              /* Numpad Enter -> (no key)     */
    { -1, -1, 0 },              /*   Right Ctrl -> (no key)     */
    { -1, -1, 0 },              /*        Pause -> (no key)     */
    { -1, -1, 0 },              /*       PrtScr -> (no key)     */
    { -1, -1, 0 },              /*     Numpad / -> (no key)     */
    { -1, -1, 0 },              /*    Right Alt -> (no key)     */
    { -1, -1, 0 },              /*        Break -> (no key)     */
    { -1, -1, 0 },              /*   Left Win95 -> (no key)     */
    { -1, -1, 0 },              /*  Right Win95 -> (no key)     */
};

int c128_kbd_init(void)
{
    return kbd_init(1,
                    1, 7, c128_keyboard, sizeof(c128_keyboard));
}
