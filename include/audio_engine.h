#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>

// Engine states

typedef struct {

int is_running;
int is_paused;
float volume;
char *current_song_path;
char current_song_name[256];
double current_time;
double total_duration;

} PlayerState;


//Functions for main.c to call

int init_audio_engine();
void play_song(const char* path, const char* name);
void stop_audio();
void seek_audio(double seconds);
void cleanup_audio_engine();

#endif