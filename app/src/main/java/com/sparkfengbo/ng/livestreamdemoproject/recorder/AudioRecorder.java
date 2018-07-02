package com.sparkfengbo.ng.livestreamdemoproject.recorder;

import com.sparkfengbo.ng.livestreamdemoproject.util.FLog;

import android.media.AudioRecord;
import android.media.MediaRecorder;

/**
 * Created by fengbo on 2018/5/2.
 *
 * 音频录音
 */

public class AudioRecorder {
    private static final int BUFFER_LENGTH = 2048;

    private byte[] mBuffer;

    private AudioRecord mAudioRecord;

    public AudioRecorder() {
        init();
    }

    private void init () {
        FLog.i("init AudioRecorder");
        int bufferSize = AudioRecord.getMinBufferSize(RtmpConfig.AUDIO_SAMPLE_RATE,
                RtmpConfig.AUDIO_FORMAT_CHANNEL_FOR_RECORD,
                RtmpConfig.AUDIO_FORMAT_BIT);

        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                RtmpConfig.AUDIO_SAMPLE_RATE,
                RtmpConfig.AUDIO_FORMAT_CHANNEL_FOR_RECORD,
                RtmpConfig.AUDIO_FORMAT_BIT,
                bufferSize);

        if (mAudioRecord.getState() == AudioRecord.STATE_UNINITIALIZED) {
            FLog.e("error : AudioRecord getState =  STATE_UNINITIALIZED");
            throw new RuntimeException("error : AudioRecord init failed!");
        }
    }

    public boolean startRecord () {
        FLog.i("java audio start record");

        if (mBuffer == null) {
            mBuffer = new byte[BUFFER_LENGTH];
        }

        if (mAudioRecord != null && mAudioRecord.getState() == AudioRecord.STATE_INITIALIZED) {
            try {
                FLog.i("AudioRecorder startRecording");
                mAudioRecord.startRecording();
            } catch (IllegalStateException e) {
                FLog.e("error : mAudioRecord startRecording fail catch exp");
                return false;
            }
            if (mAudioRecord.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
                FLog.e("mAudioRecord getRecordingState not RECORDSTATE_RECORDING");
                return false;
            }
            return true;
        } else {
            FLog.e("mAudioRecord getRecordingState STATE_UNINITIALIZED");
        }
        return false;
    }


    public byte[] readData() {
        if (mAudioRecord != null) {
            int numOfByte = mAudioRecord.read(mBuffer, 0, BUFFER_LENGTH);
            if (numOfByte == BUFFER_LENGTH) {
                return mBuffer;
            } else {
                FLog.e("audiorecorder read data error. return  " + numOfByte);
            }
        }
        return null;
    }

    public void stopRecord() {
        if (mAudioRecord != null && mAudioRecord.getState() == AudioRecord.STATE_INITIALIZED) {
            try {
                FLog.i("AudioRecorder startRecording");
                mAudioRecord.stop();
            } catch (IllegalStateException e) {
                FLog.e("error : mAudioRecord startRecording fail catch exp");
            }
        } else {
            FLog.e("mAudioRecord getRecordingState STATE_UNINITIALIZED");
        }
    }

    public void release () {
        FLog.i("AudioRecorder release");
        if (mAudioRecord != null) {
            mAudioRecord.stop();
            mAudioRecord.release();
            mAudioRecord = null;
        }
    }
}
