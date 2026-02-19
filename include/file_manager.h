#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <windows.h>

#define MAX_SONGS 1000
#define MAX_FOLDERS 20
#define MAX_FILE_PATH 512

typedef struct {
    char path[MAX_FILE_PATH];
    char name[256];
    int is_directory; 
} FileEntry;

extern FileEntry browser_list[MAX_SONGS];
extern int entry_count;
extern char found_music_folders[MAX_FOLDERS][MAX_FILE_PATH];
extern int found_folder_count;

void get_default_music_path(char* out_path);
void scan_directory(const char* dir_path);
void deep_discover_music();
void discover_animation();
void add_custom_folder();

#endif
