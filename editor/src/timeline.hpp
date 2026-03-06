#pragma once
#include <list>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include "ffmpeg.h"
#include <memory>
#include <imgui.h>
#include "opengl.h"
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
    virtual uint8_t* get_image() = 0;
    virtual GLuint get_tex() = 0;
    virtual void update_image(float ts) = 0;
};
struct ClipVideo : public Clip{
    VideoReader videoReader;
    ClipVideo(float t0, float t1, const char* filename) : Clip(t0, t1), videoReader(filename){
        this->w = this->videoReader.w;
        this->h = this->videoReader.h;
    }   
    uint8_t* get_image() override{
        return videoReader.state.frame_buffer;
    }
    GLuint get_tex() override{
        return tex;
    }
    void update_image(float ts) override{
        double pts_in_sec = (double)videoReader.pts * videoReader.get_time_base();
        double diff = (double)ts - pts_in_sec;
        //printf("pts in sec %f time base %f pts %d\n", pts_in_sec, videoReader.get_time_base(), videoReader.pts);
        if (diff < -1 || diff > 1){
            //printf("seek. diff=%f\n", diff);
            videoReader.seek_frame((double)ts);
            videoReader.read_frame();
            
        } else if ((double)ts > pts_in_sec){
        //printf("read\n");
            videoReader.read_frame();
        }

        //printf("image to text\n");
        image_to_tex(&this->tex, get_image(), this->w, this->h);


    }


    
};

struct Track{
    int id;
    std::list<std::unique_ptr<Clip>> clips;
};
struct Timeline{
    float gui_playhead = 0.0f;
    float playhead_time = 0.0f;
    bool isPlaying = false;
    std::list<Track> tracks_;
    uint8_t* playhead_frame;
    GLuint playhead_tex;
    GLuint shd_overlap;
        void init_shader(){
            const char* vs = R"(#version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTex;
    
    out vec2 TexCoord;
    
    void main()
    {
        TexCoord = aTex;
        gl_Position = vec4(aPos, 0.0, 1.0);
    })";
            const char* fs = R"(#version 330 core
            in vec2 TexCoord;
    
            uniform sampler2D tex1;
            uniform sampler2D tex2;
            
            out vec4 FragColor;
            uniform vec2 overlayPos;   // posição do canto superior esquerdo (0.0 a 1.0)
            uniform vec2 overlaySize;  // tamanho (ex: 0.5, 0.5 = metade da tela)

            void main()
            {
                vec4 base = texture(tex1, TexCoord);

                vec2 overlayCoord = (TexCoord - overlayPos) / overlaySize;

                if (overlayCoord.x >= 0.0 && overlayCoord.x <= 1.0 &&
                    overlayCoord.y >= 0.0 && overlayCoord.y <= 1.0)
                {
                    vec4 overlay = texture(tex2, overlayCoord);
                    FragColor = mix(base, overlay, 1.0);
                }
                else
                {
                    FragColor = base;
                }
            })";
    
    
            this->shd_overlap= createShader(vs, fs);
            
        }
    Clip* add_clip_video(size_t track, const char* filename, float time0, float time1){
        int _id = 0;
        for(std::list<Track>::iterator it = tracks_.begin(); it != tracks_.end(); ++it){
            if (_id == (int)track){
                (*it).clips.push_back(std::make_unique<ClipVideo>(time0, time1, filename));
                return (*it).clips.back().get();
            }
            _id++;
        }
        Track* t = &this->tracks_.emplace_back();
        t->id = tracks_.size()-1;
        t->clips.push_back(std::make_unique<ClipVideo>(time0, time1, filename));
        return t->clips.back().get();
    }

    void update(double dt){
        this->playhead_time += (float)dt;
        //printf("update %f\n", playhead_time);
        for (auto& track : tracks_){
            for (auto& clip : track.clips){
                (*clip).update_image(playhead_time);

                this->playhead_tex = overlap_textures(clip.get()->get_tex(), this->playhead_tex, this->shd_overlap);
                // image_to_tex(&this->playhead_tex, clip.get()->get_image(), clip.get()->w, clip.get()->h);
            }
        }
    }



    void key_callback(int key, int action){
        bool wasPlaying = this->isPlaying;
        if (key == GLFW_KEY_RIGHT){
            if (action == GLFW_PRESS){
                this->playhead_time += 3.0;
                ////printf("key press right\n");
            } else if (action == GLFW_REPEAT){
                this->isPlaying = false;
                this->playhead_time += 1;
            }
            // this->pts = (int64_t)(playhead_time / this->selected_video->get_time_base());
            // this->selected_video->seek_frame(this->pts);
        }
        if (key == GLFW_KEY_LEFT){
            if (action == GLFW_PRESS){
                this->playhead_time -= 3.0;
                
            } else if (action == GLFW_REPEAT){
                this->isPlaying = false;
                this->playhead_time -= 1;
            }
            
        }

        if (key == GLFW_KEY_R){
            this->playhead_time = 0;
            
        }

        this->isPlaying = wasPlaying;
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
            ////printf("key space\n");
            this->isPlaying = !this->isPlaying;
        }
    }
};

struct ClipNestedTimeline : public Clip{
    Timeline timeline;
};