//
// Created by fengbo on 2018/6/12.
//

#ifndef LIVESTREAMDEMOPROJECT_RTMPSTREAMER_H
#define LIVESTREAMDEMOPROJECT_RTMPSTREAMER_H


extern "C" {
#include <libavutil/time.h>
#include <pthread.h>
#include <libavfilter/avfilter.h>

}
#include <rtmpdump/librtmp/rtmp.h>
#include "audio/AACEncoder.h"
#include "video/H264Encoder.h"
#include "data/FrameData.h"
#include "util/LogUtils.h"


class RtmpStreamer {


private:

    AACEncoder *aacEncoder;

    H264Encoder *h264Encoder;

    AVStream *audioStream;

    AVStream *videoStream;

    AVCodecContext *audioCodecContext;

    AVCodecContext *videoCodecContext;

    AVFormatContext *formatContext = NULL;

    int videoStreamIndex;

    int audioStreamIndex;

    char *outputUrl;

    int is_end = 0;

    RTMP *rtmpHandler;

    ~ RtmpStreamer();

public:
    int init(char *url);

    int setAudioEncoder(AACEncoder *audioEncoder);

    int setVideoEncoder(H264Encoder *videoEncoder);

    int allocateStream(AVCodecContext *avCodecContext);

    int startPush();

    int stopPush();

    int pushFrame(FrameData *data, int streamIndex);

    static void *writeHead(void *pObj);

    static void *addAudioEncodeThread(void *pObj);

    static void *addVideoEncodeThread(void *pObj);
};

#endif //LIVESTREAMDEMOPROJECT_RTMPSTREAMER_H
