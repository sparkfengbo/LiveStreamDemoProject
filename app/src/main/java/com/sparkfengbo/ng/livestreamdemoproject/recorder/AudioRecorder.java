package com.sparkfengbo.ng.livestreamdemoproject.recorder;

import com.sparkfengbo.ng.livestreamdemoproject.util.Mog;

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
        Mog.i("init AudioRecorder");
        int bufferSize = AudioRecord.getMinBufferSize(RecordConfig.AUDIO_SAMPLE_RATE,
                RecordConfig.AUDIO_FORMAT_CHANNEL_FOR_RECORD,
                RecordConfig.AUDIO_FORMAT_BIT);

        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                RecordConfig.AUDIO_SAMPLE_RATE,
                RecordConfig.AUDIO_FORMAT_CHANNEL_FOR_RECORD,
                RecordConfig.AUDIO_FORMAT_BIT,
                bufferSize);

        if (mAudioRecord.getState() == AudioRecord.STATE_UNINITIALIZED) {
            Mog.e("error : AudioRecord getState =  STATE_UNINITIALIZED");
            throw new RuntimeException("error : AudioRecord init failed!");
        }
    }

    public boolean startRecord () {
        Mog.i("java audio start record");

        if (mBuffer == null) {
            mBuffer = new byte[BUFFER_LENGTH];
        }

        if (mAudioRecord != null && mAudioRecord.getState() == AudioRecord.STATE_INITIALIZED) {
            try {
                Mog.i("AudioRecorder startRecording");
                mAudioRecord.startRecording();
            } catch (IllegalStateException e) {
                Mog.e("error : mAudioRecord startRecording fail catch exp");
                return false;
            }
            if (mAudioRecord.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
                Mog.e("mAudioRecord getRecordingState not RECORDSTATE_RECORDING");
                return false;
            }
            return true;
        } else {
            Mog.e("mAudioRecord getRecordingState STATE_UNINITIALIZED");
        }
        return false;
    }


    public byte[] readData() {
        if (mAudioRecord != null) {
            int numOfByte = mAudioRecord.read(mBuffer, 0, BUFFER_LENGTH);
            if (numOfByte == BUFFER_LENGTH) {
                return mBuffer;
            } else {
                Mog.e("audiorecorder read data error. return  " + numOfByte);
            }
        }
        return null;
    }

    public void stopRecord() {
        if (mAudioRecord != null && mAudioRecord.getState() == AudioRecord.STATE_INITIALIZED) {
            try {
                Mog.i("AudioRecorder startRecording");
                mAudioRecord.stop();
            } catch (IllegalStateException e) {
                Mog.e("error : mAudioRecord startRecording fail catch exp");
            }
        } else {
            Mog.e("mAudioRecord getRecordingState STATE_UNINITIALIZED");
        }
    }

    public void release () {
        Mog.i("AudioRecorder release");
        if (mAudioRecord != null) {
            mAudioRecord.stop();
            mAudioRecord.release();
            mAudioRecord = null;
        }
    }
}
