//
// Created by fengbo on 2018/5/9.
//

#ifndef LIVESTREAMDEMOPROJECT_RECORDERCONFIG_H
#define LIVESTREAMDEMOPROJECT_RECORDERCONFIG_H


#include <jni.h>

typedef struct RecordConfig {
    JNIEnv *env;

    JavaVM *javaVM;

    //output
    //video

    //audio
    int audio_bit_rate;
    int audio_sample_rate;
    char* audio_path;
};







#endif //LIVESTREAMDEMOPROJECT_RECORDERCONFIG_H
