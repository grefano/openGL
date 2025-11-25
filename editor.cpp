#include <iostream>
#include <fstream>
#include <future>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "editor/trackalloc.h"
#include "editor/ffmpeg.h"

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
    if (!glfwInit()){
        log("falha inicializando glfw");
        return -1;
    }
    int frame_width = 640, frame_height = 360;
    VideoReaderState state;
    if (!video_reader_open(&state, "teste.mp4")){
        log("nao abriu video");
        return 0;
    }
    // unsigned char* data;
    uint8_t* data = new uint8_t[frame_width*frame_height*4];
    // std::unique_ptr<uint8_t> data = std::make_unique<uint8_t>(frame_width*frame_height*4);
    // if (!video_reader_read_frame("teste.mp4", &data, &frame_width, &frame_height)){
  
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
    

    while (!glfwWindowShouldClose(window)) {

        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        int64_t pts;
        
        if (!video_reader_read_frame(&state, &data, &pts)){
            log("nao carregou o frame");
            return 0;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        // int window_width, window_height;
        // glfwGetFramebufferSize(window, &window_width, &window_height);
        static bool first_frame = true;
        if (first_frame){
            glfwSetTime(0.0);
            first_frame = false;
        }
        double pt_seconds = pts * (double)state.time_base.num / (double)state.time_base.den;
        while(pt_seconds > glfwGetTime()){
            glfwWaitEventsTimeout(pt_seconds - glfwGetTime());
        }

        std::cout << "pt_seconds before skip: " << pt_seconds << std::endl; 
        static bool skipped = false;
        if (pt_seconds > 5.0 && !skipped){
            glfwSetTime(10.0);
            pt_seconds = 10.0;
            pts = (int64_t)(pt_seconds * (double)state.time_base.den / (double)state.time_base.num);
            video_reader_seek_frame(&state, pts);
            skipped = true;
        }


        static bool skipped2 = false;
        if (pt_seconds > 20.0 && !skipped2){
            glfwSetTime(5.0);
            pt_seconds = 5.0;
            pts = (int64_t)(pt_seconds * (double)state.time_base.den / (double)state.time_base.num);
            video_reader_seek_frame(&state, pts);
            skipped2 = true;
        }
        std::cout << "pt_seconds: " << pt_seconds << std::endl;
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
    video_reader_close(&state);
    
    delete[] data;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}