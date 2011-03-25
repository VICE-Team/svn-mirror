/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/* Dingoo SLCD SDL video driver implementation
 */

/* Modified for VICE usage */

#include <SDL_video.h>
#include <SDL_mouse.h>
#include <video/SDL_sysvideo.h>
#include <video/SDL_pixels_c.h>
#include <events/SDL_events_c.h>

#include <video/slcd/SDL_slcd_video.h>
#include <video/slcd/SDL_slcd_events_c.h>
#include <video/slcd/SDL_slcd_mouse_c.h>

#include <dingoo/slcd.h>
#include <dingoo/cache.h>

#include <dingoo/jz4740.h>
#include <archdep.h>

#define SLCDVID_DRIVER_NAME "slcd"

/* Initialization/Query functions */
static int SLCD_VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Rect **SLCD_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
static SDL_Surface *SLCD_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int SLCD_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static void SLCD_VideoQuit(_THIS);

/* Hardware surface functions */
static int SLCD_AllocHWSurface(_THIS, SDL_Surface *surface);
static int SLCD_LockHWSurface(_THIS, SDL_Surface *surface);
static void SLCD_UnlockHWSurface(_THIS, SDL_Surface *surface);
static void SLCD_FreeHWSurface(_THIS, SDL_Surface *surface);

#define EXTRA_DMA_CHANNEL 5

uint32_t lcd_frame;

/* etc. */
static void SLCD_UpdateRects(_THIS, int numrects, SDL_Rect *rects);

/* SLCD driver bootstrap functions */

static int SLCD_Available(void)
{
    return 1; /* Always available */
}

static void SLCD_DeleteDevice(SDL_VideoDevice *device)
{
    SDL_free(device->hidden);
    SDL_free(device);
}

static SDL_VideoDevice *SLCD_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
    if (device) {
        SDL_memset(device, 0, (sizeof *device));
        device->hidden = (struct SDL_PrivateVideoData *)SDL_malloc((sizeof *device->hidden));
    }
    if ((device == NULL) || (device->hidden == NULL)) {
        SDL_OutOfMemory();
        if (device) {
            SDL_free(device);
        }
        return 0;
    }
    SDL_memset(device->hidden, 0, (sizeof *device->hidden));

    /* Set the function pointers */
    device->VideoInit = SLCD_VideoInit;
    device->ListModes = SLCD_ListModes;
    device->SetVideoMode = SLCD_SetVideoMode;
    device->CreateYUVOverlay = NULL;
    device->SetColors = SLCD_SetColors;
    device->UpdateRects = SLCD_UpdateRects;
    device->VideoQuit = SLCD_VideoQuit;
    device->AllocHWSurface = SLCD_AllocHWSurface;
    device->CheckHWBlit = NULL;
    device->FillHWRect = NULL;
    device->SetHWColorKey = NULL;
    device->SetHWAlpha = NULL;
    device->LockHWSurface = SLCD_LockHWSurface;
    device->UnlockHWSurface = SLCD_UnlockHWSurface;
    device->FlipHWSurface = NULL;
    device->FreeHWSurface = SLCD_FreeHWSurface;
    device->SetCaption = NULL;
    device->SetIcon = NULL;
    device->IconifyWindow = NULL;
    device->GrabInput = NULL;
    device->GetWMInfo = NULL;
    device->InitOSKeymap = SLCD_InitOSKeymap;
    device->PumpEvents = SLCD_PumpEvents;

    device->free = SLCD_DeleteDevice;

    return device;
}

VideoBootStrap SLCD_bootstrap = {
    SLCDVID_DRIVER_NAME, "SDL Dingoo SLCD video driver",
    SLCD_Available, SLCD_CreateDevice
};


int SLCD_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
    int i;

    lcd_frame = ((uint32_t)(lcd_get_frame())) & 0x1fffffff;

    /* Initialize all variables that we clean on shutdown */
    for (i = 0; i < SDL_NUMMODES; ++i) {
        SDL_modelist[i] = SDL_malloc(sizeof(SDL_Rect));
        SDL_modelist[i]->x = SDL_modelist[i]->y = 0;
    }

    /* Modes sorted largest to smallest */
    SDL_modelist[0]->w = 320; SDL_modelist[0]->h = 240;
    SDL_modelist[1] = NULL;

    /* Determine the screen depth */
    /* we change this during the SDL_SetVideoMode implementation... */
    vformat->BitsPerPixel = 16; /* FIXME */
    vformat->BytesPerPixel = 2;

    /* We're done! */
    return 0;
}

SDL_Rect **SLCD_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
    if (format->BitsPerPixel != 16) {
        return NULL;
    }

#if 0
    if ( flags & SDL_FULLSCREEN ) {
        return SDL_modelist;
    } else {
        return (SDL_Rect **) -1;
    }
#endif

    return SDL_modelist;
}

