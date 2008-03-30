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

/* The Xv probing and allocation code is loosely based on testxv.c
   (by André Werthmann) and VideoLAN. */

#include "vice.h"

#ifdef HAVE_XVIDEO

#include "renderxv.h"

#include <stdio.h>
#include <string.h>

void render_UYVY(XvImage* image,
		 unsigned char* src,
		 int src_pitch,
		 unsigned int* src_color,
		 int src_x, int src_y,
		 unsigned int src_w, unsigned int src_h,
		 int dest_x, int dest_y);

void render_YUY2(XvImage* image,
		 unsigned char* src,
		 int src_pitch,
		 unsigned int* src_color,
		 int src_x, int src_y,
		 unsigned int src_w, unsigned int src_h,
		 int dest_x, int dest_y);

void render_YVYU(XvImage* image,
		 unsigned char* src,
		 int src_pitch,
		 unsigned int* src_color,
		 int src_x, int src_y,
		 unsigned int src_w, unsigned int src_h,
		 int dest_x, int dest_y);

void render_YV12(XvImage* image,
		 unsigned char* src,
		 int src_pitch,
		 unsigned int* src_color,
		 int src_x, int src_y,
		 unsigned int src_w, unsigned int src_h,
		 int dest_x, int dest_y);

void render_I420(XvImage* image,
		 unsigned char* src,
		 int src_pitch,
		 unsigned int* src_color,
		 int src_x, int src_y,
		 unsigned int src_w, unsigned int src_h,
		 int dest_x, int dest_y);

xv_render_t render_list[] = {
  { "UYVY", render_UYVY },
  { "YUY2", render_YUY2 },
  { "YVYU", render_YVYU },
  { "YV12", render_YV12 },
  { "I420", render_I420 },
  { "IYUV", render_I420 }, /* IYUV is a duplicate of I420. */
};

int find_yuv_port(Display* display, XvPortID* port, int* format,
		  xv_render_t* render)
{
  int i, j, k;

  /* XvQueryExtension */
  unsigned int version, release, request_base, event_base, error_base;

  /* XvQueryAdaptors */
  int num_adaptors;
  XvAdaptorInfo* adaptor_info = NULL;
  XvPortID port_id;

  /* XvListImageFormats */
  int num_formats;
  XvImageFormatValues* format_list = NULL;

  switch (XvQueryExtension(display, &version, &release,
			   &request_base, &event_base, &error_base))
  {
  case Success:
    break;
  case XvBadExtension:
    printf("XvQueryExtension returned XvBadExtension.\n");
    return 0;
  case XvBadAlloc:
    printf("XvQueryExtension returned XvBadAlloc.\n");
    return 0;
  default:
    printf("XvQueryExtension returned unknown error.\n");
    return 0;
  }

  switch (XvQueryAdaptors(display, DefaultRootWindow(display),
			  &num_adaptors, &adaptor_info))
  {
  case Success:
    break;
  case XvBadExtension:
    printf("XvQueryAdaptors returned XvBadExtension.\n");
    return 0;
  case XvBadAlloc:
    printf("XvQueryAdaptors returned XvBadAlloc.\n");
    return 0;
  default:
    printf("XvQueryAdaptors returned unknown error.\n");
    return 0;
  }

  /* Find YUV capable adaptor. */
  for (i = 0; i < num_adaptors; i++) {
    if (!(adaptor_info[i].type & XvInputMask
	  && adaptor_info[i].type & XvImageMask))
    {
      continue;
    }

    format_list = XvListImageFormats(display, adaptor_info[i].base_id,
				     &num_formats);

    for (j = 0; j < sizeof(render_list)/sizeof(*render_list); j++) {
      for (k = 0; k < num_formats; k++) {
	if (strcmp(format_list[k].guid, render_list[j].format) != 0) {
	  continue;
	}

	for (port_id = adaptor_info[i].base_id;
	     port_id < adaptor_info[i].base_id + adaptor_info[i].num_ports;
	     port_id++)
	  {
	    if (XvGrabPort(display, port_id, CurrentTime) != Success) {
	      continue;
	    }
	    *port = port_id;
	    *format = format_list[k].id;
	    *render = render_list[j];
	    XFree(format_list);
	    XvFreeAdaptorInfo(adaptor_info);
	    return 1;
	  }
      }
    }

    XFree(format_list);
  }

  XvFreeAdaptorInfo(adaptor_info);
  printf("No suitable Xv YUV adaptor/port available.\n");
  return 0;
}


