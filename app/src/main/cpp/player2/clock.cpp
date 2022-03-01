#include <cstdlib>
#include "clock.h"
#include <sys/time.h>
#include <log/log.h>

extern "C"{
    #include <libavutil/time.h>
}

namespace player{
    uint64_t clock_get_current_time() {
        struct timeval tv;
        gettimeofday(&tv, NULL);

//        return av_gettime_relative();
        return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
    }

    Clock* clock_create() {
        Clock* clock = (Clock*) malloc(sizeof(Clock));
        clock_set(clock,LONG_MAX);
        return clock;
    }

    int64_t clock_get(Clock *clock) {
        if (clock->pts == LONG_MAX) {
            LOGI("pts is nan");
            return LONG_MAX;
        }
//        LOGI("get pts : %ld",clock->pts);
        return clock->pts + clock_get_current_time();
    }

    void clock_set(Clock *clock, int64_t pts) {
        clock->update_time = clock_get_current_time();
        clock->pts = pts;
        clock->pts_drift = pts - clock->update_time;
//        LOGI("set pts : %ld",clock->pts);
    }

    void clock_free(Clock *clock) {
        free(clock);
    }

    void clock_reset(Clock *clock) {
        clock->pts = 0;
        clock->update_time = 0;
        clock->pts_drift = 0;
    }

    void sync_clock_to_slave(Clock *c,Clock *slave){
        int64_t clock = clock_get(c);
        int64_t slave_clock = clock_get(slave);
        //每10s同步一次
        if (slave_clock != LONG_MAX && (clock == LONG_MAX || fabs(clock - slave_clock) > 1000000)){
//            LOGI("sync slave clock ：%ld ,clock : %ld",slave_clock,clock);
            clock_set(c, slave_clock);
        }
    }
}
