#include "utils.h"

struct attrib_pointer {
  GLint size;
  GLenum type;
  GLboolean normalized;
  GLsizei stride;
  const GLvoid *pointer;
};

struct vertex {
  float position[4];
  float color[4];
};

static const struct attrib_pointer ATTRIB_POINTERS[ATTRIB_END] = {
    {3, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
     (const GLvoid *)offsetof(struct vertex, position)},
    {3, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
     (const GLvoid *)offsetof(struct vertex, color)},
};

static const struct vertex VERTICES[] = {
    {{-1.0, +1.0, -1.0}, {1.0, 0.0, 1.0}},
    {{+1.0, +1.0, -1.0}, {0.0, 1.0, 0.0}},
    {{+1.0, +1.0, +1.0}, {0.0, 0.0, 1.0}},
    {{-1.0, +1.0, +1.0}, {1.0, 0.0, 0.0}},
    {{-1.0, -1.0, -1.0}, {0.0, 0.0, 1.0}},
    {{-1.0, -1.0, +1.0}, {0.0, 1.0, 0.0}},
    {{+1.0, -1.0, +1.0}, {1.0, 0.0, 1.0}},
    {{+1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
};

static const unsigned short INDICES[] = {
    0, 2, 1, 2, 0, 3, 4, 6, 5, 6, 4, 7, 2, 6, 7, 7, 1, 2,
    0, 4, 5, 5, 3, 0, 3, 5, 6, 6, 2, 3, 0, 1, 7, 7, 4, 0,
};

static const GLsizei NUM_INDICES = sizeof(INDICES) / sizeof(INDICES[0]);

class Geometry {
public:
  GLuint vertex_array;
  GLuint vertex_buffer;
  GLuint index_buffer;

  void create();
  void destroy();
};