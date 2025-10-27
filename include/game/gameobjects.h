#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "vertex.h"

#include "gamesettings.h"


void createQuad(std::vector<Vertex>& outObjVertices, std::vector<GLuint>& outVerticesOrder, int gridX, int gridY, int gridW, int gridH);


class Object{
    protected:
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    glm::vec2 m_pos = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_spd = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_acc = glm::vec2(0.0f, 0.0f);
    glm::mat4 m_matModel;
    float m_coef_atrito = 0.0f;
    // GLFWwindow* m_window;

    public:
    std::vector<Vertex>& getVertices();
    std::vector<GLuint>& getIndices();

    // void Object::applyForce(float x, float y);
    void applyForce(glm::vec2 vec);

    virtual void update(float dT, GLFWwindow* window);

    glm::mat4 getModelMatrix();
    Object(float coef_atrito = 0.0f) : m_coef_atrito(coef_atrito), m_pos(viewRes.x/2, viewRes.y/2) {};
    Object(const Object& other);
};
namespace objects{
    class Collision : public Object{
        public:
        using Object::Object;
        Collision(int gridX, int gridY, int gridW, int gridH);
    };

    class Player : public Object{
        public:
        using Object::Object;
        Player(int gridX, int gridY);
        void update(float dT, GLFWwindow* window) override;
        void getInput(float dT, GLFWwindow* window);

    };
}