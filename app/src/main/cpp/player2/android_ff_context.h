#pragma once
#include "ff_context.h"

namespace player {

    /**
     * 包含mediacodec硬解的context
     */
    class AndroidFFContext : public FFContext{

    public:
        AndroidFFContext();
        ~AndroidFFContext();

        int InitHardWareCodec() override ;
    private:

    };

}


