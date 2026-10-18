/** \file   tapecart.c
 * \brief   tapecart emulation
 *
 * \author  Ingo Korb <ingo@akana.de>
 *
 * based on sense-dongle.c by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 */

/*
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

/*
 * Note: This is a reimplementation of the original tapecart firmware.
 *
 * If you just want to understand how a tapecart works, refer to
 * the original programmer's reference:
 *   https://github.com/ikorb/tapecart/blob/master/doc/ProgRef.md
 *
 *
 * Warning: Messy jungle of event-triggered callback functions ahead.
 *
 */

#include "vice.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "alarm.h"
#include "archdep.h"
#include "cmdline.h"
#include "crc32.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "snapshot.h"
#include "tapeport.h"

#include "tapecart.h"


#define TCRT_MAGIC_LEN  13


static const char idstring[] = "TAPECART V1.0 W25QFLASH";


static int tapecart_enabled       = 0;
static int tapecart_update_tcrt   = 0;
static int tapecart_optimize_tcrt = 0;
static int tapecart_loglevel      = 0;

/* ------------------------------------------------------------------------- */

#undef max
#undef min
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

/* tapecart major modes */
typedef enum { /* visible in snapshots */
    MODE_UNINITIALIZED = 0,
    MODE_STREAM,
    MODE_FASTLOAD,
    MODE_COMMAND,
    MODE_REINIT,
    MODE__SENTINEL
} tapecart_mode_t;

/* things to wait for */
typedef enum { /* visible in snapshots */
    WH_INVALID = 0,
    WH_FASTTX_NIBBLE_ADVANCE,
    WH_RX1BIT,
    WH_TX1BIT,
    WH_CMDMODE_RX_COMMAND,
    WH_CMDMODE_SEND_PULSES,
    WH__SENTINEL
} wait_handler_t;

/* next action after completing a transfer */
typedef enum { /* visible in snapshots */
    TC_INVALID = 0,
    TC_FASTLOAD_POSTDELAY,
    TC_CMDMODE_RECEIVE_COMMAND,
    TC_CMDMODE_DISPATCH_COMMAND,
    TC_CMDMODE_WRITE_FLASH_PAGE,
    TC_CMD_READ_FLASH,
    TC_CMD_READ_FLASH_FAST,
    TC_CMD_WRITE_FLASH,
    TC_CMD_ERASE_FLASH_64K,
    TC_CMD_ERASE_FLASH_BLOCK,
    TC_CMD_CRC32_FLASH,
    TC_CMD_WRITE_LOADINFO,
    TC_CMD_DIR_SETPARAMS,
    TC_CMD_DIR_LOOKUP,
    TC__SENTINEL
} transfer_complete_t;

/* destinations for the logic alarm */
typedef enum { /* visible in snapshots */
    LOGIC_INVALID = 0,
    LOGIC_FASTTX_NIBBLE_ADVANCE,
    LOGIC_FASTTX_BYTE_ADVANCE,
    LOGIC_FASTLOAD_COMPLETE,
    LOGIC_RECEIVE_1BIT_CALLBACK,
    LOGIC_RECEIVE_1BIT_DELAYED,
    LOGIC_TRANSMIT_1BIT_CALLBACK,
    LOGIC_TRANSMIT_1BIT_DELAYED,
    LOGIC_CMDMODE_RECEIVE_COMMAND,
    LOGIC_CMDMODE_SEND_PULSES,
    LOGIC_COMMAND_MODE_INIT,
    LOGIC__SENTINEL
} logic_next_t;

/* a few forward declarations */
static void    tapecart_shutdown(void);
static void    tapecart_store_motor(int port, int state);
static void    tapecart_store_write(int port, int state);
static void    tapecart_store_sense(int port, int state);
static int     tapecart_enable(int port, int val);

static int     tapecart_write_snapshot(int port, struct snapshot_s *s, int write_image);
static int     tapecart_read_snapshot(int port, struct snapshot_s *s);

static void    tapecart_pulse_alarm_handler(CLOCK offset, void *unused);
static void    tapecart_logic_alarm_handler(CLOCK offset, void *unused);

static void    tapecart_set_mode(tapecart_mode_t mode);
static clock_t fasttx_byte_advance(void);
static clock_t cmdmode_receive_command(void);

static int     load_tcrt(const char *filename, tapecart_memory_t *tcmem);
static void    update_tcrt(void);

static clock_t dispatch_wait_handler(void);
static clock_t dispatch_transfer_complete(transfer_complete_t tc);
static clock_t dispatch_logic_next(void);

#define VICE_MACHINE_MASK (VICE_MACHINE_C64|VICE_MACHINE_C64SC|VICE_MACHINE_C128)

static tapeport_device_t tapecart_device = {
    "tapecart",                   /* device name */
    TAPEPORT_DEVICE_TYPE_STORAGE, /* device is a 'storage' type device */
    VICE_MACHINE_MASK,            /* device works on x64/x64sc/x128 machines */
    TAPEPORT_PORT_1_MASK,         /* device only works on port 1 */
    tapecart_enable,              /* device enable function */
    NULL,                         /* NO device specific hard reset function */
    tapecart_shutdown,            /* device shutdown function */
    tapecart_store_motor,         /* set motor line function */
    tapecart_store_write,         /* set write line function */
    tapecart_store_sense,         /* set sense line function */
    NULL,                         /* NO set read line function */
    tapecart_write_snapshot,      /* device snapshot write function */
    tapecart_read_snapshot        /* device snapshot read function */
};

static alarm_t *tapecart_logic_alarm;
static alarm_t *tapecart_pulse_alarm;
static log_t    tapecart_log = LOG_DEFAULT;

/* current physical state of tape port lines */
static int motor_state;
static int write_state;
static int sense_state;

/* last levels the tapecart set, needed for restoring a snapshot */
static int write_out_state = 1;
static int sense_out_state = 1;

/* ------------------------------------------------------------------------- */
/*  tapecart-internal definitions and variables                              */
/* ------------------------------------------------------------------------- */

/* nominal TAP pulse lengths */
#define CBMPULSE_SHORT  0x30
#define CBMPULSE_MEDIUM 0x42
#define CBMPULSE_LONG   0x56
#define PULSE_CYCLES    8

/* trigger values for mode changes */
#define SREG_MAGIC_VALUE_FASTLOAD  0xca65
#define SREG_MAGIC_VALUE_COMMAND   0xfce2

/* flash parameters */
#define FLASH_PAGE_SIZE            256
#define FLASH_ERASE_SIZE           4096

/* note: measured length is 8585 pulses */
#define PULSE_BUFFER_SIZE 8600

/* first five bytes of the header block */
static const uint8_t tape_header[] = {
    0x03,       /* load to absolute address */
    0x02, 0x03, /* start address            */
    0x04, 0x03  /* end address + 1          */
};

/* two-byte autostart address plus checksum */
static const uint8_t tape_startvector[] = {
    0x51,       /* start address of loader */
    0x03,
    0x51 ^ 0x03 /* checksum */
};

/** stream mode variables **/

/* pulse buffer */
typedef struct {
    uint8_t length;
    uint8_t repetitions;
} pulse_t;

static pulse_t        current_pulse;
static unsigned int   pulse_read_index, pulse_count;
static unsigned int   sense_pause_ticks;
static unsigned short tapecart_shiftreg;

/** fastload mode variables **/

/* state list for 2-bit transmission */
typedef enum { /* order matters! visible in snapshot */
    FASTTX_INIT,
    FASTTX_WAIT_WRITE_HIGH,
    FASTTX_BITS_54,
    FASTTX_BITS_76,
    FASTTX_BITS_10,
    FASTTX_BITS_32,
    FASTTX_HOLD_DONE,
    FASTTX_WAIT_WRITE_LOW,
    FASTTX_BUSY_DELAY,
    FASTTX__SENTINEL
} fasttx_state_t;

static fasttx_state_t fasttx_state;

/** shared between fastload mode and command mode */
static transfer_complete_t transfer_complete;

/* 256 bytes needed for dir_lookup */
static uint8_t         transfer_buffer[max(256, FLASH_PAGE_SIZE)];
static uint8_t         transfer_byte;
static unsigned int    transfer_bit, transfer_remaining;



/** command mode variables **/

typedef enum {
  CMD_EXIT = 0,
  CMD_READ_DEVICEINFO,
  CMD_READ_DEVICESIZES,
  CMD_READ_CAPABILITIES,

  CMD_READ_FLASH  = 0x10,
  CMD_READ_FLASH_FAST,
  CMD_WRITE_FLASH,
  CMD_WRITE_FLASH_FAST, /* reserved, but not implemented */
  CMD_ERASE_FLASH_64K,
  CMD_ERASE_FLASH_BLOCK,
  CMD_CRC32_FLASH,

  CMD_READ_LOADER = 0x20,
  CMD_READ_LOADINFO,
  CMD_WRITE_LOADER,
  CMD_WRITE_LOADINFO,

  CMD_LED_OFF = 0x30,
  CMD_LED_ON,
  CMD_READ_DEBUGFLAGS,
  CMD_WRITE_DEBUGFLAGS,

  CMD_DIR_SETPARAMS = 0x40,
  CMD_DIR_LOOKUP,
} command_t;

typedef enum { /* visible in snapshot */
    RX1BIT_RECEIVE = 0,
    RX1BIT_WAIT_WRITE_LOW,
    RX1BIT_WAIT_SENSE_HIGH,
    RX1BIT_WAIT_SENSE_LOW,
    RX1BIT_DELAY,
    RX1BIT_PROCESSING,
    RX1BIT__SENTINEL
} rx1bit_state_t;
static rx1bit_state_t rx1bit_state;

typedef enum { /* visible in snapshot */
    TX1BIT_PREPARE_SEND = 0,
    TX1BIT_SEND,
    TX1BIT_WAIT_WRITE_LOW,
    TX1BIT_WAIT_WRITE_HIGH,
    TX1BIT_PROCESSING,
    TX1BIT__SENTINEL
} tx1bit_state_t;
static tx1bit_state_t tx1bit_state;

static uint8_t tapecart_debugflags[2];
static unsigned int dir_base, dir_entries, dir_name_len, dir_data_len;
static unsigned int flash_address, total_length, block_length;
static clock_t flash_erase_64k_time, flash_erase_page_time;
static clock_t flash_page_write_time;

/** common data **/

/* semi-memory-intensive data that is only allocated when tapecart is enabled */
typedef struct tapecart_buffers_s {
    pulse_t       pulse_buffer[PULSE_BUFFER_SIZE];
    unsigned char data[6 + 65535];    /* additional 3x2 byte header */
} tapecart_buffers_t;

/* callback system for waiting for specific signal levels */
typedef enum { /* visible in snapshots */
    WAIT_NONE = 0,
    WAIT_WRITE_LOW,
    WAIT_WRITE_HIGH,
    WAIT_SENSE_LOW,
    WAIT_SENSE_HIGH,
    WAIT__SENTINEL
} statewait_t;

