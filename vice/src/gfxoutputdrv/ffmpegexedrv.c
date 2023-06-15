/** \file   ffmpegexedrv.c
 * \brie    Movie driver using FFMPEG executable
 *
 * \author  Andreas Matthies <andreas.matthies@gmx.net>
 * \author  groepaz@gmx.net
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
    This is a video output driver implemented to replace the original FFMPEG
    driver. Instead of linking in the libraries, this one calls the ffmpeg
    executable and pipes the data to it.

    This driver was implemented based on a copy of the original driver, and it
    can co-exist with the original driver AND USE THE SAME RESOURCES AND SHARE
    COMMANDLINE OPTIONS for the time being. To make sure this keeps working,
    do NOT alter the resource- or commandline related code, before all of the
    old FFMPEG code was removed. (And at this point some of the code in this
    file can likely be more simplified too).

    bugs/open ends:
    - audio-only saving does not work correctly, apparently due to how the codecs
      are being listed (and somehow the results come out wrong)
    - there should probably be an endianess conversion for the audio stream
    - there should probably be a check that makes sure "ffmpeg" exists and
      can be executed
    - the available codecs/containers should get queried from the ffmpeg binary

    * https://ffmpeg.org/ffmpeg.html
    * https://ffmpeg.org/~michael/nut.txt
    * https://github.com/lu-zero/nut/blob/master/src/
    * https://gist.github.com/victusfate/b1fbc822957020bfc063

 */

/* #define DEBUG_FFMPEG */
/*#define DEBUG_FFMPEG_FRAMES*/

#include "vice.h"

#include <assert.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

#include "archdep.h"
#include "cmdline.h"
#include "coproc.h"
#include "ffmpegdrv.h"
#include "ffmpegexedrv.h"
#include "gfxoutput.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "palette.h"
#include "resources.h"
#include "screenshot.h"
#include "soundmovie.h"
#include "uiapi.h"
#include "util.h"
#include "vicesocket.h"

#ifdef DEBUG_FFMPEG
#define DBG(x) log_debug x
#else
#define DBG(x)
#endif

#ifdef DEBUG_FFMPEG_FRAMES
#define DBGFRAMES(x) log_debug x
#else
#define DBGFRAMES(x)
#endif

/* FIXME: use IDs from libavformat while we coexist with the old ffmpeg code */
#ifdef HAVE_FFMPEG
#  include "libavformat/avformat.h"
#else
#define AV_CODEC_ID_NONE      0
#define AV_CODEC_ID_MP2             1
#define AV_CODEC_ID_MP3             2
#define AV_CODEC_ID_FLAC            3
#define AV_CODEC_ID_PCM_S16LE       4
#define AV_CODEC_ID_AAC             5
#define AV_CODEC_ID_AC3             6
#define AV_CODEC_ID_MPEG4           7
#define AV_CODEC_ID_MPEG1VIDEO      8
#define AV_CODEC_ID_FFV1            9
#define AV_CODEC_ID_H264            10
#define AV_CODEC_ID_THEORA          11
#define AV_CODEC_ID_H265            12
#endif

/* FIXME: check/fix make sure this returns valid ffmpeg vcodec/acodec strings */
static char *av_codec_get_option(int id)
{
    switch(id) {
        case AV_CODEC_ID_NONE: return "none";
        case AV_CODEC_ID_MP2: return "mp2";
        case AV_CODEC_ID_MP3: return "mp3";
        case AV_CODEC_ID_FLAC: return "flac";
        case AV_CODEC_ID_PCM_S16LE: return "s16le";
        case AV_CODEC_ID_AAC: return "aac";
        case AV_CODEC_ID_AC3: return "ac3";
        case AV_CODEC_ID_MPEG4: return "mpeg4";
        case AV_CODEC_ID_MPEG1VIDEO: return "mpeg1video";
        case AV_CODEC_ID_FFV1: return "ffv1";
        case AV_CODEC_ID_H264: return "h264";
        case AV_CODEC_ID_THEORA: return "theora";
        case AV_CODEC_ID_H265: return "h265";
        default: return "unknown";
    }
}

/******************************************************************************/

/* FIXME: some SDL UIs use ffmpegdrv_formatlist directly */
#define ffmpegexedrv_formatlist ffmpegdrv_formatlist

#ifdef HAVE_FFMPEG
/* FIXME: while we coexist with the old driver, do not use our own format list */
#else

static gfxoutputdrv_codec_t mp4_audio_codeclist[] = {
    { AV_CODEC_ID_AAC,          "AAC" },
    { AV_CODEC_ID_MP3,          "MP3" },
    { AV_CODEC_ID_AC3,          "AC3" },
    { 0, NULL }
};

static gfxoutputdrv_codec_t mp4_video_codeclist[] = {
    { AV_CODEC_ID_H264,         "H264" },
    { AV_CODEC_ID_H265,         "H265" },
    { 0, NULL }
};

static gfxoutputdrv_codec_t ogg_audio_codeclist[] = {
    { AV_CODEC_ID_FLAC,         "FLAC" },
    { 0, NULL }
};

static gfxoutputdrv_codec_t ogg_video_codeclist[] = {
    { AV_CODEC_ID_THEORA,       "Theora" },
    { 0, NULL }
};

static gfxoutputdrv_codec_t avi_audio_codeclist[] = {
    { AV_CODEC_ID_MP2,          "MP2" },
    { AV_CODEC_ID_MP3,          "MP3" },
    { AV_CODEC_ID_FLAC,         "FLAC" },
    { AV_CODEC_ID_PCM_S16LE,    "PCM uncompressed" },
    { 0, NULL }
};

