#include "file_manager.h"
#include <stdio.h>

int main() {

char music_path[MAX_PATH];

get_default_music_path(music_path);
printf("Scanning Folder: %s\n", music_path);

scan_directory(music_path);

printf("Total Songs Found: %d\n", song_count);

for(int i=0; i<song_count; i++) {
    printf("%d. %s\n", i+1,playlist[i].name);
}

return 0;

}