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
    printf("\033[2J\033[H"); // Cursor ko top-left (0,0) par le jao
}

void update_timer_only() {
    // 🎯 Sniper Position: Line 15 par jao (Jahan progress bar hai)
    printf("\033[15;1H\033[?25l"); 

    char buffer[128]; 
    int bar_width = 30;
    int progress = (player_state.total_duration > 0) ? 
                   (int)((player_state.current_time / player_state.total_duration) * bar_width) : 0;

    // 🛠️ Buffer build karo (Flicker-free logic)
    int pos = sprintf(buffer, COLOR_CYAN " Progress: [" COLOR_RESET);
    for (int i = 0; i < bar_width; i++) {
        buffer[pos++] = (i < progress) ? '#' : '-';
    }
    
    // Time format jodd do
    sprintf(buffer + pos, COLOR_CYAN "] %02d:%02d / %02d:%02d \033[K" COLOR_RESET, 
           (int)player_state.current_time/60, (int)player_state.current_time%60,
           (int)player_state.total_duration/60, (int)player_state.total_duration%60);

    // Ek saath print karo
    printf("%s", buffer);
    fflush(stdout); 
}


// src/ui_manager.c mein is function ko poora replace karo:

void draw_dashboard(int selected_index, float volume, int is_paused, int start_idx) {
    printf("\033[H\033[?25l"); 

    if (selected_index < 0) selected_index = 0;
    if (start_idx < 0) start_idx = 0;

    // 1. Header Section (Fixed Width: 58 characters)
    printf(COLOR_CYAN BOLD "==========================================================\033[K\n" COLOR_RESET);
    
    char status_text[64];
    if (player_state.is_running && strlen(player_state.current_song_name) > 0) {
        // %-45.45s ka matlab hai naam 45 chars par kat jayega (No Line Wrap)
        snprintf(status_text, 60, " %s : %-45.45s", is_paused ? "PAUSED " : "PLAYING", player_state.current_song_name);
    } else {
        strcpy(status_text, " STATUS : IDLE");
    }
    printf("%-58s\033[K\n", status_text);
    printf(COLOR_CYAN BOLD "==========================================================\033[K\n" COLOR_RESET);

    // 2. Playlist Section (Strictly 8 Lines)
    printf(COLOR_YELLOW "--- LIBRARY (Scroll Mode) ---%-28s\033[K\n" COLOR_RESET, "");
    
    for (int i = 0; i < 8; i++) {
        int idx = start_idx + i;
        printf("\033[K"); // Line clear karo taaki purana text ghost na bane
        if (idx < entry_count) {
            char display_line[64];
            char *prefix = browser_list[idx].is_directory ? "[DIR] " : "      ";
            
            // 🎯 FIX 3: Song name clipping taaki auto-scroll trigger na ho
            snprintf(display_line, 54, "%s%-46.46s", prefix, browser_list[idx].name);
            
            if (idx == selected_index) 
                printf(BG_BLUE BOLD " > %-54s " COLOR_RESET "\n", display_line);
            else 
                printf("   %-54s \n", display_line);
        } else {
            // Khali lines taaki footer hamesha apni jagah rahe
            printf("%-58s\n", ""); 
        }
    }

    // 3. Footer Section (Fixed Height)
    printf(COLOR_CYAN "==========================================================\033[K\n" COLOR_RESET);
    
    // Timer Line (Line 15 update)
    update_timer_only(); 

    // Volume & Help Lines
    int vol_bar = (int)((volume * 10.0f) + 0.5f);
    printf("\n\033[K Volume : [");
    for(int i=0; i<10; i++) printf(i < vol_bar ? "#" : "-");
    printf("] %3.0f%%  |  Status: %-10s\033[K\n", (double)volume * 100.0, is_paused ? "PAUSED" : "PLAYING");

    printf(COLOR_YELLOW " Controls: [UP/DN] Scroll | [N/P] Next/Prev | [B] Back | [Q] Exit\033[K\n" COLOR_RESET);
    printf(COLOR_CYAN "==========================================================\033[J" COLOR_RESET);
    
    fflush(stdout); // [C fflush Docs](https://en.cppreference.com)
}


void show_outro_animation() {
    system("cls");
    printf("\033[?25l"); // Cursor hide karo
    
    // Center mein aane ke liye thode gaps
    for(int i=0; i<5; i++) printf("\n");

    const char* lines[] = {
        "       __________________________________________       ",
        "      |                                          |      ",
        "      |        THANK YOU FOR USING C-MUSIC       |      ",
        "      |              PLAYER v2.0                 |      ",
        "      |__________________________________________|      ",
        "                                                        ",
        "                MADE WITH <3 BY RAHUL                   ",
        "           ________________________________             "
    };

    // Animation Loop: Rang badalte huye (Cyan -> White -> Cyan)
    for(int flash=0; flash<2; flash++) {
        printf("\033[H"); // Top par jao overwrite karne ke liye
        for(int i=0; i<5; i++) printf("\n"); // Wahi vertical gap

        const char* color = (flash % 2 == 0) ? COLOR_CYAN : COLOR_YELLOW;
        
        for(int i=0; i<8; i++) {
            printf("%s%s%s\n", color, BOLD, lines[i]);
            Sleep(50); // Dheere-dheere chhapne ka feel
        }
        Sleep(500); // Thoda ruk kar chamko
    }

    printf(COLOR_RESET);
    printf(COLOR_RED "\n\n                 Exiting Gracefully...\n\n"COLOR_RESET);
    Sleep(3000);
    printf("\033[?25h"); // Cursor wapas dikhao (System stability ke liye)
}



