//
// Created by fengbo on 2018/6/12.
//

#include "RtmpStreamer.h"
/**
 * 析构，释放资源
 */
RtmpStreamer::~RtmpStreamer() {
    if  (audioEncoder != NULL) {
        free(audioEncoder);
        audioEncoder = NULL;
    }

    if  (videoEncoder != NULL) {
        free(videoEncoder);
        videoEncoder = NULL;
    }

    if (audioStream != NULL) {
        av_free(audioStream);
        audioStream = NULL;
    }

    if (videoStream != NULL) {
        av_free(videoStream);
        videoStream = NULL;
    }

    if (audioCodecContext != NULL) {
        avcodec_free_context(&audioCodecContext);
        audioCodecContext = NULL;
    }

    if (videoCodecContext != NULL) {
        avcodec_free_context(&videoCodecContext);
        videoCodecContext = NULL;
    }

    if (formatContext != NULL) {
        avformat_free_context(formatContext);
        formatContext = NULL;
    }
}

int RtmpStreamer::setAudioEncoder(AACEncoder *audioEncoder) {
    if (audioEncoder == NULL) {
        LOGE("Error ! AudioEncoder can not be NULL!");
        return -1;
    }
    this->audioEncoder = audioEncoder;
    return 0;
}

int RtmpStreamer::setVideoEncoder(H264Encoder *videoEncoder) {
    if (videoEncoder == NULL) {
        LOGE("Error ! VideoEncoder can not be NULL!");
        return -1;
    }
    this->videoEncoder = videoEncoder;
    return 0;
}

int RtmpStreamer::allocateStream(AVCodecContext *avCodecContext) {
    AVStream *pStream = avformat_new_stream(formatContext, avCodecContext->codec);
    if (!pStream) {
        LOGE("RtmpStreamer allocate stream failed");
        return -1;
    }

    int ret = 0;
    /**
     * 这里是个坑
     *
     * 调试了很久，在调用avformat_write_header的时候返回-22 ，invalid argument
     *
     * 根据FFMEPG的版本，3.0.11不能使用avcodec_parameters_from_context
     *
     * http://ffmpeg.org/pipermail/ffmpeg-cvslog/2016-June/100912.html
     */
    ret = avcodec_copy_context(pStream->codec, avCodecContext);
    if (ret < 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOGE("avcodec_copy_context failed : %s %d", buf, ret);
        return -1;
    }

    pStream->codec->codec_tag = 0;

    if (avCodecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        LOGI("Add video stream success!");
        videoStream = pStream;
        videoCodecContext = avCodecContext;
    } else if (avCodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        LOGI("Add audio stream success!");
        audioStream = pStream;
        audioCodecContext = avCodecContext;
    }
    return pStream->index;
}

int RtmpStreamer::startPush() {
    videoStreamIndex = allocateStream(videoEncoder->pCodecCtx);
    audioStreamIndex = allocateStream(audioEncoder->pCodecCtx);

    pthread_t writeHeadThread;

    pthread_t audioEncodeThread;
    pthread_t videoEncodeThread;

    pthread_create(&writeHeadThread, NULL, RtmpStreamer::writeHead, this);
    pthread_join(writeHeadThread, NULL);

    pthread_create(&audioEncodeThread, NULL, RtmpStreamer::addAudioEncodeThread, this);
    pthread_create(&videoEncodeThread, NULL, RtmpStreamer::addVideoEncodeThread, this);

    return 0;
}

int RtmpStreamer::stopPush() {
    is_end = 1;

    if (audioEncoder != NULL) {
        audioEncoder->is_end = 1;
        audioEncoder->endEncoder();
    }

    if (videoEncoder != NULL) {
        videoEncoder->is_end = 1;
        videoEncoder->endEncoder();
    }

//    pthread_join(t2, NULL);
    if (NULL != formatContext) {
        av_write_trailer(formatContext);
        avio_close(formatContext->pb);
    }
    return 0;
}

void* RtmpStreamer::addAudioEncodeThread(void *pObj) {
    RtmpStreamer *rtmpStreamer = (RtmpStreamer *) pObj;

    if (rtmpStreamer->audioEncoder == NULL) {
        LOGE("RtmpStreamer error null aac encoder");
        return 0;

    }

    int64_t beginTime = av_gettime();

//    LOGI("begin time %d", beginTime);

    AACEncoder *audioEncoder = rtmpStreamer->audioEncoder;

    while (!rtmpStreamer->is_end || !audioEncoder->frame_queue.empty()) {
        if (audioEncoder->frame_queue.empty()) {
            continue;
        }

//        uint8_t *framebuf = *audioEncoder->frame_queue.wait_and_pop().get();
//
//        LOGI("framebuf");
//
//        FrameData *audioFrameData = new FrameData();
//        audioFrameData->data = framebuf;
//        if (audioFrameData != NULL && audioFrameData->data) {
////            audioFrameData->pts = audioFrameData->pts - beginTime;
//            audioEncoder->encodeAAC(audioFrameData);
//            LOGI("encode AAC ");
//        }
//
//        if (audioFrameData != NULL && audioFrameData->avPacket->size > 0) {
//            rtmpStreamer->pushFrame(audioFrameData, rtmpStreamer->audioStreamIndex);
//            LOGI("pushFrame");
//        }
    }

    LOGI("addAudioEncodeThread exit LOOP");

    return 0;

}

