#include <iostream>
#include "shaderClass.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>

using namespace std;

string get_file_contents(const char* filename){
    ifstream in(filename, ios::binary);
    if (in){
        string contents;
        in.seekg(0, ios::end);
        contents.resize(in.tellg());
        in.seekg(0, ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    return "";
}


Shader::Shader(const char * filenamevert, const char * filenamefrag) : fragFilename(filenamefrag), vertFilename(filenamevert){
    std::string vertCode = get_file_contents(vertFilename);
    std::string fragCode = get_file_contents(fragFilename);

    const char* vertSrc = vertCode.c_str();
    const char* fragSrc = fragCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertSrc, NULL);
    glCompileShader(vertexShader);

    // verificando erros
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERRO: vertex shader falhou durante compilação" << infoLog << endl;
    }


    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSrc, NULL);
    glCompileShader(fragmentShader);

    // verificando erros
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERRO: fragment shader falhou durante compilação" << infoLog << endl;
    }


    ID = glCreateProgram();   

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);

    glLinkProgram(ID);
    
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success){
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        cout << "ERRO: shader program falhou durante compilação" << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
};

Shader::~Shader(){
    std::cout << "deleted shader " << std::endl;
    glDeleteProgram(ID);
}

void Shader::Bind() const{
    glUseProgram(ID);
}

void Shader::Unbind() const{
    glUseProgram(0);
}

void Shader::Delete(){
    glDeleteProgram(ID);

}
