/*
 * mpegdrv.c - Movie driver using FFMPEG library and screenshot API.
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
#include <avcodec.h>
#include <avformat.h>

#include "archdep.h"
#include "gfxoutput.h"
#include "lib.h"
#include "log.h"
#include "mpegdrv.h"
#include "palette.h"
#include "resources.h"
#include "screenshot.h"
#include "utils.h"

/* general */
static AVFormatContext *mpegdrv_oc;
static AVOutputFormat *mpegdrv_fmt;
static int file_init_done;

/* audio */
static AVStream *audio_st;
static mpegdrv_audio_in_t mpegdrv_audio_in;
static int audio_init_done;
static unsigned char *audio_outbuf;
static int audio_outbuf_size;
double audio_pts;

/* video */
static AVStream *video_st;
static int video_init_done;
static AVFrame *picture, *tmp_picture;
static unsigned char *video_outbuf;
static int video_outbuf_size;
static AVFrame *picture, *tmp_picture;
double video_pts;



static int mpegdrv_init_file(void);


/*-----------------------*/
/* audio stream encoding */
/*-----------------------*/
static int mpegdrv_open_audio(AVFormatContext *oc, AVStream *st)
{
    AVCodecContext *c;
    AVCodec *codec;
    int audio_inbuf_samples;

    c = &st->codec;

    /* find the audio encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        log_debug("mpegdrv: audio codec not found");
        return -1;
    }

    /* open it */
    if (avcodec_open(c, codec) < 0) {
        log_debug("mpegdrv: could not open audio codec");
        return -1;
    }

    audio_outbuf_size = 10000;
    audio_outbuf = (unsigned char*) lib_malloc(audio_outbuf_size);

    /* ugly hack for PCM codecs (will be removed ASAP with new PCM
       support to compute the input frame size in samples */
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
    mpegdrv_audio_in.buffersamples = audio_inbuf_samples;
    mpegdrv_audio_in.buffer = (SWORD*)lib_malloc(audio_inbuf_samples * sizeof(SWORD));

    return 0;
}


static void mpegdrv_close_audio(void)
{
    if (audio_st == NULL)
        return;
    
    avcodec_close(&audio_st->codec);
    lib_free(mpegdrv_audio_in.buffer);
    mpegdrv_audio_in.buffer = NULL;
    mpegdrv_audio_in.buffersamples = 0;
    lib_free(audio_outbuf);
}


void mpegdrv_init_audio(int speed, int channels, mpegdrv_audio_in_t** audio_in)
{
    AVCodecContext *c;
    AVStream *st;

    if (mpegdrv_oc == NULL || mpegdrv_fmt == NULL)
        return;

    audio_init_done = 1;

    if (mpegdrv_fmt->audio_codec == CODEC_ID_NONE)
        return;

    *audio_in = &mpegdrv_audio_in;
    
    (*audio_in)->buffersamples = 0; /* not allocated yet */
    (*audio_in)->used = 0;

    st = av_new_stream(mpegdrv_oc, 1);
    if (!st) {
        log_debug("mpegdrv: Could not alloc audio stream\n");
        return;
    }

    c = &st->codec;
    c->codec_id = mpegdrv_fmt->audio_codec;
    c->codec_type = CODEC_TYPE_AUDIO;

    /* put sample parameters */
    c->bit_rate = 64000; /* FIXME: constant for now */
    c->sample_rate = speed;
    c->channels = channels;
    audio_st = st;
    audio_pts = 0;

    if (video_init_done)
        mpegdrv_init_file();
}


/* triggered by soundmovie->write */
void mpegdrv_encode_audio(mpegdrv_audio_in_t *audio_in)
{
    int out_size;
    
    out_size = avcodec_encode_audio(&audio_st->codec, audio_outbuf,
                                         audio_outbuf_size, audio_in->buffer);

    /* FIXME: Some sync needed ?? */
    
    if (av_write_frame(mpegdrv_oc, audio_st->index, 
                       audio_outbuf, out_size) != 0)
        log_debug("mpegdrv_encode_audio: Error while writing audio frame");

    audio_pts = (double)audio_st->pts.val * mpegdrv_oc->pts_num 
                / mpegdrv_oc->pts_den;

    audio_in->used = 0;
}


