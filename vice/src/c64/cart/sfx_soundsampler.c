/*
 * sfx_soundsampler.c - SFX Sound Sampler cartridge emulation.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c64export.h"
#include "cartio.h"
#include "cartridge.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine-video.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "sfx_soundsampler.h"
#include "sid.h"
#include "snapshot.h"
#include "sound.h"
#include "uiapi.h"
#include "translate.h"

/* Testing audio input using a file */
/* #define SFX_SAMPLE_TEST */

#ifdef USE_PORTAUDIO
#include <portaudio.h>

static int stream_started;
static PaStream *stream = NULL;

static unsigned int sound_sample_counter;
static unsigned int sound_sample_cycle;
static unsigned int sound_frames_per_sec;
static unsigned int sound_cycles_per_frame;
static unsigned int sound_samples_per_frame;

static WORD *stream_buffer = NULL;

static void sfx_soundsampler_start_sampling(void)
{
    PaStreamParameters inputParameters;
    PaError err = paNoError;

    stream_started = 0;

    err = Pa_Initialize();

    if (err == paNoError ) {
        inputParameters.device = Pa_GetDefaultInputDevice();
        if (inputParameters.device != paNoDevice) {
            inputParameters.channelCount = 1;
            inputParameters.sampleFormat = paInt16;
            inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency ;
            inputParameters.hostApiSpecificStreamInfo = NULL;
            sound_cycles_per_frame = machine_get_cycles_per_frame();
            sound_frames_per_sec = machine_get_cycles_per_second() / sound_cycles_per_frame;
            sound_samples_per_frame = 44100 / sound_frames_per_sec;
            err = Pa_OpenStream(&stream, &inputParameters, NULL, 44100, sound_samples_per_frame, paClipOff, NULL, NULL);
            if (err == paNoError) {
                err = Pa_StartStream(stream);
                if (err == paNoError) {
                    stream_started = 1;
                    stream_buffer = lib_malloc(sound_samples_per_frame * 2);
                    memset(stream_buffer, 0, sound_samples_per_frame * 2);
                    sound_sample_cycle = maincpu_clk;
                    sound_sample_counter = 0;
                } else {
                    log_warning(LOG_DEFAULT, "Could not start stream");
                }
            } else {
                log_warning(LOG_DEFAULT, "Could not open stream");
            }
        } else {
            log_warning(LOG_DEFAULT, "Could not find a default input device");
        }
    } else {
        log_warning(LOG_DEFAULT, "Could not init portaudio");
    }
}

static void sfx_soundsampler_stop_sampling(void)
{
    Pa_AbortStream(stream);
    Pa_CloseStream(stream);
    stream = NULL;
    if (stream_buffer) {
        lib_free(stream_buffer);
        stream_buffer = NULL;
    }
    Pa_Terminate();
}

static BYTE sfx_soundsampler_get_sample(BYTE sample)
{
    int cycle_diff;
    int frame_diff;
    int sample_diff;
    int new_cycle_diff;
    BYTE retval;

    if (!stream_buffer) {
        return 0x80;
    }
    cycle_diff = maincpu_clk - sound_sample_cycle;
    frame_diff = cycle_diff / sound_cycles_per_frame;
    if (frame_diff) {
        sound_sample_counter += frame_diff * sound_samples_per_frame;
        cycle_diff -= frame_diff * sound_cycles_per_frame;
    }
    sample_diff = cycle_diff * sound_samples_per_frame / sound_cycles_per_frame;
    new_cycle_diff = sample_diff * sound_cycles_per_frame / sound_samples_per_frame;

    sound_sample_counter += sample_diff;
    while (sound_sample_counter >= sound_samples_per_frame) {
        sound_sample_counter -= sound_samples_per_frame;
        if (Pa_GetStreamReadAvailable(stream) >= sound_samples_per_frame) {
            Pa_ReadStream(stream, stream_buffer, sound_samples_per_frame);
        } else {
            return sample;
        }
    }
    sound_sample_cycle += new_cycle_diff;
    retval = (BYTE)((stream_buffer[sound_sample_counter] >> 8) + 0x80);
    if (retval - sample > 128 || sample - retval > 128) {
        log_warning(LOG_DEFAULT, "sample difference too big : new: %2X, old: %2X", retval, sample);
    }
    return retval;
}
#endif

#ifdef SFX_SAMPLE_TEST
#define SFX_SAMPLE_NAME "inputsound.raw"

