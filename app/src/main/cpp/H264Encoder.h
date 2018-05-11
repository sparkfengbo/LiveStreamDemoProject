//
// Created by fengbo on 2018/5/9.
//

#ifndef LIVESTREAMDEMOPROJECT_H264ENCODER_H
#define LIVESTREAMDEMOPROJECT_H264ENCODER_H


extern "C" {


}

#include <libavformat/avformat.h>
#include "threadsafe_queue.cpp"
#include "RecorderConfig.h"

class H264Encoder {

private:
    int is_end = 0;
    //编码队列
    threadsafe_queue<uint8_t *> frame_queue;

    AVFormatContext *pFormatCtx;
    AVOutputFormat *pOutFormat;
    AVStream *video_st;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVPacket pkt;
    AVFrame *pFrame;

    int picture_size;
    int in_frame_size;
    int in_frame_size_bits;
    int out_y_size;
    int pts = 0;
    int frameCnt = 0; //??

    RecordConfig *recordConfig;
private:
    int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);

    ~H264Encoder(){

    }

public:

    H264Encoder(RecordConfig* config);

    int initH264Encoder();

    static void* startEncode(void* obj);

    void endEncoder();

    int pushOneFrame(uint8_t* frame);

    void userStop();

    void custom_filter(const H264Encoder *h264Encoder, const uint8_t *picture_buf,
                    int in_y_size,
                    int format);
};


#endif //LIVESTREAMDEMOPROJECT_H264ENCODER_H
