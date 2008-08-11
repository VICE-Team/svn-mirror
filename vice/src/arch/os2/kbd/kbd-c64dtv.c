/*
 * c64dtvkbd.c -- C64DTV keyboard for MS-DOS.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "kbd.h"

int c64_kbd_init(void)
{
//    kbd_set_freeze_function(cartridge_trigger_freeze);
    return kbd_init();
}

// ------------------ for further use ---------------------

/*
char *arr2sym(int col, int row, int vshift)
{
    keymap_t *k = keymap;

    while (((k->key.col != col) || (k->key.row != row) ||
            (k->key.vshift != vshift)) && k)
        k++;

    return k->sym;
}

keyconv *sym2arr(const char *c)
{
    keymap_t *k = keymap;

    while (strcmp(c, k->sym) && k)
        k++;

    return &k->key;
}


struct _keymap
{
    char sym[10];
    keyconv key;
} keymap_t;

// unshifted
const char kbdlayout1[8][8][8] = {
// col 0        1    2    3    4    5    6          7          // row
    {"Del",    "3", "5", "7", "9", "+", "Pound",   "1"},       //  0
    {"Return", "w", "r", "y", "i", "p", "*",       "ArrowLt"}, //  1 
    {"CrsrRt", "a"  "d", "g", "j", "l", ";",       "Ctrl"},    //  2 
    {"F7",     "4", "6", "8", "0", "-", "Home",    "2"},       //  3 
    {"F1",     "z", "c", "b", "m", ".", "RtShift", "Space"},   //  4 
    {"F3",     "s", "f", "h", "k", ":", "=",       "CBM"},     //  5 
    {"F5",     "e", "t", "u", "o", "@", "ArrowUp", "q"},       //  6 
    {"CrsrDn", "",  "x", "v", "n", ",", "/",       "Stop"}     //  7 
}

// shifted
const char kbdlayout2[8][8][8] = {
// col 0        1    2    3    4    5    6          7           // row
    {"Ins",    "#", "%%", "'", ")", "+", "Pound",   "!"},       //  0
    {"Return", "W", "R",  "Y", "I", "P", "*",       "ArrowLt"}, //  1
    {"CrsrLt", "A"  "D",  "G", "J", "L", ";",       "Ctrl"},    //  2
    {"F8",     "$", "&",  "(", "0", "-", "Home",    "\""},      //  3
    {"F2",     "Z", "C",  "B", "M", ".", "RtShift", "Space"},   //  4
    {"F4",     "S", "F",  "H", "K", ":", "=",       "CBM"},     //  5
    {"F6",     "E", "T",  "U", "O", "@", "ArrowUp", "Q"},       //  6
    {"CrsrUp", "",  "X",  "V", "N", ",", "/",       "Stop"}     //  7
}

const keymap_t keymap[] = {

    // --- Unshifted ---

    {"NoKey",   {-1, -1, 0}},
    {"Stop",    {7, 7, 0}},
    {"1",       {7, 0, 0}},
    {"2",       {7, 3, 0}},
    {"3",       {1, 0, 0}},
    {"4",       {1, 3, 0}},
    {"5",       {2, 0, 0}},
    {"6",       {2, 3, 0}},
    {"7",       {3, 0, 0}},
    {"8",       {3, 3, 0}},
    {"9",       {4, 0, 0}},
    {"0",       {4, 3, 0}},
    {"+",       {5, 0, 0}},
    {"-",       {5, 3, 0}},
    {"Del",     {0, 0, 0}},
    {"Ctrl",    {7, 2, 0}},
    {"Q",       {7, 6, 0}},
    {"W",       {1, 1, 0}},
    {"E",       {1, 6, 0}},
    {"R",       {2, 1, 0}},
    {"T",       {2, 6, 0}},
    {"Y",       {3, 1, 0}},
    {"U",       {3, 6, 0}},
    {"I",       {4, 1, 0}},
    {"O",       {4, 6, 0}},
    {"P",       {5, 1, 0}},
    {"@",       {5, 6, 0}},
    {"*",       {6, 1, 0}},
    {"Return",  {0, 1, 0}},
    {"CBM",     {7, 5, 0}},
    {"A",       {1, 2, 0}},
    {"S",       {1, 5, 0}},
    {"D",       {2, 2, 0}},
    {"F",       {2, 5, 0}},
    {"G",       {3, 2, 0}},
    {"H",       {3, 5, 0}},
    {"J",       {4, 2, 0}},
    {"K",       {4, 5, 0}},
    {"L",       {5, 2, 0}},
    {":",       {5, 5, 0}},
    {";",       {6, 2, 0}},
    {"ArrowLt", {7, 1, 0}},
    {"=",       {6, 5, 0}},
    {"Z",       {1, 4, 0}},
    {"X",       {2, 7, 0}},
    {"C",       {2, 4, 0}},
    {"V",       {3, 7, 0}},
    {"B",       {3, 4, 0}},
    {"N",       {4, 7, 0}},
    {"M",       {4, 4, 0}},
    {",",       {5, 7, 0}},
    {".",       {5, 4, 0}},
    {"/",       {6, 7, 0}},
    {"RtShift", {6, 4, 0}},
    {"Space",   {7, 4, 0}},
    {"F1",      {0, 4, 0}},
    {"F3",      {0, 5, 0}},
    {"F5",      {0, 6, 0}},
    {"F7",      {0, 3, 0}},
    {"Home",    {6, 3, 0}},
    {"CrsrRt",  {0, 2, 0}},
    {"CrsrDn",  {0, 7, 0}},
    {"Pound",   {6, 0, 0}},
    {"ArrowUp", {6, 6, 0}},

    // --- Shifted ---

    {"Run",     {7, 7, 1}},
    {"!",       {7, 0, 1}},
    {"\"",      {7, 3, 1}},
    {"#",       {1, 0, 1}},
    {"$",       {1, 3, 1}},
    {"%%",       {2, 0, 1}},
    {"&",       {2, 3, 1}},
    {"'",       {3, 0, 1}},
    {"(",       {3, 3, 1}},
    {")",       {4, 0, 1}},
    {"[",       {5, 5, 1}},
    {"]",       {6, 2, 1}},
    {"<",       {5, 7, 1}},
    {">",       {5, 4, 1}},
    {"?",       {6, 7, 1}},
    {"F2",      {0, 4, 1}},
    {"F4",      {0, 5, 1}},
    {"F6",      {0, 6, 1}},
    {"F8",      {0, 3, 1}},
    {"Pi",      {6, 6, 1}},
    {"Clr",     {6, 3, 1}},
    {"CrsrUp",  {0, 7, 1}},
    {"CrsrLt",  {0, 2, 1}},
    {"LtShift", {1, 7, 1}},
    {"Ins",     {0, 0, 1}},
    {NULL,      {-1, -1, 0}}
}
*/
