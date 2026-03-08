#include "timeline.hpp"

static bool debug = false;
uint8_t* ClipVideo::get_image(){
    return videoReader.state.frame_buffer;
}
GLuint ClipVideo::get_tex(GLuint fbo){
    GLuint tex = this->tex;

    for(auto& compptr : this->shader_components){
        if (debug)
            printf("it comp\n");
        tex = compptr.get()->get_tex(tex, fbo);
    }

    return tex;
}

void ClipVideo::update_image(float ts){
    // if (ts < 0){
    //     this->tex = 0;
    //     this->videoReader.pts = 0;
    //     this->videoReader.state.frame_buffer = 0;
    // }
    double pts_in_sec = (double)videoReader.pts * videoReader.get_time_base();
    double diff = (double)ts - pts_in_sec;
    if (debug)
        printf("pts in sec %f time base %f pts %d\n", pts_in_sec, videoReader.get_time_base(), videoReader.pts);
    if (diff < -1 || diff > 1){
        if (debug)
            printf("seek. diff=%f\n", diff);
        videoReader.seek_frame((double)ts);
        videoReader.read_frame();
        
    } else if ((double)ts > pts_in_sec){
        if (debug)
            printf("read\n");
        videoReader.read_frame();
    }
    if (debug){
        printf("w %f h %f", this->w, this->h);
        printf("image to text\n");

    }
    image_to_tex(&this->tex, get_image(), this->w, this->h);


}



struct Timeline;
void WalkerTimeline::walk(Timeline* tl, std::list<Clip*>* out_clips){
    if (debug)
        printf("walk tl\n");

    for (auto it = tl->tracks_.rbegin(); it != tl->tracks_.rend(); ++it){
        walk(std::addressof(*it), out_clips);
    }
}
void WalkerTimeline::walk(Track* track, std::list<Clip*>* out_clips){
    if (debug)
        printf("walk track. track ptr %p clips size=%zu\n", track, track->clips.size());
    if (!track){
        printf("track undefined\n");

    }
    for (auto& ptrclip : track->clips) {
        if (ptrclip == nullptr){
            printf("clip null\n");
        }
        Clip* c = ptrclip.get();
        walk(c, out_clips);
    }
}
void WalkerTimeline::walk(Clip* clip, std::list<Clip*>* out_clips){
    out_clips->push_back(clip);
}
void Timeline::init_shader(){
    glGenFramebuffers(1, &this->fbo);
    this->shd_overlap= createShader(vs, fs);
    
}
Timeline::Timeline(int w, int h){
    this->frame_dimensions = ImVec2(w, h);
    
}
Timeline::~Timeline(){
    glDeleteFramebuffers(1, &this->fbo);
  glDeleteTextures(1, &this->playhead_tex);
    glDeleteProgram(this->shd_overlap);
}
Clip* Timeline::add_clip_video(size_t track, const char* filename, float time0, float time1){
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

void Timeline::update(double dt){
        printf("update\n");
    this->playhead_time += (float)dt;
    //printf("update %f\n", playhead_time);
    // track de baixo pra cima
    // pra cada track, criar uma lista de clipes na ordem
    // o resultado da iteração vai ser oq? (preciso implementar em Clip)

    // walker -> lista de clipes na ordem certa -> chamar clip.get_image pra todos -> imagem final
    std::list<Clip*> clips;
    WalkerTimeline::walk(this, &clips);
    this->playhead_tex = 0;
    // printf("--clip walk size=%zu\n", clips.size());
    for (Clip* clip : clips){
        // printf("clip t0 %f t1 %f\n", clip->tl_time0, clip->tl_time1);
        if (playhead_time < clip->tl_time0 || playhead_time > clip->tl_time1){
            continue;
        }
        float rel_ts = playhead_time-clip->tl_time0;
        clip->update_image(rel_ts);
        // printf("updated image\n");
        printf("RENDER CLIP h = %i\n", clip->h);
        this->playhead_tex = this->playhead_tex == 0 ? clip->get_tex(this->fbo) : overlap_textures(this->playhead_tex, clip->get_tex(this->fbo), this->shd_overlap);
        // this->playhead_tex = overlap_textures(clip->get_tex(), this->playhead_tex, this->shd_overlap);
        
        // printf("overlaped textures\n");

    }


}



void Timeline::key_callback(int key, int action){
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

