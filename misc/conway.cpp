#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <cmath>

#include <glm-1.0.2/glm/glm.hpp>
#include <glm-1.0.2/glm/gtc/matrix_transform.hpp>
#include <glm-1.0.2/glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shaderClass.h"
#include "renderer.h"
#include "texture.h"
#include "vertex.h"
void print(const char* str){
    std::cout << str << "\0";

}
int main(){
    if (!glfwInit()){
        print("glfw nao iniciou");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(200, 200, "teste", NULL, NULL);
    if (!window){
        print("falha ao criar janela");
        glfwTerminate() ;
        return -1;
    }   
    glfwMakeContextCurrent(window);
    
    gladLoadGL();
    glViewport(0, 0, 200, 200);
    
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height){glViewport(0,0,width,height);});
    


    Shader shd("shaders/conway.vert", "shaders/conway.frag");
    
    GLfloat squaresVertices[] = {
        -100.0f, 100.0f, 0.0f,      0.8f, 0.3f, 0.16f,      0.0f, 0.0f,     0.0f,
        -100.0f, -100.0f, 0.0f,     0.6f, 0.6f, 0.04f,      0.0f, 1.0f,     0.0f,
        100.0f, -100.0f, 0.0f,      0.2f, 0.0f, 0.32f,      1.0f, 1.0f,     0.0f,
        100.0f, 100.0f, 0.0f,       0.2f, 0.6f, 0.04f,      1.0f, 0.0f,     0.0f,
    };

    GLuint indices[] = {
        0, 1, 2, 0, 2, 3
    };

    
    VAO VAO1;//, VAO2;
    VBO VBO1(nullptr, sizeof(squaresVertices));//, VBO2(vertices);
    Layout layout;
    layout.push(GL_FLOAT,3);
    layout.push(GL_FLOAT,3);
    layout.push(GL_FLOAT,2);
    layout.push(GL_FLOAT,1);
    VAO1.addBuffer(VBO1, layout);
    
    EBO EBO1(indices, sizeof(indices));
    EBO1.Bind();


    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 200, 200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    
    
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 200, 200);
    glRenderbufferStorage(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE){
        print("coisou o framebuffer deboas");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Texture textureImg("textures/teste.png");
    
    GLuint tex0Uni = shd.getUniformLocation("tex0");//glGetUniformLocation(shd.ID, "tex0");
    shd.Bind();
    glUniform1i(tex0Uni, 0);

    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();
    shd.Unbind();

    


    Renderer renderer;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();


    print("count: " + EBO1.getCount());
    while (!glfwWindowShouldClose(window)){
       

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        
        
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.2, .13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shd.Bind();
        renderer.draw(VAO1, EBO1, shd);


        glDisable(GL_DEPTH_TEST);
        // shd.Bind();
        // VAO1.Bind();
        // VBO1.Bind();
        
        // GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));
        // glActiveTexture(GL_TEXTURE0);
        // GLCall(glBindTexture(GL_TEXTURE_2D, texture.ID));



        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    glDeleteFramebuffers(1, &FBO);
    glfwDestroyWindow(window);
    glfwTerminate();
}