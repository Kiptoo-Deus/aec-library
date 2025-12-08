package com.example.aec;

public class AecWrapper {
    static {
        System.loadLibrary("aec");
    }
    public native void init(int filterLength);
    public native void process(float[] input, float[] output);
}
