#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    // 1. Check input arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <music_file.mp3>\n", argv[0]);
        return -1;
    }

    const char* filename = argv[1];

    // 2. Open File and Get Stream Info
    AVFormatContext* pFormatCtx = NULL;
    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return -1;
    }
    avformat_find_stream_info(pFormatCtx, NULL);

    // 3. Find Audio Stream
    int audioStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i;
            break;
        }
    }
    if (audioStream == -1) {
        fprintf(stderr, "No audio stream found.\n");
        return -1;
    }

    // 4. Setup Decoder
    AVCodecParameters* pCodecParams = pFormatCtx->streams[audioStream]->codecpar;
    const AVCodec* pCodec = avcodec_find_decoder(pCodecParams->codec_id);
    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pCodecParams);
    avcodec_open2(pCodecCtx, pCodec, NULL);

    // 5. Initialize SDL2
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL Init Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = pCodecCtx->sample_rate;
    wanted_spec.format = AUDIO_S16SYS; // Signed 16-bit
    wanted_spec.channels = pCodecCtx->ch_layout.nb_channels;
    wanted_spec.samples = 1024;
    wanted_spec.callback = NULL;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, NULL, 0);
    SDL_PauseAudioDevice(device, 0); // Start playing

    // 6. Setup Resampler (Float to S16 conversion)
    struct SwrContext* swr_ctx = NULL;
    swr_alloc_set_opts2(&swr_ctx,
        &pCodecCtx->ch_layout, AV_SAMPLE_FMT_S16, pCodecCtx->sample_rate,
        &pCodecCtx->ch_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate,
        0, NULL);
    swr_init(swr_ctx);

    // 7. Playback Loop
    AVPacket packet;
    AVFrame* pFrame = av_frame_alloc();

    printf("Playing: %s\n", filename);

    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index == audioStream) {
            if (avcodec_send_packet(pCodecCtx, &packet) == 0) {
                while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
                    
                    // Buffer for converted data
                    uint8_t* out_buffer = NULL;
                    int out_samples = av_rescale_rnd(swr_get_delay(swr_ctx, pCodecCtx->sample_rate) + pFrame->nb_samples,
                                                     pCodecCtx->sample_rate, pCodecCtx->sample_rate, AV_ROUND_UP);
                    
                    av_samples_alloc(&out_buffer, NULL, pCodecCtx->ch_layout.nb_channels, 
                                     out_samples, AV_SAMPLE_FMT_S16, 0);

                    int converted_samples = swr_convert(swr_ctx, &out_buffer, out_samples,
                                                        (const uint8_t**)pFrame->data, pFrame->nb_samples);

                    int out_size = av_samples_get_buffer_size(NULL, pCodecCtx->ch_layout.nb_channels,
                                                              converted_samples, AV_SAMPLE_FMT_S16, 1);

                    // Send to Speakers
                    SDL_QueueAudio(device, out_buffer, out_size);
                    av_freep(&out_buffer);
                }
            }
        }
        av_packet_unref(&packet);
        SDL_Delay(1); // Small delay to prevent CPU spike
    }
// Jab tak speaker saara data baja na le, tab tak wait karo
while (SDL_GetQueuedAudioSize(device) > 0) {
    SDL_Delay(100); 
}


    // 8. Cleanup (Professional Memory Management)
    printf("Finished.\n");
    av_frame_free(&pFrame);
    swr_free(&swr_ctx);
    avcodec_free_context(&pCodecCtx);
    avformat_close_input(&pFormatCtx);
    SDL_CloseAudioDevice(device);
    SDL_Quit();

    return 0;
}