static DWORD sample_size = 0;
static int sound_sampling_started = 0;
static unsigned int sound_sample_counter;
static unsigned int sound_sample_cycle;
static unsigned int sound_frames_per_sec;
static unsigned int sound_cycles_per_frame;
static unsigned int sound_samples_per_frame;

static BYTE *sample_buffer = NULL;

static void sfx_soundsampler_load_sample(void)
{
    FILE *sample_file = NULL;

    sample_file = fopen(SFX_SAMPLE_NAME, "rb");
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
    }
}

static void sfx_soundsampler_free_sample(void)
{
    if (sample_buffer) {
        lib_free(sample_buffer);
        sample_buffer = NULL;
    }
}

static BYTE sfx_soundsampler_get_sample(void)
{
    int cycle_diff;
    int frame_diff;
    int sample_diff;
    int new_cycle_diff;

    if (!sample_buffer) {
        return 0;
    }
    if (!sound_sampling_started) {
        sound_sampling_started = 1;
        sound_sample_counter = 0;
        sound_sample_cycle = maincpu_clk;
        return sample_buffer[0];
    }
    cycle_diff = maincpu_clk - sound_sample_cycle;
    frame_diff = cycle_diff / sound_cycles_per_frame;
    if (frame_diff) {
        sound_sample_counter += frame_diff * sound_samples_per_frame;
        cycle_diff -= frame_diff * sound_cycles_per_frame;
    }
    sample_diff = cycle_diff * sound_samples_per_frame / sound_cycles_per_frame;
    new_cycle_diff = sample_diff * sound_cycles_per_frame / sound_samples_per_frame;

    sound_sample_counter += sample_diff;
    if (sound_sample_counter > sample_size) {
        sound_sample_counter -= sample_size;
    }
    sound_sample_cycle += new_cycle_diff;
    return sample_buffer[sound_sample_counter];
}
#endif

/* ------------------------------------------------------------------------- */

static BYTE current_sample = 0;

/* ------------------------------------------------------------------------- */

/* some prototypes are needed */
static void sfx_soundsampler_sound_store(WORD addr, BYTE value);
static BYTE sfx_soundsampler_sample_read(WORD addr);
static void sfx_soundsampler_latch_sample(WORD addr, BYTE value);

static io_source_t sfx_soundsampler_io1_device = {
    CARTRIDGE_NAME_SFX_SOUND_SAMPLER,
    IO_DETACH_RESOURCE,
    "SFXSoundSampler",
    0xde00, 0xdeff, 0x01,
    0,
    sfx_soundsampler_latch_sample,
    NULL,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    CARTRIDGE_SFX_SOUND_SAMPLER,
    0,
    0
};

static io_source_t sfx_soundsampler_io2_device = {
    CARTRIDGE_NAME_SFX_SOUND_SAMPLER,
    IO_DETACH_RESOURCE,
    "SFXSoundSampler",
    0xdf00, 0xdfff, 0x01,
    1,
    sfx_soundsampler_sound_store,
    sfx_soundsampler_sample_read,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    CARTRIDGE_SFX_SOUND_SAMPLER,
    0,
    0
};

static io_source_list_t *sfx_soundsampler_io1_list_item = NULL;
static io_source_list_t *sfx_soundsampler_io2_list_item = NULL;

static const c64export_resource_t export_res = {
    CARTRIDGE_NAME_SFX_SOUND_SAMPLER, 0, 0, &sfx_soundsampler_io1_device, &sfx_soundsampler_io2_device, CARTRIDGE_SFX_SOUND_SAMPLER
};

/* ------------------------------------------------------------------------- */

/* Some prototypes are needed */
static int sfx_soundsampler_sound_machine_init(sound_t *psid, int speed, int cycles_per_sec);
static int sfx_soundsampler_sound_machine_calculate_samples(sound_t **psid, SWORD *pbuf, int nr, int sound_output_channels, int sound_chip_channels, int *delta_t);
static void sfx_soundsampler_sound_machine_store(sound_t *psid, WORD addr, BYTE val);
static BYTE sfx_soundsampler_sound_machine_read(sound_t *psid, WORD addr);
static void sfx_soundsampler_sound_reset(sound_t *psid, CLOCK cpu_clk);

static int sfx_soundsampler_sound_machine_cycle_based(void)
{
    return 0;
}

static int sfx_soundsampler_sound_machine_channels(void)
{
    return 1;
}

