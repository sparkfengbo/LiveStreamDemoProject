//
// Created by fengbo on 2018/5/9.
//
#ifndef LIVESTREAMDEMOPROJECT_AACENCODER_H
#define LIVESTREAMDEMOPROJECT_AACENCODER_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
};

#include "RecorderConfig.h"
#include "threadsafe_queue.cpp"

class AACEncoder {

private:
    //是否结束编码
    int is_end = 0;
    //编码队列
    threadsafe_queue<uint8_t *> frame_queue;
    //用于标记编码后的帧的顺序
    int pts;
    //编码帧后的到的帧数
    int got_frame;
    //每帧大小
    int frame_size;

    AVFormatContext *pFormatCtx;
    AVOutputFormat *fmt;
    AVStream *audio_st;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame;
    AVPacket pkt;

    RecordConfig *recordConfig;
private:

    int flush_encoder(AVFormatContext* fmt_context, unsigned int stream_index);

    ~AACEncoder();

public:

    AACEncoder(RecordConfig* config);

    int initAAEncoder();

    static void* startEncode(void* obj);

    void endEncoder();

    int pushOneFrame(uint8_t* frame);

    void userStop();
};

#endif //LIVESTREAMDEMOPROJECT_AACENCODER_H
