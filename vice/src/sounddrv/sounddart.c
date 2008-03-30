/*
 * sounddart.c - Implementation of the OS/2-DART sound device
 *
 * Written by
 *  Thomas Bretz (tbretz@gsi.de)
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

#define INCL_DOSPROFILE
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES

#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sound.h"
#include "log.h"
#include "utils.h"

#include "sounddrv.h"

/* typedefs for MCI_***_PARAMS: at the end of file */
static unsigned short     usDeviceID;   /* DART Amp-mixer device ID   */

static MCI_BUFFER_PARMS   BufferParms;
static MCI_MIXSETUP_PARMS MixSetupParms;
static MCI_MIX_BUFFER    *buffers;

static UINT play=0;
static UINT last=0;
static UINT pos =0;
static HMTX hmtxSnd;
static HMTX hmtxOC;  // Open, Close  // not _really_ necessary

#define MUTE_OFF   0
#define MUTE_ON    1

void mute(int state);
int get_volume(void);
void set_volume(int vol);

static void dart_close()
{
    MCI_GENERIC_PARMS GenericParms = {0};
    ULONG             rc;

    // prevent sound from clicking
    mute(MUTE_ON);

    DosRequestMutexSem(hmtxOC, SEM_INDEFINITE_WAIT);

    //DosRequestMutexSem(hmtxSnd, SEM_INDEFINITE_WAIT);
    for (rc=0; rc<BufferParms.ulNumBuffers; rc++)
        buffers[rc].ulFlags = MIX_BUFFER_EOS;

    rc = mciSendCommand(usDeviceID, MCI_STOP, MCI_WAIT,
                        (PVOID) &GenericParms, 0);
    if (rc != MCIERR_SUCCESS) sound_err(rc, "DART_ERR_CLOSE_MIXER");

    //log_message(LOG_DEFAULT, "sounddrv.c: Sound stopped.");

    rc = mciSendCommand(usDeviceID, MCI_BUFFER, MCI_WAIT|MCI_DEALLOCATE_MEMORY,
                        (PVOID) &BufferParms, 0);
    if (rc != MCIERR_SUCCESS) sound_err(rc, "DART_ERR_DEALLOC_BUFFER");

    //log_message(LOG_DEFAULT, "sounddrv.c: Buffer deallocated.");
    /*    rc = mciSendCommand(usDeviceID, MCI_MIXSETUP, MCI_WAIT|MCI_MIXSETUP_DEINIT,
     (PVOID) &MixSetupParms, 0);
     if (rc != MCIERR_SUCCESS) return sound_err(rc, "DART_ERR_MIXSETUP_DEINIT");*/

    rc = mciSendCommand(usDeviceID, MCI_CLOSE, MCI_WAIT,
                        (PVOID) &GenericParms, 0);
    if (rc != MCIERR_SUCCESS) sound_err(rc, "DART_ERR_CLOSE_MIXER");

    usDeviceID = 0;
    //log_message(LOG_DEFAULT, "sounddrv.c: Sound closed.");

    free(buffers);
    //log_message(LOG_DEFAULT, "sounddrv.c: Buffer freed.");

    DosReleaseMutexSem(hmtxOC);
}

LONG APIENTRY DARTEvent (ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags)
{
    ULONG rc;
    switch(ulFlags)
    {
    case MIX_STREAM_ERROR | MIX_WRITE_COMPLETE: // 130  /* error occur in device */
        switch (ulStatus)
        {
        case ERROR_DEVICE_UNDERRUN: // 5626
            sound_err(ulStatus, "Error device underrun.");
            play += 2;
            play %= BufferParms.ulNumBuffers;
            break;
        case ERROR_DEVICE_OVERRUN:  // 5627
            sound_err(ulStatus, "Error device overrun.");
            break;
        }
        rc=MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle,
                                   &(buffers[play]), 1);
        if (rc != MCIERR_SUCCESS) sound_err(rc, "Error writing to Mixer (pmixWrite).");

        break;
    case MIX_WRITE_COMPLETE: /* for playback  */
        rc=MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle,
                                   &(buffers[play]), 1);
        DosRequestMutexSem(hmtxSnd, SEM_INDEFINITE_WAIT);
        memset(buffers[last].pBuffer,0,BufferParms.ulBufferSize);
        last  = play++;
        play %= BufferParms.ulNumBuffers;
        DosReleaseMutexSem(hmtxSnd);
        if (rc != MCIERR_SUCCESS) sound_err(rc, "Error writing to Mixer (pmixWrite).");
        break;

    }
    return TRUE;
}

