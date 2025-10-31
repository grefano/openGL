#include "ffmpeg.h"
extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
    #include <libswscale/swscale.h>
}
bool load_frame(const char* filename, unsigned char** out_data, int* out_width, int* out_height){
    AVFormatContext* av_format_context = avformat_alloc_context();
    if (!av_format_context){
        std::cout << "nao criou avformatcontext" << std::endl;
        return 0;
    }
    if (avformat_open_input(&av_format_context, filename, NULL, NULL) != 0){
        std::cout << "nao abriu video" << std::endl;
        return 0;
    }
    const AVCodecParameters* av_codec_params;
    const AVCodec* av_codec;
    int video_stream_index = -1;
    for(int i = 0; i < av_format_context->nb_streams; ++i){
        av_codec_params = av_format_context->streams[i]->codecpar;
        av_codec = avcodec_find_decoder(av_codec_params->codec_id);
        if (!av_codec){
            continue;
        }
        if(av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO){
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1){
        std::cout << "nao achou uma video stream válida " << std::endl;
        return 0;
    }

    AVCodecContext* av_codec_context = avcodec_alloc_context3(av_codec);
    if(!av_codec_context){
        std::cout << "nao criou avcodeccontext" << std::endl;
        return 0;
    }

    if(avcodec_parameters_to_context(av_codec_context, av_codec_params) < 0){
        std::cout << "nao iniciou avcodeccontext" << std::endl;
        return 0;
    }

    if(!avcodec_open2(av_codec_context, av_codec, NULL) < 0){
        std::cout << "nao abriu codec" << std::endl;
        return 0;
    }

    AVFrame* av_frame = av_frame_alloc();
    if(!av_frame){
        std::cout << "nao alocou avframe" << std::endl;
        return 0;
    }
    AVPacket* av_packet = av_packet_alloc();
    if (!av_packet){
        std::cout << "nao alocou avpacket" << std::endl;
        return 0;
    }
    int res;
    while (av_read_frame(av_format_context, av_packet) >= 0){
        if(av_packet->stream_index != video_stream_index){
            continue;
        }    
        res = avcodec_send_packet(av_codec_context, av_packet);
        if (res < 0){
            std::cout << "falhou decode packet " << av_err2str(res) << std::endl;
            return 0;
        }
        res = avcodec_receive_frame(av_codec_context, av_frame);
        if (res == AVERROR(EAGAIN) || res == AVERROR_EOF){
            continue;
        } else if (res < 0){
            std::cout << "falhou decode packet " << av_err2str(res) << std::endl;

        }
        av_packet_unref(av_packet);
        break;
    }
    
    uint8_t* data = new uint8_t[av_frame->width * av_frame->height * 4];
    SwsContext* sws_scaler_context = sws_getContext(av_frame->width, av_frame->height, av_codec_context->pix_fmt, av_frame->width, av_frame->height, AV_PIX_FMT_RGB0, SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_scaler_context){
        std::cout << "não iniciou o sws scaler" << std::endl;
        return 0;
    }
    uint8_t* dest[4] = {data, NULL, NULL, NULL};
    int dest_linesize[4] = {av_frame->width * 4, 0, 0, 0};
    sws_scale(sws_scaler_context, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);
    sws_free_context(&sws_scaler_context);
    
    *out_width = av_frame->width;
    *out_height = av_frame->height;
    *out_data = data;
    
    avformat_close_input(&av_format_context);
    avformat_free_context(av_format_context);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);
    avcodec_free_context(&av_codec_context);

    return 1;
}
