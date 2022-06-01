#include <VrApi.h>
#include <stdint.h>
#include "utils.h"
#include "eglHandler.h"
#include "renderer.h"

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
  static void handleInput(DesVRApp * app);
  ovrLayerProjection2& renderFrame(ovrTracking2*);
  void destroy();
  static void handleCMD(struct android_app *, int32_t);
};