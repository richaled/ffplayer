//
// Created by richard on 2022/2/14.
//

#pragma once
#include <stdint.h>

namespace player{
    //时钟，用作音视频同步
    typedef struct Clock {
        int64_t update_time = 0;
        int64_t pts = LONG_MAX;
        int64_t pts_drift = 0; //pts与更新时间的差值
        bool isPause = false;
    } Clock;

    uint64_t clock_get_current_time();
    Clock* clock_create();
    int64_t clock_get(Clock *clock);
    void clock_set(Clock *clock, int64_t pts);
    void clock_free(Clock *clock);
    void clock_reset(Clock *clock);
    void sync_clock_to_slave(Clock *clock,Clock *slave);
}


