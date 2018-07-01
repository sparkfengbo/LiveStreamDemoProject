//
// Created by fengbo on 2018/5/9.
//

#ifndef LIVESTREAMDEMOPROJECT_H264ENCODER_H
#define LIVESTREAMDEMOPROJECT_H264ENCODER_H


extern "C" {
#include <libavformat/avformat.h>
#include "libavutil/opt.h"
#include "libavformat/avformat.h"
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libyuv/convert.h>
}

#ifndef _TREAD_SAFE_QUEUE
#define _TREAD_SAFE_QUEUE
#include "../util/threadsafe_queue.cpp"
#endif

#include <mutex>
#include "../util/LogUtils.h"
#include "../data/VideoConfig.h"
#include "../data/FrameData.h"

using namespace std;

class H264Encoder {

private:

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

    mutable mutex mut;

    VideoConfig *recordConfig;
public:
    AVCodecContext *pCodecCtx;
    //编码队列
    threadsafe_queue<FrameData *> frame_queue;

private:
    int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);

    ~H264Encoder(){

    }

public:

    int is_end = 0;

    H264Encoder(VideoConfig* config);

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
