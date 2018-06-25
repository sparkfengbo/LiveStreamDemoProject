//
// Created by fengbo on 2018/6/1.
//

#ifndef LIVESTREAMDEMOPROJECT_RTMPSENDER_H
#define LIVESTREAMDEMOPROJECT_RTMPSENDER_H

extern "C" {
#include <libavformat/avformat.h>

#include <rtmpdump/librtmp/rtmp.h>
}

#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)

#ifndef _TREAD_SAFE_QUEUE

#define _TREAD_SAFE_QUEUE
#include "threadsafe_queue.cpp"
#include "RecorderConfig.h"

#endif

//@deprecated
class RTMPSender {

private:
    RTMP *rtmpHandler;
    char *url;
    RecordConfig *recordConfig;

    void send_video_frame(void *data, int len, uint64_t dts, uint64_t pts, char* str);

    void send_audio_frame(void *data, int len, uint64_t ts);

public:
    RTMPSender(RecordConfig* config);
    void init();

    int send_video_sps_pps();
    int send_rtmp_video(unsigned char* buf, int len) ;
    int send_aac_spec(unsigned char *spec_buf, int len);
    int send_rtmp_aac(unsigned char * buf,int len);

    void sendH264File(char *filepath);

    static void* startSender(void* obj);
    void endSender();
    ~RTMPSender();
};


#endif //LIVESTREAMDEMOPROJECT_RTMPSENDER_H
