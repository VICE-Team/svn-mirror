/*
 * sounddx.c - Implementation of the DirectSound sound device.
 *
 * Written by
 *  Tibor Biczo (crown@mail.matav.hu)
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include <dsound.h>

#include "sound.h"
#include "ui.h"

/*  FIXME: It would be better to convert the dxguid.lib from DX5 into the Mingw32 port of DX5 */
const GUID IID_IDirectSoundNotify={0xb0210783,0x89cd,0x11d0,{0xaf,0x08,0x00,0xa0,0xc9,0x25,0xcd,0x16}};

/* ------------------------------------------------------------------------ */

/*#define DEBUG_SOUND*/

/* Debugging stuff.  */
#ifdef DEBUG_SOUND
static void sound_debug(const char *format, ...)
{
        char tmp[1024];
        va_list args;

        va_start(args, format);
        vsprintf(tmp, format, args);
        va_end(args);
        OutputDebugString(tmp);
        printf(tmp);
}
#define DEBUG(x) sound_debug x
#else
#define DEBUG(x)
#endif

static char *ds_error(HRESULT result)
{
    switch (result)
    {
      case DSERR_ALLOCATED:
        return "Already allocated resource";
      case DSERR_CONTROLUNAVAIL:
        return "Control not available";
      case DSERR_INVALIDPARAM:
        return "Parameter not valid";
      case DSERR_INVALIDCALL:
        return "Call not valid";
      case DSERR_GENERIC:
        return "Generic error";
      case DSERR_PRIOLEVELNEEDED:
        return "Priority level needed";
      case DSERR_OUTOFMEMORY:
        return "Out of memory";
      case DSERR_BADFORMAT:
        return "Specified WAVE format not supported";
      case DSERR_UNSUPPORTED:
        return "Not supported";
      case DSERR_NODRIVER:
        return "No sound driver is available for use";
      case DSERR_ALREADYINITIALIZED:
        return "Object already initialized";
      case DSERR_NOAGGREGATION:
        return "Object does not support aggregation";
      case DSERR_BUFFERLOST:
        return "Buffer lost";
      case DSERR_OTHERAPPHASPRIO:
        return "Another app has a higher priority level";
      case DSERR_UNINITIALIZED:
        return "Object not initialized";
      case DSERR_NOINTERFACE:
        return "Requested COM interface is not available";
      default:
        return "Whadda hell?!";
    }
}

/* ------------------------------------------------------------------------ */

/* DirectSound object.  */
static LPDIRECTSOUND ds=NULL;

/* Audio buffer.  */
static LPDIRECTSOUNDBUFFER buffer=NULL;

/* Buffer offset.  */
static DWORD buffer_offset;

/* Buffer size.  */
static DWORD buffer_size;

/* Fragment size.  */
static int fragment_size;

/* Flag: are we in exclusive mode?  */
static int is_exclusive;

/*  DirectSoundNotify Interface, if present */
static LPDIRECTSOUNDNOTIFY      notify;

typedef enum {
        STREAM_NONE,
        STREAM_NOTIFY,
        STREAM_TIMER
} streammode_t;

/*  Flag: streaming mode */
static streammode_t         streammode=STREAM_NONE;

/*  Notify Position Array */
static DSBPOSITIONNOTIFY    *notifypositions;

/*  Notify Event */
static HANDLE               notifyevent;

/*  ID of Notify Thread */
static DWORD                notifyThreadID;

/*  Handle of Notify Thread */
static HANDLE               notifyThreadHandle;

/*  Flag: to sygnal notify thread if there is any fragment to handle */
static volatile int         fragment_waiting=0;

/*  Pointer for waiting fragment */
static LPVOID               fragment_pointer;

/*  Offset of sound lag */
static int                  lag_offset;

/*  Last played sample. This will be played in underflow condition */
static WORD                 last_played_sample=0;

/*  Flag: is soundcard a 16bit or 8bit card? */
static int                  is16bit;

/* ------------------------------------------------------------------------ */

