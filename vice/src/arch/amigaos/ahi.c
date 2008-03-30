/*
 * ahi.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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

#ifdef HAVE_DEVICES_AHI_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dos/dostags.h>
#include <exec/exec.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <devices/ahi.h>
#include <devices/audio.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "ahi.h"
#include "timer.h"

#define PRINTF(a...)
#define DEBUG(a...)

typedef struct audio_buffer_s {
  void *buffer;
  s32 size;
  s64 time;
  s32 used;
} audio_buffer_t;

typedef struct audio_s {
  s32 frequency;
  u32 mode;
  s32 fragsize;
  s32 frags;
  audio_buffer_t *audio_buffers;
  s64 current_time;
  s32 read_buffer;
  s32 write_buffer;
  void (*audio_sync)(s64 time);

  s32 play;
  s32 samples_to_bytes;

  s32 read_position;
  struct timeval tv;
  s32 write_position;

  /* tasks 'n' stuff */
  struct Task *main_task;
  struct Task *audio_task;
  struct SignalSemaphore *semaphore;
  struct Process *audio_process;
#ifdef AMIGA_OS4
  timer_t *timer;
#endif
} audio_t;

static audio_t audio;

/* to task:
 * SIGBREAKF_CTRL_C -
 * SIGBREAKF_CTRL_D wake up
 * SIGBREAKF_CTRL_E exit
 * SIGBREAKF_CTRL_F -
 *
 * from task:
 * SIGBREAKF_CTRL_C -
 * SIGBREAKF_CTRL_D wake up
 * SIGBREAKF_CTRL_E error
 * SIGBREAKF_CTRL_F -
 */

static char ahi_task_name[] = "AudioServerTask0000";