static sound_chip_t sfx_soundsampler_sound_chip = {
    NULL, /* no open */
    sfx_soundsampler_sound_machine_init,
    NULL, /* no close */
    sfx_soundsampler_sound_machine_calculate_samples,
    sfx_soundsampler_sound_machine_store,
    sfx_soundsampler_sound_machine_read,
    sfx_soundsampler_sound_reset,
    sfx_soundsampler_sound_machine_cycle_based,
    sfx_soundsampler_sound_machine_channels,
    0 /* chip enabled */
};

static WORD sfx_soundsampler_sound_chip_offset = 0;
static sound_dac_t sfx_soundsampler_dac;

void sfx_soundsampler_sound_chip_init(void)
{
    sfx_soundsampler_sound_chip_offset = sound_chip_register(&sfx_soundsampler_sound_chip);
}

/* ------------------------------------------------------------------------- */

static int sfx_soundsampler_io_swap = 0;

int sfx_soundsampler_cart_enabled(void)
{
    return sfx_soundsampler_sound_chip.chip_enabled;
}

static int set_sfx_soundsampler_enabled(int value, void *param)
{
    int val = value ? 1 : 0;

    if (sfx_soundsampler_sound_chip.chip_enabled != val) {
        if (val) {
            if (c64export_add(&export_res) < 0) {
                return -1;
            }
            if (machine_class == VICE_MACHINE_VIC20) {
                if (sfx_soundsampler_io_swap) {
                    sfx_soundsampler_io1_device.start_address = 0x9800;
                    sfx_soundsampler_io1_device.end_address = 0x9bff;
                    sfx_soundsampler_io2_device.start_address = 0x9c00;
                    sfx_soundsampler_io2_device.end_address = 0x9fff;
                } else {
                    sfx_soundsampler_io1_device.start_address = 0x9c00;
                    sfx_soundsampler_io1_device.end_address = 0x9fff;
                    sfx_soundsampler_io2_device.start_address = 0x9800;
                    sfx_soundsampler_io2_device.end_address = 0x9bff;
                }
            }
            sfx_soundsampler_io1_list_item = io_source_register(&sfx_soundsampler_io1_device);
            sfx_soundsampler_io2_list_item = io_source_register(&sfx_soundsampler_io2_device);
            sfx_soundsampler_sound_chip.chip_enabled = 1;
#ifdef USE_PORTAUDIO
            sfx_soundsampler_start_sampling();
#endif
#ifdef SFX_SAMPLE_TEST
            sfx_soundsampler_load_sample();
#endif
        } else {
            c64export_remove(&export_res);
            io_source_unregister(sfx_soundsampler_io1_list_item);
            io_source_unregister(sfx_soundsampler_io2_list_item);
            sfx_soundsampler_io1_list_item = NULL;
            sfx_soundsampler_io2_list_item = NULL;
            sfx_soundsampler_sound_chip.chip_enabled = 0;
#ifdef USE_PORTAUDIO
            sfx_soundsampler_stop_sampling();
#endif
#ifdef SFX_SAMPLE_TEST
            sfx_soundsampler_free_sample();
#endif
        }
    }
    return 0;
}

static int set_sfx_soundsampler_io_swap(int value, void *param)
{
    int val = value ? 1 : 0;

    if (val == sfx_soundsampler_io_swap) {
        return 0;
    }

    if (sfx_soundsampler_sound_chip.chip_enabled) {
        set_sfx_soundsampler_enabled(0, NULL);
        sfx_soundsampler_io_swap = val;
        set_sfx_soundsampler_enabled(1, NULL);
    } else {
        sfx_soundsampler_io_swap = val;
    }
    return 0;
}

void sfx_soundsampler_reset(void)
{
    /* TODO: do nothing ? */
}

int sfx_soundsampler_enable(void)
{
    return resources_set_int("SFXSoundSampler", 1);
}

void sfx_soundsampler_detach(void)
{
    resources_set_int("SFXSoundSampler", 0);
}

/* ------------------------------------------------------------------------- */

static const resource_int_t resources_int[] = {
    { "SFXSoundSampler", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &sfx_soundsampler_sound_chip.chip_enabled, set_sfx_soundsampler_enabled, NULL },
    { NULL }
};

static const resource_int_t resources_mascuerade_int[] = {
    { "SFXSoundSamplerIOSwap", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &sfx_soundsampler_io_swap, set_sfx_soundsampler_io_swap, NULL },
    { NULL }
};

