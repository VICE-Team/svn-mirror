/*
 * renderxv.c - XVideo rendering.
 *
 * Written by
 *  Dag Lem <resid@nimrod.no>
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

#ifndef _RENDERXV_H
#define _RENDERXV_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>

/* A FOURCC consists of four bytes that can be interpreted either as
   a four-character string or as a four-byte integer. */
typedef union {
  int id;
  char label[4];
} fourcc_t;

/* YUV formats in preferred order. Since the order of the four bytes
   is fixed, the integer representation of the FOURCC's is different
   on little and big endian platforms. */
#ifdef WORDS_BIGENDIAN

/* YUV 4:2:2 formats: */
#define FOURCC_UYVY 0x55595659
#define FOURCC_YUY2 0x59555932
#define FOURCC_YVYU 0x59565955
/* YUV 4:1:1 formats: */
#define FOURCC_YV12 0x59563132
#define FOURCC_I420 0x49343230
#define FOURCC_IYUV 0x49595556

#else

/* YUV 4:2:2 formats: */
#define FOURCC_UYVY 0x59565955
#define FOURCC_YUY2 0x32595559
#define FOURCC_YVYU 0x55595659
/* YUV 4:1:1 formats: */
#define FOURCC_YV12 0x32315659
#define FOURCC_I420 0x30323449
#define FOURCC_IYUV 0x56555949

#endif


int find_yuv_port(Display* display, XvPortID* port, fourcc_t* format);

XvImage* create_yuv_image(Display* display, XvPortID port, fourcc_t format,
			  int width, int height, XShmSegmentInfo* shminfo);

void destroy_yuv_image(Display* display, XvImage* image,
		       XShmSegmentInfo* shminfo);

void render_yuv_image(int double_size,
		      int double_scan,
		      int pal_mode,
		      fourcc_t format,
		      XvImage* image,
		      unsigned char* src,
		      int src_pitch,
		      unsigned int* src_color,
		      int src_x, int src_y,
		      unsigned int src_w, unsigned int src_h,
		      int dest_x, int dest_y);

void display_yuv_image(Display* display, XvPortID port, Drawable d, GC gc,
		       XvImage* image,
		       XShmSegmentInfo* shminfo,
		       int src_x, int src_y,
		       unsigned int src_w, unsigned int src_h,
		       unsigned int dest_w, unsigned int dest_h);

#endif /* _RENDERXV_H */
