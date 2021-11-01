/*
 * uistatusbar.c - SDL statusbar.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
 *
 * Based on code by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "drive.h"
#include "kbd.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uiapi.h"
#include "uimenu.h"
#include "uistatusbar.h"
#include "videoarch.h"
#include "vsyncapi.h"

/* ----------------------------------------------------------------- */
/* static functions/variables */

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/*  00000000001111111111222222222233333333334444444444555555555566666666667777777777  */
/*  01234567890123456789012345678901234567890123456789012345678901234567890123456789  */
/* "100%P50fps  000>  8:0T35  8:1T35  9:0T35  9:1T35 10:0T35 10:1T35 11:0T35 11:1T35" */
#define BLANKLINE \
   "                                                                                "

#define MAX_STATUSBAR_LEN           128
#define STATUSBAR_SPEED_POS         0
#define STATUSBAR_PAUSE_POS         4
#define STATUSBAR_TAPE_POS          12
#define STATUSBAR_DRIVE_POS         17

static int statusbar_drive_offset[4][2];    /* points to the position of the T in the widget */
static int statusbar_drive_track[4][2];
static int statusbar_drive_side[4][2];

static char statusbar_text[MAX_STATUSBAR_LEN + 1] = BLANKLINE;
static char kbdstatusbar_text[MAX_STATUSBAR_LEN + 1] = BLANKLINE;

static menufont_t *menufont = NULL;
static int pitch;
static int draw_offset;

static inline void uistatusbar_putchar(uint8_t c, int pos_x, int pos_y, uint8_t color_f, uint8_t color_b)
{
    int x, y;
    uint8_t fontchar;
    uint8_t *font_pos;
    uint8_t *draw_pos;

    font_pos = &(menufont->font[menufont->translate[(int)c]]);
    draw_pos = &(sdl_active_canvas->draw_buffer->draw_buffer[pos_x * menufont->w + pos_y * menufont->h * pitch]);

    draw_pos += draw_offset;

    for (y = 0; y < menufont->h; ++y) {
        fontchar = *font_pos;
        for (x = 0; x < menufont->w; ++x) {
            draw_pos[x] = (fontchar & (0x80 >> x)) ? color_f : color_b;
        }
        ++font_pos;
        draw_pos += pitch;
    }
}

static int tape_counter = 0;
static int tape_enabled = 0;
static int tape_motor = 0;
static int tape_control = 0;

static void display_tape(void)
{
    int len;

    if (tape_enabled) {
        len = sprintf(&(statusbar_text[STATUSBAR_TAPE_POS]), "%c%03d%c", (tape_motor) ? '*' : ' ', tape_counter, " >f<R"[tape_control]);
    } else {
        len = sprintf(&(statusbar_text[STATUSBAR_TAPE_POS]), "     ");
    }
    statusbar_text[STATUSBAR_TAPE_POS + len] = ' ';

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

static void display_speed(void)
{
    int len;
    unsigned char sep;
    double vsync_metric_cpu_percent;
    double vsync_metric_emulated_fps;
    int vsync_metric_warp_enabled;

    vsyncarch_get_metrics(&vsync_metric_cpu_percent, &vsync_metric_emulated_fps, &vsync_metric_warp_enabled);
    
    sep = ui_pause_active() ? ('P' | 0x80) : vsync_metric_warp_enabled ? ('W' | 0x80) : '/';

    len = sprintf(&(statusbar_text[STATUSBAR_SPEED_POS]), "%3d%%%c%2dfps", (int)(vsync_metric_cpu_percent + 0.5), sep, (int)(vsync_metric_emulated_fps + 0.5));
    statusbar_text[STATUSBAR_SPEED_POS + len] = ' ';

    /* TODO: Only re-render if the string changed, like GTK */
    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

/* ----------------------------------------------------------------- */
/* uiapi.h */

/* Display a mesage without interrupting emulation */
void ui_display_statustext(const char *text, int fade_out)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: \"%s\", %i\n", __func__, text, fade_out);
#endif
}

/* Drive related UI. */

/* Build the drive status widget
   state        bitfield, one drive for each drive (dual drive has one bit!)
 */