static statewait_t    wait_for_signal;
static wait_handler_t wait_handler;

static char               *tcrt_filename;
static tapecart_buffers_t *tapecart_buffers;
static tapecart_memory_t  *tapecart_memory;
static tapecart_mode_t     tapecart_mode  = MODE_UNINITIALIZED;
static tapecart_mode_t     requested_mode = MODE_STREAM;

static logic_next_t logic_next;

/* identifies which region data is read from for a transfer */
typedef enum { /* visible in snapshots */
    REGION_NONE = 0,
    REGION_TRANSFER_BUFFER, /* transfer_buffer */
    REGION_BUFFERDATA,      /* tapecart_buffers->data */
    REGION_FLASH,           /* tapecart_memory->flash */
    REGION_LOADER,          /* tapecart_memory->loader */
    REGION_DEBUGFLAGS,      /* tapecart_debugflags */
    REGION_IDSTRING,        /* idstring */
    REGION__SENTINEL
} transfer_region_t;

/* pseudo-pointer into a transmit/receive buffer for easier snapshotting */
typedef struct {
    transfer_region_t region;
    unsigned int offset;
} transfer_ptr_t;

static transfer_ptr_t transfer_ptr;

static void transfer_ptr_increment(void)
{
    transfer_ptr.offset++;
}

static transfer_ptr_t make_transfer_ptr(transfer_region_t region, int offset)
{
    transfer_ptr_t result;
    result.region = region;
    result.offset = offset;
    return result;
}

static void transfer_ptr_clamp(void)
{
    // the pointer is allowed to point one byte beyond each of the buffers
    switch (transfer_ptr.region) {
        case REGION_TRANSFER_BUFFER:
            if (transfer_ptr.offset > sizeof(transfer_buffer)) {
                transfer_ptr.offset = sizeof(transfer_buffer);
            }
            break;

        case REGION_BUFFERDATA:
            if (transfer_ptr.offset > sizeof(tapecart_buffers->data)) {
                transfer_ptr.offset = sizeof(tapecart_buffers->data);
            }
            break;

        case REGION_FLASH:
            if (transfer_ptr.offset > TAPECART_FLASH_SIZE) {
                transfer_ptr.offset = TAPECART_FLASH_SIZE;
            }
            break;

        case REGION_LOADER:
            if (transfer_ptr.offset > TAPECART_LOADER_SIZE) {
                transfer_ptr.offset = TAPECART_LOADER_SIZE;
            }
            break;

        case REGION_DEBUGFLAGS:
            if (transfer_ptr.offset > sizeof(tapecart_debugflags)) {
                transfer_ptr.offset = sizeof(tapecart_debugflags);
            }
            break;

        case REGION_IDSTRING:
            if (transfer_ptr.offset > sizeof(idstring)) {
                transfer_ptr.offset = sizeof(idstring);
            }
            break;

         default:
            log_error(tapecart_log, "invalid region %d in clamp",
                      (int)transfer_ptr.region);
            abort();
    }
}

static void transfer_ptr_store(uint8_t value)
{
    switch (transfer_ptr.region) {
        case REGION_TRANSFER_BUFFER:
            transfer_buffer[transfer_ptr.offset] = value;
            break;

        case REGION_BUFFERDATA:
            tapecart_buffers->data[transfer_ptr.offset] = value;
            break;

        case REGION_FLASH:
            tapecart_memory->flash[transfer_ptr.offset] = value;
            break;

        case REGION_LOADER:
            tapecart_memory->loader[transfer_ptr.offset] = value;
            break;

        case REGION_DEBUGFLAGS:
            tapecart_debugflags[transfer_ptr.offset] = value;
            break;

        case REGION_IDSTRING:
            log_error(tapecart_log, "attempted to write idstring offset %u",
                      transfer_ptr.offset);
            break;

        default:
            log_error(tapecart_log, "invalid transfer region %d in store",
                      (int)(transfer_ptr.region));
            abort();
    }
}

static uint8_t transfer_ptr_fetch(void)
{
    switch (transfer_ptr.region) {
        case REGION_TRANSFER_BUFFER:
            return transfer_buffer[transfer_ptr.offset];

        case REGION_BUFFERDATA:
            return tapecart_buffers->data[transfer_ptr.offset];

        case REGION_FLASH:
            return tapecart_memory->flash[transfer_ptr.offset];

        case REGION_LOADER:
            return tapecart_memory->loader[transfer_ptr.offset];

        case REGION_DEBUGFLAGS:
            return tapecart_debugflags[transfer_ptr.offset];

        case REGION_IDSTRING:
            return idstring[transfer_ptr.offset];

        default:
            log_error(tapecart_log, "invalid transfer region %d in fetch",
                      (int)(transfer_ptr.region));
            abort();
    }
}

/* ------------------------------------------------------------------------- */

/* workaround for inverted tapeport functions to keep my sanity */
static void set_sense(int value)
{
    sense_out_state = !!value;
    tapeport_set_tape_sense(!value, TAPEPORT_PORT_1);
}

static void set_write(int value)
{
    write_out_state = !!value;
    tapeport_set_write_in(!value, TAPEPORT_PORT_1);
}

/* ---------------------------------------------------------------------*/
/*  command line and resource stuff                                     */
/* ---------------------------------------------------------------------*/

static void clear_memory(tapecart_memory_t *memory)
{
    memset(memory, 0xff, sizeof(tapecart_memory_t));
    memory->changed = 0;
}

static int tapecart_enable(int port, int value)
{
    int val = !!value;

    /* don't do anything if the state stays the same */
    if (tapecart_enabled == val) {
        return 0;
    }

    if (val) {
        /* allocate the tapecart's internal memory */
        tapecart_memory = lib_malloc(sizeof(tapecart_memory_t));
        if (tapecart_memory == NULL) {
            return -1;
        }
        clear_memory(tapecart_memory);

        /* allocate buffers */
        tapecart_buffers = lib_calloc(1, sizeof(tapecart_buffers_t));
        if (tapecart_buffers == NULL) {
            return -1;
        }

        /* enable logging */
        tapecart_log = log_open("tapecart");
        if (tapecart_log == LOG_DEFAULT) {
            return -1;
        }

        /* calculate delays, using W25Q16 datasheet "typical" times */
        flash_page_write_time = machine_get_cycles_per_second() * 700 / 1000000;
        flash_erase_64k_time  = machine_get_cycles_per_second() * 180 / 1000;
        flash_erase_page_time = machine_get_cycles_per_second() *  60 / 1000;

        /* allocate alarms */
        if (tapecart_logic_alarm == NULL) {
          tapecart_logic_alarm = alarm_new(maincpu_alarm_context,
                                           "tapecart_logic",
                                           tapecart_logic_alarm_handler, NULL);
          tapecart_pulse_alarm = alarm_new(maincpu_alarm_context,
                                           "tapecart_pulse",
                                           tapecart_pulse_alarm_handler, NULL);
        }

        /* if a TCRT name is already available, load it */
        if (tcrt_filename != NULL && *tcrt_filename != 0) {
            load_tcrt(tcrt_filename, tapecart_memory);
        }

    } else {
        if (tapecart_logic_alarm != NULL) {
            alarm_unset(tapecart_logic_alarm);
            alarm_unset(tapecart_pulse_alarm);
        }

        set_sense(1);

        lib_free(tapecart_memory);
        tapecart_memory = NULL;

        lib_free(tapecart_buffers);
        tapecart_buffers = NULL;

        if (tapecart_log != LOG_DEFAULT) {
            log_close(tapecart_log);
        }
    }

    tapecart_enabled = val;
    return 0;
}

static int set_tapecart_update_tcrt(int value, void *unused_param)
{
    tapecart_update_tcrt = !!value;
    return 0;
}

static int set_tapecart_optimize_tcrt(int value, void *unused_param)
{
    tapecart_optimize_tcrt = !!value;
    return 0;
}

static int set_tapecart_loglevel(int value, void *unused_param)
{
    tapecart_loglevel = value;
    return 0;
}

static const resource_int_t resources_int[] = {
    { "TapecartUpdateTCRT", 1, RES_EVENT_STRICT, (resource_value_t)0,
      &tapecart_update_tcrt, set_tapecart_update_tcrt, NULL },
    { "TapecartOptimizeTCRT", 1, RES_EVENT_STRICT, (resource_value_t)0,
      &tapecart_optimize_tcrt, set_tapecart_optimize_tcrt, NULL },
    { "TapecartLogLevel", 0, RES_EVENT_NO, (resource_value_t)0,
      &tapecart_loglevel, set_tapecart_loglevel, NULL },
    RESOURCE_INT_LIST_END
};

static const resource_string_t resources_string[] = {
    { "TapecartTCRTFilename", "", RES_EVENT_NO, NULL,
      &tcrt_filename, tapecart_attach_tcrt, NULL },
    RESOURCE_STRING_LIST_END
};

int tapecart_resources_init(int amount)
{
    if (tapeport_device_register(TAPEPORT_DEVICE_TAPECART, &tapecart_device) < 0) {
        return -1;
    }

    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    return resources_register_string(resources_string);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-tcrt", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "TapecartTCRTFilename", NULL,
      "<Name>", "Attach TCRT tapecart image" },
    { "-tapecartupdatetcrt", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "TapecartUpdateTCRT", (resource_value_t)1,
      NULL, "Enable updating tapecart .tcrt image" },
    { "+tapecartupdatetcrt", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "TapecartUpdateTCRT", (resource_value_t)0,
      NULL, "Disable updating tapecart .tcrt image" },
    { "-tapecartoptimizetcrt", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "TapecartOptimizeTCRT", (resource_value_t)1,
      NULL, "Enable tapecart .tcrt image optimization on write" },
    { "+tapecartoptimizetcrt", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "TapecartOptimizeTCRT", (resource_value_t)0,
      NULL, "Disable tapecart .tcrt image optimization on write" },
    { "-tapecartloglevel", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "TapecartLogLevel", NULL,
      "<Loglevel>", "Set tapecart log verbosity" },
    CMDLINE_LIST_END
};

int tapecart_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}


/* ---------------------------------------------------------------------*/
/*  endian-independent buffer access functions                          */
/* ---------------------------------------------------------------------*/

static uint32_t get_u32_le(const uint8_t *buf)
{
    uint32_t val = *buf++;

    val |= (uint32_t)(*buf++) << 8;
    val |= (uint32_t)(*buf++) << 16;
    val |= (uint32_t)(*buf  ) << 24;
    return val;
}

static uint32_t get_u24_le(const uint8_t *buffer)
{
    return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16);
}

static uint16_t get_u16_le(const uint8_t *buffer)
{
    return buffer[0] | (buffer[1] << 8);
}

