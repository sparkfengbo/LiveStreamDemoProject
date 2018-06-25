//
// Created by fengbo on 2018/6/1.
//

#ifndef LIVESTREAMDEMOPROJECT_FRAMEDATA_H
#define LIVESTREAMDEMOPROJECT_FRAMEDATA_H

#include <libavcodec/avcodec.h>
#include "../../../../../../../Library/Android/sdk/ndk-bundle/sources/cxx-stl/gnu-libstdc++/4.9/include/cstdint"

class FrameData {


public:
    int64_t pts;

    AVFrame *frame;

    AVPacket *avPacket;

    uint8_t  *data;

    int size;

    void drop();
};
#endif //LIVESTREAMDEMOPROJECT_FRAMEDATA_H
