package com.sparkfengbo.ng.livestreamdemoproject.rtmptest;

import com.sparkfengbo.ng.livestreamdemoproject.base.CameraPreview;
import com.sparkfengbo.ng.livestreamdemoproject.recorder.AudioRecorder;
import com.sparkfengbo.ng.livestreamdemoproject.util.FLog;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;

/**
 * Created by fengbo on 2018/6/27.
 */

public class RTMPController {

    static {
        System.loadLibrary("fdk-aac");
        System.loadLibrary("avcodec");
        System.loadLibrary("avfilter");
        System.loadLibrary("avformat");
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("rtmp_lib");
    }

    private static final int MSG_CAMERA_FRAME_DATA = 1;

    private CameraPreview mCameraPreview;
    private AudioRecorder mAudioRecorder;

    private AudioRecordThread mAudioRecordThread;
    private HandlerThread mCameraRecordThread;

    private Handler mCameraDataHandler;

    private Handler.Callback mCameraCallback = new Handler.Callback() {
        @Override
        public boolean handleMessage(Message message) {
            if (message != null && message.what == MSG_CAMERA_FRAME_DATA) {
                if (message.obj instanceof byte[]) {
                    FLog.i("Camera Handler Thread receive : " + ((byte[]) message.obj).length);
                    byte[] data = (byte[]) message.obj;
                    nativePushYUVData(data, data.length);
                }
                return true;
            }
            return false;
        }
    };

    public RTMPController(CameraPreview cameraRecorder) {
        if (cameraRecorder == null) {
            throw new NullPointerException("Recorder Manager must init with NOT-NULL CameraPreview");
        }

        mCameraPreview = cameraRecorder;
        mCameraPreview.setCallback(new CameraPreview.OnFrameDataCallback() {
            @Override
            public void onFrameData(byte[] data, int length, int rotate) {
                if (mCameraDataHandler != null) {
                    Message msg = Message.obtain();
                    msg.what = MSG_CAMERA_FRAME_DATA;
                    msg.obj = data;
                    msg.arg1 = length;
                    msg.arg2 = rotate;
                    FLog.i("" + data.length);
                    mCameraDataHandler.sendMessage(msg);
                }
            }
        });
        mAudioRecorder = new AudioRecorder();
    }

    /**
     * 开始推流，需要初始化推流器
     * @return
     */
    public boolean startPush(String rtmpUrl) {
        if (rtmpUrl == null || rtmpUrl.equals("")) {
            return false;
        }

        try {
            boolean isSuccess = nativeStartPush(rtmpUrl);
            if (!isSuccess) {
                FLog.e("start push failed!");
                return false;
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }

//        if (mAudioRecordThread == null) {
//            mAudioRecordThread = new AudioRecordThread(mAudioRecorder);
//        }
//        mAudioRecordThread.start();

        if (mCameraRecordThread == null) {
            mCameraRecordThread = new HandlerThread("Camera Native Send");
            mCameraRecordThread.start();
        }

        if (mCameraDataHandler == null) {
            mCameraDataHandler = new Handler(mCameraRecordThread.getLooper(), mCameraCallback);
        }


        FLog.i("start push...");
        return true;
    }

    /**
     * 暂停推流
     */
    public void pausePush() {
        /**
         * TODO 暂停推流
         * TODO 需要处理 和 url 断开连接和重连的问题
         */
    }

    /**
     * 继续推流，需要验证推流器是否初始化完毕
     */
    public void resumePush() {

    }

    /**
     * 停止推流
     */
    public void stopPush() {
        //
        /**
         * TODO 停止推流
         */
    }

    public void release () {
        if (mAudioRecordThread != null) {
            mAudioRecordThread.onDestroy();
        }
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
            FLog.i("AudioRecordThread isStart : "  + isStart);
            if (!isStart) {
                return;
            } else {
                FLog.i("attemp read  audio data");
                while (true) {
                    byte[] audioData = mAudioRecorder.readData();
                    if (audioData != null) {
                        nativePushPCMData(audioData);
                    } else {
                        FLog.e("receive audioData empty");
                    }
                    try {
                        Thread.sleep(40);
                    } catch (InterruptedException e) {
                        FLog.e("audio thread can not sleep");
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

    /**
     *
     * @param url
     * @return
     */
    private native boolean nativeStartPush(String url);

    private native long nativePushPCMData(byte[] data);

    private native long nativePushYUVData(byte[] data, int length);


    /**
     * TODO 添加JNI层对Java层的回调，用于通知JNI对RTMP处理信息的回调
     * @param info
     * @param ret
     */
    private void onNativePushInfoCallback(String info, int ret) {

    }
}
