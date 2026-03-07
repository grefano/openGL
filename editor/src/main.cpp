#include <iostream>
#include <fstream>
#include <future>
#include <memory>
#include <glad.h>



#include "ffmpeg.h"
#include "timeline.hpp"
#include "ui.h"
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



TimelineUI UItl;
PreviewUI UIpreview;
// PreviewUI UIpreview2("preview2");
int main(){
    if (!glfwInit()){
        log("falha inicializando glfw");
        return -1;
    }
    // int frame_width = 1920, frame_height = 1080;


    uint8_t* frame = nullptr;
    Clip* clip = tl.add_clip_video(0, "teste.mp4", 0, 30);
    Clip* clip2 = tl.add_clip_video(1, "video3.mp4", 5, 10);
    // Clip* clip2 = tl.add_clip_video(1, "teste2.mp4", 0, 30);

    std::cout << "frame res " << clip->w << " " << clip->h << std::endl;
    
    Glfw glfw;
    Imgui imgui(glfw.window_);
    glfwSetKeyCallback(glfw.window_, key_callback);
    
    gladLoadGL();
    glViewport(0, 0, 1920, 1080);
    
    tl.init_shader();


    double lasttime = glfwGetTime();
    while (!glfwWindowShouldClose(glfw.window_)) {
        printf("loop\n");
        double now = glfwGetTime();
        double dt = now - lasttime;        
        lasttime = now;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
        printf("loop b\n");

        clip->update_image(tl.playhead_time);
        clip2->update_image(tl.playhead_time);
        tl.update(dt);
        // GLuint a = overlap_textures(clip->tex, clip->tex, tl.shd_overlap);
        GLuint a = clip->tex;
        UIpreview.draw(&tl, a);
        GLuint b = overlap_textures(clip2->tex, a, tl.shd_overlap);
        // UIpreview2.draw(&tl, b);
        ImGui::Image(b, ImVec2(640, 360));
        UItl.draw(&tl);
        GLuint c = overlap_textures(clip->tex, b, tl.shd_overlap);
        ImGui::Image(c, ImVec2(640, 360));
        printf("loop a\n");
        ImGui::Render();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(glfw.window_); // restaura o contexto
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        
        

        glfwSwapBuffers(glfw.window_);
        
        // glfwWaitEvents();
        glfwPollEvents();
        
    }
    // delete[] data;
    

    return 0;
}