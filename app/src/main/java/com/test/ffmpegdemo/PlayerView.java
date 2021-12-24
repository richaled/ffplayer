package com.test.ffmpegdemo;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceView;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

/**
 * 播放器view
 */
public class PlayerView extends FrameLayout {

//    private SurfaceView surfaceView;
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
        videoPlayer = new VideoPlayer(context);
        PlayerRender playerRender = new PlayerRender();
        glSurfaceView.setRenderer(playerRender);
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




}
