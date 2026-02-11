cmake_minimum_required(VERSION 3.10)
project(MyMusicPlayer C)

set(CMAKE_C_STANDARD 99)

# vcpkg path
set(VCPKG_PATH "C:/Users/rahul/vcpkg/installed/x64-mingw-static")

# Headers ke liye ye do raste zaroori hain
include_directories(${VCPKG_PATH}/include)
include_directories(${VCPKG_PATH}/include/SDL2) # Yeh line SDL.h dhoondhne mein madad karegi

link_directories(${VCPKG_PATH}/lib)

add_executable(my_player main.c)

target_link_libraries(my_player 
    avformat avcodec avutil swresample 
    SDL2      # Sirf SDL2 rakhein, SDL2main HATA DEIN
    ws2_32 bcrypt crypt32 ncrypt secur32
    m setupapi ole32 advapi32 shell32 user32 gdi32 imm32 version oleaut32 winmm
    -mconsole # Yeh batata hai ki humein WinMain nahi, normal main chahiye
)
