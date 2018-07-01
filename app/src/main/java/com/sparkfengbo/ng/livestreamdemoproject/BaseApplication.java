package com.sparkfengbo.ng.livestreamdemoproject;

import com.sparkfengbo.ng.livestreamdemoproject.nativehandler.NativeCrashHandler;
import com.sparkfengbo.ng.livestreamdemoproject.util.AppCrashHandler;

import android.app.Application;

/**
 * Created by fengbo on 2018/6/27.
 */

public class BaseApplication extends Application {



    @Override
    public void onCreate() {
        super.onCreate();
        AppCrashHandler crashHandler = AppCrashHandler.getInstance();
        crashHandler.init(this);

//        NativeCrashHandler nativeCrashHandler = new NativeCrashHandler();
//        nativeCrashHandler.registerForNativeCrash(this);
    }
}