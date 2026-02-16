#include "ui_manager.h"
#include <stdio.h>
#include <windows.h>

void init_terminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(hOut, dwMode);
    
    // Cursor ko hide karo (Blinking band karne ke liye)
    printf("\033[?25l");
}

void clear_screen() {
    // \033[H cursor ko bina jhatke ke (0,0) pe le jata hai (No Flicker)
    printf("\033[H");
}

void draw_dashboard(int selected_index, float volume, int is_paused) {
    clear_screen(); 

    // Header: Ekdum saaf aur professional (No garbage characters)
    printf(CYAN BOLD "==========================================================\n");
    printf("         [ C-MUSIC PLAYER v2.0 | BY RAHUL ]               \n");
    printf("==========================================================\n" RESET);

    // Playlist Section
    printf(MAGENTA "\n--- YOUR MUSIC LIBRARY ---\n" RESET);
    for (int i = 0; i < song_count; i++) {
        if (i == selected_index) {
            // Selected gaana chamkega (Background Blue)
            printf(BG_BLUE BOLD " > %-52s " RESET "\n", playlist[i].name);
        } else {
            printf("   %-52s \n", playlist[i].name);
        }
    }

    // Footer Status Section
    printf(CYAN "\n==========================================================\n");
    
    // Volume Meter: [#####-----] 
    int vol_bar = (int)(volume * 10);
    printf(" Volume: [");
    for(int i=0; i<10; i++) printf(i < vol_bar ? "#" : "-");
    
    printf("] %.0f%%  |  Status: %s\n", volume * 100, is_paused ? "PAUSED " : "PLAYING");
    printf(" Controls: [UP/DN] Scroll | [ENT] Play | [SPC] Pause\n");
    printf("==========================================================\n" RESET);
}
