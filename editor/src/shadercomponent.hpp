#pragma once

#include "opengl.h"
#include <list>
#include <stdio.h>
#include <memory>
#include <imgui.h>

struct ComponentShader{
    GLuint shader = 0;
    void bind_shader(const char* vs, const char* fs);
    GLuint get_tex(GLuint tex);
    virtual void set_uniform(GLuint shader) = 0;
};

struct Transform : public ComponentShader{
    ImVec2 position;
    ImVec2 scale;

    void set_uniform(GLuint shader) override;
};

struct Overlap : public ComponentShader{
    ImVec2 position;
    ImVec2 scale;
    void set_uniform(GLuint shader) override;


};
