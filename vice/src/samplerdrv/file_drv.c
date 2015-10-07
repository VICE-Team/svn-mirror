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

/* WAV files tested and working:
 - 22050 Hz, 8bit PCM, stereo
 - 11025 Hz, 16bit PCM, mono
 - 22050 Hz, 16bit PCM, mono
 - 23456 Hz, 16bit PCM, mono
 - 44100 Hz, 16bit PCM, mono
 - 11025 Hz, 16bit PCM, stereo
 - 22050 Hz, 16bit PCM, stereo
 - 23456 Hz, 16bit PCM, stereo
 - 44100 Hz, 16bit PCM, stereo
 - 22050 Hz, 24bit PCM, stereo
 - 22050 Hz, 32bit PCM, stereo
 - 22050 Hz, 32bit float, stereo
 - 22050 Hz, 64bit float, stereo
 - 44010 Hz, a-law, stereo
 - 44010 Hz, u-law, stereo
 */

#include "vice.h"

#include <string.h> /* for memcpy */

#include "types.h"

#include "file_drv.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "sampler.h"

/* In the future the filename can be set from either commandline or gui */
#define SAMPLE_NAME "inputsound"

#define AUDIO_TYPE_UNKNOWN -1
#define AUDIO_TYPE_PCM      0
#define AUDIO_TYPE_FLOAT    1
#define AUDIO_TYPE_ALAW     2
#define AUDIO_TYPE_ULAW     3

static unsigned sample_size = 0;
static int sound_sampling_started = 0;

static unsigned int sound_sample_frame_start;
static unsigned int old_frame;
static unsigned int sound_frames_per_sec;
static unsigned int sound_cycles_per_frame;
static unsigned int sound_samples_per_frame;

static int current_channels = 0;

static BYTE *file_buffer = NULL;
static unsigned int file_pointer = 0;
static unsigned int file_size = 0;

static int sound_audio_type = 0;
static unsigned int sound_audio_channels = 0;
static unsigned int sound_audio_rate = 0;
static unsigned int sound_audio_bits = 0;

/* ---------------------------------------------------------------------- */

static int wav_fmt_extension_bytes = 0;

static BYTE *sample_buffer1 = NULL;
static BYTE *sample_buffer2 = NULL;

static SWORD decode_ulaw(BYTE sample)
{
    SWORD t;

    sample = ~sample;

    t = ((sample & 0xf) << 3) + 0x84;
    t <<= ((unsigned)sample & 0x70) >> 4;

    return ((sample & 0x80) ? (0x84 - t) : (t - 0x84));
}

static SWORD decode_alaw(BYTE sample)
{
    SWORD t;
    SWORD seg;

    sample ^= 0x55;

    t = (sample & 0xf) << 4;
    seg = ((unsigned)sample & 0x70) >> 4;
    switch (seg) {
        case 0:
            t += 8;
            break;
        case 1:
            t += 0x108;
            break;
        default:
            t += 0x108;
            t <<= seg - 1;
    }
    return ((sample & 0x80) ? t : -t);
}

static int convert_alaw_buffer(int size, int channels)
{
    unsigned int frame_size = sound_audio_bits * sound_audio_channels / 8;
    unsigned int i;
    signed char sample;

    sample_size = size / frame_size;

    sample_buffer1 = lib_malloc(sample_size);
    if (channels == SAMPLER_OPEN_STEREO) {
        if (sound_audio_channels == 2) {
            sample_buffer2 = lib_malloc(sample_size);
        } else {
            sample_buffer2 = sample_buffer1;
        }
    }

    for (i = 0; i < sample_size; ++i) {
        sample = file_buffer[file_pointer + (i * frame_size)];
        sample_buffer1[i] = (BYTE)(decode_alaw(sample) >> 8) + 0x80;
        if (sound_audio_channels == 2 && channels == SAMPLER_OPEN_STEREO) {
            sample = file_buffer[file_pointer + (i * frame_size) + 1];
            sample_buffer2[i] = (BYTE)(decode_alaw(sample) >> 4) + 0x80;
        }
    }
    lib_free(file_buffer);
    file_buffer = NULL;
    return 0;
}

