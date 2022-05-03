#include "DesVRApp.h"
#include <android_native_app_glue.h>
#include <VrApi_Input.h>
#include <VrApi_SystemUtils.h>
#include <VrApi_Helpers.h>

static const int CPU_LEVEL = 2;
static const int GPU_LEVEL = 3;

void
DesVRApp::handleCMD(struct android_app* android_app, int32_t cmd)
{
    DesVRApp* app = (DesVRApp*)android_app->userData;
    switch (cmd) {
        case APP_CMD_START:
            info("onStart()");
            break;
        case APP_CMD_RESUME:
            info("onResume()");
            app->resumed = true;
            break;
        case APP_CMD_PAUSE:
            info("onPause()");
            app->resumed = false;
            break;
        case APP_CMD_STOP:
            info("onStop()");
            break;
        case APP_CMD_DESTROY:
            info("onDestroy()");
            app->window = NULL;
            break;
        case APP_CMD_INIT_WINDOW:
            info("surfaceCreated()");
            app->window = android_app->window;
            break;
        case APP_CMD_TERM_WINDOW:
            info("surfaceDestroyed()");
            app->window = NULL;
            break;
        default:
            break;
    }
}

void
DesVRApp::updateVRMode()
{
    if (resumed && window != NULL) {
        if (ovr == NULL) {
            ovrModeParms mode_parms = vrapi_DefaultModeParms(java);
            mode_parms.Flags |= VRAPI_MODE_FLAG_NATIVE_WINDOW;
            mode_parms.Flags &= ~VRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;
            mode_parms.Display = (size_t)egl.display;
            mode_parms.WindowSurface = (size_t)window;
            mode_parms.ShareContext = (size_t)egl.context;

            info("enter vr mode");
            ovr = vrapi_EnterVrMode(&mode_parms);
            if (ovr == NULL) {
                error("can't enter vr mode");
                exit(EXIT_FAILURE);
            }

            vrapi_SetClockLevels(ovr, CPU_LEVEL, GPU_LEVEL);
        }
    } else {
        if (ovr != NULL) {
            info("leave vr mode");
            vrapi_LeaveVrMode(ovr);
            ovr = NULL;
        }
    }
}

void
DesVRApp::handleInput()
{
    bool back_button_down_current_frame = false;

    int i = 0;
    ovrInputCapabilityHeader capability;
    while (vrapi_EnumerateInputDevices(ovr, i, &capability) >= 0) {
        if (capability.Type == ovrControllerType_TrackedRemote) {
            ovrInputStateTrackedRemote input_state;
            input_state.Header.ControllerType = ovrControllerType_TrackedRemote;
            if (vrapi_GetCurrentInputState(ovr, capability.DeviceID,
                                           &input_state.Header) == ovrSuccess) {
                back_button_down_current_frame |=
                        input_state.Buttons & ovrButton_Back;
                back_button_down_current_frame |=
                        input_state.Buttons & ovrButton_B;
                back_button_down_current_frame |=
                        input_state.Buttons & ovrButton_Y;
            }
        }
        ++i;
    }

    if (back_button_down_previous_frame &&
        !back_button_down_current_frame) {
        vrapi_ShowSystemUI(java, VRAPI_SYS_UI_CONFIRM_QUIT_MENU);
    }
    this->back_button_down_previous_frame = back_button_down_current_frame;
}

void
DesVRApp::create(ovrJava* java)
{
    this->java = java;
    egl.create();
    renderer.create(vrapi_GetSystemPropertyInt(
                            java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH),
                    vrapi_GetSystemPropertyInt(
                            java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT));
    resumed = false;
    window = NULL;
    ovr = NULL;
    back_button_down_previous_frame = false;
    frame_index = 0;
}

void
DesVRApp::destroy()
{
    egl.destroy();
    renderer.destroy();
}

ovrLayerProjection2 DesVRApp::renderFrame(ovrTracking2* tracking){
    return renderer.renderFrame(tracking);
}