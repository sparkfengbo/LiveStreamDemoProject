//
// Created by fengbo on 2018/6/12.
//

#include "RtmpStreamer.h"

int RtmpStreamer::init(const char *url, VideoConfig *videoConfig, AudioConfig *audioConfig) {
    //TODO 这里应该统一起来，虽然重复调用没问题，但总是不perfect
    av_register_all();
    avfilter_register_all();
    avformat_network_init();

    outputUrl = url;

    int ret = avformat_alloc_output_context2(&formatContext, NULL, "flv", outputUrl);

    if (ret < 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOGE("RtmpStreamer init failed : %s ", buf);
        return -1;
    } else {
        LOGI("RtmpStreamer init success");
    }

    audioEncoder = new AACEncoder(audioConfig);
    ret = audioEncoder->initAAEncoder();
    if (ret < 0) {
        return false;
    }
    LOGI("AACEncoder init finish");

    videoEncoder = new H264Encoder(videoConfig);
    ret = videoEncoder->initH264Encoder();
    if (ret < 0) {
        return false;
    }
    LOGI("H264Encoder init finish");

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
     * AVStream分配后，它的AVCodecContext并没有正确的初始化，所以会返回invalid argument
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

//    pStream->codec->codec_tag = 0;

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

    LOGI("begin time %d", beginTime);

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

    int64_t beginTime = av_gettime();

    H264Encoder *videoEncoder = rtmpStreamer->videoEncoder;

    while (!rtmpStreamer->is_end || !videoEncoder->frame_queue.empty()) {
        if (videoEncoder->frame_queue.empty()) {
            continue;
        }

        FrameData *framebuf = *videoEncoder->frame_queue.wait_and_pop().get();

        if (framebuf != NULL && framebuf->data) {
            framebuf->pts = framebuf->pts - beginTime;
            videoEncoder->startEncodeForRTMP(framebuf);
        }
        if (framebuf != NULL && framebuf->avPacket->size > 0) {
            rtmpStreamer->pushFrame(framebuf, rtmpStreamer->videoStreamIndex);
        }
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
    /**
     * FFMPEG之AVRational TimeBase成员理解
     *
     * https://blog.csdn.net/supermanwg/article/details/14521869
     */
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


    /**
     * 关于av_rescale_q的计算过程可以参考
     *
     * https://blog.csdn.net/dancing_night/article/details/46472477
     */
    packet->pts = av_rescale_q(packet->pts, stime, dtime);
    packet->dts = av_rescale_q(packet->dts, stime, dtime);
    packet->duration = av_rescale_q(packet->duration, stime, dtime);

//    LOGE("CodecContext.time_base.num : %d CodecContext.time_base.den : %d Stream.time_base.num : %d Stream.time_base.den : %d",
//         stime.num, stime.den, dtime.num, dtime.den);
//    CodecContext.time_base.num : 1 CodecContext.time_base.den : 1000000 Stream.time_base.num : 1 Stream.time_base.den : 1000

    int ret = av_interleaved_write_frame(formatContext, packet);
    if (ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOGE("stream index %d writer frame failed! :%s", streamIndex, buf);
    } else {
        if (streamIndex == audioStreamIndex) {
            LOGD("---------->write audio frame success------->!");
        } else if (streamIndex == videoStreamIndex) {
            LOGD("---------->write video frame success------->!");
        }
    }

    free(data);
    return 0;
}

AACEncoder* RtmpStreamer::getAudioEncoder() {
    return audioEncoder;
}

H264Encoder* RtmpStreamer::getVideoEncoder() {
    return videoEncoder;
}

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

