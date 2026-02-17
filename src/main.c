#define SDL_MAIN_HANDLED
#include "file_manager.h"
#include "ui_manager.h"
#include "audio_engine.h"
#include <stdio.h>
#include <conio.h>
#include <windows.h>

extern PlayerState player_state;

int main(int argc, char* argv[]) {
    init_terminal();
    av_log_set_level(AV_LOG_ERROR); 

    init_audio_engine();

    char music_path[MAX_PATH];
    get_default_music_path(music_path);
    scan_directory(music_path);

    int selected_index = 0, is_running = 1, needs_update = 1;
    int last_sec = -1;

    while (is_running) {
        // --- 1. FULL REFRESH (On Key Press or Start) ---
        if (needs_update) {
            draw_dashboard(selected_index, player_state.volume, player_state.is_paused);
            needs_update = 0;
        }

        // --- 2. PARTIAL REFRESH (Every Second for Timer) ---
        int current_sec = (int)player_state.current_time;
        if (current_sec != last_sec && !player_state.is_paused && player_state.current_time > 0) {
            update_timer_only(); // [Partial Refresh](https://en.wikipedia.org)
            last_sec = current_sec;
        }

        // --- 3. INPUT HANDLING ---
        if (_kbhit()) {
            int ch = _getch();
            needs_update = 1; // Key dabi toh full redraw zaroori hai

            if (ch == 0 || ch == 224) {
                int special = _getch();
                switch (special) {
                    case 72: if (selected_index > 0) selected_index--; break;
                    case 80: if (selected_index < song_count - 1) selected_index++; break;
                    case 60: if (player_state.volume > 0.0f) player_state.volume -= 0.05f; break; // F2
                    case 61: if (player_state.volume < 1.0f) player_state.volume += 0.05f; break; // F3
                }
            } else {
                switch (ch) {
                    case 13: play_song(playlist[selected_index].path, playlist[selected_index].name); break;
                    case ' ': player_state.is_paused = !player_state.is_paused; break;
                    case '+': case '=': player_state.volume = (player_state.volume < 1.0f) ? player_state.volume + 0.05f : 1.0f; break;
                    case '-': case '_': player_state.volume = (player_state.volume > 0.0f) ? player_state.volume - 0.05f : 0.0f; break;
                    case 'q': case 'Q': is_running = 0; break;
                }
            }
        }
        SDL_Delay(50); 
    }

    stop_audio();
    cleanup_audio_engine();
    printf("\033[?25h"); // Restore Cursor
    return 0;
}
