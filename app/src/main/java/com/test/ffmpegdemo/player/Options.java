package com.test.ffmpegdemo.player;

import com.test.ffmpegdemo.NativeObjectRef;

public class Options extends NativeObjectRef{

    public Options() {
        super(createNativeWrap(), "Options");
    }

    public static class OptionKey{
        public static final int kEnableHardware = 1;
    }

    private static native long createNativeWrap();

    public native void setBool(int key, boolean value);
    public native void setInt(int key, int value);
    public native void setInt64(int key, long value);
    public native void setDouble(int key, double value);
    public native void setString(int key, String value);
    public native void setVoidPtr(int key, long ptr);
}
