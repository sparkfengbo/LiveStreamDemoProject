#include <jni.h>
#include "ffmpeg.h"

JNIEXPORT jlong JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_sendPCMData(JNIEnv *env, jobject instance,
                                                                          jbyteArray data_) {
    jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);

    // TODO

    (*env)->ReleaseByteArrayElements(env, data_, data, 0);
}

JNIEXPORT jint JNICALL
Java_com_sparkfengbo_ng_livestreamdemoproject_RecorderManager_run(JNIEnv *env, jobject instance, jobjectArray cmd) {

    int argc = (*env)->GetArrayLength(env, cmd);
    char *argv[argc];

    int i;
    for (i = 0; i < argc; i++) {
        jstring js = (jstring) (*env)->GetObjectArrayElement(env, cmd, i);
        argv[i] = (char *) (*env)->GetStringUTFChars(env, js, 0);
    }
    return run(argc, argv);

}