#include "ffmpeg.h"
bool video_reader_open(VideoReaderState* state, const char* filename){
    state->av_format_context = avformat_alloc_context();
    if (!state->av_format_context){
        std::cout << "nao criou avformatcontext" << std::endl;
        return 0;
    }
    if (avformat_open_input(&state->av_format_context, filename, NULL, NULL) != 0){
        std::cout << "nao abriu video" << std::endl;
        return 0;
    }
    const AVCodecParameters* av_codec_params;
    const AVCodec* av_codec;
    for(int i = 0; i < state->av_format_context->nb_streams; ++i){
        av_codec_params = state->av_format_context->streams[i]->codecpar;
        av_codec = avcodec_find_decoder(av_codec_params->codec_id);
        if (!av_codec){
            continue;
        }
        if(av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO){
            state->video_stream_index = i;
            state->time_base = state->av_format_context->streams[i]->time_base;
            break;
        }
    }
    state->av_codec_context = avcodec_alloc_context3(av_codec);
    // AVFormatContext* av_format_context 

    if (state->video_stream_index == -1){
        std::cout << "nao achou uma video stream válida " << std::endl;
        return 0;
    }

    state->av_codec_context = avcodec_alloc_context3(av_codec);
    if(!state->av_codec_context){
        std::cout << "nao criou avcodeccontext" << std::endl;
        return 0;
    }

    if(avcodec_parameters_to_context(state->av_codec_context, av_codec_params) < 0){
        std::cout << "nao iniciou avcodeccontext" << std::endl;
        return 0;
    }

    if(!avcodec_open2(state->av_codec_context, av_codec, NULL) < 0){
        std::cout << "nao abriu codec" << std::endl;
        return 0;
    }

    state->av_frame = av_frame_alloc();
    if(!state->av_frame){
        std::cout << "nao alocou avframe" << std::endl;
        return 0;
    }
    state->av_packet = av_packet_alloc();
    if (!state->av_packet){
        std::cout << "nao alocou avpacket" << std::endl;
        return 0;
    }
    return 1;
}
bool video_reader_read_frame(VideoReaderState* state, uint8_t** frame_buffer, int64_t* pts){
    

    
    int res;
    while (av_read_frame(state->av_format_context, state->av_packet) >= 0){
        if(state->av_packet->stream_index != state->video_stream_index){
            av_packet_unref(state->av_packet);
            continue;
        }    
        res = avcodec_send_packet(state->av_codec_context, state->av_packet);
        if (res < 0){
            std::cout << "falhou decode packet " << av_err2str(res) << std::endl;
            return 0;
        }
        res = avcodec_receive_frame(state->av_codec_context, state->av_frame);
        if (res == AVERROR(EAGAIN) || res == AVERROR_EOF){
            av_packet_unref(state->av_packet);
            continue;
        } else if (res < 0){
            std::cout << "falhou decode packet " << av_err2str(res) << std::endl;

        }
        av_packet_unref(state->av_packet);
        break;
    }

    *pts = state->av_frame->pts;
    
    uint8_t* data = new uint8_t[state->av_frame->width * state->av_frame->height * 4];
    state->sws_scaler_context = sws_getContext(state->av_frame->width, state->av_frame->height, state->av_codec_context->pix_fmt, state->av_frame->width, state->av_frame->height, AV_PIX_FMT_RGB0, SWS_BILINEAR, NULL, NULL, NULL);
    if (!state->sws_scaler_context){
        std::cout << "não iniciou o sws scaler" << std::endl;
        return 0;
    }
    uint8_t* dest[4] = {data, NULL, NULL, NULL};
    int dest_linesize[4] = {state->av_frame->width * 4, 0, 0, 0};
    sws_scale(state->sws_scaler_context, state->av_frame->data, state->av_frame->linesize, 0, state->av_frame->height, dest, dest_linesize);
    
    // *out_width = state->av_frame->width;
    // *out_height = state->av_frame->height;
    *frame_buffer = data;
    data = nullptr;
    
    
    return 1;
}


bool video_reader_seek_frame(VideoReaderState* state, int64_t ts){
    av_seek_frame(state->av_format_context, state->video_stream_index, ts, AVSEEK_FLAG_BACKWARD);


    int res;
    while (av_read_frame(state->av_format_context, state->av_packet) >= 0){
        if(state->av_packet->stream_index != state->video_stream_index){
            av_packet_unref(state->av_packet);
            continue;
        }    
        res = avcodec_send_packet(state->av_codec_context, state->av_packet);
        if (res < 0){
            std::cout << "falhou decode packet " << av_err2str(res) << std::endl;
            return 0;
        }
        res = avcodec_receive_frame(state->av_codec_context, state->av_frame);
        if (res == AVERROR(EAGAIN) || res == AVERROR_EOF){
            av_packet_unref(state->av_packet);
            continue;
        } else if (res < 0){
            std::cout << "falhou decode packet " << av_err2str(res) << std::endl;

        }
        av_packet_unref(state->av_packet);
        break;
    }

    return true;

}

bool video_reader_close(VideoReaderState* state){
    sws_free_context(&state->sws_scaler_context);
    avformat_close_input(&state->av_format_context);
    avformat_free_context(state->av_format_context);
    av_frame_free(&state->av_frame);
    av_packet_free(&state->av_packet);
    avcodec_free_context(&state->av_codec_context);
    return true;
};

