#pragma once

// #pragma once

#include <glad/glad.h>

class EBO{
    public:
    GLuint ID;
    unsigned int count;
    EBO(GLuint* indices, GLsizeiptr size);
    ~EBO();

    void Bind() const;
    void Unbind() const;
    void Delete();

    unsigned int getCount() const;
};