static gfxoutputdrv_codec_t avi_video_codeclist[] = {
    { AV_CODEC_ID_MPEG4,        "MPEG4 (DivX)" },
    { AV_CODEC_ID_MPEG1VIDEO,   "MPEG1" },
    { AV_CODEC_ID_FFV1,         "FFV1 (lossless)" },
    { AV_CODEC_ID_H264,         "H264" },
    { AV_CODEC_ID_THEORA,       "Theora" },
    { 0, NULL }
};

static gfxoutputdrv_codec_t mp3_audio_codeclist[] = {
    { AV_CODEC_ID_MP3,          "mp3" },
    { 0, NULL }
};

static gfxoutputdrv_codec_t mp2_audio_codeclist[] = {
    { AV_CODEC_ID_MP2,          "mp2" },
    { 0, NULL }
};

#if 0
static gfxoutputdrv_codec_t none_codeclist[] = {
    { AV_CODEC_ID_NONE, "" },
    { 0, NULL }
};
#endif

/* formatlist is filled from with available formats and codecs at init time */
gfxoutputdrv_format_t *ffmpegexedrv_formatlist = NULL;
static gfxoutputdrv_format_t output_formats_to_test[] =
{
    { "mp4",        mp4_audio_codeclist, mp4_video_codeclist },
    { "ogg",        ogg_audio_codeclist, ogg_video_codeclist },
    { "avi",        avi_audio_codeclist, avi_video_codeclist },
    { "matroska",   mp4_audio_codeclist, mp4_video_codeclist },
    { "wav",        avi_audio_codeclist, NULL },
    { "mp3",        mp3_audio_codeclist, NULL }, /* formats expects png which fails in VICE */
    { "mp2",        mp2_audio_codeclist, NULL },
    { NULL, NULL, NULL }
};
#endif

/******************************************************************************/

/* general */
static int file_init_done;

#define DUMMY_FRAMES_VIDEO  1
#define DUMMY_FRAMES_AUDIO  200 /* FIXME: calculate from AUDIO_SKIP_SECONDS */

#define AUDIO_SKIP_SECONDS  4

#define SOCKETS_VIDEO_PORT  60000
#define SOCKETS_AUDIO_PORT  60001

/* input video stream */
#define INPUT_VIDEO_BPP     3

static double time_base;
static double fps;                  /* frames per second */
static uint64_t framecounter = 0;   /* number of processed video frames */

typedef struct {
    uint8_t *data;
    int linesize;
} VIDEOFrame;
static VIDEOFrame *video_st_frame;

/* input audio stream */
#define AUDIO_BUFFER_SAMPLES        0x400
#define AUDIO_BUFFER_MAX_CHANNELS   2

static soundmovie_buffer_t ffmpegexedrv_audio_in;
static int audio_init_done;
static int audio_is_open;
static int audio_has_codec = -1;
static uint64_t audio_input_counter = 0;    /* total samples played */
static int audio_input_sample_rate = -1;    /* samples per second */
static int audio_input_channels = -1;

/* output video */
static int video_init_done;
static int video_is_open;
static int video_has_codec = -1;
static int video_width = -1;
static int video_height = -1;

/* ffmpeg interface */
static int ffmpeg_stdin = 0;
static int ffmpeg_stdout = 0;

static vice_network_socket_t *ffmpeg_video_socket = NULL;
static vice_network_socket_t *ffmpeg_audio_socket = NULL;

static char *outfilename = NULL;

/******************************************************************************/

static int ffmpegexedrv_init_file(void);
static void ffmpegexedrv_shutdown(void);

/******************************************************************************/
/* resources */

/* FIXME: shadowed by resources */
static char *ffmpegexe_format = NULL;    /* FFMPEGFormat */
static int format_index;    /* FFMPEGFormat */
static int audio_codec;
static int video_codec;
static int audio_bitrate;
static int video_bitrate;
static int video_halve_framerate;

/* FIXME: resource stuff, use this one instead of the one from internal ffmpeg */
#ifndef HAVE_FFMPEG
static int set_container_format(const char *val, void *param)
{
    int i;

    /* kludge to prevent crash at startup when using --help on the commandline */
    if (ffmpegexedrv_formatlist == NULL) {
        return 0;
    }

    format_index = -1;

    for (i = 0; ffmpegexedrv_formatlist[i].name != NULL; i++) {
        if (strcmp(val, ffmpegexedrv_formatlist[i].name) == 0) {
            format_index = i;
        }
    }

    if (format_index < 0) {
        return -1;
    }

    if (ffmpegexe_format) {
        lib_free(ffmpegexe_format);
        ffmpegexe_format = NULL;
    }
    util_string_set(&ffmpegexe_format, val);

    return 0;
}

static int set_audio_bitrate(int val, void *param)
{
    audio_bitrate = val;

    if ((audio_bitrate < VICE_FFMPEG_AUDIO_RATE_MIN)
        || (audio_bitrate > VICE_FFMPEG_AUDIO_RATE_MAX)) {
        audio_bitrate = VICE_FFMPEG_AUDIO_RATE_DEFAULT;
    }
    return 0;
}

static int set_video_bitrate(int val, void *param)
{
    video_bitrate = val;

    if ((video_bitrate < VICE_FFMPEG_VIDEO_RATE_MIN)
        || (video_bitrate > VICE_FFMPEG_VIDEO_RATE_MAX)) {
        video_bitrate = VICE_FFMPEG_VIDEO_RATE_DEFAULT;
    }
    return 0;
}

static int set_audio_codec(int val, void *param)
{
    audio_codec = val;
    return 0;
}

