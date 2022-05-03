#include "framebuffer.h"

void
FrameBuffer::create(GLsizei width,
                   GLsizei height)
{
    swap_chain_index = 0;
    width = width;
    height = height;

    info("create color texture swap chain");
    color_texture_swap_chain = vrapi_CreateTextureSwapChain3(
            VRAPI_TEXTURE_TYPE_2D, GL_RGBA8, width, height, 1, 3);
    if (color_texture_swap_chain == NULL) {
        error("can't create color texture swap chain");
        exit(EXIT_FAILURE);
    }

    swap_chain_length =
            vrapi_GetTextureSwapChainLength(color_texture_swap_chain);

    info("allocate depth renderbuffers");
    depth_renderbuffers = (GLuint*)
            malloc(swap_chain_length * sizeof(GLuint));
    if (depth_renderbuffers == NULL) {
        error("can't allocate depth renderbuffers");
        exit(EXIT_FAILURE);
    }

    info("allocate framebuffers");
    framebuffers = (GLuint*)
            malloc(swap_chain_length * sizeof(GLuint));
    if (framebuffers == NULL) {
        error("can't allocate framebuffers");
        exit(EXIT_FAILURE);
    }

    glGenRenderbuffers(swap_chain_length,
                       depth_renderbuffers);
    glGenFramebuffers(swap_chain_length,
                      framebuffers);
    for (int i = 0; i < swap_chain_length; ++i) {
        info("create color texture %d", i);
        GLuint color_texture = vrapi_GetTextureSwapChainHandle(
                color_texture_swap_chain, i);
        glBindTexture(GL_TEXTURE_2D, color_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        info("create depth renderbuffer %d", i);
        glBindRenderbuffer(GL_RENDERBUFFER,
                           depth_renderbuffers[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width,
                              height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        info("create framebuffer %d", i);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[i]);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, color_texture, 0);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  depth_renderbuffers[i]);
        GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            error("can't create framebuffer %d: %s", i,
                  gl_get_framebuffer_status_string(status));
            exit(EXIT_FAILURE);
        }
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
}

void
FrameBuffer::destroy()
{
    info("destroy framebuffers");
    glDeleteFramebuffers(swap_chain_length,
                         framebuffers);

    info("destroy depth renderbuffers");
    glDeleteRenderbuffers(swap_chain_length,
                          depth_renderbuffers);

    info("free framebuffers");
    free(framebuffers);

    info("free depth renderbuffers");
    free(depth_renderbuffers);

    info("destroy color texture swap chain");
    vrapi_DestroyTextureSwapChain(color_texture_swap_chain);
}