#include "file_manager.h"
#include "ui_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <windows.h>

FileEntry browser_list[MAX_SONGS];
int entry_count = 0;
char found_music_folders[MAX_FOLDERS][MAX_FILE_PATH];
int found_folder_count = 0;

void discover_animation() {
    system("cls");
    printf("\033[H\033[?25l"); 
    printf("\n" COLOR_CYAN BOLD "   (O_-) Searching for Audio files");
    for(int i=0; i<6; i++) {
        printf(".");
        fflush(stdout);
        Sleep(200);
    }
    printf("\r\033[2K"); 
    fflush(stdout);
    system("cls");
}

void scan_directory(const char* dir_path) {
    struct dirent *entry;
    DIR *dp = opendir(dir_path);
    entry_count = 0; 

    if (dp == NULL) return;

    while ((entry = readdir(dp)) != NULL && entry_count < MAX_SONGS) {
        if (entry->d_name[0] == '.') continue; 

        // 🎯 FIX: Case-Insensitive check (mp3 vs MP3)
        char *ext = strrchr(entry->d_name, '.');
        if (ext && (stricmp(ext, ".mp3") == 0 || stricmp(ext, ".wav") == 0)) {
            browser_list[entry_count].is_directory = 0;
            strncpy(browser_list[entry_count].name, entry->d_name, 256);
            snprintf(browser_list[entry_count].path, MAX_FILE_PATH, "%s\\%s", dir_path, entry->d_name);
            entry_count++;
        }
    }
    closedir(dp);
}

int folder_has_audio(const char* dir_path) {
    char search_pattern[MAX_FILE_PATH];
    snprintf(search_pattern, MAX_FILE_PATH, "%s\\*", dir_path);

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(search_pattern, &findData);
    if (hFind == INVALID_HANDLE_VALUE) return 0;

    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char *ext = strrchr(findData.cFileName, '.');
            // 🎯 FIX: stricmp use kiya taaki Capital extensions bhi pakde jayein
            if (ext && (stricmp(ext, ".mp3") == 0 || stricmp(ext, ".wav") == 0)) {
                FindClose(hFind);
                return 1; 
            }
        }
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
    return 0;
}

void crawl_drives(const char* base_path, int depth) {
    // 🎯 FIX: Depth aur Folder limit check
    if (depth > 3 || found_folder_count >= MAX_FOLDERS) return; 

    if (folder_has_audio(base_path)) {
        strncpy(found_music_folders[found_folder_count++], base_path, MAX_FILE_PATH);
        return; 
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
                crawl_drives(next_path, depth + 1); 
            }
        } while (FindNextFile(hFind, &findData) && found_folder_count < MAX_FOLDERS);
        FindClose(hFind);
    }
}

void deep_discover_music() {
    found_folder_count = 0;
    discover_animation();

    crawl_drives("D:", 0);
    crawl_drives("E:", 0);
    char* user_music = getenv("USERPROFILE");
    if(user_music) {
        char music_dir[MAX_FILE_PATH];
        snprintf(music_dir, MAX_FILE_PATH, "%s\\Music", user_music);
        crawl_drives(music_dir, 0);
    }
}

void add_custom_folder() {
    char path[MAX_FILE_PATH];
    printf("\nEnter full folder path: ");
    fflush(stdin);
    fgets(path, MAX_FILE_PATH, stdin);
    path[strcspn(path, "\n")] = 0;
    scan_directory(path);
}