static int set_video_codec(int val, void *param)
{
    video_codec = val;
    return 0;
}

static int set_video_halve_framerate(int value, void *param)
{
    int val = value ? 1 : 0;

    if (video_halve_framerate != val && screenshot_is_recording()) {
        ui_error("Can't change framerate while recording. Try again later.");
        return 0;
    }

    video_halve_framerate = val;

    return 0;
}
#endif

/*---------- Resources ------------------------------------------------*/

static const resource_string_t resources_string[] = {
/* FIXME: register only here, not in the internal ffmpeg driver */
#ifndef HAVE_FFMPEG
    { "FFMPEGFormat", "mp4", RES_EVENT_NO, NULL,
      &ffmpegexe_format, set_container_format, NULL },
#endif
    RESOURCE_STRING_LIST_END
};

static const resource_int_t resources_int[] = {
/* FIXME: register only here, not in the internal ffmpeg driver */
#ifndef HAVE_FFMPEG
    { "FFMPEGAudioBitrate", VICE_FFMPEG_AUDIO_RATE_DEFAULT,
      RES_EVENT_NO, NULL,
      &audio_bitrate, set_audio_bitrate, NULL },
    { "FFMPEGVideoBitrate", VICE_FFMPEG_VIDEO_RATE_DEFAULT,
      RES_EVENT_NO, NULL,
      &video_bitrate, set_video_bitrate, NULL },
    { "FFMPEGAudioCodec", AV_CODEC_ID_AAC, RES_EVENT_NO, NULL,
      &audio_codec, set_audio_codec, NULL },
    { "FFMPEGVideoCodec", AV_CODEC_ID_H264, RES_EVENT_NO, NULL,
      &video_codec, set_video_codec, NULL },
    { "FFMPEGVideoHalveFramerate", 0, RES_EVENT_NO, NULL,
      &video_halve_framerate, set_video_halve_framerate, NULL },
#endif
    RESOURCE_INT_LIST_END
};

/* Driver API gfxoutputdrv_t.resources_init */
static int ffmpegexedrv_resources_init(void)
{
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }

    return resources_register_int(resources_int);
}

/*---------- Commandline options --------------------------------------*/

static const cmdline_option_t cmdline_options[] =
{
/* FIXME: register only here, not in the internal ffmpeg driver */
#ifndef HAVE_FFMPEG
    { "-ffmpegaudiobitrate", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "FFMPEGAudioBitrate", NULL,
      "<value>", "Set bitrate for audio stream in media file" },
    { "-ffmpegvideobitrate", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "FFMPEGVideoBitrate", NULL,
      "<value>", "Set bitrate for video stream in media file" },
#endif
    CMDLINE_LIST_END
};

/* Driver API gfxoutputdrv_t.cmdline_options_init */
static int ffmpegexedrv_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/*---------------------------------------------------------------------*/

#ifdef HAVE_FFMPEG
/* FIXME: when we coexist with the old FFMPEG driver, we use this function to
          read the actual resource values (defined in the old driver) into the
          variables local to this file, so other code can use these "as if" the
          resources were defined in this file, rather than the old driver. */
static void get_resource_values(void)
{
    int i;
    resources_get_string("FFMPEGFormat", (const char**)&ffmpegexe_format);
    format_index = -1;
    for (i = 0; ffmpegexedrv_formatlist[i].name != NULL; i++) {
        if (strcmp(ffmpegexe_format, ffmpegexedrv_formatlist[i].name) == 0) {
            format_index = i;
        }
    }

    resources_get_int("FFMPEGVideoCodec", &video_codec);
    resources_get_int("FFMPEGVideoBitrate", &video_bitrate);
    resources_get_int("FFMPEGAudioCodec", &audio_codec);
    resources_get_int("FFMPEGAudioBitrate", &audio_bitrate);
    resources_get_int("FFMPEGVideoHalveFramerate", &video_halve_framerate);
}
#endif

static void log_resource_values(const char *func)
{
    DBG(("%s FFMPEGFormat:%s", func, ffmpegexe_format));
    DBG(("%s FFMPEGVideoCodec:%d:'%s'", func, video_codec, av_codec_get_option(video_codec)));
    DBG(("%s FFMPEGVideoBitrate:%d", func, video_bitrate));
    DBG(("%s FFMPEGAudioCodec:%d:'%s'", func, audio_codec, av_codec_get_option(audio_codec)));
    DBG(("%s FFMPEGAudioBitrate:%d", func, audio_bitrate));
    DBG(("%s FFMPEGVideoHalveFramerate:%d", func, video_halve_framerate));
}

static int write_video_frame(VIDEOFrame *pic)
{
    size_t len = INPUT_VIDEO_BPP * video_height * video_width;
    if ((video_has_codec > 0) && (video_codec != AV_CODEC_ID_NONE)) {
        if (ffmpeg_video_socket == 0) {
            log_error(LOG_DEFAULT, "FFMPEG: write_video_frame ffmpeg_video_socket is 0 (framecount:%"PRIu64")\n", framecounter);
            return 0;
        }
        return (int)len - vice_network_send(ffmpeg_video_socket, pic->data, len, 0 /* flags */);
    }
    return 0;
}

