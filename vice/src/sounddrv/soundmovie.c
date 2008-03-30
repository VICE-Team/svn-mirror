/*
 * soundmovie.c - Implementation of the audio stream for movie encoding
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include "avformat.h"
#include "log.h"
#include "sound.h"
#include "types.h"
#include "utils.h"
#include "archdep.h"

static AVFormatContext *oc;
static AVStream *audio_st, *video_st;
static AVOutputFormat *fmt;
static int16_t *audio_inbuf;
static int last_sample;
static uint8_t *audio_outbuf;
static int audio_outbuf_size;
static int audio_inbuf_samples;

/* audio stream */
static AVStream *add_audio_stream(AVFormatContext *oc, int codec_id, 
                                  int speed, int channels)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 1);
    if (!st) {
        log_debug("soundmovie: Could not alloc stream\n");
        return NULL;
    }

    c = &st->codec;
    c->codec_id = codec_id;
    c->codec_type = CODEC_TYPE_AUDIO;

    /* put sample parameters */
    c->bit_rate = 64000; /* FIXME: constant for now */
    c->sample_rate = speed;
    c->channels = channels;
    return st;
}


int open_audio(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;
    AVCodec *codec;

    c = &st->codec;

    /* find the audio encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        log_debug("soundmovie: audio codec not found");
        return -1;
    }

    /* open it */
    if (avcodec_open(c, codec) < 0) {
        log_debug("soundmovie: could not open audio codec");
        return -1;
    }

    audio_outbuf_size = 10000;
    audio_outbuf = (uint8_t*) lib_malloc(audio_outbuf_size);

    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
       support to compute the input frame size in audio_inbuf */
    if (c->frame_size <= 1) {
        audio_inbuf_samples = audio_outbuf_size;
        switch(st->codec.codec_id) {
        case CODEC_ID_PCM_S16LE:
        case CODEC_ID_PCM_S16BE:
        case CODEC_ID_PCM_U16LE:
        case CODEC_ID_PCM_U16BE:
            audio_inbuf_samples >>= 1;
            break;
        default:
            break;
        }
    } else {
        audio_inbuf_samples = c->frame_size * c->channels;
    }
    audio_inbuf = (int16_t*)lib_malloc(audio_inbuf_samples * sizeof(int16_t));
    last_sample = 0;

    ui_error("audio_inbuf mit groesse %d alloziiert", audio_inbuf_samples);

    return 0;
}


void close_audio(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(&st->codec);
    
    av_free(audio_inbuf);
    av_free(audio_outbuf);
}


/* video stream */
static AVStream *add_video_stream(AVFormatContext *oc, int codec_id)
{
    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 0);
    if (!st) {
        log_debug("soundmovie: Could not alloc stream\n");
        return NULL;
    }
    
    c = &st->codec;
    c->codec_id = codec_id;
    c->codec_type = CODEC_TYPE_VIDEO;

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = 352;  
    c->height = 288;
    /* frames per second */
    c->frame_rate = 50;  
    c->frame_rate_base = 1;
    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
    if (c->codec_id == CODEC_ID_MPEG1VIDEO ||
        c->codec_id == CODEC_ID_MPEG2VIDEO) {
        /* just for testing, we also add B frames */
        c->max_b_frames = 2;
    }
    return st;
}

int open_video(AVFormatContext *oc, AVStream *st)
{
    AVCodec *codec;
    AVCodecContext *c;

    c = &st->codec;

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        log_debug("soundmovie: video codec not found\n");
        return -1;
    }

    /* open the codec */
    if (avcodec_open(c, codec) < 0) {
        log_debug("soundmovie: could not open video codec\n");
        return -1;
    }