static void make_task_name(void)
{
  int task_number = 0;

  for (;;) {
    sprintf(ahi_task_name, "AudioServerTask%04d", task_number);

#ifndef AMIGA_OS4
    if (FindTask(ahi_task_name) == NULL) {
#else
    if (IExec->FindTask(ahi_task_name) == NULL) {
#endif
      break;
    } else {
      task_number++;
    }
  }
}

#define FLAG_EXIT (1 << 0)

static void ahi_task(void)
{
  struct AHIRequest *AHIIO = NULL, *AHIIO1 = NULL, *AHIIO2 = NULL;
  struct MsgPort *AHIMP1 = NULL, *AHIMP2 = NULL;
  u32 signals, flags;
  s32 device, used;
  struct AHIRequest *link = NULL;
#ifdef AMIGA_OS4
  timer_t *timer = NULL;
#endif
  s32 previous_read_buffer = -1;

DEBUG("task started\n");

  flags = 0;
  signals = 0;
  device = 1;

#ifndef AMIGA_OS4
  if ((timer_init())) {
    if ((AHIMP1 = CreateMsgPort())) {
      if ((AHIMP2 = CreateMsgPort())) {
        if ((AHIIO1 = (struct AHIRequest *)CreateIORequest(AHIMP1, sizeof(struct AHIRequest)))) {
          if ((AHIIO2 = (struct AHIRequest *)CreateIORequest(AHIMP2, sizeof(struct AHIRequest)))) {
            device = OpenDevice(AHINAME, 0, (struct IORequest *)AHIIO1, 0);
#else
  if ((timer = timer_init())) {
    if ((AHIMP1 = IExec->CreateMsgPort())) {
      if ((AHIMP2 = IExec->CreateMsgPort())) {
        if ((AHIIO1 = (struct AHIRequest *)IExec->CreateIORequest(AHIMP1, sizeof(struct AHIRequest)))) {
          if ((AHIIO2 = (struct AHIRequest *)IExec->CreateIORequest(AHIMP2, sizeof(struct AHIRequest)))) {
            device = IExec->OpenDevice(AHINAME, 0, (struct IORequest *)AHIIO1, 0);
#endif
          }
        }
      }
    }
  }

  if (device == 0) {
DEBUG("AHI open\n");
    *AHIIO2 = *AHIIO1;
    AHIIO1->ahir_Std.io_Message.mn_ReplyPort = AHIMP1;
    AHIIO2->ahir_Std.io_Message.mn_ReplyPort = AHIMP2;
    AHIIO1->ahir_Std.io_Message.mn_Node.ln_Pri = 127;
    AHIIO2->ahir_Std.io_Message.mn_Node.ln_Pri = 127;
    AHIIO = AHIIO1;

PRINTF("open done\n");

#ifndef AMIGA_OS4
    Signal(audio.main_task, SIGBREAKF_CTRL_D);
#else
    IExec->Signal(audio.main_task, SIGBREAKF_CTRL_D);
#endif

    for (;;) {
      if (flags & FLAG_EXIT) {
        break;
      }

#ifndef AMIGA_OS4
      ObtainSemaphore(audio.semaphore);
#else
      IExec->ObtainSemaphore(audio.semaphore);
#endif
      used = audio.audio_buffers[audio.read_buffer].used;
#ifndef AMIGA_OS4
      ReleaseSemaphore(audio.semaphore);
#else
      IExec->ReleaseSemaphore(audio.semaphore);
#endif

      if (audio.play & used) {

PRINTF("playing buffer: %d (%d)\n", audio.read_buffer, length);

        AHIIO->ahir_Std.io_Command = CMD_WRITE;
        AHIIO->ahir_Std.io_Data    = audio.audio_buffers[audio.read_buffer].buffer;
        AHIIO->ahir_Std.io_Length  = audio.audio_buffers[audio.read_buffer].size;
        AHIIO->ahir_Std.io_Offset  = 0;
        AHIIO->ahir_Frequency      = audio.frequency;
        AHIIO->ahir_Type           = audio.mode;
        AHIIO->ahir_Volume         = 0x10000; // Volume:  0dB
        AHIIO->ahir_Position       = 0x08000; // Panning: center
        AHIIO->ahir_Link           = link;

#ifndef AMIGA_OS4
        SendIO((struct IORequest *)AHIIO);
#else
        IExec->SendIO((struct IORequest *)AHIIO);
#endif

        if (link != NULL) {
          u32 sigbit = 1L << link->ahir_Std.io_Message.mn_ReplyPort->mp_SigBit;

PRINTF("want: %08lx\n", sigbit);

          for (;;) {
#ifndef AMIGA_OS4
            signals = Wait(SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_D | sigbit);
#else
            signals = IExec->Wait(SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_D | sigbit);
#endif

PRINTF("got %08lx\n", signals);

            if (signals & SIGBREAKF_CTRL_E) {
              flags |= FLAG_EXIT;
            }

            /* break when we got what we want */
            if (signals & sigbit) {
              break;
            }
          }

#ifndef AMIGA_OS4
          WaitIO((struct IORequest *)link);
#else
          IExec->WaitIO((struct IORequest *)link);
#endif
        }

#ifndef AMIGA_OS4
        ObtainSemaphore(audio.semaphore);
#else
        IExec->ObtainSemaphore(audio.semaphore);
#endif

        if (previous_read_buffer >= 0) {
          audio.audio_buffers[previous_read_buffer].used = 0;
        }

        audio.read_position = (audio.read_buffer * audio.fragsize);
#ifndef AMIGA_OS4
        timer_gettime(&audio.tv);
#else
        timer_gettime(timer, &audio.tv);
#endif

        link = AHIIO;

        if (AHIIO == AHIIO1) {
          AHIIO = AHIIO2;
        } else {
          AHIIO = AHIIO1;
        }

        previous_read_buffer = audio.read_buffer;
        audio.read_buffer++;
        if (audio.read_buffer >= audio.frags) {
          audio.read_buffer = 0;
        }

#ifndef AMIGA_OS4
        ReleaseSemaphore(audio.semaphore);
#else
        IExec->ReleaseSemaphore(audio.semaphore);
#endif

        if (audio.audio_sync != NULL) {
          audio.audio_sync(audio.audio_buffers[previous_read_buffer].time);
        }

#ifndef AMIGA_OS4
        Signal(audio.main_task, SIGBREAKF_CTRL_D);
#else
        IExec->Signal(audio.main_task, SIGBREAKF_CTRL_D);
#endif
      } else {
PRINTF("nothing to play... waiting...\n");
#ifndef AMIGA_OS4
        signals = Wait(SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_D);
#else
        signals = IExec->Wait(SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_D);
#endif
PRINTF("got %08lx\n", signals);
        if (signals & SIGBREAKF_CTRL_E) {
          flags |= FLAG_EXIT;
        }
      }
    }
  } else {
DEBUG("AHI failed\n");
  }

DEBUG("wait for last buffer\n");

  if (link != NULL) {
    u32 sigbit = 1L << link->ahir_Std.io_Message.mn_ReplyPort->mp_SigBit;

PRINTF("want: %08lx\n", sigbit);

#ifndef AMIGA_OS4
    signals = Wait(sigbit);
#else
    signals = IExec->Wait(sigbit);
#endif

PRINTF("got %08lx\n", signals);

#ifndef AMIGA_OS4
    WaitIO((struct IORequest *)link);
#else
    IExec->WaitIO((struct IORequest *)link);
#endif
  }

DEBUG("close ahi\n");

  if (device == 0) {
#ifndef AMIGA_OS4
    CloseDevice((struct IORequest *)AHIIO1);
#else
    IExec->CloseDevice((struct IORequest *)AHIIO1);
#endif
  }
  if (AHIIO2 != NULL) {
#ifndef AMIGA_OS4
    DeleteIORequest((struct IORequest *)&AHIIO2->ahir_Std);
#else
    IExec->DeleteIORequest((struct IORequest *)&AHIIO2->ahir_Std);
#endif
  }
  if (AHIIO1 != NULL) {
#ifndef AMIGA_OS4
    DeleteIORequest((struct IORequest *)&AHIIO1->ahir_Std);
#else
    IExec->DeleteIORequest((struct IORequest *)&AHIIO1->ahir_Std);
#endif
  }
  if (AHIMP2 != NULL) {
#ifndef AMIGA_OS4
    DeleteMsgPort(AHIMP2);
#else
    IExec->DeleteMsgPort(AHIMP2);
#endif
  }
  if (AHIMP1 != NULL) {
#ifndef AMIGA_OS4
    DeleteMsgPort(AHIMP1);
#else
    IExec->DeleteMsgPort(AHIMP1);
#endif
  }
#ifndef AMIGA_OS4
  timer_exit();
#else
  if (timer != NULL) {
    timer_exit(timer);
  }
#endif

DEBUG("AHI closed\n");

  /* send error signal in case we failed */
#ifndef AMIGA_OS4
  Signal(audio.main_task, SIGBREAKF_CTRL_E);
#else
  IExec->Signal(audio.main_task, SIGBREAKF_CTRL_E);
#endif
}

s32 ahi_open(s32 frequency, u32 mode, s32 fragsize, s32 frags, void (*callback)(s64 time))
{
  struct TagItem ti[]={{NP_Entry, (ULONG)ahi_task},
                       {NP_Name, (ULONG)ahi_task_name},
                       {NP_StackSize, 4 * 65536}, /* 64KB should be enough */
                       {NP_Priority, 10},
                       {TAG_DONE, 0}};
  s32 i;
  u32 signals;

  /* reset structure */
  memset(&audio, 0, sizeof(audio_t));

#ifndef AMIGA_OS4
  timer_init();
#else
  audio.timer = timer_init();
  if (audio.timer == NULL) {
    goto fail;
  }
#endif

  /* samples to bytes */
  audio.samples_to_bytes = 1;
  if (mode & AUDIO_MODE_16BIT) {
    audio.samples_to_bytes *= 2;
  }
  if (mode & AUDIO_MODE_STEREO) {
    audio.samples_to_bytes *= 2;
  }
  audio.samples_to_bytes >>= 1; /* we use shifts */

  fragsize <<= audio.samples_to_bytes;

  /* store settings */
  audio.frequency = frequency;
  audio.mode = mode;
  audio.fragsize = fragsize;
  audio.frags = frags;
  audio.audio_sync = callback;
  audio.read_position = -1;

DEBUG("alloc buffers\n");

  /* allocate buffers */
#ifndef AMIGA_OS4
  audio.audio_buffers = AllocVec(audio.frags * sizeof(audio_buffer_t), MEMF_PUBLIC | MEMF_CLEAR);
#else
  audio.audio_buffers = IExec->AllocVec(audio.frags * sizeof(audio_buffer_t), MEMF_PUBLIC | MEMF_CLEAR);
#endif
  if (audio.audio_buffers == NULL) {
    goto fail;
  }
  for (i=0; i<audio.frags; i++) {
#ifndef AMIGA_OS4
    audio.audio_buffers[i].buffer = AllocVec(fragsize, MEMF_PUBLIC | MEMF_CLEAR);
#else
    audio.audio_buffers[i].buffer = IExec->AllocVec(fragsize, MEMF_PUBLIC | MEMF_CLEAR);
#endif
    if (audio.audio_buffers[i].buffer == NULL) {
      goto fail;
    }
    audio.audio_buffers[i].size = 0;
  }

DEBUG("get self\n");

  /* get task pointer */
#ifndef AMIGA_OS4
  audio.main_task = FindTask(NULL);
#else
  audio.main_task = IExec->FindTask(NULL);
#endif

DEBUG("remove signals\n");

  /* remove signals */
#ifndef AMIGA_OS4
  SetSignal(0, SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);
#else
  IExec->SetSignal(0, SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);
#endif

DEBUG("allocate semaphore\n");

  /* allocate semaphore */
#ifndef AMIGA_OS4
  audio.semaphore = (struct SignalSemaphore *)AllocVec(sizeof(struct SignalSemaphore), MEMF_PUBLIC | MEMF_CLEAR);
#else
  audio.semaphore = (struct SignalSemaphore *)IExec->AllocVec(sizeof(struct SignalSemaphore), MEMF_PUBLIC | MEMF_CLEAR);
#endif
  if (audio.semaphore == NULL) {
    goto fail;
  }
  memset(audio.semaphore, 0, sizeof(struct SignalSemaphore));
#ifndef AMIGA_OS4
  InitSemaphore(audio.semaphore);
#else
  IExec->InitSemaphore(audio.semaphore);
#endif

DEBUG("create task\n");

  make_task_name();
#ifndef AMIGA_OS4
  audio.audio_process = CreateNewProc(ti);
#else
  audio.audio_process = IDOS->CreateNewProc(ti);
#endif
  audio.audio_task = (struct Task *)audio.audio_process;
  if (audio.audio_task == NULL) {
    goto fail;
  }

DEBUG("success, waiting...\n");

#ifndef AMIGA_OS4
  signals = Wait(SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);
#else
  signals = IExec->Wait(SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);
#endif
  if (signals & SIGBREAKF_CTRL_E) {
    DEBUG("failed to start audio task...\n");

    /* wait for task to exit */
#ifndef AMIGA_OS4
    while (FindTask(ahi_task_name) != NULL) {
#else
    while (IExec->FindTask(ahi_task_name) != NULL) {
#endif
    }

    audio.audio_task = NULL;
    goto fail;
  }

DEBUG("done! task is running!\n");

  return 0;

fail:

DEBUG("ahi open failed\n");

  ahi_close(); /* free any allocated resources */

  return -1;
}

static s32 ahi_in_buffer(void)
{
  struct timeval dt, ct, st;
  s32 write_position, read_position, in_buffer;

  if (audio.audio_task == NULL) {
    return 0; /* not open */
  }

#ifndef AMIGA_OS4
  ObtainSemaphore(audio.semaphore);
#else
  IExec->ObtainSemaphore(audio.semaphore);
#endif

#ifndef AMIGA_OS4
  timer_gettime(&ct);
#else
  timer_gettime(audio.timer, &ct);
#endif

  write_position = audio.write_position;

  if (audio.read_position >= 0) {
    s64 diff;
    st = audio.tv;
    dt = ct;

#ifndef AMIGA_OS4
    timer_subtime(&dt, &st);
#else
    timer_subtime(audio.timer, &dt, &st);
#endif

    diff = (dt.tv_secs * 1000000) + dt.tv_micro; /* us */
    diff = (diff * audio.frequency) / 1000000;
    diff <<= audio.samples_to_bytes;

    if (diff < 0) {
      diff = 0;
    } else if (diff > audio.fragsize) {
      diff = audio.fragsize;
    }

    read_position = audio.read_position + diff;
  } else {
    read_position = 0; /* we haven't started playing anything yet */
  }

#ifndef AMIGA_OS4
  ReleaseSemaphore(audio.semaphore);
#else
  IExec->ReleaseSemaphore(audio.semaphore);
#endif

  /* in buffer */
  in_buffer = (write_position - read_position);
  if (in_buffer < 0) {
    in_buffer += (audio.frags * audio.fragsize);
  }

  return in_buffer;
}

void ahi_play_samples(void *data, s32 size, s64 time, s32 wait)
{
  s32 used, copy, max;

  if (audio.audio_task == NULL) {
    return; /* not open */
  }

  if (wait == DOWAIT) { /* Here we should wait until the buffer is half empty */
    s32 in_buffer, total_buffer;
    s64 diff;

    in_buffer = ahi_in_buffer();
    total_buffer = (audio.fragsize * audio.frags);
    diff = in_buffer - (total_buffer >> 1);
    if (diff > 0) {
      diff >>= audio.samples_to_bytes;
      diff = (diff * 1000000) / audio.frequency; /* us to wait */
#ifndef AMIGA_OS4
      timer_usleep(diff);
#else
      timer_usleep(audio.timer, diff);
#endif
    }
  }

  if (time != NOTIME) {
    audio.current_time = time;
  }

  time = audio.current_time;

  size <<= audio.samples_to_bytes;

  audio.play = 1; /* start playing */

  while (size > 0) {

PRINTF("audio_play: %d\n", size);
    for (;;) {
#ifndef AMIGA_OS4
      ObtainSemaphore(audio.semaphore);
#else
      IExec->ObtainSemaphore(audio.semaphore);
#endif
      used = audio.audio_buffers[audio.write_buffer].used;
#ifndef AMIGA_OS4
      ReleaseSemaphore(audio.semaphore);
#else
      IExec->ReleaseSemaphore(audio.semaphore);
#endif

      if (used) {
        PRINTF("used: %d\n", audio.write_buffer);
#ifndef AMIGA_OS4
        Wait(SIGBREAKF_CTRL_D);
#else
        IExec->Wait(SIGBREAKF_CTRL_D);
#endif
      } else {
        break;
      }
    }

PRINTF("buffer free: %d\n", audio.write_buffer);

#ifndef AMIGA_OS4
    ObtainSemaphore(audio.semaphore);
#else
    IExec->ObtainSemaphore(audio.semaphore);
#endif

    if (audio.audio_buffers[audio.write_buffer].size == audio.fragsize) {
      audio.audio_buffers[audio.write_buffer].size = 0;
    }

    if (audio.audio_buffers[audio.write_buffer].size == 0) {
      audio.audio_buffers[audio.write_buffer].time = time;
    }

    max = audio.fragsize - audio.audio_buffers[audio.write_buffer].size;

    copy = size;
    if (copy > max) {
      copy = max;
    }

    memcpy(audio.audio_buffers[audio.write_buffer].buffer + audio.audio_buffers[audio.write_buffer].size, data, copy);
    audio.audio_buffers[audio.write_buffer].size += copy;
    data += copy;
    size -= copy;

    time += ((copy >> audio.samples_to_bytes) * TIMEBASE) / audio.frequency;

    audio.write_position = (audio.write_buffer * audio.fragsize) + audio.audio_buffers[audio.write_buffer].size;

    if (audio.audio_buffers[audio.write_buffer].size == audio.fragsize) {
      audio.audio_buffers[audio.write_buffer].used = 1;
      audio.write_buffer++;
      if (audio.write_buffer >= audio.frags) {
        audio.write_buffer = 0;
      }
    }

PRINTF("release\n");
#ifndef AMIGA_OS4
    ReleaseSemaphore(audio.semaphore);
#else
    IExec->ReleaseSemaphore(audio.semaphore);
#endif

PRINTF("signal\n");
#ifndef AMIGA_OS4
    Signal(audio.audio_task, SIGBREAKF_CTRL_D);
#else
    IExec->Signal(audio.audio_task, SIGBREAKF_CTRL_D); /* FIXME: only signal when successfull write */
#endif
  }

  audio.current_time = time;
}

s32 ahi_samples_to_bytes(s32 samples)
{
  return samples <<= audio.samples_to_bytes;
}

s32 ahi_bytes_to_samples(s32 bytes)
{
  return bytes >>= audio.samples_to_bytes;
}

s32 ahi_samples_buffered(void)
{
  return ahi_in_buffer() >> audio.samples_to_bytes;
}

s32 ahi_samples_free(void)
{
  s32 in_buffer, total_buffer;

  in_buffer = ahi_in_buffer();
  total_buffer = (audio.fragsize * audio.frags);
  total_buffer -= in_buffer;

  return total_buffer >> audio.samples_to_bytes;
}

void ahi_pause(void)
{
  if (audio.audio_task == NULL) {
    return; /* not open */
  }

  audio.play = 0; /* stop playing */
}

void ahi_close(void)
{
  s32 i;
  u32 signals;

  if (audio.audio_task != NULL) {
DEBUG("stop server\n");
    /* stop server */
#ifndef AMIGA_OS4
    Signal(audio.audio_task, SIGBREAKF_CTRL_E);
#else
    IExec->Signal(audio.audio_task, SIGBREAKF_CTRL_E);
#endif

DEBUG("wait for signal\n");
    /* wait for signal */
    do {
#ifndef AMIGA_OS4
      signals = Wait(SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);
#else
      signals = IExec->Wait(SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);
#endif
    } while (!(signals & SIGBREAKF_CTRL_E));

DEBUG("wait for exit\n");
    /* wait for task to exit */
#ifndef AMIGA_OS4
    while (FindTask(ahi_task_name) != NULL) {
#else
    while (IExec->FindTask(ahi_task_name) != NULL) {
#endif
    }

    audio.audio_task = NULL;
  }

DEBUG("free semaphore\n");
  /* free semaphore */
  if (audio.semaphore != NULL) {
#ifndef AMIGA_OS4
    FreeVec(audio.semaphore);
#else
    IExec->FreeVec(audio.semaphore);
#endif
  }

DEBUG("free buffers\n");
  if (audio.audio_buffers != NULL) {
    for (i=0; i<audio.frags; i++) {
#ifndef AMIGA_OS4
      FreeVec(audio.audio_buffers[i].buffer);
#else
      IExec->FreeVec(audio.audio_buffers[i].buffer);
#endif
    }
#ifndef AMIGA_OS4
    FreeVec(audio.audio_buffers);
#else
    IExec->FreeVec(audio.audio_buffers);
#endif
  }

DEBUG("free timer\n");
#ifndef AMIGA_OS4
  timer_exit();
#else
  if (audio.timer != NULL) {
    timer_exit(audio.timer);
  }
#endif

  /* reset structure */
  memset(&audio, 0, sizeof(audio_t));
}
#endif

