// tutorial01.c
// Code based on a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1
// With updates from https://github.com/chelyaev/ffmpeg-tutorial
// Updates tested on:
// LAVC 54.59.100, LAVF 54.29.104, LSWS 2.1.101
// on GCC 4.7.2 in Debian February 2015

// A small sample program that shows how to use libavformat and libavcodec to
// read video from a file.
//
// Use
//
// gcc -o tutorial01 tutorial01.c -lavformat -lavcodec -lswscale -lz
//
// to build (assuming libavformat and libavcodec are correctly installed
// your system).
//
// Run using
//
// tutorial01 myvideofile.mpg
//
// to write the first five frames from "myvideofile.mpg" to disk in PPM
// format.

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#include <stdio.h>

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

char *exeDir = NULL;

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
    FILE *pFile;
    int  y;
    char szFilename[32]={0};
    // Open file
    sprintf(szFilename, "frame%d.ppm", iFrame);
    const int size = strlen(exeDir) + strlen(szFilename) + 2;
    char *filePath = malloc(size);
    bzero(filePath,size);
    memcpy(filePath,exeDir,strlen(exeDir));
    memset(filePath + strlen(exeDir),'/',1);
    memcpy(filePath + strlen(exeDir) + 1,szFilename,strlen(szFilename));
    printf("%s\n",filePath);
    
    pFile=fopen(filePath, "wb");
    if(pFile==NULL)
        return;
    
    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);
    
    // Write pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
    
    // Close file
    fclose(pFile);
}

int main01(int argc,const char *argv[]) {
    // printf("%s",argv[0]);
    const char* path = argv[0];
    char *end = strrchr(path,'/');
    if (end != NULL) {
        int length = end - path;
        exeDir = malloc(length+1);
        bzero(exeDir,length+1);
        memcpy(exeDir,path,length);
    }
    
    // printf("%s",exeDir);
    if (exeDir == NULL) {
        return -1;
    }
    
    // Initalizing these to NULL prevents segfaults!
    AVFormatContext   *pFormatCtx = NULL;
    int               i, videoStream;
    AVCodecContext    *pCodecCtx = NULL;
    AVFrame           *pFrame = NULL;
    AVFrame           *pFrameRGB = NULL;
    AVPacket          packet;
    int               numBytes;
    uint8_t           *buffer = NULL;
    struct SwsContext *sws_ctx = NULL;
    
    if(argc < 2) {
        printf("Please provide a movie file\n");
        return -1;
    }
    // Register all formats and codecs
    av_register_all();
    // Open video file
    if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)!=0)
        return -1; // Couldn't open file
    
    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        return -1; // Couldn't find stream information
    
    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, argv[1], 0);
    
    // Find the first video stream
    videoStream=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
            videoStream=i;
            break;
        }
    if(videoStream==-1)
        return -1; // Didn't find a video stream
    
    pCodecCtx = avcodec_alloc_context3(NULL);
    if (!pCodecCtx) {
        return -1;
    }
    
    if (avcodec_parameters_to_context(pCodecCtx,pFormatCtx->streams[videoStream]->codecpar) < 0) {
        avcodec_free_context(&pCodecCtx);
        return -1;
    }
    
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    
    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0){
        avcodec_free_context(&pCodecCtx);
        return -1; // Could not open codec
    }
    
    // Allocate video frame
    pFrame=av_frame_alloc();
    
    // Allocate an AVFrame structure
    pFrameRGB=av_frame_alloc();
    if(pFrameRGB==NULL)
        return -1;
    
    // Determine required buffer size and allocate buffer
    numBytes=av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
                                      pCodecCtx->height,1);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    
    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24,
                         pCodecCtx->width, pCodecCtx->height,1);
    
    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(pCodecCtx->width,
                             pCodecCtx->height,
                             pCodecCtx->pix_fmt,
                             pCodecCtx->width,
                             pCodecCtx->height,
                             AV_PIX_FMT_RGB24,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL
                             );
    
    // Read frames and save first five frames to disk
    i=0;
    while(av_read_frame(pFormatCtx, &packet)>=0) {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) {
            // Decode video frame
            int ret = AVERROR(EAGAIN);
            do
            {
                ret = avcodec_receive_frame(pCodecCtx,pFrame);
                
                //Did we get a video frame?
                if (ret >= 0) {
                    // Convert the image from its native format to RGB
                    sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                              pFrame->linesize, 0, pCodecCtx->height,
                              pFrameRGB->data, pFrameRGB->linesize);
                    
                    // Save the frame to disk
                    if(++i<=5){
                        SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
                    }
                    
                    break;
                } else if (ret == AVERROR_EOF){
                    avcodec_flush_buffers(pCodecCtx);
                    break;
                }
            } while (ret != AVERROR(EAGAIN));
            
            if (i >= 5) {
                break;
            }
            
            avcodec_send_packet(pCodecCtx,&packet);
        }
        // Free the packet that was allocated by av_read_frame
        av_packet_unref(&packet);
    }
    
    // Free the RGB image
    av_free(buffer);
    av_frame_free(&pFrameRGB);
    
    // Free the YUV frame
    av_frame_free(&pFrame);
    
    // Close the codecs
    avcodec_free_context(&pCodecCtx);
    
    // Close the video file
    avformat_close_input(&pFormatCtx);
    
    return 0;
}