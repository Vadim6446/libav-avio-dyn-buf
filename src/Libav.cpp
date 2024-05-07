#include "Libav.h"
#include <iostream>

Libav::Libav(std::string path){
    filePath = strdup(path.c_str());
    chunkDuration = 6;

    inputCtx = NULL;
    outputCtx = NULL;
}

int Libav::openInput(){
    int ret = avformat_open_input(&inputCtx, filePath, 0, 0);
    if(ret < 0){
        fprintf(stderr, "Could not open input file '%s', error value: %d (%s)\n", filePath, ret, av_err2str(ret));
        return ret;
    }

    ret = avformat_find_stream_info(inputCtx, 0);

    if(ret < 0){
        fprintf(stderr, "Failed to retrieve input stream information");
        return ret;
    }

    av_dump_format(inputCtx, 0, filePath, 0);

    return ret;
}

int Libav::openOutput(){
    int ret = avformat_alloc_output_context2(&outputCtx, av_guess_format("mpegts", NULL, NULL), NULL, NULL);

    if(ret < 0){
        fprintf(stderr, "Could not create output context\n");
        return ret;
    }

    int streamMapSize = inputCtx->nb_streams;

    for(int i = 0; i < streamMapSize; i++){
        AVStream *outStream;
        AVStream *inStream = inputCtx->streams[i];
        AVCodecParameters *inCodecPar = inStream->codecpar;

        if(inCodecPar->codec_type != AVMEDIA_TYPE_AUDIO && inCodecPar->codec_type != AVMEDIA_TYPE_VIDEO && inCodecPar->codec_type != AVMEDIA_TYPE_SUBTITLE){
            continue;
        }

        outStream = avformat_new_stream(outputCtx, NULL);
        if(!outStream){
            fprintf(stderr, "Failed allocating output stream\n");
            return AVERROR_UNKNOWN;
        }

        ret = avcodec_parameters_copy(outStream->codecpar, inCodecPar);
        if(ret < 0){
            fprintf(stderr, "Failed to copy codec parameters\n");
            return ret;
        }
        outStream->codecpar->codec_tag = 0;
    }

    //avio dyn buf
    ret = avio_open_dyn_buf(&outputCtx->pb);

    av_dump_format(outputCtx, 0, NULL, 1);

    outputCtx->flags |= AVFMT_FLAG_FLUSH_PACKETS;

    AVDictionary *options = NULL;

    av_dict_set(&options, "mpegts_flags", "resend_headers+initial_discontinuity", NULL);

    ret = avformat_write_header(outputCtx, &options);

    av_dict_free(&options);

    if(ret < 0){
        fprintf(stderr, "Error occurred when opening output file\n");
        return ret;
    }
}

void Libav::logPacket(AVFormatContext *fmt_ctx, AVPacket *pkt, char *tag)
{
    AVRational time_base = fmt_ctx->streams[pkt->stream_index]->time_base;
    
    std::cout << std::setw(3) << tag << " : " <<std::setw(5)<< " pts: " 
        << std::setw(10) << std::to_string(pkt->pts) 
        << std::setw(13) << " pts_time: " << std::setw(10) <<std::setw(10)
        << av_q2d(time_base) * pkt->pts << std::setw(13) << " dts_time: " << std::setw(8) 
        << av_q2d(time_base) * pkt->dts
        << std::setw(13)<< " duration: " << std::setw(5) << pkt->duration 
        << std::setw(18) << " duration_time: " 
        << std::setw(5) << av_q2d(time_base) * pkt->duration 
        << std::setw(8) << " stream_index: " << pkt->stream_index << std::endl;
}

void Libav::remux(){
    AVPacket *pkt = NULL;
    pkt = av_packet_alloc();

    if(!pkt){
        fprintf(stderr, "Could not allocate AVPacket\n");
        return ;
    }

    int ret = 0;
    while(1){
        AVStream *inStream, *outStream;

        ret = av_read_frame(inputCtx, pkt);
        if(ret < 0){
            fprintf(stderr, "Error read packet!");
            break;
        }

        inStream = inputCtx->streams[pkt->stream_index];
        outStream = outputCtx->streams[pkt->stream_index];
        logPacket(inputCtx, pkt, "in");

        av_packet_rescale_ts(pkt, inStream->time_base, outStream->time_base);
        pkt->pos = -1;
        logPacket(outputCtx, pkt, "out");

        AVRational time_base = inputCtx->streams[pkt->stream_index]->time_base;
        double pktDur = av_q2d(time_base) * pkt->duration;

        if(pkt->flags == AV_PKT_FLAG_KEY){
            if(currentDurationChunk > chunkDuration){
                if(chunkP.is_open()){
                    chunkP.close();
                }
            }
        }

        ret = av_interleaved_write_frame(outputCtx, pkt);

        if(ret < 0){
            fprintf(stderr, "Error muxing packet\n");
            break;
        }

        uint8_t *dyn_buf;
        int size = avio_get_dyn_buf(outputCtx->pb, &dyn_buf);

        if(!chunkP.is_open()){
            std::string fileName = unixTimeToChunkName( time(NULL) ) + ".ts";
            chunkP.open(fileName, std::ios::out | std::ios::binary);
            currentDurationChunk = 0; 
        }

        if(size > 0){
            chunkP.write((char*)dyn_buf, size);

            currentDurationChunk += pktDur;
            std::cout<<"currentDurationChunk: "<<currentDurationChunk<<std::endl;
        }

        avio_flush(outputCtx->pb);
    }
    av_packet_free(&pkt);
}

Libav::~Libav(){
    free ((void*) filePath);
    if(inputCtx != NULL){
        avformat_close_input(&inputCtx);
        avformat_free_context(inputCtx);
        inputCtx = NULL;
    }

    if(outputCtx != NULL){
        uint8_t **dst;
        avio_close_dyn_buf(outputCtx->pb, dst);
        av_freep(dst);
    
        avformat_free_context(outputCtx);
    }
    
}