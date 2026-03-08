#include "shadercomponent.hpp"

void ComponentShader::bind_shader(const char* vs, const char* fs){
    printf("-- comp bind shader\n");
    this->shader = createShader(vs, fs);
    printf("--\n");
}

GLuint ComponentShader::get_tex(GLuint tex){
    static GLuint fbo = 0;

    if (fbo == 0) {
        glGenFramebuffers(1, &fbo);
    }
    int w, h;
    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

    GLuint resultTexture;
    glGenTextures(1, &resultTexture);
    glBindTexture(GL_TEXTURE_2D, resultTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        resultTexture,
        0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("FBO ERROR\n");
    }

    glViewport(0,0,w,h);


    glUseProgram(this->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(shader,"tex"),0);

    glClear(GL_COLOR_BUFFER_BIT);

    this->set_uniform(shader);

    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return resultTexture;
}


void Transform::set_uniform(GLuint shader){
    glUniform2f(glGetUniformLocation(shader, "offset"), position.x, position.y);
    glUniform2f(glGetUniformLocation(shader, "scale"), scale.x, scale.y);
    
}



void Overlap::set_uniform(GLuint shader){
    glUniform2f(glGetUniformLocation(shader, "offset"), position.x, position.y);
    glUniform2f(glGetUniformLocation(shader, "scale"), scale.x, scale.y);
    
}

