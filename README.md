# Jonkler Tanks

Welcome to **Jonkler Tanks**! This guide explains how to set up, build, and contribute to the project on Linux and Windows.

## 📁 Table of Contents
1. [Building](#1-building)
   - 1.1. [Cloning the Repository](#11-cloning-the-repository)
   - 1.2.1. [Linux Build Instructions](#121-linux-build-instructions)
   - 1.2.2. [Windows Build Instructions](#122-linux-build-instructions)
2. [Dependencies](#2-dependencies)
   - 2.1. [Linux Dependencies](#21-linux-dependencies)
      - 2.1.1. [Using Distro Package Manager](#211-using-distro-package-manager)
      - 2.1.2. [Using vcpkg](#212-using-vcpkg-linux)
   - 2.2. [Windows Dependencies](#22-windows-dependencies)
      - 2.2.1. [Using vcpkg](#221-using-vcpkg-windows)
3. [Contribution](#3-contribution)
4. [Adding a bot](#4-adding-your-own-bot)

---

## 1. Building
Before doin' this install [Dependencies](#2-dependencies) 
### 1.1. Cloning the Repository
To start, clone the repository with this command:

```bash
git clone https://github.com/tap-jf/jonkler-tanks
```

### 1.2.1. Linux Build Instructions
Follow these steps to build the project on Linux:

1. Go to the project directory:
   ```bash
   cd jonkler-tanks
   ```
2. Create a build directory and enter it:
   ```bash
   mkdir build && cd build
   ```
3. Run CMake with the vcpkg toolchain:
   ```bash
   cmake ..
   ```
4. Build the project:
   ```bash
   make -j4
   ```
### 1.2.2 Windows Build Instructions
Follow these steps to build the project on Windows 10/11:

1. Go to the project directory:
   ```bash
   cd jonkler-tanks
   ```
2. Create a build directory and enter it:
   ```bash
   mkdir build && cd build
   ```
3. Run CMake with the vcpkg toolchain and generate files for VS22:
   ```bash
   cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=[ABSOLUTE PATH TO VCPKG]/scripts/buildsystems/vcpkg.cmake
   ```
4. Open ".sln" file in Visual Studio and build
---

## 2. Dependencies

### 2.1. Linux Dependencies

#### 2.1.1. Using Distro Package Manager
Install the required dependencies using your distribution's package manager.

**Arch / Manjaro:**
```bash
sudo pacman -S sdl2-compat sdl2_mixer sdl2_ttf sdl2_image gcc cmake make
```

**Ubuntu / Debian:**
```bash
sudo apt-get update
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev gcc cmake make
```

**Fedora:**
```bash
sudo dnf install SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel mpg123-devel gcc cmake make
```

#### 2.1.2. Using vcpkg (Linux)
Alternatively, use vcpkg to manage dependencies.

1. Install vcpkg:
   ```bash
   git clone https://github.com/microsoft/vcpkg
   cd vcpkg
   ./bootstrap-vcpkg.sh -disableMetrics
   ./vcpkg integrate install
   ```
   Set up env variable for vcpkg
   ```bash
   echo "export VCPKG_ROOT=$(pwd)" >> ~/.bashrc
   source ~/.bashrc
   ```
   Add vcpkg in path var (**Optional**)
   ```bash
   echo 'export PATH='$(pwd)':$PATH' >> ~/.bashrc
   source ~/.bashrc
   ```

2. Install SDL dependencies:

   **Install globally**
   ```bash
   ./vcpkg install sdl2 sdl2-image sdl2-mixer[mpg123] sdl2-ttf
   ```
   **OR install locally**
   
   (**If vcpkg was added in PATH** (*run in jonkler-tanks dir*))
   ```bash
   vcpkg install
   ```
3. **Fix for Arch systemd build error** (if encountered):
   ```bash
   sudo pacman -S base-devel meson ninja python python-jinja util-linux util-linux-libs acl libpwquality libseccomp tpm2-tss
   ```

### 2.2. Windows Dependencies

#### 2.2.1. Using vcpkg (Windows)
To manage dependencies on Windows, use vcpkg.

1. Install vcpkg:
   ```bash
   git clone https://github.com/microsoft/vcpkg
   cd vcpkg
   .\bootstrap-vcpkg.bat -disableMetrics
   .\vcpkg integrate install
   ```
2. Install SDL dependencies:
   ```bash
   .\vcpkg install sdl2 sdl2-image sdl2-mixer[mpg123] sdl2-ttf
   ```

---

## 3. Contribution

Here's how you can contribute:

1. **Fork the Repository**:
   Go to the [Jonkler Tanks repository](https://github.com/tap-jf/jonkler-tanks) and click "Fork" to create a copy in your GitHub account.

2. **Format Code with clang-format**:
   Use the provided `.clang-format` file to keep the code style consistent. Run this command to format your code:
   ```bash
   clang-format -i <file>
   ```
   Learn more about `clang-format` in the [official wiki](https://clang.llvm.org/docs/ClangFormat.html). Make sure to format all changed files.

3. **Submit a Pull Request**:
   - Create a new branch for your changes:
     ```bash
     git checkout -b feature/your-feature-name
     ```
   - Commit your changes with clear commit messages.
   - Push your branch to your forked repository:
     ```bash
     git push origin feature/your-feature-name
     ```
   - Open a pull request on the main repository, explaining your changes and linking any relevant issues.

## 4. Adding your own bot

Do u wanna add ur own bot? Follow this guide --> [**guide**](https://github.com/tap-jf/jonkler-tanks/blob/dev/botHowTo.md)
