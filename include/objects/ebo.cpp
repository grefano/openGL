#include "ebo.h"
#include <iostream>
#include "renderer.h"

EBO::EBO(GLuint* indices, GLsizeiptr size){
    GLCall(glGenBuffers(1, &ID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW));
}

EBO::~EBO(){
    std::cout << "deleted ebo " << std::endl;
    glDeleteBuffers(1, &ID);
}

void EBO::Bind(){
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID));
}

void EBO::Unbind(){
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void EBO::Delete(){
    glDeleteBuffers(1, &ID);
}