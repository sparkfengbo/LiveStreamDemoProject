
#include <jni.h>
#include "util/LogUtils.h"

#include "audio/AACEncoder.h"
#include "video/H264Encoder.h"
#include "RTMPSender.h"
#include "RtmpStreamer.h"
#include "data/VideoConfig.h"

extern "C" {
#include "ffmpeg/ffmpeg.h"
}

AACEncoder *audioEncoder;
H264Encoder *h264Encoder;

AACEncoder *tAudioEncoder;
H264Encoder *tH264Encoder;
RTMPSender *rtmpEncoder;
RecordConfig *config;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_nativeSendYUVData(JNIEnv *env, jobject instance,
                                                                                jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
//    LOGI("nativeSendYUVData");
    if (tH264Encoder == NULL) {
        LOGE("nativeSendYUVData 264Encoder is NULL");
        env->ReleaseByteArrayElements(data_, data, 0);
        return -1;
    }
    int i = tH264Encoder->pushOneFrame((uint8_t *) data);
    env->ReleaseByteArrayElements(data_, data, 0);
    return i;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_nativeSendPCMData(JNIEnv *env, jobject instance,
                                                                                jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    LOGI("nativeSendPCMData");
    if (tAudioEncoder == NULL) {
//        LOGE("nativeSendPCMData audioEncoder is NULL");
        env->ReleaseByteArrayElements(data_, data, 0);
        return -1;
    }
    int i = tAudioEncoder->pushOneFrame((uint8_t *) data);
    env->ReleaseByteArrayElements(data_, data, 0);
    return i;
}

/**
 * 运行ffmpeg cmd 命令
 *
 * @param env
 * @param instance
 * @param cmd
 * @return
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_run(JNIEnv *env, jobject instance, jobjectArray cmd) {

    int argc = env->GetArrayLength(cmd);
    char *argv[argc];

    int i;
    for (i = 0; i < argc; i++) {
        jstring js = (jstring) env->GetObjectArrayElement(cmd, i);
        argv[i] = (char *) env->GetStringUTFChars(js, 0);
    }

    int ret = run(argc, argv);
    LOGI("run method returns %d", ret);
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_initAACEncoder(JNIEnv *env, jobject instance) {
    AudioConfig *config = new AudioConfig();
    config->audio_bit_rate = 64000;
    config->audio_out_path = "/sdcard/DCIM/test.aac";
    config->audio_sample_rate = 44100;
    audioEncoder = new AACEncoder(config);
    audioEncoder->initAAEncoder();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_stopEncodeAAC(JNIEnv *env, jobject instance) {
    if (audioEncoder != NULL) {
        audioEncoder->userStop();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_initH264Encoder(JNIEnv *env, jobject instance) {

    VideoConfig *config = new VideoConfig();
//    1280*960
    config->video_in_width = 2048;
    config->video_in_height = 1080;

    config->video_out_width = 2048;
    config->video_out_height = 1080;
    config->video_out_path = "/sdcard/DCIM/test-video.h264";
    config->video_frame_rate = 30;
    config->video_bit_rate = 1000000;
//    config.v_custom_format;
    config->rotate_type = config->CONST_ROTATE_90;
    h264Encoder = new H264Encoder(config);
    h264Encoder->initH264Encoder();
    return;

}
extern "C"
JNIEXPORT void JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_stopEncodeH264(JNIEnv *env, jobject instance) {
    if (h264Encoder != NULL) {
        h264Encoder->userStop();
    }
    return;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_muxMP4(JNIEnv *env, jobject instance) {
//    char *cmd[10];
//    cmd[0]="ffmpeg";
//    cmd[1]="-i";
//    cmd[2]="/sdcard/DCIM/test-video.h264";
//    cmd[3]="-i";
//    cmd[4]="/sdcard/DCIM/test.aac";
//    cmd[5]="-c:v";
//    cmd[6]="copy";
//    cmd[7]="-c:a";
//    cmd[8]="copy";
//    cmd[9]="/sdcard/DCIM/test-mp4.mp4";
//    run(10,cmd);

    char *cmd[6];
    cmd[0]="ffmpeg";
    cmd[1]="-i";
    cmd[2]="/sdcard/DCIM/test-video.h264";
    cmd[3]="-c:v";
    cmd[4]="copy";
    cmd[5]="/sdcard/DCIM/test-mp4.mp4";
    run(6,cmd);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_nativeStartPushRtmp(JNIEnv *env, jobject instance) {

    RtmpStreamer *rtmpStreamer;

    AudioConfig *aacConfig = new AudioConfig();
    aacConfig->isFileOut = false;
    aacConfig->audio_bit_rate = 64000;
    aacConfig->audio_sample_rate = 44100;
    tAudioEncoder = new AACEncoder(aacConfig);
    tAudioEncoder->initAAEncoder();


    VideoConfig *videoConfig = new VideoConfig();
//    1280*960
    videoConfig->video_in_width = 1280;
    videoConfig->video_in_height = 960;

    videoConfig->video_out_width = videoConfig->video_in_width;
    videoConfig->video_out_height = videoConfig->video_in_height;
//    h264config->video_out_width = 2048;
//    h264config->video_out_height = 1080;

//    videoConfig->video_out_width = 640;
//    videoConfig->video_out_height = 480;

    videoConfig->video_frame_rate = 30;
    videoConfig->video_bit_rate = 1000000;
//    config.v_custom_format;
    videoConfig->rotate_type = videoConfig->CONST_ROTATE_180;

    tH264Encoder = new H264Encoder(videoConfig);
    tH264Encoder->initH264Encoder();

    rtmpStreamer = new RtmpStreamer();
    rtmpStreamer->init("rtmp://172.22.126.89:1935/test/live");
    rtmpStreamer->setAudioEncoder(tAudioEncoder);
    rtmpStreamer->setVideoEncoder(tH264Encoder);
    rtmpStreamer->startPush();
}
