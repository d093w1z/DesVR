#include "framebuffer.h"
#include "program.h"
#include "geometry.h"
#include "utils.h"
class Renderer {
  FrameBuffer framebuffers[VRAPI_FRAME_LAYER_EYE_MAX];
  Program program;
  Geometry geometry;

public:
  void create(GLsizei width, GLsizei height);
  void destroy();
  ovrLayerProjection2 enderer::renderFrame(ovrTracking2* tracking);
};