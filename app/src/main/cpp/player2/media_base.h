#pragma once

#include <vector>
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
        bool IsValid(){
            return !file_name.empty() && start_time > 0 && end_time > start_time;
        }
    } MediaClip;

    typedef struct MediaInfo{
        std::vector<MediaClip> clips;
        bool hardWare = false;

        bool IsEmpty() const {
            return clips.empty();
        }

    }MediaInfo;

}