static int start_ffmpeg_executable(void)
{
    static char command[0x400];
    static char tempcommand[0x400];
    int fpsint;
    int fpsfrac;
    int n;
    int frm;
    int len;
    int res;
    int audio_connected = 0;
    int video_connected = 0;

#ifdef HAVE_FFMPEG
    get_resource_values();
#endif
    log_resource_values(__FUNCTION__);

    /* FPS of the input, including "half framerate" */
    fpsint = fps;
    fpsfrac = (fps * 100.0f) - (fpsint * 100.0f);

    strcpy(command,
            "ffmpeg "
            /* Caution: at least on windows we must avoid that the ffmpeg
               executable produces output on stdout - if it does, the process
               may block and wait for ffmpeg_stderr being read */
            "-hide_banner "
#ifdef DEBUG_FFMPEG
            /* "-loglevel trace " */
            "-loglevel verbose "
            /* "-loglevel error " */
#else
            "-loglevel quiet "
            "-nostdin "
#endif
    );

    /* options to define the input video format */
    if ((video_has_codec > 0) && (video_codec != AV_CODEC_ID_NONE)) {
        sprintf(tempcommand,
                "-f rawvideo "
                "-pix_fmt rgb24 "
                "-framerate %2d.%02d "              /* exact fps */
                "-s %dx%d "                         /* size */
                /*"-readrate_initial_burst 0 "*/        /* no initial burst read */
                /*"-readrate 1 "*/                      /* Read input at native frame rate */
                "-thread_queue_size 512 "
                "-vsync 0 "
                "-i tcp://127.0.0.1:%d?listen "
                , fpsint, fpsfrac
                , video_width, video_height
                , SOCKETS_VIDEO_PORT
        );
        strcat(command, tempcommand);
    }

    /* options to define the input audio format */
    if ((audio_has_codec > 0) && (audio_codec != AV_CODEC_ID_NONE)) {
            sprintf(tempcommand,
                    "-f s16le "                     /* input audio stream format */
                    "-acodec pcm_s16le "            /* audio codec */
                    "-channels %d "                 /* input audio channels */
                    "-ac %d "                       /* input audio channels */
                    "-ar %d "                       /* input audio stream sample rate */
                    "-ss %d "                       /* skip seconds at start */
                    "-thread_queue_size 512 "
                    "-i tcp://127.0.0.1:%d?listen "
                    "-async 1 "
                    , audio_input_channels
                    , audio_input_channels
                    , audio_input_sample_rate
                    , AUDIO_SKIP_SECONDS
                    , SOCKETS_AUDIO_PORT
            );
            strcat(command, tempcommand);
    }

    /* options for the output file */
    sprintf(tempcommand,
            "-y "           /* overwrite existing file */
            "-f %s "        /* outfile format/container */
            "-shortest "    /* Finish encoding when the shortest output stream ends. */
            /*"-shortest_buf_duration 1 "*/ /* the maximum duration of buffered frames in seconds */
            "-async 1 "
            , ffmpegexe_format                      /* outfile format/container */
    );
    strcat(command, tempcommand);
    /* options for the output file (video) */
    if ((video_has_codec > 0) && (video_codec != AV_CODEC_ID_NONE)) {
        sprintf(tempcommand,
                "-framerate %2d.%02d "              /* exact fps */
                "-vcodec %s "   /* outfile video codec */
                "-b:v %d "      /* outfile video bitrate */
                , fpsint, fpsfrac
                , av_codec_get_option(video_codec)     /* outfile video codec */
                , video_bitrate               /* outfile video bitrate */
        );
        strcat(command, tempcommand);
    }
    /* options for the output file (audio) */
    if ((audio_has_codec > 0) && (audio_codec != AV_CODEC_ID_NONE)) {
        sprintf(tempcommand,
                "-acodec %s "   /* outfile audio codec */
                "-b:a %d "      /* outfile audio bitrate */
                , av_codec_get_option(audio_codec)     /* outfile audio codec */
                , audio_bitrate               /* outfile audio bitrate */
        );
        strcat(command, tempcommand);
    }

    /* last not least the output file name */
    strcat(command, outfilename ? outfilename : "outfile.avi");

    /*DBG(("forking ffmpeg: '%s'", command));*/
    if (fork_coproc(&ffmpeg_stdin, &ffmpeg_stdout, command) < 0) {
        log_error(LOG_DEFAULT, "Cannot fork process '%s'.", command);
        return -1;
    }

    if ((video_has_codec > 0) && (video_codec != AV_CODEC_ID_NONE)) {
        vice_network_socket_address_t *ad = NULL;
        ad = vice_network_address_generate("127.0.0.1", SOCKETS_VIDEO_PORT);
        if (!ad) {
            log_error(LOG_DEFAULT, "Bad device name.\n");
            return -1;
        }
        /* connect socket */
        for (n = 0; n < 200; n++) {
            ffmpeg_video_socket = vice_network_client(ad);
            if (!ffmpeg_video_socket) {
                /*log_error(LOG_DEFAULT, "ffmpegexedrv: Error connecting AUDIO socket");*/
                archdep_usleep(1000);
            } else {
                log_message(LOG_DEFAULT, "ffmpegexedrv: VIDEO connected");
                video_connected = 1;
                break;
            }
        }
        if (!video_connected) {
            log_error(LOG_DEFAULT, "ffmpegexedrv: Error connecting VIDEO socket");
            return -1;
        }
    }
    /* clear frame */
    len = INPUT_VIDEO_BPP * video_height * video_width;
    DBG(("video len:%d (%d)", len, len * DUMMY_FRAMES_VIDEO));
    memset(video_st_frame->data, 0, len);
    for (frm = 0; frm < DUMMY_FRAMES_VIDEO; frm++) {
        write_video_frame(video_st_frame);
    }

    if ((audio_has_codec > 0) && (audio_codec != AV_CODEC_ID_NONE)) {
        vice_network_socket_address_t *ad = NULL;
        ad = vice_network_address_generate("127.0.0.1", SOCKETS_AUDIO_PORT);
        if (!ad) {
            log_error(LOG_DEFAULT, "Bad device name.\n");
            return -1;
        }
        /* connect socket */
        for (n = 0; n < 200; n++) {
            ffmpeg_audio_socket = vice_network_client(ad);
            if (!ffmpeg_audio_socket) {
                /*log_error(LOG_DEFAULT, "ffmpegexedrv: Error connecting AUDIO socket");*/
                archdep_usleep(1000);
            } else {
                log_message(LOG_DEFAULT, "ffmpegexedrv: AUDIO connected");
                audio_connected = 1;
                break;
            }
        }
        if (!audio_connected) {
            log_error(LOG_DEFAULT, "ffmpegexedrv: Error connecting AUDIO socket");
            return -1;
        }
    }
    /* clear frame */
    len = (sizeof(uint16_t) * audio_input_sample_rate) / fps;
    if (video_halve_framerate) {
        len /= 2;
    }
    DBG(("audio len:%d (%d)", len, len * DUMMY_FRAMES_AUDIO));
    memset(ffmpegexedrv_audio_in.buffer, 0, len);
    for (frm = 0; frm < DUMMY_FRAMES_AUDIO; frm++) {
        res = vice_network_send(ffmpeg_audio_socket, ffmpegexedrv_audio_in.buffer, len, 0 /* flags */);
        if (res != len) {
            log_error(LOG_DEFAULT, "ffmpegexedrv: Error writing to AUDIO socket");
            return -1;
        }
        if (audio_input_channels == 2) {
            /* stereo - send twice the amount of data */
            res = vice_network_send(ffmpeg_audio_socket, ffmpegexedrv_audio_in.buffer, len, 0 /* flags */);
            if (res != len) {
                log_error(LOG_DEFAULT, "ffmpegexedrv: Error writing to AUDIO socket");
                return -1;
            }
        }
    }
    log_message(LOG_DEFAULT, "ffmpegexedrv: pipes are ready");
    return 0;
}

