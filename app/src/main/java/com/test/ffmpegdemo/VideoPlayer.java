package com.test.ffmpegdemo;

import android.content.Context;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class VideoPlayer extends NativeObjectRef{

    private GLSurfaceView surfaceView;
    private Context mContext;
    private boolean mIsPrepare = false;
    private boolean mIsRelease = false;

    public VideoPlayer(Context context){
        this(nativeCreateVideoPlayer());
        this.mContext = context;

        surfaceView = new GLSurfaceView(mContext);
        VideoRender videoRender = new VideoRender();
        surfaceView.setRenderer(videoRender);
    }

    private VideoPlayer(long wrapPtr) {
        super(wrapPtr, "VideoPlayer");
    }


    private class VideoRender implements GLSurfaceView.Renderer{

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
    }

    /**
     * 配置播放参数
     * @return
     */
    public int config(){
        return 0;
    }

    public int Prepare(String url){
        int ret = nativePrepare(url);
        mIsPrepare = ret == 0;
        return ret;
    }

    public boolean isPrepare(){
        return mIsPrepare;
    }

    public void play(){
        nativePlay();
    }

    public void pause(){
        nativePause();
    }

    public void stop(){
        nativeStop();
    }

    public void release(){
        nativeRelease();
        mIsRelease = true;
    }

    @Override
    protected void finalize() throws Throwable {
        if(mIsRelease){
            return;
        }
        release();
        super.finalize();
    }

    private static native long nativeCreateVideoPlayer();
    private native int nativePrepare(String url);
    private native void nativePlay();
    private native void nativePause();
    private native void nativeStop();
    private native void nativeRelease();

    private native void nativeSurfaceCreated();
    private native void nativeSurfaceChanged(int width, int height);
    private native void nativeDrawFrame();
}
