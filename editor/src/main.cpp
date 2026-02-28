#include <iostream>
#include <fstream>
#include <future>
#include <memory>
#include <glad.h>



#include "ffmpeg.h"
#include "timeline.hpp"
#include "implimgui.h"
#include "implglfw.h"

// preview (codec, processar e mostrar proximo frame, cache de timelines) - figma
Timeline tl;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    tl.key_callback(key, action);
}

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
    // VideoReader videoReader("teste.mp4");
    
    auto videoReader = tl.add_clip(0, "teste.mp4", 0, 30)->videoReader;
    // unsigned char* data;
    uint8_t* data = new uint8_t[frame_width*frame_height*4];
    // std::unique_ptr<uint8_t> data = std::make_unique<uint8_t>(frame_width*frame_height*4);
    // if (!video_reader_read_frame("teste.mp4", &data, &frame_width, &frame_height)){
    videoReader.read_frame(&data);
    std::cout << "frame res " << frame_width << " " << frame_height << std::endl;
    
    Glfw glfw;
    Imgui imgui(glfw.window_);
    glfwSetKeyCallback(glfw.window_, key_callback);
    
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
    
    
    // double lasttime = glfwGetTime();
    while (!glfwWindowShouldClose(glfw.window_)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
        ImGui::Begin("poggers");                          // Create a window called "Hello, world!" and append into it.
        auto drawlist = ImGui::GetWindowDrawList();
        ImVec2 screenpos = ImGui::GetCursorScreenPos();
        ImVec2 cursorpos = ImGui::GetMousePos();
        drawlist->AddCircle({cursorpos.x, cursorpos.y}, 200, IM_COL32(255,0,0,255), 5, 10);
        drawlist->AddBezierQuadratic(cursorpos, screenpos, {200,200}, IM_COL32(255,0,0,255), 5, 5);
        ImGui::SliderFloat("playback", &tl.gui_playhead, 0, 100);
        ImGui::SliderFloat("playhead", &tl.playhead_time, 0, 100);
        ImGui::Image(tex, ImVec2(frame_width, frame_height));
        ImGui::End();
        
        videoReader.read_frame(&data);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        glOrtho(0, frame_width, frame_height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        
        

        ImGui::Render();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(glfw.window_); // restaura o contexto
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        


        glfwSwapBuffers(glfw.window_);
        
        // glfwWaitEvents();
        glfwPollEvents();
        
    }
    delete[] data;
    

    return 0;
}