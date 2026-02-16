#include "file_manager.h"
#include "ui_manager.h" // Yeh zaroori hai
#include <stdio.h>
#include <conio.h>

int main() {
    init_terminal(); // Yeh ui_manager.c se apne aap utha lega
    
    char music_path[MAX_PATH];
    get_default_music_path(music_path);
    scan_directory(music_path);

    int selected_index = 0;
    int is_running = 1;
    float current_vol = 0.3f;

    while (is_running) {

        draw_dashboard(selected_index, current_vol, 0);

        int ch = _getch();

        if (ch == 0 || ch == 224) {
            switch (_getch()) {
                case 72: if (selected_index > 0) selected_index--; break;
                case 80: if (selected_index < song_count - 1) selected_index++; break;
            }
        } else {
            if (ch == 'q' || ch == 'Q') is_running = 0;
            // Enter logic baad mein
        }

        else {
 
        switch(_getch()); {
        
            case 13:play_song(playlist[selected_index].path);

            stop_audio();

            play_song(playlist[selected_index].path);

            printf("\n" GREEN ">>> Playing: %s" RESET "\n", playlist[selected_index].name);
            break;

           case ' ': 

            state.is_paused = !state.is_paused;
          SDL_PausedAudioDevice(audio_device, state.is_paused);
          break;

         }
    }

    printf("\033[?25h"); // Cursor wapas dikhao exit pe
    return 0;
}
