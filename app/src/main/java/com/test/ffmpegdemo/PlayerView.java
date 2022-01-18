package com.test.ffmpegdemo;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import static android.content.Context.ACTIVITY_SERVICE;

/**
 * 播放器view
 */
public class PlayerView extends FrameLayout {

    private static final String TAG = PlayerView.class.getName();
    private GLSurfaceView glSurfaceView;
    private VideoPlayer videoPlayer;
    public PlayerView(@NonNull Context context) {
        this(context,null);
    }

    public PlayerView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs,0);
    }

    public PlayerView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr,0);
    }

    public PlayerView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);

        glSurfaceView = new GLSurfaceView(context);
        final ActivityManager activityManager = (ActivityManager) context.getSystemService(ACTIVITY_SERVICE);
        final ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
        final boolean supportsGLES3 = configurationInfo.reqGlEsVersion >= 0x30000;
        if(supportsGLES3){
            glSurfaceView.setEGLContextClientVersion(3);
        }else{
            Log.i(TAG,"not support gles3");
        }
        this.addView(glSurfaceView);

        videoPlayer = new VideoPlayer(context,glSurfaceView);
    }

    public int prepare(String url){
        return videoPlayer.prepare(url);
    }

    public void play(){
        videoPlayer.play();
    }

    public void pause(){
        videoPlayer.pause();
    }

    public void stop(){
        videoPlayer.stop();
    }

    public void release(){
        videoPlayer.release();
    }

    public boolean isPrepare(){
        return videoPlayer.isPrepare();
    }

    public void seek(float time){
        videoPlayer.seekTo(time);
    }

    public float getDuration(){
        return videoPlayer.getDuration();
    }

    public void setPlayStateCallback(VideoPlayer.OnPlayerStateCallback onPlayerStateCallback){
        videoPlayer.setPlayerStateCallback(onPlayerStateCallback);
    }

    public void setPlayProgressCallback(VideoPlayer.OnPlayProgressCallback playProgressCallback){
        videoPlayer.setPlayProgressCallback(playProgressCallback);
    }

}
