#ifndef _SCROLLBARS_H
#define _SCROLLBARS_H

#define INCL_WINSYS
#define INCL_WINSTDFILE
#define INCL_WINSCROLLBARS
#define INCL_WINFRAMEMGR

#include <os2.h>

void SBMsetRange(HWND hwnd, int sbm, USHORT first, USHORT last, USHORT pos);
void SBMsetPos(HWND hwnd, int sbm, USHORT pos);
void SBMsetSize(HWND hwnd, int sbm, USHORT size, USHORT outof);
void SBMcalcPos(MPARAM mp2, int *pos, int counts, int visible);
void SBMsetThumb(HWND hwnd, int sbm, int pos, int visible, int counts);

#endif