static int convert_ulaw_buffer(int size, int channels)
{
    unsigned int frame_size = sound_audio_bits * sound_audio_channels / 8;
    unsigned int i;
    signed char sample;

    sample_size = size / frame_size;

    sample_buffer1 = lib_malloc(sample_size);
    if (channels == SAMPLER_OPEN_STEREO) {
        if (sound_audio_channels == 2) {
            sample_buffer2 = lib_malloc(sample_size);
        } else {
            sample_buffer2 = sample_buffer1;
        }
    }

    for (i = 0; i < sample_size; ++i) {
        sample = file_buffer[file_pointer + (i * frame_size)];
        sample_buffer1[i] = (BYTE)(decode_ulaw(sample) >> 8) + 0x80;
        if (sound_audio_channels == 2 && channels == SAMPLER_OPEN_STEREO) {
            sample = file_buffer[file_pointer + (i * frame_size) + 1];
            sample_buffer2[i] = (BYTE)(decode_ulaw(sample) >> 3) + 0x80;
        }
    }
    lib_free(file_buffer);
    file_buffer = NULL;
    return 0;
}

static int convert_pcm_buffer(int size, int channels)
{
    unsigned int frame_size = sound_audio_bits * sound_audio_channels / 8;
    unsigned int i;
    sample_size = size / frame_size;

    sample_buffer1 = lib_malloc(sample_size);
    if (channels == SAMPLER_OPEN_STEREO) {
        if (sound_audio_channels == 2) {
            sample_buffer2 = lib_malloc(sample_size);
        } else {
            sample_buffer2 = sample_buffer1;
        }
    }

    for (i = 0; i < sample_size; ++i) {
        sample_buffer1[i] = file_buffer[file_pointer + (i * frame_size) + (sound_audio_bits / 8) - 1];
        if (sound_audio_bits != 8) {
            sample_buffer1[i] += 0x80;
        }
        if (sound_audio_channels == 2 && channels == SAMPLER_OPEN_STEREO) {
            sample_buffer2[i] = file_buffer[file_pointer + (i * frame_size) + (frame_size / 2) + (sound_audio_bits / 8) - 1];
            if (sound_audio_bits != 8) {
                sample_buffer2[i] += 0x80;
            }
        }
    }
    lib_free(file_buffer);
    file_buffer = NULL;
    return 0;
}

/* FIXME: endianess */
static int convert_float_buffer(int size, int channels)
{
    unsigned int frame_size = sound_audio_bits * sound_audio_channels / 8;
    unsigned int i;
    unsigned char c[sizeof(float)];
    float f;
    SDWORD sample;

    sample_size = size / frame_size;

    sample_buffer1 = lib_malloc(sample_size);
    if (channels == SAMPLER_OPEN_STEREO) {
        if (sound_audio_channels == 2) {
            sample_buffer2 = lib_malloc(sample_size);
        } else {
            sample_buffer2 = sample_buffer1;
        }
    }

    for (i = 0; i < sample_size; ++i) {
        c[0] = file_buffer[file_pointer + (i * frame_size)];
        c[1] = file_buffer[file_pointer + (i * frame_size) + 1];
        c[2] = file_buffer[file_pointer + (i * frame_size) + 2];
        c[3] = file_buffer[file_pointer + (i * frame_size) + 3];
        memcpy(&f, c, sizeof(float));
        f *= 0x7fffffff;
        sample = (SDWORD)f;
        sample_buffer1[i] = (sample >> 24) + 0x80;
        if (sound_audio_channels == 2 && channels == SAMPLER_OPEN_STEREO) {
            c[0] = file_buffer[file_pointer + (i * frame_size) + 4];
            c[1] = file_buffer[file_pointer + (i * frame_size) + 5];
            c[2] = file_buffer[file_pointer + (i * frame_size) + 6];
            c[3] = file_buffer[file_pointer + (i * frame_size) + 7];
            memcpy(&f, c, sizeof(float));
            f *= 0x7fffffff;
            sample = (SDWORD)f;
            sample_buffer2[i] = (sample >> 24) + 0x80;
        }
    }
    lib_free(file_buffer);
    file_buffer = NULL;
    return 0;
}

