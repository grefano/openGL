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
int main(){
    if (!glfwInit()){
        log("falha inicializando glfw");
        return -1;
    }
    // int frame_width = 1920, frame_height = 1080;


    uint8_t* frame = nullptr;
    Clip* clip = tl.add_clip_video(0, "teste.mp4", 0, 30);
    Clip* clip2 = tl.add_clip_video(1, "teste2.mp4", 0, 30);

    std::cout << "frame res " << clip->w << " " << clip->h << std::endl;
    
    Glfw glfw;
    Imgui imgui(glfw.window_);
    glfwSetKeyCallback(glfw.window_, key_callback);
    
    gladLoadGL();
    glViewport(0, 0, 1920, 1080);
    
    


    double lasttime = glfwGetTime();
    while (!glfwWindowShouldClose(glfw.window_)) {
        double now = glfwGetTime();
        double dt = now - lasttime;        
        lasttime = now;
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
        ImGui::BeginDisabled();
        
   
        // float pts_in_sec = (double)videoReader->pts * videoReader->get_time_base();
        float _dtfloat = dt;
        ImGui::SliderFloat("dt", &_dtfloat, 0, 1);
        // ImGui::SliderFloat("playback", &pts_in_sec, 0, 100);
        ImGui::EndDisabled();
        ImGui::SliderFloat("playhead", &tl.playhead_time, 0, 100);
        ImGui::Image(tl.playhead_tex, ImVec2(640, 360));

        ImGui::End();

        ImGui::Begin("tl");                          // Create a window called "Hello, world!" and append into it.
        drawlist = ImGui::GetWindowDrawList();
        screenpos = ImGui::GetCursorScreenPos();
        cursorpos = ImGui::GetMousePos();
        UItl.set_size(ImGui::GetWindowWidth(), 300);
        drawlist->AddRectFilled(screenpos, ImVec2(screenpos.x+UItl.size.x, screenpos.y+UItl.size.y), IM_COL32(40, 40, 40, 255));
        for(auto& track : tl.tracks_){
            printf("track id %d", track.id);
            ImVec2 track_pos = UItl.get_track_pos(track.id);
            ImVec2 track_size = UItl.get_track_size(track.id);
            drawlist->AddRectFilled(ImVec2(screenpos.x+track_pos.x, screenpos.y+track_pos.y),
                ImVec2(screenpos.x+track_pos.x+track_size.x, screenpos.y+track_pos.y+track_size.y), IM_COL32(80, 80, 100, 255));

            for(auto& clip : track.clips){
                printf("clip t0 %f t1 %f\n", (*clip).tl_time0, (*clip).tl_time1);
                ImVec2 pos = UItl.get_clip_pos(clip.get(), &track, &tl);
                ImVec2 size = UItl.get_clip_size(clip.get(), &track, &tl);
                drawlist->AddRectFilled(ImVec2(screenpos.x+pos.x, screenpos.y+pos.y), ImVec2(screenpos.x+pos.x+size.x,screenpos.y+pos.y+size.y), IM_COL32(0, 0, 255, 255));
            }
        }

        ImGui::End();

        tl.update(dt);

        

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