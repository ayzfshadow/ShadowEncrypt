package com.ayzf.shadowencrypt;

import android.app.Application;
import android.content.Context;
import android.os.Build;
import android.widget.Toast;
import androidx.annotation.RequiresApi;

public class ShadowApp extends Application
{
    static
    {
        System.loadLibrary("shadowencrypt");
    }
    @Override
    protected void attachBaseContext(Context base)
    {
        super.attachBaseContext(base);
    }
    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override
    public void onCreate()
    {
        super.onCreate();
        init((char)1,666666,"ayzf",false);
        //ShadowEncrypt ab = new ShadowEncrypt((char)1,666666,"ayzf",false);
        String en = encrypt("欢迎使用影殇加密");
        Toast.makeText(getBaseContext(),decrypt(en),Toast.LENGTH_LONG).show();
    }
    public static native void init(char a,long b,String c,boolean d);
    public static native String encrypt(String text);
    public static native String decrypt(String text);
}