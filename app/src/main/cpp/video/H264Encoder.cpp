//
// Created by fengbo on 2018/5/9.
//
#include "H264Encoder.h"

using namespace libyuv;

H264Encoder::H264Encoder(VideoConfig *config) {
    LOGI("new h264 encoder");
    recordConfig = config;
}

int H264Encoder::initH264Encoder() {
    LOGI("start init H264 Encoder");

//    size_t path_length = strlen(recordConfig->video_path);
//
//    char *out_file = (char *) malloc(path_length + 1);
//
//    strcpy(out_file, recordConfig->video_path);


//    pFormatCtx = avformat_alloc_context();
//
//    pOutFormat = av_guess_format(NULL, out_file, NULL);
//
//    pFormatCtx->oformat = pOutFormat;
//
//    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
//        LOGE("Failed to open video output file!");
//        return -1;
//    }
//
//    video_st =avformat_new_stream(pFormatCtx, 0);
//
//    if (video_st == NULL) {
//        LOGE("allocate video stream fail");
//        return -1;
//    }
//
//    av_dump_format(pFormatCtx, 0, out_file, 1);


    av_register_all();

    //指定libx264进行编码
    pCodec = avcodec_find_encoder_by_name("libx264");

    if (!pCodec) {
        LOGE("error find video encoder");
        return -1;
    } else {
        LOGI("success find video encoder");
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);

    pCodecCtx->codec_id = AV_CODEC_ID_H264;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    //Android 摄像头会有旋转
    if (recordConfig->rotate_type == recordConfig->CONST_ROTATE_0
        || recordConfig->rotate_type ==  recordConfig->CONST_ROTATE_180) {
        pCodecCtx->width = recordConfig->video_out_width;
        pCodecCtx->height = recordConfig->video_out_height;
    } else {
        pCodecCtx->width = recordConfig->video_out_height;
        pCodecCtx->height = recordConfig->video_out_width;
    }

    pCodecCtx->gop_size = 50;
    pCodecCtx->thread_count = 12;
    pCodecCtx->max_b_frames = 0; //最大B帧个数
    pCodecCtx->qmin = 10;
    pCodecCtx->qmax = 50;
    pCodecCtx->bit_rate = 100 * 1024 * 8;
    pCodecCtx->framerate = {25, 1};

//    pCodecCtx->level = 41;
//    pCodecCtx->refs = 1;
//    pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //全局参数
//    pCodecCtx->bit_rate = recordConfig->video_bit_rate;
//    pCodecCtx->time_base.num = 1;
//    pCodecCtx->time_base.den = recordConfig->video_frame_rate;
    pCodecCtx->time_base = {1, 1000000};//AUDIO VIDEO 两边时间基数要相同

    AVDictionary *params = 0;
    av_dict_set(&params, "preset" , "ultrafast" , 0);
    av_dict_set(&params, "profile", "baseline"  , 0);

    int state = avcodec_open2(pCodecCtx, pCodec, &params);

    if (state < 0) {
        LOGE("error open video encoder ret = %d", state);
        return -1;
    } else {
        LOGI("success open video encoder");
    }

    pFrame = av_frame_alloc();
    is_end = 0;

//    picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width,pCodecCtx->height);
//    LOGI("picture size : %d", picture_size);
//    uint8_t *buf = (uint8_t *) av_malloc(picture_size);
//    avpicture_fill((AVPicture *) pFrame, buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
//    avformat_write_header(pFormatCtx, NULL);
//    av_new_packet(&pkt, picture_size);
//    out_y_size = pCodecCtx->width * pCodecCtx->height;
//    in_frame_size = recordConfig->video_in_height * recordConfig->video_in_width;
//    in_frame_size_bits = in_frame_size * 3 / 2; //NV21 YUV420P 1.5B
//    pthread_create(&thread, NULL, H264Encoder::startLoopEncodeInThread, this);
    LOGI("video encoder init finish");
    return 0;
}

int H264Encoder::pushOneFrame(uint8_t *frame, int length) {
    //YUV存储格式决定
    uint8_t *new_buf = (uint8_t *) malloc(length);
    memcpy(new_buf, frame, length);

    int w = recordConfig->video_in_width;
    int h = recordConfig->video_in_height;

    uint8_t *dstI420 = (uint8_t *) malloc(w * h * 3 / 2);

    NV21ToI420(new_buf,                      w,
               new_buf + (w * h),            w,
               dstI420,                      w,
               dstI420 + (w * h),            w / 2,
               dstI420 + (w * h * 5 / 4),    w / 2,
               w, h);

    if (new_buf) {
        delete(new_buf);
    }

    uint8_t *rotate = (uint8_t *) malloc(w * h * 3 / 2);

    I420Rotate(dstI420,                      w,
               dstI420 + w * h,              w / 2,
               dstI420 + (w * h * 5 / 4),    w / 2,
               rotate,                       h,
               rotate + (w * h),             h / 2,
               rotate + (w * h * 5 / 4),     h / 2,
               w, h,
               kRotate270);

    if (dstI420) {
        delete(dstI420);
    }

    //TODO 镜像计算错误
    uint8_t *mirror = (uint8_t *) malloc(w * h * 3 / 2);

    I420Mirror(rotate,                      w,
               rotate + w * h,              w / 2,
               rotate + (w * h * 5 / 4),    w / 2,
               mirror,                      h,
               mirror + (w * h),            h / 2,
               mirror + (w * h * 5 / 4),    h / 2,
               w, h);

    FrameData *frameData = new FrameData();
    frameData->data = rotate;
    frameData->pts = av_gettime();
    frame_queue.push(frameData);

//    if (rotate) {
//        delete(rotate);
//    }

    if (mirror) {
        delete(mirror);
    }

    return 0;
}

