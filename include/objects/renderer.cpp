
#include <iostream>
#include <glad/glad.h>
#include "renderer.h"
#include <assert.h>
#include "vao.h"
#include "ebo.h"
#include "shaderClass.h"

void GLClearError(){
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(){
    while (GLenum error = glGetError()){
        std::cout << "OpenGL error " << error << std::endl;
        return false;
    }
    return true;
}


void Renderer::draw(const VAO& vao, const EBO& ebo, const Shader& shader) const{
    shader.Bind();
    vao.Bind();
    ebo.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ebo.getCount(), GL_UNSIGNED_INT, 0));

}