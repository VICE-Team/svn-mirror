#include "pbar.h"

void drawBar(HPS hps, RECTL rectl, float val, float max, char *txt,
             int cut)
{
    int right, left;
    int width = rectl.xRight-rectl.xLeft-4;
    
    WinFillRect(hps, &rectl,SYSCLR_BUTTONDARK);       // Draw Backgrnd

    left =rectl.xLeft +2;
    right=rectl.xRight-2;
    
    rectl.yBottom +=2;
    rectl.yTop    -=2;

    rectl.xLeft +=2;
    rectl.xRight = left+(val/max)*width;
    WinFillRect(hps, &rectl, 2);                      // Draw Left Side

    rectl.xLeft  = rectl.xRight;
    rectl.xRight = right;
    WinFillRect(hps, &rectl, SYSCLR_FIELDBACKGROUND); // Draw Right Side

    if (cut) {
        rectl.xLeft  = left+(cut/max)*width-1;
        rectl.xRight = rectl.xLeft+2;
        WinFillRect(hps, &rectl,SYSCLR_BUTTONDARK);   // Draw 100%-Bar
    }

    rectl.xLeft    = left;
    rectl.xRight   = right;
    rectl.yBottom -= 1;
    GpiSetMix(hps, FM_NOTXORSRC);                     // Draw Text in bar
    WinDrawText(hps, strlen(txt), txt, &rectl, 0, 0,
                DT_TEXTATTRS | DT_VCENTER | DT_CENTER);
}

