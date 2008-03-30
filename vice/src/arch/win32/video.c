/*
 * video.c - Video implementation for Win32, using DirectDraw.
 *
 * Written by
 *  Ettore Perazzoli    (ettore@comm2000.it)
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

#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

#include "video.h"

#include "ui.h"
#include "utils.h"
#include "resources.h"
#include "cmdline.h"

/* Main DirectDraw object.  */
LPDIRECTDRAW dd;

/* ------------------------------------------------------------------------ */

/* #define DEBUG_VIDEO */

/* Debugging stuff.  */
#ifdef DEBUG_VIDEO
static void video_debug(const char *format, ...)
{
        char tmp[1024];
        va_list args;

        va_start(args, format);
        vsprintf(tmp, format, args);
        va_end(args);
        OutputDebugString(tmp);
}
#define DEBUG(x) video_debug x
#else
#define DEBUG(x)
#endif

/* ------------------------------------------------------------------------ */

/* Video-related resources.  */

/* Flag: are we in fullscreen mode?  */
int fullscreen_enabled;

static resource_t resources[] = {
    { NULL }
};

int video_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------ */

/* Video-related command-line options.  */

static cmdline_option_t cmdline_options[] = {
    { NULL }
};

int video_init_cmdline_options(void)
{
    return 0;
}

/* ------------------------------------------------------------------------ */

/* DirectDraw errors.  This is shamelessly copied from `win32.c' in UAE,
   althouigh some values were missing there and I wrote my own.  */
static const char *dd_error(HRESULT ddrval)
{
    switch (ddrval) {
      case DDERR_NODIRECTDRAWHW:
        return "No DirectDraw hardware available";
      case DDERR_NOEMULATION:
        return "No emulation layer available";
      case DDERR_NOEXCLUSIVEMODE:
        return "No exclusive mode";
      case DDERR_NOFLIPHW:
        return "No flipping hardware";
      case DDERR_NOZBUFFERHW:
        return "No ZBuffer hardware";
      case DDERR_NOCOOPERATIVELEVELSET:
        return "No cooperative level set";
      case DDERR_ALREADYINITIALIZED:
        return "This object is already initialized.";
      case DDERR_BLTFASTCANTCLIP:
        return "Cannot use BLTFAST with Clipper attached to surface.";
      case DDERR_CANNOTATTACHSURFACE:
        return "Cannot attach surface.";
      case DDERR_CANNOTDETACHSURFACE:
        return "Cannot detach surface.";
      case DDERR_CANTCREATEDC:
        return "Cannot create DC Device Context.";
      case DDERR_CANTDUPLICATE:
        return "Cannot duplicate.";
      case DDERR_CANTLOCKSURFACE:
        return "Access to surface refused (no DCI provider).";
      case DDERR_CANTPAGELOCK:
        return "PageLock failure.";
      case DDERR_CANTPAGEUNLOCK:
        return "PageUnlock failure.";
      case DDERR_CLIPPERISUSINGHWND:
        return "Can't set a clip-list for a Clipper which is attached to an HWND.";
      case DDERR_COLORKEYNOTSET:
        return "No source colour-key provided.";
      case DDERR_CURRENTLYNOTAVAIL:
        return "Support unavailable.";
      case DDERR_DCALREADYCREATED:
        return "Surface already has a Device Context.";
      case DDERR_DIRECTDRAWALREADYCREATED:
        return "DirectDraw already bound to this process.";
      case DDERR_EXCEPTION:
        return "Unexpected exception.";
      case DDERR_EXCLUSIVEMODEALREADYSET:
        return "Already in exclusive mode.";
      case DDERR_GENERIC:
        return "Undefined";
      case DDERR_HEIGHTALIGN:
        return "Height needs to be aligned.";
      case DDERR_HWNDALREADYSET:
        return "HWND already set for cooperative level.";
      case DDERR_HWNDSUBCLASSED:
        return "HWND has been subclassed.";
      case DDERR_IMPLICITLYCREATED:
        return "Can't restore an implicitly created surface.";
      case DDERR_INCOMPATIBLEPRIMARY:
        return "New params doesn't match existing primary surface.";
      case DDERR_INVALIDCAPS:
        return "Device doesn't have requested capabilities.";
      case DDERR_INVALIDCLIPLIST:
        return "Provided clip-list not supported.";
      case DDERR_INVALIDDIRECTDRAWGUID:
        return "Invalid GUID.";
      case DDERR_INVALIDMODE:
        return "Mode not supported.";
      case DDERR_INVALIDOBJECT:
        return "Invalid object.";
      case DDERR_INVALIDPARAMS:
        return "Invalid params.";
      case DDERR_INVALIDPIXELFORMAT:
        return "Device doesn't support requested pixel format.";
      case DDERR_INVALIDPOSITION:
        return "Overlay position illegal.";
      case DDERR_INVALIDRECT:
        return "Invalid RECT.";
      case DDERR_INVALIDSURFACETYPE:
        return "Wrong type of surface.";
      case DDERR_LOCKEDSURFACES:
        return "Surface locked.";
      case DDERR_NO3D:
        return "No 3d capabilities.";
      case DDERR_NOALPHAHW:
        return "No alpha h/w.";
      case DDERR_NOBLTHW:
        return "No blit h/w.";
      case DDERR_NOCLIPLIST:
        return "No clip-list.";
      case DDERR_NOCLIPPERATTACHED:
        return "No Clipper attached.";
      case DDERR_NOCOLORCONVHW:
        return "No colour-conversion h/w.";
      case DDERR_NOCOLORKEY:
        return "No colour-key.";
      case DDERR_NOTLOCKED:
        return "Not locked.";
      case DDERR_NOTPAGELOCKED:
        return "Not page-locked.";
      case DDERR_NOTPALETTIZED:
        return "Not palette-based.";
      case DDERR_OUTOFCAPS:
        return "out of caps";
      case DDERR_OUTOFMEMORY:
        return "Out of memory.";
      case DDERR_OUTOFVIDEOMEMORY:
        return "out of video memory.";
      case DDERR_PALETTEBUSY:
        return "Palette busy.";
      case DDERR_PRIMARYSURFACEALREADYEXISTS:
        return "Already a primary surface.";
      case DDERR_SURFACEBUSY:
        return "Surface busy.";
        /*case DDERR_SURFACEOBSCURED:     return "Surface is obscured."; */
      case DDERR_SURFACELOST:
        return "Surface lost.";
      case DDERR_UNSUPPORTED:
        return "Unsupported.";
      case DDERR_UNSUPPORTEDMODE:
        return "Unsupported mode.";
      case DDERR_UNSUPPORTEDFORMAT:
        return "Unsupported format.";
      case DDERR_WASSTILLDRAWING:
        return "Was still drawing.";
    }
    return "???";
}