static void close_video_stream(void)
{
    if (ffmpeg_video_socket != NULL) {
        vice_network_socket_close(ffmpeg_video_socket);
        ffmpeg_video_socket = NULL;
    }
}

static void close_audio_stream(void)
{
    if (ffmpeg_audio_socket != NULL) {
        vice_network_socket_close(ffmpeg_audio_socket);
        ffmpeg_audio_socket = NULL;
    }
}

static void close_stream(void)
{
    if (ffmpeg_video_socket != NULL) {
        vice_network_socket_close(ffmpeg_video_socket);
        ffmpeg_video_socket = NULL;
    }
    if (ffmpeg_audio_socket != NULL) {
        vice_network_socket_close(ffmpeg_audio_socket);
        ffmpeg_audio_socket = NULL;
    }

    if (ffmpeg_stdin != -1) {
        close(ffmpeg_stdin);
        ffmpeg_stdin = -1;
    }
    if (ffmpeg_stdout != -1) {
        close(ffmpeg_stdout);
        ffmpeg_stdout = -1;
    }
}

/*****************************************************************************
   audio stream encoding
 *****************************************************************************/

/* called by ffmpegexedrv_init_video -> ffmpegexedrv_init_file */
static int ffmpegexedrv_open_audio(void)
{
    size_t audio_inbuf_size;
    DBG(("ffmpegexedrv_open_audio (%d,%d)", audio_input_channels, AUDIO_BUFFER_MAX_CHANNELS));
    /*assert((audio_input_channels > 0));*/
    /*if (audio_input_channels < 1) {
        log_warning(LOG_DEFAULT, "ffmpegexedrv_open_audio audio_input_channels < 1 (%d,%d)", audio_input_channels, AUDIO_BUFFER_MAX_CHANNELS);
    }*/

    audio_is_open = 1;

    /* FIXME: audio_input_channels NOT ready yet */
    ffmpegexedrv_audio_in.size = AUDIO_BUFFER_SAMPLES;
    ffmpegexedrv_audio_in.used = 0;
    /* audio_inbuf_size = AUDIO_BUFFER_SAMPLES * sizeof(int16_t) * audio_input_channels; */
    audio_inbuf_size = AUDIO_BUFFER_SAMPLES * sizeof(int16_t) * AUDIO_BUFFER_MAX_CHANNELS;

    ffmpegexedrv_audio_in.buffer = lib_malloc(audio_inbuf_size);
    if (ffmpegexedrv_audio_in.buffer == NULL) {
        log_error(LOG_DEFAULT, "ffmpegexedrv: Error allocating audio buffer (%u bytes)", (unsigned)audio_inbuf_size);
        return -1;
    }
    return 0;
}

static void ffmpegexedrv_close_audio(void)
{
    DBG(("ffmpegexedrv_close_audio"));

    close_audio_stream();

    audio_input_sample_rate = -1;
    audio_is_open = 0;
    audio_input_counter = 0;

    if (ffmpegexedrv_audio_in.buffer) {
        lib_free(ffmpegexedrv_audio_in.buffer);
    }
    ffmpegexedrv_audio_in.buffer = NULL;
    ffmpegexedrv_audio_in.size = 0;
}

/* Soundmovie API soundmovie_funcs_t.init */
/* called via ffmpegexedrv_soundmovie_funcs->init */
static int ffmpegexe_soundmovie_init(int speed, int channels, soundmovie_buffer_t ** audio_in)
{
    DBG(("ffmpegexe_soundmovie_init(speed: %d channels: %d)", speed, channels));

    audio_init_done = 1;

    *audio_in = &ffmpegexedrv_audio_in;
    (*audio_in)->used = 0;

    audio_input_sample_rate = speed;
    audio_input_channels = channels;

#if 0
    if (video_init_done) {
        ffmpegexedrv_init_file();
    }
#endif

    start_ffmpeg_executable();

    return 0;
}

