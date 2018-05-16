package com.sparkfengbo.ng.livestreamdemoproject.avtivity;

import com.sparkfengbo.ng.livestreamdemoproject.R;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;


/**
 * 测试录音的aty
 *
 * 内容包含：
 *
 * 录音（1 裸流、2 ADTS封装、3、波形图、4、特效等）
 *
 * 目前完成：
 *  1：裸流封装AAC
 */
public class AudioRecordTestActivity extends Activity {
    private Button mStartBtn;
    private Button mStopBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio_record_test);

        mStartBtn = (Button) findViewById(R.id.btn_start_record_audio);
        mStopBtn = (Button) findViewById(R.id.btn_stop_record_audio);
        mStartBtn.setOnClickListener(mOnClickListener);
        mStopBtn.setOnClickListener(mOnClickListener);
    }

    private View.OnClickListener mOnClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            if (view == mStartBtn) {

            } else if (view == mStopBtn) {

            }
        }
    };


    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
