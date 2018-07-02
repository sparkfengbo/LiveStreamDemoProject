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
#include "audio/AACEncoder.h"
#include "video/H264Encoder.h"
#include "data/FrameData.h"
#include "util/LogUtils.h"


class RtmpStreamer {

private:
    //RTMP上下文
    AVFormatContext *formatContext;

    //AAC编码器
    AACEncoder *audioEncoder;

    //音频编码上下文
    AVCodecContext *audioCodecContext;

    //音频流
    AVStream *audioStream;

    //音频流index
    int audioStreamIndex;

    //H264编码器
    H264Encoder *videoEncoder;

    //视频编码上下文
    AVCodecContext *videoCodecContext;

    //视频流
    AVStream *videoStream;

    //视频流index
    int videoStreamIndex;

    //RTMP地址
    const char *outputUrl;

    //是否已经结束
    int is_end = 0;

    ~ RtmpStreamer();
public:
    /**
     * 初始化RTMP推流环境,包括初始化AAC编码器、H264编码器
     * @param url
     * @return
     */
    int init(const char *url, VideoConfig *videoConfig, AudioConfig *audioConfig);

    /**
     * 为AAC、H264编码分配流
     * @param avCodecContext
     * @return
     */
    int allocateStream(AVCodecContext *avCodecContext);

    /**
     * 开始推流
     * @return
     */
    int startPush();

    /**
     * TODO 停止推流
     * @return
     */
    int stopPush();

    /**
     * 将编码后的AAC/H264帧加入队列
     * @param data
     * @param streamIndex
     * @return
     */
    int pushFrame(FrameData *data, int streamIndex);

    AACEncoder* getAudioEncoder();

    H264Encoder* getVideoEncoder();

    /**
     * 推流开始前写入flv的文件头
     * @param pObj
     * @return
     */
    static void* writeHead(void *pObj);

    /**
     * 开启AAC编码线程
     * @param pObj
     * @return
     */
    static void* addAudioEncodeThread(void *pObj);

    /**
     * 开启H264编码线程
     * @param pObj
     * @return
     */
    static void* addVideoEncodeThread(void *pObj);

};

#endif //LIVESTREAMDEMOPROJECT_RTMPSTREAMER_H