void* RtmpStreamer::addVideoEncodeThread(void *pObj) {
    RtmpStreamer *rtmpStreamer = (RtmpStreamer *) pObj;

    if (rtmpStreamer->videoEncoder == NULL) {
        LOGE("RtmpStreamer error null aac encoder");
        return 0;

    }
    int64_t beginTime = av_gettime();

    H264Encoder *videoEncoder = rtmpStreamer->videoEncoder;

    while (!rtmpStreamer->is_end || !videoEncoder->frame_queue.empty()) {
        if (videoEncoder->frame_queue.empty()) {
            continue;
        }

//        uint8_t *framebuf = *videoEncoder->frame_queue.wait_and_pop().get();
//
//        FrameData *videoFrameData = new FrameData();
//        videoFrameData->data = framebuf;
//
//        if (videoFrameData != NULL && videoFrameData->data) {
////            videoFrameData->pts = audioFrameData->pts - beginTime;
//            videoEncoder->startEncodeForRtmp(videoFrameData);
//        }
//        if (videoFrameData != NULL && videoFrameData->avPacket->size > 0) {
//            rtmpStreamer->pushFrame(videoFrameData, rtmpStreamer->videoStreamIndex);
//        }

        FrameData *framebuf = *videoEncoder->frame_queue.wait_and_pop().get();

        if (framebuf != NULL && framebuf->data) {
            framebuf->pts = framebuf->pts - beginTime;
            videoEncoder->startEncodeForRtmp(framebuf);
        }
        if (framebuf != NULL && framebuf->avPacket->size > 0) {
            rtmpStreamer->pushFrame(framebuf, rtmpStreamer->videoStreamIndex);
        }
    }

    return 0;

}

/**
 * 初始化RTMP
 *
 * @param url  rtmp地址
 * @return
 */
int RtmpStreamer::init(const char *url) {
    outputUrl = url;
    int ret = 0;

    //TODO 这里应该统一起来，虽然重复调用没问题，但总是不perfect
    av_register_all();
    avfilter_register_all();
    avformat_network_init();

    ret = avformat_alloc_output_context2(&formatContext, NULL, "flv", outputUrl);

    if (ret < 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOGE("RtmpStreamer init failed : %s ", buf);
        return -1;
    } else {
        LOGI("RtmpStreamer init success");
    }
    return 0;
}

void* RtmpStreamer::writeHead(void *pObj) {
    RtmpStreamer *rtmpStreamer = (RtmpStreamer *) pObj;
    int ret = 0;
    ret = avio_open(&rtmpStreamer->formatContext->pb, rtmpStreamer->outputUrl, AVIO_FLAG_WRITE);
    if (ret < 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOGE("RtmpStreamer avio_open %s failed: %s %d", rtmpStreamer->outputUrl, buf, ret);
        return 0;

    }

    LOGI("RtmpStreamer avio_open %s success", rtmpStreamer->outputUrl);
    ret = avformat_write_header(rtmpStreamer->formatContext, NULL);
    if (ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOGE("RtmpStreamer avformat_write_header failed: %s %d", buf, ret);
        return 0;

    } else {
        LOGI("RtmpStreamer avformat_write_header success");
    }
    return 0;

}

int RtmpStreamer::pushFrame(FrameData *data, int streamIndex) {
    AVRational stime;
    AVRational dtime;

    AVPacket *packet = data->avPacket;
    packet->stream_index = streamIndex;

    if (packet->stream_index == videoStreamIndex) {
        stime = videoCodecContext->time_base;
        dtime = videoStream->time_base;
    } else if (packet->stream_index == audioStreamIndex) {
        stime = audioCodecContext->time_base;
        dtime = audioStream->time_base;
    }

    packet->pts = av_rescale_q(packet->pts, stime, dtime);
    packet->dts = av_rescale_q(packet->dts, stime, dtime);
    packet->duration = av_rescale_q(packet->duration, stime, dtime);
    int ret = av_interleaved_write_frame(formatContext, packet);

    if (ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOGE("stream index %d writer frame failed! :%s", streamIndex, buf);
    } else {
//        LOGD("stream index %d writer frame", streamIndex);
        if (streamIndex == audioStreamIndex) {
            LOGD("---------->write audio frame success------->!");
        } else if (streamIndex == videoStreamIndex) {
            LOGD("---------->write video frame success------->!");
        }
    }

//    free(packet);
    free(data);
    return 0;
}
