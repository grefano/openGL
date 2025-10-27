#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <cmath>
#include <memory>

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

#include "gameobjects.h"
#include "gamesettings.h"

using namespace std;
namespace obj = objects;



class Vertices{
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    
    public:
    void addObj(Object& obj){
        std::size_t lastIndex = m_vertices.size();
        for(GLuint i : obj.getIndices())
            m_indices.push_back(lastIndex + i);
        
        m_vertices.insert(m_vertices.end(), obj.getVertices().begin(), obj.getVertices().end());
        
        
    }
    std::vector<Vertex>& getVertices(){ return m_vertices; }
    std::vector<GLuint>& getIndices(){ return m_indices; }
};



int main(){
    if (!glfwInit()){
        cout << "falha inicializando glfw" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    std::vector<std::unique_ptr<Object>> objs;
    objs.push_back(std::make_unique<obj::Collision>(2, 2, 1, 3));
    objs.push_back(std::make_unique<obj::Player>(1, 3));
    
    for(auto& o : objs){
        GLuint verticesSize = vertices.size();
        for(GLuint i : o->getIndices()){
            indices.push_back(verticesSize + i);
        }
        vertices.insert(vertices.end(), o->getVertices().begin(), o->getVertices().end());
    }


    GLFWwindow* window = glfwCreateWindow(viewRes.x, viewRes.y, "teste", NULL, NULL);
    if (!window){
        cout << "falha ao criar janela";
        glfwTerminate() ;
        return -1;
    }   
    glfwMakeContextCurrent(window);
    
    gladLoadGL();
    glViewport(0, 0, viewRes.x, viewRes.y);
    
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glm::mat4 matProj = glm::ortho(0.0f, viewRes.x, 0.0f, viewRes.y);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height){ glViewport(0,0,width,height); });
    
    Shader shd("shaders/default.vert", "shaders/default.frag");
    Shader shd2("shaders/default.vert", "shaders/default.frag");
    


    
    VAO VAO1;//, VAO2;
    // VBO VBO2(nullptr, sizeof(squaresVertices));//, VBO2(vertices);
    VBO VBO1(nullptr, vertices.size() * sizeof(Vertex));//, VBO2(vertices);
    Layout layout;
    layout.push(GL_FLOAT,3);
    layout.push(GL_FLOAT,3);
    layout.push(GL_FLOAT,2);
    layout.push(GL_FLOAT,1);
    VAO1.addBuffer(VBO1, layout);


    
    EBO EBO1(indices.data(), indices.size() * sizeof(GLuint));
    EBO1.Bind();
    
    
    Texture texture("textures/teste.png");
    Texture texture2("textures/teste2.png");
    

    shd.Bind();
    unsigned int transformLoc = shd.getUniformLocation("transform");//glGetUniformLocation(shd.ID, "transform");


    Renderer renderer;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

    glm::vec2 translation = glm::vec2(0.0f, 0.0f);
    // glEnable(GL_DEPTH_TEST);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    cout << "count: " << EBO1.getCount() << endl;
    float lastTime = glfwGetTime();
    float dT = 0;
    float curTime = 0;
    while (!glfwWindowShouldClose(window)){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("poggers");                          // Create a window called "Hello, world!" and append into it.

        // ImGui::Checkbox("Another Window", &show_another_window);
        ImGui::SliderFloat("x", &translation.x, 0, viewRes.x);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("y", &translation.y, 0, viewRes.y);            // Edit 1 float using a slider from 0.0f to 1.0f

        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        glClearColor(0.2, .13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        curTime = glfwGetTime();
        dT = curTime - lastTime;
        lastTime = curTime; 

        shd.Bind();
        
        VAO1.Bind();

        VBO1.Bind();

        
        int samplers[2] = {0, 1};
        auto loc = glGetUniformLocation(shd.ID, "textures");   
        glUniform1iv(loc, 2, samplers);
        
        glActiveTexture(GL_TEXTURE0);
        GLCall(glBindTexture(GL_TEXTURE_2D, texture.ID));
        glActiveTexture(GL_TEXTURE0 + 1);
        GLCall(glBindTexture(GL_TEXTURE_2D, texture2.ID));

        for(auto& o : objs){
            o->applyForce(translation);
            o->update(dT);
            
            glm::mat4 matResult = matProj * (o->getModelMatrix());
            
            glUniformMatrix4fv(transformLoc,1, GL_FALSE, glm::value_ptr(matResult));
            

            
            GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, o->getVertices().size() * sizeof(Vertex), o->getVertices().data()));

            renderer.draw(VAO1, EBO1, shd);
            
        }    
        
        




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