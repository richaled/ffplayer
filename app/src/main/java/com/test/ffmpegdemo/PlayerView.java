package com.test.ffmpegdemo;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;

public class PlayerView extends GLSurfaceView {

    private PlayerRender mPlayerRender;
    public PlayerView(Context context) {
        this(context,null);
    }

    public PlayerView(Context context, AttributeSet attrs) {
        super(context, attrs);

    }


    public void prepare(){
        mPlayerRender = new PlayerRender();
        this.setRenderer(mPlayerRender);
    }



}