static int volume = 50;

int get_volume()
{
    return volume;
}

void set_volume(int vol)
{
    ULONG          rc;
    MCI_SET_PARMS  MciSetParms;

    DosRequestMutexSem(hmtxOC, SEM_INDEFINITE_WAIT);
    if (usDeviceID)
    {
        memset(&MciSetParms, 0, sizeof(MCI_SET_PARMS));

        MciSetParms.ulLevel      = vol;
        MciSetParms.ulAudio      = MCI_SET_AUDIO_ALL;

        rc = mciSendCommand(usDeviceID, MCI_SET, MCI_WAIT|
                            MCI_SET_VOLUME|MCI_SET_AUDIO,
                            (PVOID) &MciSetParms, 0);
        if (rc != MCIERR_SUCCESS)
        {
            sound_err(rc, "Error setting up Volume (MCI_SET).");
            return;
        }
    }
    volume=vol;
    DosReleaseMutexSem(hmtxOC);
}

void mute(int state)
{
    static int save_vol;
    if (state)
    {
        save_vol=volume;
        set_volume(0);
    }
    else
        set_volume(volume?volume:save_vol);
}

static float mmtime;
static int written;
static int rest;

static int dart_init(const char *param, int *speed,
                      int *fragsize, int *fragnr, double bufsize)
{

/* The application sends the MCI_MIX_SETUP message to the amp mixer to
 initialize the device for direct reading and writing of audio data in
 the correct mode and format-for example, PCM, MIDI, or MPEG audio.

 If waveform audio data will be played or recorded, the application
 fills in the ulDeviceType field with MCI_DEVICETYPE_WAVEFORM_AUDIO. It
 must also provide values for the following digital-audio-specific
 fields:  format tag, bits per sample, number of samples per second, and
 number of channels.*/

 // MCI_MIXSETUP informs the mixer device of the entry point
 // to report buffers being read or written.
 // We will also need to tell the mixer which media type
 // we will be streaming.  In this case, we'll use
 // MCI_DEVTYPE_WAVEFORM_AUDIO.

    ULONG              rc;
    MCI_AMP_OPEN_PARMS AmpOpenParms;
    ULONG ulNumBuffers, ulLoop;

    if (DosRequestMutexSem(hmtxOC, SEM_IMMEDIATE_RETURN)) return TRUE;

    // ---------
    memset (&AmpOpenParms, 0, sizeof (MCI_AMP_OPEN_PARMS));

    AmpOpenParms.usDeviceID    = (USHORT) 0;
    AmpOpenParms.pszDeviceType = (PSZ) MCI_DEVTYPE_AUDIO_AMPMIX;
    AmpOpenParms.hwndCallback  = 0;

    rc = mciSendCommand(0, MCI_OPEN,
                        MCI_WAIT|MCI_OPEN_TYPE_ID|MCI_OPEN_SHAREABLE /* | MCI_DOS_QUEUE*/,
                        (PVOID) &AmpOpenParms, 0);

    if (rc != MCIERR_SUCCESS) return sound_err(rc, "Error opening DART (MCI_OPEN).");

    usDeviceID = AmpOpenParms.usDeviceID;

    // ---------

    memset(&MixSetupParms, 0, sizeof(MCI_MIXSETUP_PARMS));

    MixSetupParms.ulBitsPerSample = 16;
    MixSetupParms.ulFormatTag     = MCI_WAVE_FORMAT_PCM;
    MixSetupParms.ulSamplesPerSec = *speed; //22050;
    MixSetupParms.ulChannels      = 1;      /* Mono */
    MixSetupParms.ulFormatMode    = MCI_PLAY;
    MixSetupParms.ulDeviceType    = MCI_DEVTYPE_WAVEFORM_AUDIO;

    // MCI_MIXSETUP_QUERYMODE
    // Queries a device to see if a specific mode is supported
    /*    rc = mciSendCommand( usDeviceID, MCI_MIXSETUP,
     MCI_WAIT | MCI_MIXSETUP_QUERYMODE,
     (PVOID) &MixSetupParms, 0);

     if (rc != MCIERR_SUCCESS) return sound_err(rc, "Can't play.");*/

    // The mixer will inform us of entry points to
    // read/write buffers to and also give us a
    // handle to use with these entry points.

    MixSetupParms.pmixEvent = DARTEvent;

    // MCI_MIXSETUP_INIT (DEINIT)
    // Initializes the mixer for the correct mode (MCI_MIXSETUP_PARMS)
    rc = mciSendCommand(usDeviceID, MCI_MIXSETUP, MCI_WAIT|MCI_MIXSETUP_INIT,
                        (PVOID) &MixSetupParms, 0);

    if (rc != MCIERR_SUCCESS) return sound_err(rc, "Error initialising mixer device (MCI_MIXSETUP_INIT).");

    //log_message(LOG_DEFAULT, "sounddart.c: %3i buffers   %6i bytes (suggested by dart)", MixSetupParms.ulNumBuffers, MixSetupParms.ulBufferSize);
    //log_message(LOG_DEFAULT, "sounddart.c: %3i buffers   %6i bytes (wanted by vice)", *fragnr, *fragsize*sizeof(SWORD));

    /*  After the mixer device is set up to use DART, the application
     instructs the device to allocate memory by sending the MCI_BUFFER
     message with the MCI_ALLOCATE_MEMORY flag set. The application uses the
     MCI_BUFFER_PARMS structure to specify the number of buffers it wants
     and the size to be used for each buffer.

     Note: Because of device driver restrictions, buffers are limited to
     64KB on Intel-based systems. No such limit exists on PowerPC systems.

     The pBufList field contains a pointer to an array of MCI_MIX_BUFFER
     structures where the allocated information is to be returned.*/

    buffers = xcalloc(1, *fragnr*sizeof(MCI_MIX_BUFFER));

    BufferParms.pBufList     = buffers;
    BufferParms.ulNumBuffers = *fragnr;
    BufferParms.ulBufferSize = *fragsize*sizeof(SWORD);

    rc = mciSendCommand(usDeviceID, MCI_BUFFER, MCI_WAIT|MCI_ALLOCATE_MEMORY,
                        (PVOID) &BufferParms, 0);

    if (rc != MCIERR_SUCCESS) return sound_err(rc, "Error allocating Memory (MCI_ALLOCATE_MEMORY).");

    // MCI driver will return the number of buffers it
    // was able to allocate
    // it will also return the size of the information
    // allocated with each buffer.

    if (*fragnr  !=BufferParms.ulNumBuffers)
    {
        *fragnr   = BufferParms.ulNumBuffers;
        log_message(LOG_DEFAULT, "sounddart.c: %3i buffers   %6i bytes (gotten from dart)", BufferParms.ulNumBuffers, BufferParms.ulBufferSize);
    }
    if (*fragsize!=BufferParms.ulBufferSize/sizeof(SWORD))
    {
        *fragsize = BufferParms.ulBufferSize/sizeof(SWORD);
        log_message(LOG_DEFAULT, "sounddart.c: %3i buffers   %6i bytes (gotten from dart)", BufferParms.ulNumBuffers, BufferParms.ulBufferSize);
    }


    // SECURITY for *fragnr <2 ????
    for (ulLoop=0; ulLoop<*fragnr; ulLoop++)
        memset(buffers[ulLoop].pBuffer,0,*fragsize*sizeof(SWORD));

    mmtime  = (float)*speed/1000;
    rest=BufferParms.ulBufferSize;
    written = 0;

    // Must write at least two buffers to start mixer
    play = 2;  // this is the buffer which is played next
    last = 1;  // this is the buffer which is played last before next
    pos  = 0;  // this is the position to which buffer we have to write
    MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle,
                            &(buffers[0]), 2);

    DosReleaseMutexSem(hmtxOC);

    mute(MUTE_OFF);

    return 0;
}


