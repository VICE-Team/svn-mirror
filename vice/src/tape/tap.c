/*
 * tap.c - TAP file support.
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
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "datasette.h"
#include "lib.h"
#include "tap.h"
#include "tape.h"
#include "types.h"
#include "util.h"
#include "zfile.h"


#define TAP_PULSE_SHORT(x) \
    ((x) >= tap_pulse_short_min && (x) <= tap_pulse_short_max)
#define TAP_PULSE_MIDDLE(x) \
    ((x) >= tap_pulse_middle_min && (x) <= tap_pulse_middle_max)
#define TAP_PULSE_LONG(x) \
    ((x) >= tap_pulse_long_min && (x) <= tap_pulse_long_max)

#define TAP_PILOT_HEADER_MIN 1000
#define TAP_PILOT_DATA_MIN   1000

#define TAP_PILOT_REPEAT_MIN 50
#define TAP_PILOT_REPEAT_MAX 500


/* Default values are for C64 tapes.  Call tap_init() to change. */
static int tap_pulse_short_min  =  36;
static int tap_pulse_short_max  =  54;
static int tap_pulse_middle_min =  55;
static int tap_pulse_middle_max =  73;
static int tap_pulse_long_min   =  74;
static int tap_pulse_long_max   = 100;


static int tap_header_read(tap_t *tap, FILE *fd)
{
    BYTE buf[TAP_HDR_SIZE];

    if (fread(buf, TAP_HDR_SIZE, 1, fd) != 1)
        return -1;

    if (strncmp("C64-TAPE-RAW", (char *)&buf[TAP_HDR_MAGIC_OFFSET], 12)
        && strncmp("C16-TAPE-RAW", (char *)&buf[TAP_HDR_MAGIC_OFFSET], 12))
        return -1;

    tap->version = buf[TAP_HDR_VERSION];
    tap->system = buf[TAP_HDR_SYSTEM];

    memcpy(tap->name, &buf[TAP_HDR_MAGIC_OFFSET], 12);

    return 0;
}

static tap_t *tap_new(void)
{
    tap_t *tap;

    tap = lib_calloc(1, sizeof(tap_t));

    tap->file_name = NULL;
    tap->counter = 0;
    tap->current_file_seek_position = 0;
    tap->mode = DATASETTE_CONTROL_STOP;
    tap->offset = TAP_HDR_SIZE;
    tap->has_changed = 0;
    tap->current_file_number = -1;
    tap->current_file_data = NULL;
    tap->current_file_size = 0;

    return tap;
}

tap_t *tap_open(const char *name, unsigned int *read_only)
{
    FILE *fd;
    tap_t *new;

    fd = NULL;

    if (*read_only == 0)
        fd = zfopen(name, MODE_READ_WRITE);

    if (fd == NULL) {
        fd = zfopen(name, MODE_READ);
        if (fd == NULL)
            return NULL;
        *read_only = 1;
    } else {
        *read_only = 0;
    }

    new = tap_new();

    if (tap_header_read(new, fd) < 0) {
        zfclose(fd);
        lib_free(new);
        return NULL;
    }

    new->fd = fd;
    new->read_only = *read_only;

    new->size = (int)util_file_length(fd) - TAP_HDR_SIZE;

    if (new->size < 3) {
        zfclose(new->fd);
        lib_free(new);
        return NULL;
    }

    new->file_name = lib_stralloc(name);
    new->tap_file_record = calloc(1, sizeof(tape_file_record_t));
    new->current_file_number = -1;
    new->current_file_data = NULL;
    new->current_file_size = 0;

    return new;
}

int tap_close(tap_t *tap)
{
    int retval;

    if (tap->fd != NULL) {
        if (tap->has_changed)
            if (!fseek(tap->fd,TAP_HDR_LEN,SEEK_SET))
                util_dword_write(tap->fd, (DWORD *)&tap->size, 4);
        retval = zfclose(tap->fd);
        tap->fd = NULL;
    } else {
        retval = 0;
    }

    if (tap->current_file_data != NULL)
        lib_free(tap->current_file_data);
    if (tap->file_name != NULL)
        lib_free(tap->file_name);
    if (tap->tap_file_record)
        lib_free(tap->tap_file_record);
    lib_free(tap);

    return retval;
}

