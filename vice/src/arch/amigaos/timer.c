/*
 * timer.c
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

#include <proto/exec.h>
#include <proto/timer.h>

#include "timer.h"

struct timer_s {
  struct MsgPort *TimerMP;
  struct timerequest *TimerIO;
  struct Device *TimerBase;
  struct TimerIFace *ITimer;
};

timer_t *timer_init(void)
{
  timer_t *timer = IExec->AllocVec(sizeof(timer_t), MEMF_PUBLIC | MEMF_CLEAR);
  if (timer == NULL) {
    return NULL;
  }

  if ((timer->TimerMP = IExec->AllocSysObject(ASOT_PORT, NULL))) {
    if ((timer->TimerIO = IExec->AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_Size, sizeof(struct timerequest), ASOIOR_ReplyPort, timer->TimerMP, TAG_DONE))) {
      if (IExec->OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *)timer->TimerIO, 0) == 0) {
        timer->TimerBase = timer->TimerIO->tr_node.io_Device;
        timer->ITimer = (struct TimerIFace *)IExec->GetInterface((struct Library *)timer->TimerBase, "main", 1, NULL);
        if (timer->ITimer != NULL) {
          return timer;
        }
      }
    }
  }

  timer_exit(timer);

  return NULL;
}

void timer_exit(timer_t *timer)
{
  if (timer != NULL) {
    if (timer->ITimer != NULL) {
      IExec->DropInterface((struct Interface *)timer->ITimer);
    }
    if (timer->TimerBase != NULL) {
      IExec->CloseDevice((struct IORequest *)timer->TimerIO);
    }
    if (timer->TimerIO != NULL) {
      IExec->FreeSysObject(ASOT_IOREQUEST, timer->TimerIO);
    }
    if (timer->TimerMP != NULL) {
      IExec->FreeSysObject(ASOT_PORT, timer->TimerMP);
    }
    IExec->FreeVec(timer);
  }
}

void timer_gettime(timer_t *timer, struct timeval *tv)
{
  if (timer != NULL) {
    timer->ITimer->GetUpTime(tv);
  }
}

void timer_subtime(timer_t *timer, struct timeval *dt, struct timeval *st)
{
  if (timer != NULL) {
    timer->ITimer->SubTime(dt, st);
  }
}

void timer_usleep(timer_t *timer, int us)
{
  if (timer != NULL) {
    /* setup */
    timer->TimerIO->tr_node.io_Command = TR_ADDREQUEST;
    timer->TimerIO->tr_time.tv_secs = us / 1000000;
    timer->TimerIO->tr_time.tv_micro = us % 1000000;

    /* send request */
    IExec->SetSignal(0, (1L << timer->TimerMP->mp_SigBit));
    IExec->SendIO((struct IORequest *)timer->TimerIO);
    IExec->Wait((1L << timer->TimerMP->mp_SigBit));
    IExec->WaitIO((struct IORequest *)timer->TimerIO);
  }
}