static int dart_write(SWORD *pbuf, size_t nr)
{
    /* The MCI_MIX_BUFFER structure is used for reading and writing data to
     and from the mixer.

     Once the device is set up and memory has been allocated, the
     application can use the function pointers obtained during MCI_MIXSETUP
     to communicate with the mixer. During a playback operation, the
     application fills the buffers with audio data and then writes the
     buffers to the mixer device using the pmixWrite entry point. When audio
     data is being recorded, the mixer device fills the buffers using the
     pmixRead entry point. Each buffer returned the the application has a
     time stamp (in milliseconds) attached so the program can determine the
     current time of the device.

     MCI_STOP, MCI_PAUSE, and MCI_RESUME are used to stop, pause, or resume
     the audio device, respectively. MCI_STOP and MCI_PAUSE can only be sent
     to the mixer device after mixRead and mixWrite have been called.
     MCI_RESUME will only work after MCI_PAUSE has been sent.

     Note:  After your application has completed data transfers, issue
     MCI_STOP to avoid a pause the next time the mixer device is started.

     If your application needs more precise timing information than
     provided by the time stamp returned with each buffer, you can use
     MCI_STATUS with the MCI_STATUS_POSITION flag to retrieve the current
     time of the device in MMTIME units.*/

    DosRequestMutexSem(hmtxSnd, SEM_INDEFINITE_WAIT);
    // if we wanna write the buffer which is played next
    // write the overnext.
    if (play==(pos+1)%BufferParms.ulNumBuffers)
        pos = (++pos)%BufferParms.ulNumBuffers;
    memcpy(buffers[pos++].pBuffer,pbuf,nr*sizeof(SWORD));
    pos %= BufferParms.ulNumBuffers;
    DosReleaseMutexSem(hmtxSnd);

    return 0;
}