/* Soundmovie API soundmovie_funcs_t.encode */
/* called via ffmpegexedrv_soundmovie_funcs->encode */
/* triggered by soundffmpegaudio->write */
static int ffmpegexe_soundmovie_encode(soundmovie_buffer_t *audio_in)
{
    int res;
#ifdef DEBUG_FFMPEG_FRAMES
    double frametime = (double)framecounter / fps;
    double audiotime = (double)audio_input_counter / (double)audio_input_sample_rate;
    if (audio_in) {
        DBGFRAMES(("ffmpegexe_soundmovie_encode(framecount:%lu, audiocount:%lu frametime:%f, audiotime:%f size:%d used:%d)",
            framecounter, audio_input_counter, frametime, audiotime, audio_in->size, audio_in->used));
    } else {
        DBG(("ffmpegexe_soundmovie_encode (NULL)"));
    }
#endif

    if (ffmpeg_audio_socket == 0) {
        log_error(LOG_DEFAULT, "FFMPEG: ffmpegexe_soundmovie_encode ffmpeg_audio_socket is 0 (framecount:%"PRIu64")\n", audio_input_counter);
        return 0;
    }

    if ((audio_has_codec > 0) && (audio_codec != AV_CODEC_ID_NONE)) {
        /* FIXME: we might have an endianess problem here, we might have to swap lo/hi on BE machines */
        if (audio_input_channels == 1) {
            res = vice_network_send(ffmpeg_audio_socket, &audio_in->buffer[0], audio_in->used * 2, 0 /* flags */);
            if (res != audio_in->used * 2) {
                return -1;
            }
            audio_input_counter += audio_in->used;
        } else if (audio_input_channels == 2) {
            res = vice_network_send(ffmpeg_audio_socket, &audio_in->buffer[0], audio_in->used * 2, 0 /* flags */);
            if (res != audio_in->used * 2) {
                return -1;
            }
            audio_input_counter += audio_in->used / 2;
        } else {
            return -1;
        }
    }

    audio_in->used = 0;
    return 0;
}

/* Soundmovie API soundmovie_funcs_t.close */
/* called via ffmpegexedrv_soundmovie_funcs->close */
static void ffmpegexe_soundmovie_close(void)
{
    DBG(("ffmpegexe_soundmovie_close"));
    /* just stop the whole recording */
    screenshot_stop_recording();
}

static soundmovie_funcs_t ffmpegexedrv_soundmovie_funcs = {
    ffmpegexe_soundmovie_init,
    ffmpegexe_soundmovie_encode,
    ffmpegexe_soundmovie_close
};

/*****************************************************************************
   video stream encoding
 *****************************************************************************/

static int video_fill_rgb_image(screenshot_t *screenshot, VIDEOFrame *pic)
{
    int x, y;
    int dx, dy;
    int colnum;
    int bufferoffset;
    int x_dim = screenshot->width;
    int y_dim = screenshot->height;
    int pix = 0;
    /* center the screenshot in the video */
    pic->linesize = video_width * INPUT_VIDEO_BPP;
    dx = (video_width - x_dim) / 2;
    dy = (video_height - y_dim) / 2;
    bufferoffset = screenshot->x_offset + (dx < 0 ? -dx : 0)
        + (screenshot->y_offset + (dy < 0 ? -dy : 0)) * screenshot->draw_buffer_line_size;

    for (y = 0; y < video_height; y++) {
        for (x = 0; x < video_width; x++) {
            colnum = screenshot->draw_buffer[bufferoffset + x];
            pic->data[pix + INPUT_VIDEO_BPP * x] = screenshot->palette->entries[colnum].red;
            pic->data[pix + INPUT_VIDEO_BPP * x + 1] = screenshot->palette->entries[colnum].green;
            pic->data[pix + INPUT_VIDEO_BPP * x + 2] = screenshot->palette->entries[colnum].blue;
        }
        bufferoffset += screenshot->draw_buffer_line_size;
        pix += pic->linesize;
    }

    return 0;
}

/* called by ffmpegexedrv_open_video() */
static VIDEOFrame* video_alloc_picture(int bpp, int width, int height)
{
    VIDEOFrame *picture;

    picture = lib_malloc(sizeof(VIDEOFrame));
    if (!picture) {
        return NULL;
    }
    picture->data = lib_malloc(bpp * width * height);
    if (!picture->data) {
        lib_free(picture);
        log_debug("ffmpegexedrv: Could not allocate frame data");
        return NULL;
    }

    picture->linesize = width;
    return picture;
}

static void video_free_picture(VIDEOFrame *picture)
{
    lib_free(picture->data);
    lib_free(picture);
}

/* called by ffmpegexedrv_init_file */
static int ffmpegexedrv_open_video(void)
{
    DBG(("ffmpegexedrv_open_video w:%d h:%d", video_width, video_height));

    video_is_open = 1;

    /* allocate the encoded raw picture */
    video_st_frame = video_alloc_picture(INPUT_VIDEO_BPP, video_width, video_height);
    if (!video_st_frame) {
        log_debug("ffmpegexedrv: could not allocate picture");
        return -1;
    }

    return 0;
}

/* called by ffmpegexedrv_close() */
static void ffmpegexedrv_close_video(void)
{
    DBG(("ffmpegexedrv_close_video"));
    close_video_stream();
    video_is_open = 0;
    if (video_st_frame) {
        video_free_picture(video_st_frame);
        video_st_frame = NULL;
    }
    framecounter = 0;
}

