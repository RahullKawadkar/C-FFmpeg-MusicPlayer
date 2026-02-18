#define SDL_MAIN_HANDLED
#include "file_manager.h"
#include "ui_manager.h"
#include "audio_engine.h"
#include <stdio.h>
#include <conio.h>
#include <string.h>

extern PlayerState player_state;

int main(int argc, char* argv[]) {
    // 1. Initializations
    init_terminal();
    av_log_set_level(AV_LOG_ERROR); // FFmpeg kachra warnings block karo [FFmpeg Log](https://ffmpeg.org)
    
    if (init_audio_engine() < 0) {
        printf("SDL Init Fail!\n");
        return -1;
    }

    int is_running = 1;
    int current_state = 0; // 0: Folder Selection, 1: Player UI
    int selected_index = 0;
    int folder_sel = 0;
    int last_sec = -1;
    int needs_update = 1; // Flag to prevent constant redraw (Flicker Fix)

    // 2. Initial Discovery
    deep_discover_music();

    while (is_running) {
        int current_sec = (int)player_state.current_time;

        // ---  SMART UI REFRESH (The Flicker-Killer) ---
        if (current_state == 0) {
            if (needs_update) {
                clear_screen();
                printf(COLOR_CYAN BOLD "==========================================================\033[K\n");
                printf("         [ SELECT YOUR MUSIC SOURCE / FOLDER ]            \033[K\n");
                printf("==========================================================\033[K\n" COLOR_RESET);
                for (int i = 0; i < found_folder_count; i++) {
                    if (i == folder_sel) printf(BG_BLUE BOLD " > %-52s " COLOR_RESET "\033[K\n", found_music_folders[i]);
                    else printf("   %-52s \033[K\n", found_music_folders[i]);
                }
                needs_update = 0;
            }
        } 
        else {
            // Player Mode: Full draw sirf tab jab kuch badle
            if (needs_update) {
                draw_dashboard(selected_index, player_state.volume, player_state.is_paused);
                needs_update = 0;
                last_sec = current_sec; // Sync timer
            }
            // Partial Refresh: Sirf timer update karo bina list ko touch kiye
            if (current_sec != last_sec && !player_state.is_paused && player_state.current_time > 0) {
                update_timer_only(); // [ANSI Sniper Mode](https://en.wikipedia.org)
                last_sec = current_sec;
            }
        }

if (player_state.playback_finished && !player_state.is_paused) {
    player_state.playback_finished = 0; // Reset karo
    
    // Agla gaana dhundo (Circular Logic)
    selected_index = (selected_index + 1) % entry_count;
    
    // Check karo ki wo folder toh nahi
    if (!browser_list[selected_index].is_directory) {
        play_song(browser_list[selected_index].path, browser_list[selected_index].name);
        needs_update = 1;
    }
}


        // --- ⌨️ NON-BLOCKING INPUT HANDLING ---
        if (_kbhit()) {
            int ch = _getch();
            needs_update = 1; // Key dabi toh screen update zaroori hai

            if (ch == 0 || ch == 224) { // Special Keys (Arrows/F-Keys)
                int spec = _getch();
                if (current_state == 0) {
                    if (spec == 72 && folder_sel > 0) folder_sel--;
                    if (spec == 80 && folder_sel < found_folder_count - 1) folder_sel++;
                } else {
                    if (spec == 72 && selected_index > 0) selected_index--;
                    if (spec == 80 && selected_index < entry_count - 1) selected_index++;
                    // F2/F3 Volume Logic
                    if (spec == 60) player_state.volume = (player_state.volume > 0.05f) ? player_state.volume - 0.05f : 0.0f;
                    if (spec == 61) player_state.volume = (player_state.volume < 0.95f) ? player_state.volume + 0.05f : 1.0f;
                }
            } else {
                switch (ch) {
                    case 13: // ENTER
                        if (current_state == 0) {
                            scan_directory(found_music_folders[folder_sel]);
                            if (entry_count > 0) current_state = 1;
                        } else {
                            play_song(browser_list[selected_index].path, browser_list[selected_index].name);
                        }
                        break;
                    case ' ': player_state.is_paused = !player_state.is_paused; break;
                    case 'b': case 'B': system("cls"); current_state = 0; needs_update = 1; break; // Back to Folders
                    case '+': case '=': player_state.volume = (player_state.volume < 0.95f) ? player_state.volume + 0.05f : 1.0f; break;
                    case '-': case '_': player_state.volume = (player_state.volume > 0.05f) ? player_state.volume - 0.05f : 0.0f; break;
                    case 'l': case 'L': seek_audio(10.0); needs_update = 1; break;
                    case 'j': case 'J': seek_audio(-10.0); needs_update = 1; break;
                    case 'q': case 'Q': is_running = 0; break;

case 'n': case 'N': // NEXT SONG
    if (entry_count > 0) {
        selected_index = (selected_index + 1) % entry_count; // Circular logic
        // Agar folder nahi hai toh play karo
        if (!browser_list[selected_index].is_directory) {
            play_song(browser_list[selected_index].path, browser_list[selected_index].name);
            needs_update = 1;
        }
    }
    break;


case 'p': case 'P': // PREVIOUS SONG
    if (entry_count > 0) {
        selected_index = (selected_index - 1 + entry_count) % entry_count; // Reverse Circular
        if (!browser_list[selected_index].is_directory) {
            play_song(browser_list[selected_index].path, browser_list[selected_index].name);
            needs_update = 1;
        }
    }
    break;

                }
            }
        }
        SDL_Delay(50); // CPU rest [SDL_Delay](https://wiki.libsdl.org)
    }

    // 3. Final Exit Cleanup
    stop_audio();
    cleanup_audio_engine();
    printf("\033[?25h"); // Cursor wapas dikhao exit par
    return 0;
}