static uint8_t *put_u32_le(uint8_t *buf, uint32_t value)
{
    *buf++ =  value        & 0xff;
    *buf++ = (value >>  8) & 0xff;
    *buf++ = (value >> 16) & 0xff;
    *buf++ = (value >> 24) & 0xff;
    return buf;
}

static uint8_t *put_u24_le(uint8_t *buf, uint32_t value)
{
    *buf++ =  value        & 0xff;
    *buf++ = (value >>  8) & 0xff;
    *buf++ = (value >> 16) & 0xff;
    return buf;
}

static uint8_t *put_u16_le(uint8_t *buf, uint16_t value)
{
    *buf++ =  value        & 0xff;
    *buf++ = (value >>  8) & 0xff;
    return buf;
}


/* ---------------------------------------------------------------------*/
/*  stream mode functions                                               */
/* ---------------------------------------------------------------------*/

static unsigned int needed_space = 0;

static int check_pulsebuffer_size(void)
{
    if (pulse_count >= PULSE_BUFFER_SIZE) {
        log_message(tapecart_log, "Pulse buffer overflow, need %u more",
                    ++needed_space);
        return 1;
    }

    return 0;
}

static void pulse_store(const unsigned char pulselen)
{
    if (check_pulsebuffer_size()) {
        return;
    }

    tapecart_buffers->pulse_buffer[pulse_count].length      = pulselen;
    tapecart_buffers->pulse_buffer[pulse_count].repetitions = 1;
    pulse_count++;
}

static void pulse_store_sync(unsigned int length)
{
    while (length > 0) {
        if (check_pulsebuffer_size()) {
            return;
        }

        tapecart_buffers->pulse_buffer[pulse_count].length = CBMPULSE_SHORT;

        if (length > 255) {
            tapecart_buffers->pulse_buffer[pulse_count].repetitions = 255;
        } else {
            tapecart_buffers->pulse_buffer[pulse_count].repetitions = length;
        }

        length -= tapecart_buffers->pulse_buffer[pulse_count].repetitions;
        pulse_count++;
    }
}

static void pulse_store_bit(const unsigned int index, const unsigned char bit)
{
    if (bit) {
        pulse_store(CBMPULSE_MEDIUM);
        pulse_store(CBMPULSE_SHORT);
    } else {
        pulse_store(CBMPULSE_SHORT);
        pulse_store(CBMPULSE_MEDIUM);
    }
}

static void pulse_store_byte(unsigned char byte)
{
    int parity = 1;
    int i;

    pulse_store(CBMPULSE_LONG);
    pulse_store(CBMPULSE_MEDIUM);

    for (i = 0; i < 8; i++) {
        pulse_store_bit(i, byte & 1);

        if (byte & 1) {
            parity = !parity;
        }

        byte >>= 1;
    }

    pulse_store_bit(8, parity);
}

static void construct_header(void)
{
    unsigned int i;
    unsigned char checksum = 0;

    for (i = 0; i < 5; i++) {
        checksum ^= tape_header[i];
        pulse_store_byte(tape_header[i]);
    }

    for (i = 0; i < 16; i++) {
        checksum ^= tapecart_memory->filename[i];
        pulse_store_byte(tapecart_memory->filename[i]);
    }

    for (i = 0; i < TAPECART_LOADER_SIZE; i++) {
        checksum ^= tapecart_memory->loader[i];
        pulse_store_byte(tapecart_memory->loader[i]);
    }

    pulse_store_byte(checksum);
}

static void construct_startvector(void)
{
    unsigned int i;

    for (i = 0; i < sizeof(tape_startvector)/sizeof(tape_startvector[0]); i++) {
        pulse_store_byte(tape_startvector[i]);
    }
}

static void construct_countdown(int repeat)
{
    unsigned int i;

    for (i = 9; i > 0; i--) {
        if (repeat) {
            pulse_store_byte(i);
        } else {
            pulse_store_byte(i | 0x80);
        }
    }
}

static void construct_datablock(void (*datafunc)(void))
{
    unsigned int repeat;

    for (repeat = 0; repeat < 2; repeat++) {
        construct_countdown(repeat);

        datafunc();

        /* end marker */
        pulse_store(CBMPULSE_LONG);
        pulse_store(CBMPULSE_SHORT);

        /* block gap */
        pulse_store_sync(60);
    }
}

static void construct_pulsestream(void)
{
    pulse_count               = 0;
    pulse_read_index          = 0;
    current_pulse.length      = 0;
    current_pulse.repetitions = 0;

    pulse_store_sync(1500);
    construct_datablock(construct_header);

    pulse_store_sync(1500);
    construct_datablock(construct_startvector);

    pulse_store_sync(100);
}

static int get_next_pulse(void)
{
    /* check for end of stream, force immediate end in non-stream mode */
    if (requested_mode != MODE_STREAM ||
        (current_pulse.repetitions == 0 && pulse_read_index >= pulse_count)) {
        pulse_read_index = 0;
        current_pulse.repetitions = 0;
        return -1;
    }

    if (!current_pulse.repetitions) {
        current_pulse = tapecart_buffers->pulse_buffer[pulse_read_index++];
    }
    current_pulse.repetitions--;

    return current_pulse.length * PULSE_CYCLES;
}

/* ---------------------------------------------------------------------*/
/*  fastload mode functions                                             */
/* ---------------------------------------------------------------------*/

/* common fasttx for fastload mode and READ_FLASH_FAST */
static clock_t fasttx_nibble_advance(void)
{
    /* FIXME: This assumes that 1 cycle == 1 microsecond */

    fasttx_state++; /* advance state */

    switch (fasttx_state) {
    case FASTTX_WAIT_WRITE_HIGH:
        wait_handler    = WH_FASTTX_NIBBLE_ADVANCE;
        wait_for_signal = WAIT_WRITE_HIGH;
        transfer_byte   = transfer_ptr_fetch();
        transfer_ptr_increment();
        transfer_remaining--;

        /* clear busy indication */
        if (tapecart_mode == MODE_FASTLOAD) {
            set_sense(0);
        } else {
            set_sense(1);
        }
        return 0; /* no alarm */

    case FASTTX_BITS_54:
        set_sense(transfer_byte & 0x20);
        set_write(transfer_byte & 0x10);
        logic_next = LOGIC_FASTTX_NIBBLE_ADVANCE;
        return 9;

    case FASTTX_BITS_76:
        set_sense(transfer_byte & 0x80);
        set_write(transfer_byte & 0x40);
        return 9;

    case FASTTX_BITS_10:
        set_sense(transfer_byte & 2);
        set_write(transfer_byte & 1);
        return 9;

    case FASTTX_BITS_32:
        set_sense(transfer_byte & 8);
        set_write(transfer_byte & 4);
        return 10;

    case FASTTX_HOLD_DONE:
        set_sense(1);
        set_write(1);
        return 1;

    case FASTTX_WAIT_WRITE_LOW:
        wait_handler    = WH_FASTTX_NIBBLE_ADVANCE;
        wait_for_signal = WAIT_WRITE_LOW;
        return 0;

    case FASTTX_BUSY_DELAY:
        /* set busy indication */
        if (tapecart_mode == MODE_FASTLOAD) {
            set_sense(1);
        } else {
            set_sense(0);
        }

        logic_next = LOGIC_FASTTX_BYTE_ADVANCE;
        return 1; /* random guess, not sure how much time it takes in reality */

    default:
        log_error(tapecart_log, "In fasttx_advance with unhandled state %u",
                  fasttx_state - 1);
        return 0;
    }
}

static clock_t fasttx_byte_advance(void)
{
    if (transfer_remaining) {
        fasttx_state = FASTTX_INIT;
        return fasttx_nibble_advance();
    } else {
        /* at end of buffer */
        return dispatch_transfer_complete(transfer_complete);
    }
}

static clock_t fastload_complete(void)
{
    tapecart_set_mode(MODE_STREAM);
    return 0;
}

static clock_t fastload_postdelay(void)
{
    /* 200ms delay after fastload */
    logic_next = LOGIC_FASTLOAD_COMPLETE;
    return machine_get_cycles_per_second() / 5;
}

static clock_t transmit_fast(clock_t delay,
                             transfer_ptr_t buffer, unsigned int length,
                             transfer_complete_t complete_callback)
{
    transfer_remaining = length;
    transfer_ptr       = buffer;
    fasttx_state       = FASTTX_INIT;
    logic_next         = LOGIC_FASTTX_NIBBLE_ADVANCE;
    transfer_complete  = complete_callback;
    return delay;
}

static clock_t fastload_mode_init(void)
{
    unsigned char *ptr = tapecart_buffers->data;
    unsigned short loadaddr, endaddr;

    /* retrieve/calculate actual start/end addresses */
    loadaddr = tapecart_memory->flash[tapecart_memory->data_offset    ] |
              (tapecart_memory->flash[tapecart_memory->data_offset + 1] << 8);
    endaddr = loadaddr + tapecart_memory->data_length - 2;

    ptr = put_u16_le(ptr, tapecart_memory->call_address);
    ptr = put_u16_le(ptr, endaddr);
    ptr = put_u16_le(ptr, loadaddr);

    /* copy the actual data to load */
    if (tapecart_memory->data_length >= 2) {
        /* cannot overflow beyond flash size, offset and length are both 16-bit */
        memcpy(ptr,
               tapecart_memory->flash + tapecart_memory->data_offset + 2,
               tapecart_memory->data_length - 2);
    } else {
        log_warning(tapecart_log, "invalid data length %d", tapecart_memory->data_length);
    }

    /* transfer data after 100ms to give the C64 time to turn off the motor */
    return transmit_fast(machine_get_cycles_per_second() / 10,
                         make_transfer_ptr(REGION_BUFFERDATA, 0),
                         6 + tapecart_memory->data_length - 2,
                         TC_FASTLOAD_POSTDELAY);
}


/* ---------------------------------------------------------------------*/
/*  command mode functions                                              */
/* ---------------------------------------------------------------------*/

static int validate_flashaddress(unsigned int address, unsigned int length)
{
    return (address < TAPECART_FLASH_SIZE) &&
        (address + length <= TAPECART_FLASH_SIZE);
}

