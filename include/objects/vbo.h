#pragma once

#include <glad/glad.h>

class VBO{
    public:
    GLuint ID;
    VBO(GLfloat* vertices, GLsizeiptr size);
    ~VBO();

    void Bind();
    void Unbind();
    void Delete();
};

