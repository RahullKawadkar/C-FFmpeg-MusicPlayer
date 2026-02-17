#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "file_manager.h"

// --- Standard Colors ---
#define COLOR_RESET  "\x1b[0m"
#define COLOR_RED    "\x1b[31m"
#define COLOR_GREEN  "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE   "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN   "\x1b[36m"
#define COLOR_WHITE  "\x1b[37m"

// --- Bright/Bold Variants (Modern Look) ---
#define BOLD         "\x1b[1m"
#define UNDERLINE    "\x1b[4m"
#define BRIGHT_CYAN    "\x1b[96m"
#define BRIGHT_MAGENTA "\x1b[95m"
#define BRIGHT_YELLOW  "\x1b[93m"
#define BRIGHT_GREEN   "\x1b[92m"

// --- Background Colors (For Highlighting) ---
#define BG_BLUE      "\x1b[44m"
#define BG_MAGENTA   "\x1b[45m"
#define BG_CYAN      "\x1b[46m"

// --- Functions ---
void init_terminal();
void clear_screen();
void draw_header();
void move_cursor(int row, int col);
void update_timer_only(); 
void draw_dashboard(int selected_index, float volume, int is_paused);

#endif
