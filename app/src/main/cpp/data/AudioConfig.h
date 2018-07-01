//
// Created by fengbo on 2018/6/12.
//

#include "RecorderConfig.h"

#ifndef LIVESTREAMDEMOPROJECT_AUDIOCONFIG_H
#define LIVESTREAMDEMOPROJECT_AUDIOCONFIG_H


class AudioConfig : RecordConfig {
public:
    //比特率
    int audio_bit_rate;

    //采样率
    int audio_sample_rate;

    //是否是文件输出
    bool isFileOut = false;

    //file out path
    char *audio_out_path;
};
#endif //LIVESTREAMDEMOPROJECT_AUDIOCONFIG_H
