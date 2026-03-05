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
    int id;
    std::list<Clip> clips;
};
struct Timeline{
    float gui_playhead = 0.0f;
    float playhead_time = 0.0f;
    bool isPlaying = false;
    std::list<Track> tracks_;


    Clip* add_clip(size_t track, const char* filename, float time0, float time1){
        //printf("add clip\n");
        int _id = 0;
        for(std::list<Track>::iterator it = tracks_.begin(); it != tracks_.end(); ++it){
            if (_id == (int)track){
                return &(*it).clips.emplace_back(time0, time1, filename);
            }
            _id++;
        }
        Track* t = &this->tracks_.emplace_back();
        t->id = tracks_.size()-1;
        return &t->clips.emplace_back(time0, time1, filename);

    }
    void update(double dt){
        this->playhead_time += (float)dt;

        for (auto& track : tracks_){
            for (auto& clip : track.clips){
                // clip.videoReader.seek_frame(playhead_time);
                // printf("playtime %f pts %f", this->playhead_time, clip.videoReader.pts);
                // printf("playhead %f this pts in sec %d this pts %d\n", this->playhead_time, (double)clip.videoReader.get_time_base(),  clip.videoReader.pts);
                // if (this->playhead_time < clip.videoReader.){

                // }
                // printf("read pts %d in sec %f playhead %f\n", clip.videoReader.pts, (double)clip.videoReader.pts * clip.videoReader.get_time_base(), playhead_time);
                double pts_in_sec = (double)clip.videoReader.pts * clip.videoReader.get_time_base();
                double diff = (double)playhead_time - pts_in_sec;
                // printf("playhead=%.3f pts=%.3f diff=%.3f\n", (double)playhead_time, pts_in_sec, diff);
                if (diff < -1 || diff > 1){
                    printf("seek. diff=%f\n", diff);
                    clip.videoReader.seek_frame((double)playhead_time);
                    clip.videoReader.read_frame();
                    
                } else if ((double)playhead_time > pts_in_sec){
                    clip.videoReader.read_frame();
                }

            }
        }
    }


    void key_callback(int key, int action){
        bool wasPlaying = this->isPlaying;
        if (key == GLFW_KEY_RIGHT){
            if (action == GLFW_PRESS){
                this->playhead_time += 3.0;
                //printf("key press right\n");
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
            //printf("key space\n");
            this->isPlaying = !this->isPlaying;
        }
    }
};