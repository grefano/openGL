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

const int gridSize = 8;

glm::vec2 viewRes(320, 180);
glm::vec2 windowRes(1280, 720);


void createQuad(std::vector<Vertex>& outObjVertices, std::vector<GLuint>& outVerticesOrder, int gridX, int gridY, int gridW, int gridH){
    array<GLuint, 6> verticesOrder = {0, 1, 2, 0, 2, 3};
    
    outVerticesOrder.insert(outVerticesOrder.end(), verticesOrder.begin(), verticesOrder.end());

    float xleft = gridX * gridSize;
    float ytop = -gridY * gridSize;
    float xright = xleft + gridW*gridSize;
    float ybottom = ytop - gridH*gridSize;

    Vertex v0;
    v0.pos = {xleft, ytop, 0.0f};
    v0.color = {0.8f, 0.3f, 0.16f};
    v0.texcoord = {0.0f, 0.0f};
    v0.texid = {0.0f};
    outObjVertices.push_back(v0);

    Vertex v1;
    v1.pos = {xleft, ybottom, 0.0f};
    v1.color = {0.6f, 0.6f, 0.04f};
    v1.texcoord = {0.0f, 1.0f};
    v1.texid = {0.0f};
    outObjVertices.push_back(v1);

    Vertex v2;
    v2.pos = {xright, ybottom, 0.0f};
    v2.color = {0.2f, 0.0f, 0.32f};
    v2.texcoord = {1.0f, 1.0f};
    v2.texid = {0.0f};
    outObjVertices.push_back(v2);

    Vertex v3;
    v3.pos = {xright, ytop, 0.0f};
    v3.color = { 0.2f, 0.6f, 0.04f};
    v3.texcoord = {1.0f, 0.0f};
    v3.texid = {0.0f};
    outObjVertices.push_back(v3);

}
class Object{
    protected:
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;

    public:
    std::vector<Vertex>& getVertices(){ return m_vertices; };
    std::vector<GLuint>& getIndices(){ return m_indices; };

    Object() = default;
    Object(const Object& other){
        cout << "Object foi copiado\0"; 
    };
};
class ObjCollision : public Object{
    public:
    using Object::Object;
    ObjCollision(int gridX, int gridY, int gridW, int gridH){
        createQuad(m_vertices, m_indices, gridX, gridY, gridW, gridH);
    }
};

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


    Vertices vertices;
    ObjCollision col1(2, 2, 1, 3);
    vertices.addObj(col1);
    ObjCollision col2(3, 3, 4, 3);
    vertices.addObj(col2);


    for(Vertex v : vertices.getVertices()){
        cout << "vertex: ";
        cout << "pos: " << v.pos[0] << " " << v.pos[1] << " " << v.pos[2] << endl;
        cout << "col: " << v.color[0] << " " << v.color[1] << " " << v.color[2] << endl;
        cout << "texcoord: " << v.texcoord[0] << " " << v.texcoord[1] << endl;
        cout << "texid: " << v.texid << endl;
    }
    cout << "indices: ";
    for(GLuint v : vertices.getIndices()){
        cout << v << " ";
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
    VBO VBO1(nullptr, vertices.getVertices().size() * sizeof(Vertex));//, VBO2(vertices);
    Layout layout;
    layout.push(GL_FLOAT,3);
    layout.push(GL_FLOAT,3);
    layout.push(GL_FLOAT,2);
    layout.push(GL_FLOAT,1);
    VAO1.addBuffer(VBO1, layout);


    
    EBO EBO1(vertices.getIndices().data(), vertices.getIndices().size() * sizeof(GLuint));
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
        
        shd.Bind();
        
        VAO1.Bind();

        VBO1.Bind();
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.getVertices().size() * sizeof(Vertex), vertices.getVertices().data()));
        
        
        int samplers[2] = {0, 1};
        auto loc = glGetUniformLocation(shd.ID, "textures");   
        glUniform1iv(loc, 2, samplers);
        glm::mat4 matModel = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, 0.0f));
        glm::mat4 matResult = matProj * matModel;
        
        glUniformMatrix4fv(transformLoc,1, GL_FALSE, glm::value_ptr(matResult));
        
        
        glActiveTexture(GL_TEXTURE0);
        GLCall(glBindTexture(GL_TEXTURE_2D, texture.ID));
        glActiveTexture(GL_TEXTURE0 + 1);
        GLCall(glBindTexture(GL_TEXTURE_2D, texture2.ID));

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