/* ------------------------------------------------------------------------- */

int tap_create(const char *name)
{
    FILE *fd;
    BYTE block[256];

    memset(block, 0, sizeof(block));

    fd = fopen(name, MODE_WRITE);

    if (fd == NULL)
        return -1;

    /* create an empty tap */
    strcpy((char *)&block[TAP_HDR_MAGIC_OFFSET], "C64-TAPE-RAW");

    block[TAP_HDR_VERSION] = 1;

    util_dword_to_le_buf(&block[TAP_HDR_LEN], 4);

    if (fwrite(block, 24, 1, fd) < 1) {
        fclose(fd);
        return -1;
    }

    fclose(fd);

    return 0;
}

/* ------------------------------------------------------------------------- */

inline static void tap_unget_bit(tap_t *tap)
{
    fseek(tap->fd, -1, SEEK_CUR);
}

inline static int tap_get_bit(tap_t *tap)
{
    BYTE data;
    size_t res;

    res = fread(&data, 1, 1, tap->fd);

    if (res == 0)
        return -1;

    /*if( TAP_PULSE_SHORT(data) )
      fprintf(stderr, "s");
    else if( TAP_PULSE_MIDDLE(data) )
      fprintf(stderr, "m");
    else if( TAP_PULSE_LONG(data) )
    fprintf(stderr, "l");*/

    return (int)data;
}

static int tap_find_pilot(tap_t *tap, unsigned int min_length)
{
    unsigned int i, count, res;
    BYTE data[256];

    count = 0;
    while( count<min_length )
      {
        res = fread(&data, 256, 1, tap->fd);

        if( res < 1 )
          return -1;

        for (i = 0; (i < 256) && (count < min_length) ; i++)
          {
            if( TAP_PULSE_SHORT(data[i]) )
              count++;
            else
              count = 0;
          }
      }

    return (int) (ftell(tap->fd) - count);
}

static int tap_skip_pilot(tap_t *tap)
{
    int data, count;

    count = 0;
    do {
        data = tap_get_bit(tap);
        if (data < 0) return -1;
        count++;
    } while( TAP_PULSE_SHORT(data) );

    tap_unget_bit(tap);
    return count;
}

static int tap_read_byte(tap_t *tap)
{
    unsigned int i, checksum;
    int data, data2;
    BYTE read;

    read = 0;
    checksum = 1;

    data = tap_get_bit(tap);
    if (data < 0)
        return -1;
    if (!TAP_PULSE_LONG(data))
        return -2;

    data = tap_get_bit(tap);
    if (data < 0)
        return -1;
    if (TAP_PULSE_SHORT(data))
        return -3;
    if (!TAP_PULSE_MIDDLE(data))
        return -2;

    for (i = 0; i < 8; i++) {
        data = tap_get_bit(tap);
        if (data < 0)
            return -1;
        data2 = tap_get_bit(tap);
        if (data2 < 0)
            return -1;
        read >>= 1;
        if (TAP_PULSE_MIDDLE(data) && TAP_PULSE_SHORT(data2)) {
            read |= 0x80;
            checksum ^= 1;
        }
    }

    data = tap_get_bit(tap);
    if (data < 0)
        return -1;
    data2 = tap_get_bit(tap);
    if (data2 < 0)
        return -1;

    if (TAP_PULSE_MIDDLE(data) && TAP_PULSE_SHORT(data2))
        checksum ^= 1;

    if (checksum)
        return -2;

    return read;
}

static int tap_read_sync(tap_t *tap, int start)
{
    unsigned int i;
    int read;

    for (i = 0; i < 9; i++) {
        read = tap_read_byte(tap);
        if (read < 0)
            return read;
        if (read != start - (int)i)
            return -2;
    }
    return 0;
}

