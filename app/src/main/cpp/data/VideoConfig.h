//
// Created by fengbo on 2018/6/12.
//

#include "RecorderConfig.h"

#ifndef LIVESTREAMDEMOPROJECT_VIDEOCONFIG_H
#define LIVESTREAMDEMOPROJECT_VIDEOCONFIG_H


class VideoConfig : RecordConfig {

public:
    //是否是文件输入
    int isFileIn = 0;

    //是否是文件输出
    int isFileOut = 0;

    //file out path
    char *video_out_path;

    int video_in_width;
    int video_in_height;
    int video_out_width;
    int video_out_height;
    int video_frame_rate;
    long long video_bit_rate;
    int rotate_type = -1;
    int v_custom_format;

public:

    static const int CONST_ROTATE_0 = 1;

    static const int CONST_ROTATE_90 = 2;

    static const int CONST_ROTATE_180 = 3;

    static const int CONST_ROTATE_270 = 4;
};

#endif //LIVESTREAMDEMOPROJECT_VIDEOCONFIG_H
