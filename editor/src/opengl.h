#pragma once
#include <glad.h>
GLuint create_texture(){
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    return tex;
}

void image_to_tex(GLuint* tex, uint8_t* buffer, int w, int h){
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        w,
        h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        buffer
    );

}

// GLuint overlap_textures(GLuint tex1, GLuint tex2){
    
//     GLuint fbo;
//     GLuint resultTexture;

//     glGenFramebuffers(1, &fbo);
//     glBindFramebuffer(GL_FRAMEBUFFER, fbo);

//     glGenTextures(1, &resultTexture);
//     glBindTexture(GL_TEXTURE_2D, resultTexture);

//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

//     glFramebufferTexture2D(
//         GL_FRAMEBUFFER,
//         GL_COLOR_ATTACHMENT0,
//         GL_TEXTURE_2D,
//         resultTexture,
//         0
//     );


//     glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//     glViewport(0,0,width,height);

//     glUseProgram(shader);

//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, baseTex);


//     glActiveTexture(GL_TEXTURE1);
//     glBindTexture(GL_TEXTURE_2D, overlayTex);

//     RenderQuad(); // quad fullscreen

//     return resultTexture;
// }