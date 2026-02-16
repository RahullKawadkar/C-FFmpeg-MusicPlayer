#include "audio_engine.h"
#include <stdio.h>

//Global objects for audio

SDL_AudioDeviceID audio_device;

PlayerState state = {1, 0, 0.3f, NULL};

int init_audio_engine() {

 if(SDL_Init(SDL_INIT_AUDIO) < 0) return -1;
 avformat_network_init();
 return 0;

}

void play_song(const char* path) {



}