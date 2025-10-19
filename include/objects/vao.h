#pragma once

#include <glad/glad.h>
#include "vbo.h"

class VAO{
    public:
    GLuint ID;
    VAO();
    ~VAO();
    void LinkAttrib(VBO& VBO, GLuint layout, GLuint num_components, GLenum type, GLsizeiptr stride, void* offset);
    void Bind();
    void Unbind();
    void Delete();
};

