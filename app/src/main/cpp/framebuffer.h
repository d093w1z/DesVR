#include <VrApi.h>
#include <VrApi_Helpers.h>
#include "utils.h"

class FrameBuffer
{
    public:

    GLsizei height;
    GLsizei width;
    int swap_chain_index;
    int swap_chain_length;
    ovrTextureSwapChain* color_texture_swap_chain;
    GLuint* depth_renderbuffers;
    GLuint* framebuffers;

    void destroy();
    void create(GLsizei width, GLsizei height);
};