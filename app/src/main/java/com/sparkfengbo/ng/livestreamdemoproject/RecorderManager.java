package com.sparkfengbo.ng.livestreamdemoproject;

/**
 * Created by fengbo on 2018/5/2.
 */

public class RecorderManager {

    private CameraRecorder mCameraRecorder;
    private AudioRecorder mAudioRecorder;

    private AudioRecordThread mAudioRecordThread;
    private CameraRecordTherad mCameraRecordTherad;

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
        Mlog.e("RecorderManager  sys load");
    }

    public RecorderManager(CameraRecorder cameraRecorder) {
        mCameraRecorder = cameraRecorder;
        mAudioRecorder = new AudioRecorder();

        System.loadLibrary("native-lib");
//        Mlog.e("RecorderManager  sys load");
    }

    public void startLivePush() {
        if (mAudioRecordThread == null) {
            mAudioRecordThread = new AudioRecordThread(mAudioRecorder);
        }

        mAudioRecordThread.start();
        Mlog.e("start live push");
    }

    public void release() {
        if (mAudioRecordThread != null) {
            mAudioRecordThread.stop();
            mAudioRecordThread.onDestroy();
        }




    }



    public class CameraRecordTherad extends Thread {

    }



    public class AudioRecordThread extends Thread {
        private AudioRecorder mAudioRecorder;

        public AudioRecordThread(AudioRecorder audioRecorder) {
            mAudioRecorder = audioRecorder;
        }

        @Override
        public void run() {
            if (mAudioRecorder == null)  {
                return;
            }

            boolean isStart = mAudioRecorder.startRecord();
            if(!isStart) {
                return;
            } else {
                while (true) {
                    byte[] audioData = mAudioRecorder.readData();
                    if (audioData != null) {
                        long legnth = sendPCMData(audioData);
                        Mlog.e("length = " + legnth);
//                        Mlog.e("receive audioData, audioData.length : " + audioData.length + "  thread : " + Thread.currentThread().getName());
                    } else {
                        Mlog.e("receive audioData empty");
                    }

                    //TODO
                    try {
                        sleep(40);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }

        public void onDestroy() {
            if (mAudioRecorder != null) {
                mAudioRecorder.release();
            }
        }
    }


    private native void run();
    public native long sendPCMData(byte[] data);

}