/* FIXME: endianess */
static int convert_double_buffer(int size, int channels)
{
    unsigned int frame_size = sound_audio_bits * sound_audio_channels / 8;
    unsigned int i;
    unsigned char c[sizeof(double)];
    double f;
    SDWORD sample;

    sample_size = size / frame_size;

    sample_buffer1 = lib_malloc(sample_size);
    if (channels == SAMPLER_OPEN_STEREO) {
        if (sound_audio_channels == 2) {
            sample_buffer2 = lib_malloc(sample_size);
        } else {
            sample_buffer2 = sample_buffer1;
        }
    }

    for (i = 0; i < sample_size; ++i) {
        c[0] = file_buffer[file_pointer + (i * frame_size)];
        c[1] = file_buffer[file_pointer + (i * frame_size) + 1];
        c[2] = file_buffer[file_pointer + (i * frame_size) + 2];
        c[3] = file_buffer[file_pointer + (i * frame_size) + 3];
        c[4] = file_buffer[file_pointer + (i * frame_size) + 4];
        c[5] = file_buffer[file_pointer + (i * frame_size) + 5];
        c[6] = file_buffer[file_pointer + (i * frame_size) + 6];
        c[7] = file_buffer[file_pointer + (i * frame_size) + 7];

        memcpy(&f, c, sizeof(double));
        f *= 0x7fffffff;
        sample = (SDWORD)f;
        sample_buffer1[i] = (sample >> 24) + 0x80;
        if (sound_audio_channels == 2 && channels == SAMPLER_OPEN_STEREO) {
            c[0] = file_buffer[file_pointer + (i * frame_size) + 8];
            c[1] = file_buffer[file_pointer + (i * frame_size) + 9];
            c[2] = file_buffer[file_pointer + (i * frame_size) + 10];
            c[3] = file_buffer[file_pointer + (i * frame_size) + 11];
            c[4] = file_buffer[file_pointer + (i * frame_size) + 12];
            c[5] = file_buffer[file_pointer + (i * frame_size) + 13];
            c[6] = file_buffer[file_pointer + (i * frame_size) + 14];
            c[7] = file_buffer[file_pointer + (i * frame_size) + 15];
            memcpy(&f, c, sizeof(double));
            f *= 0x7fffffff;
            sample = (SDWORD)f;
            sample_buffer2[i] = (sample >> 24) + 0x80;
        }
    }
    lib_free(file_buffer);
    file_buffer = NULL;
    return 0;
}

