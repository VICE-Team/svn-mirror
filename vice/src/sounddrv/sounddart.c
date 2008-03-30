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

unsigned short     usDeviceID;   /* DART Amp-mixer device ID   */

MCI_BUFFER_PARMS   BufferParms;
MCI_MIXSETUP_PARMS MixSetupParms;
MCI_MIX_BUFFER    *buffers;

UINT  play;
HMTX  hmtxSnd;

static void dart_close(warn_t *w)
{
    MCI_GENERIC_PARMS GenericParms = {0};
    ULONG             rc;

    for (rc=0; rc<BufferParms.ulNumBuffers; rc++)
        buffers[rc].ulFlags = MIX_BUFFER_EOS;

    rc = mciSendCommand(usDeviceID, MCI_STOP, MCI_WAIT,
                        (PVOID) &GenericParms, 0);
    if (rc != MCIERR_SUCCESS) sound_err(rc, "DART_ERR_CLOSE_MIXER");

    rc = mciSendCommand(usDeviceID, MCI_BUFFER, MCI_WAIT|MCI_DEALLOCATE_MEMORY,
                        (PVOID) &(BufferParms), 0);
    if (rc != MCIERR_SUCCESS) sound_err(rc, "DART_ERR_DEALLOC_BUFFER");

/*    rc = mciSendCommand(usDeviceID, MCI_MIXSETUP, MCI_WAIT|MCI_MIXSETUP_DEINIT,
                        (PVOID) &MixSetupParms, 0);
    if (rc != MCIERR_SUCCESS) return sound_err(rc, "DART_ERR_MIXSETUP_DEINIT");*/

    rc = mciSendCommand(usDeviceID, MCI_CLOSE, MCI_WAIT,
                        (PVOID) &GenericParms, 0);
    if (rc != MCIERR_SUCCESS) sound_err(rc, "DART_ERR_CLOSE_MIXER");

    free(buffers);
}

// Using a Thread for this is to prevent the mainthread from beeing
// stalled due to a waiting DosRequestMutexSem.
/*void MixingThread(void *data) {
    static int played;

    ULONG eventCounter, rc;
//    DosOpenEventSem(semNeedsBuffer);
    do {
        DosWaitEventSem(semNeedsBuffer, SEM_INDEFINITE_WAIT);
        DosResetEventSem(semNeedsBuffer,&eventCounter);


        // Wait until DART_write is ready.
        //        DosRequestMutexSem(hmtxSnd, SEM_INDEFINITE_WAIT);
        rc=MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle,
                                   &(buffers[played]), 1);
        memset(buffers[(played+BufferParms.ulNumBuffers-1)%BufferParms.ulNumBuffers].pBuffer,0,BufferParms.ulBufferSize);
        //        DosReleaseMutexSem(hmtxSnd);
        played = (++played)%BufferParms.ulNumBuffers;
        if (rc != MCIERR_SUCCESS) sound_err(rc, "Error writing to Mixer (pmixWrite).");
//      log_message(LOG_DEFAULT, "pmixWrite %i %li", played, pBuffer->ulTime);


    } while (1);
}*/

LONG APIENTRY DARTEvent (ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags)
{
    ULONG rc;
    static int last;
    switch(ulFlags)
    {
    case MIX_STREAM_ERROR | MIX_WRITE_COMPLETE: // 130  /* error occur in device */
        switch (ulStatus)
        {
        case ERROR_DEVICE_UNDERRUN: // 5626
            sound_err(ulStatus, "Error device underrun.");
            play += 2;
            play %= BufferParms.ulNumBuffers;
        case ERROR_DEVICE_OVERRUN:  // 5627
            sound_err(ulStatus, "Error device overrun.");
            break;
        }
        //        DosRequestMutexSem(hmtxSnd, SEM_INDEFINITE_WAIT);
        rc=MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle,
                                   &(buffers[play]), 1);
        //        DosReleaseMutexSem(hmtxSnd);
        if (rc != MCIERR_SUCCESS) sound_err(rc, "Error writing to Mixer (pmixWrite).");

        break;
    case MIX_WRITE_COMPLETE: // 2           /* for playback  */
        //        if (Buffer[play]) {
        rc=MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle,
                                   &(buffers[play]), 1);
        //        } else {
        //            rc=MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle,
        //                                       &(buffers[7]), 1);
        //        }
        DosRequestMutexSem(hmtxSnd, SEM_INDEFINITE_WAIT);
        memset(buffers[last].pBuffer,0,BufferParms.ulBufferSize);
        last  = play++;
        play %= BufferParms.ulNumBuffers;
        DosReleaseMutexSem(hmtxSnd);
        if (rc != MCIERR_SUCCESS) sound_err(rc, "Error writing to Mixer (pmixWrite).");
        /* rc=MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle,
         &(buffers[cnt]), 1);
         played += BufferParms.ulBufferSize;
         played %= BufferParms.ulNumBuffers*BufferParms.ulBufferSize;
         cnt = (++cnt)%BufferParms.ulNumBuffers;
         DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
         memcpy(buffers[cnt].pBuffer, (ULONG)Buffer+played,BufferParms.ulBufferSize);
         DosReleaseMutexSem(hmtx);*/
        break;

    } /* end switch */
    return (TRUE);
}

