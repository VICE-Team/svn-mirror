/*         ______   ___    ___
 *        /\  _  \ /\_ \  /\_ \
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *      By Shawn Hargreaves,
 *      1 Salisbury Road,
 *      Market Drayton,
 *      Shropshire,
 *      England, TF9 1AJ.
 *
 *      Soundblaster driver: supports DMA driven sample playback.
 *
 *      Adapted for use with VICE by Ettore Perazzoli (ettore@comm2000.it).
 *
 *      This file is now part of VICE, the Versatile Commodore Emulator.
 *      See README for copyright notice.
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *      02111-1307  USA.
 *
 */

/* Warning: This might not work with versions of Allegro other than 3.0.  */

#ifndef DJGPP
#error This file should only be used by the djgpp version of Allegro
#endif

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <go32.h>
#include <dpmi.h>
#include <limits.h>
#include <sys/farptr.h>

#include <allegro.h>

#include "log.h"

/* Extern declarations for internal Allegro stuff.  */
extern int _sb_port;
extern int _sb_dma;
extern int _sb_irq;
extern int _dma_allocate_mem(int bytes, int *sel, unsigned long *phys);
extern void _dma_start(int channel, unsigned long addr, int size, int auto_init);
extern void _dma_stop(int channel);
extern unsigned long _dma_todo(int channel);
extern int _install_irq(int num, int (*handler)());
extern void _remove_irq(int num);

static char vicesb_desc[80] = "not initialised";

static int vicesb_in_use = FALSE;	/* is SB being used? */
static int vicesb_stereo = FALSE;	/* in stereo mode? */
static int vicesb_16bit = FALSE;	/* in 16 bit mode? */
static int vicesb_int = -1;		/* interrupt vector */
static int vicesb_dsp_ver = -1;	/* SB DSP version */
static int vicesb_hw_dsp_ver = -1;	/* as reported by autodetect */
static int vicesb_dma_size = -1;	/* size of dma transfer in bytes */
static int vicesb_dma_count = 0;	/* need to resync with dma? */
static int vicesb_max_freq = 0;         /* maximum frequency */
static int vicesb_freq = 0;             /* sample rate */
static volatile int vicesb_semaphore = FALSE;	/* reentrant interrupt? */

static void (*vicesb_interrupt_function)(unsigned long buf);

static int vicesb_sel[2];		/* selectors for the buffers */
static unsigned long vicesb_buf[2];	/* pointers to the two buffers */
static int vicesb_bufnum = 0;	/* the one currently in use */

static unsigned char vicesb_default_pic1;	/* PIC mask flags to restore */
static unsigned char vicesb_default_pic2;

static int vicesb_master_vol;	/* stored mixer settings */
static int vicesb_digi_vol;

static void vicesb_lock_mem();

static log_t vicesb_log = LOG_ERR;

/* vicesb_read_dsp:
 *  Reads a byte from the VICESB DSP chip. Returns -1 if it times out.
 */
static inline volatile int vicesb_read_dsp()
{
    int x;

    for (x = 0; x < 0xffff; x++)
	if (inportb(0x0E + _sb_port) & 0x80)
	    return inportb(0x0A + _sb_port);

    return -1;
}



/* vicesb_write_dsp:
 *  Writes a byte to the SB DSP chip. Returns -1 if it times out.
 */
static inline volatile int vicesb_write_dsp(unsigned char byte)
{
    int x;

    for (x = 0; x < 0xffff; x++) {
	if (!(inportb(0x0C + _sb_port) & 0x80)) {
	    outportb(0x0C + _sb_port, byte);
	    return 0;
	}
    }
    return -1;
}



/* vicesb_voice:
 *  Turns the SB speaker on or off.
 */
static void vicesb_voice(int state)
{
    if (state) {
	vicesb_write_dsp(0xD1);

	if (vicesb_hw_dsp_ver >= 0x300) {	/* set up the mixer */

	    outportb(_sb_port + 4, 0x22);	/* store master volume */
	    vicesb_master_vol = inportb(_sb_port + 5);

	    outportb(_sb_port + 4, 4);	/* store DAC level */
	    vicesb_digi_vol = inportb(_sb_port + 5);
	}
    } else {
	vicesb_write_dsp(0xD3);

	if (vicesb_hw_dsp_ver >= 0x300) {	/* reset previous mixer settings */

	    outportb(_sb_port + 4, 0x22);	/* restore master volume */
	    outportb(_sb_port + 5, vicesb_master_vol);

	    outportb(_sb_port + 4, 4);	/* restore DAC level */
	    outportb(_sb_port + 5, vicesb_digi_vol);
	}
    }
}



