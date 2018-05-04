package com.sparkfengbo.ng.livestreamdemoproject;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

/**
 * Created by fengbo on 2018/5/2.
 */

public class AudioRecorder {
    /**
     * 音频采样率
     */
    public static final int AUDIO_FREQUENCY = 44100;

    /**
     * 音频数据格式
     */
    public static final int AUDIO_FORMAT_BIT = AudioFormat.ENCODING_PCM_16BIT;

    /**
     * 音频播放声道
     */
    public static final int AUDIO_FORMAT_CHANNEL_FOR_PLAY = AudioFormat.CHANNEL_OUT_MONO;

    /**
     * 音频录制声道
     */
    public static final int AUDIO_FORMAT_CHANNEL_FOR_RECORD = AudioFormat.CHANNEL_IN_MONO;



    private static final int BUFFER_LENGTH = 2048;

    private byte[] mBuffer;
    private AudioRecord mAudioRecord;

    public AudioRecorder() {
        init();
    }

    private void init () {
        int bufferSize = AudioRecord.getMinBufferSize(AUDIO_FREQUENCY, AUDIO_FORMAT_CHANNEL_FOR_RECORD, AUDIO_FORMAT_BIT);

        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, AUDIO_FREQUENCY, AUDIO_FORMAT_CHANNEL_FOR_RECORD, AUDIO_FORMAT_BIT, bufferSize);


        //TODO check is init success
        if (mAudioRecord.getState() == AudioRecord.STATE_UNINITIALIZED) {
            Mlog.e("AudioRecord getState : STATE_UNINITIALIZED");
            mAudioRecord = null;
            return;
        }
    }

    public boolean startRecord () {
        if (mBuffer == null) {
            mBuffer = new byte[BUFFER_LENGTH];
        }

        if (mAudioRecord != null && mAudioRecord.getState() == AudioRecord.STATE_INITIALIZED) {
            try {
                mAudioRecord.startRecording();
            } catch (IllegalStateException e) {
                Mlog.e("mAudioRecord startRecording fail");
                return false;
            }
            if (mAudioRecord.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
                Mlog.e("mAudioRecord getRecordingState not RECORDSTATE_RECORDING");
                return false;
            }
            return true;
        } else {
            Mlog.e("mAudioRecord getRecordingState STATE_INITIALIZED");
        }

        return false;
    }


    public byte[] readData() {
        if (mAudioRecord != null) {
            int numOfByte = mAudioRecord.read(mBuffer, 0, BUFFER_LENGTH);
            if (numOfByte == BUFFER_LENGTH) {
                return mBuffer;
            } else {
                Mlog.e("audiorecorder read data error. return  " + numOfByte);
            }
        }
        return null;
    }


    public void release () {
        if (mAudioRecord != null) {
            mAudioRecord.stop();
            mAudioRecord.release();
            mAudioRecord = null;
        }
    }

}
