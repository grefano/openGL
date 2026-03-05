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

struct TimelineUI{
    ImVec2 view_time_window;
    ImVec2 view_track_window;
    float px_per_sec;
    float px_per_track;
    
    ImVec2 size;
    TimelineUI(){
        this->view_time_window = ImVec2(0, 100);
        this->view_track_window = ImVec2(0, 2);
        this->px_per_sec = 20;
    }
    ImVec2 get_track_pos(int id){
        return ImVec2(5, id*px_per_track);
    }
    ImVec2 get_track_size(int id){
        return ImVec2(this->size.x, (id+1)*px_per_track);
    }
    ImVec2 get_clip_pos(Clip* clip, Track* track, Timeline* tl){
        return ImVec2(clip->tl_time0 - view_time_window.x * px_per_sec, get_track_pos(track->id).y);
    }
    ImVec2 get_clip_size(Clip* clip, Track* track, Timeline* tl){
        return ImVec2( (clip->tl_time1 - clip->tl_time0) * px_per_sec, get_track_size(track->id).y);
    }

    void set_size(float w, float h){
        this->size = ImVec2(w, h);
        this->px_per_sec = w/(view_time_window.y-view_time_window.x);
        this->px_per_track = h / (view_track_window.y+1 - view_track_window.x);
    }

};

TimelineUI UItl;
int main(){
    if (!glfwInit()){
        log("falha inicializando glfw");
        return -1;
    }
    int frame_width = 640, frame_height = 360;
    // VideoReader videoReader("teste.mp4");
    
    auto* videoReader = &tl.add_clip(0, "teste.mp4", 0, 30)->videoReader;
    // unsigned char* data;
    // uint8_t* data = new uint8_t[frame_width*frame_height*4];
    // std::unique_ptr<uint8_t> data = std::make_unique<uint8_t>(frame_width*frame_height*4);
    // if (!video_reader_read_frame("teste.mp4", &data, &frame_width, &frame_height)){
        videoReader->seek_frame(0);

    // videoReader->read_frame();
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
        
   
        float pts_in_sec = (double)videoReader->pts * videoReader->get_time_base();
        float _dtfloat = dt;
        ImGui::SliderFloat("dt", &_dtfloat, 0, 1);
        ImGui::SliderFloat("playback", &pts_in_sec, 0, 100);
        ImGui::EndDisabled();
        ImGui::SliderFloat("playhead", &tl.playhead_time, 0, 100);
        ImGui::Image(tex, ImVec2(frame_width, frame_height));

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
                printf("clip t0 %f t1 %f\n", clip.tl_time0, clip.tl_time1);
                ImVec2 pos = UItl.get_clip_pos(&clip, &track, &tl);
                ImVec2 size = UItl.get_clip_size(&clip, &track, &tl);
                drawlist->AddRectFilled(ImVec2(screenpos.x+pos.x, screenpos.y+pos.y), ImVec2(screenpos.x+pos.x+size.x,screenpos.y+pos.y+size.y), IM_COL32(0, 0, 255, 255));
            }
        }

        ImGui::End();

        // //printf("loop\n");
        // if (!videoReader.read_frame()){
        //     //printf("cant read frame\n");
        // }
        // //printf("after read\n");
        tl.update(dt);
        // videoReader->seek_frame(tl.playhead_time);
        // printf("play time %f\n", tl.playhead_time);
        // videoReader->read_frame();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, videoReader->state.frame_buffer);

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
    // delete[] data;
    

    return 0;
}