static clock_t receive_1bit_callback(void)
{
    switch (rx1bit_state) {
        case RX1BIT_RECEIVE:
            transfer_ptr_store((transfer_ptr_fetch() << 1) | !!sense_state);

            if (++transfer_bit == 8) {
                transfer_ptr_increment();
                transfer_remaining--;
                transfer_bit = 0;
                wait_for_signal = WAIT_WRITE_LOW;
                rx1bit_state = RX1BIT_WAIT_WRITE_LOW;
            } else {
                wait_for_signal = WAIT_WRITE_HIGH;
            }
            break;

        case RX1BIT_WAIT_WRITE_LOW:
            /* waiting only triggers on changes, so check that sense is low */
            if (!sense_state) {
                wait_for_signal = WAIT_SENSE_HIGH;
                rx1bit_state = RX1BIT_WAIT_SENSE_HIGH;
                break;
            }

            /* sense is already high: */
            /* fall through */
        case RX1BIT_WAIT_SENSE_HIGH:
            wait_for_signal = WAIT_SENSE_LOW;
            rx1bit_state = RX1BIT_WAIT_SENSE_LOW;
            break;

        case RX1BIT_WAIT_SENSE_LOW:
            rx1bit_state = RX1BIT_DELAY;
            logic_next = LOGIC_RECEIVE_1BIT_CALLBACK;
            return machine_get_cycles_per_second() / 100000; /* 10 microseconds */

        case RX1BIT_DELAY:
            set_sense(0);
            rx1bit_state = RX1BIT_PROCESSING;
            return 5; /* simulate a bit of processing delay */

        case RX1BIT_PROCESSING:
            if (transfer_remaining > 0) {
                wait_for_signal = WAIT_WRITE_HIGH;
                rx1bit_state    = RX1BIT_RECEIVE;
                set_sense(1);
            } else {
                return dispatch_transfer_complete(transfer_complete);
            }
            break;

        case RX1BIT__SENTINEL:
            log_error(tapecart_log, "internal error: in rx1bit sentinel state");
            abort();
    }

    return 0;
}

static clock_t receive_1bit_delayed(void)
{
    set_sense(1);
    wait_for_signal = WAIT_WRITE_HIGH;
    return 0;
}

static clock_t receive_1bit(clock_t delay,
                            transfer_ptr_t buffer, unsigned int length,
                            transfer_complete_t complete_callback)
{
    if (length == 0) {
        log_warning(tapecart_log,
                    "WARNING: attempted to do 1-bit-receive with length 0");
        return dispatch_transfer_complete(complete_callback);
    }

    transfer_ptr       = buffer;
    transfer_remaining = length;
    transfer_bit       = 0;
    transfer_complete  = complete_callback;

    wait_handler = WH_RX1BIT;
    rx1bit_state = RX1BIT_RECEIVE;

    if (delay > 0) {
        logic_next = LOGIC_RECEIVE_1BIT_DELAYED;
        return delay;
    } else {
        set_sense(1);
        wait_for_signal = WAIT_WRITE_HIGH;
        return 0;
    }
}

static clock_t transmit_1bit_callback(void)
{
    switch (tx1bit_state) {
        case TX1BIT_PREPARE_SEND:
            wait_for_signal = WAIT_WRITE_LOW;
            tx1bit_state = TX1BIT_SEND;
            transfer_byte = transfer_ptr_fetch();
            break;

        case TX1BIT_SEND:
            set_sense(transfer_byte & 0x80);
            transfer_byte <<= 1;

            if (++transfer_bit == 8) {
                transfer_ptr_increment();
                transfer_remaining--;
                transfer_bit = 0;
                wait_for_signal = WAIT_WRITE_HIGH;
                tx1bit_state = TX1BIT_WAIT_WRITE_HIGH;
            } else {
                wait_for_signal = WAIT_WRITE_LOW;
            }
            break;

        case TX1BIT_WAIT_WRITE_HIGH:
            tx1bit_state = TX1BIT_WAIT_WRITE_LOW;
            wait_for_signal = WAIT_WRITE_LOW;
            break;

        case TX1BIT_WAIT_WRITE_LOW:
            set_sense(0);
            tx1bit_state = TX1BIT_PROCESSING;
            logic_next = LOGIC_TRANSMIT_1BIT_CALLBACK;
            return 5; /* simulate a bit of processing delay */

        case TX1BIT_PROCESSING:
            if (transfer_remaining > 0) {
                wait_for_signal = WAIT_WRITE_HIGH;
                tx1bit_state    = TX1BIT_PREPARE_SEND;
                set_sense(1);
            } else {
                return dispatch_transfer_complete(transfer_complete);
            }
            break;

        case TX1BIT__SENTINEL:
            log_error(tapecart_log, "internal error: in tx1bit sentinel state");
            abort();
    }

    return 0;
}

static clock_t transmit_1bit_delayed(void)
{
    set_sense(1);
    wait_for_signal = WAIT_WRITE_HIGH;
    return 0;
}

static clock_t transmit_1bit(clock_t delay,
                             transfer_ptr_t buffer, unsigned int length,
                             transfer_complete_t complete_callback)
{
    transfer_ptr       = buffer;
    transfer_remaining = length;
    transfer_bit       = 0;
    transfer_complete  = complete_callback;

    wait_handler    = WH_TX1BIT;
    tx1bit_state    = TX1BIT_PREPARE_SEND;

    if (delay > 0) {
        logic_next = LOGIC_TRANSMIT_1BIT_DELAYED;
        return delay;
    } else {
        set_sense(1);
        wait_for_signal = WAIT_WRITE_HIGH;
        return 0;
    }
}

/*
 * Callbacks for command implementations
 */

static clock_t cmd_read_flash(void)
{
    unsigned int address = get_u24_le(transfer_buffer);
    unsigned int length  = get_u16_le(transfer_buffer + 3);

    if (!validate_flashaddress(address, length)) {
        log_message(tapecart_log,
                    "read attempt beyond end of flash memory: address 0x%X length 0x%04x",
                    address, length);
        address = 0;
    }

    if (tapecart_loglevel > 1) {
        log_message(tapecart_log,
                "reading %u byte from flash address 0x%X",
                length, address);
    }

    /* pre-transmit delay is pretty much handwaved */
    return transmit_1bit(4, make_transfer_ptr(REGION_FLASH, address), length,
                         TC_CMDMODE_RECEIVE_COMMAND);
}

static clock_t cmd_read_flash_fast(void)
{
    unsigned int address = get_u24_le(transfer_buffer);
    unsigned int length  = get_u16_le(transfer_buffer + 3);

    if (!validate_flashaddress(address, length)) {
        log_message(tapecart_log,
                    "read attempt beyond end of flash memory: address 0x%X length 0x%04x",
                    address, length);
        address = 0;
    }

    if (tapecart_loglevel > 1) {
        log_message(tapecart_log, "reading %u byte from flash address 0x%X",
                    length, address);
    }

    /* note: 1 cycle delay before start is probably inaccurate */
    return transmit_fast(1, make_transfer_ptr(REGION_FLASH, address), length,
                         TC_CMDMODE_RECEIVE_COMMAND);
}

static clock_t cmdmode_write_flash_page(void)
{
    int non_erased_write = 0;
    unsigned int i = 0;

    /* accept writes only to bytes that are erased */
    /* (flash datasheets no not specify what happens with non-erased bytes) */
    for (i = 0; i < block_length; i++) {
        if (tapecart_memory->flash[flash_address + i] == 0xff) {
            tapecart_memory->flash[flash_address + i] = transfer_buffer[i];
            tapecart_memory->changed = 1;
        } else if (tapecart_loglevel > 0 && !non_erased_write) {
            log_message(tapecart_log,
                        "detected write to non-erased address at 0x%X",
                        flash_address + i);
            non_erased_write = 1;
        }
    }

    flash_address += block_length;
    total_length  -= block_length;

    if (total_length > 0) {
        block_length = min(FLASH_PAGE_SIZE, total_length);
        return receive_1bit(flash_page_write_time,
                            make_transfer_ptr(REGION_TRANSFER_BUFFER, 0),
                            block_length, TC_CMDMODE_WRITE_FLASH_PAGE);

    } else {
        logic_next = LOGIC_CMDMODE_RECEIVE_COMMAND;
        return flash_page_write_time;
    }
}

static clock_t cmd_write_flash(void)
{
    flash_address = get_u24_le(transfer_buffer);
    total_length  = get_u16_le(transfer_buffer + 3);

    if (!validate_flashaddress(flash_address, total_length)) {
        log_message(tapecart_log,
                    "write attempt beyond end of flash memory: address 0x%X length 0x%04x",
                    flash_address, total_length);
        flash_address = 0;
    }

    if (tapecart_loglevel > 1) {
        log_message(tapecart_log, "writing %u byte to flash address 0x%X",
                    total_length, flash_address);
    }

    if (flash_address & (FLASH_PAGE_SIZE - 1)) {
        /* start is unaligned */
        block_length = FLASH_PAGE_SIZE - (flash_address & (FLASH_PAGE_SIZE - 1));
    } else {
        block_length = FLASH_PAGE_SIZE;
    }
    block_length = min(block_length, total_length);

    return receive_1bit(flash_page_write_time,
                        make_transfer_ptr(REGION_TRANSFER_BUFFER, 0),
                        block_length, TC_CMDMODE_WRITE_FLASH_PAGE);
}

static clock_t cmd_erase_flash_64k(void)
{
    unsigned int address = get_u24_le(transfer_buffer);

    if (!validate_flashaddress(address, 0)) {
        log_message(tapecart_log,
                    "erase attempt beyond end of flash memory: address 0x%X",
                    address);
    } else {
        address = address & ~0xffff;

        if (tapecart_loglevel > 1) {
            log_message(tapecart_log,
                        "erasing 64KiB starting at flash address 0x%X", address);
        }

        memset(tapecart_memory->flash + address, 0xff, 65536);
        tapecart_memory->changed = 1;
    }

    /* simulate typical erase time of SPI flash */
    logic_next = LOGIC_CMDMODE_RECEIVE_COMMAND;
    return flash_erase_64k_time;
}

static clock_t cmd_erase_flash_block(void)
{
    unsigned int address = get_u24_le(transfer_buffer);

    if (!validate_flashaddress(address, 0)) {
        log_message(tapecart_log,
                    "erase attempt beyond end of flash memory: address 0x%X",
                    address);
    } else {
        address = address & ~(FLASH_ERASE_SIZE - 1);

        if (tapecart_loglevel > 1) {
            log_message(tapecart_log,
                        "erasing %d bytes starting at flash address 0x%X",
                        FLASH_ERASE_SIZE, address);
        }

        memset(tapecart_memory->flash + address, 0xff, FLASH_ERASE_SIZE);
        tapecart_memory->changed = 1;
    }

    /* simulate typical erase time of SPI flash */
    logic_next = LOGIC_CMDMODE_RECEIVE_COMMAND;
    return flash_erase_page_time;
}

static clock_t cmd_crc32_flash(void)
{
    unsigned int address = get_u24_le(transfer_buffer);
    unsigned int length  = get_u24_le(transfer_buffer + 3);
    uint32_t crc;

    if (!validate_flashaddress(address, length)) {
        log_message(tapecart_log,
                    "CRC32 attempt beyond end of flash memory: address 0x%X length 0x%X",
                    address, length);
        address = 0;
        length  = 1;
    }

    if (tapecart_loglevel > 1) {
        log_message(tapecart_log,
                    "calculating CRC from flash address 0x%X length %u",
                    address, length);
    }

    crc = crc32_buf((char *)(tapecart_memory->flash + address), length);
    put_u32_le(transfer_buffer, crc & 0xffffffffU);

    /* very handwaved processing time estimate */
    return transmit_1bit(4 * length, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 4,
                         TC_CMDMODE_RECEIVE_COMMAND);
}

