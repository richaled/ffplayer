#pragma once

#include "opengl_render.h"

namespace player {
    class FrameBufferRender : public OpenglRender{
    public:
        FrameBufferRender(const int width, const int height, const char *vertex, const char *fragment);
    };

}

