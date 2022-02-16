package com.test.ffmpegdemo.player;

import android.view.Surface;

import com.test.ffmpegdemo.NativeObjectRef;

import java.util.HashMap;
import java.util.Map;

public class MediaPlayer2 extends NativeObjectRef {

    private static final String TAG = MediaPlayer2.class.getName();

    public MediaPlayer2() {
        this(nativeCreateMediaPlayer());
    }

    private MediaPlayer2(long wrapPtr) {
        super(wrapPtr, "MediaPlayer2");
    }

    private boolean mMediaSourceSet = false;
    private OnPlayStateListener mOnPlayStateListener;

    public void createSurface(Surface surface) {
        nativeCreateSurface(surface);
    }

    public void surfaceSizeChange(int width, int height){
        nativeSurfaceSizeChange(width,height);
    }

    public void destorySurface(){
        nativeDestorySurface();
    }

    public void prepare(Options options) {
        nativePrepare(options);
    }

    public boolean isPrepare() {
        return nativeIsPrepare();
    }

    /**
     * 设置播放的媒体片段
     * @param mediaClips
     */
    public void setMediaClipSource(MediaClip[] mediaClips){
        nativeSetMediaClipSource(mediaClips);
        mMediaSourceSet = mediaClips != null;
    }

    /**
     * 添加播放片段
     * @param mediaClip
     * @return
     */
    public int addClip(MediaClip mediaClip){
        return nativeAddClip(mediaClip);
    }

    /**
     * 播放
     */
    public void play() {
        if(!mMediaSourceSet){
            if(mOnPlayStateListener != null){
            }
            return;
        }
        nativePlay();
    }

    /**
     * 暂停播放
     */
    public void pause() {

    }

    /**
     * 停止播放
     */
    public void stop() {
        nativeStop();
    }


    public void release() {

    }

    public void seek(float timeMs) {

    }


    private static native long nativeCreateMediaPlayer();

    private native void nativeSetMediaClipSource(MediaClip[] mediaClips);
    private native int nativeAddClip(MediaClip mediaClip);
    private native void nativePrepare(Options options);
    private native void nativePlay();
    private native void nativeStop();

    private native boolean nativeIsPrepare();

    private native void nativeCreateSurface(Surface surface);

    private native void nativeSurfaceSizeChange(int width, int height);

    private native void nativeDestorySurface();

    public interface OnPlayStateListener{

    }
}
