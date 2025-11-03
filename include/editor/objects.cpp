#include "editor/objects.h"

// Track& Editor::create_track(){
//     m_tracks.emplace_back();
//     // !!! ta certo isso?
//     return m_tracks[m_tracks.size() - 1];
// }

void Playback::update(float dT){
    if (m_playing)
        m_time += dT;
}

Clip::Clip(Track& track, const char* filepath, unsigned int vidStart, unsigned int trackStart, unsigned int trackEnd) : 
    m_track(track), m_filepath(filepath), m_vidStart(vidStart), m_trackStart(trackStart), m_trackEnd(trackEnd) {};

void Clip::split(unsigned int trackTime){
    m_track.create_clip(m_filepath, trackTime, m_trackEnd, m_vidStart + trackTime-m_trackStart);
    m_trackEnd = trackTime;
}

Clip& Track::create_clip(const char* filepath, unsigned int trackStart, unsigned int trackEnd, unsigned int vidStart){
    return m_clips.emplace_back(*this,filepath,vidStart,trackStart,trackEnd);
}

// void Track::split_clip(unsigned int trackTime){
//     create_clip(m_filePa)
// }
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
