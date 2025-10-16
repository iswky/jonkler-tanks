# Jonkler Tanks

Welcome to **Jonkler Tanks**!

## 📁 Table of Contents
1. [Building](#building)  
   1.1 [Cloning the Repository](#cloning-the-repository)  
   1.2 [Linux](#linux)  
2. [Dependencies](#dependencies)  
   2.1 [Linux](#linux-1)  
   - 2.1.1 [Using Distro Package Manager](#using-distro-package-manager)  
   - 2.1.2 [Using vcpkg](#using-vcpkg)  

   2.2 [Windows](#windows)  
      - 2.2.1 [Using vcpkg](#using-vcpkg-1)  


---

## Building

### Cloning the Repository
```bash
git clone https://github.com/tap-jf/jonkler-tanks --depth 1
```

### Linux
```bash
cd jonkler-tanks
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[ABSOLUTE PATH TO VCPKG]scripts/buildsystems/vcpkg.cmake
make -j4
```

---

## Dependencies

### Linux

#### Using Distro Package Manager

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

#### Using vcpkg

**Install vcpkg:**
```bash
git clone https://github.com/microsoft/vcpkg --depth 1
cd vcpkg
./bootstrap-vcpkg.sh -disableMetrics
```

**Install SDL:**
```bash
./vcpkg install sdl2 sdl2-image sdl2-mixer[mpg123] sdl2-ttf
```

---

### Windows

#### Using vcpkg

**Install vcpkg:**
```bash
git clone https://github.com/microsoft/vcpkg --depth 1
cd vcpkg
.\bootstrap-vcpkg.bat -disableMetrics
```

**Install SDL:**
```bash
./vcpkg install sdl2 sdl2-image sdl2-mixer[mpg123] sdl2-ttf
```

**FIX:** Arch systemd build error
```bash
sudo pacman -S base-devel meson ninja python python-jinja util-linux util-linux-libs acl libpwquality libseccomp tpm2-tss
```
