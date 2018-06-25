//
// Created by fengbo on 2018/5/9.
//

#ifndef LIVESTREAMDEMOPROJECT_RECORDERCONFIG_H
#define LIVESTREAMDEMOPROJECT_RECORDERCONFIG_H


#include <jni.h>

class RecordConfig {

public:
    JNIEnv *env;
    JavaVM *javaVM;

    //output mp4
    //video
    int video_in_width;
    int video_in_height;
    int video_out_width;
    int video_out_height;
    char* video_path;
    int video_frame_rate;
    long long video_bit_rate;
    int rotate_type = -1;
    int v_custom_format;

    //audio
    int audio_bit_rate;
    int audio_sample_rate;
    char* audio_path;

    //RTMP

    char* rtmpUrl;

public:

    static const int CONST_ROTATE_0 = 1;

    static const int CONST_ROTATE_90 = 2;

    static const int CONST_ROTATE_180 = 3;

    static const int CONST_ROTATE_270 = 4;


};







#endif //LIVESTREAMDEMOPROJECT_RECORDERCONFIG_H
