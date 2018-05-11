//
// Created by fengbo on 2018/5/9.
//

extern "C" {
#include "libavutil/opt.h"
#include "libavformat/avformat.h"
}
#include "H264Encoder.h"
#include "LogUtils.h"

H264Encoder::H264Encoder(RecordConfig *config) {
    LOGI("new h264 encoder");
    recordConfig = config;
}

int H264Encoder::initH264Encoder() {

    LOGI("start init H264 Encoder");

    size_t path_length = strlen(recordConfig->video_path);

    char *out_file = (char *) malloc(path_length + 1);

    strcpy(out_file, recordConfig->video_path);

    av_register_all();

    pFormatCtx = avformat_alloc_context();

    pOutFormat = av_guess_format(NULL, out_file, NULL);

    pFormatCtx->oformat = pOutFormat;

    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
        LOGE("Failed to open video output file!");
        return -1;
    }

    video_st =avformat_new_stream(pFormatCtx, 0);

    if (video_st == NULL) {
        LOGE("allocate video stream fail");
        return -1;
    }

    av_dump_format(pFormatCtx, 0, out_file, 1);

    //指定libx264进行编码
    pCodec = avcodec_find_encoder_by_name("lib_x264");

    if (!pCodec) {
        LOGE("error find video encoder");
        return -1;
    } else {
        LOGI("success find video encoder");
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    pCodecCtx->codec_id = AV_CODEC_ID_H264;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P; //TODO 确认android手机是否支持这个

    if (recordConfig->rotate_type == recordConfig->CONST_ROTATE_90
        || recordConfig->rotate_type ==  recordConfig->CONST_ROTATE_180) {
        pCodecCtx->width = recordConfig->video_in_height;
        pCodecCtx->height = recordConfig->video_out_width;
    } else {
        pCodecCtx->width = recordConfig->video_out_width;
        pCodecCtx->height = recordConfig->video_in_height;
    }

    pCodecCtx->bit_rate = recordConfig->video_bit_rate;
    pCodecCtx->gop_size = 250;

    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = recordConfig->video_frame_rate;

    pCodecCtx->qmin = 10; //TODO 搞清楚这些字段的含义
    pCodecCtx->qmax = 51;

//    pCodecCtx->max_b_frames = 3; //最大B帧个数

    AVDictionary *params = 0;

    av_opt_set(pCodecCtx->priv_data, "preset", "ultrafast", 0);

    av_dict_set(&params, "profile", "baseline", 0);

    int state = avcodec_open2(pCodecCtx, pCodec, NULL);

    if (state < 0) {
        LOGE("error open video encoder ret = %d", state);
        return -1;
    } else {
        LOGI("success open video encoder");
    }

    pFrame = av_frame_alloc();

    picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width,
    pCodecCtx->height);

    LOGI("picture size : %d", picture_size);

    uint8_t *buf = (uint8_t *) av_malloc(picture_size);

    avpicture_fill((AVPicture *) pFrame, buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

    avformat_write_header(pFormatCtx, NULL);

    av_new_packet(&pkt, picture_size);

    out_y_size = pCodecCtx->width * pCodecCtx->height;

    in_frame_size = recordConfig->video_in_height * recordConfig->video_in_width;

    in_frame_size_bits = in_frame_size * 3 /2;

    is_end = 0;
    pthread_t thread;
    pthread_create(&thread, NULL, H264Encoder::startEncode, this);
    LOGI("video encoder init finish");
    return 0;
}

int H264Encoder::pushOneFrame(uint8_t *frame) {
    //YUV存储格式决定
    uint8_t *new_buf = (uint8_t *) malloc(in_frame_size_bits);
    memcpy(new_buf, frame, in_frame_size_bits);
    frame_queue.push(new_buf);
    return 0;
}

void* H264Encoder::startEncode(void *obj) {
    LOGI("start encode video");

    H264Encoder *h264Encoder = (H264Encoder *) obj;
    while (!h264Encoder->is_end || !h264Encoder->frame_queue.empty()) {
        if (h264Encoder->frame_queue.empty()) {
            continue;
        }

        uint8_t *picture_buf = *h264Encoder->frame_queue.wait_and_pop().get();

        h264Encoder->custom_filter(h264Encoder, picture_buf, h264Encoder->in_frame_size, h264Encoder->recordConfig->v_custom_format);

        //TODO 这里不太懂
        h264Encoder->pFrame->pts = h264Encoder->pts;
        h264Encoder->pts++;

        int got_frame = 0;

        int ret = avcodec_encode_video2(h264Encoder->pCodecCtx, &h264Encoder->pkt, h264Encoder->pFrame, &got_frame);

        if (ret < 0) {
            LOGE("Fail encode video!");
        }

        if (got_frame == 1) {
            h264Encoder->pkt.stream_index = h264Encoder->video_st->index;
            h264Encoder->frameCnt++;
            ret = av_write_frame(h264Encoder->pFormatCtx, &h264Encoder->pkt);
            av_free_packet(&h264Encoder->pkt);
        }
        delete(picture_buf);
    }

    if (h264Encoder->is_end) {
        LOGI("END encoding");
        h264Encoder->endEncoder();
        delete h264Encoder;
    }
    return 0;
}

void H264Encoder::custom_filter(const H264Encoder *h264Encoder, const uint8_t *picture_buf, int in_y_size,
                                int format) {
    //TODO
    return;
}

void H264Encoder::userStop() {
    is_end = 1;
}

void H264Encoder::endEncoder() {
    int ret = flush_encoder(pFormatCtx, 0);
    if (ret < 0) {
        LOGE("flush video encoder fail!");
        return;
    }
    av_write_trailer(pFormatCtx);

    if (video_st) {
        avcodec_close(pCodecCtx);
        av_free(pFrame);
    }

    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);
    LOGI("h264 encoder encode finish");
}

int H264Encoder::flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index) {

    int ret;
    int got_frame;
    AVPacket end_pkt;

    if (!(pCodecCtx->codec->capabilities & CODEC_CAP_DELAY)) {
        LOGE("!CODEC_CAP_DELAY");
        return 0;
    } else {
        LOGI("CODEC_CAP_DELAY");
    }

    while(true) {
        end_pkt.data = NULL;
        end_pkt.size = 0;

        av_init_packet(&end_pkt);

        ret = avcodec_encode_video2(pCodecCtx, &end_pkt, NULL, &got_frame);

//        av_frame_free(NULL);

        if (ret < 0) {
            break;
        }

        if (!got_frame) {
            ret = 0;
            break;
        }

        LOGI("Flush encode size %d", end_pkt.size);

        ret = av_write_frame(fmt_ctx, &end_pkt);

        if (ret < 0) {
            break;
        }
    }
    return ret;
}