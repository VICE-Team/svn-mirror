/*
 * sprites.h - Sprite drawing macros.
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

#define SPRITE_PIXEL(do_draw, sprite_bit, imgptr, collmskptr, pos,	\
		     color, collmsk_return)				\
  do {									\
      if ((do_draw) && (collmskptr)[(pos)] == 0)			\
	  (imgptr)[(pos)] = PIXEL((color));				\
      (collmsk_return) |= (collmskptr)[(pos)];				\
      (collmskptr)[(pos)] |= (sprite_bit);				\
  } while (0)

#ifdef ALLOW_UNALIGNED_ACCESS

#  define SPRITE_PIXEL_2x(do_draw, sprite_bit, imgptr, collmskptr, pos,	\
			  color, collmsk_return)			\
  do {									\
     if ((do_draw) && (collmskptr)[(pos)] == 0)				\
         *((PIXEL2 *)(imgptr) + (pos)) = PIXEL2((color));		\
     (collmsk_return) |= (collmskptr)[(pos)];				\
     (collmskptr)[(pos)] |= (sprite_bit);				\
  } while (0)

#else

#  define SPRITE_PIXEL_2x(do_draw, sprite_bit, imgptr, collmskptr, pos, \
			      color, collmsk_return)			    \
  do {									    \
     if ((do_draw) && (collmskptr)[(pos)] == 0)				    \
         (imgptr)[(pos) * 2] = (imgptr)[(pos) * 2 + 1] = PIXEL((color));    \
     (collmsk_return) |= (collmskptr)[pos];				    \
     (collmskptr)[pos] |= (sprite_bit);					    \
  } while (0)

#endif


/* Hires sprites */

#define _SPRITE_MASK(msk, gfxmsk, size, sprite_bit, imgptr, collmskptr,	\
		     color, collmsk_return, DRAW)			\
  do {									\
      DWORD __m;							\
      int __p;								\
									\
      for (__m = 1 << ((size) - 1), __p = 0;				\
	   __p < (size);						\
	   __p++, __m >>= 1)						\
	  if ((msk) & __m) {						\
              if ((gfxmsk) & __m)					\
	          DRAW(0, sprite_bit, imgptr, collmskptr, __p, color,	\
		       collmsk_return);					\
	      else							\
	          DRAW(1, sprite_bit, imgptr, collmskptr, __p, color,	\
		       collmsk_return);					\
	  }								\
  } while (0)


#define SPRITE_MASK(msk, gfxmsk, size, sprite_bit, imgptr, collmskptr,	\
		    color, collmsk_return)				\
  _SPRITE_MASK(msk, gfxmsk, size, sprite_bit, imgptr, collmskptr,	\
	       color, collmsk_return, SPRITE_PIXEL)

#define SPRITE_MASK_2x(msk, gfxmsk, size, sprite_bit, imgptr,		\
		       collmskptr, color, collmsk_return)		\
  _SPRITE_MASK(msk, gfxmsk, size, sprite_bit, imgptr, collmskptr,	\
	       color, collmsk_return, SPRITE_PIXEL_2x)


/* Multicolor sprites */

#define _MCSPRITE_MASK(mcmsk, gfxmsk, size, sprite_bit, imgptr,		\
		       collmskptr, pixel_table, collmsk_return, DRAW)	\
  do {									\
      DWORD __m;							\
      int __p;								\
									\
      for (__m = 1 << ((size) - 1), __p = 0; __p < (size);		\
	   __p += 2, __m >>= 2, (mcmsk) <<= 2) {			\
          BYTE __c = ((mcmsk) >> 22) & 0x3;				\
									\
  	  if (__c) {							\
              if ((gfxmsk) & __m)					\
	          DRAW(0, sprite_bit, imgptr, collmskptr, __p,		\
		       pixel_table[__c], collmsk_return);		\
	      else							\
	          DRAW(1, sprite_bit, imgptr, collmskptr, __p,		\
		       pixel_table[__c], collmsk_return);		\
	      if ((gfxmsk) & (__m >> 1))				\
		  DRAW(0, sprite_bit, imgptr, collmskptr, __p + 1,	\
		       pixel_table[__c], collmsk_return);		\
	      else							\
	          DRAW(1, sprite_bit, imgptr, collmskptr, __p + 1,	\
		       pixel_table[__c], collmsk_return);		\
	  }								\
      }									\
  } while (0)


#define MCSPRITE_MASK(mcmsk, gfxmsk, size, sprite_bit, imgptr,	\
		      collmskptr, pixel_table, collmsk_return)	\
  _MCSPRITE_MASK(mcmsk, gfxmsk, size, sprite_bit, imgptr,	\
		 collmskptr, pixel_table, collmsk_return,	\
		 SPRITE_PIXEL)

#define MCSPRITE_MASK_2x(mcmsk, gfxmsk, size, sprite_bit, imgptr,	\
			 collmskptr, pixel_table, collmsk_return)	\
  _MCSPRITE_MASK(mcmsk, gfxmsk, size, sprite_bit, imgptr,		\
		 collmskptr, pixel_table, collmsk_return,		\
		 SPRITE_PIXEL_2x)


#define _MCSPRITE_DOUBLE_MASK(mcmsk, gfxmsk, size, sprite_bit,	\
			      imgptr, collmskptr, pixel_table,	\
			      collmsk_return, DRAW)		\
  do {								\
      DWORD __m;						\
      int __p, __i;						\
								\
      for (__m = 1 << ((size) - 1), __p = 0; __p < (size);	\
	   __p += 4, (mcmsk) <<= 2) {				\
          BYTE __c = ((mcmsk) >> 22) & 0x3;			\
								\
	  for (__i = 0; __i < 4; __i++, __m >>= 1)		\
   	      if (__c) {					\
                  if ((gfxmsk) & __m)				\
	              DRAW(0, sprite_bit, imgptr, collmskptr,	\
			   __p + __i, pixel_table[__c],		\
			   collmsk_return);			\
	          else						\
	              DRAW(1, sprite_bit, imgptr, collmskptr,	\
			   __p + __i, pixel_table[__c],		\
			   collmsk_return);			\
	      }							\
      }								\
  } while (0)

#define MCSPRITE_DOUBLE_MASK(mcmsk, gfxmsk, size, sprite_bit, imgptr,	\
			     collmskptr, pixel_table, collmsk_return)	\
  _MCSPRITE_DOUBLE_MASK(mcmsk, gfxmsk, size, sprite_bit, imgptr,	\
			collmskptr, pixel_table, collmsk_return,	\
			SPRITE_PIXEL)

#define MCSPRITE_DOUBLE_MASK_2x(mcmsk, gfxmsk, size, sprite_bit,	\
				imgptr, collmskptr, pixel_table,	\
				collmsk_return)				\
  _MCSPRITE_DOUBLE_MASK(mcmsk, gfxmsk, size, sprite_bit, imgptr,	\
			collmskptr, pixel_table, collmsk_return,	\
			SPRITE_PIXEL_2x)
