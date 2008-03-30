/*
 * vicii-sprites.c - Sprites for the MOS 6569 (VIC-II) emulation.
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

#include "types.h"
#include "vicii.h"

#include "vicii-sprites.h"



const vic_ii_sprites_fetch_t vic_ii_sprites_fetch_table[256][4] =
{
  /* $00 */ { { -1, -1 } },
  /* $01 */ { { 0, 5, 0, 0 },  { -1, -1 } },
  /* $02 */ { { 2, 5, 1, 1 },  { -1, -1 } },
  /* $03 */ { { 0, 7, 0, 1 },  { -1, -1 } },
  /* $04 */ { { 4, 5, 2, 2 },  { -1, -1 } },
  /* $05 */ { { 0, 9, 0, 2 },  { -1, -1 } },
  /* $06 */ { { 2, 7, 1, 2 },  { -1, -1 } },
  /* $07 */ { { 0, 9, 0, 2 },  { -1, -1 } },
  /* $08 */ { { 6, 5, 3, 3 },  { -1, -1 } },
  /* $09 */ { { 0, 5, 0, 0 },  { 6, 5, 3, 3 },  { -1, -1 } },
  /* $0A */ { { 2, 9, 1, 3 },  { -1, -1 } },
  /* $0B */ { { 0, 11, 0, 3 },  { -1, -1 } },
  /* $0C */ { { 4, 7, 2, 3 },  { -1, -1 } },
  /* $0D */ { { 0, 11, 0, 3 },  { -1, -1 } },
  /* $0E */ { { 2, 9, 1, 3 },  { -1, -1 } },
  /* $0F */ { { 0, 11, 0, 3 },  { -1, -1 } },
  /* $10 */ { { 8, 5, 4, 4 },  { -1, -1 } },
  /* $11 */ { { 0, 5, 0, 0 },  { 8, 5, 4, 4 },  { -1, -1 } },
  /* $12 */ { { 2, 5, 1, 1 },  { 8, 5, 4, 4 },  { -1, -1 } },
  /* $13 */ { { 0, 7, 0, 1 },  { 8, 5, 4, 4 },  { -1, -1 } },
  /* $14 */ { { 4, 9, 2, 4 },  { -1, -1 } },
  /* $15 */ { { 0, 13, 0, 4 },  { -1, -1 } },
  /* $16 */ { { 2, 11, 1, 4 },  { -1, -1 } },
  /* $17 */ { { 0, 13, 0, 4 },  { -1, -1 } },
  /* $18 */ { { 6, 7, 3, 4 },  { -1, -1 } },
  /* $19 */ { { 0, 5, 0, 0 },  { 6, 7, 3, 4 },  { -1, -1 } },
  /* $1A */ { { 2, 11, 1, 4 },  { -1, -1 } },
  /* $1B */ { { 0, 13, 0, 4 },  { -1, -1 } },
  /* $1C */ { { 4, 9, 2, 4 },  { -1, -1 } },
  /* $1D */ { { 0, 13, 0, 4 },  { -1, -1 } },
  /* $1E */ { { 2, 11, 1, 4 },  { -1, -1 } },
  /* $1F */ { { 0, 13, 0, 4 },  { -1, -1 } },
  /* $20 */ { { 10, 5, 5, 5 },  { -1, -1 } },
  /* $21 */ { { 0, 5, 0, 0 },  { 10, 5, 5, 5 },  { -1, -1 } },
  /* $22 */ { { 2, 5, 1, 1 },  { 10, 5, 5, 5 },  { -1, -1 } },
  /* $23 */ { { 0, 7, 0, 1 },  { 10, 5, 5, 5 },  { -1, -1 } },
  /* $24 */ { { 4, 5, 2, 2 },  { 10, 5, 5, 5 },  { -1, -1 } },
  /* $25 */ { { 0, 9, 0, 2 },  { 10, 5, 5, 5 },  { -1, -1 } },
  /* $26 */ { { 2, 7, 1, 2 },  { 10, 5, 5, 5 },  { -1, -1 } },
  /* $27 */ { { 0, 9, 0, 2 },  { 10, 5, 5, 5 },  { -1, -1 } },
  /* $28 */ { { 6, 9, 3, 5 },  { -1, -1 } },
  /* $29 */ { { 0, 5, 0, 0 },  { 6, 9, 3, 5 },  { -1, -1 } },
  /* $2A */ { { 2, 13, 1, 5 },  { -1, -1 } },
  /* $2B */ { { 0, 15, 0, 5 },  { -1, -1 } },
  /* $2C */ { { 4, 11, 2, 5 },  { -1, -1 } },
  /* $2D */ { { 0, 15, 0, 5 },  { -1, -1 } },
  /* $2E */ { { 2, 13, 1, 5 },  { -1, -1 } },
  /* $2F */ { { 0, 15, 0, 5 },  { -1, -1 } },
  /* $30 */ { { 8, 7, 4, 5 },  { -1, -1 } },
  /* $31 */ { { 0, 5, 0, 0 },  { 8, 7, 4, 5 },  { -1, -1 } },
  /* $32 */ { { 2, 5, 1, 1 },  { 8, 7, 4, 5 },  { -1, -1 } },
  /* $33 */ { { 0, 7, 0, 1 },  { 8, 7, 4, 5 },  { -1, -1 } },
  /* $34 */ { { 4, 11, 2, 5 },  { -1, -1 } },
  /* $35 */ { { 0, 15, 0, 5 },  { -1, -1 } },
  /* $36 */ { { 2, 13, 1, 5 },  { -1, -1 } },
  /* $37 */ { { 0, 15, 0, 5 },  { -1, -1 } },
  /* $38 */ { { 6, 9, 3, 5 },  { -1, -1 } },
  /* $39 */ { { 0, 5, 0, 0 },  { 6, 9, 3, 5 },  { -1, -1 } },
  /* $3A */ { { 2, 13, 1, 5 },  { -1, -1 } },
  /* $3B */ { { 0, 15, 0, 5 },  { -1, -1 } },
  /* $3C */ { { 4, 11, 2, 5 },  { -1, -1 } },
  /* $3D */ { { 0, 15, 0, 5 },  { -1, -1 } },
  /* $3E */ { { 2, 13, 1, 5 },  { -1, -1 } },
  /* $3F */ { { 0, 15, 0, 5 },  { -1, -1 } },
  /* $40 */ { { 12, 5, 6, 6 },  { -1, -1 } },
  /* $41 */ { { 0, 5, 0, 0 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $42 */ { { 2, 5, 1, 1 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $43 */ { { 0, 7, 0, 1 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $44 */ { { 4, 5, 2, 2 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $45 */ { { 0, 9, 0, 2 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $46 */ { { 2, 7, 1, 2 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $47 */ { { 0, 9, 0, 2 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $48 */ { { 6, 5, 3, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $49 */ { { 0, 5, 0, 0 },  { 6, 5, 3, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $4A */ { { 2, 9, 1, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $4B */ { { 0, 11, 0, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $4C */ { { 4, 7, 2, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $4D */ { { 0, 11, 0, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $4E */ { { 2, 9, 1, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $4F */ { { 0, 11, 0, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
  /* $50 */ { { 8, 9, 4, 6 },  { -1, -1 } },
  /* $51 */ { { 0, 5, 0, 0 },  { 8, 9, 4, 6 },  { -1, -1 } },
  /* $52 */ { { 2, 5, 1, 1 },  { 8, 9, 4, 6 },  { -1, -1 } },
  /* $53 */ { { 0, 7, 0, 1 },  { 8, 9, 4, 6 },  { -1, -1 } },
  /* $54 */ { { 4, 13, 2, 6 },  { -1, -1 } },
  /* $55 */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $56 */ { { 2, 15, 1, 6 },  { -1, -1 } },
  /* $57 */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $58 */ { { 6, 11, 3, 6 },  { -1, -1 } },
  /* $59 */ { { 0, 5, 0, 0 },  { 6, 11, 3, 6 },  { -1, -1 } },
  /* $5A */ { { 2, 15, 1, 6 },  { -1, -1 } },
  /* $5B */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $5C */ { { 4, 13, 2, 6 },  { -1, -1 } },
  /* $5D */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $5E */ { { 2, 15, 1, 6 },  { -1, -1 } },
  /* $5F */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $60 */ { { 10, 7, 5, 6 },  { -1, -1 } },
  /* $61 */ { { 0, 5, 0, 0 },  { 10, 7, 5, 6 },  { -1, -1 } },
  /* $62 */ { { 2, 5, 1, 1 },  { 10, 7, 5, 6 },  { -1, -1 } },
  /* $63 */ { { 0, 7, 0, 1 },  { 10, 7, 5, 6 },  { -1, -1 } },
  /* $64 */ { { 4, 5, 2, 2 },  { 10, 7, 5, 6 },  { -1, -1 } },
  /* $65 */ { { 0, 9, 0, 2 },  { 10, 7, 5, 6 },  { -1, -1 } },
  /* $66 */ { { 2, 7, 1, 2 },  { 10, 7, 5, 6 },  { -1, -1 } },
  /* $67 */ { { 0, 9, 0, 2 },  { 10, 7, 5, 6 },  { -1, -1 } },
  /* $68 */ { { 6, 11, 3, 6 },  { -1, -1 } },
  /* $69 */ { { 0, 5, 0, 0 },  { 6, 11, 3, 6 },  { -1, -1 } },
  /* $6A */ { { 2, 15, 1, 6 },  { -1, -1 } },
  /* $6B */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $6C */ { { 4, 13, 2, 6 },  { -1, -1 } },
  /* $6D */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $6E */ { { 2, 15, 1, 6 },  { -1, -1 } },
  /* $6F */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $70 */ { { 8, 9, 4, 6 },  { -1, -1 } },
  /* $71 */ { { 0, 5, 0, 0 },  { 8, 9, 4, 6 },  { -1, -1 } },
  /* $72 */ { { 2, 5, 1, 1 },  { 8, 9, 4, 6 },  { -1, -1 } },
  /* $73 */ { { 0, 7, 0, 1 },  { 8, 9, 4, 6 },  { -1, -1 } },
  /* $74 */ { { 4, 13, 2, 6 },  { -1, -1 } },
  /* $75 */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $76 */ { { 2, 15, 1, 6 },  { -1, -1 } },
  /* $77 */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $78 */ { { 6, 11, 3, 6 },  { -1, -1 } },
  /* $79 */ { { 0, 5, 0, 0 },  { 6, 11, 3, 6 },  { -1, -1 } },
  /* $7A */ { { 2, 15, 1, 6 },  { -1, -1 } },
  /* $7B */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $7C */ { { 4, 13, 2, 6 },  { -1, -1 } },
  /* $7D */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $7E */ { { 2, 15, 1, 6 },  { -1, -1 } },
  /* $7F */ { { 0, 17, 0, 6 },  { -1, -1 } },
  /* $80 */ { { 14, 5, 7, 7 },  { -1, -1 } },
  /* $81 */ { { 0, 5, 0, 0 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $82 */ { { 2, 5, 1, 1 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $83 */ { { 0, 7, 0, 1 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $84 */ { { 4, 5, 2, 2 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $85 */ { { 0, 9, 0, 2 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $86 */ { { 2, 7, 1, 2 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $87 */ { { 0, 9, 0, 2 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $88 */ { { 6, 5, 3, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $89 */ { { 0, 5, 0, 0 },  { 6, 5, 3, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $8A */ { { 2, 9, 1, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $8B */ { { 0, 11, 0, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $8C */ { { 4, 7, 2, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $8D */ { { 0, 11, 0, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $8E */ { { 2, 9, 1, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $8F */ { { 0, 11, 0, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $90 */ { { 8, 5, 4, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $91 */ { { 0, 5, 0, 0 },  { 8, 5, 4, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $92 */ { { 2, 5, 1, 1 },  { 8, 5, 4, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $93 */ { { 0, 7, 0, 1 },  { 8, 5, 4, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $94 */ { { 4, 9, 2, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $95 */ { { 0, 13, 0, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $96 */ { { 2, 11, 1, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $97 */ { { 0, 13, 0, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $98 */ { { 6, 7, 3, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $99 */ { { 0, 5, 0, 0 },  { 6, 7, 3, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $9A */ { { 2, 11, 1, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $9B */ { { 0, 13, 0, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $9C */ { { 4, 9, 2, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $9D */ { { 0, 13, 0, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $9E */ { { 2, 11, 1, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $9F */ { { 0, 13, 0, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
  /* $A0 */ { { 10, 9, 5, 7 },  { -1, -1 } },
  /* $A1 */ { { 0, 5, 0, 0 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $A2 */ { { 2, 5, 1, 1 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $A3 */ { { 0, 7, 0, 1 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $A4 */ { { 4, 5, 2, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $A5 */ { { 0, 9, 0, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $A6 */ { { 2, 7, 1, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $A7 */ { { 0, 9, 0, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $A8 */ { { 6, 13, 3, 7 },  { -1, -1 } },
  /* $A9 */ { { 0, 5, 0, 0 },  { 6, 13, 3, 7 },  { -1, -1 } },
  /* $AA */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $AB */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $AC */ { { 4, 15, 2, 7 },  { -1, -1 } },
  /* $AD */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $AE */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $AF */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $B0 */ { { 8, 11, 4, 7 },  { -1, -1 } },
  /* $B1 */ { { 0, 5, 0, 0 },  { 8, 11, 4, 7 },  { -1, -1 } },
  /* $B2 */ { { 2, 5, 1, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
  /* $B3 */ { { 0, 7, 0, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
  /* $B4 */ { { 4, 15, 2, 7 },  { -1, -1 } },
  /* $B5 */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $B6 */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $B7 */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $B8 */ { { 6, 13, 3, 7 },  { -1, -1 } },
  /* $B9 */ { { 0, 5, 0, 0 },  { 6, 13, 3, 7 },  { -1, -1 } },
  /* $BA */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $BB */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $BC */ { { 4, 15, 2, 7 },  { -1, -1 } },
  /* $BD */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $BE */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $BF */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $C0 */ { { 12, 7, 6, 7 },  { -1, -1 } },
  /* $C1 */ { { 0, 5, 0, 0 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $C2 */ { { 2, 5, 1, 1 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $C3 */ { { 0, 7, 0, 1 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $C4 */ { { 4, 5, 2, 2 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $C5 */ { { 0, 9, 0, 2 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $C6 */ { { 2, 7, 1, 2 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $C7 */ { { 0, 9, 0, 2 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $C8 */ { { 6, 5, 3, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $C9 */ { { 0, 5, 0, 0 },  { 6, 5, 3, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $CA */ { { 2, 9, 1, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $CB */ { { 0, 11, 0, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $CC */ { { 4, 7, 2, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $CD */ { { 0, 11, 0, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $CE */ { { 2, 9, 1, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $CF */ { { 0, 11, 0, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
  /* $D0 */ { { 8, 11, 4, 7 },  { -1, -1 } },
  /* $D1 */ { { 0, 5, 0, 0 },  { 8, 11, 4, 7 },  { -1, -1 } },
  /* $D2 */ { { 2, 5, 1, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
  /* $D3 */ { { 0, 7, 0, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
  /* $D4 */ { { 4, 15, 2, 7 },  { -1, -1 } },
  /* $D5 */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $D6 */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $D7 */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $D8 */ { { 6, 13, 3, 7 },  { -1, -1 } },
  /* $D9 */ { { 0, 5, 0, 0 },  { 6, 13, 3, 7 },  { -1, -1 } },
  /* $DA */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $DB */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $DC */ { { 4, 15, 2, 7 },  { -1, -1 } },
  /* $DD */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $DE */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $DF */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $E0 */ { { 10, 9, 5, 7 },  { -1, -1 } },
  /* $E1 */ { { 0, 5, 0, 0 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $E2 */ { { 2, 5, 1, 1 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $E3 */ { { 0, 7, 0, 1 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $E4 */ { { 4, 5, 2, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $E5 */ { { 0, 9, 0, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $E6 */ { { 2, 7, 1, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $E7 */ { { 0, 9, 0, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
  /* $E8 */ { { 6, 13, 3, 7 },  { -1, -1 } },
  /* $E9 */ { { 0, 5, 0, 0 },  { 6, 13, 3, 7 },  { -1, -1 } },
  /* $EA */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $EB */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $EC */ { { 4, 15, 2, 7 },  { -1, -1 } },
  /* $ED */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $EE */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $EF */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $F0 */ { { 8, 11, 4, 7 },  { -1, -1 } },
  /* $F1 */ { { 0, 5, 0, 0 },  { 8, 11, 4, 7 },  { -1, -1 } },
  /* $F2 */ { { 2, 5, 1, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
  /* $F3 */ { { 0, 7, 0, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
  /* $F4 */ { { 4, 15, 2, 7 },  { -1, -1 } },
  /* $F5 */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $F6 */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $F7 */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $F8 */ { { 6, 13, 3, 7 },  { -1, -1 } },
  /* $F9 */ { { 0, 5, 0, 0 },  { 6, 13, 3, 7 },  { -1, -1 } },
  /* $FA */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $FB */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $FC */ { { 4, 15, 2, 7 },  { -1, -1 } },
  /* $FD */ { { 0, 19, 0, 7 },  { -1, -1 } },
  /* $FE */ { { 2, 17, 1, 7 },  { -1, -1 } },
  /* $FF */ { { 0, 19, 0, 7 },  { -1, -1 } }
};



const int vic_ii_sprites_crunch_table[64] =
{
  1,   4,   3, /* 0 */
  4,   1,   0, /* 3 */
  -1,   0,   1, /* 6 */
  4,   3,   4, /* 9 */
  1,   8,   7, /* 12 */
  8,   1,   4, /* 15 */
  3,   4,   1, /* 18 */
  0,  -1,   0, /* 21 */
  1,   4,   3, /* 24 */
  4,   1,  -8, /* 27 */
  -9,  -8,   1, /* 30 */
  4,   3,   4, /* 33 */
  1,   0,  -1, /* 36 */
  0,   1,   4, /* 39 */
  3,   4,   1, /* 42 */
  8,   7,   8, /* 45 */
  1,   4,   3, /* 48 */
  4,   1,   0, /* 51 */
  -1,   0,   1, /* 54 */
  4,   3,   4, /* 57 */
  1, -40, -41, /* 60 */
  0
};



/* Each byte in this array is a bit mask representing the sprites that
   have a pixel turned on in that position.  This is used for sprite-sprite
   collision checking.  */
static BYTE sprline[VIC_II_SCREEN_WIDTH + 2 * VIC_II_MAX_SPRITE_WIDTH];

/* Sprite tables.  */
#ifdef AVOID_STATIC_ARRAYS
static DWORD *sprite_doubling_table;
static BYTE *mcsprtable;
#else
static DWORD sprite_doubling_table[65536];
static BYTE mcsprtable[256];
#endif



static void
init_drawing_tables (void)
{
  unsigned int i, lmsk, wmsk;

#ifdef AVOID_STATIC_ARRAYS
  if (sprite_doubling_table == NULL)
    sprite_doubling_table = xmalloc (sizeof (*sprite_doubling_table)
                                     * 65536);
  if (mcsprtable == NULL)
    mcsprtable = xmalloc (sizeof (*mcsprtable) * 256);
#endif

  for (i = 0; i <= 0xff; i++)
    mcsprtable[i] = ((i & 0xc0 ? 0xc0 : 0) | (i & 0x30 ? 0x30 : 0)
                     | (i & 0x0c ? 0x0c : 0) | (i & 0x03 ? 0x03 : 0));

  for (i = 0; i <= 0xffff; i++)
    {
      sprite_doubling_table[i] = 0;
      for (lmsk = 0xc0000000, wmsk = 0x8000; wmsk; lmsk >>= 2, wmsk >>= 1)
        if (i & wmsk)
          sprite_doubling_table[i] |= lmsk;
    }
}



/* Sprite drawing macros.  */

#define SPRITE_PIXEL(raster, do_draw, sprite_bit, imgptr, collmskptr,   \
                     pos, color, collmsk_return)                        \
  do                                                                    \
    {                                                                   \
      if ((do_draw) && (collmskptr)[(pos)] == 0)                        \
        (imgptr)[(pos)] = RASTER_PIXEL((raster), (color));              \
      (collmsk_return) |= (collmskptr)[(pos)];                          \
      (collmskptr)[(pos)] |= (sprite_bit);                              \
    }                                                                   \
  while (0)

#ifdef ALLOW_UNALIGNED_ACCESS

#define SPRITE_PIXEL_2x(raster, do_draw, sprite_bit, imgptr,             \
                        collmskptr, pos, color, collmsk_return)          \
  do                                                                     \
    {                                                                    \
     if ((do_draw) && (collmskptr)[(pos)] == 0)                          \
       *((PIXEL2 *)(imgptr) + (pos)) = RASTER_PIXEL2((raster), (color)); \
     (collmsk_return) |= (collmskptr)[(pos)];                            \
     (collmskptr)[(pos)] |= (sprite_bit);                                \
    }                                                                    \
  while (0)

#else

#define SPRITE_PIXEL_2x(raster, do_draw, sprite_bit, imgptr, collmskptr, \
                          pos, color, collmsk_return)                    \
  do                                                                     \
    {                                                                    \
      if ((do_draw) && (collmskptr)[(pos)] == 0)                         \
        (imgptr)[(pos) * 2] = (imgptr)[(pos) * 2 + 1]                    \
          = RASTER_PIXEL((raster), (color));                             \
     (collmsk_return) |= (collmskptr)[pos];                              \
     (collmskptr)[pos] |= (sprite_bit);                                  \
    }                                                                    \
  while (0)

#endif


/* Hires sprites */

#define _SPRITE_MASK(raster, msk, gfxmsk, size, sprite_bit, imgptr,     \
                     collmskptr, color, collmsk_return, DRAW)           \
  do                                                                    \
    {                                                                   \
      DWORD __m;                                                        \
      int __p;                                                          \
                                                                        \
      for (__m = 1 << ((size) - 1), __p = 0;                            \
           __p < (size);                                                \
           __p++, __m >>= 1)                                            \
        if ((msk) & __m)                                                \
          {                                                             \
            if ((gfxmsk) & __m)                                         \
              DRAW (raster, 0, sprite_bit, imgptr, collmskptr, __p,     \
                    color, collmsk_return);                             \
            else                                                        \
              DRAW (raster, 1, sprite_bit, imgptr, collmskptr, __p,     \
                    color, collmsk_return);                             \
          }                                                             \
    }                                                                   \
  while (0)

#define SPRITE_MASK(raster, msk, gfxmsk, size, sprite_bit, imgptr,         \
                    collmskptr, color, collmsk_return)                     \
  _SPRITE_MASK (raster, msk, gfxmsk, size, sprite_bit, imgptr, collmskptr, \
                color, collmsk_return, SPRITE_PIXEL)

#define SPRITE_MASK_2x(raster, msk, gfxmsk, size, sprite_bit, imgptr,      \
                       collmskptr, color, collmsk_return)                  \
  _SPRITE_MASK (raster, msk, gfxmsk, size, sprite_bit, imgptr, collmskptr, \
                color, collmsk_return, SPRITE_PIXEL_2x)


/* Multicolor sprites */

#define _MCSPRITE_MASK(raster, mcmsk, gfxmsk, size, sprite_bit, imgptr,   \
                       collmskptr, pixel_table, collmsk_return, DRAW)     \
  do                                                                      \
    {                                                                     \
      DWORD __m;                                                          \
      int __p;                                                            \
                                                                          \
      for (__m = 1 << ((size) - 1), __p = 0;                              \
           __p < (size);                                                  \
           __p += 2, __m >>= 2, (mcmsk) <<= 2)                            \
        {                                                                 \
          BYTE __c;                                                       \
                                                                          \
          __c = ((mcmsk) >> 22) & 0x3;                                    \
                                                                          \
          if (__c)                                                        \
            {                                                             \
              if ((gfxmsk) & __m)                                         \
                DRAW (raster, 0, sprite_bit, imgptr, collmskptr, __p,     \
                      pixel_table[__c], collmsk_return);                  \
              else                                                        \
                DRAW (raster, 1, sprite_bit, imgptr, collmskptr, __p,     \
                      pixel_table[__c], collmsk_return);                  \
                                                                          \
              if ((gfxmsk) & (__m >> 1))                                  \
                DRAW (raster, 0, sprite_bit, imgptr, collmskptr, __p + 1, \
                     pixel_table[__c], collmsk_return);                   \
              else                                                        \
                DRAW (raster, 1, sprite_bit, imgptr, collmskptr, __p + 1, \
                      pixel_table[__c], collmsk_return);                  \
            }                                                             \
        }                                                                 \
    }                                                                     \
  while (0)


#define MCSPRITE_MASK(raster, mcmsk, gfxmsk, size, sprite_bit, imgptr,  \
                      collmskptr, pixel_table, collmsk_return)          \
  _MCSPRITE_MASK (raster, mcmsk, gfxmsk, size, sprite_bit, imgptr,      \
                  collmskptr, pixel_table, collmsk_return,              \
                  SPRITE_PIXEL)

#define MCSPRITE_MASK_2x(raster, mcmsk, gfxmsk, size, sprite_bit, imgptr, \
                         collmskptr, pixel_table, collmsk_return)         \
  _MCSPRITE_MASK (raster, mcmsk, gfxmsk, size, sprite_bit, imgptr,        \
                  collmskptr, pixel_table, collmsk_return,                \
                  SPRITE_PIXEL_2x)


#define _MCSPRITE_DOUBLE_MASK(raster, mcmsk, gfxmsk, size, sprite_bit,  \
                              imgptr, collmskptr, pixel_table,          \
                              collmsk_return, DRAW)                     \
  do                                                                    \
    {                                                                   \
      DWORD __m;                                                        \
      int __p, __i;                                                     \
                                                                        \
      for (__m = 1 << ((size) - 1), __p = 0; __p < (size);              \
           __p += 4, (mcmsk) <<= 2)                                     \
        {                                                               \
          BYTE __c;                                                     \
                                                                        \
          __c = ((mcmsk) >> 22) & 0x3;                                  \
                                                                        \
          for (__i = 0; __i < 4; __i++, __m >>= 1)                      \
            if (__c)                                                    \
              {                                                         \
                if ((gfxmsk) & __m)                                     \
                  DRAW (raster, 0, sprite_bit, imgptr, collmskptr,      \
                        __p + __i, pixel_table[__c],                    \
                        collmsk_return);                                \
                else                                                    \
                  DRAW (raster, 1, sprite_bit, imgptr, collmskptr,      \
                       __p + __i, pixel_table[__c],                     \
                       collmsk_return);                                 \
              }                                                         \
      }                                                                 \
    }                                                                   \
  while (0)

#define MCSPRITE_DOUBLE_MASK(raster, mcmsk, gfxmsk, size, sprite_bit,     \
                             imgptr, collmskptr, pixel_table,             \
                             collmsk_return)                              \
  _MCSPRITE_DOUBLE_MASK (raster, mcmsk, gfxmsk, size, sprite_bit, imgptr, \
                         collmskptr, pixel_table, collmsk_return,         \
                         SPRITE_PIXEL)

#define MCSPRITE_DOUBLE_MASK_2x(raster, mcmsk, gfxmsk, size, sprite_bit,  \
                                imgptr, collmskptr, pixel_table,          \
                                collmsk_return)                           \
  _MCSPRITE_DOUBLE_MASK (raster, mcmsk, gfxmsk, size, sprite_bit, imgptr, \
                         collmskptr, pixel_table, collmsk_return,         \
                         SPRITE_PIXEL_2x)



/* Draw one hires sprite.  */
inline static void 
draw_hires_sprite (PIXEL * line_ptr,
                   BYTE * gfx_msk_ptr,
                   int n,
                   int double_size)
{
  if (vic_ii.raster.sprite_status.sprites[n].x < VIC_II_SCREEN_WIDTH)
    {
      DWORD sprmsk, collmsk;
      BYTE *msk_ptr = gfx_msk_ptr + ((vic_ii.raster.sprite_status.sprites[n].x + VIC_II_MAX_SPRITE_WIDTH
                                      - vic_ii.raster.xsmooth) / 8);
      BYTE *sptr = sprline + VIC_II_MAX_SPRITE_WIDTH + vic_ii.raster.sprite_status.sprites[n].x;
      PIXEL *ptr = line_ptr + vic_ii.raster.sprite_status.sprites[n].x * ((double_size) ? 2 : 1);
      BYTE *data_ptr = (BYTE *) (vic_ii.raster.sprite_status.sprite_data + n);
      int lshift = (vic_ii.raster.sprite_status.sprites[n].x - vic_ii.raster.xsmooth) & 0x7;
      int in_background = vic_ii.raster.sprite_status.sprites[n].in_background;

      if (vic_ii.raster.sprite_status.sprites[n].x_expanded)
        {
          WORD sbit = 0x101 << n;
          WORD cmsk = 0;

          collmsk = ((((msk_ptr[1] << 24) | (msk_ptr[2] << 16)
                       | (msk_ptr[3] << 8) | msk_ptr[4]) << lshift)
                     | (msk_ptr[5] >> (8 - lshift)));
          sprmsk = sprite_doubling_table[(data_ptr[0] << 8)
                                         | data_ptr[1]];
          cmsk = 0;
          if (!vic_ii.idle_state && (sprmsk & collmsk) != 0)
            vic_ii.raster.sprite_status.sprite_background_collisions |= sbit;
          if (in_background)
            {
              if (double_size)
                SPRITE_MASK_2x (&vic_ii.raster, sprmsk, collmsk, 32, sbit, ptr, sptr,
                        vic_ii.raster.sprite_status.sprites[n].color, cmsk);
              else
                SPRITE_MASK (&vic_ii.raster, sprmsk, collmsk, 32, sbit, ptr, sptr,
                        vic_ii.raster.sprite_status.sprites[n].color, cmsk);
            }
          else
            {
              if (double_size)
                SPRITE_MASK_2x (&vic_ii.raster, sprmsk, 0, 32, sbit, ptr, sptr,
                        vic_ii.raster.sprite_status.sprites[n].color, cmsk);
              else
                SPRITE_MASK (&vic_ii.raster, sprmsk, 0, 32, sbit, ptr, sptr,
                        vic_ii.raster.sprite_status.sprites[n].color, cmsk);
            }
          sprmsk = sprite_doubling_table[data_ptr[2]];
          collmsk = ((((msk_ptr[5] << 8) | msk_ptr[6]) << lshift)
                     | (msk_ptr[7] >> (8 - lshift)));
          if (!vic_ii.idle_state && (sprmsk & collmsk) != 0)
            vic_ii.raster.sprite_status.sprite_background_collisions |= sbit;
          if (in_background)
            {
              if (double_size)
                SPRITE_MASK_2x (&vic_ii.raster, sprmsk, collmsk, 16, sbit, ptr + 64,
                                sptr + 32, vic_ii.raster.sprite_status.sprites[n].color, cmsk);
              else
                SPRITE_MASK (&vic_ii.raster, sprmsk, collmsk, 16, sbit, ptr + 32,
                             sptr + 32, vic_ii.raster.sprite_status.sprites[n].color, cmsk);
            }
          else
            {
              if (double_size)
                SPRITE_MASK_2x (&vic_ii.raster, sprmsk, 0, 16, sbit, ptr + 64,
                                sptr + 32, vic_ii.raster.sprite_status.sprites[n].color, cmsk);
              else
                SPRITE_MASK (&vic_ii.raster, sprmsk, 0, 16, sbit, ptr + 32,
                             sptr + 32, vic_ii.raster.sprite_status.sprites[n].color, cmsk);
            }
          if (cmsk)
            vic_ii.raster.sprite_status.sprite_sprite_collisions
              |= (cmsk >> 8) | ((cmsk | sbit) & 0xff);
        }
      else
        {                       /* Unexpanded */
          BYTE sbit = 1 << n;
          BYTE cmsk = 0;

          collmsk = ((((msk_ptr[0] << 24) | (msk_ptr[1] << 16)
                       | (msk_ptr[2] << 8) | msk_ptr[3]) << lshift)
                     | (msk_ptr[4] >> (8 - lshift)));
          sprmsk = (data_ptr[0] << 16) | (data_ptr[1] << 8) | data_ptr[2];
          if (!vic_ii.idle_state && (sprmsk & collmsk) != 0)
            vic_ii.raster.sprite_status.sprite_background_collisions |= sbit;
          if (in_background)
            {
              if (double_size)
                SPRITE_MASK_2x (&vic_ii.raster, sprmsk, collmsk, 24, sbit, ptr, sptr,
                        vic_ii.raster.sprite_status.sprites[n].color, cmsk);
              else
                SPRITE_MASK (&vic_ii.raster, sprmsk, collmsk, 24, sbit, ptr, sptr,
                        vic_ii.raster.sprite_status.sprites[n].color, cmsk);
            }
          else
            {
              if (double_size)
                SPRITE_MASK_2x (&vic_ii.raster, sprmsk, 0, 24, sbit, ptr, sptr,
                        vic_ii.raster.sprite_status.sprites[n].color, cmsk);
              else
                SPRITE_MASK (&vic_ii.raster, sprmsk, 0, 24, sbit, ptr, sptr,
                        vic_ii.raster.sprite_status.sprites[n].color, cmsk);
            }
          if (cmsk)
            vic_ii.raster.sprite_status.sprite_sprite_collisions |= cmsk | sbit;
        }
    }
}

/* Draw one multicolor sprite.  */
inline static void 
draw_mc_sprite (PIXEL * line_ptr,
                BYTE * gfx_msk_ptr,
                int n,
                int double_size)
{
  if (vic_ii.raster.sprite_status.sprites[n].x < VIC_II_SCREEN_WIDTH)
    {
      DWORD sprmsk, mcsprmsk;
      BYTE *msk_ptr;
      PIXEL *ptr = line_ptr + vic_ii.raster.sprite_status.sprites[n].x *
      ((double_size) ? 2 : 1);
      BYTE *sptr = sprline + VIC_II_MAX_SPRITE_WIDTH + vic_ii.raster.sprite_status.sprites[n].x;
      BYTE *data_ptr = (BYTE *) (vic_ii.raster.sprite_status.sprite_data + n);
      int in_background = vic_ii.raster.sprite_status.sprites[n].in_background;
      BYTE cmsk = 0, sbit = 1 << n;
      int lshift = (vic_ii.raster.sprite_status.sprites[n].x - vic_ii.raster.xsmooth) & 0x7;
      DWORD c[4];

      c[1] = vic_ii.raster.sprite_status.mc_sprite_color_1;
      c[2] = vic_ii.raster.sprite_status.sprites[n].color;
      c[3] = vic_ii.raster.sprite_status.mc_sprite_color_2;
      msk_ptr = gfx_msk_ptr + ((vic_ii.raster.sprite_status.sprites[n].x + VIC_II_MAX_SPRITE_WIDTH
                                - vic_ii.raster.xsmooth) / 8);
      mcsprmsk = (data_ptr[0] << 16) | (data_ptr[1] << 8) | data_ptr[2];
      if (vic_ii.raster.sprite_status.sprites[n].x_expanded)
        {
          DWORD collmsk = ((((msk_ptr[1] << 24) | (msk_ptr[2] << 16)
                             | (msk_ptr[3] << 8) | msk_ptr[4]) << lshift)
                           | (msk_ptr[5] >> (8 - lshift)));

          sprmsk = sprite_doubling_table[((mcsprtable[data_ptr[0]] << 8)
                                          | mcsprtable[data_ptr[1]])];
          if (!vic_ii.idle_state && (sprmsk & collmsk) != 0)
            vic_ii.raster.sprite_status.sprite_background_collisions |= sbit;
          if (in_background)
            {
              if (double_size)
                MCSPRITE_DOUBLE_MASK_2x (&vic_ii.raster, mcsprmsk, collmsk, 32,
                                         sbit, ptr, sptr, c, cmsk);
              else
                MCSPRITE_DOUBLE_MASK (&vic_ii.raster, mcsprmsk, collmsk, 32,
                                      sbit, ptr, sptr, c, cmsk);
            }
          else
            {
              if (double_size)
                MCSPRITE_DOUBLE_MASK_2x (&vic_ii.raster, mcsprmsk, 0, 32,
                                         sbit, ptr, sptr, c, cmsk);
              else
                MCSPRITE_DOUBLE_MASK (&vic_ii.raster, mcsprmsk, 0, 32,
                                      sbit, ptr, sptr, c, cmsk);
            }
          sprmsk = sprite_doubling_table[mcsprtable[data_ptr[2]]];
          collmsk = ((((msk_ptr[5] << 8) | msk_ptr[6]) << lshift)
                     | (msk_ptr[7] >> (8 - lshift)));
          if (!vic_ii.idle_state && (sprmsk & collmsk) != 0)
            vic_ii.raster.sprite_status.sprite_background_collisions |= sbit;
          if (in_background)
            {
              if (double_size)
                MCSPRITE_DOUBLE_MASK_2x (&vic_ii.raster, mcsprmsk, collmsk, 16, sbit,
                                         ptr + 64, sptr + 32, c, cmsk);
              else
                MCSPRITE_DOUBLE_MASK (&vic_ii.raster, mcsprmsk, collmsk, 16, sbit,
                                      ptr + 32, sptr + 32, c, cmsk);
            }
          else
            {
              if (double_size)
                MCSPRITE_DOUBLE_MASK_2x (&vic_ii.raster, mcsprmsk, 0, 16, sbit,
                                         ptr + 64, sptr + 32, c, cmsk);
              else
                MCSPRITE_DOUBLE_MASK (&vic_ii.raster, mcsprmsk, 0, 16, sbit,
                                      ptr + 32, sptr + 32, c, cmsk);
            }
        }
      else
        {                       /* Unexpanded */
          DWORD collmsk = ((((msk_ptr[0] << 24) | (msk_ptr[1] << 16)
                             | (msk_ptr[2] << 8) | msk_ptr[3]) << lshift)
                           | (msk_ptr[4] >> (8 - lshift)));
          sprmsk = ((mcsprtable[data_ptr[0]] << 16)
                    | (mcsprtable[data_ptr[1]] << 8)
                    | mcsprtable[data_ptr[2]]);
          if (!vic_ii.idle_state && (sprmsk & collmsk) != 0)
            vic_ii.raster.sprite_status.sprite_background_collisions |= sbit;
          if (in_background)
            {
              if (double_size)
                MCSPRITE_MASK_2x (&vic_ii.raster, mcsprmsk, collmsk, 24, sbit, ptr,
                                  sptr, c, cmsk);
              else
                MCSPRITE_MASK (&vic_ii.raster, mcsprmsk, collmsk, 24, sbit, ptr,
                               sptr, c, cmsk);
            }
          else
            {
              if (double_size)
                MCSPRITE_MASK_2x (&vic_ii.raster, mcsprmsk, 0, 24, sbit, ptr,
                                  sptr, c, cmsk);
              else
                MCSPRITE_MASK (&vic_ii.raster, mcsprmsk, 0, 24, sbit, ptr,
                               sptr, c, cmsk);
            }
        }
      if (cmsk)
        vic_ii.raster.sprite_status.sprite_sprite_collisions |= cmsk | (sbit);
    }
}

inline static void 
draw_all_sprites (PIXEL * line_ptr, BYTE * gfx_msk_ptr)
{
  vic_ii.raster.sprite_status.sprite_sprite_collisions = 0;
  vic_ii.raster.sprite_status.sprite_background_collisions = 0;

  if (vic_ii.raster.sprite_status.dma_msk)
    {
      int n;

      memset (sprline, 0, sizeof (sprline));

      for (n = 0; n < 8; n++)
        {
          if (vic_ii.raster.sprite_status.dma_msk & (1 << n))
            {
              if (vic_ii.raster.sprite_status.sprites[n].multicolor)
                draw_mc_sprite (line_ptr, gfx_msk_ptr, n, 0);
              else
                draw_hires_sprite (line_ptr, gfx_msk_ptr, n, 0);
            }
        }

      vic_ii.sprite_sprite_collisions
        |= vic_ii.raster.sprite_status.sprite_sprite_collisions;
      vic_ii.sprite_background_collisions
        |= vic_ii.raster.sprite_status.sprite_background_collisions;
    }
}

#ifdef VIC_II_NEED_2X
inline static void 
draw_all_sprites_2x (PIXEL * line_ptr, BYTE * gfx_msk_ptr)
{
  vic_ii.raster.sprite_status.sprite_sprite_collisions = 0;
  vic_ii.raster.sprite_status.sprite_background_collisions = 0;

  if (vic_ii.raster.sprite_status.dma_msk)
    {
      int n;

      memset (sprline, 0, sizeof (sprline));

      for (n = 0; n < 8; n++)
        {
          if (vic_ii.raster.sprite_status.dma_msk & (1 << n))
            {
              if (vic_ii.raster.sprite_status.sprites[n].multicolor)
                draw_mc_sprite (line_ptr, gfx_msk_ptr, n, 1);
              else
                draw_hires_sprite (line_ptr, gfx_msk_ptr, n, 1);
            }
        }

      vic_ii.sprite_sprite_collisions
        |= vic_ii.raster.sprite_status.sprite_sprite_collisions;
      vic_ii.sprite_background_collisions
        |= vic_ii.raster.sprite_status.sprite_background_collisions;
    }
}
#endif /* VIC_II_NEED_2X */



void
vic_ii_sprites_init (void)
{
  init_drawing_tables ();

  vic_ii_sprites_set_double_size (0);

  return;
}

void
vic_ii_sprites_set_double_size (int enabled)
{
#ifdef VIC_II_NEED_2X
  if (enabled)
    raster_sprite_status_set_draw_function (&vic_ii.raster.sprite_status,
                                            draw_all_sprites_2x);
  else
#endif
    raster_sprite_status_set_draw_function (&vic_ii.raster.sprite_status,
                                            draw_all_sprites);
}



/* Set the X coordinate of the `num'th sprite to `new_x'; the current
   vic_ii.raster X position is `raster_x'.  */
void
vic_ii_sprites_set_x_position (unsigned int num,
                               int new_x,
                               int raster_x)
{
  raster_sprite_t *sprite;

  sprite = vic_ii.raster.sprite_status.sprites + num;

  new_x += 8;

  if (new_x > VIC_II_SPRITE_WRAP_X - VIC_II_MAX_SPRITE_WIDTH)
    {
      /* Sprites in the $1F8 - $1FF range are not visible at all and never
         cause collisions.  */
      if (new_x >= 0x1f8 + 8)
        new_x = VIC_II_SCREEN_WIDTH;
      else
        new_x -= VIC_II_SPRITE_WRAP_X;
    }

  if (new_x < sprite->x)
    {
      if (raster_x + 8 <= new_x)
        sprite->x = new_x;
      else if (raster_x + 8 < sprite->x)
        sprite->x = VIC_II_SCREEN_WIDTH;
      raster_add_int_change_next_line (&vic_ii.raster, &sprite->x, new_x);
    }
  else
    {                           /* new_x >= sprite->x */
      if (raster_x + 8 < sprite->x)
        sprite->x = new_x;
      raster_add_int_change_next_line (&vic_ii.raster, &sprite->x, new_x);
    }
}