/*-----------------------*/
/* video stream encoding */
/*-----------------------*/
static int mpegdrv_fill_rgb_image(screenshot_t *screenshot, AVFrame *pic)
{ 
    int x, y;
    int colnum;
    int bufferoffset;
    int x_dim = screenshot->width;
    int y_dim = screenshot->height;
    int pix = 0;

    bufferoffset = screenshot->x_offset;

    for (y = 0; y < y_dim; y++) {
        for (x=0; x < x_dim; x++) {
            colnum = screenshot->draw_buffer[bufferoffset + x];
            pic->data[0][pix++] = screenshot->palette->entries[colnum].red;
            pic->data[0][pix++] = screenshot->palette->entries[colnum].green;
            pic->data[0][pix++] = screenshot->palette->entries[colnum].blue;
        }

        bufferoffset += screenshot->draw_buffer_line_size;
    }

    return 0;
}


static AVFrame* mpegdrv_alloc_picture(int pix_fmt, int width, int height)
{
    AVFrame *picture;
    unsigned char *picture_buf;
    int size;
    
    picture = (AVFrame*)lib_malloc(sizeof(AVFrame));

    size = avpicture_get_size(pix_fmt, width, height);
    picture_buf = (unsigned char *)lib_malloc(size);
    if (!picture_buf) {
        lib_free(picture);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf, 
                   pix_fmt, width, height);

    return picture;
}


static int mpegdrv_open_video(AVFormatContext *oc, AVStream *st)
{
    AVCodec *codec;
    AVCodecContext *c;

    c = &st->codec;

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        log_debug("mpegdrv: video codec not found");
        return -1;
    }

    /* open the codec */
    if (avcodec_open(c, codec) < 0) {
        log_debug("mpegdrv: could not open video codec");
        return -1;
    }

    video_outbuf = NULL;
    if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
        /* allocate output buffer */
        /* XXX: API change will be done */
        video_outbuf_size = 200000;
        video_outbuf = (unsigned char*)lib_malloc(video_outbuf_size);
    }

    /* allocate the encoded raw picture */
    picture = mpegdrv_alloc_picture(c->pix_fmt, c->width, c->height);
    if (!picture) {
        log_debug("mpegdrv: could not allocate picture");
        return -1;
    }

    /* if the output format is not RGB24, then a temporary YUV420P
       picture is needed too. It is then converted to the required
       output format */
    tmp_picture = NULL;
    if (c->pix_fmt != PIX_FMT_RGB24) {
        tmp_picture = mpegdrv_alloc_picture(PIX_FMT_RGB24, c->width, c->height);
        if (!tmp_picture) {
            log_debug("mpegdrv: could not allocate temporary picture");
            return -1;
        }
    }
    return 0;
}


static void mpegdrv_close_video(void)
{
    if (video_st != NULL) {
        avcodec_close(&video_st->codec);
        if (video_outbuf != NULL)
            lib_free(video_outbuf);
        lib_free(picture->data[0]);
        lib_free(picture);
        picture = NULL;
        if (tmp_picture) {
            lib_free(tmp_picture->data[0]);
            lib_free(tmp_picture);
            tmp_picture = NULL;
        }
    }
}


static void mpegdrv_init_video(screenshot_t *screenshot)
{
    AVCodecContext *c;
    AVStream *st;

    if (mpegdrv_oc == NULL || mpegdrv_fmt == NULL)
        return;

     video_init_done = 1;

     if (mpegdrv_fmt->video_codec == CODEC_ID_NONE)
        return;

    st = av_new_stream(mpegdrv_oc, 0);
    if (!st) {
        log_debug("mpegdrv: Could not alloc video stream\n");
        return;
    }

    c = &st->codec;
    c->codec_id = mpegdrv_fmt->video_codec;
    c->codec_type = CODEC_TYPE_VIDEO;

    /* put sample parameters */
    c->bit_rate = 800000; /* FIXME: constant for now */
    /* resolution must be a multiple of two */
    c->width = screenshot->width;  
    c->height = screenshot->height;
    /* frames per second */
    c->frame_rate = 50;  /* FIXME */
    c->frame_rate_base = 1;
    c->gop_size = 12; /* emit one intra frame every twelve frames at most */

    video_st = st;
    video_pts = 0;

    if (audio_init_done)
        mpegdrv_init_file();
}


static int mpegdrv_init_file(void)
{
    if (!video_init_done || !audio_init_done)
        return 0;

    if (av_set_parameters(mpegdrv_oc, NULL) < 0) {
        log_debug("mpegdrv: Invalid output format parameters");
            return -1;
    }

    dump_format(mpegdrv_oc, 0, mpegdrv_oc->filename, 1);

    if (video_st && (mpegdrv_open_video(mpegdrv_oc, video_st) < 0))
        return -1;
    if (audio_st && (mpegdrv_open_audio(mpegdrv_oc, audio_st) < 0))
        return -1;

    if (!(mpegdrv_fmt->flags & AVFMT_NOFILE)) {
        if (url_fopen(&mpegdrv_oc->pb, mpegdrv_oc->filename, URL_WRONLY) < 0) {
            log_debug("mpegdrv: Cannot open %s", mpegdrv_oc->filename);
            return -1;
        }
    }

    av_write_header(mpegdrv_oc);

    log_debug("mpegdrv: Initialized file successfully");

    file_init_done = 1;

    return 0;
}


