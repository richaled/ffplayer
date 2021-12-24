package com.test.ffmpegdemo;

import android.util.Log;

public class NativeObjectRef {
    private long mWrapPtr;
    private String mName = "Unknown";
    private boolean mMoved;
    private boolean mRequireFreeManually;

    static {
        System.loadLibrary("avformat");
        System.loadLibrary("avcodec");
        System.loadLibrary("avfilter");
        System.loadLibrary("ffmpegdemo");
    }

    public long getWrapPtr() {
        return mWrapPtr;
    }

    public long moveGetWrapPtr() {
        if(mMoved) {
            throw new RuntimeException(mName + " has moved");
        }
        mMoved = true;
        long ptr = mWrapPtr;
        mWrapPtr = 0;
        return ptr;
    }

    public boolean isMoved() {
        return mMoved;
    }

    public void setRequireFreeManually() {
        mRequireFreeManually = true;
    }

    public void setRequireFreeManually(boolean requireFreeManually) {
        mRequireFreeManually = requireFreeManually;
    }

    /**
     * Free the native pointer.
     * It's safe to free more than once, though it do nothing to free a object which has freed.
     * NOTE
     * 1: because the native pointer hold a std::shared_ptr of the real underlying object,
     * free the native pointer doesn't means the underlying object, it just cause the ref_count of the shared_ptr
     * decrease 1. So if other component holds the shared_ptr, the underlying object is still valid.
     * 2. copy the java object of the NativeObjectRef won't affect the underlying shared_ptr in native pointer, because
     * you just copy the java reference of the NativeObjectRef, underlying shared_ptr not copied.
     * 3. pass the object to some native component may increase the underlying shared_ptr, such as pass RenderModel
     * to HybridRender, in c++, HybridRender will hold the shared_ptr pass from RenderModel java object
     */
    // @CallSuper
    public void free() {
//        Log.i("NativeObjectRef"," free name " + getName());
        doFree(false);
    }

    private void doFree(boolean finalize) {
        if(!mMoved && mWrapPtr != 0) {
            if(finalize && mRequireFreeManually) {
                Log.e("ins", "Freeing NativeObjectRef: " + mName + "(" + mWrapPtr + ")"
                        + " in finalize! but this object require free manually!!!");
            }
            nativeFree();
            mWrapPtr = 0;
        }
    }

    @Override
    protected void finalize() throws Throwable {
//        Log.i("NativeObjectRef"," finalize name " + mName + " mWrapPtr "
//                + mWrapPtr + " tid " + Thread.currentThread().getId());
        doFree(true);
        super.finalize();
    }

    // use to move from another object ref instance
    public NativeObjectRef(long wrapPtr, String name) {
        mWrapPtr = wrapPtr;
        mName = name;
//        Log.i("NativeObjectRef"," new  name " + mName + " mWrapPtr " + mWrapPtr);
    }

    public boolean hasSameNativeObject(Object ref) {
        if(ref == null) {
            return false;
        }
        if(ref instanceof NativeObjectRef) {
            return nativeHasSameNativeObject((NativeObjectRef) ref);
        }
        return false;
    }

    public String getName()
    {
        return mName;
    }

    private native boolean nativeHasSameNativeObject(NativeObjectRef ref);

    private native void nativeFree();

}