/* _vicesb_set_mixer:
 *  Alters the SB-Pro hardware mixer.
 */
int vicesb_set_volume(int volume)
{
    if (vicesb_hw_dsp_ver < 0x300)
	return -1;

    if (volume >= 0) {	/* set DAC level */
	outportb(_sb_port + 4, 4);
	outportb(_sb_port + 5, (volume & 0xF0) | (volume >> 4));
    }
    return 0;
}


/* vicesb_stereo_mode:
 *  Enables or disables stereo output for SB-Pro.
 */
static void vicesb_stereo_mode(int enable)
{
    outportb(_sb_port + 0x04, 0x0E);
    outportb(_sb_port + 0x05, (enable ? 2 : 0));
}


/* vicesb_set_sample_rate:
 *  The parameter is the rate to set in Hz (samples per second).
 */
static void vicesb_set_sample_rate(unsigned int rate)
{
    if (vicesb_16bit) {
	vicesb_write_dsp(0x41);
	vicesb_write_dsp(rate >> 8);
	vicesb_write_dsp(rate & 0xff);
    } else {
	if (vicesb_stereo)
	    rate *= 2;

	vicesb_write_dsp(0x40);
	vicesb_write_dsp((unsigned char) (256 - 1000000 / rate));
    }
}



/* vicesb_reset_dsp:
 *  Resets the SB DSP chip, returning -1 on error.
 */
static int vicesb_reset_dsp()
{
    int x;

    outportb(0x06 + _sb_port, 1);

    for (x = 0; x < 8; x++)
	inportb(0x06 + _sb_port);

    outportb(0x06 + _sb_port, 0);

    if (vicesb_read_dsp() != 0xAA)
	return -1;

    return 0;
}



/* vicesb_read_dsp_version:
 *  Reads the version number of the SB DSP chip, returning -1 on error.
 */
static int vicesb_read_dsp_version(void)
{
    int x, y;

    if (vicesb_hw_dsp_ver > 0)
	return vicesb_hw_dsp_ver;

    if (_sb_port <= 0)
	_sb_port = 0x220;

    if (vicesb_reset_dsp() != 0)
	vicesb_hw_dsp_ver = -1;
    else {
	vicesb_write_dsp(0xE1);
	x = vicesb_read_dsp();
	y = vicesb_read_dsp();
	vicesb_hw_dsp_ver = ((x << 8) | y);
    }

    return vicesb_hw_dsp_ver;
}



/* vicesb_play_buffer:
 *  Starts a dma transfer of size bytes. On cards capable of it, the
 *  transfer will use auto-initialised dma, so there is no need to call
 *  this routine more than once. On older cards it must be called from
 *  the end-of-buffer handler to switch to the new buffer.
 */
static void vicesb_play_buffer(int size)
{
    if (vicesb_dsp_ver <= 0x200) {	/* 8 bit single-shot */
	vicesb_write_dsp(0x14);
	vicesb_write_dsp((size - 1) & 0xFF);
	vicesb_write_dsp((size - 1) >> 8);
    } else if (vicesb_dsp_ver < 0x400) {	/* 8 bit auto-initialised */
	vicesb_write_dsp(0x48);
	vicesb_write_dsp((size - 1) & 0xff);
	vicesb_write_dsp((size - 1) >> 8);
	vicesb_write_dsp(0x90);
    } else {			/* 16 bit */
	size /= 2;
	vicesb_write_dsp(0xB6);
        vicesb_write_dsp(0x10);
	/* vicesb_write_dsp(0x20); */
	vicesb_write_dsp((size - 1) & 0xFF);
	vicesb_write_dsp((size - 1) >> 8);
    }
}

static END_OF_FUNCTION(vicesb_play_buffer);



/* vicesb_interrupt:
 *  The SB end-of-buffer interrupt handler. Swaps to the other buffer
 *  if the card doesn't have auto-initialised dma, and then refills the
 *  buffer that just finished playing.
 */
