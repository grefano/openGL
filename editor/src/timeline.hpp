#pragma once
#include "opengl.h"
#include "ffmpeg.h"
#include <list>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <imgui.h>
struct Transform{
    ImVec2 position;
    ImVec2 scale;
};


struct Clip{
    GLuint tex;
    float tl_time0;
    float tl_time1;
    int w, h;
    Transform transform;
    Clip(float t0, float t1){
        this->tl_time0 = t0;
        this->tl_time1 = t1;
    }
    ~Clip(){
        glDeleteTextures(1, &tex);
    }
    virtual uint8_t* get_image() = 0;
    virtual GLuint get_tex() = 0;
    virtual void update_image(float ts) = 0;
};
struct ClipVideo : public Clip{
    VideoReader videoReader;
    ClipVideo(float t0, float t1, const char* filename) : Clip(t0, t1), videoReader(filename){};
    uint8_t* get_image() override;
    GLuint get_tex() override;
    void update_image(float ts) override;
};

struct Track{
    int id;
    std::list<std::unique_ptr<Clip>> clips;
};
struct Timeline;
struct WalkerTimeline{
    static void walk(Timeline* tl, std::list<Clip*>* out_clips);
    static void walk(Track* track, std::list<Clip*>* out_clips);
    static void walk(Clip* clip, std::list<Clip*>* out_clips);
};
struct Timeline{
    float gui_playhead = 0.0f;
    float playhead_time = 0.0f;
    bool isPlaying = false;
    std::list<Track> tracks_;
    uint8_t* playhead_frame;
    GLuint playhead_tex;
    GLuint shd_overlap;
    void init_shader();
    Clip* add_clip_video(size_t track, const char* filename, float time0, float time1);
    void update(double dt);
    void key_callback(int key, int action);
};

struct ClipNestedTimeline : public Clip{
    Timeline timeline;
};