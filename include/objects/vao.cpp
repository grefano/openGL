#include "vao.h"

VAO::VAO(){
    glGenVertexArrays(1, &ID);
}

VAO::~VAO(){
    glDeleteVertexArrays(1, &ID);
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint num_components, GLenum type, GLsizeiptr stride, void* offset){
    VBO.Bind();
    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);    
    VBO.Unbind();
}



void VAO::Bind(){
    glBindVertexArray(ID);
}

void VAO::Unbind(){
    glBindVertexArray(0);
}

void VAO::Delete(){
    glDeleteVertexArrays(1, &ID);
}