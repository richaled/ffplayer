package com.test.ffmpegdemo;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.util.Log;
import android.view.Surface;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static android.content.Context.ACTIVITY_SERVICE;

public class VideoPlayer extends NativeObjectRef {
    private static final String TAG = VideoPlayer.class.getName();

    private PlayerRender playerRender;
    private Context mContext;
    private boolean mIsPrepare = false;
    private boolean mIsRelease = false;
    private OnPlayerStateCallback mPlayerStateCallback;
    private OnPlayProgressCallback mPlayProgressCallback;

    public VideoPlayer(Context context,GLSurfaceView glSurfaceView){
        this(nativeCreateVideoPlayer());
        nativeSetCallback();
        this.mContext = context;
        playerRender = new PlayerRender();
        glSurfaceView.setRenderer(playerRender);
    }

    private VideoPlayer(long wrapPtr) {
        super(wrapPtr, "VideoPlayer");
    }

    /**
     * 配置播放参数
     * @return
     */
    public int config(){
        return 0;
    }

    public int prepare(String url){
        if(mIsPrepare){
            Log.i(TAG,"aready prepared !");
            return PlayState.SUCCESS;
        }
        int ret = nativePrepare(url,playerRender);
        mIsPrepare = ret == PlayState.SUCCESS;
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

    public void seekTo(float time){
        nativeSeekTo(time);
    }

    public float getDuration(){
        return nativeGetDuration();
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

    public void setPlayerStateCallback(OnPlayerStateCallback mPlayerStateCallback) {
        this.mPlayerStateCallback = mPlayerStateCallback;
    }

    public void setPlayProgressCallback(OnPlayProgressCallback mPlayProgressCallback) {
        this.mPlayProgressCallback = mPlayProgressCallback;
    }

    //jni callback method
    private void onStateChange(int state){
        if(mPlayerStateCallback != null){
            mPlayerStateCallback.onPlayState(state);
        }
    }

    private void onProgress(float progress, float total){
        if(mPlayProgressCallback != null){
            mPlayProgressCallback.onPlayProgress(progress, total);
        }
    }

    private static native long nativeCreateVideoPlayer();
    private native int nativePrepare(String url,PlayerRender playerRender);
    private native void nativePlay();
    private native void nativePause();
    private native void nativeStop();
    private native void nativeRelease();
    private native void nativeSeekTo(float time);
    private native float nativeGetDuration();
    private native void nativeSetCallback();

    public interface OnPlayerStateCallback{
        void onPlayState(int state);
    }

    public interface OnPlayProgressCallback{
        void onPlayProgress(float progress, float total);
    }

    public static class PlayState{
        public static final int SUCCESS = 0;
        public static final int INIT_ERROR = 1;
        public static final int READY = 2;
        public static final int DONE = 3;

    }
}
