
#include "AACEncoder.h"

#include <jni.h>
#include "LogUtils.h"

extern "C" {
#include "ffmpeg.h"
}


AACEncoder *audioEncoder;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_nativeSendYUVData(JNIEnv *env, jobject instance,
                                                                                jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    LOGE("nativeSendYUVData");
    // TODO
    env->ReleaseByteArrayElements(data_, data, 0);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_nativeSendPCMData(JNIEnv *env, jobject instance,
                                                                                jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
//    LOGI("nativeSendPCMData");
    if (audioEncoder == NULL) {
        LOGE("nativeSendPCMData audioEncoder is NULL");
        return -1;
    }
    int i = audioEncoder->pushOneFrame((uint8_t *) data);
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

    return run(argc, argv);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_initAACEncoder(JNIEnv *env, jobject instance) {
    struct RecordConfig config = {
            NULL,
            NULL,
            64000,
            44100,
            "/sdcard/DCIM/test.aac"
    };
    audioEncoder = new AACEncoder(&config);
    audioEncoder->initAAEncoder();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_stopEncodeAAC(JNIEnv *env, jobject instance) {
    audioEncoder->userStop();
}