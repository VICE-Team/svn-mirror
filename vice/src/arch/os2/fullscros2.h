/*****************************************************************************
 * FullScreen header file                                                    *
 *****************************************************************************/
#ifndef OS2_FULLSCREEN_H
#define OS2_FULLSCREEN_H

/*
 **  Defines and structures for the GHI_CMD_SHOWPTR DDI
 */
typedef struct _HWSHOWPTRIN {     // hwshowptrin
    ULONG ulLength;
    BOOL  fShow;
} HWSHOWPTRIN;
typedef HWSHOWPTRIN *PHWSHOWPTRIN;

#include <dive.h>
#include "gradd.h"

#define FOURCC_SCRN 0

typedef struct _FBINFO {
   ULONG ulLength;          /*  Length of FBINFO data structure, in bytes. */
   ULONG ulCaps;            /*  Specifies the capabilities. */
   ULONG ulBPP;             /*  Screen bits per pel. */
   ULONG ulXRes;            /*  Number of screen X pels. */
   ULONG ulYRes;            /*  Number of screen Y pels. */
   ULONG ulScanLineBytes;   /*  Number of bytes per scanline. */
   ULONG fccColorEncoding;  /*  Screen color encoding. */
   ULONG ulNumENDIVEDrivers;/*  Number of pluggable EnDIVE drivers.  */
} FBINFO;
typedef FBINFO *PFBINFO;

typedef struct _APERTURE {  /* aperture */
      ULONG ulPhysAddr;       /* physical address */
      ULONG ulApertureSize;   /* 1 Meg, 4 Meg or 64k */
      ULONG ulScanLineSize;   /* this is >= the screen width */
      RECTL rctlScreen;       /* device independant co-ordinates */
} APERTURE;
typedef APERTURE FAR *PAPERTURE;

void APIENTRY DiveFullScreenInit(PAPERTURE pNewAperture, PFBINFO pNewFrameBuffer);
void APIENTRY DiveFullScreenTerm (void);

#endif
