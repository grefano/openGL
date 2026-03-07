#include "timeline.hpp"




uint8_t* ClipVideo::get_image(){
    return videoReader.state.frame_buffer;
}
GLuint ClipVideo::get_tex(){
    return tex;
}
void ClipVideo::update_image(float ts){
    double pts_in_sec = (double)videoReader.pts * videoReader.get_time_base();
    double diff = (double)ts - pts_in_sec;
    printf("pts in sec %f time base %f pts %d\n", pts_in_sec, videoReader.get_time_base(), videoReader.pts);
    if (diff < -1 || diff > 1){
        printf("seek. diff=%f\n", diff);
        videoReader.seek_frame((double)ts);
        videoReader.read_frame();
        
    } else if ((double)ts > pts_in_sec){
    printf("read\n");
        videoReader.read_frame();
    }
    printf("w %f h %f", this->w, this->h);
    printf("image to text\n");
    image_to_tex(&this->tex, get_image(), 640, 360);


}



struct Timeline;
void WalkerTimeline::walk(Timeline* tl, std::list<Clip*>* out_clips){
        printf("walk tl\n");

    for (auto it = tl->tracks_.rbegin(); it != tl->tracks_.rend(); ++it){
        walk(std::addressof(*it), out_clips);
    }
}
void WalkerTimeline::walk(Track* track, std::list<Clip*>* out_clips){
        printf("walk track. track ptr %p clips size=%zu\n", track, track->clips.size());
        if (!track){
            printf("track undefined\n");

        }
    for (auto& ptrclip : track->clips) {
        printf("walk loop clips\n");
        if (ptrclip == nullptr){
            printf("clip null\n");
        }
            printf("before get\n");
        Clip* c = ptrclip.get();
        printf("pointer clip %p\n", c);
        walk(c, out_clips);
    }
}
void WalkerTimeline::walk(Clip* clip, std::list<Clip*>* out_clips){
        printf("walk lip\n");
    out_clips->push_back(clip);
}
void Timeline::init_shader(){
    this->shd_overlap= createShader(vs, fs);
    
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

    printf("--clip walk size=%zu\n", clips.size());
    for (Clip* clip : clips){
        printf("clip t0 %f t1 %f\n", clip->tl_time0, clip->tl_time1);
        clip->update_image(playhead_time);
        printf("updated image\n");
        this->playhead_tex = overlap_textures(clip->get_tex(), this->playhead_tex, this->shd_overlap);
        printf("overlaped textures\n");

    }

    // for(auto it = tracks_.begin(); it != tracks_.end(); ++it){

    //     auto track = *it;
        

    //     for (auto& clip : track.clips){
    //         (*clip).update_image(playhead_time);

    //         this->playhead_tex = overlap_textures(clip.get()->get_tex(), this->playhead_tex, this->shd_overlap);
    //         // image_to_tex(&this->playhead_tex, clip.get()->get_image(), clip.get()->w, clip.get()->h);
    //     }
    
    // }
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

