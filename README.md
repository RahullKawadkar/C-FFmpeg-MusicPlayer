\# 🎵 C-Music Player (FFmpeg + SDL2)



!\[Author](https://img.shields.io)

!\[Language](https://img.shields.io)





A professional command-line music player built with \*\*C\*\*, \*\*FFmpeg\*\* for decoding, and \*\*SDL2\*\* for audio playback.



\## ✨ Features

\- \*\*Smart Decoding:\*\* Buffer-aware decoding to keep the UI responsive.

\- \*\*Interactive Controls:\*\* 

&nbsp; - `Space`: Pause / Play

&nbsp; - `Up/Down Arrow`: Volume Control (Software Scaling)

&nbsp; - `Q`: Quit Player

\- \*\*Anti-Hang Logic:\*\* Prevents "Not Responding" issues during long pauses.



\## 🛠️ Requirements

\- FFmpeg (avcodec, avformat, avutil, swresample)

\- SDL2

\- MinGW-w64 (64-bit)

\- CMake



\## 🚀 How to Build

1\. Create a build folder: `mkdir build \&\& cd build`

2\. Configure with CMake: `cmake -G "MinGW Makefiles" ..`

3\. Compile: `mingw32-make`



\## 🎧 Usage

```cmd

my\_player.exe <path\_to\_music\_file.mp3>



---

\## 👨‍💻 Author

\- \*\*Name:\*\* \*\*Rahul Kawadkar\*\*

\- \*\*GitHub:\*\* \[Rahul](https://github.com/RahullKawadkar/C-FFmpeg-MusicPlayer)

\- \*\*Project:\*\* C-Music Player (FFmpeg + SDL2)





