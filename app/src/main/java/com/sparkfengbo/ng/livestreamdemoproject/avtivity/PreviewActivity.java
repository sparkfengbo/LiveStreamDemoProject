package com.sparkfengbo.ng.livestreamdemoproject.avtivity;

import com.sparkfengbo.ng.livestreamdemoproject.CameraPreview;
import com.sparkfengbo.ng.livestreamdemoproject.R;
import com.sparkfengbo.ng.livestreamdemoproject.RecorderManager;
import com.sparkfengbo.ng.livestreamdemoproject.util.Mog;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;

public class PreviewActivity extends Activity {
    /**
     * 翻转摄像头
     */
    private Button mBtnTurnAround;
    /**
     * 打开/关闭闪光灯
     */
    private Button mBtnLignts;
    /**
     * 开始/结束推流
     */
    private Button mBtnPushLive;
    /**
     * 转GIF
     */
    private Button mBtnConvertGif;
    /**
     * 推流或展示其他信息
     */
    private TextView mPushInfoTextView;
    /**
     * 录制AAC音频
     */
    private Button mBtnRecordAAC;
    /**
     * 停止录制AAC音频
     */
    private Button mBtnStopAAC;
    /**
     * 录制h264
     */
    private Button mBtnStart264;
    /**
     * 停止录制h264
     */
    private Button mBtnStop264;
    /**
     * 合成MP4视频
     */
    private Button mBtnMuxMP4;

    private Button mBtnRtmp;

    /**
     * 摄像头预览和采集
     */
    private CameraPreview mCameraRecorder;

    private RecorderManager mRecorderManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_preview);

        mBtnTurnAround = (Button) findViewById(R.id.btn_turn_around);
        mBtnLignts = (Button) findViewById(R.id.btn_lignts);
        mBtnPushLive = (Button) findViewById(R.id.btn_push_live);
        mCameraRecorder = (CameraPreview) findViewById(R.id.camera_preview);
        mPushInfoTextView = (TextView) findViewById(R.id.push_info);
        mBtnConvertGif = (Button) findViewById(R.id.btn_convert_gift);
        mBtnRecordAAC = (Button) findViewById(R.id.btn_record_aac);
        mBtnStopAAC = (Button) findViewById(R.id.btn_stop_aac);
        mBtnStart264 = (Button) findViewById(R.id.btn_record_h264);
        mBtnStop264 = (Button) findViewById(R.id.btn_stop_h264);
        mBtnMuxMP4 = (Button) findViewById(R.id.btn_mux_mp4);
        mBtnRtmp = (Button) findViewById(R.id.btn_rtmp);


        mBtnTurnAround.setOnClickListener(mOnClickListener);
        mBtnLignts.setOnClickListener(mOnClickListener);
        mBtnPushLive.setOnClickListener(mOnClickListener);
        mBtnConvertGif.setOnClickListener(mOnClickListener);
        mBtnRecordAAC.setOnClickListener(mOnClickListener);
        mBtnStopAAC.setOnClickListener(mOnClickListener);
        mBtnStart264.setOnClickListener(mOnClickListener);
        mBtnStop264.setOnClickListener(mOnClickListener);
        mBtnMuxMP4.setOnClickListener(mOnClickListener);
        mBtnRtmp.setOnClickListener(mOnClickListener);

        mRecorderManager = new RecorderManager(mCameraRecorder);
    }


    /**
     * TODO : 1. 闪光灯
     * TODO : 2. 开始推流
     * TODO : 3. 摄像头翻转后的配置动态化
     * TODO : 4. 开始和停止录制的button做逻辑处理
     * TODO : 5. 录制队列化，不是将AAC和H264合成
     * TODO : ...
     */
    private View.OnClickListener mOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            if (view == mBtnTurnAround) {
                if (mCameraRecorder != null) {
                    mCameraRecorder.switchCamera();
                }
            } else if (view == mBtnLignts) {

            } else if  (view == mBtnPushLive) {
                mRecorderManager.startLivePush();
            } else if (view == mBtnStart264) {
                mRecorderManager.startRecordH264();
            } else if (view == mBtnStop264) {
                mRecorderManager.stopRecordH264();
            } else if (view == mBtnMuxMP4) {
                mRecorderManager.startMuxMP4();
            } else if (view == mBtnRecordAAC) {
                String path = Environment.getExternalStorageDirectory().getAbsolutePath();
                mRecorderManager.startRecordAAC();
            } else if (view ==mBtnStopAAC) {
                mRecorderManager.stopAAC();
            } else if (view == mBtnConvertGif) {
                //参考文章 https://blog.csdn.net/kong_gu_you_lan/article/details/79707513
                if (mRecorderManager == null) {
                    return;
                }
                /**
                 * 首先确保你的路径正确
                 * 我的测试路径    /sdcard/DCIM/Camera/VID_20180504_133613.mp4
                 */
                String path = Environment.getExternalStorageDirectory().getAbsolutePath()
                        + "/DCIM/Camera/";
                final String source = "VID_20180504_133613.mp4";
                final String dest = "test.gif";
                File file = new File(path);
                if (!file.exists()) {
                    Mog.e("File path = " + path + " not  exits");
                    return;
                }
                final String cmd = "ffmpeg -i " + path + source + " -vframes 100 -y -f gif -s 480×320 " + path + dest;
                int a  = mRecorderManager.cmdRun(cmd);
                Mog.i("执行命令" + a + "");
            } else if (view == mBtnRtmp) {
                mRecorderManager.startPushRtmp();
            }
        }
    };

}
