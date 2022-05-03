#include "utils.h"

enum uniform
{
    UNIFORM_BEGIN = 0,
    UNIFORM_MODEL_MATRIX = UNIFORM_BEGIN,
    UNIFORM_VIEW_MATRIX = 1,
    UNIFORM_PROJECTION_MATRIX = 2,
    UNIFORM_END = 3,
};

enum attrib
{
    ATTRIB_BEGIN = 0,
    ATTRIB_POSITION = ATTRIB_BEGIN,
    ATTRIB_COLOR = 1,
    ATTRIB_END = 2,
};

class Program
{
    public:
    GLuint program;
    GLint uniform_locations[UNIFORM_END];
    void create();
    void destroy();
};