static clock_t cmd_write_loadinfo(void)
{
    tapecart_memory->data_offset  = get_u16_le(transfer_buffer);
    tapecart_memory->data_length  = get_u16_le(transfer_buffer + 2);
    tapecart_memory->call_address = get_u16_le(transfer_buffer + 4);
    memcpy(tapecart_memory->filename, transfer_buffer + 6,
           TAPECART_FILENAME_SIZE);
    tapecart_memory->changed = 0;

    if (tapecart_loglevel > 1) {
        log_message(tapecart_log,
                    "write_loadinfo data address 0x%04x data length %d call_address 0x%04x (filename omitted)",
                    tapecart_memory->data_offset,
                    tapecart_memory->data_length,
                    tapecart_memory->call_address);
    }

    return cmdmode_receive_command();
}

static clock_t cmd_dir_setparams(void)
{
    dir_base     = get_u24_le(transfer_buffer);
    dir_entries  = get_u16_le(transfer_buffer + 3);
    dir_name_len = transfer_buffer[5];
    dir_data_len = transfer_buffer[6];

    if (dir_name_len > 16)
        dir_name_len = 16;

    if (!validate_flashaddress(dir_base,
                               dir_entries * (dir_name_len + dir_data_len))) {
        log_message(tapecart_log,
                    "directory search would fall off end of flash: "
                    "base 0x%X namelen %u datalen %u",
                    dir_base, dir_name_len, dir_data_len);
        dir_base    = 0;
        dir_entries = 1;
    }

    if (tapecart_loglevel > 1) {
        log_message(tapecart_log,
                    "dir_setparams base 0x%X entries %u name length %u data length %u",
                    dir_base, dir_entries, dir_name_len, dir_data_len);
    }

    return cmdmode_receive_command();
}

static clock_t cmd_dir_lookup(void)
{
    unsigned int i;
    uint8_t *ptr;
    clock_t processing_delay; /* note: very rough guess */

    ptr = tapecart_memory->flash + dir_base;
    for (i = 0; i < dir_entries; i++) {
        if (ptr >= tapecart_memory->flash + TAPECART_FLASH_SIZE - dir_name_len) {
            log_warning(tapecart_log, "dir lookup beyond end of flash memory");
            break;
        }

        if (!memcmp(transfer_buffer, ptr, dir_name_len)) {
            /* name part matches */
            transfer_buffer[0] = 0;
            memcpy(transfer_buffer + 1, ptr + dir_name_len, dir_data_len);

            if (tapecart_loglevel > 1) {
                log_message(tapecart_log,
                            "successful dir lookup at entry %u", i);
            }

            processing_delay = (dir_name_len + dir_data_len) * (i+1);
            return transmit_1bit(processing_delay,
                                 make_transfer_ptr(REGION_TRANSFER_BUFFER, 0),
                                 1 + dir_data_len, TC_CMDMODE_RECEIVE_COMMAND);
        }

        ptr += dir_name_len + dir_data_len;
    }

    /* no matching entry found */
    if (tapecart_loglevel > 1) {
        log_message(tapecart_log, "failed dir lookup");
    }

    transfer_buffer[0] = 1;
    processing_delay = (dir_name_len + dir_data_len) * dir_entries;
    return transmit_1bit(processing_delay,
                         make_transfer_ptr(REGION_TRANSFER_BUFFER, 0),
                         1, TC_CMDMODE_RECEIVE_COMMAND);
}

/*
 * main command dispatcher and implementation of "simple" commands
 */
static clock_t cmdmode_dispatch_command(void)
{
    uint8_t *ptr;

    if (tapecart_loglevel > 0) {
        log_message(tapecart_log, "received command 0x%02x",
                    transfer_buffer[0]);
    }

    switch (transfer_buffer[0]) {
        case CMD_EXIT:
            tapecart_set_mode(MODE_STREAM);
            break;

        case CMD_READ_DEVICEINFO:
            transmit_1bit(0, make_transfer_ptr(REGION_IDSTRING, 0),
                          (unsigned int)(sizeof(idstring)),
                          TC_CMDMODE_RECEIVE_COMMAND);
            break;

        case CMD_READ_DEVICESIZES:
            ptr = put_u24_le(transfer_buffer, TAPECART_FLASH_SIZE);
            ptr = put_u16_le(ptr, FLASH_PAGE_SIZE);
            ptr = put_u16_le(ptr, FLASH_ERASE_SIZE / FLASH_PAGE_SIZE);
            transmit_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 7,
                          TC_CMDMODE_RECEIVE_COMMAND);
            break;

        case CMD_READ_CAPABILITIES:
            /* no extended capabilities currently exist, so none are supported */
            memset(transfer_buffer, 0, 4);
            transmit_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 4,
                          TC_CMDMODE_RECEIVE_COMMAND);
            break;

        case CMD_READ_FLASH:
            receive_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 5,
                         TC_CMD_READ_FLASH);
            break;

        case CMD_READ_FLASH_FAST:
            receive_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 5,
                         TC_CMD_READ_FLASH_FAST);
            break;

        case CMD_WRITE_FLASH:
            receive_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 5,
                         TC_CMD_WRITE_FLASH);
            break;

        case CMD_ERASE_FLASH_64K:
            receive_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 3,
                         TC_CMD_ERASE_FLASH_64K);
            break;

        case CMD_ERASE_FLASH_BLOCK:
            receive_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 3,
                         TC_CMD_ERASE_FLASH_BLOCK);
            break;

        case CMD_CRC32_FLASH:
            receive_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 6,
                         TC_CMD_CRC32_FLASH);
            break;

        case CMD_READ_LOADER:
            transmit_1bit(0, make_transfer_ptr(REGION_LOADER, 0), TAPECART_LOADER_SIZE,
                          TC_CMDMODE_RECEIVE_COMMAND);
            break;

        case CMD_READ_LOADINFO:
            ptr = put_u16_le(transfer_buffer, tapecart_memory->data_offset);
            ptr = put_u16_le(ptr, tapecart_memory->data_length);
            ptr = put_u16_le(ptr, tapecart_memory->call_address);
            memcpy(ptr, tapecart_memory->filename, TAPECART_FILENAME_SIZE);
            transmit_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0),
                          6 + TAPECART_FILENAME_SIZE,
                          TC_CMDMODE_RECEIVE_COMMAND);
            break;

        case CMD_WRITE_LOADER:
            receive_1bit(0, make_transfer_ptr(REGION_LOADER, 0), TAPECART_LOADER_SIZE,
                         TC_CMDMODE_RECEIVE_COMMAND);
            tapecart_memory->changed = 1;
            break;

        case CMD_WRITE_LOADINFO:
            receive_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0),
                         6 + TAPECART_FILENAME_SIZE,
                         TC_CMD_WRITE_LOADINFO);
            break;

        case CMD_LED_OFF:
        case CMD_LED_ON:
            /* no parameters, ignore */
            cmdmode_receive_command();
            break;

        case CMD_READ_DEBUGFLAGS:
            /* debugflags are stored, but ignored */
            /* technically this is incorrect for bit 0, */
            /* but I suspect nobody will ever use that over the tapeport */
            transmit_1bit(0, make_transfer_ptr(REGION_DEBUGFLAGS, 0), 2,
                          TC_CMDMODE_RECEIVE_COMMAND);
            break;

        case CMD_WRITE_DEBUGFLAGS:
            receive_1bit(0, make_transfer_ptr(REGION_DEBUGFLAGS, 0), 2,
                         TC_CMDMODE_RECEIVE_COMMAND);
            break;

        case CMD_DIR_SETPARAMS:
            receive_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 7,
                         TC_CMD_DIR_SETPARAMS);
            break;

        case CMD_DIR_LOOKUP:
            if (dir_name_len > 0) {
                receive_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0),
                             dir_name_len, TC_CMD_DIR_LOOKUP);
            } else {
                cmd_dir_lookup();
            }
            break;

        default:
            if (tapecart_loglevel > 0) {
                log_message(tapecart_log,
                            "switching to streaming mode due to unknown command");
            }
            tapecart_set_mode(MODE_STREAM);
            break;
    }

    return 0;
}

static clock_t cmdmode_receive_command(void)
{
    alarm_unset(tapecart_logic_alarm);
    return receive_1bit(0, make_transfer_ptr(REGION_TRANSFER_BUFFER, 0), 1,
                        TC_CMDMODE_DISPATCH_COMMAND);
}

static clock_t cmdmode_send_pulses(void)
{
    wait_handler    = WH_CMDMODE_RX_COMMAND;
    wait_for_signal = WAIT_WRITE_LOW;

    logic_next = LOGIC_CMDMODE_SEND_PULSES;
    tapeport_trigger_flux_change(1, TAPEPORT_PORT_1);

    return CBMPULSE_SHORT * PULSE_CYCLES;
}

static clock_t command_mode_init(void)
{
    set_sense(1);
    wait_handler    = WH_CMDMODE_SEND_PULSES;
    wait_for_signal = WAIT_WRITE_HIGH;

    return 0;
}

/* ---------------------------------------------------------------------*/
/*  common fastload/command mode functions                              */
/* ---------------------------------------------------------------------*/

static void tapecart_set_mode(tapecart_mode_t mode)
{
    clock_t delay = 0;

    if (tapecart_mode == mode) {
        return;
    }

    /* clear any pending alarms */
    alarm_unset(tapecart_logic_alarm);
    alarm_unset(tapecart_pulse_alarm);

    tapecart_mode = mode;

    switch (mode) {
        case MODE_REINIT:
        case MODE_STREAM:
            /* reset stream */
            tapecart_mode = MODE_STREAM;
            requested_mode = MODE_STREAM;
            construct_pulsestream();
            set_sense(0);
            sense_pause_ticks = 0;
            alarm_set(tapecart_pulse_alarm, maincpu_clk + get_next_pulse());
            return;

        case MODE_FASTLOAD:
            delay = fastload_mode_init();
            break;

        case MODE_COMMAND:
            /* just one ms delay here */
            logic_next = LOGIC_COMMAND_MODE_INIT;
            delay = machine_get_cycles_per_second() / 1000;
            break;

        default:
            break;
    }

    if (delay) {
        alarm_set(tapecart_logic_alarm, (CLOCK)(maincpu_clk + delay));
    }
}

static void tapecart_pulse_alarm_handler(CLOCK offset, void *data)
{
    alarm_unset(tapecart_pulse_alarm);

    /* pulses are only sent when the motor is active */
    if (tapecart_mode == MODE_STREAM && motor_state) {
        int pulselen = get_next_pulse();

        if (pulselen < 0) {
            /* at end of stream, release sense for 200ms */
            set_sense(1);
            sense_pause_ticks = 210; /* slightly more because of rounding */
            alarm_set(tapecart_logic_alarm,
                      (CLOCK)(maincpu_clk + machine_get_cycles_per_second() / 1000));
        } else {
            tapeport_trigger_flux_change(1, TAPEPORT_PORT_1);
            alarm_set(tapecart_pulse_alarm, maincpu_clk + pulselen - offset);
        }
    } /* no alarm needed if motor was turned off */
}

