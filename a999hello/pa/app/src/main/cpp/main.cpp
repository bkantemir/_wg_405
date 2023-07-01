#include "platform.h"
#include <jni.h>
#include <EGL/egl.h>

#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>
#include <game-activity/native_app_glue/android_native_app_glue.c>

#include "TheApp.cpp"

TheApp theApp;

struct android_app* pAndroidApp = NULL;
EGLDisplay androidDisplay = EGL_NO_DISPLAY;
EGLSurface androidSurface = EGL_NO_SURFACE;
EGLContext androidContext = EGL_NO_CONTEXT;
bool bExitApp = false;
int screenSize[2] = {0,0};

void android_init_display() {
    // Choose your render attributes
    constexpr EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };
    // The default display is probably what you want on Android
    auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, nullptr, nullptr);

    // figure out how many configs there are
    EGLint numConfigs;
    eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

    // get the list of configurations
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

    // Find a config we like.
    // Could likely just grab the first if we don't care about anything else in the config.
    // Otherwise hook in your own heuristic
    auto config = *std::find_if(
            supportedConfigs.get(),
            supportedConfigs.get() + numConfigs,
            [&display](const EGLConfig &config) {
                EGLint red, green, blue, depth;
                if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red)
                    && eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green)
                    && eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue)
                    && eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depth)) {

                    //aout << "Found config with " << red << ", " << green << ", " << blue << ", "
                    //     << depth << std::endl;
                    return red == 8 && green == 8 && blue == 8 && depth == 24;
                }
                return false;
            });
    // create the proper window surface
    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    EGLSurface surface = eglCreateWindowSurface(display, config, pAndroidApp->window, nullptr);

    // Create a GLES 3 context
    EGLint contextAttribs[] = {
            EGL_CONTEXT_MAJOR_VERSION, 3,
            EGL_CONTEXT_MINOR_VERSION, 2,
            EGL_NONE};
    EGLContext context = eglCreateContext(display, config, nullptr, contextAttribs);

    // get some window metrics
    auto madeCurrent = eglMakeCurrent(display, surface, surface, context);
    if(!madeCurrent) {
        ;
    }
    androidDisplay = display;
    androidSurface = surface;
    androidContext = context;
}
void android_term_display() {
    if (androidDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(androidDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (androidContext != EGL_NO_CONTEXT) {
            eglDestroyContext(androidDisplay, androidContext);
            androidContext = EGL_NO_CONTEXT;
        }
        if (androidSurface != EGL_NO_SURFACE) {
            eglDestroySurface(androidDisplay, androidSurface);
            androidSurface = EGL_NO_SURFACE;
        }
        eglTerminate(androidDisplay);
        androidDisplay = EGL_NO_DISPLAY;
    }
}


void handle_cmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            android_init_display();
            //updateRenderArea
            EGLint width,height;
            eglQuerySurface(androidDisplay, androidSurface, EGL_WIDTH, &width);
            eglQuerySurface(androidDisplay, androidSurface, EGL_HEIGHT, &height);
            screenSize[0] = 0;
            screenSize[1] = 0;
            theApp.onScreenResize(width,height);
            break;
        case APP_CMD_TERM_WINDOW:
            android_term_display();
            break;
        default:
            break;
    }
}

/*!
 * This the main entry point for a native activity
 */

void android_main(struct android_app *pApp) {
    pAndroidApp = pApp;

    // register an event handler for Android events
    pApp->onAppCmd = handle_cmd;

    myPollEvents(); //this will wait for display initialization

    theApp.run();

    android_term_display();
    std::terminate();
}