/* ------------------------------------------------------------------------ */

/* Initialization.  */
int video_init(void)
{
    HRESULT ddresult;

    /* Create the DirectDraw object.  */
    ddresult = DirectDrawCreate(NULL, &dd, NULL);
    if (ddresult != DD_OK)
        return -1;

    /* FIXME: We have to change this afterwards.  */
    ddresult = IDirectDraw_SetCooperativeLevel(dd, ui_get_main_hwnd(), DDSCL_NORMAL);
    if (ddresult != DD_OK) {
        ui_error("Cannot set DirectDraw cooperative level:\n%s",
                 dd_error(ddresult));
        return -1;
    }

    DEBUG(("DirectDraw successfully initialized.\n"));
    return 0;
}

/* ------------------------------------------------------------------------ */

/* Frame buffer functions.  */

#if 0

/* This is how it would look like if DirectX allowed to create off-screen
   surfaces that are larger than the screen itself...  Unluckily, it
   doesn't.  */

/* Lock the DD surface of buffer `f', filling the `dd_surface_desc'
   struct.  */
static int frame_buffer_lock(frame_buffer_t f)
{
    while (1) {
        HRESULT result;

        result = IDirectDrawSurface_Lock(f->dd_surface, NULL,
                                         &f->dd_surface_desc, 0, NULL);
        if (result == DD_OK)
            break;
        if (result == DDERR_SURFACELOST) {
            DEBUG(("Surface lost while locking frame buffer!\n"));
            if (IDirectDrawSurface_Restore(f->dd_surface) != DD_OK) {
                DEBUG(("Cannot restore surface: %s\n", dd_error(result)));
                return -1;
            }
            /* FIXME: Should clean up and maybe re-alloc colors?  */
        } else if (result != DDERR_WASSTILLDRAWING) {
            DEBUG(("Cannot lock surface: %s\n", dd_error(result)));
            return -1;
        }
    }

    /* Done.  */
    return 0;
}

