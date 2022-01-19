#include <log/log.h>
#include "player.h"

namespace player{

    Player::Player() {
        dispatcher_ = std::make_shared<EventDispatcher>();
        dispatcher_->Run();
    }

    Player::~Player() noexcept {

    }

    void Player::Start() {
        //判断当前状态
        if(playerState_ >= PlayerState::kStart){
            return;
        }
        NewEvent(kPlayerStart,shared_from_this(),dispatcher_)->Post();
    }


    void Player::HandleEvent(const std::shared_ptr<Event> &event) {
        uint32_t kEvent = event->What();
        switch (kEvent) {
            case kPlayerStart:{
                LOGI("play start");
                break;
            }
        }
    }


}