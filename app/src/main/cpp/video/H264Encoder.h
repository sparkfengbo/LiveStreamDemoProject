//
// Created by fengbo on 2018/5/9.
//

#ifndef LIVESTREAMDEMOPROJECT_H264ENCODER_H
#define LIVESTREAMDEMOPROJECT_H264ENCODER_H


extern "C" {
#include <libavformat/avformat.h>
}
#ifndef _TREAD_SAFE_QUEUE
#define _TREAD_SAFE_QUEUE
#include "../util/threadsafe_queue.cpp"
#endif
#include "../data/RecorderConfig.h"
#include "../data/FrameData.h"

class H264Encoder {

private:
    int is_end = 0;


//    AVFormatContext *pFormatCtx;
//    AVOutputFormat *pOutFormat;
//    AVStream *video_st;
    AVCodec *pCodec;
    AVPacket pkt;
    AVFrame *pFrame;

    pthread_t thread;

    int picture_size;
    int in_frame_size;
    int in_frame_size_bits;
    int out_y_size;
    int pts = 0;
    int frameCnt = 0; //??

    RecordConfig *recordConfig;

public:
    AVCodecContext *pCodecCtx;
    //编码队列
    threadsafe_queue<uint8_t *> frame_queue;

private:
    int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);

    ~H264Encoder(){

    }

public:

    H264Encoder(RecordConfig* config);

    int initH264Encoder();

    void endEncoder();

    int pushOneFrame(uint8_t* frame);

    void userStop();

    void custom_filter(const H264Encoder *h264Encoder, const uint8_t *picture_buf,
                    int in_y_size,
                    int format);

    static void* startLoopEncodeInThread(void* obj);

    int startEncodeForRtmp(FrameData *data);
};


#endif //LIVESTREAMDEMOPROJECT_H264ENCODER_H