int frame_buffer_alloc(frame_buffer_t *i, unsigned int width,
                       unsigned int height)
{
    HRESULT result;
    DDSURFACEDESC desc;
    LPDIRECTDRAWSURFACE surface = NULL;

    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
    desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    desc.dwWidth = 640;
    desc.dwHeight = 480;

    result = IDirectDraw_CreateSurface(dd, &desc, &surface, NULL);
    if (result != DD_OK) {
        DEBUG(("Cannot create DirectDrawSurface: %s\n", dd_error(result)));
        return -1;
    }

    *i = (struct _frame_buffer *) xmalloc(sizeof(struct _frame_buffer));
    (*i)->dd_surface = surface;

    return frame_buffer_lock(*i);
}

#else

/* This is the real version...  Without using DirectDrawSurfaces.  */

int frame_buffer_alloc(frame_buffer_t *f,
                       unsigned int width,
                       unsigned int height)
{
    unsigned int i;

    DEBUG(("frame_buffer_alloc()\n"));
    *f = (frame_buffer_t) xmalloc(sizeof(struct _frame_buffer));
    (*f)->width = width;
    (*f)->height = height;
    (*f)->lines = (PIXEL **) xmalloc(height * sizeof(PIXEL *));
    for (i = 0; i < height; i++)
        (*f)->lines[i] = (PIXEL *) xmalloc(width * sizeof(PIXEL));

    DEBUG(("Allocated frame buffer, %d x %d pixels.\n", width, height));

    return 0;
}

#endif

void frame_buffer_free(frame_buffer_t *f)
{
    int i;

    for (i = 0; i < (*f)->height; i++)
        free((*f)->lines[i]);
    free(*f);

}

void frame_buffer_clear(frame_buffer_t *f, BYTE value)
{
    int i;

    for (i = 0; i < (*f)->height; i++)
        memset((*f)->lines[i], value, (*f)->width);
}

/* ------------------------------------------------------------------------ */

/* Canvas functions.  */

/* Set the palettes for canvas `c'.  */
/* static FIXME */ int set_palette(canvas_t c)
{
    if (c->depth == 8) {
        HRESULT result;

        /* FIXME: Surface lost errors?  */
        result = IDirectDrawSurface_SetPalette(c->primary_surface,
                                               c->dd_palette);
        if (result == DDERR_SURFACELOST) {
            IDirectDrawSurface_Restore(c->primary_surface);
            result = IDirectDrawSurface_SetPalette(c->primary_surface,
                                                   c->dd_palette);
        }
        if (result != DD_OK) {
            ui_error("Cannot set palette on primary DirectDraw surface:\n%s",
                     dd_error(result));
            return -1;
        }

#if 0
        result = IDirectDrawSurface_SetPalette(c->temporary_surface,
                                               c->dd_palette);
        if (result == DDERR_SURFACELOST) {
            IDirectDrawSurface_Restore(c->temporary_surface);
            result = IDirectDrawSurface_SetPalette(c->temporary_surface,
                                                   c->dd_palette);
        }
        if (result != DD_OK) {
            DEBUG(("Cannot set palette on temporary surface: %s\n",
                   dd_error(result)));
            return -1;
        }

        if (c->back_surface != NULL) {
            result = IDirectDrawSurface_SetPalette(c->back_surface,
                                                   c->dd_palette);
            if (result == DDERR_SURFACELOST) {
                IDirectDrawSurface_Restore(c->back_surface);
                result = IDirectDrawSurface_SetPalette(c->back_surface,
                                                       c->dd_palette);
            }
            if (result != DD_OK) {
                DEBUG(("Cannot set palette on back surface: %s\n",
                       dd_error(result)));
                return -1;
            }
        }
#endif
    }
}

