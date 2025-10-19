#pragma once
#include <glad/glad.h>

class Texture{
    public:
    GLuint ID;
    Texture(const char* filename);
    ~Texture();
};