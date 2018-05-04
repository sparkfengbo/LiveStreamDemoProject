#include <jni.h>
#include <string>
#include "LogUtils.h"

//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavfilter/avfilter.h>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_stringFromJNI(JNIEnv *env, jobject instance) {

    // TODO
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_sendPCMData(JNIEnv *env, jobject instance,
                                                                          jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);

    LOGE("This is log");
    env->ReleaseByteArrayElements(data_, data, 0);

    return env->GetArrayLength(data_);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_run(JNIEnv *env, jobject instance) {

    // TODO

}