static int set_physical_colors(canvas_t c)
{
    HDC hdc;
    DDSURFACEDESC ddsd;
    int i;
    HRESULT result;

    for (i = 0; i < c->palette->num_entries; i++) {
        COLORREF rgb = RGB(c->palette->entries[i].red,
                           c->palette->entries[i].green,
                           c->palette->entries[i].blue);
        DWORD p;

        DEBUG(("Allocating color \"%s\"\n",
               c->palette->entries[i].name));
        result = IDirectDrawSurface_GetDC(c->temporary_surface,
                                          &hdc);
        if (result == DDERR_SURFACELOST) {
            IDirectDrawSurface_Restore(c->temporary_surface);
            result = IDirectDrawSurface_GetDC(c->temporary_surface,
                                              &hdc);
        }
        if (result != DD_OK) {
            ui_error("Cannot get DC on DirectDraw surface while allocating colors:\n%s",
                     dd_error(result));
            return -1;
        }
        SetPixel(hdc, 0, 0, PALETTERGB(c->palette->entries[i].red,
                                       c->palette->entries[i].green,
                                       c->palette->entries[i].blue));
        IDirectDrawSurface_ReleaseDC(c->temporary_surface, &hdc);

        ddsd.dwSize = sizeof(ddsd);
        while ((result
                = IDirectDrawSurface_Lock(c->temporary_surface,
                                          NULL, &ddsd, 0,
                                          NULL))
               == DDERR_WASSTILLDRAWING)
            ;
        if (result == DDERR_SURFACELOST) {
            IDirectDrawSurface_Restore(c->temporary_surface);
            result = IDirectDrawSurface_Lock(c->temporary_surface,
                                             NULL, &ddsd, 0,
                                             NULL);
        }
        if (result != DD_OK) {
            ui_error("Cannot lock temporary surface:\n%s",
                     dd_error(result));
            return -1;
        }

        p = *(DWORD *)ddsd.lpSurface;
        c->physical_colors[i] = p;
        if (c->depth == 8)
            c->pixels[i] = (BYTE) p & 0xff;
        else
            c->pixels[i] = i;
        DEBUG(("Pixel return %d 0x%02X\n", i, c->pixels[i]));
        if (IDirectDrawSurface_Unlock(c->temporary_surface, NULL)
            == DDERR_SURFACELOST) {
            IDirectDrawSurface_Restore(c->temporary_surface);
            IDirectDrawSurface_Unlock(c->temporary_surface, NULL);
        }
    }
}

/* Create a `canvas_t' with tile `win_name', of widht `*width' x `*height'
   pixels, exposure handler callback `exposure_handler' and palette
   `palette'.  If specified width/height is not possible, return an
   alternative in `*width' and `*height'; return the pixel values for the
   requested palette in `pixel_return[]'.  */
canvas_t canvas_create(const char *title, unsigned int *width,
                       unsigned int *height, int mapped,
                       canvas_redraw_t exposure_handler,
                       const palette_t *palette, PIXEL *pixel_return)
{
    HRESULT result;
    DDSURFACEDESC desc;
    canvas_t c;
    int i;

    c = xmalloc(sizeof(struct _canvas));
    memset(c, 0, sizeof(struct _canvas));

    /* "Normal" window stuff.  */
    c->title = stralloc(title);
    c->width = *width;
    c->height = *height;
    c->exposure_handler = exposure_handler;
    c->palette = palette;
    c->hwnd = ui_open_canvas_window(title, *width, *height, exposure_handler);

    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_CAPS;
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    result = IDirectDraw_CreateSurface(dd, &desc, &c->primary_surface, NULL);
    if (result != DD_OK) {
        DEBUG(("Cannot create primary surface: %s\n", dd_error(result)));
        goto error;
    }

    result = IDirectDraw_CreateClipper(dd, 0, &c->clipper, NULL);
    if (result != DD_OK) {
        ui_error("Cannot create clipper for primary surface:\n%s",
                 dd_error(result));
        goto error;
    }
    result = IDirectDrawClipper_SetHWnd(c->clipper, 0, c->hwnd);
    if (result != DD_OK) {
        ui_error("Cannot set HWND for primary surface clipper:\n%s",
                 dd_error(result));
        goto error;
    }
    result = IDirectDrawSurface_SetClipper(c->primary_surface, c->clipper);
    if (result != DD_OK) {
        ui_error("Cannot set clipper for primary surface:\n%s",
                 dd_error(result));
        goto error;
    }

    /* For now, the back surface is always NULL because we have not
       implemented the full-screen mode yet.  */
    c->back_surface = NULL;

    /* Create the temporary surface.  */
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    /* FIXME: SYSTEMMEMORY?  */
    desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    desc.dwWidth = GetSystemMetrics(SM_CXSCREEN);
    desc.dwHeight = GetSystemMetrics(SM_CYSCREEN);
    result = IDirectDraw_CreateSurface(dd, &desc, &c->temporary_surface, NULL);
    if (result != DD_OK) {
        ui_error("Cannot create temporary DirectDraw surface:\n%s",
                 dd_error(result));
        goto error;
    }

    IDirectDrawSurface_GetCaps(c->temporary_surface, &desc.ddsCaps);
    DEBUG(("Allocated working surface in %s memory successfully.\n",
           (desc.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY
            ? "system"
            : (desc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY
               ? "video"
               : "unknown"))));

    /* Find the color depth.  */
    {
        HDC hdc = GetDC(NULL);

        c->depth = GetDeviceCaps(hdc, PLANES) * GetDeviceCaps(hdc,
                                                              BITSPIXEL);
        ReleaseDC(NULL, hdc);
        DEBUG(("Initializing canvas at %d bits per pixel.\n", c->depth));
    }

    /* Create palette.  */
    if (c->depth == 8) {
        PALETTEENTRY ape[256];
        HRESULT result;

#if 1
        /* Default to a 332 palette.  */
        for (i = 0; i < 256; i++) {
            ape[i].peRed   = (BYTE)(((i >> 5) & 0x07) * 255 / 7);
            ape[i].peGreen = (BYTE)(((i >> 2) & 0x07) * 255 / 7);
            ape[i].peBlue  = (BYTE)(((i >> 0) & 0x03) * 255 / 3);
            ape[i].peFlags = (BYTE)0;
        }
#else
        {
            HDC hdc = GetDC(NULL);
            GetSystemPaletteEntries(hdc, 0, 256, ape);
            ReleaseDC(NULL, hdc);
        }
#endif

        /* Overwrite first colors with the palette ones.  */
        for (i = 0; i < c->palette->num_entries; i++) {
            ape[i].peRed = c->palette->entries[i].red;
            ape[i].peGreen = c->palette->entries[i].green;
            ape[i].peBlue = c->palette->entries[i].blue;
            ape[i].peFlags = 0;
        }

        result = IDirectDraw_CreatePalette(dd, DDPCAPS_8BIT,
                                           ape, &c->dd_palette, NULL);
        if (result != DD_OK) {
            DEBUG(("Cannot create palette: %s\n", dd_error(result)));
            goto error;
        }
    }

    if (set_palette(c) < 0)
        goto error;
    c->pixels = pixel_return;
    if (set_physical_colors(c) < 0)
        goto error;
    return c;

error:
    if (c != NULL)
        canvas_destroy(c);
    return NULL;
}

