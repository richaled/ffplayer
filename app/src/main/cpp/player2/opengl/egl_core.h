//
// Created by richard on 2022/2/14.
//

#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace player {

    class EglCore {
    public:
        EglCore();
        virtual ~EglCore();
        bool Init(const bool enableMutableRenderBuffer = false, const EGLContext sharedEglContext = EGL_NO_CONTEXT);
        EGLSurface CreateWindowSurface(ANativeWindow *window);
        EGLSurface CreateOffscreenSurface(const int width, const int height);

        bool MakeCurrent(EGLSurface eglSurface);
        void DoneCurrent();
        bool SwapBuffers(EGLSurface eglSurface);
        void ReleaseSurface(EGLSurface eglSurface);
        void Release();
    private:
        EGLDisplay display_;
        EGLConfig config_;
        EGLContext context_;

        PFNEGLPRESENTATIONTIMEANDROIDPROC eglPresentationTimeANDROID;
    };
}


