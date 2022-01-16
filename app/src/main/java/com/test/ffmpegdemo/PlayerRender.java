package com.test.ffmpegdemo;

import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class PlayerRender extends NativeObjectRef implements GLSurfaceView.Renderer {
    private static final String TAG = PlayerRender.class.getName();

    public PlayerRender(){
        this(nativeCreateRender());
    }

    private PlayerRender(long wrapPtr) {
        super(wrapPtr,"PlayerRender");
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
//        Log.i(TAG,"onSurfaceCreated");
        nativeOnSurfaceCreated();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
//        Log.i(TAG,"onSurfaceChanged");
        nativeonSurfaceChanged(width,height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        nativeonDrawFrame();
    }

    private static native long nativeCreateRender();
    private native void nativeOnSurfaceCreated();
    private native void nativeonSurfaceChanged(int width, int height);
    private native void nativeonDrawFrame();

}
