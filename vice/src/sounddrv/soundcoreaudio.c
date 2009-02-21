/*
 * soundcoreaudio.c - Implementation of the CoreAudio sound device.
 *
 * Written by
 *  Michael Klein <michael.klein@puffin.lb.shuttle.de>
 *  Christian Vogelgsang <C.Vogelgsang@web.de> (Ported to Intel Mac)
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

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>

#include "lib.h"
#include "log.h"
#include "sound.h"

/* ------------------------------------------------------------------------- */

typedef volatile int atomic_int_t;

static AudioDeviceID device = kAudioDeviceUnknown;
static AudioConverterRef converter = 0;

/* the cyclic buffer */
static SWORD *soundbuffer;

/* silence fragment */
static SWORD *silence;

/* current read position */
static unsigned int read_position;

/* the next position to write */
static unsigned int write_position;

/* Size of fragment (samples).  */
static unsigned int fragment_size;

/* Size of fragment (bytes).  */
static unsigned int fragment_byte_size;

/* total number of fragments */
static unsigned int fragment_count;

/* current number of fragments in buffer */
static atomic_int_t fragments_in_queue;

/* bytes per output packet/frame */
static unsigned int out_bytes_per_packet;

/* proc id */
#if defined(MAC_OS_X_VERSION_10_5) && (MAC_OS_X_VERSION_MIN_REQUIRED>=MAC_OS_X_VERSION_10_5)
AudioDeviceIOProcID procID;
#endif

/* ------------------------------------------------------------------------- */

#if defined(__x86_64__) || defined(__i386__)
/* Intel Mac Implementation */

static inline void atomic_increment(atomic_int_t * addr)
{
    __asm__ __volatile__ ("lock ; incl %0"
                          :"=m" (*addr)
                          :"m" (*addr));
}

static inline void atomic_decrement(atomic_int_t * addr)
{
    __asm__ __volatile__ ("lock ; decl %0"
                          :"=m" (*addr)
                          :"m" (*addr));
}

#else
/* PowerPC Mac Implementation */

static inline void atomic_add(atomic_int_t * addr, int val)
{
    register int tmp;
    asm volatile("    lwarx  %0,0,%2  \n\t"  /* load value & reserve */
                 "    addc   %0,%0,%3 \n\t"  /* add <val> */
                 "    stwcx. %0,0,%2  \n\n"  /* store new value */
                 "    bne-   $-12"           /* check if store was successful */
                 : "=&r"(tmp), "=m"(addr)
                 : "r"(addr), "r"(val), "m"(addr)
                 : "cr0"
                );
}

static inline void atomic_increment(atomic_int_t * addr)
{
    atomic_add(addr, 1);
}

static inline void atomic_decrement(atomic_int_t * addr)
{
    atomic_add(addr, -1);
}

#endif

/* ------------------------------------------------------------------------- */

#ifdef OLD_API

static OSStatus coreaudio_converter_inputproc(AudioConverterRef converter,
                                              UInt32 * io_data_size,
                                              void ** out_data,
                                              void * in_user_data)
{
    if (fragments_in_queue)
    {
        /* convert one fragment */
        *io_data_size = fragment_byte_size;
        *out_data = soundbuffer + fragment_size * read_position;

        read_position = (read_position + 1) % fragment_count;
        atomic_decrement(&fragments_in_queue);
    }
    else
    {
        /* output silence */
        *io_data_size = fragment_byte_size;
        *out_data = silence;
    }

    return kAudioHardwareNoError;
}

#else

static OSStatus coreaudio_converter_inputproc(AudioConverterRef inAudioConverter, 
                                              UInt32 * ioNumberDataPackets, 
                                              AudioBufferList * ioData, 
                                              AudioStreamPacketDescription** outDataPacketDescription, 
                                              void * inUserData)
{
  SWORD *buffer;
  if (fragments_in_queue)
  {
      /* convert one fragment */
      buffer = soundbuffer + fragment_size * read_position;

      read_position = (read_position + 1) % fragment_count;
      atomic_decrement(&fragments_in_queue);
  }
  else
  {
      /* output silence */
      buffer = silence;
  }

  ioData->mBuffers[0].mDataByteSize = fragment_byte_size;
  ioData->mBuffers[0].mData = buffer;
  *ioNumberDataPackets = fragment_size;

  return kAudioHardwareNoError;
}

#endif

static OSStatus coreaudio_ioproc(AudioDeviceID device,
                                 const AudioTimeStamp  * now,
                                 const AudioBufferList * input_data,
                                 const AudioTimeStamp  * input_time,
                                 AudioBufferList       * output_data,
                                 const AudioTimeStamp  * output_time,
                                 void                  * client_data)
{
#ifdef OLD_API
    return AudioConverterFillBuffer(converter,
                                    coreaudio_converter_inputproc,
                                    NULL,
                                    &output_data->mBuffers[0].mDataByteSize,
                                    output_data->mBuffers[0].mData);
#else
    UInt32 dataPacketSize = fragment_size;
    UInt32 bufferPacketSize = output_data->mBuffers[0].mDataByteSize / out_bytes_per_packet;
    if(dataPacketSize > bufferPacketSize)
        dataPacketSize = bufferPacketSize;

    return AudioConverterFillComplexBuffer(converter,
                                    coreaudio_converter_inputproc,
                                    NULL,
                                    &dataPacketSize,
                                    output_data,
                                    NULL);
#endif
}


/* ------------------------------------------------------------------------- */

static int coreaudio_suspend(void)
{
    AudioDeviceStop(device, coreaudio_ioproc);
    return 0;
}

