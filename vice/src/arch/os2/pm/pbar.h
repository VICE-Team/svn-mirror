#ifndef _PBAR_H
#define _PBAR_H

#define INCL_WINSYS
#define INCL_WININPUT
#define INCL_WINRECTANGLES
#define INCL_WINWINDOWMGR

#include <os2.h>

void drawBar(HPS hps, RECTL rectl, float val, float max, char *txt,
             int cut);

#endif