/* called by ffmpegexedrv_save */
static void ffmpegexedrv_init_video(screenshot_t *screenshot)
{
    DBG(("ffmpegexedrv_init_video"));
    video_init_done = 1;

    /* resolution should be a multiple of 16 */
    /* video_fill_rgb_image only implements cutting so */
    /* adding black border was removed */
    video_width = screenshot->width & ~0xf;
    video_height = screenshot->height & ~0xf;
    /* frames per second */
#ifdef HAVE_FFMPEG
    get_resource_values();
#endif
    log_resource_values(__FUNCTION__);

    DBG(("ffmpegexedrv_init_video w:%d h:%d (halve framerate:%d)",
           video_width, video_height, video_halve_framerate));
    time_base = machine_get_cycles_per_frame()
                                    / (double)(video_halve_framerate ?
                                        machine_get_cycles_per_second() / 2 :
                                        machine_get_cycles_per_second());
    fps = 1.0f / time_base;
    DBG(("ffmpegexedrv_init_video fps: %f timebase: %f", fps, time_base));
    framecounter = 0;

    ffmpegexedrv_init_file();
}


/*****************************************************************************/

/* called by ffmpegexedrv_init_video */
static int ffmpegexedrv_init_file(void)
{
    DBG(("ffmpegexedrv_init_file"));
#if 0
    if (!video_init_done || !audio_init_done) {
        return 0;
    }
#endif
    if (ffmpegexedrv_open_video() < 0) {
        ui_error("ffmpegexedrv: Cannot open video stream");
        screenshot_stop_recording();
        return -1;
    }

    if (ffmpegexedrv_open_audio() < 0) {
        ui_error("ffmpegexedrv: Cannot open audio stream");
        screenshot_stop_recording();
        return -1;
    }

    log_debug("ffmpegexedrv: Initialized file successfully");

    /*start_ffmpeg_executable();*/

    file_init_done = 1;

    return 0;
}

/* Driver API gfxoutputdrv_t.save */
/* called once to start recording video+audio */
static int ffmpegexedrv_save(screenshot_t *screenshot, const char *filename)
{
    gfxoutputdrv_format_t *format;

    DBG(("ffmpegexedrv_save(%s)", filename));

    audio_init_done = 0;
    video_init_done = 0;
    file_init_done = 0;

#ifdef HAVE_FFMPEG
    get_resource_values();
#endif
    log_resource_values(__FUNCTION__);

    DBG(("FFMPEGFormat:%s (format_index:%d)", ffmpegexe_format, format_index));

    if (format_index < 0) {
        return -1;
    }

    format = &ffmpegexedrv_formatlist[format_index];

    audio_has_codec = (format->audio_codecs != NULL);
    /* the codec from resource */
    DBG(("ffmpegexedrv_save(audio_has_codec: %d audio_codec:%d:%s)",
            audio_has_codec, audio_codec, av_codec_get_option(audio_codec)));

    video_has_codec = (format->video_codecs != NULL);
    /* the codec from resource */
    DBG(("ffmpegexedrv_save(video_has_codec: %d video_codec:%d:%s)",
            video_has_codec, video_codec, av_codec_get_option(video_codec)));

    outfilename = lib_strdup(filename);

    ffmpegexedrv_init_video(screenshot);

    soundmovie_start(&ffmpegexedrv_soundmovie_funcs);

    return 0;
}

/* Driver API gfxoutputdrv_t.close */
static int ffmpegexedrv_close(screenshot_t *screenshot)
{
    DBG(("ffmpegexedrv_close"));

    soundmovie_stop();

    ffmpegexedrv_close_video();
    ffmpegexedrv_close_audio();

    /* free the streams */
    close_stream();

    log_debug("ffmpegexedrv: Closed successfully");

    file_init_done = 0;

    if (outfilename) {
        lib_free(outfilename);
        outfilename = NULL;
    }

    return 0;
}

/* Driver API gfxoutputdrv_t.record */
/* triggered by screenshot_record, periodically called to output video data stream */
static int ffmpegexedrv_record(screenshot_t *screenshot)
{
    double frametime = (double)framecounter / fps;
    double audiotime = (double)audio_input_counter / (double)audio_input_sample_rate;
    DBGFRAMES(("ffmpegexedrv_record(framecount:%lu, audiocount:%lu frametime:%f, audiotime:%f)",
        framecounter, audio_input_counter, frametime, audiotime));
#ifdef HAVE_FFMPEG
    get_resource_values();
#endif
    /* log_resource_values(__FUNCTION__); */

    framecounter++;

    if (video_halve_framerate && (framecounter & 1)) {
        /* drop every second frame */
        return 0;
    }

    if (video_halve_framerate) {
        frametime /= 2;
    }

    /* the video is ahead */
    if (frametime > (audiotime + (time_base * 1.5f))) {
        /* drop one frame */
        framecounter--;
        DBG(("video is ahead, dropping a frame (framecount:%lu, audiocount:%lu frametime:%f, audiotime:%f)",
            framecounter, audio_input_counter, frametime, audiotime));
        return 0;
    }

    /*DBGFRAMES(("ffmpegexedrv_record (%u)", framecounter));*/
    video_fill_rgb_image(screenshot, video_st_frame);

    write_video_frame(video_st_frame);

    /* the video is late */
    if (frametime < (audiotime - (time_base * 1.5f))) {
        /* insert one frame */
        framecounter++;
        DBG(("video is late, inserting a frame (framecount:%lu, audiocount:%lu frametime:%f, audiotime:%f)",
            framecounter, audio_input_counter, frametime, audiotime));
        write_video_frame(video_st_frame);
    }
    return 0;
}

