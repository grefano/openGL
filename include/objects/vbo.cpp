#include <iostream>
#include "vbo.h"
#include "renderer.h"

VBO::VBO(GLfloat* vertices, GLsizeiptr size){
    GLCall(glGenBuffers(1, &ID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
}

VBO::~VBO(){
    std::cout << "deleted vbo " << std::endl;
    glDeleteBuffers(1, &ID);

}

void VBO::Bind(){
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, ID));
}

void VBO::Unbind(){
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VBO::Delete(){
    glDeleteBuffers(1, &ID);
}