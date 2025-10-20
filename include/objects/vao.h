#pragma once

#include "vbo.h"
#include "layout.h"
class VAO{
    public:
    GLuint ID;
    VAO();
    ~VAO();
    void LinkAttrib(VBO& VBO, GLuint layout, GLuint num_components, GLenum type, GLsizeiptr stride, void* offset);
    void addBuffer(VBO& VBO, Layout& layout);
    void Bind();
    void Unbind();
    void Delete();
};

