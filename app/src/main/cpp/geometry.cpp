#include "geometry.h"
#include "program.h"

void
Geometry::create()
{
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
    for (enum attrib attrib = ATTRIB_BEGIN; attrib != ATTRIB_END; attrib = (enum attrib) (int(attrib)+1)) {
        struct attrib_pointer attrib_pointer = ATTRIB_POINTERS[attrib];
        glEnableVertexAttribArray(attrib);
        glVertexAttribPointer(attrib, attrib_pointer.size, attrib_pointer.type,
                              attrib_pointer.normalized, attrib_pointer.stride,
                              attrib_pointer.pointer);
    }


    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES,
                 GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void
Geometry::destroy()
{
    glDeleteBuffers(1, &index_buffer);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteVertexArrays(1, &vertex_array);
}