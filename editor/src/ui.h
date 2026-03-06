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

};
