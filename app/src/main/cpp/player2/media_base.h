#pragma once

#include "cstdint"

namespace test{
    /**
     * 媒体片段的结构
     */
    typedef struct MediaClip{
        std::string file_name;
        int64_t start_time;
        int64_t end_time;
        int type;
    } MediaClip;

    typedef struct MediaInfo{
        std::vector<MediaClip> clips;
        bool hardWare = false;

        bool IsEmpty() const {
            return clips.empty();
        }

    }MediaInfo;

}
