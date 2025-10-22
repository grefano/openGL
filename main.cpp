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


using namespace std;

static array<Vertex, 4> createQuad(float x, float y){
    Vertex v0;
    v0.pos = {-100.0f, 100.0f, 0.0f};
    v0.color = {0.8f, 0.3f, 0.16f};
    v0.texcoord = {0.0f, 0.0f};
    v0.texid = {0.0f};
    Vertex v1;
    v1.pos = {-100.0f, -100.0f, 0.0f};
    v1.color = {0.6f, 0.6f, 0.04f};
    v1.texcoord = {0.0f, 1.0f};
    v1.texid = {0.0f};

    Vertex v2;
    v2.pos = {100.0f, -100.0f, 0.0f};
    v2.color = {0.2f, 0.0f, 0.32f};
    v2.texcoord = {1.0f, 1.0f};
    v2.texid = {0.0f};

    Vertex v3;
    v3.pos = {100.0f, 100.0f, 0.0f};
    v3.color = { 0.2f, 0.6f, 0.04f};
    v3.texcoord = {1.0f, 0.0f};
    v3.texid = {0.0f};

    return {v0, v1, v2, v3};
}

int main(){
    if (!glfwInit()){
        cout << "falha inicializando glfw" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);




    GLfloat squaresVertices[] = {
        -100.0f, 100.0f, 0.0f,      0.8f, 0.3f, 0.16f,      0.0f, 0.0f,     0.0f,
        -100.0f, -100.0f, 0.0f,     0.6f, 0.6f, 0.04f,      0.0f, 1.0f,     0.0f,
        100.0f, -100.0f, 0.0f,      0.2f, 0.0f, 0.32f,      1.0f, 1.0f,     0.0f,
        100.0f, 100.0f, 0.0f,       0.2f, 0.6f, 0.04f,      1.0f, 0.0f,     0.0f,

        150.0f, 350.0f, 0.0f,       0.8f, 0.3f, 0.16f,      0.0f, 0.0f,     1.0f,
        150.0f, 150.0f, 0.0f,       0.6f, 0.6f, 0.04f,      0.0f, 1.0f,     1.0f,
        350.0f, 150.0f, 0.0f,       0.2f, 0.0f, 0.32f,      1.0f, 1.0f,     1.0f,
        350.0f, 350.0f, 0.0f,       0.2f, 0.6f, 0.04f,      1.0f, 0.0f,     1.0f,

        150.0f, 550.0f, 0.0f,       0.8f, 0.3f, 0.16f,      0.0f, 0.0f,     1.0f,
        150.0f, 350.0f, 0.0f,       0.6f, 0.6f, 0.04f,      0.0f, 1.0f,     0.0f,
        350.0f, 350.0f, 0.0f,       0.2f, 0.0f, 0.32f,      1.0f, 1.0f,     0.0f,
        350.0f, 550.0f, 0.0f,       0.2f, 0.6f, 0.04f,      1.0f, 0.0f,     1.0f,

    };

    Vertex vertices[4];
    auto q0 = createQuad(0,0); 
    memcpy(vertices, q0.data(), q0.size() * sizeof(Vertex));
    for(Vertex v : vertices){
        cout << "vertex: ";
        cout << "pos: " << v.pos[0] << " " << v.pos[1] << " " << v.pos[2] << endl;
        cout << "col: " << v.color[0] << " " << v.color[1] << " " << v.color[2] << endl;
        cout << "texcoord: " << v.texcoord[0] << " " << v.texcoord[1] << endl;
        cout << "texid: " << v.texid << endl;

    }
    GLuint indices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
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
    
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height){glViewport(0,0,width,height);});
    
    Shader shd("shaders/default.vert", "shaders/default.frag");
    Shader shd2("shaders/default.vert", "shaders/default.frag");
    


    
    VAO VAO1;//, VAO2;
    VBO VBO1(nullptr, sizeof(vertices));//, VBO2(vertices);
    Layout layout;
    layout.push(GL_FLOAT,3);
    layout.push(GL_FLOAT,3);
    layout.push(GL_FLOAT,2);
    layout.push(GL_FLOAT,1);
    VAO1.addBuffer(VBO1, layout);
    
    EBO EBO1(indices, sizeof(indices));
    EBO1.Bind();
    
    glm::mat4 matProj = glm::ortho(0.0f, 800.0f, 0.0f, 800.0f);
    
    Texture texture("textures/teste.png");
    Texture texture2("textures/teste2.png");
    

    // GLuint tex0Uni = shd.getUniformLocation("tex0");//glGetUniformLocation(shd.ID, "tex0");
    // GLuint tex1Uni = glGetUniformLocation(shd.ID, "tex1");
    shd.Bind();
    // glUniform1i(tex0Uni, 0);
    unsigned int transformLoc = shd.getUniformLocation("transform");//glGetUniformLocation(shd.ID, "transform");
    
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();
    shd.Unbind();

    


    Renderer renderer;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

    ImVec4 clear_color = ImVec4(0.43f, 0.55f, 0.60f, 1.0f);


    glm::vec2 translation = glm::vec2(0.0f, 0.0f);
    // glEnable(GL_DEPTH_TEST);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    cout << "count: " << EBO1.getCount() << endl;
    while (!glfwWindowShouldClose(window)){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("poggers");                          // Create a window called "Hello, world!" and append into it.

        // ImGui::Checkbox("Another Window", &show_another_window);
        ImGui::SliderFloat2("translation", &translation.x, 0, 800);            // Edit 1 float using a slider from 0.0f to 1.0f


        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        glClearColor(0.2, .13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        shd.Bind();
        
        VAO1.Bind();
        VBO1.Bind();
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));
        // auto loc = glGetUniformLocation(shd.ID, "textures[0]");
        // assert(loc);
        // std::cout << "loc: " << loc << endl;
        // glUniform1f(loc, 0.0f);
        // loc = glGetUniformLocation(shd.ID, "textures[1]");
        // assert(loc);
        // std::cout << "loc: " << loc << endl;
        // glUniform1f(loc, 1.0f);
        int samplers[2] = {0, 1};
        auto loc = glGetUniformLocation(shd.ID, "textures");   
        glUniform1iv(loc, 2, samplers);
        glm::mat4 matModel = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, 0.0f));
        glm::mat4 matResult = matProj * matModel;
        // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));    
        // trans = glm::rotate(trans, (float)glfwGetTime() * -1.0f, glm::vec3(0.0f, 0.0f, 1.0f));    
        glUniformMatrix4fv(transformLoc,1, GL_FALSE, glm::value_ptr(matResult));
        
        // GLCall(glBindTexture(GL_TEXTURE_2D, texture2.ID));
        
        glActiveTexture(GL_TEXTURE0);
        GLCall(glBindTexture(GL_TEXTURE_2D, texture.ID));
        glActiveTexture(GL_TEXTURE0 + 1);
        GLCall(glBindTexture(GL_TEXTURE_2D, texture2.ID));
        // renderer.draw(VAO1, EBO1, shd);

        // trans = glm::mat4(1.0f);
        // trans = glm::translate(trans, glm::vec3(f, 0.0f, 0.0f));
        // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));    
        // // trans = glm::rotate(trans, (float)glfwGetTime() * -1.0f, glm::vec3(0.0f, 0.0f, 1.0f));    
        // glUniformMatrix4fv(transformLoc,1, GL_FALSE, glm::value_ptr(trans));

        renderer.draw(VAO1, EBO1, shd);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}