/* Destroy `s'.  */
void canvas_destroy(canvas_t c)
{
    DestroyWindow(c->hwnd);
    free(c->title);
    free(c);
}

/* Make `s' visible.  */
void canvas_map(canvas_t c)
{
}

/* Make `s' unvisible.  */
void canvas_unmap(canvas_t c)
{
}

/* Change the size of `s' to `width' * `height' pixels.  */
void canvas_resize(canvas_t c, unsigned int width, unsigned int height)
{
    ui_resize_canvas_window(c->hwnd, width, height);
}

/* Set the palette of `c' to `p', and return the pixel values in
   `pixel_return[].  */
int canvas_set_palette(canvas_t c, const palette_t *p, PIXEL *pixel_return)
{
    /* Always OK.  */
    return 0;
}

/* ------------------------------------------------------------------------ */

/*
   Here is where the whole thing gets complicate.  We basically have three
   ways to update the window:

   a) manual/direct framebuffer -> primary_surface copy;

   b) framebuffer -> tmp_surface conversion, and consequent tmp_surface ->
      primary_surface blit;

   c) manual/direct framebuffer -> back_surface copy, and consequent
      back_surface <-> primary_surface flip.

   As the frame buffer is 8bpp, method (a) is only possible when the screen
   is in 8bpp mode.  Unluckily, this cannot be done with some Windows NT
   DirectDraw drivers either, because some of them (e.g. the ET4000 one I am
   using now) do not provide direct access to windows in non-exclusive mode:
   in this case, the only way is to use method (b) (see below).  It would be
   great to just use a DirectDraw surface as the framebuffer, but unluckily
   this is not possible because we might need a framebuffer that is larger
   than the physical screen (to clip sprites), and this is not allowed by
   DirectX 3.  (Maybe future versions?  Does anybody have any info on this?)

   Method (b) is necessary when we work in non-exclusive mode and the depth
   is not 8 bpp: in this case, we convert the framebuffer to 16, 24 or 32 bpp
   into a temporary surface and then Blit the temporary surface into the
   window.

   Method (c) is only possible in full-screen mode: we copy the bitmap into a
   backing surface (without any conversion, because we assume full-screen
   mode is always 8bpp) and then order a flip which will happen at vsync.
   This is the best method because it's faster than the other two and makes
   animations as smooth as possible.

*/

