#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "file_manager.h"

// Professional ANSI Colors
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define CYAN        "\033[36m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define MAGENTA     "\033[35m"
#define BG_BLUE     "\033[44m"

// UI Functions
void init_terminal();           // Windows colors enable karne ke liye
void draw_dashboard(int selected_index, float volume, int is_paused);
void clear_screen();            // Screen flicker rokne ke liye

#endif


