[![CD](https://github.com/humbertodias/ttf2pcx/actions/workflows/cd.yml/badge.svg)](https://github.com/humbertodias/ttf2pcx/actions/workflows/cd.yml)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/humbertodias/ttf2pcx)
![GitHub all downloads](https://img.shields.io/github/downloads/humbertodias/ttf2pcx/total)

# TTF to PCX Font Generator

A cross-platform Qt6 tool that converts TrueType fonts to PCX files for use with the Allegro game engine. Originally a Windows-only MFC application, now rewritten with Qt6 for Linux, Windows, and macOS.

## Features

* **Platform-agnostic** — Works on Linux, Windows, and macOS
* Converts TrueType fonts to PCX files for use with Allegro
* Supports both monochrome and antialiased output
* Live preview of selected fonts
* Configurable character ranges (Unicode support up to 0xFFFF)
* Antialiased fonts use 8×8 supersampling for 64 intensity levels
* Built with CMake for easy cross-platform compilation

## Building

### Requirements

* CMake 3.16 or later
* Qt6 (Core, Gui, Widgets modules)
* C++17 compatible compiler


## 📦 Build Instructions (per platform)

### Linux (Ubuntu/Debian)

<details>
  <summary>Show instructions</summary>

```bash
# Install dependencies
sudo apt-get install qt6-base-dev cmake build-essential

# Build
cmake -Bbuild
cmake --build build --config Release

# Run
./ttf2pcx-qt
```

</details>

### Windows

<details>
  <summary>Show instructions</summary>

```bash
# Install Qt6 from https://www.qt.io/download
# Make sure Qt6 is in your PATH or set CMAKE_PREFIX_PATH

cmake -Bbuild
cmake --build build --config Release

# Run
.\Release\ttf2pcx-qt.exe
```

</details>

### macOS

<details>
  <summary>Show instructions</summary>

```bash
# Install Qt6 via Homebrew
brew install qt6

# Build
cmake -Bbuild -DCMAKE_PREFIX_PATH=$(brew --prefix qt6)
cmake --build build --config Release

# Run
./ttf2pcx-qt
```

</details>

---

## Project Structure

```
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── src/                    # Source files
│   ├── main.cpp            # Application entry point
│   ├── mainwindow.cpp/h    # Main UI and logic
│   ├── character.cpp/h     # Character rendering
│   └── previewwidget.cpp/h # Font preview widget
```

## Usage

1. Launch the application: `./ttf2pcx-qt`
2. Select a font family from the dropdown (e.g., Times New Roman, Arial)
3. Choose a font size (default: 12)
4. Select a style (Regular, Bold, Italic, Bold Italic)
5. Set the character range:

   * Start: `0x20` (space character)
   * End: `0x7F` (standard ASCII range)
   * Or use custom ranges for Unicode characters (1–65535)
6. Choose output mode:

   * **Monochrome**
   * **Antialiased** with configurable Min/Max color (1–254)
7. Click **Export…** to save the font as a PCX file
8. Import the PCX into the Allegro grabber program


## Antialiased Fonts

Antialiased fonts are rendered at 8× the requested size and downsampled using an 8×8 supersampling grid, producing 64 grayscale intensity levels.

To use antialiased fonts in Allegro:

* Pass `-1` as the color parameter to `textout()`
* Ensure your palette has a gradient matching the Min/Max color range


## Architecture

This tool is built with Qt6 for cross-platform compatibility. All source files are in the `src/` directory:

* `src/mainwindow.cpp/h` — Main UI and application logic
* `src/character.cpp/h` — Platform-agnostic character rendering
* `src/previewwidget.cpp/h` — Font preview widget
* `src/main.cpp` — Entry point

The PCX export format remains identical to the original Allegro-compatible implementation.


## Original Credits

Based on **TTF → PCX CONVERTER**, version 2.2 (April 2002)
By Shawn Hargreaves
[shawn@talula.demon.co.uk](mailto:shawn@talula.demon.co.uk)
[http://www.talula.demon.co.uk/](http://www.talula.demon.co.uk/)

**Qt conversion**: December 2024
Platform-agnostic rewrite using Qt6 and CMake
