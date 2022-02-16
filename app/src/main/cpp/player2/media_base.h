#pragma once

#include <vector>
#include <map>
#include "cstdint"
#include "eventloop/any.h"

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
            return !file_name.empty() && start_time >= 0 && end_time >= start_time;
        }
    } MediaClip;

    typedef struct MediaInfo{
        std::vector<MediaClip> clips;

        bool IsEmpty() const {
            return clips.empty();
        }

    }MediaInfo;


    enum class OptionKey{
        kEnableHardware = 1,//硬件编解码
    };

    using Options = std::map<OptionKey, any>;

    template<typename T>
    T OptionsGet(const Options &options, OptionKey key, const T &defaultVal) {
        const auto &itr = options.find(key);
        if (itr == options.end()) {
            return defaultVal;
        }
        return any_cast<T>(itr->second);
    }

}
