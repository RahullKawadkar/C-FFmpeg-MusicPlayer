#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: my_player.exe <filename>\n");
        return -1;
    }

int64_t accumulated_samples = 0; // Ginti rakhega kitne samples baj gaye

    // 1. FFmpeg Setup
    AVFormatContext* pFormatCtx = NULL;
    av_log_set_level(AV_LOG_ERROR);
    if (avformat_open_input(&pFormatCtx, argv[1], NULL, NULL) != 0) return -1;
    avformat_find_stream_info(pFormatCtx, NULL);

    int audioStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i; break;
        }
    }

    AVCodecParameters* pCodecParams = pFormatCtx->streams[audioStream]->codecpar;
    const AVCodec* pCodec = avcodec_find_decoder(pCodecParams->codec_id);
    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pCodecParams);
    avcodec_open2(pCodecCtx, pCodec, NULL);

    // 2. SDL2 Setup
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = pCodecCtx->sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = pCodecCtx->ch_layout.nb_channels;
    wanted_spec.samples = 1024;
    wanted_spec.callback = NULL;

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, NULL, 0);
    SDL_PauseAudioDevice(device, 0);

    // Window creation for Event Handling and Heartbeat
    SDL_Window* window = SDL_CreateWindow("C-Music Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 200, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // 3. Resampler Setup
    struct SwrContext* swr_ctx = NULL;
    swr_alloc_set_opts2(&swr_ctx, &pCodecCtx->ch_layout, AV_SAMPLE_FMT_S16, pCodecCtx->sample_rate,
                        &pCodecCtx->ch_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0, NULL);
    swr_init(swr_ctx);

    // 4. Playback Variables
    int is_running = 1;
    int is_paused = 0;
    float volume = 1.0f;
    SDL_Event event;
    AVPacket packet;
    AVFrame* pFrame = av_frame_alloc();

    // Max buffer limit: Speaker ke paas kitna audio data "Queue" mein rahega
    const int MAX_AUDIO_BUFFER = pCodecCtx->sample_rate * pCodecCtx->ch_layout.nb_channels * 2 * 2;

    printf("Controls: [SPACE] Pause/Play  [UP/DOWN] Volume  [Q] Quit\n");

    // 5. Smart Playback Loop
    while (is_running) {
        
        // --- STEP A: Event Handling (Keyboard/Window) ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { is_running = 0; goto end_playback; }
            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_SPACE:
                        is_paused = !is_paused;
                        SDL_PauseAudioDevice(device, is_paused);
                        printf(is_paused ? "[PAUSED]\n" : "[RESUMED]\n");
                        break;
                    case SDLK_q: is_running = 0; goto end_playback;
                    case SDLK_UP: 
                        volume = (volume < 1.0f) ? volume + 0.1f : 1.0f; 
                        printf("Volume: %.0f%%\n", volume * 100);
                        break;
                    case SDLK_DOWN: 
                        volume = (volume > 0.0f) ? volume - 0.1f : 0.0f; 
                        printf("Volume: %.0f%%\n", volume * 100);
                        break;
                }
            }
        }

        // --- STEP B: Window Heartbeat (Keep Windows Happy) ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        // --- STEP C: Pause Handle ---
        if (is_paused) {
            SDL_Delay(10);
            continue;
        }

        // --- STEP D: Buffer Management (Anti-Hang) ---
        // Agar speaker ke paas pehle se kafi data hai, toh wait karo
        if (SDL_GetQueuedAudioSize(device) > MAX_AUDIO_BUFFER / 2) {
            SDL_Delay(10); 
            continue;
        }

        // --- STEP E: Decoding ---
        if (av_read_frame(pFormatCtx, &packet) >= 0) {
            if (packet.stream_index == audioStream) {
                if (avcodec_send_packet(pCodecCtx, &packet) == 0) { 

double time_base = av_q2d(pFormatCtx->streams[audioStream]->time_base);
double total_duration = pFormatCtx->duration / (double)AV_TIME_BASE;
double current_time = pFrame->pts *time_base;


                    while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {

                         accumulated_samples += pFrame->nb_samples;

  double current_time = (double)accumulated_samples / (double)pCodecCtx->sample_rate;
    double total_duration = pFormatCtx->duration / (double)AV_TIME_BASE;
                        
                        uint8_t* out_buffer = NULL;
                        int out_samples = av_rescale_rnd(swr_get_delay(swr_ctx, pCodecCtx->sample_rate) + pFrame->nb_samples, 
                                                         pCodecCtx->sample_rate, pCodecCtx->sample_rate, AV_ROUND_UP);
                        
                        av_samples_alloc(&out_buffer, NULL, pCodecCtx->ch_layout.nb_channels, out_samples, AV_SAMPLE_FMT_S16, 0);
                        int converted = swr_convert(swr_ctx, &out_buffer, out_samples, (const uint8_t**)pFrame->data, pFrame->nb_samples);
                        int size = av_samples_get_buffer_size(NULL, pCodecCtx->ch_layout.nb_channels, converted, AV_SAMPLE_FMT_S16, 1);
                        
                        // Software Volume Scaling
                        int16_t* s = (int16_t*)out_buffer;
                        for(int i=0; i < size/2; i++) s[i] = (int16_t)(s[i] * volume);

                        SDL_QueueAudio(device, out_buffer, size);
                        av_freep(&out_buffer);

                        printf("\rPlaying: [%02d:%02d / %02d:%02d] Volume: %.0f%%   ", 
            (int)current_time/60, (int)current_time%60, 
            (int)total_duration/60, (int)total_duration%60,
            volume * 100);
    fflush(stdout);
                        
                    }
                }
            }
            av_packet_unref(&packet);
        } else {
            // End of file: Wait for remaining audio to finish
            if (SDL_GetQueuedAudioSize(device) == 0) is_running = 0;
            SDL_Delay(1);
        }
    }

end_playback:
    printf("Cleaning up and exiting...\n");
    av_frame_free(&pFrame);
    swr_free(&swr_ctx);
    avcodec_free_context(&pCodecCtx);
    avformat_close_input(&pFormatCtx);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(device);
    SDL_Quit();
    return 0;
}
