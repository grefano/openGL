#include <iostream>
#include <fstream>
#include <future>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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

double playhead_time = 0;
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

bool video_jump_to_ts(float ts_sec, VideoReaderState* state, double* pt_seconds, int64_t* pts){
    glfwSetTime(ts_sec);
    *pt_seconds = ts_sec;
    *pts = (int64_t)(*pt_seconds  * (double)state->time_base.den / (double)state->time_base.num);
    video_reader_seek_frame(state, *pts);

    return true;
}

bool isPlaying = true;
int64_t pts;
VideoReaderState state;
double dt = 0;



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){

    bool wasPlaying = isPlaying;
    if (key == GLFW_KEY_RIGHT){
        if (action == GLFW_PRESS){
            playhead_time += 3.0;
            printf("key press right");
        } else if (action == GLFW_REPEAT){
            isPlaying = false;
            playhead_time += dt / 10000;
        }
        pts = (int64_t)(playhead_time * (double)state.time_base.den / (double)state.time_base.num);
        video_reader_seek_frame(&state, pts);
    }
    if (key == GLFW_KEY_LEFT){
        if (action == GLFW_PRESS){
            playhead_time -= 3.0;
            
        } else if (action == GLFW_REPEAT){
            isPlaying = false;
            playhead_time -= dt / 10000;
        }
        pts = (int64_t)(playhead_time * (double)state.time_base.den / (double)state.time_base.num);
        video_reader_seek_frame(&state, pts);
    }
    isPlaying = wasPlaying;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        printf("key space\n");
        isPlaying = !isPlaying;
    }
}



int main(){
    if (!glfwInit()){
        log("falha inicializando glfw");
        return -1;
    }
    int frame_width = 640, frame_height = 360;
    if (!video_reader_open(&state, "teste.mp4")){
        log("nao abriu video");
        return 0;
    }
    float gui_playhead = 0;
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
    glfwSetKeyCallback(window, key_callback);

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
    
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
    double lasttime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("poggers");                          // Create a window called "Hello, world!" and append into it.
        ImGui::SliderFloat("playback", &gui_playhead, 0, 100);

        ImGui::End();


        double now = glfwGetTime();
        dt = now - lasttime;
        lasttime = now;
        if (isPlaying){
            playhead_time += dt;
            gui_playhead = (float)playhead_time;
        } else {
            playhead_time = (double)gui_playhead;
        }
        
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        double pt_seconds = pts * (double)state.time_base.num / (double)state.time_base.den;
        if (pt_seconds <= playhead_time){

            if (!video_reader_read_frame(&state, &data, &pts)){
                log("nao carregou o frame");
                return 0;
            }
        } else {
            pts = (int64_t)(playhead_time * (double)state.time_base.den / (double)state.time_base.num);
            video_reader_seek_frame(&state, pts);
            pt_seconds = playhead_time;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        std::cout << "pt_seconds: " << pt_seconds << " playhead time: " << playhead_time << std::endl;
        

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
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        
        // glfwWaitEvents();
        glfwPollEvents();
        
    }
    video_reader_close(&state);
    
    delete[] data;
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}