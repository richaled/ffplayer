//
// Created by richard on 2022/2/14.
//

#pragma once

#include "player.h"

/**
 * Android 播放器，主要是实现一些Android特有的如anativewindow，jni等
 */
namespace player{
    class AndroidPlayer : public Player {
    public:
        AndroidPlayer();
        ~AndroidPlayer();

        EGLSurface GetPlatformSurface(void *window) override;
        void ReleaseSurface(void *window) override ;
    private:

    };
}




