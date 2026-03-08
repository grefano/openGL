#pragma once
#include "opengl.h"
#include "ffmpeg.h"
#include "shadercomponent.hpp"
#include <list>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <imgui.h>


struct Clip{
    GLuint tex;
    float tl_time0;
    float tl_time1;
    int w, h;
    std::list<std::unique_ptr<ComponentShader>> shader_components;
    Clip(float t0, float t1){
        this->tl_time0 = t0;
        this->tl_time1 = t1;
    }
    ~Clip(){
        glDeleteTextures(1, &tex);
    }
    template <typename T>
    T* add_component();
    virtual uint8_t* get_image() = 0;
    virtual GLuint get_tex(GLuint fbo) = 0;
    virtual void update_image(float ts) = 0;
};

template <typename T>
T* Clip::add_component(){
    printf("\n -- add component\n");
    static_assert(std::is_base_of_v<ComponentShader, T>);

    
    std::unique_ptr<T> element = std::make_unique<T>();
    T* ptr = element.get();
    shader_components.push_back(std::move(element));

    printf("\n -- \n");

    return ptr;
}

struct ClipVideo : public Clip{
    VideoReader videoReader;
    ClipVideo(float t0, float t1, const char* filename) : Clip(t0, t1), videoReader(filename){
        this->w = this->videoReader.w;
        this->h = this->videoReader.h;
    };
    uint8_t* get_image() override;
    GLuint get_tex(GLuint fbo) override;
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
    ImVec2 frame_dimensions;
    GLuint playhead_tex;
    GLuint shd_overlap;
    GLuint fbo;
    
    void init_shader();
    Clip* add_clip_video(size_t track, const char* filename, float time0, float time1);
    void update(double dt);
    void key_callback(int key, int action);
    Timeline(int, int);
    ~Timeline();
};

struct ClipNestedTimeline : public Clip{
    Timeline timeline;
};