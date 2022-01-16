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

    public VideoPlayer(Context context,GLSurfaceView glSurfaceView){
        this(nativeCreateVideoPlayer());
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
        int ret = nativePrepare(url,playerRender);
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

    public interface IPlayerStatusCallback{

    }

    private static native long nativeCreateVideoPlayer();
    private native int nativePrepare(String url,PlayerRender playerRender);
    private native void nativePlay();
    private native void nativePause();
    private native void nativeStop();
    private native void nativeRelease();

    public interface OnPlayerStateCallback{
        void onPLayState();
    }

    public interface OnPlayProgressCallback{
        void onPlayProgress(float progress, float total);
    }
}
