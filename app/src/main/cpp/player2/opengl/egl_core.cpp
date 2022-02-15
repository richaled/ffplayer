#include "egl_core.h"
#include <log/log.h>
#include <android/native_window.h>

namespace player {

    EglCore::EglCore() {

    }

    EglCore::~EglCore() {

    }

    bool EglCore::Init(const bool enableMutableRenderBuffer, const EGLContext sharedEglContext) {
        EGLint eglConfigAttrs[] = {
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_NONE
        };

        if ((display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
            LOGE("egl get display fail");
            return false;
        }
        if (!eglInitialize(display_, 0, 0)) {
            LOGE("egl initialize fail");
            return false;
        }
        EGLint numConfigs;
        if (!eglChooseConfig(display_, eglConfigAttrs, &config_, 1, &numConfigs)) {
            LOGE("egl choose config fail");
            return false;
        }
        EGLint eglContextAttributes[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        //create context
        context_ = eglCreateContext(display_, config_, sharedEglContext, eglContextAttributes);
        if (context_ == nullptr) {
            LOGE("egl create context fail");

            return false;
        }
        //get android presentation time
        eglPresentationTimeANDROID = (PFNEGLPRESENTATIONTIMEANDROIDPROC) eglGetProcAddress(
                "eglPresentationTimeANDROID");
        if (eglPresentationTimeANDROID == nullptr) {
            LOGW("eglPresentationTimeANDROID is not support");
        }

        return true;
    }

    EGLSurface EglCore::CreateWindowSurface(ANativeWindow *window) {
        if (window == nullptr) {
            return nullptr;
        }
        EGLint format;
        if (!eglGetConfigAttrib(display_, config_, EGL_NATIVE_VISUAL_ID, &format)) {
            LOGE("egl get config attrib fail");
            return nullptr;
        }
        ANativeWindow_setBuffersGeometry(window, 0, 0, format);
        EGLSurface surface = nullptr;
        if (!(surface = eglCreateWindowSurface(display_, config_, window, 0))) {
            LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
        }
        return surface;
    }

    EGLSurface EglCore::CreateOffscreenSurface(const int width, const int height) {
        EGLint pbufferAttributes[] = {EGL_WIDTH, width, EGL_HEIGHT, height, EGL_NONE, EGL_NONE};
        EGLSurface surface = eglCreatePbufferSurface(display_, config_, pbufferAttributes);
        if (!surface) {
            LOGE("eglCreatePbufferSurface() returned error %d", eglGetError());
        }
        return surface;
    }

    bool EglCore::MakeCurrent(EGLSurface eglSurface) {
        return eglMakeCurrent(display_, eglSurface, eglSurface, context_);
    }

    bool EglCore::SwapBuffers(EGLSurface eglSurface) {
        eglSwapBuffers(display_, eglSurface);
    }

    void EglCore::DoneCurrent() {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, context_);
    }

    void EglCore::ReleaseSurface(EGLSurface eglSurface) {
        eglDestroySurface(display_, eglSurface);
        eglSurface = EGL_NO_SURFACE;
    }

    void EglCore::Release() {
        if (display_ != EGL_NO_DISPLAY && context_ != EGL_NO_CONTEXT) {
            eglMakeCurrent(display_, EGL_NO_DISPLAY, EGL_NO_DISPLAY, EGL_NO_CONTEXT);
            eglDestroyContext(display_, EGL_NO_CONTEXT);
            eglTerminate(display_);
            eglReleaseThread();
        }
        display_ = EGL_NO_DISPLAY;
        context_ = EGL_NO_CONTEXT;
    }
}