static int mpegdrv_save(screenshot_t *screenshot, const char *filename)
{
    video_st = NULL;
    audio_st = NULL;

    audio_init_done = 0;
    video_init_done = 0;
    file_init_done = 0;

    mpegdrv_fmt = guess_format(NULL, filename, NULL);

    if (!mpegdrv_fmt)
        mpegdrv_fmt = guess_format("mpeg", NULL, NULL);

    if (!mpegdrv_fmt) {
        log_debug("mpegdrv: Cannot find suitable output format");
        return -1;
    }

    mpegdrv_oc = av_mallocz(sizeof(AVFormatContext));
    if (!mpegdrv_oc) {
        log_debug("mpegdrv: Cannot allocate format context");
        return -1;
    }

    mpegdrv_oc->oformat = mpegdrv_fmt;
    snprintf(mpegdrv_oc->filename, sizeof(mpegdrv_oc->filename), "%s", filename);

    mpegdrv_init_video(screenshot);

    resources_set_value("SoundRecordDeviceName", "movie");
    resources_set_value("Sound", (resource_value_t)1);

    return 0;
}


static int mpegdrv_close(screenshot_t *screenshot)
{
    int i;

    if (video_st)
        mpegdrv_close_video();
    if (audio_st)
        mpegdrv_close_audio();

    /* write the trailer, if any */
    av_write_trailer(mpegdrv_oc);
    
    /* free the streams */
    for(i = 0; i < mpegdrv_oc->nb_streams; i++) {
        av_freep(&mpegdrv_oc->streams[i]);
    }

    if (!(mpegdrv_fmt->flags & AVFMT_NOFILE)) {
        /* close the output file */
        url_fclose(&mpegdrv_oc->pb);
    }

    /* free the stream */
    av_free(mpegdrv_oc);
    log_debug("mpegdrv: Closed successfully");

    resources_set_value("SoundRecordDeviceName", "");

    return 0;
}


/* triggered by screenshot_record */
static int mpegdrv_record(screenshot_t *screenshot)
{
    AVCodecContext *c;
    int out_size;
    int ret;

    if (audio_init_done && video_init_done && !file_init_done)
        mpegdrv_init_file();

    if (video_st == NULL || !file_init_done)
        return 0;

    if (audio_st && video_pts > audio_pts) {
        /* drop this frame */
        return 0;
    }

    c = &video_st->codec;

    if (c->pix_fmt != PIX_FMT_RGB24) {
        mpegdrv_fill_rgb_image(screenshot, tmp_picture);
        img_convert((AVPicture *)picture, c->pix_fmt,
                    (AVPicture *)tmp_picture, PIX_FMT_RGB24,
                    c->width, c->height);
    } else {
        mpegdrv_fill_rgb_image(screenshot, picture);
    }

    if (mpegdrv_oc->oformat->flags & AVFMT_RAWPICTURE) {
        /* raw video case. The API will change slightly in the near
           futur for that */
        ret = av_write_frame(mpegdrv_oc, video_st->index, 
                       (unsigned char *)picture, sizeof(AVPicture));
    } else {
        /* encode the image */
        out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
        /* if zero size, it means the image was buffered */
        if (out_size != 0) {
            /* write the compressed frame in the media file */
            /* XXX: in case of B frames, the pts is not yet valid */
            ret = av_write_frame(mpegdrv_oc, video_st->index, video_outbuf, out_size);
        } else {
            ret = 0;
        }
    }
    if (ret != 0) {
        log_debug("Error while writing video frame");
        return -1;
    }

    video_pts = (double)video_st->pts.val * mpegdrv_oc->pts_num 
                    / mpegdrv_oc->pts_den;

    return 0;
}


static int mpegdrv_write(screenshot_t *screenshot)
{
    return 0;
}


static gfxoutputdrv_t mpeg_drv =
{
    "MPEG",
    "mpg",
    NULL,
    mpegdrv_close,
    mpegdrv_write,
    mpegdrv_save,
    mpegdrv_record
};

void gfxoutput_init_mpeg(void)
{
    gfxoutput_register(&mpeg_drv);
    av_register_all();
}

