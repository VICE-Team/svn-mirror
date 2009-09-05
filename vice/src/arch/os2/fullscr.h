/*****************************************************************************
 * FullScreen header file                                                    *
 *****************************************************************************/
#ifndef OS2_FULLSCR_H
#define OS2_FULLSCR_H

#include <dive.h>

struct GDDMODEINFO;
struct video_canvas_t;

extern int FullscreenSwitch(HWND hwnd);
extern void FullscreenDeactivate(HWND hwnd);
extern void FullscreenDisable(void);
extern int FullscreenActivate(HWND hwnd);
extern int FullscreenIsInMode(void);
extern int FullscreenInit(FOURCC fcc);
extern int FullscreenFree(void);
extern int FullscreenPrintModes(void);
extern int FullscreenQueryHorzRes(void);
extern int FullscreenQueryVertRes(void);
extern int FullscreenIsInMode(void);
extern int FullscreenIsInFS(void);
extern int FullscreenIsNow(void);
extern void FullscreenChangeMode(HWND hwnd);
extern void FullscreenShowPointer(HWND hwnd, int state);
extern void FullscreenChangeRate(HWND hwnd, int state);

#endif