XvImage* create_yuv_image(Display* display, XvPortID port, int format,
			  int width, int height, XShmSegmentInfo* shminfo)
{
  XvImage* image;

  if (shminfo) {
    if (!(image = XvShmCreateImage(display, port, format, NULL, width, height,
				   shminfo)))
    {
      printf("Unable to create shm XvImage\n");
      return NULL;
    }
      
    if ((shminfo->shmid = shmget(IPC_PRIVATE, image->data_size, IPC_CREAT | 0777)) == -1)
    {
      printf("Unable to allocate shared memory\n");
      XFree(image);
      return NULL;
    }
    if (!(shminfo->shmaddr = shmat(shminfo->shmid, 0, 0))) {
      printf("Unable to attach shared memory\n");
      XFree(image);
      shmctl(shminfo->shmid, IPC_RMID, 0);
      return NULL;
    }
    shminfo->readOnly = False;

    image->data = shminfo->shmaddr;

    if (!XShmAttach(display, shminfo)) {
      printf("XShmAttach failed\n");
      XFree(image);
      shmctl(shminfo->shmid, IPC_RMID, 0);
      shmdt(shminfo->shmaddr);
      return NULL;
    }

    /* Send image to X server. This instruction is required, since having
     * built a Shm XImage and not using it causes an error on XCloseDisplay. */
    XSync(display, False);

    /* Mark the segment to be automatically removed when the last
       attachment is broken (i.e. on shmdt or process exit). */
    shmctl(shminfo->shmid, IPC_RMID, 0);
  }
  else {
    if (!(image = XvCreateImage(display, port, format, NULL, width, height))) {
      printf("Unable to create XvImage\n");
      return NULL;
    }
    image->data = malloc(image->data_size);
  }

  return image;
}


void destroy_yuv_image(Display* display, XvImage* image,
		       XShmSegmentInfo* shminfo)
{
  if (shminfo) {
    XShmDetach(display, shminfo);
    XFree(image);
    shmdt(shminfo->shmaddr);
  }
  else {
    XFree(image);
  }
}


void display_yuv_image(Display* display, XvPortID port, Drawable d, GC gc,
		       XvImage* image,
		       XShmSegmentInfo* shminfo,
		       int src_x, int src_y,
		       unsigned int src_w, unsigned int src_h,
		       unsigned int dest_w, unsigned int dest_h)
{
  int dest_x = 0, dest_y = 0;

  /* Keep aspect ratio of src image. */
  if (dest_w*src_h < src_w*dest_h) {
    dest_y = dest_h;
    dest_h = dest_w*src_h/src_w;
    dest_y = (dest_y - dest_h)/2;
  }
  else {
    dest_x = dest_w;
    dest_w = dest_h*src_w/src_h;
    dest_x = (dest_x - dest_w)/2;
  }

  if (shminfo) {
    XvShmPutImage(display, port, d, gc, image,
		  src_x, src_y, src_w, src_h,
		  dest_x, dest_y, dest_w, dest_h, False);
  }
  else {
    XvPutImage(display, port, d, gc, image,
	       src_x, src_y, src_w, src_h,
	       dest_x, dest_y, dest_w, dest_h);
  }
}


/* Render packed YUV 4:2:2 formats. */
void render_4_2_2(XvImage* image,
		  int shift_y0, int shift_u, int shift_v, int shift_y1,
		  unsigned char* src,
		  int src_pitch,
		  unsigned int* src_color,
		  int src_x, int src_y,
		  unsigned int src_w, unsigned int src_h,
		  int dest_x, int dest_y)
{
  int x, y;
  unsigned int YUV0, YUV1;
  unsigned int U, V;
  unsigned int* dest = (unsigned int*)(image->data + image->offsets[0]);
  int dest_pitch = image->pitches[0]/4;

  /* Normalize to 2x1 blocks. */
  if (dest_x & 1) {
    dest_x--;
    src_x--;
    src_w++;
  }
  if (src_w & 1) {
    src_w++;
  }

  /* Add start offsets. */
  dest += dest_pitch*dest_y + (dest_x >> 1);
  src += src_pitch*src_y + src_x;

  /* Render 2x1 blocks, YUV 4:2:2 */
  for (y = 0; y < src_h; y++) {
    for (x = 0; x < src_w; x += 2) {
      YUV0 = src_color[*src++];
      YUV1 = src_color[*src++];
      U = (((YUV0 >> 8) & 0xff) + ((YUV1 >> 8) & 0xff)) >> 1;
      V = ((YUV0 & 0xff) + (YUV1 & 0xff)) >> 1;
      *dest++ =
	((YUV0 >> 16) << shift_y0)
	| (U << shift_u)
	| (V << shift_v)
	| ((YUV1 >> 16) << shift_y1);
    }
    src += src_pitch - src_w;
    dest += dest_pitch - (src_w >> 1);
  }
}


