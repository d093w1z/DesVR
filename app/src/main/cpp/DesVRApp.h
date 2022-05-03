#include <VrApi.h>
#include <stdint.h>

#include "eglHandler.h"
#include "renderer.h"
#include "utils.h"

class DesVRApp {
  ovrJava *java;
  eglHandler egl;
  Renderer renderer;
  bool resumed;
  ANativeWindow *window;
  bool back_button_down_previous_frame;

public:
  uint64_t frame_index;
  ovrMobile *ovr;
  void create(ovrJava *java);
  void updateVRMode();
  void handleInput();
  ovrLayerProjection2 renderFrame(ovrTracking2*);
  void destroy();
  static void DesVRApp::handleCMD(struct android_app *, int32_t);
};