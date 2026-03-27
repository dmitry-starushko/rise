# Building RISE from Source

This document describes how to build the RISE ZX Spectrum emulator from source code.

> ⚠️ **Note**: RISE requires modern compilers and libraries. Please read the requirements carefully before starting.

## Table of Contents
- [Requirements](#requirements)
- [Tested Platforms](#tested-platforms)
- [Building on Linux](#building-on-linux)
- [Building on Windows](#building-on-windows)
- [CMake Options](#cmake-options)

---

## Requirements

### Common (all platforms)
| Dependency | Minimum Version | Notes |
|------------|----------------|-------|
| CMake | 3.30 | Build system |
| C++ Compiler | C++23 support | See platform-specific requirements below |
| Qt6 | 6.10+ | Core, Gui, Widgets, Multimedia modules |
| Boost | 1.74+ | Header-only usage |
| zstd | 1.5+ | Compression library (static on Windows) |
| SDL3 | 3.4+ | Gamepad/joystick support (static on Windows) |

### Compiler Requirements

| Platform | Compiler | Minimum Version |
|----------|----------|----------------|
| **Linux** | GCC | 15+ |
| **Linux** | Clang | 21+ |
| **Windows** | MinGW-w64 (GCC) | 15+ |

> ⚠️ **Important for Windows**: Qt6 must be **rebuilt with the same MinGW version** you use for RISE. Prebuilt Qt binaries from qt.io are typically built with older MinGW and are **not compatible**.
>
> **Qt 6.10.2 source code used for Windows builds is available here:**  
> [Download Qt 6.10.2 source – Google Drive](https://drive.google.com/file/d/1LuffFA2iebAFpstSQVcbaohxpAhkoxTg/view?usp=sharing)
>
> This is the exact source archive used to build the Qt libraries included with RISE. You may use it to rebuild Qt with your own toolchain if needed.

---

## Tested Platforms

RISE build is actively tested on the following configurations:

| OS | Version | Compiler | Qt6 Source |
|----|---------|----------|------------|
| 🐧 Linux | Manjaro (latest) | GCC 15 / Clang 21 | System packages |
| 🐧 Linux | Ubuntu 24.04 LTS | GCC 15 (via PPA/toolchain) | Built from source / Qt installer |
| 🪟 Windows | Windows 10 | MinGW-w64 GCC 15 | Qt 6.10+ rebuilt with same MinGW |

> Builds on other distributions or Windows versions may work but are not officially tested.

---

## Building on Linux

### 1. Install dependencies

#### Manjaro (latest)
```bash
# Compiler
sudo pacman -S gcc clang cmake

# Qt6 modules (from official repos)
sudo pacman -S qt6-base qt6-multimedia

# Other dependencies
sudo pacman -S boost-libs zstd sdl3
```

#### Ubuntu 24.04 LTS
```bash
# Update package lists
sudo apt update

# Install build tools
sudo apt install cmake build-essential

# GCC 15 (if not available in main repos, use toolchain PPA)
sudo apt install gcc-15 g++-15

# Qt6 development packages
sudo apt install qt6-base-dev qt6-multimedia-dev

# Boost and zstd (available in repos)
sudo apt install libboost-dev libzstd-dev

# ⚠️ SDL3 is NOT available in Ubuntu 24.04 repositories
# You must build it from source. See SDL3 documentation for instructions.
```

### 2. Clone the repository
```bash
git clone https://github.com/dmitry-starushko/rise.git  
cd rise
```

### 3. Create and enter build directory
```bash
mkdir build && cd build
```

### 4. Configure with CMake
```bash
# Using GCC 15
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-15

# Or using Clang 21
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-21
```

> 💡 Optional: Specify install prefix
> ```bash
> cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
> ```

### 5. Build the project
```bash
# Using all available CPU cores
cmake --build . --config Release -j$(nproc)
```

### 6. (Optional) Install system-wide
```bash
sudo cmake --install .
```

### 7. Run the emulator
```bash
# If installed:
rise

# Or run directly from build directory:
./rise
```

---

## Building on Windows

### Prerequisites
- **MinGW-w64 with GCC 15+** installed and in `PATH`
- **Qt 6.10+ built from source** using the **same MinGW toolchain** (pre-built archive available above)
- CMake 3.30+ in `PATH`
- Boost headers (header-only, no compilation required)

**Boost on Windows:**

Download the latest version from [boost.org](https://www.boost.org/), extract to a folder (e.g., `C:/boost_1_85_0`), and set `BOOST_ROOT` to that path.

> ⚠️ **Critical**: Do not use prebuilt Qt binaries from qt.io — they are built with different toolchains and will cause ABI incompatibilities.

### 1. Clone the repository
```cmd
git clone https://github.com/dmitry-starushko/rise.git  
cd rise
```

### 2. Create and enter build directory
```cmd
mkdir build && cd build
```

### 3. Configure with CMake
```cmd
cmake .. ^
    -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_CXX_COMPILER="C:/path/to/mingw15/bin/g++.exe" ^
    -DBOOST_ROOT="C:/path/to/boost" ^
    -DCMAKE_PREFIX_PATH="C:/path/to/qt-minGW-6.10.2"
```

> 🔧 Replace paths with your actual installation directories:
> - `CMAKE_CXX_COMPILER`: Full path to `g++.exe` from your MinGW 15 installation
> - `BOOST_ROOT`: Directory containing `boost/version.hpp`
> - `CMAKE_PREFIX_PATH`: Root of your Qt 6.10+ build (the directory containing `lib/cmake/Qt6`)

> 💡 **Note about dependencies on Windows**: Both **SDL3** and **zstd** will be **automatically downloaded, built from source, and statically linked** by CMake using FetchContent. No manual installation required.

### 4. Build the project
```cmd
cmake --build . --config Release -j8
```

### 5. (Optional) Install
```cmd
cmake --install . --config Release --prefix "C:/Program Files/RISE"
```

### 6. Run the emulator
```cmd
# From build directory:
Release\rise.exe

# Or after install:
"C:\Program Files\RISE\bin\rise.exe"
```

---

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | *(empty)* | Build type: `Release`, `Debug`, `RelWithDebInfo` |
| `CMAKE_INSTALL_PREFIX` | Platform default | Installation directory |
| `CMAKE_CXX_COMPILER` | System default | Path to C++ compiler (g++-15, clang++-21, etc.) |
| `BOOST_ROOT` | *(required on Windows)* | Path to Boost installation |
| `CMAKE_PREFIX_PATH` | *(empty)* | Additional paths for CMake to find Qt/packages |
| `PROF_GEN` | `OFF` | Enable profiling instrumentation (advanced) |
| `PROF_USE` | `OFF` | Use previously generated profiling data (advanced) |

### Example: Debug build with custom compiler
```bash
# Linux with Clang 21
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++-21

# Windows with custom MinGW path
cmake .. -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="D:/toolchains/mingw15/bin/g++.exe"
```

---

## Getting Help

- Report build issues: [GitHub Issues](https://github.com/dmitry-starushko/rise/issues)  
  Please include: OS version, compiler version, Qt version, SDL3 version, and full CMake output.
- Community support: [Discussions](https://github.com/dmitry-starushko/rise/discussions)
- Contact: dmitry.starushko@gmail.com

---

*Last updated: March 2026*