/* Render planar YUV 4:1:1 formats. */
void render_4_1_1(XvImage* image,
		  int plane_y, int plane_u, int plane_v,
		  unsigned char* src,
		  int src_pitch,
		  unsigned int* src_color,
		  int src_x, int src_y,
		  unsigned int src_w, unsigned int src_h,
		  int dest_x, int dest_y)
{
  int x, y;
  unsigned int YUV0, YUV1, YUV2, YUV3;
  unsigned char* Yptr = image->data + image->offsets[plane_y];
  unsigned char* Uptr = image->data + image->offsets[plane_u];
  unsigned char* Vptr = image->data + image->offsets[plane_v];
  int Ypitch = image->pitches[plane_y];
  int Upitch = image->pitches[plane_u];
  int Vpitch = image->pitches[plane_v];

  /* Normalize to 2x2 blocks. */
  if (dest_x & 1) {
    dest_x--;
    src_x--;
    src_w++;
  }
  if (dest_y & 1) {
    dest_y--;
    src_y--;
    src_h++;
  }
  if (src_w & 1) {
    src_w++;
  }
  if (src_h & 1) {
    src_h++;
  }

  /* Add start offsets. */
  Yptr += Ypitch*dest_y + dest_x;
  Uptr += (Upitch*dest_y + dest_x) >> 1;
  Vptr += (Vpitch*dest_y + dest_x) >> 1;
  src += src_pitch*src_y + src_x;

  /* Render 2x2 blocks, YUV 4:1:1 */
  for (y = 0; y < src_h; y += 2) {
    for (x = 0; x < src_w; x += 2) {
      YUV0 = src_color[*src];
      YUV1 = src_color[*(src + 1)];
      YUV2 = src_color[*(src + src_pitch)];
      YUV3 = src_color[*(src + src_pitch + 1)];
      src += 2;
      *Yptr = YUV0 >> 16;
      *(Yptr + 1) = YUV1 >> 16;
      *(Yptr + Ypitch) = YUV2 >> 16;
      *(Yptr + Ypitch + 1) = YUV3 >> 16;
      Yptr += 2;
      *Uptr++ = (((YUV0 >> 8) & 0xff) + ((YUV1 >> 8) & 0xff)
		 + ((YUV2 >> 8) & 0xff) + ((YUV3 >> 8) & 0xff)) >> 2;
      *Vptr++ = ((YUV0 & 0xff) + (YUV1 & 0xff)
		 + (YUV2 & 0xff) + (YUV3 & 0xff)) >> 2;
    }
    src += (src_pitch << 1) - src_w;
    Yptr += (Ypitch << 1) - src_w;
    Uptr += Upitch - (src_w >> 1);
    Vptr += Vpitch - (src_w >> 1);
  }
}


void render_UYVY(XvImage* image,
		 unsigned char* src,
		 int src_pitch,
		 unsigned int* src_color,
		 int src_x, int src_y,
		 unsigned int src_w, unsigned int src_h,
		 int dest_x, int dest_y)
{
#ifdef WORDS_BIGENDIAN
  render_4_2_2(image, 16, 24, 8, 0, src, src_pitch, src_color,
	       src_x, src_y, src_w, src_h, dest_x, dest_y);
#else
  render_4_2_2(image, 8, 0, 16, 24, src, src_pitch, src_color,
	       src_x, src_y, src_w, src_h, dest_x, dest_y);
#endif
}


void render_YUY2(XvImage* image,
		 unsigned char* src,
		 int src_pitch,
		 unsigned int* src_color,
		 int src_x, int src_y,
		 unsigned int src_w, unsigned int src_h,
		 int dest_x, int dest_y)
{
#ifdef WORDS_BIGENDIAN
  render_4_2_2(image, 24, 16, 0, 8, src, src_pitch, src_color,
	       src_x, src_y, src_w, src_h, dest_x, dest_y);
#else
  render_4_2_2(image, 0, 8, 24, 16, src, src_pitch, src_color,
	       src_x, src_y, src_w, src_h, dest_x, dest_y);
#endif
}


void render_YVYU(XvImage* image,
		 unsigned char* src,
		 int src_pitch,
		 unsigned int* src_color,
		 int src_x, int src_y,
		 unsigned int src_w, unsigned int src_h,
		 int dest_x, int dest_y)
{
#ifdef WORDS_BIGENDIAN
  render_4_2_2(image, 24, 0, 16, 8, src, src_pitch, src_color,
	       src_x, src_y, src_w, src_h, dest_x, dest_y);
#else
  render_4_2_2(image, 0, 24, 8, 16, src, src_pitch, src_color,
	       src_x, src_y, src_w, src_h, dest_x, dest_y);
#endif
}


void render_YV12(XvImage* image,
		 unsigned char* src,
		 int src_pitch,
		 unsigned int* src_color,
		 int src_x, int src_y,
		 unsigned int src_w, unsigned int src_h,
		 int dest_x, int dest_y)
{
  render_4_1_1(image, 0, 2, 1, src, src_pitch, src_color,
	       src_x, src_y, src_w, src_h, dest_x, dest_y);
}


void render_I420(XvImage* image,
		 unsigned char* src,
		 int src_pitch,
		 unsigned int* src_color,
		 int src_x, int src_y,
		 unsigned int src_w, unsigned int src_h,
		 int dest_x, int dest_y)
{
  render_4_1_1(image, 0, 1, 2, src, src_pitch, src_color,
	       src_x, src_y, src_w, src_h, dest_x, dest_y);
}

#endif /* HAVE_XVIDEO */
