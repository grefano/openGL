#pragma once
#include <list>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include "ffmpeg.h"
struct Clip{
    float tl_time0;
    float tl_time1;
    VideoReader videoReader;
    Clip(float t0, float t1, const char* filename) : videoReader(filename){
        this->tl_time0 = t0;
        this->tl_time1 = t1;
    }
};
struct Track{
    std::list<Clip> clips;
};
struct Timeline{
    float gui_playhead = 0.0f;
    float playhead_time = 0.0f;
    bool isPlaying = false;
    std::list<Track> tracks_;


    Clip* add_clip(size_t track, const char* filename, float time0, float time1){
        return &this->tracks_.emplace_back().clips.emplace_back(time0, time1, filename);

    }
    void update(double dt){
        if (this->isPlaying){
            this->playhead_time += .001;
            this->gui_playhead = this->playhead_time;
            printf("PLYAING: dt %f playhead time: %f pts %f\n", dt, this->playhead_time, 0);
        } else {
            this->gui_playhead = this->playhead_time;
            printf("NOTP: dt %f playhead time: %f pts %f\n", dt, this->playhead_time, 0);

        }
    }


    void key_callback(int key, int action){
        bool wasPlaying = this->isPlaying;
        if (key == GLFW_KEY_RIGHT){
            if (action == GLFW_PRESS){
                this->playhead_time += 3.0;
                printf("key press right\n");
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

        this->isPlaying = wasPlaying;
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
            printf("key space\n");
            this->isPlaying = !this->isPlaying;
        }
    }
};