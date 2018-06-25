//
// Created by fengbo on 2018/6/1.
//

#include "RTMPSender.h"
#include "util/LogUtils.h"
#include "data/RecorderConfig.h"

//@deprecated
void RTMPSender::init() {
    rtmpHandler = RTMP_Alloc();
    RTMP_Init(rtmpHandler);
    if (!RTMP_SetupURL(rtmpHandler, url)) {
        LOGE("ERROR setup rtmp url");
        RTMP_Free(rtmpHandler);
        return;
    }

    RTMP_EnableWrite(rtmpHandler);

    if (!RTMP_Connect(rtmpHandler, NULL)) {
        LOGE("ERROR rtmp connenct");
        RTMP_Free(rtmpHandler);
        return;
    }

    if (!RTMP_ConnectStream(rtmpHandler, 0)) {
        LOGE("ERROR rtmp connenct stream");
        RTMP_Free(rtmpHandler);
        return;
    }
}


int RTMPSender::send_rtmp_aac(unsigned char *buf, int len) {


    return 0;
}

int RTMPSender::send_aac_spec(unsigned char *spec_buf, int spec_len) {

    RTMPPacket *packet;
    unsigned char *body;
    int len;
    len = spec_len;  /*spec data长度,一般是2*/
    packet = (RTMPPacket *) malloc(RTMP_HEAD_SIZE + len + 2);
    memset(packet, 0, RTMP_HEAD_SIZE);

    packet->m_body = (char *) packet + RTMP_HEAD_SIZE;
    body = (unsigned char *) packet->m_body;

    /*AF 00 + AAC RAW data*/
    body[0] = 0xAF;
    body[1] = 0x00;
    memcpy(&body[2], spec_buf, len); /*spec_buf是AAC sequence header数据*/

    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nBodySize = len + 2;
    packet->m_nChannel = 0x04;
    packet->m_nTimeStamp = 0;
    packet->m_hasAbsTimestamp = 0;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_nInfoField2 = rtmpHandler->m_stream_id;

    /*调用发送接口*/
    RTMP_SendPacket(rtmpHandler, packet, TRUE);
    return 1;
}

int RTMPSender::send_video_sps_pps() {
//
//    RTMPPacket * packet;
//    unsigned char * body;
//    int i;
//
//    packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+1024);
//    memset(packet,0,RTMP_HEAD_SIZE);
//
//    packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
//    body = (unsigned char *)packet->m_body;
//
//    memcpy(winsys->pps,buf,len);
//    winsys->pps_len = len;
//
//    i = 0;
//    body[i++] = 0x17;
//    body[i++] = 0x00;
//
//    body[i++] = 0x00;
//    body[i++] = 0x00;
//    body[i++] = 0x00;
//
//    /*AVCDecoderConfigurationRecord*/
//    body[i++] = 0x01;
//    body[i++] = sps[1];
//    body[i++] = sps[2];
//    body[i++] = sps[3];
//    body[i++] = 0xff;
//
//    /*sps*/
//    body[i++]   = 0xe1;
//    body[i++] = (sps_len >> 8) & 0xff;
//    body[i++] = sps_len & 0xff;
//    memcpy(&body[i],sps,sps_len);
//    i +=  sps_len;
//
//    /*pps*/
//    body[i++]   = 0x01;
//    body[i++] = (pps_len >> 8) & 0xff;
//    body[i++] = (pps_len) & 0xff;
//    memcpy(&body[i],pps,pps_len);
//    i +=  pps_len;
//
//    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
//    packet->m_nBodySize = i;
//    packet->m_nChannel = 0x04;
//    packet->m_nTimeStamp = 0;
//    packet->m_hasAbsTimestamp = 0;
//    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
//    packet->m_nInfoField2 = rtmp->m_stream_id;
//
//    /*调用发送接口*/
//    RTMP_SendPacket(rtmp,packet,TRUE);
//    free(packet);

    return 0;

}


void* RTMPSender::startSender(void *obj) {
    RTMPSender *rtmpSender = (RTMPSender *) obj;
//    while (!rtmpSender->is_end || !rtmpSender->frame_queue.empty()) {


}



void RTMPSender::sendH264File(char *filepath) {





}



RTMPSender::RTMPSender(RecordConfig *config) {
    url = config->rtmpUrl;
}

RTMPSender::~RTMPSender() {
    RTMP_Close(rtmpHandler);
    RTMP_Free(rtmpHandler);
}