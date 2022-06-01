#include "program.h"

static const char* ATTRIB_NAMES[ATTRIB_END] = {
        "aPosition", "aColor",
};

static const char* UNIFORM_NAMES[UNIFORM_END] = {
        "uModelMatrix", "uViewMatrix", "uProjectionMatrix",
};

static const char VERTEX_SHADER[] =
        "#version 300 es\n"
        "\n"
        "in vec3 aPosition;\n"
        "in vec3 aColor;\n"
        "uniform mat4 uModelMatrix;\n"
        "uniform mat4 uViewMatrix;\n"
        "uniform mat4 uProjectionMatrix;\n"
        "\n"
        "out vec3 vColor;\n"
        "void main()\n"
        "{\n"
        "	gl_Position = uProjectionMatrix * ( uViewMatrix * ( uModelMatrix * vec4( "
        "aPosition * 0.1, 1.0 ) ) );\n"
        "	vColor = aColor;\n"
        "}\n";

static const char FRAGMENT_SHADER[] = "#version 300 es\n"
                                      "\n"
                                      "in lowp vec3 vColor;\n"
                                      "out lowp vec4 outColor;\n"
                                      "void main()\n"
                                      "{\n"
                                      "	outColor = vec4(vColor, 1.0);\n"
                                      "}\n";

static GLuint
compile_shader(GLenum type, const char* string)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &string, NULL);
    glCompileShader(shader);
    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* log = (char*) malloc(length);
        glGetShaderInfoLog(shader, length, NULL, log);
        error("can't compile shader: %s", log);
        exit(EXIT_FAILURE);
    }
    return shader;
}

void
Program::create()
{
    program = glCreateProgram();
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER);
    glAttachShader(program, vertex_shader);
    GLuint fragment_shader =
            compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
    glAttachShader(program, fragment_shader);
    for (enum attrib attrib = ATTRIB_BEGIN; attrib != ATTRIB_END; attrib = (enum attrib) (int(attrib)+1) ) {
        glBindAttribLocation(program, attrib, ATTRIB_NAMES[attrib]);
    }
    info("linking program");
    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* log = (char*) malloc(length);
        glGetProgramInfoLog(program, length, NULL, log);
        error("can't link program: %s", log);
        exit(EXIT_FAILURE);
    }
    for (enum uniform uniform = UNIFORM_BEGIN; uniform != UNIFORM_END; uniform = (enum uniform) (int(uniform)+1)) {
        uniform_locations[uniform] =
                glGetUniformLocation(program, UNIFORM_NAMES[uniform]);
    }
}

void
Program::destroy()
{
    glDeleteProgram(program);
}
