//
// Created by richard on 2022/2/14.
//

#pragma once

#include "player.h"

namespace player{
    class AndroidPlayer : public Player {
    public:
        AndroidPlayer();
        ~AndroidPlayer();

        EGLSurface GetPlatformSurface(void *window) override;

    private:

    };
}