DWORD WINAPI HandleNotifications(LPVOID lpparam)
{
DWORD       play_cursor, write_cursor;
HRESULT     result;
DWORD       buffer_lock_size;
LPVOID      lpvPtr1,lpvPtr2;
DWORD       dwBytes1,dwBytes2;
int         i;
SWORD       *copyptr;

    buffer_lock_size=fragment_size*(is16bit ? sizeof(SWORD) : 1);

    while (WaitForSingleObject(notifyevent,INFINITE)!=WAIT_FAILED) {
        result=IDirectSoundBuffer_GetCurrentPosition(buffer,&play_cursor,&write_cursor);
        DEBUG(("Notify play %d write %d % buffer %d\n",play_cursor,write_cursor,buffer_offset));
        result=IDirectSoundBuffer_Lock(buffer,buffer_offset,buffer_lock_size,&lpvPtr1, &dwBytes1,&lpvPtr2, &dwBytes2,0);

        if (result==DSERR_BUFFERLOST) {
            IDirectSoundBuffer_Restore(buffer);
            result=IDirectSoundBuffer_Lock(buffer,buffer_offset,buffer_lock_size,&lpvPtr1,&dwBytes1,&lpvPtr2,&dwBytes2,0);
        }

        if (result==DS_OK) {
            /*  Now lets check underflow condition */
            if (!fragment_waiting) {
                if (is16bit) {
                    for (i=0; i<dwBytes1/2; i++) {
                        ((WORD*)lpvPtr1)[i]=last_played_sample;
                    }
                    if (lpvPtr2!=NULL) {
                        for (i=0; i<dwBytes2/2; i++) {
                            ((WORD*)lpvPtr2)[i]=last_played_sample;
                        }
                    }
                } else {
                    for (i=0; i<dwBytes1; i++) {
                        ((BYTE*)lpvPtr1)[i]=last_played_sample;
                    }
                    if (lpvPtr2!=NULL) {
                        for (i=0; i<dwBytes2; i++) {
                            ((BYTE*)lpvPtr2)[i]=last_played_sample;
                        }
                    }
                }
            } else {
                if (is16bit) {
                    memcpy(lpvPtr1,fragment_pointer,dwBytes1);
                    if (lpvPtr2!=NULL) {
                        memcpy(lpvPtr2,(BYTE *)fragment_pointer+dwBytes1,dwBytes2);
                    }
                    last_played_sample=*((WORD*)(fragment_pointer+buffer_lock_size)-1);
                } else {
                    copyptr=fragment_pointer;
                    for (i=0; i<dwBytes1; i++) {
                        ((BYTE*)lpvPtr1)[i]=(*(copyptr++)>>8)+0x80;
                    }
                    if (lpvPtr2!=NULL) {
                        for (i=0; i<dwBytes2; i++) {
                            ((BYTE*)lpvPtr2)[i]=(*(copyptr++)>>8)+0x80;
                        }
                    }
                    last_played_sample=(*(--copyptr)>>8)+0x80;
                }
                fragment_waiting=0;
            }
            result=IDirectSoundBuffer_Unlock(buffer,lpvPtr1,dwBytes1,lpvPtr2,dwBytes2);
            /*  Set up write pointer for next fragment */
            buffer_offset+=buffer_lock_size;
            if (buffer_offset==buffer_size) buffer_offset=0;
        }
    }
}

DSBUFFERDESC desc;
PCMWAVEFORMAT pcmwf;
DSCAPS  capabilities;

