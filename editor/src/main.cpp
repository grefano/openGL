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
Timeline tl = Timeline(640, 360);
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

    Clip* clip = tl.add_clip_video(0, "teste.mp4", 0, 30);
    Overlap* comp= clip->add_component<Overlap>();
    comp->position = {.5, .5};
    comp->scale = {1, 1};
    
    Clip* clip2 = tl.add_clip_video(1, "video3.mp4", 5, 10);
    Transform* comp2= clip2->add_component<Transform>();
    comp2->position = {.5, .5};
    comp2->scale = {1, 1};
    

    Glfw glfw;
    Imgui imgui(glfw.window_);
    glfwSetKeyCallback(glfw.window_, key_callback);

    gladLoadGL();
    
    tl.init_shader();
    comp->bind_shader(vs, fs);
    comp2->bind_shader(vs, fs);
    
    
    double lasttime = glfwGetTime();
    while (!glfwWindowShouldClose(glfw.window_)) {
        double now = glfwGetTime();
        double dt = now - lasttime;        
        lasttime = now;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        tl.update(dt);

        UItl.draw(&tl);
        UIpreview.draw(&tl, tl.playhead_tex, tl.frame_dimensions);
        printf("DIM %d %d", clip->w, clip->h);
        ImGui::Image(clip->get_tex(), ImVec2(clip->w, clip->h));
        ImGui::Image(clip2->get_tex(), ImVec2(clip2->w, clip2->h));
        // ImGui::Image(overlap_textures(clip->tex, clip->tex, tl.shd_overlap), ImVec2(640, 360));
        
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