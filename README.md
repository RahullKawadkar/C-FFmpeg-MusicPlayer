# 🎵 C-Music Player (FFmpeg + SDL2)
### Developed by [Rahul Kawadkar](https://github.com) 🚀

![Language](https://img.shields.io/badge/C-blue)

![Author](https://img.shields.io^logo=github)

A high-performance command-line music player built with **C**, **FFmpeg** for decoding, and **SDL2** for audio playback.

## ✨ Features
- **Smart Decoding:** Buffer-aware decoding to keep the UI responsive.
- **Interactive Controls:** 
  - `Space`: Pause / Play
  - `Up/Down Arrow`: Volume Control (Software Scaling)
  - `Q`: Quit Player
- **Anti-Hang Logic:** Prevents "Not Responding" issues during long pauses.

## 🚀 How to Build & Run
1. `mkdir build && cd build`
2. `cmake -G "MinGW Makefiles" ..`
3. `mingw32-make`
4. `my_player.exe <path_to_music_file.mp3>`

---
## 👨‍💻 Author Info
- **Name:** Rahul Kawadkar
- **GitHub Profile:** [RahullKawadkar](https://github.com)
