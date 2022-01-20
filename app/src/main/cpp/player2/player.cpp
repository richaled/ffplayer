#include <log/log.h>
#include <vector>
#include "player.h"

namespace player{

    Player::Player() {
        if(!dispatcher_){
            dispatcher_ = std::make_shared<EventDispatcher>();
        }
        if(!dispatcher_->IsRunning()){
            dispatcher_->Run();
        }
    }

    Player::~Player() noexcept {

    }
    void Player::Prepare(const MediaInfo &mediaInfo){
        //判断当前状态
        if(playerState_ >= PlayerState::kStart){
            return;
        }
        mediaInfo_ = mediaInfo;
        NewEvent(kPlayerPrepare,shared_from_this(),dispatcher_)
                ->Post();
    }

    void Player::HandleEvent(const std::shared_ptr<Event> &event) {
        uint32_t kEvent = event->What();
        switch (kEvent) {
            case kPlayerPrepare:{
                LOGI("play prepare");
                OnPrepare(event);
                break;
            }
        }
    }

    void Player::OnPrepare(const std::shared_ptr<Event> &event) {
        LOGI("clip size : %d",mediaInfo_.clips.size());

        //初始化opengl环境，初始化，开启解码线程
        int ret = InitRender();

    }
}