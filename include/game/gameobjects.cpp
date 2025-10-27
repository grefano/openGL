#include <iostream>

#include <glm-1.0.2/glm/glm.hpp>
#include <glm-1.0.2/glm/gtc/matrix_transform.hpp>

#include "gameobjects.h"

void createQuad(std::vector<Vertex>& outObjVertices, std::vector<GLuint>& outVerticesOrder, int gridX, int gridY, int gridW, int gridH){
    std::array<GLuint, 6> verticesOrder = {0, 1, 2, 0, 2, 3};
    
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

std::vector<Vertex>& Object::getVertices(){ return m_vertices; }

std::vector<GLuint>& Object::getIndices(){ return m_indices; }

// Object::Object(float coef_atrito = 0.0f) : m_coef_atrito(coef_atrito){};

Object::Object(const Object& other){
    std::cout << "Object foi copiado\0"; 
}

// void Object::applyForce(float x, float y){
//     m_acc.x += x;
//     m_acc.y += y;
// }

void Object::applyForce(glm::vec2 vec){
    m_acc += vec;
}

void Object::update(float dT, GLFWwindow* window){


    m_spd += m_acc * dT;
    m_spd *= 1-m_coef_atrito;
    m_pos += m_spd * dT;

    m_acc = glm::vec2(0.0f);

    m_matModel = glm::translate(glm::mat4(1.0f), glm::vec3(m_pos.x, m_pos.y, 0.0f));
}

glm::mat4 Object::getModelMatrix(){
    return m_matModel;
}

objects::Collision::Collision(int gridX, int gridY, int gridW, int gridH){
    createQuad(m_vertices, m_indices, gridX, gridY, gridW, gridH);

}

objects::Player::Player(int gridX, int gridY) : Object(.1f){
    createQuad(m_vertices, m_indices, gridX, gridY, 2, 2);
}

void objects::Player::getInput(float dT, GLFWwindow* window){
    bool kRight = (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
    bool kLeft = (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
    int kAxisH = kRight - kLeft;

    bool kDown = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
    bool kUp = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
    int kAxisV = kUp - kDown;

    if (kAxisH == 0 && kAxisV == 0)
        return;
    glm::vec2 move = glm::vec2(kAxisH, kAxisV);
    glm::vec2 acc = glm::normalize(move);
    acc.x *= 1000.0f;
    acc.y *= 600.0f;
    m_acc += acc;
    std::cout << "move " << move.x << " " << move.y << " movenorm " << acc.x << " " << acc.y << std::endl;
    // float ang = glm::atan((float)kAxisV, (float)kAxisH);
    // std::cout << "ang " << ang << std::endl;
    // m_acc += glm::vec2(kAxisH*600.0f * glm::cos(ang), kAxisV*600.0f * glm::sin(ang));

}


void objects::Player::update(float dT, GLFWwindow* window) {
    getInput(dT, window);
    m_acc.y -= 300.0f;

    m_spd += m_acc * dT;
    m_spd.x -= m_spd.x*m_coef_atrito;

    glm::vec2 posNext = m_pos + m_spd;


    float yToBottom = 20.0f;
    float ybottom = m_pos.y - yToBottom;
    float xmiddle = m_pos.x;
    if ((posNext.y - yToBottom) < 0){
        while (ybottom-1 > 0){
            
            ybottom -= 1;
        }
        m_pos.y = ybottom + yToBottom;
        
        m_spd.y = 0.0f;
    }

    m_pos += m_spd * dT;
    m_acc = glm::vec2(0.0f);

    m_matModel = glm::translate(glm::mat4(1.0f), glm::vec3(m_pos.x, m_pos.y, 0.0f));
}
