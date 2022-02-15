package com.test.ffmpegdemo.player;

/**
 * 视频片段
 */
public class MediaClip {
    public String path;
    public TimeRange timeRange;
    public int type;//类型

    public static class TimeRange{
        public TimeRange(){

        }

        public TimeRange(long startTime, long endTime) {
            this.startTime = startTime;
            this.endTime = endTime;
        }

        public long startTime = 0;
        public long endTime = 0;
    }
}
