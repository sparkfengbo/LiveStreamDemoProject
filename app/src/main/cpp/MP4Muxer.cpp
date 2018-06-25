//
// Created by fengbo on 2018/5/11.
//
#include "MP4Muxer.h"
#include "util/LogUtils.h"

extern "C" {
#include "ffmpeg/ffmpeg.h"
}

int MP4Muxer::startMuxer(const char *video, const char *audio, const char *out_file) {

    size_t in_video_file_name_size = strlen(video);

    char *new_in_video_file_name = (char *) malloc(in_video_file_name_size + 1);

    strcpy((new_in_video_file_name), video);

    size_t in_audio_file_name_size = strlen(audio);

    char *new_in_audio_file_name = (char *) malloc(in_audio_file_name_size + 1);

    strcpy((new_in_video_file_name), video);

    size_t out_file_name_size = strlen(out_file);
    char *new_out_filename = (char *) malloc(out_file_name_size + 1);
    strcpy((new_out_filename), out_file);

    LOGI("start muxer mp4...");

    char *cmd[10];
    cmd[0] = "ffmpeg";
    cmd[1] = "-i";
    cmd[2] = new_in_video_file_name;
    cmd[3] = "-i";
    cmd[4] = new_in_audio_file_name;
    cmd[5] = "-c:v";
    cmd[6] = "copy";
    cmd[7] = "-c:a";
    cmd[8] = "copy";
    cmd[9] = new_out_filename;
    return run(10, cmd);
}