//
// Created by fengbo on 2018/6/27.
//

#include <jni.h>
#include "RtmpStreamer.h"

RtmpStreamer *rtmpStreamer;
AACEncoder *audioEncoder;
H264Encoder *videoEncoder;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RtmpController_nativeStartPush(JNIEnv *env, jobject instance,
                                                                             jstring url_) {
    int ret = 0;
    AudioConfig *aacConfig = new AudioConfig();
    aacConfig->isFileOut = false;
    aacConfig->audio_bit_rate = 64000;
    aacConfig->audio_sample_rate = 44100;
    audioEncoder = new AACEncoder(aacConfig);
    ret = audioEncoder->initAAEncoder();
    if (ret < 0) {
        return false;
    }
    LOGI("AACEncoder init finish");

    VideoConfig *videoConfig = new VideoConfig();

    videoConfig->video_in_width = 2048;
    videoConfig->video_in_height = 1080;

    //旋转90度的话，是这样：
//    videoConfig->video_out_width = videoConfig->video_in_height;
//    videoConfig->video_out_height = videoConfig->video_in_width;

    videoConfig->video_out_width = videoConfig->video_in_width;
    videoConfig->video_out_height = videoConfig->video_in_height;

    videoConfig->video_frame_rate = 30;
    videoConfig->video_bit_rate = 1000000;
    videoConfig->rotate_type = videoConfig->CONST_ROTATE_90;
    videoEncoder = new H264Encoder(videoConfig);
    ret = videoEncoder->initH264Encoder();
    if (ret < 0) {
        return false;
    }
    LOGI("H264Encoder init finish");

    const char *url = env->GetStringUTFChars(url_, 0);
    rtmpStreamer = new RtmpStreamer();
    ret = rtmpStreamer->init(url);
    if (ret < 0) {
        return false;
    }
    env->ReleaseStringUTFChars(url_, url);
    LOGI("RtmpStreamer init finish");

    rtmpStreamer->setAudioEncoder(audioEncoder);
    rtmpStreamer->setVideoEncoder(videoEncoder);

    ret = rtmpStreamer->startPush();
    if (ret < 0) {
        return false;
    }

    return true;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RtmpController_nativePushPCMData(JNIEnv *env, jobject instance,
                                                                               jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    int i = 0;
//    int i = audioEncoder->pushOneFrame((uint8_t *) data);
    env->ReleaseByteArrayElements(data_, data, 0);
    return i;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RtmpController_nativePushYUVData(JNIEnv *env, jobject instance,
                                                                               jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    int i = videoEncoder->pushOneFrame((uint8_t *) data);
    env->ReleaseByteArrayElements(data_, data, 0);
    return i;
}