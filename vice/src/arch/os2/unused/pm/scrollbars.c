#include "scrollbars.h"

void SBMsetRange(HWND hwnd, int sbm, USHORT first, USHORT last, USHORT pos)
{
    WinSendMsg(WinWindowFromID(WinQueryWindow(hwnd,QW_PARENT),sbm),
               SBM_SETSCROLLBAR,
               MPFROM2SHORT(    0, pos),
               MPFROM2SHORT(first,last));
}

void SBMsetPos(HWND hwnd, int sbm, USHORT pos)
{
    WinSendMsg(WinWindowFromID(WinQueryWindow(hwnd,QW_PARENT),sbm),
               SBM_SETPOS, MPFROMSHORT(pos), 0);
}

void SBMsetSize(HWND hwnd, int sbm, USHORT size, USHORT outof)
{
    WinSendMsg(WinWindowFromID(WinQueryWindow(hwnd,QW_PARENT),sbm),
               SBM_SETTHUMBSIZE, MPFROM2SHORT(size,outof), 0);
}

void SBMcalcPos(MPARAM mp2, int *pos, int counts, int visible)
{
    switch (SHORT2FROMMP(mp2)) {
    case SB_LINELEFT:    *pos-=1;                 break;
    case SB_PAGELEFT:    *pos-=5;                 break;
    case SB_LINERIGHT:   *pos+=1;                 break;
    case SB_PAGERIGHT:   *pos+=5;                 break;
    case SB_SLIDERTRACK: *pos =SHORT1FROMMP(mp2); break;
    }
    if (*pos<0)                *pos=0;
    if (*pos>counts-visible-1) *pos=counts-visible;
}

void SBMsetThumb(HWND hwnd, int sbm, int pos, int visible, int counts)
{
    SBMsetRange(hwnd, sbm, 0, counts-visible, pos);
    SBMsetSize (hwnd, sbm, visible, counts);
}

