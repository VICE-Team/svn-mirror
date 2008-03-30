/*
 * serial-iec-device.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  David Hansel <hansel@reactive-systems.com>
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

#include "cmdline.h"
#include "iecbus.h"
#include "resources.h"
#include "serial-iec-device.h"
#include "serial.h"
#include "log.h"
#include "serial-iec-bus.h"

void serial_iec_device_enable(int devnr);
void serial_iec_device_disable(int devnr);
static void serial_iec_device_exec_main(int devnr, CLOCK clk_value);

/* ------------------------------------------------------------------------- */


static unsigned int iec_device_enabled[IECBUS_NUM];

static int set_iec_device_enable(resource_value_t v, void *param)
{
    int enable;
    unsigned int unit;

    enable = (int)v;
    unit = (unsigned int)param;

    if ((unit < 4 || unit > 5) && (unit < 8 || unit > 11))
        return -1;

    iec_device_enabled[unit] = enable;
    if( enable )
      serial_iec_device_enable(unit);
    else
      serial_iec_device_disable(unit);

    iecbus_status_set(IECBUS_STATUS_IECDEVICE, unit, enable);

    return 0;
}

static const resource_t resources[] = {
    { "IECDevice4", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[4],
      set_iec_device_enable, (void *)4 },
    { "IECDevice5", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[5],
      set_iec_device_enable, (void *)5 },
    { "IECDevice8", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[8],
      set_iec_device_enable, (void *)8 },
    { "IECDevice9", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[9],
      set_iec_device_enable, (void *)9 },
    { "IECDevice10", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[10],
      set_iec_device_enable, (void *)10 },
    { "IECDevice11", RES_INTEGER, (resource_value_t)0,
      (void *)&iec_device_enabled[11],
      set_iec_device_enable, (void *)11 },
    { NULL }
};

int serial_iec_device_resources_init(void)
{
    return resources_register(resources);
}

