/*
 * file_drv.c - File based audio input driver.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "types.h"

#include "file_drv.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "sampler.h"

#define SAMPLE_NAME "inputsound.raw"

static unsigned sample_size = 0;
static int sound_sampling_started = 0;

static unsigned int sound_sample_frame_start;
static unsigned int old_frame;
static unsigned int sound_frames_per_sec;
static unsigned int sound_cycles_per_frame;
static unsigned int sound_samples_per_frame;

static BYTE *sample_buffer = NULL;

/* For now channels is ignored */
static void file_load_sample(int channels)
{
    FILE *sample_file = NULL;

    sample_file = fopen(SAMPLE_NAME, "rb");
    if (sample_file) {
        fseek(sample_file, 0, SEEK_END);
        sample_size = ftell(sample_file);
        fseek(sample_file, 0, SEEK_SET);
        sample_buffer = lib_malloc(sample_size);
        fread(sample_buffer, 1, sample_size, sample_file);
        fclose(sample_file);
        sound_sampling_started = 0;
        sound_cycles_per_frame = machine_get_cycles_per_frame();
        sound_frames_per_sec = machine_get_cycles_per_second() / sound_cycles_per_frame;
        sound_samples_per_frame = 44100 / sound_frames_per_sec;
        log_warning(LOG_DEFAULT, "Loaded sample, size: %d, cycles per frame: %d, frames per sec: %d, samples per frame: %d", sample_size, sound_cycles_per_frame, sound_frames_per_sec, sound_samples_per_frame);
    } else {
        log_warning(LOG_DEFAULT, "Cannot open sample : %s", SAMPLE_NAME);
    }
}

static void file_free_sample(void)
{
    if (sample_buffer) {
        lib_free(sample_buffer);
        sample_buffer = NULL;
    }
}

/* For now channel is ignored */
static BYTE file_get_sample(int channel)
{
    unsigned int current_frame = 0;
    unsigned int current_cycle = 0;
    unsigned int frame_diff = 0;
    unsigned int frame_sample = 0;

    if (!sample_buffer) {
        return 0x80;
    }
    if (!sound_sampling_started) {
        sound_sampling_started = 1;
        old_frame = maincpu_clk / sound_cycles_per_frame;
        return sample_buffer[0];
    }
    current_frame = maincpu_clk / sound_cycles_per_frame;
    current_cycle = maincpu_clk % sound_cycles_per_frame;

    if (current_frame > old_frame) {
        frame_diff = current_frame - old_frame;
        while (frame_diff) {
            --frame_diff;
            ++old_frame;
            sound_sample_frame_start += sound_samples_per_frame;
            if (sound_sample_frame_start >= sample_size) {
                sound_sample_frame_start -= sample_size;
            }
        }
    }
    frame_sample = current_cycle * sound_samples_per_frame / sound_cycles_per_frame;

    return sample_buffer[(frame_sample + sound_sample_frame_start) % sample_size];
}

static sampler_device_t file_device =
{
    "file",
    file_load_sample,
    file_free_sample,
    file_get_sample
};

void fileaudio_init(void)
{
    sampler_device_register(&file_device);
}
