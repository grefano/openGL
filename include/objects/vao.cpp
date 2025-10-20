#include <iostream>
#include "vao.h"
#include "layout.h"
#include "vbElement.h"
#include "renderer.h"

VAO::VAO(){
    GLCall(glGenVertexArrays(1, &ID));
}

VAO::~VAO(){
    std::cout << "deleted vao " << std::endl;
    glDeleteVertexArrays(1, &ID);
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint num_components, GLenum type, GLsizeiptr stride, void* offset){
    VBO.Bind();
    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);    
    VBO.Unbind();
}

void VAO::addBuffer(VBO& VBO, Layout& layout){
    Bind();
    GLCall(VBO.Bind());
    const auto& elements = layout.getElements();
    uintptr_t offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++){
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        std::cout << i << " " << element.count << " " << element.type << " " << element.normalized << " " << layout.getStride() << " " << offset << std::endl;
        GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset));    
        offset += element.count * sizeof(float);
    }
}



void VAO::Bind(){
    GLCall(glBindVertexArray(ID));
}

void VAO::Unbind(){
    GLCall(glBindVertexArray(0));
}

void VAO::Delete(){
    glDeleteVertexArrays(1, &ID);
}