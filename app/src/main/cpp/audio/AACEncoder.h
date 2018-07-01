//
// Created by fengbo on 2018/5/9.
//
#ifndef LIVESTREAMDEMOPROJECT_AACENCODER_H
#define LIVESTREAMDEMOPROJECT_AACENCODER_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include <libavutil/time.h>
};

#include "../data/AudioConfig.h"
#include "../util/LogUtils.h"
#include "../data/FrameData.h"

#ifndef _TREAD_SAFE_QUEUE
#define _TREAD_SAFE_QUEUE
#include "../util/threadsafe_queue.cpp"
#endif


class AACEncoder {

private:
    //用于标记编码后的帧的顺序
    int pts;
    //每帧大小
    int frame_size;

    AVFormatContext *pFormatCtx;
    AVOutputFormat *fmt;
    AVStream *audio_st;

    AVCodec *pCodec;
    AVFrame *pFrame;
    AVPacket pkt;

    pthread_t thread;

    AudioConfig *recordConfig;
public:
    AVCodecContext *pCodecCtx;
    //编码队列
    threadsafe_queue<uint8_t *> frame_queue;

private:

    int flush_encoder(AVFormatContext* fmt_context, unsigned int stream_index);

    ~AACEncoder();

public:
    //是否结束编码
    int is_end = 0;

    AACEncoder(AudioConfig* config);

    int initAAEncoder();

    static void* startLoopEncodeInThread(void* obj);

    int startEncodeForRtmp(FrameData *data);

    int encodeAAC(FrameData *data);

    void endEncoder();

    int pushOneFrame(uint8_t* frame);

    void userStop();
};

#endif //LIVESTREAMDEMOPROJECT_AACENCODER_H
