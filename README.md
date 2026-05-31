# Sky's VLC Reimagined Pro

A free and open-source media player architected using C++17, Qt6, and CMake. Designed with a modular architecture similar to industry giants like VLC, MPV, and MPC-HC, featuring a hardware-accelerated rendering pipeline.

## Project Architecture

The codebase has been meticulously structured into a modular, usable system.
Each domain of the media player is isolated to ensure scalability, ease of maintenance, and future backend expandability.

### Directory Structure

- **`backend/`**: Contains the abstract `IMediaBackend` interface and concrete implementations (`QtMediaBackend`). Ready for future `FFmpegBackend` integration.
- **`dsp/`**: Digital Signal Processing logic. Features a fully functional 10-band audio `Equalizer` with standard presets (Flat, Rock, Bass Boost, etc.).
- **`metadata/`**: Contains `MetadataEngine` for asynchronous extraction of media properties without blocking the UI thread, and `SubtitleEngine` for parsing subtitle data.
- **`rendering/`**: A custom `OpenGLRenderer` subclassing `QOpenGLWidget` providing a GPU-accelerated texture pipeline. It features robust GLSL viewport math to perfectly handle dynamic Aspect Ratios and sub-pixel High-DPI scaling.
- **`settings/`**: The `SettingsManager` cleanly encapsulates all disk I/O for user preferences using `QSettings`.
- **`ui/`**: All visual components, including `MainWindow`, `MediaInfoDialog`, and `EqualizerDialog`. Styled with a gorgeous vibrant blue professional identity.
- **`src/`** & **`include/`**: Core application bootstrapping and Qt Resource (`.qrc`) asset binding.

## Core Features & Upgrades
- **Hardware-Accelerated Display Modes**: Manually override how video frames are rendered to the screen. Cycle through display modes (Fit, Fill, Stretch, 100%) and Aspect Ratios (16:9, 4:3, 21:9) instantly.
- **Detachable Mini-Player**: Toggle using `Ctrl+M` to detach the video into an always-on-top, borderless minimalist frame that dynamically collapses symmetrically to the center of your screen.
- **Media Information Inspector**: Asynchronously queries the metadata engine to display deep file properties (`Ctrl+I`).
- **Persistent State**: The player automatically remembers your volume, playback progress per file, equalizer state, and recent file history across sessions.

## Build Requirements
- **C++17** compatible compiler (GCC/Clang/MSVC)
- **CMake** 3.16+
- **Qt6**: `Core`, `Gui`, `Widgets`, `Multimedia`, `MultimediaWidgets`, `OpenGLWidgets`, `Concurrent`

## Build Instructions (MSYS2 / Windows)

```bash
mkdir build
cd build
cmake -G Ninja ..
cmake --build .
./SkyVLCReimagined.exe
```

*Note: For fully native FFmpeg integration without relying on QtMultimedia, developers can implement the `FFmpegBackend` stub by binding `libavformat`, `libavcodec`, and mapping decoded `AVFrame` data to `OpenGLRenderer`.*