void ui_enable_drive_status(ui_drive_enable_t state, int *drive_led_color)
{
    int drive_number;
    int drive_state = (int)state;
    size_t offset;  /* offset in status text */
    size_t size;    /* number of bytes to bleep out */

    offset = STATUSBAR_DRIVE_POS + 1; /* point to the T */

    for (drive_number = 0; drive_number < 4; ++drive_number) {
        if (drive_state & 1) {
            statusbar_drive_offset[drive_number][0] = (int)offset;
            ui_display_drive_led(drive_number, 0, 0, 0);
            ui_display_drive_track(drive_number, 0, 
                                   statusbar_drive_track[drive_number][0],
                                   statusbar_drive_side[drive_number][0]);
            if (drive_is_dualdrive_by_devnr(drive_number + 8)) {
                offset += (drive_number > 1) ? 6 : 5;
                statusbar_drive_offset[drive_number][1] = (int)offset;
                ui_display_drive_led(drive_number, 1, 0, 0);
                ui_display_drive_track(drive_number, 1, 
                                    statusbar_drive_track[drive_number][1],
                                    statusbar_drive_side[drive_number][1]);
            } else {
                statusbar_drive_offset[drive_number][1] = 0;
            }
            offset += (drive_number > 0) ? 6 : 5;
        }
        size = MAX_STATUSBAR_LEN - offset;
        memset(statusbar_text + offset, ' ', size);
        drive_state >>= 1;
    }

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

void ui_display_drive_track(unsigned int drive_number, 
                            unsigned int drive_base, 
                            unsigned int half_track_number,
                            unsigned int disk_side)
{
    unsigned int track_number = half_track_number / 2;
    unsigned int offset;

#ifdef SDL_DEBUG
    fprintf(stderr, "%s\n", __func__);
#endif
    /* printf("ui_display_drive_track drive_number:%d drive_base:%d half_track_number:%d\n",
           drive_number, drive_base, half_track_number); */

    /* FIXME: disk side not displayed yet */

    /* remember for when we need to refresh it */
    statusbar_drive_track[drive_number][drive_base] = half_track_number;
    statusbar_drive_side[drive_number][drive_base] = disk_side;

    offset = statusbar_drive_offset[drive_number][drive_base] + 1;
    statusbar_text[offset] = (track_number / 10) + '0';
    statusbar_text[offset + 1] = (track_number % 10) + '0';

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

/* The pwm value will vary between 0 and 1000.  */
void ui_display_drive_led(unsigned int drive_number, 
                          unsigned int drive_base, 
                          unsigned int led_pwm1, 
                          unsigned int led_pwm2)
{
    int high, low, trk;
    int offset;

#ifdef SDL_DEBUG
    fprintf(stderr, "%s: drive %i, led_pwm1 = %i, led_pwm2 = %u\n", __func__, drive_number, led_pwm1, led_pwm2);
#endif
    /* printf("ui_display_drive_led drive_number:%d drive_base:%d led_pwm1:%d led_pwm2:%d\n",
           drive_number, drive_base, led_pwm1, led_pwm2); */

    /* LED1 highlights the drive number, LED2 highlights the T */
    low = "8901"[drive_number] | ((led_pwm1 > 500) ? 0x80 : 0);
    high = '1' | ((led_pwm1 > 500) ? 0x80: 0);
    trk = 'T' | ((led_pwm2 > 500) ? 0x80: 0);
    
    offset = statusbar_drive_offset[drive_number][drive_base];

    if (drive_number < 2) {
        statusbar_text[offset - 1] = low;
        statusbar_text[offset] = trk;
    } else {
        statusbar_text[offset - 2] = high;
        statusbar_text[offset - 1] = low;
        statusbar_text[offset] = trk;
    }

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

void ui_display_drive_current_image(unsigned int init_number, unsigned int drive_number, const char *image)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s\n", __func__);
#endif
}

/* Tape related UI */

void ui_set_tape_status(int port, int tape_status)
{
    tape_enabled = tape_status;

    display_tape();
}

void ui_display_tape_motor_status(int port, int motor)
{
    tape_motor = motor;

    display_tape();
}

void ui_display_tape_control_status(int port, int control)
{
    tape_control = control;

    display_tape();
}

void ui_display_tape_counter(int port, int counter)
{
    if (tape_counter != counter) {
        display_tape();
    }

    tape_counter = counter;
}

void ui_display_tape_current_image(int port, const char *image)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %s\n", __func__, image);
#endif
}

/* Recording UI */
void ui_display_playback(int playback_status, char *version)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %i, \"%s\"\n", __func__, playback_status, version);
#endif
}

void ui_display_recording(int recording_status)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %i\n", __func__, recording_status);
#endif
}

void ui_display_event_time(unsigned int current, unsigned int total)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %i, %i\n", __func__, current, total);
#endif
}

/* Joystick UI */
void ui_display_joyport(uint16_t *joyport)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x\n", __func__, joyport[0], joyport[1], joyport[2], joyport[3], joyport[4], joyport[5], joyport[6], joyport[7], joyport[8], joyport[9]);
#endif
}

/* Volume UI */
void ui_display_volume(int vol)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %i\n", __func__, vol);
#endif
}

/* ----------------------------------------------------------------- */
/* resources */

static int statusbar_enabled;

#if 0
static int kbdstatusbar_enabled;
#endif


static int set_statusbar(int val, void *param)
{
    statusbar_enabled = val ? 1 : 0;

    if (statusbar_enabled) {
        uistatusbar_open();
    } else {
        uistatusbar_close();
    }

    return 0;
}

#if 0
static int set_kbdstatusbar(int val, void *param)
{
    kbdstatusbar_enabled = val ? 1 : 0;

    return 0;
}
#endif

static const resource_int_t resources_int[] = {
    { "SDLStatusbar", 0, RES_EVENT_NO, NULL,
      &statusbar_enabled, set_statusbar, NULL },
#if 0
    { "KbdStatusbar", 0, RES_EVENT_NO, NULL,
      &kbdstatusbar_enabled, set_kbdstatusbar, NULL },
#endif
    RESOURCE_INT_LIST_END
};