void canvas_refresh(canvas_t c, frame_buffer_t f,
                    int xs, int ys, int xi, int yi, int w, int h)
{
    HRESULT result;
    DDSURFACEDESC desc;
    int x, y;
    BYTE *dp;
    LPDIRECTDRAWSURFACE surface = NULL;
    RECT rect;
    int depth, pitch;

    if (IsIconic(c->hwnd))
        return;

    {
        extern int syscolorchanged, displaychanged, querynewpalette, palettechanged;

        if (syscolorchanged) {
            ui_error("System colors changed!\n(not implemented yet)");
            syscolorchanged = 0;
        }
        if (displaychanged) {
            ui_error("Display changed!\n(not implemented yet)");
            displaychanged = 0;
        }
#if 0
        if (querynewpalette) {
            MessageBox(c->hwnd, "querynewpalette", c->title, MB_OK);
            querynewpalette = 0;
            set_palette(c);
            set_physical_colors(c);
            video_resize();
        }
#endif
        if (palettechanged) {
            ui_error("Palette changed!\n(not implemented yet)");
            palettechanged = 0;
            set_physical_colors(c);
            video_resize();
        }
    }

    GetClientRect(c->hwnd, &rect);

    /* Clip.  Not always necessary, but not very expensive either.  */
    if (rect.right - rect.left < w)
        w = rect.right - rect.left;
    if (rect.bottom - rect.top < h)
        h = rect.bottom - rect.top;

    ClientToScreen(c->hwnd, (LPPOINT) &rect);
    rect.left += xi;
    rect.right = rect.left + w;
    rect.top += yi;
    rect.bottom = rect.top + h;

    {
        int tmp;

        tmp = GetSystemMetrics(SM_CXSCREEN);
        if (rect.right > tmp) {
            w -= rect.right - tmp;
            rect.right = tmp;
        }
        tmp = GetSystemMetrics(SM_CYSCREEN);
        if (rect.bottom > tmp) {
            h -= rect.bottom - tmp;
            rect.bottom = tmp;
        }
    }

    if (c->back_surface != NULL) {
        desc.dwSize = sizeof(desc);
        do {
            result = IDirectDrawSurface_Lock(c->back_surface, NULL, &desc,
                                             DDLOCK_WAIT, NULL);
            if (result == DDERR_SURFACELOST)
                if (IDirectDrawSurface_Restore(c->back_surface) != DD_OK)
                    break;
        } while (result == DDERR_SURFACELOST);
        if (result == DD_OK)
            surface = c->back_surface;
#if 0
        else
            DEBUG(("Cannot lock back surface: %s\n", dd_error(result)));
#endif
    }

    /* Don't even try to use the primary surface directly if this is not the
       active window, because we are not able to handle clip lists ourselves
       in that case.  */
    if (surface == NULL && GetForegroundWindow() == c->hwnd) {
        desc.dwSize = sizeof(desc);
        do {
            result = IDirectDrawSurface_Lock(c->primary_surface, NULL, &desc,
                                             0, NULL);
            if (result == DDERR_SURFACELOST)
                if (IDirectDrawSurface_Restore(c->primary_surface) != DD_OK)
                    break;
        } while (result == DDERR_SURFACELOST);
        if (result == DD_OK)
            surface = c->primary_surface;
                else {
                        static int no_primary_lock_reported;

                        if (!no_primary_lock_reported && result == DDERR_CANTLOCKSURFACE) {
                                ui_error("Your DirectDraw driver does not let me lock\n"
                                         "the primary DirectDraw surface.\n\n"
                                         "Performance will be poor!");
                                no_primary_lock_reported = 1;
                        }
#if 0
            DEBUG(("Cannot lock primary surface: %s\n", dd_error(result)));
#endif
                }
    }

    if (surface == NULL) {
        /* Try to lock the temporary surface (last resort).  */
        desc.dwSize = sizeof(desc);
        do {
            result = IDirectDrawSurface_Lock(c->temporary_surface, NULL, &desc,
                                             0, NULL);
            if (result == DDERR_SURFACELOST)
                if (IDirectDrawSurface_Restore(c->temporary_surface) != DD_OK)
                    break;
        } while (result == DDERR_WASSTILLDRAWING);
        if (result == DD_OK)
            surface = c->temporary_surface;
#if 0
        else
            DEBUG(("Cannot lock temporary surface: %s",
                   dd_error(result)));
#endif
    }

    if (surface == NULL) {
        ui_error("Cannot lock any surface: Giving up!\n"
                 "(This is weird, tell Ettore!)");
        return;
    }

    depth = desc.ddpfPixelFormat.u1.dwRGBBitCount;
    pitch = desc.u1.lPitch;

    switch (depth) {
      case 8:
        dp = (BYTE *) desc.lpSurface + pitch * rect.top + rect.left;
        for (y = 0; y < h; y++, dp += pitch) {
            DWORD *p = (DWORD *) dp;
            DWORD *sp = (DWORD *) (f->lines[y + ys] + xs);

#if 1
            {
                int j;

                for (j = 0; j < w; j++)
                    dp[j] = ((BYTE *)sp)[j];
            }
#elif 0
            /* This alternative and the next one cannot work, because
               DirectDraw wants us to always write on aligned boundaries.  */
            {
                int j;

                for (j = 0; j < w >> 2; j++, sp++, p++)
                    *p = *sp;
                for (j = 0; j < w & 0x3; j++)
                    *((BYTE *)p + j) = *((BYTE *)sp + j);
            }
#else
            memcpy((BYTE *) p, (BYTE *) sp, w);
#endif
            /*DEBUG(("Done blitting\n"));*/
        }
        break;
      case 16:
        dp = ((BYTE *) desc.lpSurface + pitch * rect.top
              + 2 * rect.left);
        for (y = ys; y < ys + h; y++, dp += pitch) {
            BYTE *sp = f->lines[y] + xs;
            int i;

            for (i = 0; i < w; i++)
                *((WORD *)dp + i) = (WORD) c->physical_colors[sp[i]];
        }
        break;
      case 24:
        dp = ((BYTE *) desc.lpSurface + pitch * rect.top
              + 3 * rect.left);
        for (y = ys; y < ys + h; y++, dp += pitch) {
            BYTE *sp = f->lines[y] + xs;
            BYTE *p = dp;
            int i;

            for (i = 0; i < w; i++) {
                BYTE *s = (BYTE *) &c->physical_colors[sp[i]];

                p[0] = s[0];
                p[1] = s[1];
                p[2] = s[2];
                p += 3;
            }
        }
        break;
      case 32:
        dp = ((BYTE *) desc.lpSurface + pitch * rect.top
              + 4 * rect.left);
        for (y = ys; y < ys + h; y++, dp += pitch) {
            BYTE *sp = f->lines[y] + xs;
            int i;

            for (i = 0; i < w; i++)
                *((DWORD *)dp + i) = (DWORD) c->physical_colors[sp[i]];
        }
        break;
    }

    if (IDirectDrawSurface_Unlock(surface, NULL) == DDERR_SURFACELOST) {
        IDirectDrawSurface_Restore(surface);
        IDirectDrawSurface_Unlock(surface, NULL);
    }

    if (surface == c->back_surface) {
        /* Back surface: we can flip.  */
    } else if (surface == c->primary_surface) {
        /* Nothing to do...  the window has already been updated.  */
    } else{
        /* Temporary surface: we have to blit.  */
        while (1) {
#if 1
            result = IDirectDrawSurface_Blt(c->primary_surface,
                                            &rect,
                                            surface,
                                            &rect,
                                            DDBLT_WAIT, NULL);
#else
            /* Mmmh...  This could be faster in those cases when the window
               is on top.  It does not work if a Clipper is attached, though.
               FIXME: We could handle things so that this is used when it is
               useful.  */
            result = IDirectDrawSurface_BltFast(c->primary_surface,
                                                rect.left,
                                                rect.top,
                                                surface, &rect,
                                                DDBLTFAST_WAIT);
#endif
            if (result == DD_OK)
                break;
            else if (result == DDERR_INVALIDRECT) {
                DEBUG(("INVALID rect %d, %d, %d, %d\n",
                       rect.left, rect.top,
                       rect.right, rect.bottom));
                break;
            } else if (result == DDERR_SURFACELOST) {
                result = IDirectDrawSurface_Restore(c->primary_surface);
                if (result != DD_OK) {
                    ui_error("Cannot restore primary DirectDraw surface!");
                    break;
                }
            } else {
                    ui_error("Cannot update emulation window:\n%s",
                             dd_error(result));
            }
        }
    }
}