static int dx_init(warn_t *w, const char *param, int *speed,
                   int *fragsize, int *fragnr, double bufsize)
{
HRESULT result;
int     i;

    DEBUG(("DirectSound driver initialization: speed = %d, fragsize = %d, fragnr = %d, bufsize = %.4f\n",
           *speed, *fragsize, *fragnr, bufsize));

    if (ds == NULL) {
        result = DirectSoundCreate(NULL, &ds, NULL);
        if (result != DS_OK) {
            ui_error("Cannot initialize DirectSound:\n%s", ds_error(result));
            return -1;
        }

#if 0
        result = IDirectSound_SetCooperativeLevel(ds, ui_get_main_hwnd(),
                                                  DSSCL_WRITEPRIMARY);
#else
        /* FIXME: Hack.  For now, we only use `DSSCL_NORMAL' as the cooperative
           level.  */
        result = DS_OK + 1;
#endif
        if (result != DS_OK) {
            DEBUG(("Cannot set DDSCL_WRITEPRIMARY CooperativeLevel: %s\n",
                   ds_error(result)));
            result = IDirectSound_SetCooperativeLevel(ds, ui_get_main_hwnd(),
                                                      DSSCL_NORMAL);
            if (result != DS_OK) {
                ui_error("Cannot set cooperative level:\n%s",
                         ds_error(result));
                return -1;
            }
        }

#if 0
        ui_error("DirectSound cooperative level set:\n%s",
                 is_exclusive ? "exclusive" : "normal");
#endif
    }

/*  Kill previous buffer first */
    if (buffer != NULL) {
        IDirectSoundBuffer_Stop(buffer);
        if (streammode==STREAM_NOTIFY) {
            CloseHandle(notifyThreadHandle);
            CloseHandle(notifyevent);
            free(notifypositions);
        }
        IDirectSoundBuffer_Release(buffer);
    }

    IDirectSound_GetCaps(ds,&capabilities);
    if (capabilities.dwFlags&DSCAPS_SECONDARY16BIT) {
        is16bit=1;
    } else {
        is16bit=0;
    }

    memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
    pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
    pcmwf.wf.nChannels = 1;
    pcmwf.wf.nSamplesPerSec = *speed;
    pcmwf.wBitsPerSample = is16bit ? 16 : 8;
/* Hack to fix if mmsystem header is bad
    ((WORD*)&pcmwf)[7]=16;
*/
    pcmwf.wf.nBlockAlign = is16bit ? 2 : 1;
    pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;

    memset(&desc, 0, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2  | DSBCAPS_CTRLDEFAULT ;

    fragment_size = *fragsize;

    buffer_size = *fragsize * *fragnr * (is16bit ? sizeof(SWORD) : 1);
    desc.dwBufferBytes = buffer_size;
    desc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;

    result = IDirectSound_CreateSoundBuffer(ds, &desc, &buffer, NULL);
    if (result != DS_OK) {
        ui_error("Cannot create DirectSound buffer:\n%s", ds_error(result));
        return -1;
    }

    /*  Now let's check if IDirectSoundNotify interface is available or not. */
    /*  It should be there if the user has DX5 or higher...*/
    /*  On NT with DX3, streaming will not be implemented right now */
    /*  FIXME */

    result=IDirectSoundNotify_QueryInterface(buffer,(GUID *)&IID_IDirectSoundNotify,(LPVOID FAR *)&notify);

    if (SUCCEEDED(result)) {
        streammode=STREAM_NOTIFY;
        notifypositions=malloc(*fragnr*sizeof(DSBPOSITIONNOTIFY));
        notifyevent=CreateEvent(NULL,FALSE,FALSE,NULL);
        for (i=0; i<*fragnr; i++) {
            notifypositions[i].dwOffset=*fragsize*i*(is16bit ? sizeof(SWORD) : 1);
            notifypositions[i].hEventNotify=notifyevent;
        }
        /*  Set write pointer for last fragment in buffer, to guarantee maximum safe area */
        buffer_offset=*fragsize*(is16bit ? sizeof(SWORD) : 1)*(*fragnr-1);
        lag_offset=buffer_offset;
        notifyThreadHandle=CreateThread(NULL,0,HandleNotifications,NULL,0,&notifyThreadID);
        notify->lpVtbl->SetNotificationPositions(notify,*fragnr,notifypositions);
    } else {
        streammode=STREAM_NONE;
        /*  Set write pointer for first fragment in buffer */
        buffer_offset=0;
        lag_offset=buffer_offset;
    }

    /* Let's go...  */
    result = IDirectSoundBuffer_Play(buffer, 0, 0, DSBPLAY_LOOPING);
    if (result == DSERR_BUFFERLOST) {
        ui_error("Restoring DirectSound buffer.");
        if ((result = IDirectSoundBuffer_Restore(buffer)) != DS_OK)
            ui_error("Cannot restore buffer:\n%s", ds_error(result));
        result = IDirectSoundBuffer_Play(buffer, 0, 0, DSBPLAY_LOOPING);
    }
    if (result != DS_OK) {
        ui_error("Cannot play DirectSound buffer:\n%s", ds_error(result));
        return -1;
    }

    DEBUG(("DirectSound initialization done succesfully.\n"));

    return 0;
}

static void dx_close(warn_t *w)
{
    IDirectSoundBuffer_Stop(buffer);
}

static int dx_bufferstatus(warn_t *s, int first)
{
    DWORD play_cursor, write_cursor;
    HRESULT result;
    int value;

    if (first) {

        /* No buffered bytes so far.  */

        return 0;

    }

    result = IDirectSoundBuffer_GetCurrentPosition(buffer,
                                                   &play_cursor,
                                                   &write_cursor);
    if (result != DS_OK)
        return 0; /* FIXME */

    value = (int)buffer_offset - (int)play_cursor;
    if (value < 0)
        value += buffer_size;
//    value-=lag_offset;
//    if (value<0) value=0;
    DEBUG(("buffer status %d %d %d \n",play_cursor,buffer_offset,value));
    if (is16bit) {
        return value / sizeof(SWORD);
    } else {
        return value;
    }
}

static int dx_write(warn_t *w, SWORD *pbuf, int nr)
{
    LPVOID lpvPtr1;
    DWORD dwBytes1;
    LPVOID lpvPtr2;
    DWORD dwBytes2;
    HRESULT result;
    DWORD buffer_lock_size, buffer_lock_end;
    int i, count;

    /* XXX: Assumes `nr' is multiple of `fragment_size'.  */
    count = nr / fragment_size;
    buffer_lock_size = fragment_size * (is16bit ? sizeof(SWORD) : 1);

    /* Write one fragment at a time.  FIXME: This could be faster.  */
    for (i = 0; i < count; i++) {
        switch (streammode) {
            case STREAM_NONE:
                DEBUG(("dx_write(), offset = %d, nr = %d\n", buffer_offset, nr));

                /* XXX: We do not use module here because we assume we always write
                full fragments.  */
                buffer_lock_end = buffer_offset + buffer_lock_size - 1;

                DEBUG(("buffer_lock_end %d\n", buffer_lock_end));

                /* Block if we are writing too early.  Notice that we also assume
                that the part of the buffer we are going to lock is small enough
                to fit in the safe space.  FIXME: Is there a better
                (non-busy-waiting) way?)  */
                {
                    DWORD play_cursor, write_cursor;

                    while (1) {
                        result = IDirectSoundBuffer_GetCurrentPosition(buffer,
                                                                    &play_cursor,
                                                                    &write_cursor);
                        if (result != DS_OK)
                            break;
                        DEBUG(("play %d write %d\n", play_cursor, write_cursor));
                        if (write_cursor > play_cursor) {
                            if (buffer_offset >= write_cursor
                                || buffer_lock_end < play_cursor)
                                break;
                        } else {
                            if (buffer_lock_end < play_cursor
                                && buffer_offset >= write_cursor)
                                break;
                        }
                    }
                }

                DEBUG(("Locking %d...%d\n", buffer_offset, buffer_lock_end));

                /* Obtain write pointer.  */
                result = IDirectSoundBuffer_Lock(buffer, buffer_offset,
                                                buffer_lock_size,
                                                &lpvPtr1, &dwBytes1,
                                                &lpvPtr2, &dwBytes2,
                                                0);

                if (result == DSERR_BUFFERLOST) {
                    IDirectSoundBuffer_Restore(buffer);
                    result = IDirectSoundBuffer_Lock(buffer, buffer_offset,
                                                    buffer_lock_size,
                                                    &lpvPtr1, &dwBytes1,
                                                    &lpvPtr2, &dwBytes2,
                                                    0);
                }

                if (result == DS_OK) {
                    if (is16bit) {
                        memcpy(lpvPtr1, pbuf, dwBytes1);
                        if (lpvPtr2 != NULL)
                            memcpy(lpvPtr2, (BYTE *) pbuf + dwBytes1, dwBytes2);
                    } else {
                        SWORD *copybuff=pbuf;

                        for (i=0; i<dwBytes1; i++) {
                            ((BYTE*)lpvPtr1)[i]=(*(copybuff++)>>8)+0x80;
                        }
                        if (lpvPtr2!=NULL) {
                            for (i=0; i<dwBytes2; i++) {
                                ((BYTE*)lpvPtr2)[i]=(*(copybuff++)>>8)+0x80;
                            }
                        }
                    }
                    result = IDirectSoundBuffer_Unlock(buffer, lpvPtr1, dwBytes1, lpvPtr2,
                                                    dwBytes2);
                    if (result != DS_OK)
                        DEBUG(("dx_write(): cannot unlock: %s\n", ds_error(result)));
                } else {
                    DEBUG(("dx_write(): cannot lock: %s\n", ds_error(result)));
                }

                buffer_offset += buffer_lock_size;
                if (buffer_offset >= buffer_size)
                    buffer_offset -= buffer_size;
                break;
            case STREAM_NOTIFY:
                /*  Wait for previous fragment serviced */
                /*  FIXME should be not a busy wait */
                while (fragment_waiting);
                fragment_pointer=pbuf;
                fragment_waiting=1;
                break;
        }
        pbuf+=fragment_size;
    }
    return 0;
}

static sound_device_t dx_device =
{
    "dx",
    dx_init,
    dx_write,
    NULL,
    NULL,
    dx_bufferstatus,
    dx_close,
    NULL,
    NULL
};

int sound_init_dx_device(void)
{
    return sound_register_device(&dx_device);
}