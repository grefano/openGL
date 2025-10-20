#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <cmath>

#include <glm-1.0.2/glm/glm.hpp>
#include <glm-1.0.2/glm/gtc/matrix_transform.hpp>
#include <glm-1.0.2/glm/gtc/type_ptr.hpp>

#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shaderClass.h"
#include "renderer.h"
#include "texture.h"



using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main(){
    if (!glfwInit()){
        cout << "falha inicializando glfw" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLfloat vertices1[] = {
        -0.5f, 0.5f, 0.0f,      0.8f, 0.3f, 0.16f,      0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,     0.6f, 0.6f, 0.04f,      0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,      0.4f, 0.9f, 0.02f,      1.0f, 1.0f,
        0.5f, 0.5f, 0.0f,       0.2f, 0.6f, 0.04f,      1.0f, 0.0f,

    };
    GLfloat vertices2[] = {
        0.0f, 0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        0.5f, 0.5f, 0.0f,     0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        0.5f, 0.0f, 0.0f,     0.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        0.0f, 0.0f, 0.0f,     0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    };
    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    
    GLFWwindow* window = glfwCreateWindow(800, 800, "teste", NULL, NULL);
    if (!window){
        cout << "falha ao criar janela";
        glfwTerminate() ;
        return -1;
    }   
    glfwMakeContextCurrent(window);
    
    gladLoadGL();
    glViewport(0, 0, 800, 800);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    Shader shd("shaders/default.vert", "shaders/default.frag");
    Shader shd2("shaders/default.vert", "shaders/default.frag");
    



    VAO VAO1;//, VAO2;
    VBO VBO1(vertices1, sizeof(vertices1));//, VBO2(vertices);
    Layout layout;
    layout.push(GL_FLOAT,3);
    layout.push(GL_FLOAT,3);
    layout.push(GL_FLOAT,2);
    VAO1.addBuffer(VBO1, layout);
    
    EBO EBO1(indices, sizeof(indices));
    EBO1.Bind();
    

    Texture texture("textures/teste.png");
    Texture texture2("textures/teste2.png");
    
    GLuint tex0Uni = glGetUniformLocation(shd.ID, "tex0");
    // GLuint tex1Uni = glGetUniformLocation(shd.ID, "tex1");
    shd.Activate();
    glUniform1i(tex0Uni, 0);
    unsigned int transformLoc = glGetUniformLocation(shd.ID, "transform");
    
    while (!glfwWindowShouldClose(window)){
        processInput(window);
        
        glClearColor(0.2, .13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        shd.Activate();
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));    
        // trans = glm::rotate(trans, (float)glfwGetTime() * -1.0f, glm::vec3(0.0f, 0.0f, 1.0f));    
        
        glUniformMatrix4fv(transformLoc,1, GL_FALSE, glm::value_ptr(trans));
        GLCall(glBindTexture(GL_TEXTURE_2D, texture2.ID));

        VAO1.Bind();
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        glfwSwapBuffers(window);
        glfwPollEvents();

    }


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}