SDL_Surface *SLCD_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags)
{
    if (width != 320 || height != 240 || bpp != 16) {
        SDL_SetError("Wrong format!");
        return NULL;
    }

    if (this->hidden->buffer) {
        SDL_free( this->hidden->buffer );
    }

    this->hidden->buffer = SDL_malloc(width * height * (bpp / 8));
    if (!this->hidden->buffer) {
        SDL_SetError("Couldn't allocate buffer for requested mode");
        return NULL;
    }

    SDL_memset(this->hidden->buffer, 0, width * height * (bpp / 8));

    /* Allocate the new pixel format for the screen */
    if (!SDL_ReallocFormat(current, bpp, 0, 0, 0, 0)) {
        SDL_free(this->hidden->buffer);
        this->hidden->buffer = NULL;
        SDL_SetError("Couldn't allocate new pixel format for requested mode");
        return NULL;
    }

    /* Set up the new mode framebuffer */
    current->flags = flags & SDL_FULLSCREEN;
    this->hidden->w = current->w = width;
    this->hidden->h = current->h = height;
    current->pitch = current->w * (bpp / 8);
    current->pixels = this->hidden->buffer;

    /* We're done */
    return current;
}

/* We don't actually allow hardware surfaces other than the main one */
static int SLCD_AllocHWSurface(_THIS, SDL_Surface *surface)
{
    return -1;
}
static void SLCD_FreeHWSurface(_THIS, SDL_Surface *surface)
{
    return;
}

/* We need to wait for vertical retrace on page flipped displays */
static int SLCD_LockHWSurface(_THIS, SDL_Surface *surface)
{
    return 0;
}

static void SLCD_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
    return;
}

static void SLCD_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
    if (this->hidden->buffer == NULL) {
        return;
    }
    __dcache_writeback_all();

#if 0
    while(!(REG_DMAC_DCCSR(EXTRA_DMA_CHANNEL) & DMAC_DCCSR_TT));
#endif

    /* Disable DMA channel while configuring. */
    REG_DMAC_DCCSR(EXTRA_DMA_CHANNEL) = 0;

    /* DMA request source is SLCD.*/
    REG_DMAC_DRSR(EXTRA_DMA_CHANNEL) = DMAC_DRSR_RS_AUTO;

    /* Set source, target and count.*/
    REG_DMAC_DSAR(EXTRA_DMA_CHANNEL) = ((uint32_t)(this->hidden->buffer)) & 0x1fffffff;
    REG_DMAC_DTAR(EXTRA_DMA_CHANNEL) = lcd_frame;
    REG_DMAC_DTCR(EXTRA_DMA_CHANNEL) = 320 * 240 / 16;

    /*
     * Source address increment, source width 32 bit,
     * destination width 32 bit
     * block transfer mode, no interrupt.
     */
    REG_DMAC_DCMD(EXTRA_DMA_CHANNEL) = DMAC_DCMD_SAI | DMAC_DCMD_SWDH_32 |
                                       DMAC_DCMD_DWDH_32 | DMAC_DCMD_DAI |
                                       DMAC_DCMD_DS_32BYTE | DMAC_DCMD_TM;

    /* No DMA descriptor used.*/
    REG_DMAC_DCCSR(EXTRA_DMA_CHANNEL) |= DMAC_DCCSR_NDES;

    /* Wait for transfer terminated bit of LCD */
    while(!(REG_DMAC_DCCSR(0) & DMAC_DCCSR_TT));

    /* Set enable bit to start DMA.*/
    REG_DMAC_DCCSR(EXTRA_DMA_CHANNEL) |= DMAC_DCCSR_EN;
	
    if (is_lcd_active()) {
        /* Enable DMA on the SLCD.*/
        REG_SLCD_CTRL = 1;

        /* Disable DMA channel while configuring. */
        REG_DMAC_DCCSR(0) = 0;

        /* DMA request source is SLCD. */
        REG_DMAC_DRSR(0) = DMAC_DRSR_RS_SLCD;

        /* Set source, target and count.*/
        REG_DMAC_DSAR(0) = lcd_frame;
        REG_DMAC_DTAR(0) = SLCD_FIFO & 0x1fffffff;
        REG_DMAC_DTCR(0) = 320 * 240 * 2 / 16;

        /*
         * Source address increment, source width 32 bit,
         * destination width 16 bit, data unit size 16 bytes,
         * block transfer mode, no interrupt.
         */
        REG_DMAC_DCMD(0) = DMAC_DCMD_SAI | DMAC_DCMD_SWDH_32 |
                           DMAC_DCMD_DWDH_16 | DMAC_DCMD_DS_16BYTE |
                           DMAC_DCMD_TM;
	
        /* No DMA descriptor used.*/
        REG_DMAC_DCCSR(0) |= DMAC_DCCSR_NDES;
	
        /* Set enable bit to start DMA.*/
        REG_DMAC_DCCSR(0) |= DMAC_DCCSR_EN;
    } else {
        lcd_set_frame();
    }
}	

int SLCD_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
    /* do nothing of note. */
    return 1;
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
void SLCD_VideoQuit(_THIS)
{
    int i;

    /* Free video mode lists */
    for (i = 0; i < SDL_NUMMODES; ++i ) {
        if (SDL_modelist[i] != NULL ) {
            SDL_free(SDL_modelist[i]);
            SDL_modelist[i] = NULL;
        }
    }

    if (this->screen->pixels != NULL) {
        SDL_free(this->screen->pixels);
        this->screen->pixels = NULL;
    }
}