void* H264Encoder::startLoopEncodeInThread(void *obj) {
//    LOGI("start encode video");
//
//    H264Encoder *h264Encoder = (H264Encoder *) obj;
//    while (!h264Encoder->is_end || !h264Encoder->frame_queue.empty()) {
//        if (h264Encoder->frame_queue.empty()) {
//            continue;
//        }
//
//        uint8_t *picture_buf = *h264Encoder->frame_queue.wait_and_pop().get();
//
//        h264Encoder->custom_filter(h264Encoder, picture_buf, h264Encoder->in_frame_size, h264Encoder->recordConfig->v_custom_format);
//
//        //TODO 这里不太懂
//        h264Encoder->pFrame->pts = h264Encoder->pts;
//        h264Encoder->pts++;
//
//        int got_frame = 0;
//
//        int ret = avcodec_encode_video2(h264Encoder->pCodecCtx, &h264Encoder->pkt, h264Encoder->pFrame, &got_frame);
//
//        if (ret < 0) {
//            LOGE("Fail encode video!");
//        }
//
////        if (got_frame == 1) {
////            h264Encoder->pkt.stream_index = h264Encoder->video_st->index;
////            h264Encoder->frameCnt++;
////            ret = av_write_frame(h264Encoder->pFormatCtx, &h264Encoder->pkt);
////            av_free_packet(&h264Encoder->pkt);
////        }
//        delete(picture_buf);
//    }
//
//    if (h264Encoder->is_end) {
//        LOGI("END encoding");
//        h264Encoder->endEncoder();
//        delete h264Encoder;
//    }
    return 0;
}

int H264Encoder::startEncodeForRTMP(FrameData *data) {
    std::lock_guard<std::mutex> lk(mut);

    int ret = 0;

    av_image_fill_arrays(pFrame->data,
                         pFrame->linesize, data->data,
                         AV_PIX_FMT_YUV420P, pCodecCtx->width,
                         pCodecCtx->height, 1);

    pFrame->pts = data->pts;

//    pFrame->pts = pts;
//    pts++;

    int got_frame = 0;

    ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_frame);

    if (ret < 0) {
        LOGE("Fail encode video!");
    }

    if (data->data) {
        delete(data->data);
    }

    data->avPacket = &pkt;
    return pkt.size;
}

void H264Encoder::custom_filter(const H264Encoder *h264Encoder, const uint8_t *picture_buf, int in_y_size,
                                int format) {
    //   y值在H方向开始行
    int y_height_start_index = h264Encoder->recordConfig->video_in_height - h264Encoder->recordConfig->video_out_height;
    //   uv值在H方向开始行
    int uv_height_start_index = y_height_start_index / 2;


    //只支持旋转90度
    for (int i = y_height_start_index; i < h264Encoder->recordConfig->video_in_height; i++) {

        for (int j = 0; j < h264Encoder->recordConfig->video_out_width; j++) {

            int index = h264Encoder->recordConfig->video_in_width * i + j;
            uint8_t value = *(picture_buf + index);
            *(h264Encoder->pFrame->data[0] + j * h264Encoder->recordConfig->video_out_height +
              (h264Encoder->recordConfig->video_out_height - (i - y_height_start_index) - 1)) = value;
        }
    }

    for (int i = uv_height_start_index; i < h264Encoder->recordConfig->video_in_height / 2; i++) {
        for (int j = 0; j < h264Encoder->recordConfig->video_out_width / 2; j++) {
            int index = h264Encoder->recordConfig->video_in_width / 2 * i + j;
            uint8_t v = *(picture_buf + in_y_size + index);
            uint8_t u = *(picture_buf + in_y_size * 5 / 4 + index);
            *(h264Encoder->pFrame->data[2] + (j * h264Encoder->recordConfig->video_out_height / 2 +
                                              (h264Encoder->recordConfig->video_out_height / 2 -
                                               (i - uv_height_start_index) -
                                               1))) = v;
            *(h264Encoder->pFrame->data[1] + (j * h264Encoder->recordConfig->video_out_height / 2 +
                                              (h264Encoder->recordConfig->video_out_height / 2 -
                                               (i - uv_height_start_index) -
                                               1))) = u;
        }
    }
    return;
}

void H264Encoder::userStop() {
    is_end = 1;
}

void H264Encoder::endEncoder() {
//    int ret = flush_encoder(pFormatCtx, 0);
//    if (ret < 0) {
//        LOGE("flush video encoder fail!");
//        return;
//    }
//    av_write_trailer(pFormatCtx);
//
//    if (video_st) {
//        avcodec_close(pCodecCtx);
//        av_free(pFrame);
//    }
//
//    avio_close(pFormatCtx->pb);
//    avformat_free_context(pFormatCtx);
//    LOGI("h264 encoder encode finish");
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