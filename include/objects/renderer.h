#pragma once
#include <assert.h>
#include "vao.h"
#include "ebo.h"
#include "shaderClass.h"



#define GLCall(x) GLClearError();\
    x;\
    assert(GLLogCall())


void GLClearError();

bool GLLogCall();


class Renderer{
    public:
    void draw(const VAO& vao, const EBO& ebo, const Shader& shader) const;
};