static int dart_write2(SWORD *pbuf, size_t nr)
{
    if (rest>BufferParms.ulBufferSize) rest=BufferParms.ulBufferSize;

    written += nr;
    nr *= sizeof(SWORD);

    while (nr)
    {
        int nrtowrite = nr>rest ? rest : nr;

        DosRequestMutexSem(hmtxSnd, SEM_INDEFINITE_WAIT);
        if (play==(pos+1)%BufferParms.ulNumBuffers)
            pos = (++pos)%BufferParms.ulNumBuffers;

        memcpy((ULONG)buffers[pos].pBuffer+(BufferParms.ulBufferSize-rest),
               pbuf, nrtowrite);

        rest -= nrtowrite;

        if (!rest)
        {
            pos = (++pos)%BufferParms.ulNumBuffers;
            rest = BufferParms.ulBufferSize;
        }
        DosReleaseMutexSem(hmtxSnd);
        nr -= nrtowrite;
    }
    return 0;
}

/* return number of samples unplayed in the kernel buffer at the moment */
static int dart_bufferstatus(int first)
{

    MCI_STATUS_PARMS mciStatus;
    LONG rc;
    MCI_SET_PARMS  MciSetParms;
    static int offset;

    mciStatus.ulItem = MCI_STATUS_POSITION;

    rc = mciSendCommand(usDeviceID, MCI_STATUS,
                              MCI_WAIT|MCI_STATUS_ITEM,
                              (PVOID) &mciStatus, 0);

    rc = written - (int)(mmtime*mciStatus.ulReturn);
//    log_debug("w: %i  p: %i  r: %i", written, (int)(mmtime*mciStatus.ulReturn), rc<0?0:rc);

//    return BufferParms.ulBufferSize/sizeof(SWORD);
    return rc<0?0:rc;
}

static int dart_suspend()
{
    mute(MUTE_ON);
    log_message(LOG_DEFAULT, "dart_suspend");
    return 0;
}

