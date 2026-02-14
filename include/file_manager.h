#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#define MAX_SONGS 1000
#define MAX_PATH 512

typedef struct {

char path[MAX_PATH];
char name[256];

} Song;

extern Song playlist[MAX_SONGS];
extern int song_count;

void get_default_music_path(char* out_path);
void scan_directory(const char* dir_path);
void add_custom_folder();

#endif