static void tapecart_logic_alarm_handler(CLOCK offset, void *data)
{
    clock_t next_alarm;

    alarm_unset(tapecart_logic_alarm);

    switch (tapecart_mode) {
        case MODE_STREAM:
            /* pulse completed, advance stream state */

            if (sense_pause_ticks > 0) {
                sense_pause_ticks--;
                if (sense_pause_ticks == 0) {
                    /* end of pause, activate sense again */
                    set_sense(0);
                    if (motor_state) {
                        alarm_set(tapecart_pulse_alarm, maincpu_clk + 10);
                    }
                } else {
                    /* check for mode change */
                    switch (requested_mode) {
                        case MODE_FASTLOAD:
                            if (tapecart_loglevel > 0) {
                                log_message(tapecart_log, "entering fastload mode");
                            }
                            tapecart_set_mode(MODE_FASTLOAD);
                            break;

                        case MODE_COMMAND:
                            if (tapecart_loglevel > 0) {
                                log_message(tapecart_log, "entering command mode");
                            }
                            tapecart_set_mode(MODE_COMMAND);
                            break;

                        default:
                            /* nothing, check again in one ms */
                            alarm_set(tapecart_logic_alarm,
                                      maincpu_clk + (CLOCK)machine_get_cycles_per_second() / 1000);
                            break;
                    }
                }
            }
            break;

        case MODE_FASTLOAD:
        case MODE_COMMAND:
            next_alarm = dispatch_logic_next();

            if (next_alarm != 0) {
                alarm_set(tapecart_logic_alarm,
                        (CLOCK)(maincpu_clk + next_alarm - offset));
            }
            break;

        default:
            log_message(tapecart_log, "alarm while in unhandled mode %u",
                        tapecart_mode);
            break;
    }
}

static clock_t dispatch_wait_handler(void)
{
    switch (wait_handler) {
        case WH_FASTTX_NIBBLE_ADVANCE:
            return fasttx_nibble_advance();

        case WH_RX1BIT:
            return receive_1bit_callback();

        case WH_TX1BIT:
            return transmit_1bit_callback();

        case WH_CMDMODE_RX_COMMAND:
            return cmdmode_receive_command();

        case WH_CMDMODE_SEND_PULSES:
            return cmdmode_send_pulses();

        default:
            log_error(tapecart_log, "internal error: invalid wait handler state %d",
                      (int)wait_handler);
            abort();
    }
}

static clock_t dispatch_transfer_complete(transfer_complete_t tc)
{
    switch (tc) {
        case TC_FASTLOAD_POSTDELAY:
            return fastload_postdelay();

        case TC_CMDMODE_RECEIVE_COMMAND:
            return cmdmode_receive_command();

        case TC_CMDMODE_DISPATCH_COMMAND:
            return cmdmode_dispatch_command();

        case TC_CMDMODE_WRITE_FLASH_PAGE:
            return cmdmode_write_flash_page();

        case TC_CMD_READ_FLASH:
            return cmd_read_flash();

        case TC_CMD_READ_FLASH_FAST:
            return cmd_read_flash_fast();

        case TC_CMD_WRITE_FLASH:
            return cmd_write_flash();

        case TC_CMD_ERASE_FLASH_64K:
            return cmd_erase_flash_64k();

        case TC_CMD_ERASE_FLASH_BLOCK:
            return cmd_erase_flash_block();

        case TC_CMD_CRC32_FLASH:
            return cmd_crc32_flash();

        case TC_CMD_WRITE_LOADINFO:
            return cmd_write_loadinfo();

        case TC_CMD_DIR_SETPARAMS:
            return cmd_dir_setparams();

        case TC_CMD_DIR_LOOKUP:
            return cmd_dir_lookup();

        default:
            log_error(tapecart_log, "internal error: invalid transfer complete state %d",
                      (int)tc);
            abort();
    }
}

static clock_t dispatch_logic_next(void)
{
    switch (logic_next) {
        case LOGIC_FASTTX_NIBBLE_ADVANCE:
            return fasttx_nibble_advance();

        case LOGIC_FASTTX_BYTE_ADVANCE:
            return fasttx_byte_advance();

        case LOGIC_FASTLOAD_COMPLETE:
            return fastload_complete();

        case LOGIC_RECEIVE_1BIT_CALLBACK:
            return receive_1bit_callback();

        case LOGIC_RECEIVE_1BIT_DELAYED:
            return receive_1bit_delayed();

        case LOGIC_TRANSMIT_1BIT_CALLBACK:
            return transmit_1bit_callback();

        case LOGIC_TRANSMIT_1BIT_DELAYED:
            return transmit_1bit_delayed();

        case LOGIC_CMDMODE_RECEIVE_COMMAND:
            return cmdmode_receive_command();

        case LOGIC_CMDMODE_SEND_PULSES:
            return cmdmode_send_pulses();

        case LOGIC_COMMAND_MODE_INIT:
            return command_mode_init();

        default:
            log_error(tapecart_log, "internal error: invalid logic_next state %d",
                      (int)logic_next);
            abort();
    }
}

/* ---------------------------------------------------------------------*/

static void tapecart_shutdown(void)
{
    update_tcrt();
}

static void tapecart_store_motor(int port, int state)
{
    /* called by VICE with the inverted state of the processor port bit, */
    /* which is the physical state of the line */
    motor_state = state;

    if (tapecart_mode == MODE_STREAM) {
        if (motor_state) {
            /* capture write line when motor changes */
            tapecart_shiftreg =
                ((tapecart_shiftreg << 1) | !!write_state) & 0xffff;
            if (tapecart_loglevel > 2) {
                log_message(tapecart_log, "shift contents now $%04x",
                            tapecart_shiftreg);
            }

            /* start sending pulses - the exact delay doesn't matter here */
            alarm_set(tapecart_pulse_alarm, maincpu_clk + 10);

            switch (tapecart_shiftreg) {
                case SREG_MAGIC_VALUE_FASTLOAD:
                    if (tapecart_loglevel > 1) {
                        log_message(tapecart_log,
                                    "found fastload mode magic value in shift register");
                    }
                    requested_mode = MODE_FASTLOAD;
                    break;

                case SREG_MAGIC_VALUE_COMMAND:
                    if (tapecart_loglevel > 1) {
                        log_message(tapecart_log,
                                    "found command mode magic value in shift register");
                    }
                    requested_mode = MODE_COMMAND;
                    break;

                default:
                    break;
            }
        }
    } else {
        if (motor_state) {
            /* technically not 100% like the real cart, but close enough */
            if (tapecart_loglevel > 0) {
                log_message(tapecart_log,
                            "switching to stream mode because motor is active");
            }
            tapecart_set_mode(MODE_STREAM);
        }
    }
}

static void tapecart_store_write(int port, int state)
{
    /* called by VICE with the actual state of the processor port bit */
    clock_t delta_ticks;

    write_state = state;

    if ((state == 0 && wait_for_signal == WAIT_WRITE_LOW) ||
        (state != 0 && wait_for_signal == WAIT_WRITE_HIGH)) {
        wait_for_signal = WAIT_NONE;
        delta_ticks = dispatch_wait_handler();

        if (delta_ticks > 0) {
            alarm_set(tapecart_logic_alarm, (CLOCK)(maincpu_clk + delta_ticks));
        }
    }
}

static void tapecart_store_sense(int port, int inv_state)
{
    /* called by VICE with the INVERTED state of the processor port bit */
    int state = !inv_state;
    clock_t delta_ticks;

    sense_state = state;

    if ((state == 0 && wait_for_signal == WAIT_SENSE_LOW) ||
        (state != 0 && wait_for_signal == WAIT_SENSE_HIGH)) {
        wait_for_signal = WAIT_NONE;
        delta_ticks = dispatch_wait_handler();

        if (delta_ticks > 0) {
            alarm_set(tapecart_logic_alarm, (CLOCK)(maincpu_clk + delta_ticks));
        }
    }
}

/* ---------------------------------------------------------------------*/
/*  TCRT handling                                                       */
/* ---------------------------------------------------------------------*/

#define TCRT_VERSION              1
#define TCRT_SIGNATURE_SIZE       16
#define TCRT_OFFSET_VERSION       16
#define TCRT_OFFSET_DATAADDR      18
#define TCRT_OFFSET_DATALENGTH    20
#define TCRT_OFFSET_CALLADDR      22
#define TCRT_OFFSET_FILENAME      24
#define TCRT_OFFSET_FLAGS         40
#define TCRT_FLAG_LOADERPRESENT   1
#define TCRT_FLAG_DATABLOCKOFFSET 2
#define TCRT_OFFSET_LOADER        41
#define TCRT_OFFSET_FLASHLENGTH   212
#define TCRT_OFFSET_FLASHDATA     216

#define TCRT_HEADER_SIZE          TCRT_OFFSET_FLASHDATA

const uint8_t tcrt_signature[TCRT_SIGNATURE_SIZE] = {
    0x74, 0x61, 0x70, 0x65,
    0x63, 0x61, 0x72, 0x74,
    0x49, 0x6d, 0x61, 0x67,
    0x65, 0x0d, 0x0a, 0x1a
};

/* the default tapecart loader, in case it isn't included in the TCRT file */
static const unsigned char default_loader[TAPECART_LOADER_SIZE] = {
#  include "tapecart-loader.h"
};

static int load_tcrt(const char *filename, tapecart_memory_t *tcmem)
{
    int retval = 0;
    FILE *fd;
    uint8_t tcrt_header[TCRT_HEADER_SIZE];
    uint32_t flashlen;

    fd = fopen(filename, MODE_READ);
    if (fd == NULL) {
        return 0;
    }

    do {
        /* sanity checks */
        if (fread(tcrt_header, TCRT_HEADER_SIZE, 1, fd) < 1) {
            log_error(LOG_DEFAULT, "could not read TCRT header.");
            break;
        }

        if (memcmp(tcrt_header, tcrt_signature, TCRT_SIGNATURE_SIZE)) {
            log_error(LOG_DEFAULT, "TCRT header invalid.");
            break;
        }

        if (tcrt_header[TCRT_OFFSET_VERSION] != TCRT_VERSION) {
            log_error(LOG_DEFAULT, "unknown TCRT version: %d",
                      tcrt_header[TCRT_OFFSET_VERSION]);
            break;
        }

        flashlen = get_u32_le(tcrt_header + TCRT_OFFSET_FLASHLENGTH);
        if (flashlen > TAPECART_FLASH_SIZE) {
            log_error(LOG_DEFAULT, "invalid flash data size in TCRT header: %u",
                      flashlen);
            break;
        }

        /* read data */
        if (fread(tcmem->flash, flashlen, 1, fd) < 1) {
            log_error(LOG_DEFAULT, "could not read data from TCRT file.");
            break;
        }
        memset(tcmem->flash + flashlen, 0xff, TAPECART_FLASH_SIZE - flashlen);

        if (tcrt_header[TCRT_OFFSET_FLAGS] & TCRT_FLAG_LOADERPRESENT) {
            memcpy(tcmem->loader, tcrt_header + TCRT_OFFSET_LOADER,
                   TAPECART_LOADER_SIZE);
        } else {
            memcpy(tcmem->loader, default_loader, TAPECART_LOADER_SIZE);
        }

        memcpy(tcmem->filename, tcrt_header + TCRT_OFFSET_FILENAME, 16);

        tcmem->data_offset  = get_u16_le(tcrt_header + TCRT_OFFSET_DATAADDR);
        tcmem->data_length  = get_u16_le(tcrt_header + TCRT_OFFSET_DATALENGTH);
        tcmem->call_address = get_u16_le(tcrt_header + TCRT_OFFSET_CALLADDR);
        tcmem->changed      = 0;

        retval = 1;
    } while (0);

    fclose(fd);
    return retval;
}

