# 🎵 C-FFmpeg-MusicPlayer v2.1 (Stable)

A high-performance, lightweight terminal-based music player built from scratch using **C**, **FFmpeg**, and **SDL2**. This project showcases low-level audio decoding, multi-threading, and optimized terminal UI rendering.

![C-Music Player Demo](assets/Player_stable_v2.1.gif)

---

## 🚀 What's New in v2.1
- **Flicker-Free UI:** Optimized terminal refresh using ANSI escape sequences (Removed `system("cls")` lag).
- **Stable Auto-Play:** Improved signal synchronization between the audio engine and the main loop.
- **Deep Discovery:** Efficiently scans `D:`, `E:`, and `User/Music` folders for audio files.
- **Memory Management:** Fixed thread-leaks and implemented proper resource deallocation.

---

## ⌨️ Controls & Navigation

| Key | Action |
|-----|--------|
| `Enter` | Select Folder / Play Song |
| `Space` | Pause / Resume Playback |
| `N` / `P` | Next / Previous Track |
| `L` / `J` | Fast Forward / Rewind (10 seconds) |
| `+/-` | Volume Up / Down |
| `B` | Back to Folder Selection |
| `Q` | Exit Gracefully |

---

## 🛠 Technical Architecture

This player works by separating the concerns into three main modules:
1. **File Manager:** Handles directory crawling and metadata scanning using Windows API.
2. **Audio Engine:** Decodes compressed audio (MP3/WAV) into raw PCM using **FFmpeg** and manages the playback buffer via **SDL2**.
3. **UI Manager:** Renders a responsive dashboard in the terminal using ANSI escape codes for a smooth experience.



---

## 📦 Build Instructions

### Prerequisites
- **Compiler:** MinGW-w64 (GCC)
- **Libraries:** SDL2, FFmpeg (avcodec, avformat, swresample, avutil)
- **Build Tool:** CMake 3.10+

### Compilation
1. Clone the repository:
   ```bash
   git clone [https://github.com/RahullKawadkar/C-FFmpeg-MusicPlayer.git](https://github.com/RahullKawadkar/C-FFmpeg-MusicPlayer.git)
  **Create a build directory:** 
  mkdir build && cd build

**Generate build files and compile:**
cmake ..
mingw32-make

**Run the player**
./my_player_v2.exe

🤝 Acknowledgments
FFmpeg Team for the incredible multimedia libraries.

SDL Team for the robust audio handling.

Inspired by a passion for low-level C programming.
Made with ❤️
---
## 👨‍💻 Author Info
- ![Author](https://img.shields.io/badge/Author%20%3A-Rahul_Kawadkar-blue
)
- **GitHub Profile:** [RahullKawadkar](https://github.com)