static void check_and_skip_chunk(void)
{
    unsigned int size = 0;
    int skip_chunk = 0;

    /* if the current chunk is of type 'LIST' we need to skip it */
    if (file_buffer[file_pointer] == 0x4C && file_buffer[file_pointer + 1] == 0x49 && file_buffer[file_pointer + 2] == 0x53 && file_buffer[file_pointer + 3] == 0x54) {
        skip_chunk = 1;
        log_warning(LOG_DEFAULT, "LIST chunk found, skipping");
    }

    /* if the current chunk is of type 'PEAK' we need to skip it */
    if (file_buffer[file_pointer] == 0x50 && file_buffer[file_pointer + 1] == 0x45 && file_buffer[file_pointer + 2] == 0x41 && file_buffer[file_pointer + 3] == 0x4B) {
        skip_chunk = 1;
        log_warning(LOG_DEFAULT, "PEAK chunk found, skipping");
    }

    /* if the current chunk is of type 'fact' we need to skip it */
    if (file_buffer[file_pointer] == 0x66 && file_buffer[file_pointer + 1] == 0x61 && file_buffer[file_pointer + 2] == 0x63 && file_buffer[file_pointer + 3] == 0x74) {
        skip_chunk = 1;
        log_warning(LOG_DEFAULT, "fact chunk found, skipping");
    }

    if (skip_chunk) {
        file_pointer += 4;
        size = (file_buffer[file_pointer + 3] << 24) | (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
        file_pointer += size + 4;
    }
}

static int valid_wav_ftm_chunk_size(void)
{
    if (file_buffer[file_pointer] == 0x10 && file_buffer[file_pointer + 1] == 0 && file_buffer[file_pointer + 2] == 0 && file_buffer[file_pointer + 3] == 0) {
        wav_fmt_extension_bytes = 0;
        return 1;
    }
    if (file_buffer[file_pointer] == 0x12 && file_buffer[file_pointer + 1] == 0 && file_buffer[file_pointer + 2] == 0 && file_buffer[file_pointer + 3] == 0) {
        wav_fmt_extension_bytes = 2;
        return 1;
    }
    if (file_buffer[file_pointer] == 0x28 && file_buffer[file_pointer + 1] == 0 && file_buffer[file_pointer + 2] == 0 && file_buffer[file_pointer + 3] == 0) {
        wav_fmt_extension_bytes = 24;
        return 1;
    }
    return 0;
}

static int handle_wav_file(int channels)
{
    unsigned int size = 0;
    unsigned int bps = 0;

    if (file_size < 8) {
        log_warning(LOG_DEFAULT, "file size smaller than 8 bytes.");
        return -1;
    }

    /* sanity check header indicated size with loaded size */
    size = (file_buffer[7] << 24) | (file_buffer[6] << 16) | (file_buffer[5] << 8) | file_buffer[4];
    if (size != file_size - 8) {
        log_warning(LOG_DEFAULT, "header reported size not what was expected: header says: %d, filesize - 8 is %d.", size, file_size - 8);
        return -1;
    }

    /* next needs to be 'WAVE' */
    if (file_buffer[8] != 0x57 || file_buffer[9] != 0x41 || file_buffer[10] != 0x56 || file_buffer[11] != 0x45) {
        log_warning(LOG_DEFAULT, "WAVE not found at expected header position, found: %X %X %X %X.", file_buffer[8], file_buffer[9], file_buffer[10], file_buffer[11]);
        return -1;
    }

    file_pointer = 12;

    check_and_skip_chunk();

    /* current chunk now needs to be 'fmt ' */
    if (file_buffer[file_pointer] != 0x66 || file_buffer[file_pointer + 1] != 0x6D || file_buffer[file_pointer + 2] != 0x74 || file_buffer[file_pointer + 3] != 0x20) {
        log_warning(LOG_DEFAULT, "'fmt ' chunk not found in the expected header position, %X %X %X %X", file_buffer[file_pointer], file_buffer[file_pointer + 1], file_buffer[file_pointer + 2], file_buffer[file_pointer + 3]);
        return -1;
    }
    file_pointer += 4;

    /* chunk size needs to be 0x10 */
    if (!valid_wav_ftm_chunk_size()) {
        log_warning(LOG_DEFAULT, "unexpected chunk size %2X%2X%2X%2X", file_buffer[file_pointer + 3], file_buffer[file_pointer + 2], file_buffer[file_pointer + 1], file_buffer[file_pointer]);
        return -1;
    }
    file_pointer += 4;

    /* get the audio format 1: PCM (8/16/24/32bit), 3: float (32/64bit), 6: Alaw, 7: Ulaw */
    if (file_buffer[file_pointer] == 1 && file_buffer[file_pointer + 1] == 0) {
        sound_audio_type = AUDIO_TYPE_PCM;
    } else if (file_buffer[file_pointer] == 3 && file_buffer[file_pointer + 1] == 0) {
        sound_audio_type = AUDIO_TYPE_FLOAT;
    } else if (file_buffer[file_pointer] == 6 && file_buffer[file_pointer + 1] == 0) {
        sound_audio_type = AUDIO_TYPE_ALAW;
    } else if (file_buffer[file_pointer] == 7 && file_buffer[file_pointer + 1] == 0) {
        sound_audio_type = AUDIO_TYPE_ULAW;
    } else {
        log_warning(LOG_DEFAULT, "unexpected audio format : %2X%2X", file_buffer[file_pointer + 1], file_buffer[file_pointer]);
        return -1;
    }
    file_pointer += 2;

    /* channels used in the file */
    sound_audio_channels = (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (sound_audio_channels == 0 || sound_audio_channels > 2) {
        log_warning(LOG_DEFAULT, "unexpected amount of audio channels : %d", sound_audio_channels);
        return -1;
    }
    file_pointer +=2;

    /* sample rate used in file */
    sound_audio_rate = (file_buffer[file_pointer + 3] << 24) | (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (sound_audio_rate == 0) {
        log_warning(LOG_DEFAULT, "audio rate is 0");
        return -1;
    }
    file_pointer += 4;

    /* get 1st instance of bits per sample */
    bps = (file_buffer[file_pointer + 3] << 24) | (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    sound_audio_bits = (bps / (sound_audio_rate * sound_audio_channels)) * 8;
    file_pointer += 4;

    /* get 2nd instance of bits per sample */
    bps = (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    bps = bps * 8 / sound_audio_channels;
    if (bps != sound_audio_bits) {
        log_warning(LOG_DEFAULT, "1st instance of bps does not match second instance: %d %d", sound_audio_bits, bps);
        return -1;
    }
    file_pointer += 2;

    /* get real instance of bits per sample */
    bps = (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (bps != sound_audio_bits) {
        log_warning(LOG_DEFAULT, "1st instance of bps does not match real instance: %d %d", sound_audio_bits, bps);
        return -1;
    }
    file_pointer += 2;

    if (wav_fmt_extension_bytes) {
        file_pointer += wav_fmt_extension_bytes;
    }

    check_and_skip_chunk();
    check_and_skip_chunk();
    check_and_skip_chunk();

    /* current chunk now needs to be 'data' */
    if (file_buffer[file_pointer] != 0x64 || file_buffer[file_pointer + 1] != 0x61 || file_buffer[file_pointer + 2] != 0x74 || file_buffer[file_pointer + 3] != 0x61) {
        log_warning(LOG_DEFAULT, "data chunk not found at expected header position: %X%X%X%X", file_buffer[file_pointer], file_buffer[file_pointer + 1], file_buffer[file_pointer + 2], file_buffer[file_pointer + 3]);
        return -1;
    }
    file_pointer += 4;

    /* get remaining size */
    size = (file_buffer[file_pointer + 3] << 24) | (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (size != file_size - (file_pointer + 4)) {
        log_warning(LOG_DEFAULT, "data chunk size does not match remaining file size: %d %d", size, file_size - (file_pointer + 4));
        return -1;
    }
    file_pointer += 4;

    switch (sound_audio_type) {
        case AUDIO_TYPE_PCM:
            return convert_pcm_buffer(size, channels);
        case AUDIO_TYPE_FLOAT:
            switch (sound_audio_bits) {
                case 32:
                    return convert_float_buffer(size, channels);
                case 64:
                    return convert_double_buffer(size, channels);
                default:
                    log_warning(LOG_DEFAULT, "Unhandled float format : %d", sound_audio_bits);
                    return -1;
            }
        case AUDIO_TYPE_ALAW:
            return convert_alaw_buffer(size, channels);
        case AUDIO_TYPE_ULAW:
            return convert_ulaw_buffer(size, channels);
        default:
            log_warning(LOG_DEFAULT, "unhandled audio type");
            return -1;
    }
    return -1;
}

static int is_wav_file(void)
{
    if (file_size < 4) {
        return 0;
    }

    /* Check for wav header signature */
    if (file_buffer[0] == 0x52 && file_buffer[1] == 0x49 && file_buffer[2] == 0x46 && file_buffer[3] == 0x46) {
        return 1;
    }
    return 0;
}

/* ---------------------------------------------------------------------- */

static BYTE *voc_buffer1 = NULL;
static BYTE *voc_buffer2 = NULL;
static unsigned int voc_buffer_size;

static int voc_handle_sound_1(int channels)
{
    unsigned int size;
    BYTE fd;
    BYTE codec;

    if (file_pointer + 6 > file_size) {
        return -1;
    }

    ++file_pointer;

    size = (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (file_pointer + size > file_size) {
        return -1;
    }
    file_pointer += 3;

    if (!sound_audio_rate) {
        fd = file_buffer[file_pointer];
        codec = file_buffer[file_pointer + 1];

        switch (codec) {
            case 0:
                sound_audio_type = AUDIO_TYPE_PCM;
                sound_audio_bits = 8;
                break;
            case 4:
                sound_audio_type = AUDIO_TYPE_PCM;
                sound_audio_bits = 16;
                break;
            case 6:
                sound_audio_type = AUDIO_TYPE_ALAW;
                sound_audio_bits = 16;
                break;
            case 7:
                sound_audio_type = AUDIO_TYPE_ULAW;
                sound_audio_bits = 16;
                break;
            default:
                return -1;
                break;
        }
        sound_audio_rate = 1000000 / (256 - fd);
    }
    file_pointer += 2;
    size -= 2;

    if (voc_buffer1) {
        lib_free(voc_buffer1);
        voc_buffer1 = NULL;
        return -1;
    }

    voc_buffer1 = lib_malloc(size);
    memcpy(voc_buffer1, file_buffer + file_pointer, size);
    file_pointer += size;
    voc_buffer_size = size;

    return 0;
}

static int voc_handle_sound_2(int channels)
{
    unsigned int size;

    if (file_pointer + 6 > file_size) {
        return -1;
    }

    ++file_pointer;

    size = (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (file_pointer + size > file_size) {
        return -1;
    }
    file_pointer += 5;
    size -= 2;
    
    if (!voc_buffer1) {
        return -1;
    }
    voc_buffer2 = lib_malloc(voc_buffer_size + size);
    memcpy(voc_buffer2, voc_buffer1, voc_buffer_size);
    memcpy(voc_buffer2 + voc_buffer_size, file_buffer + file_pointer, size);
    lib_free(voc_buffer1);
    voc_buffer1 = voc_buffer2;
    voc_buffer2 = NULL;
    voc_buffer_size += size;
    file_pointer += size;

    return 0;
}

static int voc_handle_silence(int channels)
{
    unsigned int size;

    if (file_pointer + 7 > file_size) {
        return -1;
    }

    ++file_pointer;

    size = (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (size != 3) {
        return -1;
    }

    file_pointer += 3;

    size = (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];

    if (sound_audio_type == AUDIO_TYPE_UNKNOWN) {
        return -1;
    }

    if (sound_audio_type == AUDIO_TYPE_PCM) {
        size *= (sound_audio_bits / 8);
    }
    size *= sound_audio_channels;

    voc_buffer2 = lib_malloc(voc_buffer_size + size);

    if (sound_audio_bits == 8) {
        memset(voc_buffer2 + voc_buffer_size, 0x80, size);
    } else {
        memset(voc_buffer2 + voc_buffer_size, 0, size);
    }

    if (voc_buffer1) {
        memcpy(voc_buffer2, voc_buffer1, voc_buffer_size);
        lib_free(voc_buffer1);
    }
    voc_buffer1 = voc_buffer2;
    voc_buffer2 = NULL;
    voc_buffer_size += size;
    file_pointer += 3;
    return 0;
}

static int voc_handle_sound_9(int channels)
{
    unsigned int size;
    WORD codec;

    if (file_pointer + 16 > file_size) {
        return -1;
    }

    ++file_pointer;

    size = (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (size + file_pointer > file_size) {
        return -1;
    }

    file_pointer += 3;

    sound_audio_rate = (file_buffer[file_pointer + 3] << 24) | (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (!sound_audio_rate) {
        return -1;
    }

    file_pointer += 4;
    size -= 4;

    sound_audio_bits = file_buffer[file_pointer];

    switch (sound_audio_bits) {
        case 8:
        case 16:
        case 24:
        case 32:
            break;
        default:
            return -1;
    }

    ++file_pointer;
    --size;

    sound_audio_channels = file_buffer[file_pointer];
    if (sound_audio_channels < 1 || sound_audio_channels > 2) {
        return -1;
    }

    ++file_pointer;
    --size;

    codec = (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];

    switch (codec) {
        case 0:
        case 4:
            sound_audio_type = AUDIO_TYPE_PCM;
            break;
        case 6:
            sound_audio_type = AUDIO_TYPE_ALAW;
            break;
        case 7:
            sound_audio_type = AUDIO_TYPE_ULAW;
            break;
        default:
            return -1;
            break;
    }

    file_pointer += 6;
    size -= 6;

    voc_buffer2 = lib_malloc(voc_buffer_size + size);

    if (voc_buffer1) {
        memcpy(voc_buffer2, voc_buffer1, voc_buffer_size);
        memcpy(voc_buffer2 + voc_buffer_size, file_buffer + file_pointer, size);
        lib_free(voc_buffer1);
    }
    voc_buffer1 = voc_buffer2;
    voc_buffer2 = NULL;
    voc_buffer_size += size;
    file_pointer += size;

    return 0;
}

static int voc_handle_extra_info(int channels)
{
    unsigned int size;
    WORD fd;
    BYTE codec;

    if (file_pointer + 8 > file_size) {
        return -1;
    }

    ++file_pointer;

    size = (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (size != 4) {
        return -1;
    }
    file_pointer += 3;

    fd = (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    codec = file_buffer[file_pointer + 2];

    switch (codec) {
        case 0:
            sound_audio_type = AUDIO_TYPE_PCM;
            sound_audio_bits = 8;
            break;
        case 4:
            sound_audio_type = AUDIO_TYPE_PCM;
            sound_audio_bits = 16;
            break;
        case 6:
            sound_audio_type = AUDIO_TYPE_ALAW;
            sound_audio_bits = 16;
            break;
        case 7:
            sound_audio_type = AUDIO_TYPE_ULAW;
            sound_audio_bits = 16;
            break;
        default:
            return -1;
            break;
    }
    sound_audio_channels = file_buffer[file_pointer + 3] + 1;
    sound_audio_rate = 256000000 / (sound_audio_channels * (65536 - fd));
    file_pointer += 4;
    return 0;
}

static int voc_handle_text(void)
{
    unsigned int size;

    if (file_pointer + 4 > file_size) {
        return -1;
    }
    ++file_pointer;
    size = (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    file_pointer += 3;

    if (file_pointer + size > file_size) {
        return -1;
    }
    if (size) {
        file_pointer += size;
    }
    return 0;
}

static int voc_handle_ignore(unsigned int amount)
{
    unsigned int size;

    if (amount + 4 + file_pointer > file_size) {
        return -1;
    }
    ++file_pointer;
    size = (file_buffer[file_pointer + 2] << 16) | (file_buffer[file_pointer + 1] << 8) | file_buffer[file_pointer];
    if (size != amount) {
        return -1;
    }
    file_pointer += 3;
    if (amount) {
        file_pointer += amount;
    }
    return 0;
}

static int handle_voc_file(int channels)
{
    WORD version;
    WORD check;
    int end_of_stream = 0;
    int err = 0;

    sound_audio_channels = 0;
    sound_audio_rate = 0;
    sound_audio_bits = 0;
    sound_audio_type = AUDIO_TYPE_UNKNOWN;

    if (file_buffer[19] != 0x1A) {
        log_warning(LOG_DEFAULT, "$1A signature not found");
        return -1;
    }

    if (file_buffer[20] != 0x1A || file_buffer[21] != 0) {
        log_warning(LOG_DEFAULT, "Incorrect voc file header length : %X", (file_buffer[21] << 8) | file_buffer[20]);
        return -1;
    }

    version = (file_buffer[23] << 8) | file_buffer[22];
    check = (file_buffer[25] << 8) | file_buffer[24];

    if (check != ~version + 0x1234) {
        log_warning(LOG_DEFAULT, "VOC file header checksum incorrect: %4X %4X", check, version);
        return -1;
    }

    file_pointer = 26;

    while (file_pointer <= file_size && !end_of_stream) {
        switch (file_buffer[file_pointer]) {
            case 0:
                end_of_stream = 1;
                break;
            case 1:
                err = voc_handle_sound_1(channels);
                break;
            case 2:
                err = voc_handle_sound_2(channels);
                break;
            case 3:
                err = voc_handle_silence(channels);
                break;
            case 4:
            case 6:
                err = voc_handle_ignore(2);
                break;
            case 5:
                err = voc_handle_text();
                break;
            case 7:
                err = voc_handle_ignore(0);
                break;
            case 8:
                err = voc_handle_extra_info(channels);
                break;
            case 9:
                err = voc_handle_sound_9(channels);
                break;
            default:
                log_warning(LOG_DEFAULT, "Unknown VOC block type : %2X", file_buffer[file_pointer]);
                return -1;
        }
        if (err) {
            if (voc_buffer1) {
                lib_free(voc_buffer1);
                voc_buffer1 = NULL;
            }
            return -1;
        }
    }

    /* This is a wip, the actual decoding of the resulting buffer still needs to be made */

    return 0;
}

static int is_voc_file(void)
{
    char header[] = { 0x43, 0x72, 0x65, 0x61, 0x74, 0x69, 0x76, 0x65, 0x20, 0x56, 0x6F, 0x69, 0x63, 0x65, 0x20, 0x46, 0x69, 0x6C, 0x65 };
    int i;

    if (file_size < 26) {
        return 0;
    }

    /* Check for voc header signature */
    for (i = 0; i < sizeof(header); ++i) {
        if (file_buffer[i] != header[i]) {
            return 0;
        }
    }
    return 1;
}

/* ---------------------------------------------------------------------- */

static int handle_file_type(int channels)
{
    /* Check for wav file */
    if (is_wav_file()) {
        log_warning(LOG_DEFAULT, "filetype recognized as a WAVE file, starting parsing.");
        return handle_wav_file(channels);
    }

    /* Check for voc file */
    if (is_voc_file()) {
        log_warning(LOG_DEFAULT, "filetype recognized as a VOC file, starting parsing.");
        return handle_voc_file(channels);
    }

    log_warning(LOG_DEFAULT, "filetype was not handled.");
    return -1;
}

/* ---------------------------------------------------------------------- */

static void file_load_sample(int channels)
{
    FILE *sample_file = NULL;
    int err = 0;

    sample_file = fopen(SAMPLE_NAME, "rb");
    if (sample_file) {
        fseek(sample_file, 0, SEEK_END);
        file_size = ftell(sample_file);
        fseek(sample_file, 0, SEEK_SET);
        file_buffer = lib_malloc(file_size);
        if (fread(file_buffer, 1, file_size, sample_file) != file_size) {
            log_warning(LOG_DEFAULT, "Unexpected end of data in '%s'.", SAMPLE_NAME);
        }
        fclose(sample_file);
        err = handle_file_type(channels);
        if (!err) {
            sound_sampling_started = 0;
            sound_cycles_per_frame = machine_get_cycles_per_frame();
            sound_frames_per_sec = machine_get_cycles_per_second() / sound_cycles_per_frame;
            sound_samples_per_frame = sound_audio_rate / sound_frames_per_sec;
            current_channels = channels;
        } else {
            lib_free(file_buffer);
            file_buffer = NULL;
            log_warning(LOG_DEFAULT, "Unknown file type for '%s'.", SAMPLE_NAME);
        }
    } else {
        log_warning(LOG_DEFAULT, "Cannot open sampler file: '%s'.", SAMPLE_NAME);
    }
}

static void file_free_sample(void)
{
    if (sample_buffer1) {
        if (sample_buffer2) {
            if (sample_buffer1 != sample_buffer2) {
                lib_free(sample_buffer2);
            }
            sample_buffer2 = NULL;
        }
        lib_free(sample_buffer1);
        sample_buffer1 = NULL;
    }
}

/* For now channel is ignored */
static BYTE file_get_sample(int channel)
{
    unsigned int current_frame = 0;
    unsigned int current_cycle = 0;
    unsigned int frame_diff = 0;
    unsigned int frame_sample = 0;

    if (!sample_buffer1) {
        return 0x80;
    }
    if (!sound_sampling_started) {
        sound_sampling_started = 1;
        old_frame = maincpu_clk / sound_cycles_per_frame;
        return sample_buffer1[0];
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

    return sample_buffer1[(frame_sample + sound_sample_frame_start) % sample_size];
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
