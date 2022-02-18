package com.test.ffmpegdemo.player;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.SurfaceTexture;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.test.ffmpegdemo.R;

public class MediaPlayerView extends FrameLayout implements SurfaceHolder.Callback, TextureView.SurfaceTextureListener, View.OnClickListener {

    private static final String TAG = MediaPlayerView.class.getName();
    private SurfaceView surfaceView;
    private TextureView textureView;

    private MediaPlayer2 mediaPlayer;
    private boolean useTextureView = false;
    private ImageView imageView;

    public MediaPlayerView(@NonNull Context context) {
        super(context);
        initView(context);
    }

    public MediaPlayerView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MediaPlayerView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        TypedArray ta = context.obtainStyledAttributes(attrs, R.styleable.MediaPlayerView);
        useTextureView = ta.getBoolean(R.styleable.MediaPlayerView_usetextureview,false);
        initView(context);
    }

    void initView(Context context){
        if(useTextureView){
            textureView = new TextureView(context);
            textureView.setSurfaceTextureListener(this);
            addView(textureView);
        }else {
            surfaceView = new SurfaceView(context);
            surfaceView.getHolder().addCallback(this);
            addView(surfaceView);
        }
        imageView = new ImageView(context);
        imageView.setImageResource(R.drawable.ic_play);
        LayoutParams layoutParams = new LayoutParams(80, 80);
        layoutParams.gravity = Gravity.CENTER;
        addView(imageView,layoutParams);

        mediaPlayer = new MediaPlayer2();
        imageView.setOnClickListener(this);
        setTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if(imageView.getVisibility() == GONE){
                    imageView.setVisibility(VISIBLE);
                }
                return false;
            }
        });
    }

    //surface view
    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.i(TAG,"surfaceCreated");
        mediaPlayer.createSurface(holder.getSurface());
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Log.i(TAG,"surfaceChanged");
        mediaPlayer.surfaceSizeChange(width,height);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        mediaPlayer.destorySurface();
    }

    //textureview
    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surface, int width, int height) {
        Surface surface1 = new Surface(surface);
        mediaPlayer.createSurface(surface1);

    }

    @Override
    public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surface, int width, int height) {
        mediaPlayer.surfaceSizeChange(width,height);
    }

    @Override
    public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surface) {
        mediaPlayer.destorySurface();
        return false;
    }

    @Override
    public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surface) {

    }

    public MediaPlayer2 getMediaPlayer(){
        return mediaPlayer;
    }


    public void setTouchListener(OnTouchListener onTouchListener){
        if(useTextureView){
            textureView.setOnTouchListener(onTouchListener);
        }else {
            surfaceView.setOnTouchListener(onTouchListener);
        }
    }

    @Override
    public void onClick(View v) {
        if(!mediaPlayer.isMediaSourceSet()){
            Toast.makeText(getContext(),"请先设置播放数据源",Toast.LENGTH_SHORT).show();
            return;
        }

        if(mediaPlayer.isPlaying()){
            mediaPlayer.pause();
            imageView.setImageResource(R.drawable.ic_play);
        }else {
            if(!mediaPlayer.isPrepare()){
                mediaPlayer.play();
            }else {
                mediaPlayer.resume();
            }
            imageView.setImageResource(R.drawable.ic_pause);
        }
        imageView.setVisibility(GONE);
    }
}
