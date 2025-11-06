#include "editor/objects.h"
#include "editor/ffmpeg.h"
void Playhead::update(float dT){
    if (m_playing)
    m_time += dT;
    
    std::list<Clip>& active_clips = m_editor.get_clips_at_time(m_time);
    for (auto clip : active_clips){
        int frame = clip.get_frame_number(m_time);
        unsigned char* data;
        clip.load_frame(&data, frame);


        // Frame
    }
}

Clip::Clip(Track& track, const char* filepath, unsigned int vidStart, unsigned int trackStart, unsigned int trackEnd) : 
m_track(track), m_filepath(filepath), m_vidStart(vidStart), m_trackStart(trackStart), m_trackEnd(trackEnd) {};

int Clip::get_frame_number(unsigned int trackTime){
    return (trackTime-m_trackStart) * 24;//m_fps;
}

void Clip::load_frame(unsigned char** out_data, unsigned int frame_number){
    // load_video_frame(m_filepath, nullptr, nullptr, nullptr);
}

void Clip::split(unsigned int trackTime){
    m_track.create_clip(m_filepath, trackTime, m_trackEnd, m_vidStart + trackTime-m_trackStart);
    m_trackEnd = trackTime;
}

// !!! ¿ou editor.create_clip(track)?
Clip& Track::create_clip(const char* filepath, unsigned int trackStart, unsigned int trackEnd, unsigned int vidStart){
    return m_clips.emplace_back(*this,filepath,vidStart,trackStart,trackEnd);
}

std::list<Clip>& Track::get_clips(){
    return m_clips;
}

// void Track::split_clip(unsigned int trackTime){
    //     create_clip(m_filePa)
    // }

std::list<Clip>& Editor::get_clips_at_time(unsigned int trackTime){
    return m_tracks[0].get_clips();
}
void Editor::set_playing(){
    m_playback.m_playing = !m_playback.m_playing;
}
void Editor::set_playing(bool state = NULL){
    m_playback.m_playing = state;
}

void Editor::update(float dT){
    if (m_playback.m_playing){
        m_playback.update(dT);
    }
}

void Editor::create_track(){
    m_tracks.emplace_back();
}

Track& Editor::get_track(unsigned int index){
    return m_tracks[index];
}