static int dart_resume()
{
    mute(MUTE_OFF);
    log_message(LOG_DEFAULT, "dart_resume");
    return 0;
}

static sound_device_t dart_device =
{
    "dart",
    dart_init,         // dart_init
    dart_write,        // dart_write
    NULL,              // dart_dump
    NULL,              // dart_flush
    NULL,              // dart_bufferstatus
    dart_close,        // dart_close
    dart_suspend,      // dart_suspend
    dart_resume        // dart_resume
};

static sound_device_t dart2_device =
{
    "dart2",
    dart_init,         // dart_init
    dart_write2,       // dart_write
    NULL,              // dart_dump
    NULL,              // dart_flush
    dart_bufferstatus, // dart_bufferstatus
    dart_close,        // dart_close
    dart_suspend,      // dart_suspend
    dart_resume        // dart_resume
};

void sound_init_dart(void)
{
    static int first=TRUE;
    if (first)
    {
        DosCreateMutexSem("\\SEM32\\Vice2\\Sound\\OC.sem",    &hmtxOC,  0, FALSE);
        DosCreateMutexSem("\\SEM32\\Vice2\\Sound\\Write.sem", &hmtxSnd, 0, FALSE);
        first=FALSE;
    }
}

int sound_init_dart_device(void)
{
    sound_init_dart();
    return sound_register_device(&dart_device);
}

int sound_init_dart2_device(void)
{
    sound_init_dart();
    return sound_register_device(&dart2_device);
}

// typedef struct_MCI_MIXSETUP_PARMS
// {
//     HWND        hwndCallback;    /* IN  Window for notifications */
//     ULONG       ulBitsPerSample; /* IN  Number of bits per sample */
//     ULONG       ulFormatTag;     /* IN  Format tag */
//     ULONG       ulSamplesPerSec; /* IN  Sampling rate */
//     ULONG       ulChannels;      /* IN  Number of channels */
//     ULONG       ulFormatMode;    /* IN  MCI_RECORD or MCI_PLAY */
//     ULONG       ulDeviceType;    /* IN  MCI_DEVTYPE */
//     ULONG       ulMixHandle;     /* OUT Read/Write handle */
//     PMIXERPROC  pmixWrite;       /* OUT Write routine entry point */
//     PMIXERPROC  pmixRead;        /* OUT Read routine entry point */
//     PMIXEREVENT pmixEvent;       /* IN  Event routine entry point */
//     PVOID       pExtendedInfo;   /* IN  Media-specific info */
//     ULONG       ulBufferSize;    /* OUT Recommended buffer size */
//     ULONG       ulNumBuffers;    /* OUT Recommended num buffers */
// } MCI_MIXSETUP_PARMS;

// typedef struct_MCI_BUFFER_PARMS {
//     HWND   hwndCallback;    /* Window for notifications */
//     ULONG  ulStructLength;  /* Length of MCI_BUFFER_PARMS */
//     ULONG  ulNumBuffers;    /* Number of buffers to allocate (IN/OUT)*/
//     ULONG  ulBufferSize;    /* Size of buffers mixer should use */
//     ULONG  ulMintoStart;    /* Unused */
//     ULONG  ulSrcStart;      /* Unused */
//     ULONG  ulTgtStart;      /* Unused */
//     PVOID  pBufList;        /* Pointer to array of buffers */
// } MCI_BUFFER_PARMS;

// typedef MCI_BUFFER_PARMS *PMCI_BUFFER_PARMS;

// typedef struct_MCI_MIX_BUFFER {
//     ULONG  ulStructLength;  /* Length of the structure */
//     ULONG  pBuffer;         /* Pointer to a buffer */
//     ULONG  ulBufferLength;  /* Length of the buffer */
//     ULONG  ulFlags;         /* Flags */
//     ULONG  ulUserParm;      /* User buffer parameter */
//     ULONG  ulTime;          /* Device time in milliseconds */
//     ULONG  ulReserved1;     /* Unused */
//     ULONG  ulReserved2;     /* Unused */
// } MCI_MIX_BUFFER;

// typedef MCI_MIX_BUFFER *PMCI_MIX_BUFFER

