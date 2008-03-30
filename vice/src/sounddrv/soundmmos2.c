/*
 * soundmmos2.c - Implementation of the MMOS2 sound device
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sound.h"


typedef struct _playlist
{
    ULONG ulCommand;
    ULONG ulOperandOne;
    ULONG ulOperandTwo;
    ULONG ulOperandThree;
} PLAYLIST;

PLAYLIST Playlist[3]={
    {DATA_OPERATION,   0, 0, 0},
    {BRANCH_OPERATION, 0, 0, 0},  // jump to instruction 0
    {EXIT_OPERATION,   0, 0, 0}
};

MCI_OPEN_PARMS mciOpen;

USHORT usSoundDevID;
ULONG  ulSamplesPerSec;
USHORT usBitsPerSample;

static void mmlog (char *s, int i)
{
    FILE *fl;
    fl=fopen("output","a");
    fprintf(fl,"MMOS2: %s %i\n",s,i);
    fclose(fl);
}

static int mmos2_err(ULONG rc, char *s)
{
    char text[128];
    mciGetErrorString(rc, text, 128);  // angegebener einheitenname ungueltig
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, text, s, 0, MB_OK);
    mmlog(s, rc);
    mmlog(text,0);
    return 1;
    //    WORD lo order=unsigned short
}

SWORD *dataBuf, *old;

static void mmos2_close(warn_t *w)
{
    int rc;
    // MCI_WAIT hangs???
    mmlog("stopping",0);
    rc=mciSendCommand(usSoundDevID, MCI_STOP, MCI_WAIT, NULL, 0);
    if (rc != MCIERR_SUCCESS) mmos2_err(rc, "Error stopping MMOS2 Playback (MCI_STOP)");
    mmlog("stopped",0);
    free(dataBuf);
    rc=mciSendCommand(usSoundDevID, MCI_CLOSE, MCI_WAIT, NULL, 0);
    if (rc != MCIERR_SUCCESS) mmos2_err(rc, "Error closing MMOS2 Waveform Audio Device (MCI_CLOSE)");
    mmlog("closed", rc);
}

int pos, nrOfFragsPS;

static int mmos2_init(warn_t *w, const char *param, int *speed,
                      int *fragsize, int *fragnr, double bufsize)
{
    int rc, i;
    int bufsz      = *fragsize*sizeof(SWORD); //((*fragnr)>>3);
    MCI_WAVE_SET_PARMS mciSet;

    if (usSoundDevID) return 1;

    mciOpen.pszDeviceType  = MCI_DEVTYPE_WAVEFORM_AUDIO_NAME;
    // parameter fuer diesen befehl ungueltig
    mciOpen.hwndCallback   = (HWND) NULL;
    mciOpen.pszAlias       = (CHAR) NULL;
    mciOpen.pszElementName = (PSZ)  Playlist;
    rc=mciSendCommand(0, MCI_OPEN,
                      MCI_WAIT|MCI_OPEN_PLAYLIST|MCI_OPEN_SHAREABLE,
                      &mciOpen, 0);
//
    if (rc != MCIERR_SUCCESS) return mmos2_err(rc, "Error open MMOS2 Waveform Audio Device (MCI_OPEN)");

    mmlog("opened",rc);

    usSoundDevID = mciOpen.usDeviceID;

    mciSet.ulSamplesPerSec = *speed;
    mciSet.usBitsPerSample = *fragnr; //*speed*2;
    rc=mciSendCommand(usSoundDevID, MCI_SET,
                      MCI_WAIT|
                      MCI_WAVE_SET_SAMPLESPERSEC|
                      MCI_WAVE_SET_BITSPERSAMPLE,
                      &mciSet, 0);

    if (rc != MCIERR_SUCCESS) return mmos2_err(rc, "Error setting Sample Rate or Saples Per Sec (MCI_SET)");

    mmlog("set",rc);

    //    atexit(mmos2_close);
    pos=0;
    nrOfFragsPS = (*speed)/(*fragsize)*bufsize;
    mmlog("nrFragsPS",nrOfFragsPS);
    dataBuf=(short*)malloc(bufsz*nrOfFragsPS);
    memset(dataBuf, 0, bufsz*nrOfFragsPS);
    Playlist[0].ulOperandOne   = (ULONG)dataBuf;
    Playlist[0].ulOperandTwo   = bufsz*nrOfFragsPS;
    Playlist[0].ulOperandThree = 0;

    rc=mciSendCommand(usSoundDevID, MCI_PLAY, 0, &mciOpen, 0);
    if (rc != MCIERR_SUCCESS) return mmos2_err(rc, "MCI_PLAY");
    return 0;
}

static int mmos2_write(warn_t *w, SWORD *pbuf, int nr)
{
    pos %= nrOfFragsPS;
    //----------------------------------------------
    memcpy((VOID*)((ULONG)dataBuf+(pos*nr*sizeof(SWORD))), pbuf, nr*sizeof(SWORD));
    pos++;
    //        mmlog("1st",PlayList[0].ulOperandThree);
    //-------------------------------------------

    return 0;
}

static sound_device_t mmos2_device =
{
    "mmos2",
    mmos2_init,
    mmos2_write,
    NULL,  // dump
    NULL,  // flush
    NULL,  // bufferstatus
    mmos2_close,
    NULL,  // suspend
    NULL
};

int sound_init_mmos2_device(void)
{
    return sound_register_device(&mmos2_device);
}