static int tap_read_block_once(tap_t *tap, BYTE *buffer, int buf_size)
{
  int count, ret, data, data2;
  BYTE checksum;

  /* skip pilot */
  ret=tap_skip_pilot(tap);
  if( ret<0 ) return ret;

  /* check sync */
  ret = tap_read_sync(tap, 0x89);
  if( ret<0 ) return ret;

  /* read data */
  count    = 0;
  checksum = 0;
  data2    = -1;
  while(1)
    {
      data = tap_read_byte(tap);
      if( data==-3 )
        {
          /* found L-S sequence (end-of-block) */
          if( checksum != 0 )
            return -2; /* checksum error */
          else
            return count;
        }
      else if( data<0 )
        return data;
      else
        {
          if( data2>=0 )
            {
              if( count<buf_size ) buffer[count] = (BYTE) data2;
              count++;
            }

          data2 = data;
        }

      checksum ^= (BYTE) data;
    }
}

static int tap_read_block(tap_t *tap, BYTE *buffer, int buf_size)
{
  int ret;

  /* try to read block */
  ret = tap_read_block_once(tap, buffer, buf_size);
  if( ret==-1 ) 
    {
      /* reached the end of the tape */
      return -1;
    }
  else if( ret<0 )
    {
      /* read error.  find start of repeat */
      ret = tap_find_pilot(tap, TAP_PILOT_REPEAT_MIN);
      if( ret<0 ) return ret;

      /* if the pilot we found is too long then it's an 
	     entirely different block */ 
      ret = tap_skip_pilot(tap);
      if( ret < 0 ) return ret;
      if( ret > TAP_PILOT_REPEAT_MAX ) return -2;

      /* try again */
      ret = tap_read_block_once(tap, buffer, buf_size);
    }

  return ret;
}

static int tap_read_header(tap_t *tap)
{
  int ret;
  BYTE buffer[21];

  /* read header data */
  ret = tap_read_block(tap, buffer, 21);
  if( ret<0  ) return ret;
  if( ret<21 ) return -2;

  /* extract info */
  tap->tap_file_record->type       = buffer[0];
  tap->tap_file_record->start_addr = (WORD)(buffer[1] + buffer[2] * 256);
  tap->tap_file_record->end_addr   = (WORD)(buffer[3] + buffer[4] * 256);
  memcpy(tap->tap_file_record->name, buffer+5, 16);

  return 0;
}

static int tap_find_header(tap_t *tap)
{
  int res, pilot_start;
  
  while (1) 
    {
      /* find the beginning of a header */
      pilot_start = tap_find_pilot(tap, TAP_PILOT_HEADER_MIN);
      if( pilot_start < 0 )
        {
          /* reached the end of the tape */
          return -1;
        }
      
      /* try to read the header */
      res = tap_read_header(tap);
      if( res == 0 )
        {
          /* success.  Rewind to start of header and return. */
          fseek(tap->fd, pilot_start, SEEK_SET);
          tap->current_file_seek_position = pilot_start;
          return 0;
        }
    }
}


static int tap_read_file_prg(tap_t *tap)
{
  int size, ret;
  size = tap->tap_file_record->end_addr - tap->tap_file_record->start_addr + 1;

  if( size<0 )
    return -1;
  else
    {
      tap->current_file_size = size;
      tap->current_file_data = lib_malloc(tap->current_file_size);

      /* find data pilot */
      ret = tap_find_pilot(tap, TAP_PILOT_DATA_MIN);
      if( ret<0 ) return ret;

      return tap_read_block(tap, tap->current_file_data,
                            tap->current_file_size);
    }
}


static int tap_read_file_seq(tap_t *tap)
{
  int ret;
  BYTE buffer[200];

  while(1)
    {
      /* find data pilot */
      ret = tap_find_pilot(tap, TAP_PILOT_DATA_MIN);
      if( ret<0 ) 
        {
          /* no more pilot found => end of data */
          break;
        }

      /* read next data block */
      ret = tap_read_block(tap, buffer, 200);
      if( ret<1 ) return -1;

      if( ret<1 || buffer[0] != 2 )
        { 
          /* next block is not a data continuation block => end of data */
          break;
        }

      /* add received data minus the first byte (which is the continuation
         marker) */
      tap->current_file_size += ret-1;
      tap->current_file_data  = lib_realloc(tap->current_file_data, 
                                            tap->current_file_size);
      memcpy(tap->current_file_data+tap->current_file_size-ret+1,
             buffer+1, ret-1);
    }

  return 0;
}


