#pragma once

#include <iostream>
extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
    #include <libswscale/swscale.h>
}

struct VideoReaderState{
    int width, height;
    AVRational time_base;
    uint8_t* frame_buffer;
    AVFormatContext* av_format_context;
    AVCodecContext* av_codec_context;
    AVFrame* av_frame;
    AVPacket* av_packet;
    SwsContext* sws_scaler_context;
    int video_stream_index;

};


bool video_reader_open(VideoReaderState* state, const char* filename);
bool video_reader_read_frame(VideoReaderState* state, uint8_t** frame_buffer, int64_t* pts);
bool video_reader_close(VideoReaderState* state);
