//
// Created by fengbo on 2018/5/9.
//

#include "AACEncoder.h"
#include "LogUtils.h"

using namespace std;

/**
 * 初始化函数
 * @param config
 */
AACEncoder::AACEncoder(RecordConfig *config) {
    LOGI("new aac encoder");
    recordConfig = config;
}

int AACEncoder::initAAEncoder() {
    LOGI("start init AACEncoder ");

    size_t path_length = strlen(recordConfig->audio_path);

    char* outfile = (char * )malloc(path_length + 1);
    strcpy(outfile, recordConfig->audio_path);

    av_register_all();

    /***TEST  查看当前支持的编解码器***/
//    AVCodec *c_temp = av_codec_next(NULL);
//
//    while (c_temp != NULL)
//    {
//        switch (c_temp->type)
//        {
//            case AVMEDIA_TYPE_VIDEO:
//                LOGI("%s %10s\n", "[Video]", c_temp->name);
//
//                break;
//            case AVMEDIA_TYPE_AUDIO:
//                LOGI("%s %10s\n", "[Audeo]", c_temp->name);
//                break;
//            default:
//                LOGI("%s %10s\n", "[Other]", c_temp->name);
//                break;
//        }
//        c_temp = c_temp->next;
//    }
    /********/

    pFormatCtx = avformat_alloc_context();
    fmt = av_guess_format(NULL, outfile, NULL);
    pFormatCtx->oformat = fmt;

    if (avio_open(&pFormatCtx->pb, outfile, AVIO_FLAG_READ_WRITE) < 0) {
        LOGE("Failed to open audio output file!");
        return -1;
    }

    audio_st = avformat_new_stream(pFormatCtx, 0);

    if (audio_st == NULL) {
        LOGE("allocate audio stream fail");
        return -1;
    }

    av_dump_format(pFormatCtx, 0, outfile, 1);

//    pCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);

    //指定libfdk_aac进行编码
    pCodec = avcodec_find_encoder_by_name("libfdk_aac");
    if (!pCodec) {
        LOGE("error find audio encoder");
        return -1;
    } else {
        LOGI("success find audio encoder");
    }

    /**
    * ffmpeg默认的aac编码器不能指定为AV_SAMPLE_FMT_S16，
    * 如果 使用avcodec_find_encoder(AV_CODEC_ID_AAC)指定AVCodec，会识别为默认的aac编码器，会avcodec_open2返回-22
    * 所以需要使用avcodec_find_encoder_by_name("libfdk_aac")指定为fdk_aac编码器
    * 如果使用ffmepg默认的aac编码器，可以使用SwrContext将AV_SAMPLE_FMT_FLTP转换成AV_SAMPLE_FMT_S16
    */

    pCodecCtx = avcodec_alloc_context3(pCodec); //或者使用pCodecCtx = audio_st->codec;也可以
    pCodecCtx->codec_id = AV_CODEC_ID_AAC;
    pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    pCodecCtx->sample_rate = 44100;
    pCodecCtx->channel_layout = AV_CH_LAYOUT_MONO;
    pCodecCtx->channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_MONO);
//    pCodecCtx->thread_count = 2;
//    pCodecCtx->bit_rate = recordConfig->audio_bit_rate;
    pCodecCtx->bit_rate = 64000;

    LOGI("channels : %d ", pCodecCtx->channels);
    int state = avcodec_open2(pCodecCtx, pCodec, NULL);

    if (state < 0) {
        LOGE("error open audio encoder ret = %d", state);
        return -1;
    } else {
        LOGI("success open audio encoder");
    }

    pFrame = av_frame_alloc();
    pFrame->nb_samples = pCodecCtx->frame_size;
    pFrame->format = pCodecCtx->sample_fmt;

    frame_size = av_samples_get_buffer_size(NULL, pCodecCtx->channels, pCodecCtx->frame_size, pCodecCtx->sample_fmt, 1);

    LOGI("frame_size %d", frame_size);
    uint8_t *frame_buf = (uint8_t *) av_malloc(frame_size);
    avcodec_fill_audio_frame(pFrame,pCodecCtx->channels, pCodecCtx->sample_fmt, (const uint8_t *) frame_buf, frame_size, 1);

    avformat_write_header(pFormatCtx, NULL);
    av_new_packet(&pkt, frame_size);
    is_end = 0;

    pthread_t thread;
    pthread_create(&thread, NULL, AACEncoder::startEncode, this);
    LOGI("audio encoder init finish");
    return 0;
}

int AACEncoder::flush_encoder(AVFormatContext *fmt_context, unsigned int stream_index) {

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

        ret = avcodec_encode_audio2(pCodecCtx, &end_pkt, NULL, &got_frame);

//        av_frame_free(NULL);

        if (ret < 0) {
            break;
        }

        if (!got_frame) {
            ret = 0;
            break;
        }

        LOGI("Flush encode size %d", end_pkt.size);

        ret = av_write_frame(fmt_context, &end_pkt);

        if (ret < 0) {
            break;
        }
    }
    return ret;
}

void AACEncoder::userStop() {
    is_end = 1;
}

void AACEncoder::endEncoder() {
    int ret = flush_encoder(pFormatCtx, 0);
    if (ret < 0) {
        LOGE("flush audio encoder fail!");
        return;
    }
    av_write_trailer(pFormatCtx);

    if (audio_st) {
        avcodec_close(pCodecCtx);
        av_free(pFrame);
    }

    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);
    LOGI("aac encoder encode finish");
    //TODO 通知muxer
}

int AACEncoder::pushOneFrame(uint8_t *frame) {
    uint8_t *new_buf = (uint8_t *) malloc(frame_size);
    memcpy(new_buf, frame, frame_size);
    frame_queue.push(new_buf);
    return 0;
}

void *AACEncoder::startEncode(void *obj) {
    LOGI("start encode audio");
    AACEncoder *aac_encoder = (AACEncoder *)obj;
    //需要两个判断条件，外置的isend标志位，有时队列可能空，而我们并没有想要停止encode
    while (!aac_encoder->is_end || !aac_encoder->frame_queue.empty()) {
        if (aac_encoder->frame_queue.empty()) {
//            LOGI("frame_queue empty... ");
            continue;
        }

        uint8_t *framebuf = *aac_encoder->frame_queue.wait_and_pop().get();

        aac_encoder->pFrame->data[0] = framebuf;
        aac_encoder->pFrame->pts = aac_encoder->pts; //Presentation timestamp in time_base units (time when frame should be shown to user).
        aac_encoder->pts++;

        LOGI("frame_queue pts : %d" , aac_encoder->pts);

        int got_frame = 0;

        int ret = avcodec_encode_audio2(aac_encoder->pCodecCtx, &aac_encoder->pkt, aac_encoder->pFrame, &got_frame);

        if (ret < 0) {
            LOGE("Fail encode audio!");
        }

        if (got_frame == 1) {
            aac_encoder->pkt.stream_index = aac_encoder->audio_st->index;
            ret = av_write_frame(aac_encoder->pFormatCtx, &aac_encoder->pkt);
            av_free_packet(&aac_encoder->pkt);
        }
        delete(framebuf);
    }

    if (aac_encoder->is_end) {
        LOGI("END encoding");
        aac_encoder->endEncoder();
        delete aac_encoder;
    }

    return 0;
}

/**
 * 析构
 */
AACEncoder::~AACEncoder() {

}