#if 0
    video_outbuf = NULL;
    if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
        /* XXX: API change will be done */
        video_outbuf_size = 200000;
        video_outbuf = malloc(video_outbuf_size);
    }

    /* allocate the encoded raw picture */
    picture = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!picture) {
        fprintf(stderr, "Could not allocate picture\n");
        exit(1);
    }

    /* if the output format is not YUV420P, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
    tmp_picture = NULL;
    if (c->pix_fmt != PIX_FMT_YUV420P) {
        tmp_picture = alloc_picture(PIX_FMT_YUV420P, c->width, c->height);
        if (!tmp_picture) {
            fprintf(stderr, "Could not allocate temporary picture\n");
            exit(1);
        }
    }
#endif
    return 0;
}


void close_video(AVFormatContext *oc, AVStream *st)
{
    avcodec_close(&st->codec);
#if 0
    av_free(picture->data[0]);
    av_free(picture);
    if (tmp_picture) {
        av_free(tmp_picture->data[0]);
        av_free(tmp_picture);
    }
    av_free(video_outbuf);
#endif
}


/* the movie driver */
static int movie_init(const char *param, int *speed,
		   int *fragsize, int *fragnr, int *channels)
{
    av_register_all();
    fmt = guess_format(NULL, param, NULL);

    if (!fmt)
        fmt = guess_format("mpeg", NULL, NULL);

    if (!fmt) {
        log_debug("soundmovie: Cannot find suitable output format");
        return -1;
    }

    oc = av_mallocz(sizeof(AVFormatContext));
    if (!oc) {
        log_debug("soundmovie: Cannot allocate format context");
        return -1;
    }

    oc->oformat = fmt;
    snprintf(oc->filename, sizeof(oc->filename), "%s", param);

    video_st = NULL;
    audio_st = NULL;

    if (fmt->video_codec != CODEC_ID_NONE)
        video_st = add_video_stream(oc, fmt->video_codec);

    if (fmt->audio_codec != CODEC_ID_NONE)
        audio_st = add_audio_stream(oc, fmt->audio_codec, *speed, *channels);

    if (av_set_parameters(oc, NULL) < 0) {
        log_debug("soundmovie: Invalid output format parameters");
            return -1;
    }

    dump_format(oc, 0, param, 1);

    if (video_st && (open_video(oc, video_st) < 0))
        return -1;
    if (audio_st && (open_audio(oc, audio_st) < 0))
        return -1;

    if (!(fmt->flags & AVFMT_NOFILE)) {
        if (url_fopen(&oc->pb, param, URL_WRONLY) < 0) {
            log_debug("soundmovie: Cannot open %s", param);
            return -1;
        }
    }

    av_write_header(oc);

    log_debug("soundmovie: Initialized successfully");
    return 0;
}

static int movie_write(SWORD *pbuf, size_t nr)
{
    /* audio stream */
    size_t copied = 0;
    int out_size;

    while (copied < nr) {
        int samples_to_copy = audio_inbuf_samples - last_sample;
        if (samples_to_copy > nr - copied)
            samples_to_copy = nr - copied;
        memcpy(audio_inbuf + last_sample, pbuf + copied,
                samples_to_copy * sizeof(SWORD));
        last_sample += samples_to_copy;
        copied += samples_to_copy;
        if (last_sample >= audio_inbuf_samples) {
            last_sample = 0;
            /* encode here */
            out_size = avcodec_encode_audio(&audio_st->codec, audio_outbuf, 
                                            audio_outbuf_size, audio_inbuf);

            /* write the compressed frame in the media file */
            if (av_write_frame(oc, audio_st->index, audio_outbuf, out_size) != 0) {
                log_debug("soundmovie: Error while writing audio frame");
                return -1;
            }
        }
    }

    return 0;
}

static void movie_close(void)
{
    int i;

    if (video_st)
        close_video(oc, video_st);
    if (audio_st)
        close_audio(oc, audio_st);

    /* write the trailer, if any */
    av_write_trailer(oc);
    
    /* free the streams */
    for(i = 0; i < oc->nb_streams; i++) {
        av_freep(&oc->streams[i]);
    }

    if (!(fmt->flags & AVFMT_NOFILE)) {
        /* close the output file */
        url_fclose(&oc->pb);
    }

    /* free the stream */
    av_free(oc);
    log_debug("soundmovie: Closed successfully");
}

static sound_device_t movie_device =
{
    "movie",
    movie_init,
    movie_write,
    NULL,
    NULL,
    NULL,
    movie_close,
    NULL,
    NULL,
    0
};

int sound_init_movie_device(void)
{
    return sound_register_device(&movie_device);
}

