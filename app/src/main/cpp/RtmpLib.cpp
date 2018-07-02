//
// Created by fengbo on 2018/6/27.
//

#include <jni.h>
#include "RtmpStreamer.h"

RtmpStreamer *rtmpStreamer;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_rtmptest_RTMPController_nativeStartPush(JNIEnv *env, jobject instance,
                                                                                      jstring url_) {
    AudioConfig *aacConfig = new AudioConfig();
    aacConfig->isFileOut = false;
    aacConfig->audio_bit_rate = 64000;
    aacConfig->audio_sample_rate = 44100;

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

    const char *url = env->GetStringUTFChars(url_, 0);
    rtmpStreamer = new RtmpStreamer();
    int ret = rtmpStreamer->init(url, videoConfig, aacConfig);
    if (ret < 0) {
        return false;
    }
    env->ReleaseStringUTFChars(url_, url);
    LOGI("RtmpStreamer init finish");

    ret = rtmpStreamer->startPush();
    if (ret < 0) {
        return false;
    }
    return true;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_rtmptest_RTMPController_nativePushPCMData(JNIEnv *env, jobject instance,
                                                                                        jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    int i = -1;
//    if (rtmpStreamer->getAudioEncoder()) {
//        i = rtmpStreamer->getAudioEncoder()->pushOneFrame((uint8_t *) data);
//    }
    env->ReleaseByteArrayElements(data_, data, 0);
    return i;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_rtmptest_RTMPController_nativePushYUVData(JNIEnv *env, jobject instance,
                                                                                        jbyteArray data_, jint length) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    int i = -1;
    if (rtmpStreamer->getVideoEncoder()) {
        i = rtmpStreamer->getVideoEncoder()->pushOneFrame((uint8_t *) data, length);
    }
    env->ReleaseByteArrayElements(data_, data, 0);
    return i;
}