int uistatusbar_init_resources(void)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s\n", __func__);
#endif
    return resources_register_int(resources_int);
}


/* ----------------------------------------------------------------- */
/* uistatusbar.h */

int uistatusbar_state = 0;

void uistatusbar_open(void)
{
    uistatusbar_state = UISTATUSBAR_ACTIVE | UISTATUSBAR_REPAINT;
}

void uistatusbar_close(void)
{
    uistatusbar_state = UISTATUSBAR_REPAINT;
}

#define KBDSTATUSENTRYLEN   15

void uistatusbar_draw(void)
{
    int i;
    uint8_t c, color_f, color_b;
    unsigned int line, maxchars;
    menu_draw_t *limits = NULL;
    int kbd_status;
    char *text;
    size_t text_len;

    menufont = sdl_ui_get_menu_font();

    if (resources_get_int("KbdStatusbar", &kbd_status) < 0) {
        kbd_status = 0;
    }

    /* Update the cpu/fps each frame */
    display_speed();

    sdl_ui_init_draw_params();
    limits = sdl_ui_get_menu_param();

    color_f = limits->color_default_front;
    color_b = limits->color_default_back;
    pitch = limits->pitch;

    line = MIN(sdl_active_canvas->viewport->last_line, 
               sdl_active_canvas->geometry->last_displayed_line);

    draw_offset = (line - menufont->h + 1) * pitch
                  + sdl_active_canvas->geometry->extra_offscreen_border_left
                  + sdl_active_canvas->viewport->first_x;

    maxchars = pitch / menufont->w;

    if (kbd_status) {
        for (i = 0; i < maxchars; ++i) {
            c = kbdstatusbar_text[i];
            if (c == 0) {
                break;
            }

            if (((i / KBDSTATUSENTRYLEN) & 1) == 1) {
                uistatusbar_putchar(c, i, -1, color_b, color_f);
            } else {
                uistatusbar_putchar(c, i, -1, color_f, color_b);
            }
        }
    }

    if (machine_is_jammed()) {
        text = machine_jam_reason();
    } else {
        text = statusbar_text;
    }
    text_len = strlen(text);

    for (i = 0; i < maxchars; ++i) {
        c = i < text_len ? text[i] : ' ';

        if (c & 0x80) {
            uistatusbar_putchar((uint8_t)(c & 0x7f), i, 0, color_b, color_f);
        } else {
            uistatusbar_putchar(c, i, 0, color_f, color_b);
        }
    }
}

void ui_display_kbd_status(SDL_Event *e)
{
    char *p = &kbdstatusbar_text[KBDSTATUSENTRYLEN * 2];
    int kbd_status;

    if (machine_class == VICE_MACHINE_VSID) {
        return; /* vsid doesn't have a statusbar */
    }

    resources_get_int("KbdStatusbar", &kbd_status);

    if (kbd_status) {
        memmove(kbdstatusbar_text, &kbdstatusbar_text[KBDSTATUSENTRYLEN], 
                MAX_STATUSBAR_LEN - KBDSTATUSENTRYLEN);
        memset(p + KBDSTATUSENTRYLEN, ' ', MAX_STATUSBAR_LEN - (KBDSTATUSENTRYLEN * 3));
        /* The SDL1 and SDL2 ports have different types for the e->key.keysym.sym
         * and SDLKey arguments. We could cast the arguments to fix the -Wformat
         * warnings, but that might bite us in the arse in the future.
         * So I use conditional compiling to make the issue clear.  -- compyx
         */
#ifdef USE_SDLUI2
        sprintf(p, "%c%03d>%03d %c%04x    ",
#else
        sprintf(p, "%c%03u>%03u %c%04x    ",
#endif
                (e->type == SDL_KEYUP) ? 'U' : 'D',
                e->key.keysym.sym & 0xffff,
                SDL2x_to_SDL1x_Keys(e->key.keysym.sym),
                ((e->key.keysym.sym & 0xffff0000) == 0x40000000) ? 'M' : ((e->key.keysym.sym & 0xffff0000) != 0x00000000) ? 'E' : ' ',
                e->key.keysym.mod);
#ifdef USE_SDLUI2
        log_message(LOG_DEFAULT, "%s %03d>%03d %c%04x",
#else
        log_message(LOG_DEFAULT, "%s %03u>%03u %c%04x",
#endif
                (e->type == SDL_KEYUP) ? "release" : "press  ",
                e->key.keysym.sym & 0xffff,
                SDL2x_to_SDL1x_Keys(e->key.keysym.sym),
                ((e->key.keysym.sym & 0xffff0000) == 0x40000000) ? 'M' : ((e->key.keysym.sym & 0xffff0000) != 0x00000000) ? 'E' : ' ',
                e->key.keysym.mod);
    }
}


#ifdef ANDROID_COMPILE
void loader_set_statusbar(int val)
{
    set_statusbar(val, 0);
}
#endif