static int vicesb_interrupt(void)
{
    if (vicesb_dsp_ver <= 0x200) {	/* not auto-initialised */
	_dma_start(_sb_dma, vicesb_buf[1 - vicesb_bufnum],
                   vicesb_dma_size, FALSE);
	vicesb_play_buffer(vicesb_dma_size);
    } else {			/* poll dma position */
	vicesb_dma_count++;
	if (vicesb_dma_count > 16) {
	    vicesb_bufnum = ((_dma_todo(_sb_dma) > (unsigned) vicesb_dma_size)
                             ? 1 : 0);
	    vicesb_dma_count = 0;
	}
    }

    if (!vicesb_semaphore) {
	vicesb_semaphore = TRUE;

        asm volatile ("sti");
        vicesb_interrupt_function((unsigned long) vicesb_buf[vicesb_bufnum]);
        asm volatile ("cli");

	vicesb_semaphore = FALSE;
    }
    vicesb_bufnum = 1 - vicesb_bufnum;

    if (vicesb_16bit)		/* acknowlege SB */
	inportb(_sb_port + 0x0F);
    else
	inportb(_sb_port + 0x0E);

    outportb(0x20, 0x20);	/* acknowledge interrupt */
    outportb(0xA0, 0x20);

    return 0;
}

static END_OF_FUNCTION(vicesb_interrupt);



/* vicesb_detect:
 *  SB detection routine. Uses the BLASTER environment variable,
 *  or 'sensible' guesses if that doesn't exist.
 */
int vicesb_detect(int *is_16bit)
{
    char *blaster = getenv("BLASTER");
    char *msg;
    int dma8 = 1;
    int dma16 = 5;
    int default_freq;

    /* what breed of SB are we looking for? */
    switch (digi_card) {

      case DIGI_SB10:
        vicesb_dsp_ver = 0x100;
        break;

      case DIGI_SB15:
        vicesb_dsp_ver = 0x200;
        break;

      case DIGI_SB20:
        vicesb_dsp_ver = 0x201;
        break;

      case DIGI_SBPRO:
        vicesb_dsp_ver = 0x300;
        break;

      case DIGI_SB16:
        vicesb_dsp_ver = 0x400;
        break;

      default:
        vicesb_dsp_ver = -1;
        break;
    }

    /* parse BLASTER env */
    if (blaster) {
	while (*blaster) {
	    while ((*blaster == ' ') || (*blaster == '\t'))
		blaster++;

	    if (*blaster) {
		switch (*blaster) {
		  case 'a':
		  case 'A':
                    if (_sb_port < 0)
                        _sb_port = strtol(blaster + 1, NULL, 16);
                    break;

		  case 'i':
		  case 'I':
                    if (_sb_irq < 0)
                        _sb_irq = strtol(blaster + 1, NULL, 10);
                    break;

		  case 'd':
		  case 'D':
                    dma8 = strtol(blaster + 1, NULL, 10);
                    break;

		  case 'h':
		  case 'H':
                    dma16 = strtol(blaster + 1, NULL, 10);
                    break;
		}

		while ((*blaster) && (*blaster != ' ') && (*blaster != '\t'))
		    blaster++;
	    }
	}
    }
    if (_sb_port < 0)
	_sb_port = 0x220;

    if (_sb_irq < 0)
	/* _sb_irq = 7; */
	_sb_irq = 5;

    /* make sure we got a good port address */
    if (vicesb_reset_dsp() != 0) {
	static int bases[] = {0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0};
	int i;

	for (i = 0; bases[i]; i++) {
	    _sb_port = bases[i];
	    if (vicesb_reset_dsp() == 0)
		break;
	}
    }
    /* check if the card really exists */
    vicesb_read_dsp_version();
    if (vicesb_hw_dsp_ver < 0) {
	strcpy(allegro_error, "Sound Blaster not found");
	return FALSE;
    }
    if (vicesb_dsp_ver < 0)
	vicesb_dsp_ver = vicesb_hw_dsp_ver;
    else {
	if (vicesb_dsp_ver > vicesb_hw_dsp_ver) {
	    vicesb_hw_dsp_ver = vicesb_dsp_ver = -1;
	    strcpy(allegro_error, "Older SB version detected");
	    return FALSE;
	}
    }

    /* figure out the hardware interrupt number */
    if (_sb_irq > 7)
	vicesb_int = _sb_irq + 104;
    else
	vicesb_int = _sb_irq + 8;

    /* what breed of SB? */
    if (vicesb_dsp_ver >= 0x400) {
	msg = "SB 16";
	vicesb_max_freq = 45454;
    } else if (vicesb_dsp_ver >= 0x300) {
	msg = "SB Pro";
	vicesb_max_freq = 45454;
    } else if (vicesb_dsp_ver >= 0x201) {
	msg = "SB 2.0";
	vicesb_max_freq = 45454;
    } else if (vicesb_dsp_ver >= 0x200) {
	msg = "SB 1.5";
	vicesb_max_freq = 16129;
    } else {
	msg = "SB 1.0";
	vicesb_max_freq = 16129;
    }

    /* can we handle 16 bit playback? */
    if (vicesb_dsp_ver >= 0x400) {
	if (_sb_dma < 0)
	    _sb_dma = dma16;
	vicesb_16bit = TRUE;
    } else {
	if (_sb_dma < 0)
	    _sb_dma = dma8;
	vicesb_16bit = FALSE;
    }

#if 0
    /* can we handle stereo? */
    if (vicesb_dsp_ver >= 0x300) {
	vicesb_stereo = TRUE;
	vicesb_dma_size <<= 1;
    } else
	vicesb_stereo = FALSE;
#else
    vicesb_stereo = FALSE;
#endif

    /* set up the card description */
    sprintf(vicesb_desc, "%s on port %X, using IRQ %d and DMA channel %d",
	    msg, _sb_port, _sb_irq, _sb_dma);
    puts(vicesb_desc);

    *is_16bit = vicesb_16bit;
    return TRUE;
}



