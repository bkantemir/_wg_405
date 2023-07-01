#include <android/log.h>
#include "stdio.h"
#include "TheApp.h"
#include <EGL/egl.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

extern struct android_app* pAndroidApp;
extern EGLDisplay androidDisplay;
extern EGLSurface androidSurface;

extern TheApp theApp;
 
void mylog(const char* _Format, ...) {
    char outStr[1024];
    va_list _ArgList;
    va_start(_ArgList, _Format);
    vsprintf(outStr, _Format, _ArgList);
    __android_log_print(ANDROID_LOG_INFO, "mylog", outStr, NULL);
    va_end(_ArgList);
};
 
void mySwapBuffers() {
    eglSwapBuffers(androidDisplay, androidSurface);
}
void myPollEvents() {
    // Process all pending events before running game logic.
    int events;
    android_poll_source *pSource;
    if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0)
        if (pSource)
            pSource->process(pAndroidApp, pSource);
    //if no display - wait for it
    while (androidDisplay == EGL_NO_DISPLAY)
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0)
            if (pSource)
                pSource->process(pAndroidApp, pSource);

    // handle all queued inputs
    for (auto i = 0; i < pAndroidApp->motionEventsCount; i++) {

        // cache the current event
        auto &motionEvent = pAndroidApp->motionEvents[i];

        // cache the current action
        auto action = motionEvent.action;

        // Find the pointer index, mask and bitshift to turn it into a readable value
        auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        //aout << "Pointer " << pointerIndex << ":";

        // get the x and y position of this event
        auto &pointer = motionEvent.pointers[pointerIndex];
        auto x = GameActivityPointerAxes_getX(&pointer);
        auto y = GameActivityPointerAxes_getY(&pointer);
        //aout << "(" << x << ", " << y << ") ";

        // Only consider touchscreen events, like touches
        auto actionMasked = action & AINPUT_SOURCE_TOUCHSCREEN;

        // determine the kind of event it is
        switch (actionMasked) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                //aout << "Pointer Down";
                break;

            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                //aout << "Pointer Up";
                break;

            default:
                ;//aout << "Pointer Move";
        }
    }
    android_app_clear_motion_events(pAndroidApp);

    // handle key inputs
    for (auto i = 0; i < pAndroidApp->keyUpEventsCount; i++) {
        // cache the current event
        auto &keyEvent = pAndroidApp->keyUpEvents[i];
        if (keyEvent.keyCode == AKEYCODE_BACK) {
            // actions on back key
            theApp.bExitApp = true;
        }
    }
    android_app_clear_key_up_events(pAndroidApp);
}

