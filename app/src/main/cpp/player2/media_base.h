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
        bool operator==(const MediaClip & obj2) const{
            return this->file_name.compare(obj2.file_name) == 0;
        }

        std::string ToString(){
            return "file : " + file_name
            + ", start time : " + std::to_string(start_time)
            + ",end time : " + std::to_string(end_time);
        }
    } MediaClip;

    typedef struct MediaInfo{
        std::vector<MediaClip> clips;

        bool IsEmpty() const {
            return clips.empty();
        }

        int GetClipIndex(MediaClip mediaClip){
            std::vector<MediaClip>::iterator it;
            it = std::find(clips.begin(), clips.end(), mediaClip);
            int index = 0;
            if (it != clips.end()){
                index = it - clips.begin();
            }
            return index;
        }

        MediaClip AtClipIndex(int index){
            int size = clips.size();
            if(index >= size){
                return clips[0];
            }
            return clips.at(index);
        }

        int GetClipTime(int64_t time){
            int index = 0;
            int end_time = 0;
            while (index < clips.size() && end_time < time) {
                auto clip = clips.at(index);
                auto duration = clip.end_time - clip.start_time;
                end_time += duration;
                index++;
            }
            return end_time > time ? index - 1 : -1;
        }

        int64_t GetVideoTime(int index, int64_t clip_time) {
            if (index < 0 || index >= clips.size()) {
                return 0;
            }
            int64_t offset = 0;
            for (int i = 0; i < index; ++i) {
                MediaClip clip = clips.at(i);
                offset += (clip.end_time - clip.start_time);
            }
            return clip_time + offset;
        }

        int64_t GetDuration(){
            if(clips.empty()){
                return 0;
            }
            int64_t duration = 0;
            for(auto& clip : clips){
                duration += (clip.end_time - clip.start_time);
            }
            return duration;
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