/* vicesb_init:
 *  SB init routine: returns zero on success, -1 on failure.
 */
int vicesb_init(int *frequency, int *dma_size,
                void (*interrupt_func)(unsigned long))
{
    if (vicesb_log == LOG_ERR)
        vicesb_log = log_open("SB");

    /* XXX: Do we really need this?  */
    if ((digi_card == DIGI_SB) || (digi_card == DIGI_AUTODETECT)) {
	if (vicesb_dsp_ver <= 0x100)
	    digi_card = DIGI_SB10;
	else if (vicesb_dsp_ver <= 0x200)
	    digi_card = DIGI_SB15;
	else if (vicesb_dsp_ver < 0x300)
	    digi_card = DIGI_SB20;
	else if (vicesb_dsp_ver < 0x400)
	    digi_card = DIGI_SBPRO;
	else
	    digi_card = DIGI_SB16;
    }

    if (*frequency > vicesb_max_freq)
        *frequency = vicesb_max_freq;
    vicesb_freq = *frequency;

    /* Check DMA size.  */
    {
        int min_factor;

        log_message(vicesb_log, "Fixing DMA size; requested %d.", *dma_size);
        if (vicesb_dsp_ver <= 0x200)
            min_factor = 4;
        else
            min_factor = 1;
        if (vicesb_freq < 20000) {
            if (*dma_size < 128 * min_factor)
                *dma_size = 128 * min_factor;
        } else if (vicesb_freq < 40000) {
            if (*dma_size < 256 * min_factor)
                *dma_size = 256 * min_factor;
        } else {
            if (*dma_size < 512 * min_factor)
                *dma_size = 512 * min_factor;
        }
        vicesb_dma_size = *dma_size;
        log_message(vicesb_log, "Fixing DMA size; given %d.", *dma_size);
    }

    if (vicesb_dsp_ver <= 0x200) {	/* two conventional mem buffers */
	if ((_dma_allocate_mem(vicesb_dma_size, &vicesb_sel[0], &vicesb_buf[0]) != 0) ||
	    (_dma_allocate_mem(vicesb_dma_size, &vicesb_sel[1], &vicesb_buf[1]) != 0))
	    return -1;
    } else {			/* auto-init dma, one big buffer */
	if (_dma_allocate_mem(vicesb_dma_size * 2, &vicesb_sel[0], &vicesb_buf[0]) != 0)
	    return -1;

	vicesb_sel[1] = vicesb_sel[0];
	vicesb_buf[1] = vicesb_buf[0] + vicesb_dma_size;
    }

    {
        /* Clear buffers.  */
        int i;

        _farsetsel(_dos_ds);

        if (vicesb_16bit) {
            for (i = 0; i < vicesb_dma_size; i += 2) {
                _farnspokew(vicesb_buf[0] + i, 0x0);
                _farnspokew(vicesb_buf[1] + i, 0x0);
            }
        } else {
            for (i = 0; i < vicesb_dma_size; i++) {
                _farnspokeb(vicesb_buf[0] + i, 0x80);
                _farnspokeb(vicesb_buf[1] + i, 0x80);
            }
        }
    }

    vicesb_lock_mem();
    vicesb_bufnum = 0;

    vicesb_default_pic1 = inportb(0x21);
    vicesb_default_pic2 = inportb(0xA1);

    if (_sb_irq > 7) {		/* enable irq2 and PIC-2 irq */
	outportb(0x21, vicesb_default_pic1 & 0xFB);
	outportb(0xA1, vicesb_default_pic2 & (~(1 << (_sb_irq - 8))));
    } else			/* enable PIC-1 irq */
	outportb(0x21, vicesb_default_pic1 & (~(1 << _sb_irq)));

    if (_install_irq(vicesb_int, vicesb_interrupt) < 0) {
        log_error(vicesb_log, "Cannot install IRQ %d.", vicesb_int);
        return FALSE;
    }

    vicesb_voice(1);
    vicesb_set_sample_rate(*frequency);

#if 0
    if ((vicesb_hw_dsp_ver >= 0x300) && (vicesb_dsp_ver < 0x400))
	vicesb_stereo_mode(vicesb_stereo);
#else
    vicesb_stereo_mode(0);
#endif

    if (vicesb_dsp_ver <= 0x200)
	_dma_start(_sb_dma, vicesb_buf[0], vicesb_dma_size, FALSE);
    else
	_dma_start(_sb_dma, vicesb_buf[0], vicesb_dma_size * 2, TRUE);

    vicesb_play_buffer(vicesb_dma_size);

    vicesb_in_use = TRUE;

    vicesb_interrupt_function = interrupt_func;

    return TRUE;
}



