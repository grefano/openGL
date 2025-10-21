#pragma once

#include <glad/glad.h>
#include <iostream>
#include <unordered_map>

std::string get_file_contents(const char* filename);



class Shader{
    const char * fragFilename;
    const char * vertFilename;
    std::string fragCode;
    std::string vertCode;
    const char * vertSrc;
    const char * fragSrc;
    mutable std::unordered_map<std::string, GLint> uniformLocationCache;
    const char * source; 
    public:
    GLuint ID;
    Shader(const char * filenamevert, const char * filenamefrag);
    ~Shader();
    void Bind() const;
    void Unbind() const;
    void Delete();
    GLint getUniformLocation(const std::string& name) const;
};