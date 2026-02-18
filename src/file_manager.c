#include "file_manager.h"
#include "ui_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <windows.h>

// 1. Global Variables (Jo Linker dhoond raha hai)
FileEntry browser_list[MAX_SONGS];
int entry_count = 0;
char found_music_folders[MAX_FOLDERS][MAX_FILE_PATH];
int found_folder_count = 0;

// 2. Animation Logic
void discover_animation() {
    system("cls");
    printf("\033[H\033[?25l"); 

    printf("\n" COLOR_CYAN BOLD "\n   (O_-) Searching for Audio files");
    for(int i=0; i<6; i++) {
        printf(".");
        fflush(stdout);
        Sleep(200);
    }

    printf("\r\033[2K"); 
    fflush(stdout);
    system("cls");
}

// 3. Scan Logic (Folders aur Files dono ke liye)
void scan_directory(const char* dir_path) {
    struct dirent *entry;
    DIR *dp = opendir(dir_path);
    entry_count = 0; // <--- Sabse zaroori: Purani list (Folders) ko saaf karo

    if (dp == NULL) return;

    while ((entry = readdir(dp)) != NULL && entry_count < MAX_SONGS) {
        if (entry->d_name[0] == '.') continue; // Hidden files skip karo

        // Sirf .mp3 aur .wav files ko browser_list mein dalo
        if (strstr(entry->d_name, ".mp3") || strstr(entry->d_name, ".wav")) {
            browser_list[entry_count].is_directory = 0; // Ye ab gaana hai
            strncpy(browser_list[entry_count].name, entry->d_name, 256);
            snprintf(browser_list[entry_count].path, MAX_FILE_PATH, "%s\\%s", dir_path, entry->d_name);
            entry_count++;
        }
    }
    closedir(dp);
}


// 4. Auto Discovery (Jo humne pehle discuss kiya tha)

// 5. Custom Path Function
void add_custom_folder() {
    char path[MAX_FILE_PATH];
    printf("\nEnter full folder path: ");
    fflush(stdin);
    fgets(path, MAX_FILE_PATH, stdin);
    path[strcspn(path, "\n")] = 0;
    scan_directory(path);
}


// Helper function: Kya is folder mein gaane hain?
int folder_has_audio(const char* dir_path) {
    char search_pattern[MAX_FILE_PATH];
    snprintf(search_pattern, MAX_FILE_PATH, "%s\\*", dir_path);

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(search_pattern, &findData);

    if (hFind == INVALID_HANDLE_VALUE) return 0;

    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (strstr(findData.cFileName, ".mp3") || strstr(findData.cFileName, ".wav")) {
                FindClose(hFind);
                return 1; // Mil gaya!
            }
        }
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
    return 0;
}

// Asli Recursive Scanner
void crawl_drives(const char* base_path, int depth) {
    if (depth > 3 || found_folder_count >= MAX_FOLDERS) return; // Zyada gehra mat jao (Speed ke liye)

    if (folder_has_audio(base_path)) {
        strncpy(found_music_folders[found_folder_count++], base_path, MAX_FILE_PATH);
        return; // Is folder ka kaam ho gaya
    }

    char search_pattern[MAX_FILE_PATH];
    snprintf(search_pattern, MAX_FILE_PATH, "%s\\*", base_path);

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(search_pattern, &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                
                char next_path[MAX_FILE_PATH];
                snprintf(next_path, MAX_FILE_PATH, "%s\\%s", base_path, findData.cFileName);
                crawl_drives(next_path, depth + 1); // Agle level par jao
            }
        } while (FindNextFile(hFind, &findData));
        FindClose(hFind);
    }
}

void deep_discover_music() {
    found_folder_count = 0;
    discover_animation();

    // In rasto se shuruat karo
    crawl_drives("D:", 0);
    crawl_drives("E:", 0);
    char* user_music = getenv("USERPROFILE");
    if(user_music) {
        char music_dir[MAX_FILE_PATH];
        snprintf(music_dir, MAX_FILE_PATH, "%s\\Music", user_music);
        crawl_drives(music_dir, 0);
    }
}

