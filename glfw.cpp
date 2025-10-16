#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include "shaderClass.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"

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




    GLfloat vertices[] = {
        -0.5f, 0.5f, 0.0f,     0.8f, 0.3f, 0.16f,      0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,      0.6f, 0.6f, 0.04f,      0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,       0.4f, 0.9f, 0.02f,      1.0f, 1.0f,
        0.5f, 0.5f, 0.0f,       0.2f, 0.6f, 0.04f,     1.0f, 0.0f,
    };
    GLuint indices[] = {
        0, 2, 1,
        0, 3, 2
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

    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8*sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8*sizeof(float), (void*)(3*sizeof(float)));
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8*sizeof(float), (void*)(6*sizeof(float)));
    
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();


    GLuint uniID = glGetUniformLocation(shd.ID, "scale");
    
    int wImg, hImg, numColCh;
    unsigned char* bytes = stbi_load("textures/teste.png", &wImg, &hImg, &numColCh, 0);

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wImg, hImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint tex0Uni = glGetUniformLocation(shd.ID, "tex0");
    shd.Activate();
    glUniform1i(tex0Uni, 0);

    while (!glfwWindowShouldClose(window)){
        processInput(window);

        glClearColor(0.2, .13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shd.Activate();
        glUniform1f(uniID, 0.5f);
        glBindTexture(GL_TEXTURE_2D, texture);
        VAO1.Bind();
        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }




    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    glDeleteTextures(1, &texture);
    shd.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}