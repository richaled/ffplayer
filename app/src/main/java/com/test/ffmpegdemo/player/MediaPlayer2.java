package com.test.ffmpegdemo.player;

import com.test.ffmpegdemo.NativeObjectRef;

public class MediaPlayer2 extends NativeObjectRef {

    private static final String TAG = MediaPlayer2.class.getName();
    public MediaPlayer2(){
        this(nativeCreateMediaPlayer());
    }


    private MediaPlayer2(long wrapPtr) {
        super(wrapPtr, "MediaPlayer2");
    }

    public void prepare(MediaClip[] mediaClips){
        nativePrepare(mediaClips);
    }

    public boolean isPrepare(){
        return nativeIsPrepare();
    }


    public void play(){

    }

    public void pause(){

    }

    public void stop(){

    }

    public void release(){

    }

    public void seek(float timeMs){

    }



    private static native long nativeCreateMediaPlayer();
    private native void nativePrepare(MediaClip[] mediaClips);
    private native boolean nativeIsPrepare();

}
