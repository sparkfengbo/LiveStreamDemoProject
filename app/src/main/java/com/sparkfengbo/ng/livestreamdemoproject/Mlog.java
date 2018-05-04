package com.sparkfengbo.ng.livestreamdemoproject;

import android.util.Log;

/**
 * Created by fengbo on 2017/7/31.
 */

public class Mlog {
    private static String TAG = "fengbo";

    public static void e(String arg) {
        StackTraceElement[] elements = Thread.currentThread().getStackTrace();
        if (elements.length < 5) {
            return;
        }
        StackTraceElement element = elements[4];
        String methodName = element.getMethodName();
        String className = element.getClassName();

        StringBuffer fullMsg = new StringBuffer(100);
        fullMsg.append(className);
        fullMsg.append(":");
        fullMsg.append(methodName);
        fullMsg.append(":");
        fullMsg.append(className);
        Log.e(TAG, fullMsg.toString() + " : " +arg);
    }
}
