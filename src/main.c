#define SDL_MAIN_HANDLED
#include "file_manager.h"
#include "ui_manager.h"
#include "audio_engine.h"
#include <stdio.h>
#include <conio.h>
#include <string.h>

extern PlayerState player_state;

// Helper to keep cursor consistent
void navigate_to(int new_index, int* selected_index, int* start_idx, int window_size) {
    if (entry_count <= 0) return;
    *selected_index = (new_index + entry_count) % entry_count;

    // Folder skipping logic
    int attempts = 0;
    while (browser_list[*selected_index].is_directory && attempts < entry_count) {
        *selected_index = (*selected_index + 1) % entry_count;
        attempts++;
    }

    // Viewport Sync
    if (*selected_index >= *start_idx + window_size) {
        *start_idx = *selected_index - window_size + 1;
    }
    if (*selected_index < *start_idx) {
        *start_idx = *selected_index;
    }
}

int main(int argc, char* argv[]) {
    init_terminal();
    av_log_set_level(AV_LOG_ERROR); 
    if (init_audio_engine() < 0) return -1;

    int is_running = 1;
    int current_state = 0; 
    int selected_index = 0;
    int start_idx = 0;
    int window_size = 8;
    int folder_sel = 0;
    int last_sec = -1;
    int needs_update = 1;
    
    // 🟢 Fix: Variables declared
    unsigned int last_skip_time = 0;
    unsigned int current_time = 0;

    deep_discover_music();

    while (is_running) {
        // 🟢 Fix: Update Time
        current_time = SDL_GetTicks();
        int current_sec = (int)player_state.current_time;

        // --- 🟢 SMART UI REFRESH ---
        if (needs_update) {
            if (current_state == 0) {
                clear_screen();
                printf(COLOR_CYAN BOLD "==========================================================\033[K\n");
                printf("         [ SELECT YOUR MUSIC SOURCE / FOLDER ]            \033[K\n");
                printf("==========================================================\033[K\n" COLOR_RESET);
                for (int i = 0; i < found_folder_count; i++) {
                    if (i == folder_sel) printf(BG_BLUE BOLD " > %-52s " COLOR_RESET "\033[K\n", found_music_folders[i]);
                    else printf("   %-52s \033[K\n", found_music_folders[i]);
                }
            } else {
                draw_dashboard(selected_index, player_state.volume, player_state.is_paused, start_idx);
                last_sec = current_sec;
            }
            needs_update = 0;
        }

        // Timer Update (Partial Refresh)
        if (current_state == 1 && current_sec != last_sec && !player_state.is_paused && player_state.current_time > 0) {
            update_timer_only();
            last_sec = current_sec;
        }

        // --- AUTO-PLAY NEXT LOGIC (FIXED) ---
        // 'current_time - last_skip_time > 1000' ye check karta hai ki 1 sec mein do baar skip na ho
        if (current_state == 1 && player_state.playback_finished && (current_time - last_skip_time > 1000)) {
            
            player_state.playback_finished = 0; // Flag reset
            last_skip_time = current_time;      // Timer set

            if (entry_count > 0) {
                navigate_to(selected_index + 1, &selected_index, &start_idx, window_size);
                
              //  system("cls"); // Viewport clean
                play_song(browser_list[selected_index].path, browser_list[selected_index].name);
                needs_update = 1;
            }
        }

        // --- ⌨️ INPUT HANDLING ---
        if (_kbhit()) {
            int ch = _getch();
            needs_update = 1;

            if (ch == 0 || ch == 224) { 
                int spec = _getch();
                if (current_state == 0) {
                    if (spec == 72 && folder_sel > 0) folder_sel--;
                    if (spec == 80 && folder_sel < found_folder_count - 1) folder_sel++;
                } else {
                    if (spec == 72) navigate_to(selected_index - 1, &selected_index, &start_idx, window_size);
                    if (spec == 80) navigate_to(selected_index + 1, &selected_index, &start_idx, window_size);
                    if (spec == 60) player_state.volume = (player_state.volume > 0.05f) ? player_state.volume - 0.05f : 0.0f;
                    if (spec == 61) player_state.volume = (player_state.volume < 0.95f) ? player_state.volume + 0.05f : 1.0f;
                }
            } else {
                switch (ch) {
                    case 13: // ENTER
                        if (current_state == 0) {
                            scan_directory(found_music_folders[folder_sel]);
                            if (entry_count > 0) {
                                system("cls");
                                current_state = 1;
                                selected_index = 0; 
                                start_idx = 0;
                            }
                        } else {
                            player_state.playback_finished = 0;
                            play_song(browser_list[selected_index].path, browser_list[selected_index].name);
                        }
                        break;
                    
                    case ' ': player_state.is_paused = !player_state.is_paused; break;
                    
                    case 'b': case 'B': 
                        system("cls");
                        stop_audio(); // Folder menu par jane se pehle audio band karo
                        current_state = 0; selected_index = 0; start_idx = 0;
                        break;

                    case 'l': case 'L': seek_audio(10.0); break;
                    case 'j': case 'J': seek_audio(-10.0); break;

                    case 'n': case 'N': // NEXT (Manual)
                        // Manual next dabane par hum direct 'playback_finished = 1' karte hain.
                        // Upake auto-play logic ye signal pakad lega aur gaana change karega.
                        // Timer check taaki spamming na ho.
                        if (current_time - last_skip_time > 500) {
                            player_state.playback_finished = 1;
                            last_skip_time = current_time; 
                        }
                        break;

                    case 'p': case 'P': // PREV
                         if (current_time - last_skip_time > 500) {
                            navigate_to(selected_index - 1, &selected_index, &start_idx, window_size);
                            play_song(browser_list[selected_index].path, browser_list[selected_index].name);
                            last_skip_time = current_time;
                         }
                        break;

                    case '+': case '=': player_state.volume = (player_state.volume < 0.95f) ? player_state.volume + 0.05f : 1.0f; break;
                    case '-': case '_': player_state.volume = (player_state.volume > 0.05f) ? player_state.volume - 0.05f : 0.0f; break;
                    case 'q': case 'Q': is_running = 0; break;
                }
            }
        }
        SDL_Delay(30);
    }

    stop_audio();
    cleanup_audio_engine();
    show_outro_animation();
    return 0;
}