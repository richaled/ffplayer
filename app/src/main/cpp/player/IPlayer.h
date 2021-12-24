#include "string"

class IPlayer{
public:
    virtual void Init(const std::string &url) = 0;
    virtual void UnInit() = 0;
    virtual void Play() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void SeekTo(float position) = 0;
    virtual ~IPlayer() {};
};