static const cmdline_option_t cmdline_options[] = {
    { "-iecdevice4", SET_RESOURCE, 0, NULL, NULL, "IECDevice4",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #4" },
    { "+iecdevice4", SET_RESOURCE, 0, NULL, NULL, "IECDevice4",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #4" },
    { "-iecdevice5", SET_RESOURCE, 0, NULL, NULL, "IECDevice5",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #5" },
    { "+iecdevice5", SET_RESOURCE, 0, NULL, NULL, "IECDevice5",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #5" },
    { "-iecdevice8", SET_RESOURCE, 0, NULL, NULL, "IECDevice8",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #8" },
    { "+iecdevice8", SET_RESOURCE, 0, NULL, NULL, "IECDevice8",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #8" },
    { "-iecdevice9", SET_RESOURCE, 0, NULL, NULL, "IECDevice9",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #9" },
    { "+iecdevice9", SET_RESOURCE, 0, NULL, NULL, "IECDevice9",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #9" },
    { "-iecdevice10", SET_RESOURCE, 0, NULL, NULL, "IECDevice10",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #10" },
    { "+iecdevice10", SET_RESOURCE, 0, NULL, NULL, "IECDevice10",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #10" },
    { "-iecdevice11", SET_RESOURCE, 0, NULL, NULL, "IECDevice11",
      (resource_value_t)1,
      NULL, "Enable IEC device emulation for device #11" },
    { "+iecdevice11", SET_RESOURCE, 0, NULL, NULL, "IECDevice11",
      (resource_value_t)0,
      NULL, "Disable IEC device emulation for device #11" },
    { NULL }
};


int serial_iec_device_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/*------------------------------------------------------------------------*/

/* Implement IEC devices here.  */

/*------------------------------------------------------------------------*/

#define IEC_DEVICE_DEBUG 0

/* Logging goes here.  */
#if IEC_DEVICE_DEBUG > 0
static log_t serial_iec_device_log = LOG_ERR;
#endif

typedef struct serial_iec_device_state_s 
{
  BYTE  enabled;
  BYTE  byte, state, flags, primary, secondary, secondary_prev;
  BYTE  st[16];
  CLOCK clock;
} serial_iec_device_state_t;

static serial_iec_device_state_t serial_iec_device_state[IECBUS_NUM];



void serial_iec_device_init(void)
{
  int i;
  static int init_done = 0;

  /* only initialize once, no matter how many times we are called */
  if( !init_done )
    {
#if IEC_DEVICE_DEBUG > 0
      serial_iec_device_log = log_open("Serial-IEC-Device");
      log_message(serial_iec_device_log, "serial_iec_device_init()");
#endif
      for(i=0; i<IECBUS_NUM; i++) 
        {
          serial_iec_device_state[i].enabled = 0;
          iecbus_device_write(i, IECBUS_DEVICE_WRITE_CLK | IECBUS_DEVICE_WRITE_DATA);
        }

      init_done = 1;
    }
}


void serial_iec_device_reset(void)
{
  int i;

#if IEC_DEVICE_DEBUG > 0
  log_message(serial_iec_device_log, "serial_iec_device_reset()");
#endif

  for(i=0; i<IECBUS_NUM; i++) 
    if( serial_iec_device_state[i].enabled )
      {
        iecbus_device_write(i, IECBUS_DEVICE_WRITE_CLK | IECBUS_DEVICE_WRITE_DATA);
        serial_iec_device_state[i].flags = 0;
        memset(&serial_iec_device_state[i].st, 0, 15);
      }
}


void serial_iec_device_enable(int devnr)
{
  /* make sure we are initialized (command-line options are
     processed before the official call to serial_iec_device_init) */
  serial_iec_device_init();

  if( !serial_iec_device_state[devnr].enabled )
    {
#if IEC_DEVICE_DEBUG > 0
      log_message(serial_iec_device_log, "serial_iec_device_enable(%i)", devnr);
#endif
      serial_iec_device_state[devnr].enabled  = 1;
      serial_iec_device_state[devnr].flags    = 0;
      memset(&serial_iec_device_state[devnr].st, 0, 15);
    }
}


void serial_iec_device_disable(int devnr)
{
  /* make sure we are initialized (command-line options are
     processed before the official call to serial_iec_device_init) */
  serial_iec_device_init();

  if( serial_iec_device_state[devnr].enabled )
    {
#if IEC_DEVICE_DEBUG > 0
      log_message(serial_iec_device_log, "serial_iec_device_disable(%i)", devnr);
#endif
      iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK | IECBUS_DEVICE_WRITE_DATA);
      serial_iec_device_state[devnr].enabled = 0;
    }
}


void serial_iec_device_exec(CLOCK clk_value)
{
  int i;

  for(i=0; i<IECBUS_NUM; i++)
    if( serial_iec_device_state[i].enabled )
      serial_iec_device_exec_main(i, clk_value);
}

/* ------------------------------------------------------------------------- */

static BYTE serial_iec_device_st;

inline static void set_st(BYTE b)
{
  serial_iec_device_st = b;
}

inline static BYTE get_st(void)
{
  return serial_iec_device_st;
}

/* ------------------------------------------------------------------------- */

enum
  { 
    P_PRE0=0, P_PRE1, P_PRE2, 
    P_READY,
    P_EOI,  P_EOIw,
    P_BIT0, P_BIT0w,
    P_BIT1, P_BIT1w,
    P_BIT2, P_BIT2w,
    P_BIT3, P_BIT3w,
    P_BIT4, P_BIT4w,
    P_BIT5, P_BIT5w,
    P_BIT6, P_BIT6w,
    P_BIT7, P_BIT7w,
    P_DONE0, P_DONE1,
    P_FRAMEERR0, P_FRAMEERR1,

    P_TALKING   = 0x20,
    P_LISTENING = 0x40,
    P_ATN       = 0x80
  };


static void serial_iec_device_exec_main(int devnr, CLOCK clk_value)
{
  int bus;
  serial_iec_device_state_t *iec = &(serial_iec_device_state[devnr]);

  /* read bus */
  bus = iecbus_device_read();

#if IEC_DEVICE_DEBUG > 3
  log_message(serial_iec_device_log, "serial_iec_device_exec_main(%i, %i) S=%i, ATN=%i CLK=%i DTA=%i", 
              devnr, clk_value, iec->state,
              (bus & IECBUS_DEVICE_READ_ATN)  ? 1 : 0,
              (bus & IECBUS_DEVICE_READ_CLK)  ? 1 : 0,
              (bus & IECBUS_DEVICE_READ_DATA) ? 1 : 0);
  /*{
    int i;
    for(i=8; i<=11; i++)
      printf("%i ", iecbus.drv_bus[i]);
    printf("\n");
    }*/
#endif

  if( !(iec->flags & P_ATN) && !(bus & IECBUS_DEVICE_READ_ATN) )
    {
      /* falling flank on ATN (bus master addressing all devices) */
      iec->state          = P_PRE0;
      iec->flags         |= P_ATN;
      iec->primary        = 0;
      iec->secondary_prev = iec->secondary;
      iec->secondary      = 0;
      iec->clock          = clk_value;

      /* set DATA=0 ("I am here").  If nobody on the bus does this within 1ms,
         busmaster will assume that "Device not present" */
      iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK);
    }
  else if( (iec->flags & P_ATN) && (bus & IECBUS_DEVICE_READ_ATN) )
    {
      /* rising flank on ATN (bus master finished addressing all devices) */
      iec->flags &= ~P_ATN;

      if( (iec->primary == 0x20+devnr) || (iec->primary == 0x40+devnr) )
        {
          if( (iec->secondary & 0xf0) == 0x60 )
            serial_iec_bus_listentalk(devnr, iec->secondary, set_st);
          else if( (iec->secondary & 0xf0) == 0xe0 )
            {
              set_st(0);
              serial_iec_bus_close(devnr, iec->secondary, set_st);
              iec->st[iec->secondary & 0x0f] = get_st();
            }
          else if( (iec->secondary & 0xf0) == 0xf0 )
            {
              /* iec_bus_open() will not actually open the file (since we don't
                 have a filename yet) but just set things up so that the 
                 characters passed to iec_bus_write() before the next call to
                 iec_bus_unlisten()will be interpreted as the filename.  
                 The file will actually be opened during the next call to 
                 iec_bus_unlisten() */
              set_st(0);
              serial_iec_bus_open(devnr, iec->secondary, set_st);
              iec->st[iec->secondary & 0x0f] = get_st();
            }

          if( iec->primary == 0x20+devnr )
            {
              /* we were told to listen */
              iec->flags &= ~P_TALKING;

              /* st!=0 means that the previous OPEN command failed, i.e. we
                 could not open a file for writing.  In that case, ignore
                 the "LISTEN" request which will signal the error to the sender */

              if( iec->st[iec->secondary & 0x0f] == 0 )
                {
                  iec->flags |=  P_LISTENING;
                  iec->state  =  P_PRE1;
#if IEC_DEVICE_DEBUG > 2
                  log_message(serial_iec_device_log, "device %i start listening", devnr);
#endif
                }
              
              /* set DATA=0 ("I am here") */
              iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK);
            }
          else if( iec->primary == 0x40+devnr )
            {
              /* we were told to talk */
              iec->flags &= ~P_LISTENING;
              iec->flags |=  P_TALKING;
              iec->state  =  P_PRE0;
#if IEC_DEVICE_DEBUG > 2
              log_message(serial_iec_device_log, "device %i start talking", devnr);
#endif
            }
        }
      else if( (iec->primary == 0x3f) && (iec->flags & P_LISTENING) )
        {
          /* all devices were told to stop listening */
          iec->flags &= ~P_LISTENING;
#if IEC_DEVICE_DEBUG > 2
          log_message(serial_iec_device_log, "device %i stop listening", devnr);
#endif

          /* if this is an UNLISTEN that followed an OPEN (0x2_ 0xf_), then
             iec_bus_unlisten will try to open the file with the filename that
             was received in between the OPEN and now.  If the file cannot be
             opened, it will set st != 0. */
          set_st(iec->st[iec->secondary_prev & 0x0f]);
          serial_iec_bus_unlisten(devnr, iec->secondary_prev, set_st);
          iec->st[iec->secondary_prev & 0x0f] = get_st();
        }
      else if( iec->primary == 0x5f && (iec->flags & P_TALKING) )
        {
          /* all devices were told to stop talking */
          serial_iec_bus_untalk(devnr, iec->secondary_prev, set_st);
          iec->flags &= ~P_TALKING;
#if IEC_DEVICE_DEBUG > 2
          log_message(serial_iec_device_log, "device %i stop talking", devnr);
#endif
        }

      if( !(iec->flags & (P_LISTENING | P_TALKING)) )
        {
          /* we're neither listening nor talking => make sure we're not holding DATA 
             or CLOCK line to 0 */
          iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK | IECBUS_DEVICE_WRITE_DATA);
        }
    }

  if( iec->flags & (P_ATN | P_LISTENING) )
    {
      /* we are either under ATN or in "listening" mode */

      switch( iec->state )
        {
        case P_PRE0: 
          {
            /* ignore anything that happens during first 100us after falling flank on ATN 
               (other devices may have been sending and need some time to set CLK=1) */
            if( clk_value >= (iec->clock+100) )
              iec->state = P_PRE1;
              
            break;
          }

        case P_PRE1: 
          {
            /* make sure CLK=0 so we actually detect a rising flank in state P_PRE2 */
            if( !(bus & IECBUS_DEVICE_READ_CLK) ) 
              iec->state = P_PRE2;

            break;
          }

        case P_PRE2: 
          {
            /* wait for rising flank on CLK ("ready-to-send") */
            if( bus & IECBUS_DEVICE_READ_CLK )
              {
                /* react by setting DATA=1 ("ready-for-data") */
                iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK | IECBUS_DEVICE_WRITE_DATA);
                iec->clock = clk_value;
                iec->state = P_READY;
              }
              
            break;
          }

        case P_READY:
          {
            if( !(bus & IECBUS_DEVICE_READ_CLK) ) 
              {
                /* sender set CLK=0, is about to send first bit */
                iec->state = P_BIT0;
              }
            else if( !(iec->flags & P_ATN) && (clk_value >= (iec->clock+200)) )
              {
                /* sender did not set CLK=0 within 200us after we set DATA=1
                   => it is signaling EOI (not so if we are under ATN)
                   acknowledge we received it by setting DATA=0 for 80us */
#if IEC_DEVICE_DEBUG > 2
                log_message(serial_iec_device_log, "device %i got EOI on channel %i", 
                            devnr, iec->secondary & 0x0f);
#endif
                iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK);
                iec->state = P_EOI;
                iec->clock = clk_value;
              }

            break;
          }

        case P_EOI:
          {
            if( clk_value >= (iec->clock+80) )
              {
                /* the 80us have passed. Set DATA back to 1 and wait 
                   for sender to set CLK=0 */
                iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK | IECBUS_DEVICE_WRITE_DATA);
                iec->state = P_EOIw;
              }

            break;
          }

        case P_EOIw:
          {
            if( !(bus & IECBUS_DEVICE_READ_CLK) )
              {
                /* sender set CLK=0, is about to send first bit */
                iec->state = P_BIT0;
              }

            break;
          }

        case P_BIT0:
        case P_BIT1:
        case P_BIT2:
        case P_BIT3:
        case P_BIT4:
        case P_BIT5:
        case P_BIT6:
        case P_BIT7:
          {
            if( bus & IECBUS_DEVICE_READ_CLK )
              {
                /* sender set CLK=1, signaling that the DATA line represents a valid bit */
                int bit = 1<<((iec->state-P_BIT0)/2);
                iec->byte = (iec->byte & ~bit) | ((bus & IECBUS_DEVICE_READ_DATA) ? bit : 0);

                /* go to associated P_BIT(n)w state, waiting for sender to set CLK=0 */
                iec->state++;
              }

            break;
          }
          
        case P_BIT0w:
        case P_BIT1w:
        case P_BIT2w:
        case P_BIT3w:
        case P_BIT4w:
        case P_BIT5w:
        case P_BIT6w:
          {
            if( !(bus & IECBUS_DEVICE_READ_CLK) ) 
              {
                /* sender set CLK=0. go to P_BIT(n+1) state to receive next bit */
                iec->state++;
              }

            break;
          }

        case P_BIT7w:
          {
            if( !(bus & IECBUS_DEVICE_READ_CLK) ) 
              {
                /* sender set CLK=0 and this was the last bit */
#if IEC_DEVICE_DEBUG > 1
                log_message(serial_iec_device_log, "device %i received : 0x%02x (%c)", 
                            devnr, iec->byte, isprint(iec->byte) ? iec->byte : '.');
#endif
                if( iec->flags & P_ATN )
                  {
                    /* We are currently receiving under ATN.  Store first two bytes received
                       (contain primary and secondary address) */
                    if( iec->primary==0 ) 
                      iec->primary = iec->byte;
                    else if( iec->secondary==0 )
                      iec->secondary = iec->byte;

                    if( !(iec->primary & 0x10) && (iec->primary & 0x0f)!=devnr )
                      {
                        /* This is NOT a UNLISTEN (0x3f) or UNTALK (0x5f) command
                           and the primary address is not ours => Don't acknowledge the frame and stop listening.
                           If all devices on the bus do this, the busmaster knows that "Device not present" */
                        iec->state = P_DONE0;
                      }
                    else
                      {
                        /* Acknowledge frame by setting DATA=0 */
                        iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK);
                        
                        /* repeat from P_PRE2 (we know that CLK=0 so no need to go to P_PRE1) */
                        iec->state = P_PRE2;
                      }
                  }
                else if( iec->flags & P_LISTENING )
                  {
                    /* We are currently listening for data
                       => pass received byte on to the upper level */
#if IEC_DEVICE_DEBUG > 0
                    log_message(serial_iec_device_log, "device %i received 0x%02x (%c) on channel %i", 
                                devnr, iec->byte, isprint(iec->byte) ? iec->byte : '.', iec->secondary & 0x0f);
#endif
                    set_st(iec->st[iec->secondary & 0x0f]);
                    serial_iec_bus_write(devnr, iec->secondary, iec->byte, set_st);
                    iec->st[iec->secondary & 0x0f] = get_st();

                    if( iec->st[iec->secondary & 0x0f] != 0 )
                      {
                        /* there was an error during iec_bus_write => stop listening.  This will signal
                           an error condition to the sender */
                        iec->state = P_DONE0;
                      }
                    else
                      {
                        /* Acknowledge frame by setting DATA=0 */
                        iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK);
                        
                        /* repeat from P_PRE2 (we know that CLK=0 so no need to go to P_PRE1) */
                        iec->state = P_PRE2;
                      }
                  }
              }

            break;
          }

        case P_DONE0:
          {
            /* we're just waiting for the busmaster to set ATN back to 1 */
            break;
          }
        }
    }
  else if( iec->flags & P_TALKING )
    {
      /* we are in "talking" mode */

      switch( iec->state )
        {
        case P_PRE0: 
          {
            if( bus & IECBUS_DEVICE_READ_CLK )
              {
                /* busmaster set CLK=1 (and before that should have set DATA=0)
                   we are getting ready for role reversal.  Set DATA=1 */
                iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_DATA);
                iec->state = P_PRE1;
                iec->clock = clk_value;
              }

            break;
          }

        case P_PRE1:
          {
            if( clk_value >= (iec->clock+130) )
              {
                /* signal "ready-to-send" (CLK=1) */
                iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK | IECBUS_DEVICE_WRITE_DATA);
                iec->state = P_READY;
                break;
              }
          }

        case P_READY:
          {
            if( bus & IECBUS_DEVICE_READ_DATA )
              {
                /* receiver signaled "ready-for-data" (DATA=1) */

                set_st(iec->st[iec->secondary & 0x0f]);
                iec->byte = serial_iec_bus_read(devnr, iec->secondary, set_st);
                iec->st[iec->secondary & 0x0f] = get_st();
                
                if( iec->st[iec->secondary & 0x0f] == 0 )
                  {
                    /* at least two bytes left to send.  Go on to send first bit. */
                    iec->state = P_BIT0;
                    
                    /* no need to wait the 60us before sending the first bit */
                    iec->clock = clk_value-60;
                  }
                else if( iec->st[iec->secondary & 0x0f] == 0x40 )
                  {
                    /* only this byte left to send => signal EOI by keeping CLK=1 */
#if IEC_DEVICE_DEBUG > 2
                    log_message(serial_iec_device_log, "device %i signaling EOI on channel %i", 
                                devnr, iec->secondary & 0x0f);
#endif
                    iec->state = P_EOI;
                    iec->clock = clk_value;
                  }
                else
                  {
                    /* There was some kind of error, we have nothing to send.
                       Just stop talking and wait for ATN.  
                       (This will produce a "File not found" when loading) */
                    iec->flags &= ~P_TALKING;
                  }
              }

            break;
          }

        case P_EOI:
          {
            if( !(bus & IECBUS_DEVICE_READ_DATA) )
              {
                /* receiver set DATA=0, first part of acknowledging the EOI */
                iec->state = P_EOIw;
              }

            break;
          }

        case P_EOIw:
          {
            if( bus & IECBUS_DEVICE_READ_DATA )
              {
                /* receiver set DATA=1, final part of acknowledging the EOI.
                   Go on to send first bit */
                iec->state = P_BIT0;

                /* no need to wait the 60us before sending the first bit */
                iec->clock = clk_value-60;
              }

            break;
          }

        case P_BIT0:
        case P_BIT1:
        case P_BIT2:
        case P_BIT3:
        case P_BIT4:
        case P_BIT5:
        case P_BIT6:
        case P_BIT7:
          {
            if( clk_value >= (iec->clock+90) )
              {
                /* 90us have passed since we set CLK=1 to signal "data valid" for the previous bit.
                   Pull CLK=0 and put next bit out on DATA. */
                int bit = 1<<((iec->state-P_BIT0)/2);
                iecbus_device_write(devnr, (iec->byte & bit) ? IECBUS_DEVICE_WRITE_DATA : 0);

                /* go to associated P_BIT(n)w state */
                iec->clock = clk_value;
                iec->state++;
              }

            break;
          }

        case P_BIT0w:
        case P_BIT1w:
        case P_BIT2w:
        case P_BIT3w:
        case P_BIT4w:
        case P_BIT5w:
        case P_BIT6w:
        case P_BIT7w:
          {
            if( clk_value >= (iec->clock+90) )
              {
                /* 90us have passed since we pulled CLK=0 and put the current bit on DATA.
                   set CLK=1, keeping data as it is (this signals "data valid" to the receiver) */
                if( bus & IECBUS_DEVICE_READ_DATA )
                  iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK | IECBUS_DEVICE_WRITE_DATA);
                else
                  iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK);

                /* go to associated P_BIT(n+1) state to send the next bit.  If this was 
                   the final bit then next state is P_DONE0 */
                iec->clock = clk_value;
                iec->state++;
              }

            break;
          }

        case P_DONE0:
          {
            if( clk_value >= (iec->clock+90) )
              {
                /* 90us have passed since we set CLK=1 to signal "data valid" for the final bit.
                   Pull CLK=0 and set DATA=1.  This prepares for the receiver acknowledgement. */
                iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_DATA);
                iec->clock = clk_value;
                iec->state = P_DONE1;
              }

            break;
          }

        case P_DONE1:
          {
            if( !(bus & IECBUS_DEVICE_READ_DATA) )
              {
                /* Receiver set DATA=0, acknowledging the frame */
#if IEC_DEVICE_DEBUG > 0
                log_message(serial_iec_device_log, "device %i sent 0x%02x (%c) on channel %i", 
                            devnr, iec->byte, isprint(iec->byte) ? iec->byte : '.',  
                            iec->secondary & 0x0f);
#endif
                if( iec->st[iec->secondary & 0x0f] == 0x40 )
                  {
                    /* This was the last byte => stop talking.  This leaves us waiting for ATN. */
                    iec->flags &= ~P_TALKING;
                    iec->st[iec->secondary & 0x0f] = 0;
                    
                    /* Release the CLOCK line to 1 */
                    iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK | IECBUS_DEVICE_WRITE_DATA);
                  } 
                else
                  {
                    /* There is at least one more byte to send
                       Start over from P_PRE1 */
                    iec->clock = clk_value;
                    iec->state = P_PRE1;
                  }
              }
            else if( clk_value >= (iec->clock+1000) )
              {
                /* We didn't receive an acknowledgement within 1ms.
                   Set CLOCK=0 and after 100us back to CLOCK=1 */
#if IEC_DEVICE_DEBUG > 2
                log_message(serial_iec_device_log, "device %i got NACK on channel %i", 
                            devnr, iec->secondary & 0x0f);
#endif
                iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_CLK | IECBUS_DEVICE_WRITE_DATA);
                iec->clock = clk_value;
                iec->state = P_FRAMEERR0;
              }

            break;
          }

        case P_FRAMEERR0:
          {
            if( clk_value >= (iec->clock+100) )
              {
                /* finished 1-0-1 sequence of CLOCK signal to acknowledge the
                   frame-error.  Now wait for sender to set DATA=0 so we can continue. */
                iecbus_device_write(devnr, IECBUS_DEVICE_WRITE_DATA);
                iec->state = P_FRAMEERR1;
              }

            break;
          }

        case P_FRAMEERR1:
          {
            if( !(bus & IECBUS_DEVICE_READ_DATA) )
              {
                /* sender set DATA=0, we can retry to send the byte */
                iec->clock = clk_value;
                iec->state = P_PRE1;
              }

            break;
          }
        }
    }
}
