#include "audio_engine.h"
#include "ui_manager.h"
#include <stdio.h>

// Global variables
SDL_AudioDeviceID audio_device = 0;
volatile double seek_target = -1.0; 
PlayerState player_state = {0, 0, 0.3f, ""}; // Initialize properly
SDL_Thread *audio_thread = NULL;

void log_engine_error(const char* stage, const char* msg) {
    // Error logging (same as before)
}

// --- Background Worker Thread ---
int audio_thread_func(void *data) {
    char *path = (char*)data; // const hataya warning ke liye
    
    AVFormatContext *pFormatCtx = NULL;
    AVPacket packet;
    AVFrame *pFrame = NULL;
    struct SwrContext *swr_ctx = NULL;
    AVCodecContext *pCodecCtx = NULL;

    // 1. Open File
    if (avformat_open_input(&pFormatCtx, path, NULL, NULL) != 0) {
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) goto cleanup;

    // 2. Find Audio Stream
    int audioStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = i; break;
        }
    }
    if (audioStream == -1) goto cleanup;

    // 3. Setup Decoder
    AVCodecParameters *pCodecParams = pFormatCtx->streams[audioStream]->codecpar;
    const AVCodec *pCodec = avcodec_find_decoder(pCodecParams->codec_id);
    AVCodecContext *pCodecCtx_local = avcodec_alloc_context3(pCodec); // Local var use karo crash se bachne ke liye
    avcodec_parameters_to_context(pCodecCtx_local, pCodecParams);
    if (avcodec_open2(pCodecCtx_local, pCodec, NULL) < 0) {
        avcodec_free_context(&pCodecCtx_local);
        goto cleanup;
    }
    pCodecCtx = pCodecCtx_local; // Assign back

    // 4. SDL Audio Setup
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = pCodecCtx->sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = pCodecCtx->ch_layout.nb_channels;
    wanted_spec.samples = 1024;
    wanted_spec.callback = NULL;

    audio_device = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, NULL, 0);
    if (audio_device == 0) goto cleanup;
    
    SDL_PauseAudioDevice(audio_device, 0);

    // 5. Resampler Setup
    swr_alloc_set_opts2(&swr_ctx, &pCodecCtx->ch_layout, AV_SAMPLE_FMT_S16, pCodecCtx->sample_rate,
                        &pCodecCtx->ch_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0, NULL);
    swr_init(swr_ctx);

    pFrame = av_frame_alloc();
    double time_base = av_q2d(pFormatCtx->streams[audioStream]->time_base);
    player_state.total_duration = pFormatCtx->duration / (double)AV_TIME_BASE;

    // --- MAIN DECODING LOOP ---
    while (player_state.is_running) {
        
        // Seek Logic
        if (seek_target >= 0) {
            int64_t target_timestamp = (int64_t)(seek_target * AV_TIME_BASE);
            if (av_seek_frame(pFormatCtx, -1, target_timestamp, AVSEEK_FLAG_BACKWARD) >= 0) {
                avcodec_flush_buffers(pCodecCtx); 
                SDL_ClearQueuedAudio(audio_device);
                player_state.current_time = seek_target;
            }
            seek_target = -1.0;
        }

        // Read Frame
        if (av_read_frame(pFormatCtx, &packet) < 0) {
            // EOF (End of File) Reached naturally
            if (player_state.is_running) {
                player_state.playback_finished = 1; // 🟢 Sirf tab 1 karo jab gaana khatam ho
            }
            break; 
        }

        if (packet.stream_index == audioStream) {
            player_state.current_time = packet.pts * time_base;

            if (avcodec_send_packet(pCodecCtx, &packet) == 0) {
                while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
                    
                    // Pause Handling
                    while (player_state.is_paused && player_state.is_running) {
                        SDL_Delay(10);
                    }

                    // Buffer Management
                    while (SDL_GetQueuedAudioSize(audio_device) > (Uint32)(pCodecCtx->sample_rate * 4)) {
                         if (!player_state.is_running) break;
                         SDL_Delay(10);
                    }
                    
                    if (!player_state.is_running) break;

                    // Resampling & Playing
                    uint8_t *out_buffer = NULL;
                    int out_samples = av_rescale_rnd(swr_get_delay(swr_ctx, pCodecCtx->sample_rate) + pFrame->nb_samples, 
                                                     pCodecCtx->sample_rate, pCodecCtx->sample_rate, AV_ROUND_UP);
                    
                    av_samples_alloc(&out_buffer, NULL, pCodecCtx->ch_layout.nb_channels, out_samples, AV_SAMPLE_FMT_S16, 0);
                    swr_convert(swr_ctx, &out_buffer, out_samples, (const uint8_t**)pFrame->data, pFrame->nb_samples);
                    
                    int out_size = av_samples_get_buffer_size(NULL, pCodecCtx->ch_layout.nb_channels, out_samples, AV_SAMPLE_FMT_S16, 1);
                    
                    // Volume Control
                    int16_t *s = (int16_t*)out_buffer;
                    for (int i = 0; i < out_size / 2; i++) {
                        s[i] = (int16_t)(s[i] * player_state.volume);
                    }

                    SDL_QueueAudio(audio_device, out_buffer, out_size);
                    av_freep(&out_buffer);
                }
            }
        }
        av_packet_unref(&packet);
    }

    
cleanup:
    if (pFrame) av_frame_free(&pFrame);
    if (swr_ctx) swr_free(&swr_ctx);
    if (pCodecCtx) avcodec_free_context(&pCodecCtx);
    if (pFormatCtx) avformat_close_input(&pFormatCtx);
    if (audio_device) { SDL_CloseAudioDevice(audio_device); audio_device = 0; }
    
    // Free path memory allocated in play_song
    free(path); 
    return 0;
}

void play_song(const char* path, const char* name) {
    player_state.playback_finished = 0; 
    stop_audio(); // Purana thread band karo
    
    player_state.is_running = 1;
    player_state.is_paused = 0;
    player_state.current_time = 0;
    seek_target = -1.0;
    
    if (name) strncpy(player_state.current_song_name, name, 256);
    
    // Path ko duplicate karo kyunki thread async chalta hai

    char *path_copy = strdup(path); 
    
    audio_thread = SDL_CreateThread(audio_thread_func, "AudioThread", (void*)path_copy);
}

void stop_audio() {
    player_state.is_running = 0;
    seek_target = -1.0; 
    if (audio_thread) {
        SDL_WaitThread(audio_thread, NULL);
        audio_thread = NULL;
    }
}

void seek_audio(double seconds) {
    // Same as before
    double new_time = player_state.current_time + seconds;
    if (new_time < 0) new_time = 0;
    if (new_time > player_state.total_duration) new_time = player_state.total_duration;
    seek_target = new_time;
}

int init_audio_engine() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) return -1;
    return 0;
}

void cleanup_audio_engine() {
    stop_audio();
    SDL_Quit();
}