static int save_tcrt(const char *filename, tapecart_memory_t *tcmem)
{
    FILE *fd;
    uint8_t *ptr;
    uint8_t tcrt_header[TCRT_HEADER_SIZE];
    uint32_t flashlen = TAPECART_FLASH_SIZE;
    int retval = 0;

    if (tapecart_optimize_tcrt) {
        /* scan for last non-0xff byte */
        while (flashlen > 0 && tcmem->flash[flashlen - 1] == 0xff) {
            flashlen--;
        }
    }

    /* construct a tcrt header */
    memset(tcrt_header, 0, TCRT_HEADER_SIZE);
    memcpy(tcrt_header, tcrt_signature, TCRT_SIGNATURE_SIZE);

    ptr = put_u16_le(tcrt_header + TCRT_OFFSET_VERSION, TCRT_VERSION);
    ptr = put_u16_le(ptr, tapecart_memory->data_offset);
    ptr = put_u16_le(ptr, tapecart_memory->data_length);
    ptr = put_u16_le(ptr, tapecart_memory->call_address);
    memcpy(ptr, tapecart_memory->filename, TAPECART_FILENAME_SIZE);

    tcrt_header[TCRT_OFFSET_FLAGS] = TCRT_FLAG_LOADERPRESENT;
    memcpy(tcrt_header + TCRT_OFFSET_LOADER, tapecart_memory->loader,
           TAPECART_LOADER_SIZE);
    put_u32_le(tcrt_header + TCRT_OFFSET_FLASHLENGTH, flashlen);

    fd = fopen(filename, MODE_WRITE);
    if (fd == NULL) {
        return 0;
    }

    do {
        if (fwrite(tcrt_header, TCRT_HEADER_SIZE, 1, fd) != 1) {
            break;
        }

        if (fwrite(tapecart_memory->flash, flashlen, 1, fd) != 1) {
            break;
        }

        retval = 1;
    } while (0);

    fclose(fd);
    return retval;
}

/* update TCRT if updates enabled and changes pending */
static void update_tcrt(void)
{
    if (tcrt_filename && tapecart_memory->changed && tapecart_update_tcrt) {
        save_tcrt(tcrt_filename, tapecart_memory);
    }
}


/** \brief  Check magic of the tapecart \a filename
 *
 * Checks the first 13 bytes for 'tapecartImage', which seems to be the TCRT
 * header magic.
 *
 * \return  bool
 */
int tapecart_is_valid(const char *filename)
{
    unsigned char buffer[TCRT_SIGNATURE_SIZE];
    FILE *fp;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return 0;
    }

    if (fread(buffer, 1, TCRT_SIGNATURE_SIZE, fp) != TCRT_SIGNATURE_SIZE) {
        fclose(fp);
        return 0;
    }

    if (memcmp(buffer, tcrt_signature, TCRT_SIGNATURE_SIZE) != 0) {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}


int tapecart_attach_tcrt(const char *filename, void *unused)
{
    if (!tapecart_enabled) {
        /* remember the name in case the tapecart is enabled later */
        if (tcrt_filename != NULL) {
            lib_free(tcrt_filename);
            tcrt_filename = NULL;
        }

        if (filename != NULL && *filename != 0) {
            tcrt_filename = lib_strdup(filename);
        }

        return 0;
    }

    update_tcrt();

    if (tcrt_filename != NULL) {
        lib_free(tcrt_filename);
        tcrt_filename = NULL;
    }

    if (filename == NULL || *filename == 0) {
        clear_memory(tapecart_memory);
    } else {
        if (!load_tcrt(filename, tapecart_memory)) {
            return -1;
        }

        tcrt_filename = lib_strdup(filename);
    }

    tapecart_set_mode(MODE_REINIT);
    return 0;
}


/** \brief  Flush current tapecart data to image file
 *
 * \return  0 on success, < 0 on failure
 */
int tapecart_flush_tcrt(void)
{
    if (tcrt_filename == NULL || tapecart_memory == NULL) {
        return -1;
    }
    if (!save_tcrt(tcrt_filename, tapecart_memory)) {
        return -1;
    }
    return 0;
}


/** \brief  Clean up memory used by the tapecart emulation
 *
 * `trct_filename` is allocated even when the tapecart is disabled, but never
 * freed. This will clean up the memory used by the filename.
 * Also, #tapecart_memory and #tapecart_buffers weren't properly freed.
 *
 * And yes, I know we use ${module}_shutdown() as the name for a cleanup
 * function, but #tapecart_shutdown is already taken.
 */
void tapecart_exit(void)
{
    if (tcrt_filename != NULL) {
        lib_free(tcrt_filename);
        tcrt_filename = NULL;
    }
    if (tapecart_memory != NULL) {
        lib_free(tapecart_memory);
        tapecart_memory = NULL;
    }
    if (tapecart_buffers != NULL) {
        lib_free(tapecart_buffers);
        tapecart_buffers = NULL;
    }
}


/* ---------------------------------------------------------------------*/
/*  snapshots                                                           */
/* ---------------------------------------------------------------------*/

/* TP_TAPECART snapshot module format:
   type   | name                | description
   ------------------------------------------
   DWORD  | flashlen            | number of stored bytes of flash memory
   ARRAY  | flash               | flashlen BYTES, rest of the flash is 0xff
   ARRAY  | loader              | 171 BYTES of loader code
   ARRAY  | filename            | 16 BYTES of file name stored in tapecart
   WORD   | data offset         | offset of the initial loader in tapecart
   WORD   | data length         | length of the initial loader in tapecart
   WORD   | call address        | start address of initial loader
   BYTE   | changed             | indicates if any data was changed since loading the tcrt
   BYTE   | mode                | current tapecart mode
   BYTE   | requested mode      | mode requested by the C64
   BYTE   | motor line state    | state of the motor line
   BYTE   | write line state    | state of the write line
   BYTE   | sense line state    | state of the sense line
   BYTE   | write driven state  | state of the write line set by the cart
   BYTE   | sense driven state  | state of the sense line set by the cart
   WORD   | shift register      | mode detection shift register
   DWORD  | pulse read index    | read position in the pulse buffer
   DWORD  | pulse count         | number of pulses in the pulse buffer
   ARRAY  | pulse buffer        | 2 BYTES per pulse (length, repetitions)
   BYTE   | current pulse len   | length of current pulse
   BYTE   | current pulse reps  | repetition of current pulse
   DWORD  | sense pause ticks   | remaining ms of the end-of-stream pause
   BYTE   | wait for signal     | line transition currently waited for
   BYTE   | wait handler        | next wait handler to dispatch to
   BYTE   | transfer complete   | next transfer complete handler to dispatch to
   BYTE   | logic next          | next logic handler to dispatch to
   BYTE   | fasttx state        | state of the 2-bit transmitter
   BYTE   | rx1bit state        | state of the 1-bit receiver
   BYTE   | tx1bit state        | state of the 1-bit transmitter
   BYTE   | transfer_ptr region | region of transfer_ptr
   DWORD  | transfer_ptr offset | offset of transfer_ptr within the region
   BYTE   | transfer byte       | byte currently being shifted
   DWORD  | transfer bit        | number of bits transferred so far
   DWORD  | transfer remaining  | number of bytes left to transfer
   ARRAY  | transfer buffer     | 256 BYTES of command buffer
   ARRAY  | debug flags         | 2 BYTES of (ignored) debug flags
   DWORD  | dir base            | directory base address in flash
   DWORD  | dir entries         | number of directory entries
   BYTE   | dir name len        | length of the name part of an entry
   BYTE   | dir data len        | length of the data part of an entry
   DWORD  | flash address       | current address of a flash write
   DWORD  | total length        | bytes left to write to flash
   DWORD  | block length        | bytes in the current flash page write
   BYTE   | logic alarm pending | logic alarm is scheduled
   CLOCK  | logic alarm clk     | clock the logic alarm is scheduled for
   BYTE   | pulse alarm pending | pulse alarm is scheduled
   CLOCK  | pulse alarm clk     | clock the pulse alarm is scheduled for
   DWORD  | bufferdata size     | size of the following array
   ARRAY  | bufferdata          | buffer for fastload mode, always 65541 BYTES or absent
*/

static const char snap_module_name[] = "TP_TAPECART";
#define SNAP_MAJOR 1
#define SNAP_MINOR 0

static int read_enum(snapshot_module_t* m, int* value, int sentinel)
{
    if (SMR_B_INT(m, value) < 0) {
        return -1;
    }

    if (*value < 0 || *value >= sentinel) {
        log_error(tapecart_log, "invalid enum value %d in snapshot", *value);
        return -1;
    }

    return 0;
}

static bool alarm_is_pending(alarm_t* alarm, CLOCK* clk)
{
    *clk = 0;
    if (alarm == NULL || alarm->pending_idx < 0) {
        return false;
    }

    *clk = alarm->context->pending_alarms[alarm->pending_idx].clk;
    return true;
}