static int dart_init(warn_t *w, const char *param, int *speed,
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

    log_message(LOG_DEFAULT, "MCI_MIXSETUP: %i by, #%i (suggested)", MixSetupParms.ulBufferSize,MixSetupParms.ulNumBuffers);
    log_message(LOG_DEFAULT, "MCI_MIXSETUP: %i by, #%i (vice)", *fragsize*sizeof(SWORD), *fragnr);

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
    
     BufferParms.ulNumBuffers = *fragnr;  //40;
     BufferParms.ulBufferSize = *fragsize*sizeof(SWORD); //4096;
     BufferParms.pBufList     = buffers;

     rc = mciSendCommand(usDeviceID, MCI_BUFFER, MCI_WAIT|MCI_ALLOCATE_MEMORY,
                         (PVOID) &BufferParms, 0);

     if (rc != MCIERR_SUCCESS) return sound_err(rc, "Error allocating Memory (MCI_ALLOCATE_MEMORY).");

     // MCI driver will return the number of buffers it
     // was able to allocate
     // it will also return the size of the information
     // allocated with each buffer.

     log_message(LOG_DEFAULT, "MCI_MIXSETUP: %i by, #%i", BufferParms.ulBufferSize,BufferParms.ulNumBuffers);

     /* Set the "end-of-stream" flag */
     //     buffers[*fragnr-1].ulFlags = MIX_BUFFER_EOS;

     for (ulLoop=0; ulLoop<*fragnr; ulLoop++)
         memset(buffers[ulLoop].pBuffer,0,*fragsize*sizeof(SWORD));

     DosCreateMutexSem("\\SEM32\\Snd", &hmtxSnd, 0, FALSE);

     //     DosCreateEventSem("\\SEM32\\Evt", &semNeedsBuffer, 0, FALSE);
     //     _beginthread(MixingThread,NULL,0x8000,NULL);

     // Must write at least two buffers to start mixer
     MixSetupParms.pmixWrite(MixSetupParms.ulMixHandle,
                             &(buffers[0]), 2);
     return 0;
}


static int dart_write(warn_t *w, SWORD *pbuf, size_t nr)
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

    static int pos;
    
    DosRequestMutexSem(hmtxSnd, SEM_INDEFINITE_WAIT);
    // if we wanna write the buffer which is played next
    // write the overnext.
    if (play==(pos+1)%BufferParms.ulNumBuffers)
        pos = (++pos)%BufferParms.ulNumBuffers;
    //    Buffer[pos]=TRUE;
    memcpy(buffers[pos++].pBuffer,pbuf,nr*sizeof(SWORD));
    pos %= BufferParms.ulNumBuffers;
    DosReleaseMutexSem(hmtxSnd);
    /*    static int cnt;
     int bufsz = BufferParms.ulBufferSize*BufferParms.ulNumBuffers;
     DosRequestMutexSem(hmtx, SEM_INDEFINITE_WAIT);
     if (pos+nr*sizeof(SWORD)>bufsz)
     {
     int first = pos+nr*sizeof(SWORD)-bufsz;
     int last  = bufsz-(pos+nr*sizeof(SWORD)-bufsz);;
     memcpy((ULONG)Buffer+pos,pbuf,      first);
     memcpy((ULONG)Buffer    ,pbuf+first,last);
     }
     else
     {
     memcpy((ULONG)Buffer+pos,pbuf,nr*sizeof(SWORD));
     }
     DosReleaseMutexSem(hmtx);
     pos += nr*sizeof(SWORD);
     pos %= bufsz;*/
    return 0;
}

/* return number of samples unplayed in the kernel buffer at the moment */
static int dart_bufferstatus(warn_t *s, int first)
{
    //    return 2048/sizeof(SWORD)*((pos+BufferParms.ulNumBuffers-played)%BufferParms.ulNumBuffers);
    //    int bufsz = BufferParms.ulBufferSize*BufferParms.ulNumBuffers;
    //    return ((pos+bufsz-played)%bufsz)/sizeof(SWORD);
    return 0;
}

static int dart_suspend(warn_t *w)
{
    return 0;
}

static int dart_resume(warn_t *w)
{
    return 0;
}

static sound_device_t mmos2_device =
{
    "dart",
    dart_init,   // dart_init
    dart_write,  // dart_write
    NULL,        // dart_dump
    NULL,        // dart_flush
    NULL,        // dart_bufferstatus
    dart_close,  // dart_close
    NULL,        // dart_suspend
    NULL         // dart_resume
};

int sound_init_dart_device(void)
{
    return sound_register_device(&mmos2_device);
}
