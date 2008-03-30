#ifndef _PBAR_H
#define _PBAR_H

#define INCL_WINSYS
#define INCL_WININPUT
#define INCL_WINRECTANGLES
#define INCL_WINWINDOWMGR

// VAC++ incls
#define INCL_GPIPRIMITIVES

#include <os2.h>
#include <string.h>

void drawBar(HPS hps, RECTL rectl, float val, float cut, char *txt);

#endif
