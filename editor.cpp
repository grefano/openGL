#include <iostream>
#include <fstream>
#include <future>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "editor/trackalloc.h"
#include "editor/ffmpeg.h"
#include "editor/objects.h"

extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
    #include <libswscale/swscale.h>
}

// preview (codec, processar e mostrar proximo frame, cache de timelines) - figma

void log(const char* str){
    std::cout << str << "\0";
}
void* operator new(size_t size){
    log("alocou " + size);
    return malloc(size);
}

void operator delete(void* memory, size_t size) noexcept{
    log("liberou " + size);
    free(memory);
}


int main(){
    Editor editor;
    editor.create_track();

    Track& track_selected = editor.get_track(0);
    Clip& clip_selected = track_selected.create_clip("teste.mp4", 1000, 2000, 0);
    clip_selected.split(1500);


    int a = 0;
    

    


    return 0;

    if (!glfwInit()){
        log("falha inicializando glfw");
        return -1;
    }
    int frame_width, frame_height;
    unsigned char* data;
    if (!load_frame("teste.mp4", &data, &frame_width, &frame_height)){
        log("nao carregou o frame");
        return 0;
    }
    std::cout << "frame res " << frame_width << " " << frame_height << std::endl;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    GLFWwindow* window = glfwCreateWindow(frame_width, frame_height, "teste", NULL, NULL);
    if (!window){
        log("falha ao criar janela");
        glfwTerminate() ;
        return -1;
    }   
    glfwMakeContextCurrent(window);
    
    gladLoadGL();
    glViewport(0, 0, frame_width, frame_height);
    

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

     while (!glfwWindowShouldClose(window)) {

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            editor.set_playing();
        }
        editor.update(0.0001);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // int window_width, window_height;
        // glfwGetFramebufferSize(window, &window_width, &window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glOrtho(0, frame_width, frame_height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);


        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
        glBegin(GL_QUADS);
            glTexCoord2d(0,0); glVertex2i(0, 0);
            glTexCoord2d(1,0); glVertex2i(frame_width, 0);
            glTexCoord2d(1,1); glVertex2i(frame_width, frame_height);
            glTexCoord2d(0,1); glVertex2i(0, frame_height);
        glEnd();

        glDisable(GL_TEXTURE_2D);



        glfwSwapBuffers(window);

        // glfwWaitEvents();
        glfwPollEvents();

    }

    delete[] data;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}