/* Driver API gfxoutputdrv_t.write */
static int ffmpegexedrv_write(screenshot_t *screenshot)
{
    DBG(("ffmpegexedrv_write"));
    return 0;
}

/******************************************************************************/

static gfxoutputdrv_t ffmpegexe_drv = {
    "FFMPEGEXE",
    "FFMPEG (Executable)",
    NULL,
    NULL, /* filled in get_formats_and_codecs */
    NULL, /* open */
    ffmpegexedrv_close,
    ffmpegexedrv_write,
    ffmpegexedrv_save,
    NULL,
    ffmpegexedrv_record,
    ffmpegexedrv_shutdown,
    ffmpegexedrv_resources_init,
    ffmpegexedrv_cmdline_options_init
#ifdef FEATURE_CPUMEMHISTORY
    , NULL
#endif
};

/* gfxoutputdrv_t.shutdown */
static void ffmpegexedrv_shutdown(void)
{
#ifndef HAVE_FFMPEG
    int i = 0;
#endif
    DBG(("ffmpegexedrv_shutdown"));
#ifndef HAVE_FFMPEG
    if (ffmpegexe_drv.formatlist != NULL) {

        while (ffmpegexe_drv.formatlist[i].name != NULL) {
            lib_free(ffmpegexe_drv.formatlist[i].name);
            if (ffmpegexe_drv.formatlist[i].audio_codecs != NULL) {
                lib_free(ffmpegexe_drv.formatlist[i].audio_codecs);
            }
            if (ffmpegexe_drv.formatlist[i].video_codecs != NULL) {
                lib_free(ffmpegexe_drv.formatlist[i].video_codecs);
            }
            i++;
        }
        if (ffmpegexe_drv.formatlist) {
            lib_free(ffmpegexe_drv.formatlist);
            ffmpegexe_drv.formatlist = NULL;
        }
    }
/* ??? this is actually ffmpegexe_drv.formatlist
    if (ffmpegexedrv_formatlist) {
        lib_free(ffmpegexedrv_formatlist);
    }
*/
#endif

    if (ffmpegexe_format) {
        lib_free(ffmpegexe_format);
        ffmpegexe_format = NULL;
    }
}

/* FIXME: when the regular FFMPEG driver was removed, this should interrogate
          the ffmpeg binary and list all available formats and codecs */
#ifndef HAVE_FFMPEG
static void get_formats_and_codecs(void)
{
    int i, j, ai = 0, vi = 0, f;
    gfxoutputdrv_codec_t *audio_codec_list;
    gfxoutputdrv_codec_t *video_codec_list;
    gfxoutputdrv_codec_t *ac, *vc;

    f = 0;
    ffmpegexedrv_formatlist = lib_malloc(sizeof(gfxoutputdrv_format_t));

    for (i = 0; output_formats_to_test[i].name != NULL; i++) {
            audio_codec_list = NULL;
            video_codec_list = NULL;
            if (output_formats_to_test[i].audio_codecs != NULL) {
                ai = 0;
                audio_codec_list = lib_malloc(sizeof(gfxoutputdrv_codec_t));
                ac = output_formats_to_test[i].audio_codecs;
                for (j = 0; ac[j].name != NULL; j++) {
                    DBG(("audio_codec_list[%d].name='%s'\n", ai, ac[j].name));
                    audio_codec_list[ai++] = ac[j];
                    audio_codec_list = lib_realloc(audio_codec_list, (ai + 1) * sizeof(gfxoutputdrv_codec_t));
                }
                audio_codec_list[ai].name = NULL;
            }
            if (output_formats_to_test[i].video_codecs != NULL) {
                vi = 0;
                video_codec_list = lib_malloc(sizeof(gfxoutputdrv_codec_t));
                vc = output_formats_to_test[i].video_codecs;
                for (j = 0; vc[j].name != NULL; j++) {
                    DBG(("video_codec_list[%d].name='%s'", vi, output_formats_to_test[i].video_codecs[j].name));
                    video_codec_list[vi++] = output_formats_to_test[i].video_codecs[j];
                    video_codec_list = lib_realloc(video_codec_list, (vi + 1) * sizeof(gfxoutputdrv_codec_t));
                }
                video_codec_list[vi].name = NULL;
            }
            if (((audio_codec_list == NULL) || (ai > 0)) && ((video_codec_list == NULL) || (vi > 0))) {
                ffmpegexedrv_formatlist[f].name = lib_strdup(output_formats_to_test[i].name);
                DBG(("ffmpegexedrv_formatlist[%d].name='%s'", f, ffmpegexedrv_formatlist[f].name));
                ffmpegexedrv_formatlist[f].audio_codecs = audio_codec_list;
                ffmpegexedrv_formatlist[f++].video_codecs = video_codec_list;
                ffmpegexedrv_formatlist = lib_realloc(ffmpegexedrv_formatlist, (f + 1) * sizeof(gfxoutputdrv_format_t));
            }
    }
    ffmpegexedrv_formatlist[f].name = NULL;
    ffmpegexe_drv.formatlist = ffmpegexedrv_formatlist;
}
#endif

/* public, init this output driver */
void gfxoutput_init_ffmpegexe(int help)
{
    if (help) {
        gfxoutput_register(&ffmpegexe_drv);
        return;
    }

#ifdef HAVE_FFMPEG
    /* FIXME: as long as we coexist with the old FFMPEG driver, we reuse its formatlist */
    ffmpegexe_drv.formatlist = ffmpegdrv_formatlist;
#else
    get_formats_and_codecs();
#endif
    gfxoutput_register(&ffmpegexe_drv);
}
