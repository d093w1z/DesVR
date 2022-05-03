#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include "utils.h"

class eglHandler
{
    public:
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    void create();
    void destroy();
};
