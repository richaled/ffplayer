package com.test.ffmpegdemo.player;

import android.view.Surface;

import com.test.ffmpegdemo.NativeObjectRef;

public class MediaPlayer2 extends NativeObjectRef {

    private static final String TAG = MediaPlayer2.class.getName();

    public MediaPlayer2() {
        this(nativeCreateMediaPlayer());
    }

    private MediaPlayer2(long wrapPtr) {
        super(wrapPtr, "MediaPlayer2");
    }

    public void createSurface(Surface surface) {
        nativeCreateSurface(surface);
    }

    public void surfaceSizeChange(int width, int height){
        nativeSurfaceSizeChange(width,height);
    }

    public void destorySurface(){
        nativeDestorySurface();
    }


    public void prepare(MediaClip[] mediaClips) {
        nativePrepare(mediaClips);
    }

    public boolean isPrepare() {
        return nativeIsPrepare();
    }

    /**
     * 从头开始播放
     */
    public void play() {
        
    }

    /**
     * 播放指定片段的区间
     */
    public void playRange() {

    }

    public void pause() {

    }

    public void stop() {

    }

    public void release() {

    }

    public void seek(float timeMs) {

    }


    private static native long nativeCreateMediaPlayer();

    private native void nativePrepare(MediaClip[] mediaClips);

    private native boolean nativeIsPrepare();

    private native void nativeCreateSurface(Surface surface);

    private native void nativeSurfaceSizeChange(int width, int height);

    private native void nativeDestorySurface();

}
