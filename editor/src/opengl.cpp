#include "opengl.h"
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
void RenderQuad()
{
    //printf("render quad\n");
    static GLuint quadVAO = 0;
    static GLuint quadVBO;

    if (quadVAO == 0)
    {
        float quadVertices[] = {

            // pos   // tex
            -1.0f,  1.0f, 0.0f,1.0f,
            -1.0f, -1.0f, 0.0f,0.0f,
             1.0f, -1.0f, 1.0f,0.0f,

            -1.0f,  1.0f, 0.0f,1.0f,
             1.0f, -1.0f, 1.0f,0.0f,
             1.0f,  1.0f, 1.0f,1.0f
        };

        glGenVertexArrays(1,&quadVAO);
        glGenBuffers(1,&quadVBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
        glBufferData(GL_ARRAY_BUFFER,sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);
}
GLuint createShader(const char* vs, const char* fs)
{
    //printf("shader source:\n%s\n", vs);
    //printf("shader source:\n%s\n", fs);
    //printf("create shader\n");
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex,1,&vs,nullptr);
    glCompileShader(vertex);
    printf("vertex shader %d\n", vertex);
    //printf("awdawd\n");
    GLint success;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        char info[512];
        glGetShaderInfoLog(vertex,512,NULL,info);
        printf("error %s\n", info);
    }
    //printf("create fragment\n");

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment,1,&fs,nullptr);
    glCompileShader(fragment);

    //printf("program\n");
    GLuint program = glCreateProgram();
    glAttachShader(program,vertex);
    glAttachShader(program,fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}
GLuint overlap_textures(GLuint tex_below, GLuint tex_above, GLuint shader)
{
    static GLuint fbo;

    static GLuint resultTexture;

    if (fbo == 0){
        printf("CRIOU FBO");
        glGenFramebuffers(1,&fbo);
        glBindFramebuffer(GL_FRAMEBUFFER,fbo);
    
        glGenTextures(1,&resultTexture);
        glBindTexture(GL_TEXTURE_2D,resultTexture);
    
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,640,360,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,resultTexture,0);
        GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, drawBuffers);

    }
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){
        printf("FBO ERROR\n");
    }
        glBindFramebuffer(GL_FRAMEBUFFER,fbo);

    glViewport(0,0,640,360);

    glUseProgram(shader);
    GLint current;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex_below);
    glUniform1i(glGetUniformLocation(shader,"tex1"),0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,tex_above);
    glUniform1i(glGetUniformLocation(shader,"tex2"),1);

    glClear(GL_COLOR_BUFFER_BIT);

    // glClearColor(1,0,0,1);
// glClear(GL_COLOR_BUFFER_BIT);

glUniform2f(glGetUniformLocation(shader, "overlayPos"), 0.25f + sin(glfwGetTime())*.2, 0.25f);
glUniform2f(glGetUniformLocation(shader, "overlaySize"), 0.5f, 0.5f);
    RenderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER,0);

    return resultTexture;
}


const char* vs = R"(#version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTex;
    
    out vec2 TexCoord;
    
    void main()
    {
        TexCoord = aTex;
        gl_Position = vec4(aPos, 0.0, 1.0);
    })";
const char* fs = R"(#version 330 core
            in vec2 TexCoord;
    
            uniform sampler2D tex1;
            uniform sampler2D tex2;
            
            out vec4 FragColor;
            uniform vec2 overlayPos;   // posição do canto superior esquerdo (0.0 a 1.0)
            uniform vec2 overlaySize;  // tamanho (ex: 0.5, 0.5 = metade da tela)

            void main()
            {

            vec4 base = texture(tex1, TexCoord);
            // vec4 overlay = texture(tex2, TexCoord);
                // if (TexCoord.x > 0.2 && TexCoord.x < 0.8 && TexCoord.y > 0.2 && TexCoord.y < 0.8){
                //     FragColor = base;
                    
                //     } else {
                //         FragColor = overlay;
                // }

                vec2 overlayCoord = (TexCoord - overlayPos) / overlaySize;

                if (overlayCoord.x >= 0.0 && overlayCoord.x <= 1.0 &&
                    overlayCoord.y >= 0.0 && overlayCoord.y <= 1.0)
                {
                    vec4 overlay = texture(tex2, overlayCoord);
                    FragColor = mix(base, overlay, 1.0);
                }
                else
                {
                    FragColor = base;
                }
            })";
    