#include "ui_manager.h"
#include "audio_engine.h"
#include <stdio.h>
#include <windows.h>

extern PlayerState player_state;

void init_terminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    
    // Windows 10+ mein ANSI mode enable karna [Microsoft Docs](https://learn.microsoft.com)
    dwMode |= 0x0004; 
    SetConsoleMode(hOut, dwMode);
    
    printf("\033[?25l"); // Cursor ko permanent chupa do (No Blinking)
    system("cls");       // Shuruat mein ek baar screen saaf
}

void clear_screen() {
    printf("\033[H"); // Cursor ko top-left (0,0) par le jao
}

void draw_dashboard(int selected_index, float volume, int is_paused) {
    printf("\033[H\033[?25l"); // Top-left + Hide Cursor

    // 1. Header (Same)
    printf(COLOR_CYAN BOLD "==========================================================\n" COLOR_RESET);
    printf(" %-10s: %-45s \033[K\n", is_paused ? COLOR_RED "PAUSED " : COLOR_GREEN "PLAYING", player_state.current_song_name);
    printf(COLOR_CYAN BOLD "==========================================================\n" COLOR_RESET);

    // 2. Playlist/Browser Logic (Updated Names)
    printf(COLOR_YELLOW "--- LIBRARY (Files & Folders) ---\033[K\n" COLOR_RESET);
    int window_size = 8;
    static int start_idx = 0;

    if (selected_index >= start_idx + window_size) start_idx = selected_index - window_size + 1;
    if (selected_index < start_idx) start_idx = selected_index;

    for (int i = 0; i < window_size; i++) {
        int idx = start_idx + i;
        printf("\033[K");
        if (idx < entry_count) { // song_count -> entry_count
            char *prefix = browser_list[idx].is_directory ? "[DIR] " : "      ";
            if (idx == selected_index) {
                printf(BG_BLUE BOLD " > %s%-46s " COLOR_RESET "\n", prefix, browser_list[idx].name); // playlist -> browser_list
            } else {
                printf("   %s%-46s \n", prefix, browser_list[idx].name);
            }
        } else {
            printf("\n");
        }
    }

    // 3. Footer Section (Line 14 Se Shuru)
    printf(COLOR_CYAN "==========================================================\033[K\n" COLOR_RESET);
    
    update_timer_only(); 

    // Volume & Controls (Line 16-18)
    int vol_bar = (int)((volume * 10.0f) + 0.5f);
    printf("\n\033[K Volume : ["); // \n hata kar dekho agar gap zyada lage
    for(int i=0; i<10; i++) printf(i < vol_bar ? "#" : "-");
    printf("] %3.0f%%  |  Status: %-10s\033[K\n", (double)volume * 100.0, is_paused ? "PAUSED" : "PLAYING");

    printf(COLOR_YELLOW " Controls: [UP/DN] Scroll | [ENT] Play | [F2/F3] Vol | [Q] Exit\033[K\n" COLOR_RESET);
    printf(COLOR_CYAN "==========================================================\033[K" COLOR_RESET);
}

// ui_manager.c mein ye do functions dhyan se update karo
void update_timer_only() {
    printf("\033[15;1H\033[?25l"); 

    char buffer[128]; // In-memory bar banane ke liye
    int bar_width = 30;
    int progress = (player_state.total_duration > 0) ? 
                   (int)((player_state.current_time / player_state.total_duration) * bar_width) : 0;

    // 🛠️ Ek string mein poora bar aur time build karo (No multiple printfs)
    int pos = sprintf(buffer, COLOR_CYAN " Progress: [" COLOR_RESET);
    for (int i = 0; i < bar_width; i++) {
        buffer[pos++] = (i < progress) ? '#' : '-';
    }
    
    // Time format: [00:00 / 03:30]
    sprintf(buffer + pos, COLOR_CYAN "] %02d:%02d / %02d:%02d \033[K" COLOR_RESET, 
           (int)player_state.current_time/60, (int)player_state.current_time%60,
           (int)player_state.total_duration/60, (int)player_state.total_duration%60);

    printf("%s", buffer);
    fflush(stdout); 
}


