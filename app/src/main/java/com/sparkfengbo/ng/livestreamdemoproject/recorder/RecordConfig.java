package com.sparkfengbo.ng.livestreamdemoproject.recorder;

import android.media.AudioFormat;

/**
 * Created by fengbo on 2018/5/15.
 *
 * 录制时的配置信息
 */
public class RecordConfig {
    /**
     * 音频采样率
     */
    public static final int AUDIO_SAMPLE_RATE = 44100;

    /**
     * 音频比特率
     */

    public static final int AUDIO_BIT_RATE = 64000;
    /**
     * 音频数据格式
     */
    public static final int AUDIO_FORMAT_BIT = AudioFormat.ENCODING_PCM_16BIT;

    /**
     * 音频录制声道
     */
    public static final int AUDIO_FORMAT_CHANNEL_FOR_RECORD = AudioFormat.CHANNEL_IN_MONO;



    public static final int VIDEO_FRAME_RATE = 30;



}
