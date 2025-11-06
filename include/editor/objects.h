#pragma once
#include <vector>
#include <list>
#include "editor/ffmpeg.h"

class Track;
class Clip{
    Track& m_track;
    const char* m_filepath;
    unsigned int m_vidStart;
    unsigned int m_trackStart;
    unsigned int m_trackEnd; 
    unsigned int m_fps;
    
    public:
    Clip(Track& track, const char* filepath, unsigned int vidStart, unsigned int trackStart, unsigned int trackEnd);
    void split(unsigned int time);
    int get_frame_number(unsigned int trackTime);
    void load_frame(unsigned char** out_data, unsigned int frame_number); 
};
class Track{
    std::list<Clip> m_clips;
    // volume, efeitos, sla
    public:
    Clip& create_clip(const char* filepath, unsigned int trackStart, unsigned int trackEnd, unsigned int vidStart);
    std::list<Clip>& get_clips();
};
class Editor;

class Playhead{
    Editor& m_editor;
    unsigned int m_time = 0;
    
    public:
    Playhead(Editor& editor) : m_editor(editor){};
    bool m_playing = false;
    void update(float dT);
};

class Editor{
    Playhead m_playback;
    std::vector<Track> m_tracks;
    
    public:
    Editor() : m_playback(*this){};
    void create_track();
    void set_playing();
    void set_playing(bool state);
    void update(float dT);
    Track& get_track(unsigned int index);
    std::list<Clip>& get_clips_at_time(unsigned int time);
};
