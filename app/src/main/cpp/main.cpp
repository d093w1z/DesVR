#include <VrApi.h>
#include <VrApi_Helpers.h>
#include <android_native_app_glue.h>
#include <android/window.h>
#include <unistd.h>
#include "utils.h"
#include "DesVRApp.h"


void
android_main(struct android_app* android_app)
{
    ANativeActivity_setWindowFlags(android_app->activity,
                                   AWINDOW_FLAG_KEEP_SCREEN_ON, 0);

    info("attach current thread");
    ovrJava java;
    java.Vm = android_app->activity->vm;
    (java.Vm)->AttachCurrentThread(&java.Env, NULL);
    java.ActivityObject = android_app->activity->clazz;

    info("initialize vr api");
    const ovrInitParms init_parms = vrapi_DefaultInitParms(&java);
    if (vrapi_Initialize(&init_parms) != VRAPI_INITIALIZE_SUCCESS) {
        info("can't initialize vr api");
        exit(EXIT_FAILURE);
    }

    DesVRApp app;
    // app_create(&app, &java);
    app.create(&java);

    android_app->userData = &app;
    android_app->onAppCmd = DesVRApp::handleCMD;
    while (!android_app->destroyRequested) {
        for (;;) {
            int events = 0;
            struct android_poll_source* source = NULL;
            if (ALooper_pollAll(
                    android_app->destroyRequested || app.ovr != NULL ? 0 : -1,
                    NULL, &events, (void**)&source) < 0) {
                break;
            }
            if (source != NULL) {
                source->process(android_app, source);
            }

            // app_update_vr_mode(&app);
            app.updateVRMode();
        }

        // app_handle_input(&app);
        app.handleInput();

        if (app.ovr == NULL) {
            continue;
        }
        app.frame_index++;
        const double display_time =
                vrapi_GetPredictedDisplayTime(app.ovr, app.frame_index);
        ovrTracking2 tracking =
                vrapi_GetPredictedTracking2(app.ovr, display_time);
        const ovrLayerProjection2 layer =
                app.renderFrame(&tracking);
                // renderer_render_frame(&app.renderer, &tracking);
        const ovrLayerHeader2* layers[] = { &layer.Header };
        ovrSubmitFrameDescription2 frame;
        frame.Flags = 0;
        frame.SwapInterval = 1;
        frame.FrameIndex = app.frame_index;
        frame.DisplayTime = display_time;
        frame.LayerCount = 1;
        frame.Layers = layers;
        vrapi_SubmitFrame2(app.ovr, &frame);
    }

    // app_destroy(&app);
    app.destroy();

    info("shut down vr api");
    vrapi_Shutdown();

    info("detach current thread");
    (java.Vm)->DetachCurrentThread();
}