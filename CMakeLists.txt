cmake_minimum_required(VERSION 3.10)
project(MyMusicPlayer_v2 C)

set(CMAKE_C_STANDARD 99)

# vcpkg path (Same as before)
set(VCPKG_PATH "C:/Users/rahul/vcpkg/installed/x64-mingw-static")

# Header files dhoondne ke liye rasta
include_directories(include)
include_directories(${VCPKG_PATH}/include)
include_directories(${VCPKG_PATH}/include/SDL2)

link_directories(${VCPKG_PATH}/lib)

# Saari files ko ek saath compile karna
add_executable(my_player_v2 
    src/main.c 
    src/file_manager.c
    src/ui_manager.c
    src/audio_engine.c
    # Agle modules: src/audio_engine.c src/ui_manager.c
)

# Static linking for portability
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

target_link_libraries(my_player_v2 
    PRIVATE
    mingw32 #
    SDL2main 
    SDL2 
    avformat avcodec avutil swresample 
    ws2_32 bcrypt crypt32 ncrypt secur32
    m setupapi ole32 advapi32 shell32 user32 gdi32 imm32 version oleaut32 winmm
    -mconsole # 
)