static int tapecart_write_snapshot(int port, struct snapshot_s *s, int write_image)
{
    /* write_image is ignored on purpose and the content is always saved */
    /* to ensure that it is consistent with the saved program state even if */
    /* writing back to the original tcrt is disabled */
    snapshot_module_t* m;
    uint32_t flashlen = TAPECART_FLASH_SIZE;
    uint8_t logic_pending;
    uint8_t pulse_pending;
    CLOCK logic_clk;
    CLOCK pulse_clk;
    int i;

    if (tapecart_memory == NULL || tapecart_buffers == NULL) {
        /* not initialized, cannot snapshot */
        return -1;
    }

    logic_pending = alarm_is_pending(tapecart_logic_alarm, &logic_clk);
    pulse_pending = alarm_is_pending(tapecart_pulse_alarm, &pulse_clk);

    /* trim unused area at end to reduce snapshot size */
    while (flashlen > 0 && tapecart_memory->flash[flashlen - 1] == 0xff) {
        flashlen--;
    }

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_DW(m, flashlen) < 0
        || (flashlen > 0 &&
            SMW_BA(m, tapecart_memory->flash, flashlen) < 0)
        || SMW_BA(m, tapecart_memory->loader, TAPECART_LOADER_SIZE) < 0
        || SMW_BA(m, tapecart_memory->filename, TAPECART_FILENAME_SIZE) < 0
        || SMW_W(m, tapecart_memory->data_offset) < 0
        || SMW_W(m, tapecart_memory->data_length) < 0
        || SMW_W(m, tapecart_memory->call_address) < 0
        || SMW_B(m, tapecart_memory->changed) < 0
        || SMW_B(m, tapecart_mode) < 0
        || SMW_B(m, requested_mode) < 0
        || SMW_B(m, motor_state) < 0
        || SMW_B(m, write_state) < 0
        || SMW_B(m, sense_state) < 0
        || SMW_B(m, write_out_state) < 0
        || SMW_B(m, sense_out_state) < 0
        || SMW_W(m, tapecart_shiftreg) < 0
        || SMW_DW(m, pulse_read_index) < 0
        || SMW_DW(m, pulse_count) < 0) {
        /* something failed */
        snapshot_module_close(m);
        return -1;
    }

    for (i = 0; i < pulse_count; ++i) {
        if (SMW_B(m, tapecart_buffers->pulse_buffer[i].length) < 0) {
            snapshot_module_close(m);
            return -1;
        }

        if (SMW_B(m, tapecart_buffers->pulse_buffer[i].repetitions) < 0) {
            snapshot_module_close(m);
            return -1;
        }
    }

    if (0
        || SMW_B(m, current_pulse.length) < 0
        || SMW_B(m, current_pulse.repetitions) < 0
        || SMW_DW(m, sense_pause_ticks) < 0
        || SMW_B(m, wait_for_signal) < 0
        || SMW_B(m, wait_handler) < 0
        || SMW_B(m, transfer_complete) < 0
        || SMW_B(m, logic_next) < 0
        || SMW_B(m, fasttx_state) < 0
        || SMW_B(m, rx1bit_state) < 0
        || SMW_B(m, tx1bit_state) < 0
        || SMW_B(m, transfer_ptr.region) < 0
        || SMW_DW(m, transfer_ptr.offset) < 0
        || SMW_B(m, transfer_byte) < 0
        || SMW_DW(m, transfer_bit) < 0
        || SMW_DW(m, transfer_remaining) < 0
        || SMW_BA(m, transfer_buffer, sizeof(transfer_buffer)) < 0
        || SMW_BA(m, tapecart_debugflags, sizeof(tapecart_debugflags)) < 0
        || SMW_DW(m, dir_base) < 0
        || SMW_DW(m, dir_entries) < 0
        || SMW_B(m, dir_name_len) < 0
        || SMW_B(m, dir_data_len) < 0
        || SMW_DW(m, flash_address) < 0
        || SMW_DW(m, total_length) < 0
        || SMW_DW(m, block_length) < 0
        || SMW_B(m, logic_pending) < 0
        || SMW_CLOCK(m, logic_clk) < 0
        || SMW_B(m, pulse_pending) < 0
        || SMW_CLOCK(m, pulse_clk) < 0) {
        /* something failed */
        snapshot_module_close(m);
        return -1;
    }

    if (transfer_ptr.region == REGION_BUFFERDATA) {
        if (SMW_DW(m, sizeof(tapecart_buffers->data)) < 0 ||
            SMW_BA(m, tapecart_buffers->data, sizeof(tapecart_buffers->data))) {
            snapshot_module_close(m);
            return -1;
        }
    } else {
        if (SMW_DW(m, 0) < 0) {
            snapshot_module_close(m);
            return -1;
        }
    }

    return snapshot_module_close(m);
}

static int tapecart_read_snapshot(int port, struct snapshot_s *s)
{
    uint8_t major_version;
    uint8_t minor_version;
    snapshot_module_t* m;
    uint32_t flashlen;
    int tmpmode;
    int tmpreqmode;
    int tmpwaitsignal;
    int tmpwaithandler;
    int tmptranscomplete;
    int tmplogicnext;
    int tmpfasttx;
    int tmprx1bit;
    int tmptx1bit;
    int tmptpregion;
    unsigned int tmpuint;
    int i;
    uint8_t logic_pending;
    uint8_t pulse_pending;
    CLOCK logic_clk;
    CLOCK pulse_clk;

    if (tapecart_memory == NULL || tapecart_buffers == NULL) {
        /* not initialized, cannot restore */
        return -1;
    }

    m = snapshot_module_open(s, snap_module_name, &major_version, &minor_version);
    if (m == NULL) {
        return -1;
    }

    if (snapshot_version_is_bigger(major_version, minor_version,
                                   SNAP_MAJOR, SNAP_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (SMR_DW(m, &flashlen) < 0 || flashlen > TAPECART_FLASH_SIZE) {
        log_error(tapecart_log, "invalid flash data size in snapshot");
        goto fail;
    }

    memset(tapecart_memory->flash, 0xff, TAPECART_FLASH_SIZE);
    if (flashlen > 0 && SMR_BA(m, tapecart_memory->flash, flashlen) < 0) {
        goto fail;
    }

    if (0
        || SMR_BA(m, tapecart_memory->loader, TAPECART_LOADER_SIZE) < 0
        || SMR_BA(m, tapecart_memory->filename, TAPECART_FILENAME_SIZE) < 0
        || SMR_W(m, &tapecart_memory->data_offset) < 0
        || SMR_W(m, &tapecart_memory->data_length) < 0
        || SMR_W(m, &tapecart_memory->call_address) < 0
        || SMR_B_INT(m, &tapecart_memory->changed) < 0
        || read_enum(m, &tmpmode, MODE__SENTINEL) < 0
        || read_enum(m, &tmpreqmode, MODE__SENTINEL) < 0
        || SMR_B_INT(m, &motor_state) < 0
        || SMR_B_INT(m, &write_state) < 0
        || SMR_B_INT(m, &sense_state) < 0
        || SMR_B_INT(m, &write_out_state) < 0
        || SMR_B_INT(m, &sense_out_state) < 0
        || SMR_W_UINT(m, &tmpuint) < 0) {
        goto fail;
    }
    tapecart_shiftreg = tmpuint;
    tapecart_mode = tmpmode;
    requested_mode = tmpreqmode;

    if (0
        || SMR_DW_UINT(m, &pulse_read_index) < 0
        || SMR_DW_UINT(m, &pulse_count) < 0) {
        goto fail;
    }

    if (pulse_count > PULSE_BUFFER_SIZE ||
        pulse_read_index > pulse_count) {
        log_error(tapecart_log, "invalid pulse buffer values in snapshot");
        goto fail;
    }

    for (i = 0; i < pulse_count; ++i) {
        if (SMR_B(m, &tapecart_buffers->pulse_buffer[i].length) < 0) {
            goto fail;
        }

        if (SMR_B(m, &tapecart_buffers->pulse_buffer[i].repetitions) < 0) {
            goto fail;
        }
    }

    if (0
        || SMR_B(m, &current_pulse.length) < 0
        || SMR_B(m, &current_pulse.repetitions) < 0
        || SMR_DW_UINT(m, &sense_pause_ticks) < 0
        || read_enum(m, &tmpwaitsignal, WAIT__SENTINEL) < 0
        || read_enum(m, &tmpwaithandler, WH__SENTINEL) < 0
        || read_enum(m, &tmptranscomplete, TC__SENTINEL) < 0
        || read_enum(m, &tmplogicnext, LOGIC__SENTINEL) < 0
        || read_enum(m, &tmpfasttx, FASTTX__SENTINEL) < 0
        || read_enum(m, &tmprx1bit, RX1BIT__SENTINEL) < 0
        || read_enum(m, &tmptx1bit, TX1BIT__SENTINEL) < 0
        || read_enum(m, &tmptpregion, REGION__SENTINEL) < 0
        || SMR_DW_UINT(m, &transfer_ptr.offset) < 0
        || SMR_B(m, &transfer_byte) < 0
        || SMR_DW_UINT(m, &transfer_bit) < 0
        || SMR_DW_UINT(m, &transfer_remaining) < 0
        || SMR_BA(m, transfer_buffer, sizeof(transfer_buffer)) < 0
        || SMR_BA(m, tapecart_debugflags, sizeof(tapecart_debugflags)) < 0
        || SMR_DW_UINT(m, &dir_base) < 0
        || SMR_DW_UINT(m, &dir_entries) < 0
        || SMR_B_UINT(m, &dir_name_len) < 0
        || SMR_B_UINT(m, &dir_data_len) < 0
        || SMR_DW_UINT(m, &flash_address) < 0
        || SMR_DW_UINT(m, &total_length) < 0
        || SMR_DW_UINT(m, &block_length) < 0
        || SMR_B(m, &logic_pending) < 0
        || SMR_CLOCK(m, &logic_clk) < 0
        || SMR_B(m, &pulse_pending) < 0
        || SMR_CLOCK(m, &pulse_clk) < 0) {
        goto fail;
    }
    wait_for_signal = tmpwaitsignal;
    wait_handler = tmpwaithandler;
    transfer_complete = tmptranscomplete;
    logic_next = tmplogicnext;
    fasttx_state = tmpfasttx;
    rx1bit_state = tmprx1bit;
    tx1bit_state = tmptx1bit;
    transfer_ptr.region = tmptpregion;

    if (0
        || dir_base >= TAPECART_FLASH_SIZE
        || dir_entries > 65535
        || flash_address > TAPECART_FLASH_SIZE
        || total_length > 65535
        || block_length > FLASH_PAGE_SIZE) {
        log_warning(tapecart_log, "invalid values in snapshot");
        dir_base = 0;
        dir_entries = 0;
        flash_address = 0;
        total_length = 0;
        block_length = 0;
        goto fail;
    }

    if (transfer_ptr.region == REGION_BUFFERDATA) {
        unsigned int datasize;

        if (SMR_DW(m, &datasize) < 0 || datasize > sizeof(tapecart_buffers->data)) {
            goto fail;
        }

        if (SMR_BA(m, tapecart_buffers->data, datasize) < 0) {
            goto fail;
        }
    }

    if (logic_pending) {
        alarm_set(tapecart_logic_alarm, logic_clk);
    } else {
        alarm_unset(tapecart_logic_alarm);
    }

    if (pulse_pending) {
        alarm_set(tapecart_pulse_alarm, pulse_clk);
    } else {
        alarm_unset(tapecart_pulse_alarm);
    }

    /* restore the levels set by the tapecart */
    set_sense(sense_out_state);
    set_write(write_out_state);

    transfer_ptr_clamp();
    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}
