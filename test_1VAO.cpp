#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <cmath>

#include <glm-1.0.2/glm/glm.hpp>
#include <glm-1.0.2/glm/gtc/matrix_transform.hpp>
#include <glm-1.0.2/glm/gtc/type_ptr.hpp>
#include <assert.h>

#include "shaderClass.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "renderer.h"

using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// static void GLClearError(){
//     while (glGetError() != GL_NO_ERROR);
// }

// static bool GLLogCall(){
//     while (GLenum error = glGetError()){
//         cout << "OpenGL error " << error << endl;
//         return false;
//     }
//     return true;
// }

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
    VBO VBO2(vertices1, sizeof(vertices1));//, VBO2(vertices);
    VAO1.Bind();
    // VBO VBO1(vertices1, sizeof(vertices1));//, VBO2(vertices2, sizeof(vertices2));
    EBO EBO1(indices, sizeof(indices));
    EBO1.Bind();
    

    VAO1.Unbind();
    EBO1.Unbind();

    

    
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

    
    bytes = stbi_load("textures/teste2.png", &wImg, &hImg, &numColCh, 0);
    
    GLuint texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture2);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wImg, hImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);

    
    
    GLuint tex0Uni = glGetUniformLocation(shd.ID, "tex0");
    // GLuint tex1Uni = glGetUniformLocation(shd.ID, "tex1");
    GLCall(shd.Bind());
    glUniform1i(tex0Uni, 0);
    unsigned int transformLoc = glGetUniformLocation(shd.ID, "transform");
    // glUniform1i(tex0Uni, 0);
    // transformLoc = glGetUniformLocation(shd2.ID, "transform");
    
    while (!glfwWindowShouldClose(window)){
        processInput(window);
        
        glClearColor(0.2, .13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shd.Bind();
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));    
        // trans = glm::rotate(trans, (float)glfwGetTime() * -1.0f, glm::vec3(0.0f, 0.0f, 1.0f));    
        
        glUniformMatrix4fv(transformLoc,1, GL_FALSE, glm::value_ptr(trans));
        // glUniform1f(uniTime, 0.0f);
        GLCall(glBindTexture(GL_TEXTURE_2D, texture2));


        GLCall(VAO1.Bind());
        GLCall(VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8*sizeof(float), (void*)0));
        GLCall(VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8*sizeof(float), (void*)(3*sizeof(float))));
        GLCall(VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8*sizeof(float), (void*)(6*sizeof(float))));
        
        
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
        VAO1.Unbind();


        // GLCall(shd2.Activate());
        trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.0f));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.3f, 1.0f, 0.2f));    

        glUniformMatrix4fv(transformLoc,1, GL_FALSE, glm::value_ptr(trans));
        // glUniform1f(uniTime, 0.0f);
        GLCall(glBindTexture(GL_TEXTURE_2D, texture));
        GLCall(VAO1.Bind());
        GLCall(VAO1.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 8*sizeof(float), (void*)0));
        GLCall(VAO1.LinkAttrib(VBO2, 1, 3, GL_FLOAT, 8*sizeof(float), (void*)(3*sizeof(float))));
        GLCall(VAO1.LinkAttrib(VBO2, 2, 2, GL_FLOAT, 8*sizeof(float), (void*)(6*sizeof(float))));

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
        VAO1.Unbind();

        glfwSwapBuffers(window);
        
        glfwPollEvents();

    }




    VAO1.Delete();
    VBO1.Delete();
    VBO2.Delete();
    EBO1.Delete();
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &texture2);
    shd.Delete();
    shd2.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}