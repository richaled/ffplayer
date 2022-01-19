package com.test.ffmpegdemo.player;

import com.test.ffmpegdemo.NativeObjectRef;

public class MediaPlayer2 extends NativeObjectRef {

    public MediaPlayer2(){
        this(nativeCreateMediaPlayer());
    }


    private MediaPlayer2(long wrapPtr) {
        super(wrapPtr, "MediaPlayer2");
    }

    public void start(){
        nativeStart();
    }


    private static native long nativeCreateMediaPlayer();
    private native void nativeStart();

}
