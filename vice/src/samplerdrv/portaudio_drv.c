/*
 * portaudio_drv.c - PortAudio audio input driver.
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

#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "portaudio_drv.h"

#ifdef USE_PORTAUDIO
#include <portaudio.h>

static int stream_started = 0;
static PaStream *stream = NULL;

static unsigned int sound_sample_counter;
static unsigned int sound_sample_cycle;
static unsigned int sound_frames_per_sec;
static unsigned int sound_cycles_per_frame;
static unsigned int sound_samples_per_frame;
static unsigned int same_sample = 0;

static WORD *stream_buffer = NULL;
static BYTE old_sample = 0x80;

static void portaudio_start_stream(void)
{
    PaStreamParameters inputParameters;
    PaError err = paNoError;

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
                memset(stream_buffer, 0x80, sound_samples_per_frame * 2);
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
}

static void portaudio_stop_stream(void)
{
    Pa_AbortStream(stream);
    Pa_CloseStream(stream);
    stream = NULL;
    if (stream_buffer) {
        lib_free(stream_buffer);
        stream_buffer = NULL;
    }
    stream_started = 0;
}

void portaudio_start_sampling(void)
{
    PaError err = paNoError;

    if (stream_started) {
        log_warning(LOG_DEFAULT, "Attempted to start portaudio twice");
    } else {

        err = Pa_Initialize();

        if (err == paNoError ) {
            portaudio_start_stream();
        } else {
            log_warning(LOG_DEFAULT, "Could not init portaudio");
        }
    }
}

void portaudio_stop_sampling(void)
{
    portaudio_stop_stream();
    Pa_Terminate();
}

BYTE portaudio_get_sample(void)
{
    int cycle_diff;
    int frame_diff;
    int sample_diff;
    int new_cycle_diff;

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
    sound_sample_cycle += new_cycle_diff;
    while (sound_sample_counter >= sound_samples_per_frame) {
        sound_sample_counter -= sound_samples_per_frame;
        if (Pa_GetStreamReadAvailable(stream) >= sound_samples_per_frame) {
            Pa_ReadStream(stream, stream_buffer, sound_samples_per_frame);
            same_sample = 0;
        } else {
            ++same_sample;
            if (same_sample >= sound_samples_per_frame) {
                same_sample = 0;
                portaudio_stop_stream();
                portaudio_start_stream();
                log_warning(LOG_DEFAULT, "Had to restart the stream");
            }
            return old_sample;
        }
    }
    old_sample = (BYTE)((stream_buffer[sound_sample_counter] >> 8) + 0x80);
    return old_sample;
}
#endif