int sfx_soundsampler_resources_init(void)
{
    if (machine_class == VICE_MACHINE_VIC20) {
        if (resources_register_int(resources_mascuerade_int) < 0) {
            return -1;
        }
    }
    return resources_register_int(resources_int);
}

void sfx_soundsampler_resources_shutdown(void)
{
}

static const cmdline_option_t cmdline_options[] =
{
    { "-sfxss", SET_RESOURCE, 0,
      NULL, NULL, "SFXSoundSampler", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_SFX_SS,
      NULL, NULL },
    { "+sfxss", SET_RESOURCE, 0,
      NULL, NULL, "SFXSoundSampler", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_SFX_SS,
      NULL, NULL },
    { NULL }
};

static const cmdline_option_t cmdline_mascuerade_options[] =
{
    { "-sfxssioswap", SET_RESOURCE, 0,
      NULL, NULL, "SFXSoundSamplerIOSwap", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_MAP_CART_IO_2,
      NULL, NULL },
    { "+sfxssioswap", SET_RESOURCE, 0,
      NULL, NULL, "SFXSoundSamplerIOSwap", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_MAP_CART_IO_3,
      NULL, NULL },
    { NULL }
};

int sfx_soundsampler_cmdline_options_init(void)
{
    if (machine_class == VICE_MACHINE_VIC20) {
        if (cmdline_register_options(cmdline_mascuerade_options) < 0) {
            return -1;
        }
    }
    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

static void sfx_soundsampler_latch_sample(WORD addr, BYTE value)
{
#ifdef USE_PORTAUDIO
    current_sample = sfx_soundsampler_get_sample(current_sample);
#endif
#ifdef SFX_SAMPLE_TEST
    current_sample = sfx_soundsampler_get_sample();
#endif
}

static BYTE sfx_soundsampler_sample_read(WORD addr)
{
    return current_sample;
}

/* ---------------------------------------------------------------------*/

static BYTE sfx_soundsampler_sound_data;

static void sfx_soundsampler_sound_store(WORD addr, BYTE value)
{
    sfx_soundsampler_sound_data = value;
    sound_store(sfx_soundsampler_sound_chip_offset, value, 0);
}

struct sfx_soundsampler_sound_s {
    BYTE voice0;
};

static struct sfx_soundsampler_sound_s snd;

static int sfx_soundsampler_sound_machine_calculate_samples(sound_t **psid, SWORD *pbuf, int nr, int soc, int scc, int *delta_t)
{
    return sound_dac_calculate_samples(&sfx_soundsampler_dac, pbuf, (int)snd.voice0 * 128, nr, soc, (soc > 1) ? 3 : 1);
}

static int sfx_soundsampler_sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    sound_dac_init(&sfx_soundsampler_dac, speed);
    snd.voice0 = 0;

    return 1;
}

static void sfx_soundsampler_sound_machine_store(sound_t *psid, WORD addr, BYTE val)
{
    snd.voice0 = val;
}

static BYTE sfx_soundsampler_sound_machine_read(sound_t *psid, WORD addr)
{
    return sfx_soundsampler_sound_data;
}

static void sfx_soundsampler_sound_reset(sound_t *psid, CLOCK cpu_clk)
{
    snd.voice0 = 0;
    sfx_soundsampler_sound_data = 0;
}

/* ---------------------------------------------------------------------*/
/*    snapshot support functions                                             */

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "CARTSFXSS"

int sfx_soundsampler_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME, CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0 || (SMW_B(m, (BYTE)sfx_soundsampler_sound_data) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int sfx_soundsampler_snapshot_read_module(snapshot_t *s)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;

    m = snapshot_module_open(s, SNAP_MODULE_NAME, &vmajor, &vminor);
    if (m == NULL) {
        return -1;
    }

    if ((vmajor != CART_DUMP_VER_MAJOR) || (vminor != CART_DUMP_VER_MINOR)) {
        snapshot_module_close(m);
        return -1;
    }

    if (0 || (SMR_B(m, &sfx_soundsampler_sound_data) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    if (!sfx_soundsampler_sound_chip.chip_enabled) {
        set_sfx_soundsampler_enabled(1, NULL);
    }
    sound_store(sfx_soundsampler_sound_chip_offset, sfx_soundsampler_sound_data, 0);

    snapshot_module_close(m);
    return 0;
}
