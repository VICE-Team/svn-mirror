/*
 * renderXv.c - X Video rendering.
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

/* Some of the Xv code is loosely based on testxv.c (by André Werthmann)
   and VideoLAN. */

#include "vice.h"

#ifdef HAVE_XVIDEO

#include "renderxv.h"

#include <stdio.h>
#include <string.h>

int find_yuv_port(Display* display, XvPortID* port, int* format)
{
  int i, j;

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

  /* Find YV12 capable adaptor. */
  for (i = 0; i < num_adaptors; i++) {
    if (!(adaptor_info[i].type & XvInputMask
	  && adaptor_info[i].type & XvImageMask))
    {
      continue;
    }

    format_list = XvListImageFormats(display, adaptor_info[i].base_id,
				     &num_formats);

    for (j = 0; j < num_formats; j++) {
      if (strcmp(format_list[j].guid, "YV12") != 0) {
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
	*format = format_list[j].id;
	XFree(format_list);
	XvFreeAdaptorInfo(adaptor_info);
	printf("Got port %ld, format 0x%x\n", *port, *format);
	return 1;
      }
    }

    XFree(format_list);
  }

  XvFreeAdaptorInfo(adaptor_info);
  printf("No Xv YV12 adaptor/port available.\n");
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
    printf("XvImage created (shared)\n");
  }
  else {
    if (!(image = XvCreateImage(display, port, format, NULL, width, height))) {
      printf("Unable to create XvImage\n");
      return NULL;
    }
    image->data = malloc(image->data_size);
    printf("XvImage created\n");
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


void render_yuv_image(XvImage* image,
		      unsigned char* src,
		      int src_pitch,
		      unsigned int* src_color,
		      int src_x, int src_y,
		      unsigned int src_w, unsigned int src_h,
		      int dest_x, int dest_y)
{
  int x, y;
  unsigned int YUV;
  char* Yptr = image->data + image->offsets[0];
  char* Vptr = image->data + image->offsets[1];
  char* Uptr = image->data + image->offsets[2];
  int Ypitch = image->pitches[0];
  int Vpitch = image->pitches[1];
  int Upitch = image->pitches[2];

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
  Vptr += Vpitch*dest_y/2 + dest_x/2;
  Uptr += Upitch*dest_y/2 + dest_x/2;
  src += src_pitch*src_y + src_x;

  /* Render 2x2 YV12 blocks. */
  for (y = 0; y < src_h; y += 2) {
    for (x = 0; x < src_w; x += 2) {
      YUV = src_color[*src++];
      *Yptr++ = YUV >> 16;
      *Vptr++ = YUV & 0xff;
      *Uptr++ = (YUV >> 8) & 0xff;
      YUV = src_color[*src++];
      *Yptr++ = YUV >> 16;
    }
    Yptr += Ypitch - src_w;
    Vptr += Vpitch - src_w/2;
    Uptr += Upitch - src_w/2;
    src += src_pitch - src_w;
    for (x = 0; x < src_w; x++) {
      YUV = src_color[*src++];
      *Yptr++ = YUV >> 16;
    }
    Yptr += Ypitch - src_w;
    src += src_pitch - src_w;
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

#endif /* HAVE_XVIDEO */
