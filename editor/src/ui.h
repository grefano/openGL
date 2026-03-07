#pragma once
#include "imgui.h"
#include "timeline.hpp"
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

    void draw(Timeline* tl){
        printf("draw timeline\n");
        ImGui::Begin("tl");                          // Create a window called "Hello, world!" and append into it.
        ImDrawList* drawlist = ImGui::GetWindowDrawList();
        ImVec2 screenpos = ImGui::GetCursorScreenPos();
        ImVec2 cursorpos = ImGui::GetMousePos();
        this->set_size(ImGui::GetWindowWidth(), 300);
        drawlist->AddRectFilled(screenpos, ImVec2(screenpos.x+this->size.x, screenpos.y+this->size.y), IM_COL32(40, 40, 40, 255));
        for(auto& track : (*tl).tracks_){
            //printf("track id %d", track.id);
            ImVec2 track_pos = this->get_track_pos(track.id);
            ImVec2 track_size = this->get_track_size(track.id);
            drawlist->AddRectFilled(ImVec2(screenpos.x+track_pos.x, screenpos.y+track_pos.y),
                ImVec2(screenpos.x+track_pos.x+track_size.x, screenpos.y+track_pos.y+track_size.y), IM_COL32(80, 80, 100, 255));

            for(auto& clip : track.clips){
                //printf("clip t0 %f t1 %f\n", (*clip).tl_time0, (*clip).tl_time1);
                ImVec2 pos = this->get_clip_pos(clip.get(), &track, tl);
                ImVec2 size = this->get_clip_size(clip.get(), &track, tl);
                drawlist->AddRectFilled(ImVec2(screenpos.x+pos.x, screenpos.y+pos.y), ImVec2(screenpos.x+pos.x+size.x,screenpos.y+pos.y+size.y), IM_COL32(0, 0, 255, 255));
            }
        }

        ImGui::End();
        printf("end draw timeline\n");
    }

};


struct PreviewUI{
    
    void draw(Timeline* tl, ImTextureRef tex){
        printf("draw preview\n");

        static double lasttime = 0;
        double now = glfwGetTime();
        double dt = now - lasttime;        
        lasttime = now;
        ImGui::Begin("preview");                          // Create a window called "Hello, world!" and append into it.
        auto drawlist = ImGui::GetWindowDrawList();
        ImVec2 screenpos = ImGui::GetCursorScreenPos();
        ImVec2 cursorpos = ImGui::GetMousePos();
        ImGui::BeginDisabled();
        float _dtfloat = dt;
        ImGui::SliderFloat("dt", &_dtfloat, 0, 1);
        ImGui::EndDisabled();
        ImGui::SliderFloat("playhead", &(*tl).playhead_time, 0, 100);
        ImGui::Image(tex, ImVec2(640, 360));

        ImGui::End();

        printf("end draw preview\n");

    }
};