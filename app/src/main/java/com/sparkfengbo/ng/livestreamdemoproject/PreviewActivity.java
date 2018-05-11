package com.sparkfengbo.ng.livestreamdemoproject;

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


    private Button mBtnRecordAAC;

    private Button mBtnStopAAC;



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

        mBtnTurnAround.setOnClickListener(mOnClickListener);
        mBtnLignts.setOnClickListener(mOnClickListener);
        mBtnPushLive.setOnClickListener(mOnClickListener);
        mBtnConvertGif.setOnClickListener(mOnClickListener);
        mBtnRecordAAC.setOnClickListener(mOnClickListener);

        mBtnStopAAC.setOnClickListener(mOnClickListener);

        mRecorderManager = new RecorderManager(mCameraRecorder);
    }

    private View.OnClickListener mOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            if (view == mBtnTurnAround) {
                if (mCameraRecorder != null) {
                    mCameraRecorder.switchCamera();
                }
            } else if (view == mBtnLignts) {
                //TODO 闪光灯
            } else if  (view == mBtnPushLive) {
                //TODO 开始推流
                //TODO 摄像头翻转后的推流
                mRecorderManager.startLivePush();
            } else if (view == mBtnConvertGif) {
                //参考文章 https://blog.csdn.net/kong_gu_you_lan/article/details/79707513
                if (mRecorderManager == null) {
                    return;
                }
                /**
                 * 首先确保你的路径正确
                 *
                 * 我的测试路径    /sdcard/DCIM/Camera/VID_20180504_133613.mp4
                 *
                 */
                String path = Environment.getExternalStorageDirectory().getAbsolutePath()
                        + "/DCIM/Camera/";

                final String source = "VID_20180504_133613.mp4";

                final String dest = "test.gift";

                File file = new File(path);
                if (!file.exists()) {
                    Mog.e("File path = " + path + " not  exits");
                    return;
                }

                final String cmd = "ffmpeg -i " + path + source + " -vframes 100 -y -f gif -s 480×320 " + path + dest;

                int a  = mRecorderManager.cmdRun(cmd);
                Mog.i("执行命令" + a + "");
            } else if (view == mBtnRecordAAC) {
                String path = Environment.getExternalStorageDirectory().getAbsolutePath();
                mRecorderManager.startRecordAAC();
            } else if (view ==mBtnStopAAC) {
                mRecorderManager.stopAAC();

            }
        }
    };

}
