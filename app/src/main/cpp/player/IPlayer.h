#include "string"

class IPlayer{
public:
    virtual void Init(const std::string &url, const std::shared_ptr<VideoRender> &render) = 0;
    virtual void UnInit() = 0;
    virtual void Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void SeekTo(float position) = 0;
    virtual float GetDuration() = 0;
    virtual ~IPlayer() {};
};