static int coreaudio_resume(void)
{
    OSStatus err = AudioDeviceStart(device, coreaudio_ioproc);
    if (err != kAudioHardwareNoError)
    {
    	log_error(LOG_DEFAULT,
                  "sound (coreaudio_init): could not start IO proc: err=%d", (int)err);
        return -1;
    }
    return 0;
}

static int coreaudio_init(const char *param, int *speed,
                          int *fragsize, int *fragnr, int *channels)
{
    OSStatus err;
    UInt32 size;

    AudioStreamBasicDescription in;
    AudioStreamBasicDescription out;

    size = sizeof(device);
    err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,
                                   &size, (void*)&device);
    if(err != kAudioHardwareNoError)
    {
        log_error(LOG_DEFAULT, "sound (coreaudio_init): Failed to get default output device");
        return -1;
    }

    size = sizeof(out);
    err = AudioDeviceGetProperty(device, 0, false,
                                 kAudioDevicePropertyStreamFormat,
                                 &size, (void*)&out);
    if(err != kAudioHardwareNoError)
    {
        log_error(LOG_DEFAULT, "sound (coreaudio_init): stream format not support");
        return -1;
    }
    
    out_bytes_per_packet = out.mBytesPerPacket;
    
    if ((int)out.mSampleRate != *speed)
    {
        log_warning(LOG_DEFAULT, "sound (coreaudio_init): sampling rate conversion %dHz->%dHz",
                    *speed, (int)out.mSampleRate);
    }

    in.mChannelsPerFrame = *channels;
    in.mSampleRate = (float)*speed;
    in.mFormatID = kAudioFormatLinearPCM;
#if defined(__x86_64__) || defined(__i386__)
    in.mFormatFlags = kAudioFormatFlagIsSignedInteger;
#else
    in.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
#endif
    in.mBytesPerFrame = sizeof(SWORD) * *channels;
    in.mBytesPerPacket = in.mBytesPerFrame;
    in.mFramesPerPacket = 1;
    in.mBitsPerChannel = 8 * sizeof(SWORD);
    in.mReserved = 0;

    err = AudioConverterNew(&in, &out, &converter);
    if(err != noErr)
    {
        log_error(LOG_DEFAULT,
                  "sound (coreaudio_init): could not create AudioConverter: err=%d", (int)err);
        return -1;
    }

    /* duplicate mono stream to all output channels */
    if (*channels == 1 && out.mChannelsPerFrame > 1)
    {
        Boolean writable;
        err = AudioConverterGetPropertyInfo(converter, kAudioConverterChannelMap, &size, &writable);
        if (err == noErr && writable)
        {
            SInt32 * channel_map = lib_malloc(size);
            if (channel_map)
            {
                memset(channel_map, 0, size);
                AudioConverterSetProperty(converter, kAudioConverterChannelMap, size, channel_map);
                lib_free(channel_map);
            }
        }
    }

    fragment_count = *fragnr;
    fragment_size  = *fragsize;

    fragment_byte_size  = fragment_size * sizeof(SWORD);

    soundbuffer = (SWORD*)lib_calloc(fragment_count, fragment_byte_size);
    silence = (SWORD*)lib_calloc(1, fragment_byte_size);

    read_position = 0;
    write_position = 0;
    fragments_in_queue = 0;

#if defined(MAC_OS_X_VERSION_10_5) && (MAC_OS_X_VERSION_MIN_REQUIRED>=MAC_OS_X_VERSION_10_5)
    err = AudioDeviceCreateIOProcID( device, coreaudio_ioproc, NULL, &procID );
#else
    err = AudioDeviceAddIOProc( device, coreaudio_ioproc, NULL );
#endif
    if (err != kAudioHardwareNoError)
    {
    	log_error(LOG_DEFAULT,
                  "sound (coreaudio_init): could not add IO proc: err=%d", (int)err);
        return -1;
    }

    coreaudio_resume();

    return 0;
}

static int coreaudio_write(SWORD *pbuf, size_t nr)
{
    int i, count;

    /* number of fragments */
    count = nr / fragment_size;

    for (i = 0; i < count; i++)
    {
        if(fragments_in_queue >= (fragment_count-1))
        {
            log_warning(LOG_DEFAULT, "sound (coreaudio): buffer overrun");

            /* block */
            while (fragments_in_queue >= fragment_count)
                ;
        }

        memcpy(soundbuffer + fragment_size * write_position,
               pbuf + i * fragment_size,
               fragment_byte_size);

        write_position = (write_position + 1) % fragment_count;

        atomic_increment(&fragments_in_queue);
    }

    return 0;
}

static int coreaudio_bufferspace(void)
{
    int ret;

    if (fragment_count == fragments_in_queue)
        ret = 0;
    else
        ret = (fragment_count - fragments_in_queue - 1) * fragment_size;

    /*
     * FIXME: ideally we should just return the available space here;
     * but this causes massive buffer overruns in coreaudio_write().
     */
    return ret * 2 / 3;
}

static void coreaudio_close(void)
{
#if defined(MAC_OS_X_VERSION_10_5) && (MAC_OS_X_VERSION_MIN_REQUIRED>=MAC_OS_X_VERSION_10_5)
    AudioDeviceDestroyIOProcID(device, procID);
#else
    AudioDeviceRemoveIOProc(device, coreaudio_ioproc);
#endif
    if (converter)
    {
        AudioConverterDispose(converter);
        converter = 0;
    }
    lib_free(soundbuffer);
    lib_free(silence);
}


static sound_device_t coreaudio_device =
{
    "coreaudio",
    coreaudio_init,
    coreaudio_write,
    NULL,
    NULL,
    coreaudio_bufferspace,
    coreaudio_close,
    coreaudio_suspend,
    coreaudio_resume,
    1
};


int sound_init_coreaudio_device(void)
{
    return sound_register_device(&coreaudio_device);
}
