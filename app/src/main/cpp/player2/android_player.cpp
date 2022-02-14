#include "android_player.h"

namespace player {

    AndroidPlayer::AndroidPlayer() {

    }

    AndroidPlayer::~AndroidPlayer() {

    }

    EGLSurface AndroidPlayer::GetPlatformSurface(void *window) {
        ANativeWindow *nativeWindow = reinterpret_cast<ANativeWindow*>(window);
        return eglCore_->CreateWindowSurface(nativeWindow);
    }
}