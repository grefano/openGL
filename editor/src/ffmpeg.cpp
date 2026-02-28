#include "ffmpeg.h"


VideoReader::VideoReader(const char* filename){
    this->file_open(filename);
}
VideoReader::~VideoReader(){
    VideoReaderState* state = &this->state;
    sws_free_context(&state->sws_scaler_context);
    avformat_close_input(&state->av_format_context);
    avformat_free_context(state->av_format_context);
    av_frame_free(&state->av_frame);
    av_packet_free(&state->av_packet);
    avcodec_free_context(&state->av_codec_context);
}

bool VideoReader::jump_to_ts(float ts_sec, double* pt_seconds, int64_t* pts){
    // glfwSetTime(ts_sec);
    *pt_seconds = ts_sec;
    *pts = (int64_t)(*pt_seconds  * (double)this->state.time_base.den / (double)this->state.time_base.num);
    this->seek_frame(*pts);

    return true;
}

bool VideoReader::file_open(const char* filename){
    this->state.av_format_context = avformat_alloc_context();
    if (!this->state.av_format_context){
        std::cout << "nao criou avformatcontext" << std::endl;
        return 0;
    }
    if (avformat_open_input(&this->state.av_format_context, filename, NULL, NULL) != 0){
        std::cout << "nao abriu video" << std::endl;
        return 0;
    }
    const AVCodecParameters* av_codec_params;
    const AVCodec* av_codec;
    for(int i = 0; i < this->state.av_format_context->nb_streams; ++i){
        av_codec_params = this->state.av_format_context->streams[i]->codecpar;
        av_codec = avcodec_find_decoder(av_codec_params->codec_id);
        if (!av_codec){
            continue;
        }
        if(av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO){
            this->state.video_stream_index = i;
            this->state.time_base = this->state.av_format_context->streams[i]->time_base;
            break;
        }
    }
    this->state.av_codec_context = avcodec_alloc_context3(av_codec);
    // AVFormatContext* av_format_context 

    if (this->state.video_stream_index == -1){
        std::cout << "nao achou uma video stream válida " << std::endl;
        return 0;
    }

    this->state.av_codec_context = avcodec_alloc_context3(av_codec);
    if(!this->state.av_codec_context){
        std::cout << "nao criou avcodeccontext" << std::endl;
        return 0;
    }

    if(avcodec_parameters_to_context(this->state.av_codec_context, av_codec_params) < 0){
        std::cout << "nao iniciou avcodeccontext" << std::endl;
        return 0;
    }

    if(!avcodec_open2(this->state.av_codec_context, av_codec, NULL) < 0){
        std::cout << "nao abriu codec" << std::endl;
        return 0;
    }

    this->state.av_frame = av_frame_alloc();
    if(!this->state.av_frame){
        std::cout << "nao alocou avframe" << std::endl;
        return 0;
    }
    this->state.av_packet = av_packet_alloc();
    if (!this->state.av_packet){
        std::cout << "nao alocou avpacket" << std::endl;
        return 0;
    }
    return 1;
}
bool VideoReader::read_frame(uint8_t** frame_buffer, int64_t* pts){
    

    
    int res;
    while (av_read_frame(this->state.av_format_context, this->state.av_packet) >= 0){
        if(this->state.av_packet->stream_index != this->state.video_stream_index){
            av_packet_unref(this->state.av_packet);
            continue;
        }    
        res = avcodec_send_packet(this->state.av_codec_context, this->state.av_packet);
        if (res < 0){
            std::cout << "falhou decode packet " << av_err2str(res) << std::endl;
            return 0;
        }
        res = avcodec_receive_frame(this->state.av_codec_context, this->state.av_frame);
        if (res == AVERROR(EAGAIN) || res == AVERROR_EOF){
            av_packet_unref(this->state.av_packet);
            continue;
        } else if (res < 0){
            std::cout << "falhou decode packet " << av_err2str(res) << std::endl;

        }
        av_packet_unref(this->state.av_packet);
        break;
    }

    *pts = this->state.av_frame->pts;
    printf("read pts %d", *pts);
    
    uint8_t* data = new uint8_t[this->state.av_frame->width * this->state.av_frame->height * 4];
    this->state.sws_scaler_context = sws_getContext(this->state.av_frame->width, this->state.av_frame->height, this->state.av_codec_context->pix_fmt, this->state.av_frame->width, this->state.av_frame->height, AV_PIX_FMT_RGB0, SWS_BILINEAR, NULL, NULL, NULL);
    if (!this->state.sws_scaler_context){
        std::cout << "não iniciou o sws scaler" << std::endl;
        return 0;
    }
    uint8_t* dest[4] = {data, NULL, NULL, NULL};
    int dest_linesize[4] = {this->state.av_frame->width * 4, 0, 0, 0};
    sws_scale(this->state.sws_scaler_context, this->state.av_frame->data, this->state.av_frame->linesize, 0, this->state.av_frame->height, dest, dest_linesize);
    
    // *out_width = this->state.av_frame->width;
    // *out_height = this->state.av_frame->height;
    *frame_buffer = data;
    data = nullptr;
    
    
    return 1;
}

double VideoReader::get_time_base(){
    return (double)this->state.time_base.num / (double)this->state.time_base.den;
}

bool VideoReader::seek_frame(int64_t ts){
    printf("--seek frame\n");
    // if (!this->state.av_format_context || !this->state.av_codec_context){
    //     printf("format or stream are null\n");
    //     return false;
    // }
    // av_seek_frame(this->state.av_format_context, this->state.video_stream_index, ts, AVSEEK_FLAG_ANY);
    // avcodec_flush_buffers(this->state.av_codec_context); 

    printf("flushed\n");

    int res;
    while (av_read_frame(this->state.av_format_context, this->state.av_packet) >= 0){
        printf("while start\n");

        if(this->state.av_packet->stream_index != this->state.video_stream_index){
            av_packet_unref(this->state.av_packet);
            continue;
        }    
        res = avcodec_send_packet(this->state.av_codec_context, this->state.av_packet);
        if (res < 0){
            std::cout << "falhou decode packet " << av_err2str(res) << std::endl;
            return 0;
        }
        res = avcodec_receive_frame(this->state.av_codec_context, this->state.av_frame);
        if (res == AVERROR(EAGAIN) || res == AVERROR_EOF){
            av_packet_unref(this->state.av_packet);
            continue;
        } else if (res < 0){
            std::cout << "falhou decode packet " << av_err2str(res) << std::endl;

        }
        av_packet_unref(this->state.av_packet);
        break;
    }

    return true;

}

