package com.sparkfengbo.ng.livestreamdemoproject;

import com.sparkfengbo.ng.livestreamdemoproject.recorder.AudioRecorder;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;

/**
 * Created by fengbo on 2018/5/2.
 */

public class RecorderManager {

    private static final int MSG_CAMERA_FRAME_DATA = 1;
    private CameraPreview mCameraRecorder;
    private AudioRecorder mAudioRecorder;

    private AudioRecordThread mAudioRecordThread;
    private HandlerThread mCameraRecordTherad;

    private Handler mMainThreadHandler;
    private Handler mCameraDataHandler;

    static {
        System.loadLibrary("fdk-aac");
        System.loadLibrary("avcodec");
        System.loadLibrary("avfilter");
        System.loadLibrary("avformat");
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("record_util");
    }

    public RecorderManager(CameraPreview cameraRecorder) {
        if (cameraRecorder == null) {
            throw new NullPointerException();
        }

        mCameraRecorder = cameraRecorder;
        mCameraRecorder.setCallback(new CameraPreview.OnFrameDataCallback() {
            @Override
            public void onFrameData(byte[] data, int length, int rotate) {
                if (mCameraDataHandler != null) {
                    Message msg = Message.obtain();
                    msg.what = MSG_CAMERA_FRAME_DATA;
                    msg.obj = data;
                    msg.arg1 = length;
                    msg.arg2 = rotate;
                    mCameraDataHandler.sendMessage(msg);
                }
            }
        });
        mAudioRecorder = new AudioRecorder();

        mMainThreadHandler = new Handler(Looper.getMainLooper());
    }

    public void startLivePush() {
//        Mog.i("startLivePush");
//        if (mAudioRecordThread == null) {
//            mAudioRecordThread = new AudioRecordThread(mAudioRecorder);
//        }
//
//        if (mCameraRecordTherad == null) {
//            mCameraRecordTherad = new HandlerThread("CameraSendData");
//            mCameraRecordTherad.start();
//        }
//
//        if (mCameraDataHandler == null) {
//            mCameraDataHandler = new Handler(mCameraRecordTherad.getLooper(), mCameraCallback);
//        }
//
//        mAudioRecordThread.start();
    }

    private Handler.Callback mCameraCallback = new Handler.Callback() {
        @Override
        public boolean handleMessage(Message message) {
            if (message != null && message.what == MSG_CAMERA_FRAME_DATA) {
                if (message.obj instanceof byte[]) {
                    nativeSendYUVData((byte[]) message.obj);
                }
                return true;
            }
            return false;
        }
    };

    public void release() {
        if (mAudioRecordThread != null) {
//            mAudioRecordThread.stop();
            mAudioRecordThread.onDestroy();
        }
    }

    public void startRecordAAC() {
//        if (mAudioRecordThread != null) {
//            mAudioRecordThread.interrupt();
//            mAudioRecordThread.onDestroy();
//        }

        if (mAudioRecordThread == null) {
            mAudioRecordThread = new AudioRecordThread(mAudioRecorder);
        }

        initAACEncoder();
        mAudioRecordThread.start();

    }

    public void stopAAC() {
        Mog.i("stop encode");
        stopEncodeAAC();
    }


    public class AudioRecordThread extends Thread {

        private AudioRecorder mAudioRecorder;

        public AudioRecordThread(AudioRecorder audioRecorder) {
            mAudioRecorder = audioRecorder;
        }

        @Override
        public void run() {
            if (mAudioRecorder == null) {
                return;
            }
            boolean isStart = mAudioRecorder.startRecord();
            Mog.i("AudioRecordThread isStart : "  + isStart);
            if (!isStart) {
                return;
            } else {
                Mog.i("attemp read  audio data");
                while (true) {
                    byte[] audioData = mAudioRecorder.readData();
                    if (audioData != null) {
                        long legnth = nativeSendPCMData(audioData);
//                        Mog.i("length = " + legnth);
//                        Mog.i("receive audioData, audioData.length : " + audioData.length + "  thread : " + Thread.currentThread().getName());
                    } else {
                        Mog.e("receive audioData empty");
                    }

                    //TODO
//                    try {
//                        sleep(40);
//                    } catch (InterruptedException e) {
//                        e.printStackTrace();
//                    }
                }
            }
        }

        public void onDestroy() {
            if (mAudioRecorder != null) {
                mAudioRecorder.release();
            }
        }
    }


    /**
     * 以空格分割指令，生成String类型的数组
     *
     * @param cmd 指令
     * @return 执行code
     */
    public int cmdRun(String cmd) {
        String regulation = "[ \\t]+";
        final String[] split = cmd.split(regulation);
        return run(split);
    }


    /*********************** Native方法 ***********************/
    /**
     * ffmpeg_cmd中定义的run方法
     *
     * @param cmd 指令
     * @return 执行code
     */
    public native int run(String[] cmd);

    public native long nativeSendPCMData(byte[] data);

    public native long nativeSendYUVData(byte[] data);

    private native void initAACEncoder();

    private native void stopEncodeAAC();

}