static int tap_read_file(tap_t *tap)
{
  int res;

  /* clear old file data */
  tap->current_file_size = 0;
  if( tap->current_file_data != NULL )
    {
      lib_free(tap->current_file_data);
      tap->current_file_data = NULL;
    }
  
  /* read header */
  res = tap_read_header(tap);
  if( res>=0 )
    {
      switch( tap->tap_file_record->type )
        {
        case  1: res = tap_read_file_prg(tap); break;
        case  3: res = tap_read_file_prg(tap); break;
        case  4: res = tap_read_file_seq(tap); break;
        default: res = -1;
        }
    }

  if( res<0 )
    {
      /* we failed to read the file.  Set size=1 and data=NULL to 
         permanently indicate error condition */
      tap->current_file_size = 1;
      if( tap->current_file_data != NULL )
        {
          lib_free(tap->current_file_data);
          tap->current_file_data = NULL;
        }
    }

  return res;
}


/* ------------------------------------------------------------------------- */

tape_file_record_t *tap_get_current_file_record(tap_t *tap)
{
  return tap->tap_file_record;
}

int tap_seek_start(tap_t *tap)
{
  /* clear old file data */
  tap->current_file_size = 0;
  if( tap->current_file_data != NULL )
    {
      lib_free(tap->current_file_data);
      tap->current_file_data = NULL;
    }
  
  tap->current_file_number = -1;
  tap->current_file_seek_position = 0;
  fseek(tap->fd, tap->offset, SEEK_SET);
  return 0;
}

int tap_seek_to_file(tap_t *tap, unsigned int file_number)
{
  tap_seek_start(tap);
  while( (int) file_number > tap->current_file_number )
    {
      if( tap_seek_to_next_file(tap, 0) < 0 )
        return -1;
    }

  return 0;
}

int tap_seek_to_next_file(tap_t *tap, unsigned int allow_rewind)
{
  int res;

  if (tap == NULL)
    return -1;

  /* clear old file content buffer */
  tap->current_file_size = 0;
  if( tap->current_file_data != NULL )
    {
      lib_free(tap->current_file_data);
      tap->current_file_data = NULL;
    }

  do
    {
      /* skip over current pilot.  that way, tap_find_header() will
         actually find the NEXT header and not the current one again.
         don't skip if we're at the beginning of the file. */
      if( tap->current_file_number>=0 )
        tap_skip_pilot(tap);
  
      res = tap_find_header(tap);
      if( res < 0 )
        {
          if( allow_rewind ) 
            {
              tap->current_file_number = -1;
              tap_seek_start(tap);
              allow_rewind = 0; /* don't rewind again.  */
              continue;
            } 
          else 
            {
              return -1;
            }
        }
    }
  while( tap->tap_file_record->type == 2 ); /* skip over seq file continuation blocks */
  
  tap->current_file_number++;
  return 0;
}

int tap_read(tap_t *tap, BYTE *buf, size_t size)
{
  if( tap->current_file_data==NULL )
    {
      /* no file data yet */
      if( tap->current_file_size>0 )
        return -1; /* data==NULL and size>0 indicates read error */
      else if( tap_read_file(tap)<0 )
        return -1; /* reading the file failed */
      else
        tap->current_file_data_pos = 0;
    }
  
  if( tap->current_file_data_pos < tap->current_file_size )
    {
      if( size > tap->current_file_size-tap->current_file_data_pos ) 
        size = tap->current_file_size-tap->current_file_data_pos;

      memcpy(buf, tap->current_file_data+tap->current_file_data_pos, size);
      tap->current_file_data_pos += size;

      return (int) size;
    }

  return 0;
}


void tap_get_header(tap_t *tap, BYTE *name)
{
    memcpy(name, tap->name, 12);
}


void tap_init(const tape_init_t *init)
{
    tap_pulse_short_min = init->pulse_short_min / 8;
    tap_pulse_short_max = init->pulse_short_max / 8;
    tap_pulse_middle_min = init->pulse_middle_min / 8;
    tap_pulse_middle_max = init->pulse_middle_max / 8;
    tap_pulse_long_min = init->pulse_long_min / 8;
    tap_pulse_long_max = init->pulse_long_max / 8;
}

