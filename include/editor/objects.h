class Playback{
    unsigned int time;
};

class Track{
    // volume, efeitos, sla
};

class Clip{
    const char* filepath;
    unsigned int vidStart;
    unsigned int vidEnd;
    Track track;
};
