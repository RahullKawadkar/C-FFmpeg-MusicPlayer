#include "file_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <windows.h> // Windows API ke liye zaroori hai

Song playlist[MAX_SONGS];
int song_count = 0;

// 1. Windows se "User Profile" mangwane ka logic
void get_default_music_path(char* out_path) {
    // USERPROFILE environment variable se "C:\Users\Name" milta hai
    char* user_profile = getenv("USERPROFILE");
    if (user_profile != NULL) {
        sprintf(out_path, "%s\\Music", user_profile);
    } else {
        strcpy(out_path, "C:\\"); // Fallback agar kuch na mile
    }
}

// 2. Folder scan karke sirf .mp3 uthane ka logic
void scan_directory(const char* dir_path) {
    struct dirent *entry;
    DIR *dp = opendir(dir_path);

    if (dp == NULL) return;

    while ((entry = readdir(dp)) != NULL && song_count < MAX_SONGS) {
        // Sirf .mp3 aur .wav files dhoondo
        if (strstr(entry->d_name, ".mp3") || strstr(entry->d_name, ".wav")) {
            // Poora path (folder + filename)
            snprintf(playlist[song_count].path, MAX_PATH, "%s\\%s", dir_path, entry->d_name);
            // Sirf dikhane ke liye naam
            strncpy(playlist[song_count].name, entry->d_name, 256);
            song_count++;
        }
    }
    closedir(dp);
}
