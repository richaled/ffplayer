package com.test.ffmpegdemo;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class PlayerRender  extends NativeObjectRef implements GLSurfaceView.Renderer {

    public PlayerRender() {
        this(nativeCreateVertexObj());
    }

    private PlayerRender(long wrapPtr) {
        super(wrapPtr, "VertexObjRender");
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        nativeSurfaceCreated();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        nativeSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        nativeDrawFrame();
    }

    private static native long nativeCreateVertexObj();

    private native void nativeSurfaceCreated();

    private native void nativeSurfaceChanged(int width, int height);

    private native void nativeDrawFrame();
}