/* vicesb_close:
 *  SB driver cleanup routine, removes ints, stops dma, frees buffers, etc.
 */
void vicesb_close(void)
{
    /* halt sound output */
    vicesb_voice(0);

    /* stop dma transfer */
    _dma_stop(_sb_dma);

    if (vicesb_dsp_ver <= 0x0200)
	vicesb_write_dsp(0xD0);

    vicesb_reset_dsp();

    /* restore interrupts */
    _remove_irq(vicesb_int);

    /* reset PIC channels */
    outportb(0x21, vicesb_default_pic1);
    outportb(0xA1, vicesb_default_pic2);

    /* free conventional memory buffer */
    __dpmi_free_dos_memory(vicesb_sel[0]);
    if (vicesb_sel[1] != vicesb_sel[0])
	__dpmi_free_dos_memory(vicesb_sel[1]);

    vicesb_in_use = FALSE;
}


/* vicesb_lock_mem:
 *  Locks all the memory touched by parts of the SB code that are executed
 *  in an interrupt context.
 */
static void vicesb_lock_mem()
{
    LOCK_VARIABLE(digi_sb);
    LOCK_VARIABLE(midi_sb_out);
    LOCK_VARIABLE(_sb_port);
    LOCK_VARIABLE(_sb_dma);
    LOCK_VARIABLE(_sb_irq);
    LOCK_VARIABLE(vicesb_int);
    LOCK_VARIABLE(vicesb_in_use);
    LOCK_VARIABLE(vicesb_dsp_ver);
    LOCK_VARIABLE(vicesb_hw_dsp_ver);
    LOCK_VARIABLE(vicesb_dma_size);
    LOCK_VARIABLE(vicesb_freq);
    LOCK_VARIABLE(vicesb_sel);
    LOCK_VARIABLE(vicesb_buf);
    LOCK_VARIABLE(vicesb_bufnum);
    LOCK_VARIABLE(vicesb_default_pic1);
    LOCK_VARIABLE(vicesb_default_pic2);
    LOCK_VARIABLE(vicesb_dma_count);
    LOCK_VARIABLE(vicesb_semaphore);
    LOCK_FUNCTION(vicesb_play_buffer);
    LOCK_FUNCTION(vicesb_interrupt);
}
