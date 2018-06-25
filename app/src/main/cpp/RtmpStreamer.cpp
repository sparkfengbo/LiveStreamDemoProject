//
// Created by fengbo on 2018/6/12.
//


#include "RtmpStreamer.h"

RtmpStreamer::~RtmpStreamer() {
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
    this->aacEncoder = audioEncoder;
    return this->aacEncoder != NULL ? 0 : -1;
}

int RtmpStreamer::setVideoEncoder(H264Encoder *videoEncoder) {
    this->h264Encoder = videoEncoder;
    return this->h264Encoder != NULL ? 0 : -1;
}

int RtmpStreamer::allocateStream(AVCodecContext *avCodecContext) {
    AVStream *pStream = avformat_new_stream(formatContext, avCodecContext->codec);

    if (!pStream) {
        LOGE("RtmpStreamer allocate stream failed");
        return -1;
    }

    int ret = 0;

    //TODO 验证 没有avcodec_parameters_from_context 对此不会造成影响

    if (avCodecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        LOGD("Add video stream success!");
        videoStream = pStream;
        videoCodecContext = avCodecContext;
    } else if (avCodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        LOGD("Add audio stream success!");
        audioStream = pStream;
        audioCodecContext = avCodecContext;
    }
    return pStream->index;
}



int RtmpStreamer::startPush() {
    videoStreamIndex = allocateStream(h264Encoder->pCodecCtx);
    audioStreamIndex = allocateStream(aacEncoder->pCodecCtx);

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

//    pthread_join(t2, NULL);
    if (NULL != formatContext) {
        av_write_trailer(formatContext);
        avio_close(formatContext->pb);
    }
    return 0;
}

void* RtmpStreamer::addAudioEncodeThread(void *pObj) {
    RtmpStreamer *rtmpStreamer = (RtmpStreamer *) pObj;

    if (rtmpStreamer->aacEncoder == NULL) {
        LOGE("RtmpStreamer error null aac encoder");
        return 0;
    }
    int64_t beginTime = av_gettime();

    AACEncoder *audioEncoder = rtmpStreamer->aacEncoder;
    while (!rtmpStreamer->is_end || !audioEncoder->frame_queue.empty()) {
        if (audioEncoder->frame_queue.empty()) {
            continue;
        }

        uint8_t *framebuf = *audioEncoder->frame_queue.wait_and_pop().get();
        FrameData *audioFrameData = new FrameData();

        audioFrameData->data = framebuf;
        if (audioFrameData != NULL && audioFrameData->data) {
//            audioFrameData->pts = audioFrameData->pts - beginTime;
            audioEncoder->startEncodeForRtmp(audioFrameData);
        }

        if (audioFrameData != NULL && audioFrameData->avPacket->size > 0) {
            rtmpStreamer->pushFrame(audioFrameData, rtmpStreamer->audioStreamIndex);
        }
    }
    return 0;
}

void* RtmpStreamer::addVideoEncodeThread(void *pObj) {
    RtmpStreamer *rtmpStreamer = (RtmpStreamer *) pObj;

    if (rtmpStreamer->h264Encoder == NULL) {
        LOGE("RtmpStreamer error null aac encoder");
        return 0;
    }
    int64_t beginTime = av_gettime();

    H264Encoder *videoEncoder = rtmpStreamer->h264Encoder;

    while (!rtmpStreamer->is_end || !videoEncoder->frame_queue.empty()) {
        if (videoEncoder->frame_queue.empty()) {
            continue;
        }

        uint8_t *framebuf = *videoEncoder->frame_queue.wait_and_pop().get();

        FrameData *videoFrameData = new FrameData();
        videoFrameData->data = framebuf;

        if (videoFrameData != NULL && videoFrameData->data) {
//            videoFrameData->pts = audioFrameData->pts - beginTime;
            videoEncoder->startEncodeForRtmp(videoFrameData);
        }
        if (videoFrameData != NULL && videoFrameData->avPacket->size > 0) {
            rtmpStreamer->pushFrame(videoFrameData, rtmpStreamer->videoStreamIndex);
        }
    }

    return 0;
}


int RtmpStreamer::init(char *url) {
    outputUrl = url;
    int ret = 0;
    ret = avformat_alloc_output_context2(&formatContext, NULL, "flv", outputUrl);
    av_register_all();
    avformat_network_init();
    if (ret < 0) {
        LOGE("RtmpStreamer init failed");
        return -1;
    } else {
        LOGI("RtmpStreamer allocate context success");
    }
    return 0;
}

void *RtmpStreamer::writeHead(void *pObj) {
    RtmpStreamer *rtmpStreamer = (RtmpStreamer *) pObj;
    int ret = 0;
    ret = avio_open(&rtmpStreamer->formatContext->pb, rtmpStreamer->outputUrl, AVIO_FLAG_WRITE);
    if (ret < 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOGE("RtmpStreamer avio open failed: %s %s %d", rtmpStreamer->outputUrl, buf, ret);
        return 0;
    }

    LOGD("avio open %s success", rtmpStreamer->outputUrl);
    ret = avformat_write_header(rtmpStreamer->formatContext, NULL);
    if (ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOGE("avformat write header failed!: %s %d", buf, ret);
        return 0;
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
    } else {
        LOGE("unknow stream index");
        return -1;
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
        LOGD("stream index %d writer frame", streamIndex);
    }
    return 0;
}
