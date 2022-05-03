#include <VrApi.h>
#include <VrApi_Helpers.h>
#include "renderer.h"



void
Renderer::create( GLsizei width, GLsizei height)
{
    for (int i = 0; i < VRAPI_FRAME_LAYER_EYE_MAX; ++i) {
        framebuffers[i].create( width, height);
    }
    program.create();
    geometry.create();
}

void
Renderer::destroy()
{
    geometry.destroy();
    program.destroy();
    for (int i = 0; i < VRAPI_FRAME_LAYER_EYE_MAX; ++i) {
        framebuffers[i].destroy();
    }
}

ovrLayerProjection2
Renderer::renderFrame(ovrTracking2* tracking)
{
    ovrMatrix4f model_matrix = ovrMatrix4f_CreateTranslation(0.0, 0.0, -1.0);
    model_matrix = ovrMatrix4f_Transpose(&model_matrix);

    ovrLayerProjection2 layer = vrapi_DefaultLayerProjection2();
    layer.Header.Flags |=
            VRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION;
    layer.HeadPose = tracking->HeadPose;

    for (int i = 0; i < VRAPI_FRAME_LAYER_EYE_MAX; ++i) {
        ovrMatrix4f view_matrix =
                ovrMatrix4f_Transpose(&tracking->Eye[i].ViewMatrix);
        ovrMatrix4f projection_matrix =
                ovrMatrix4f_Transpose(&tracking->Eye[i].ProjectionMatrix);

        FrameBuffer* framebuffer = &framebuffers[i];
        layer.Textures[i].ColorSwapChain =
                framebuffer->color_texture_swap_chain;
        layer.Textures[i].SwapChainIndex = framebuffer->swap_chain_index;
        layer.Textures[i].TexCoordsFromTanAngles =
                ovrMatrix4f_TanAngleMatrixFromProjection(
                        &tracking->Eye[i].ProjectionMatrix);

        glBindFramebuffer(
                GL_DRAW_FRAMEBUFFER,
                framebuffer->framebuffers[framebuffer->swap_chain_index]);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glViewport(0, 0, framebuffer->width, framebuffer->height);
        glScissor(0, 0, framebuffer->width, framebuffer->height);
        glClearColor(0.0, 0.0, 0.0, 0.0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program.program);
        glUniformMatrix4fv(
                program.uniform_locations[UNIFORM_MODEL_MATRIX], 1,
                GL_FALSE, (const GLfloat*)&model_matrix);
        glUniformMatrix4fv(
                program.uniform_locations[UNIFORM_VIEW_MATRIX], 1,
                GL_FALSE, (const GLfloat*)&view_matrix);
        glUniformMatrix4fv(
                program.uniform_locations[UNIFORM_PROJECTION_MATRIX], 1,
                GL_FALSE, (const GLfloat*)&projection_matrix);
        glBindVertexArray(geometry.vertex_array);
        glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_SHORT, NULL);
        glBindVertexArray(0);
        glUseProgram(0);

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glScissor(0, 0, 1, framebuffer->height);
        glClear(GL_COLOR_BUFFER_BIT);
        glScissor(framebuffer->width - 1, 0, 1, framebuffer->height);
        glClear(GL_COLOR_BUFFER_BIT);
        glScissor(0, 0, framebuffer->width, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glScissor(0, framebuffer->height - 1, framebuffer->width, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        static const GLenum ATTACHMENTS[] = { GL_DEPTH_ATTACHMENT };
        static const GLsizei NUM_ATTACHMENTS =
                sizeof(ATTACHMENTS) / sizeof(ATTACHMENTS[0]);
        glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, NUM_ATTACHMENTS,
                                ATTACHMENTS);
        glFlush();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        framebuffer->swap_chain_index = (framebuffer->swap_chain_index + 1) %
                                        framebuffer->swap_chain_length;
    }
    return layer;
}