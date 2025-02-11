# Set Up for linux debian

## libs

### Dear ImGui

- All in one \
    `wget -P lib/ "https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.zip" && unzip lib/v1.90.9.zip -d lib/ && mv lib/imgui-1.90.9 lib/Dear-ImGui && rm lib/v1.90.9.zip`
- Download \
    `wget -P lib/ "https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.zip"`
- Unzip file \
    `unzip lib/v1.90.9.zip -d lib/`
- Rename \
    `mv lib/imgui-1.90.9 lib/Dear-ImGui`
- Remove zip file \
    `rm lib/v1.90.9.zip`

### GLFW

- All in one \
    `wget -P lib/ "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip" && unzip lib/3.4.zip -d lib/ && mv lib/glfw-3.4 lib/glfw && rm lib/3.4.zip && sudo apt install -y wayland-protocols libwayland-bin libwayland-dev libxkbcommon-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev pkg-config mingw-w64-x86-64-dev mingw-w64`
- Download \
    `wget -P lib/ "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip"`
- Unzip \
    `unzip lib/3.4.zip -d lib/`
- Rename \
    `mv lib/glfw-3.4 lib/glfw`
- Remove zip file \
    `rm lib/3.4.zip`
- Install wayland
    `sudo apt install wayland-protocols`
    `sudo apt install libwayland-bin`
    `sudo apt install libwayland-dev`
- Install xkbcommon
    `sudo apt install libxkbcommon-dev `
- Install libxrandr
    `sudo apt install libxrandr-dev`
- Install libxinerama
    `sudo apt install libxinerama-dev `
- Install libxcursor
    `sudo apt install libxcursor-dev`
- Install libxi
    `sudo apt install libxi-dev`
- Install pkg-config
    `sudo apt install pkg-config`
- Install mingw-w64
    `sudo apt install mingw-w64 mingw-w64-x86-64-dev`

### Vulkan

- All in one \
    `wget -qO - "https://packages.lunarg.com/lunarg-signing-key-pub.asc" | sudo apt-key add - && sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.296-noble.list "https://packages.lunarg.com/vulkan/1.3.296/lunarg-vulkan-1.3.296-noble.list" && sudo apt update && sudo apt install vulkan-sdk`
- apt key \
    `wget -qO - "https://packages.lunarg.com/lunarg-signing-key-pub.asc" | sudo apt-key add -`
- apt sources \
    `sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.296-noble.list "https://packages.lunarg.com/vulkan/1.3.296/lunarg-vulkan-1.3.296-noble.list"`
- apt update
    `sudo apt update`
- install
    `sudo apt install vulkan-sdk`
- download Vulkan Loader for windows
    `cd lib/`
    `git clone git@github.com:KhronosGroup/Vulkan-Loader.git`
    `cd Vulkan-Loader`
    `cmake -S . -B build -D UPDATE_DEPS=On`
    `cmake --build build`
- download Vulkan Headers for windows
    `git clone git@github.com:KhronosGroup/Vulkan-Headers.git`
    `cd Vulkan-Headers/`
    `cmake -S . -B build/`
    `cmake --install build --prefix build/install`
## Build

- Create folder
    `mkdir build`
- Get Inside
    `cd build`
- Build
    `cmake ..`
- Run
    `make`

## Build for Windows

- Create folder
    `mkdir build-windows`
- Get Inside
    `cd build-windows`
- Build
    ```sheel
    cmake .. \
    -D CMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake \
    -D UPDATE_DEPS=ON \
    -D CMAKE_INSTALL_PREFIX="../lib/Vulkan-Loader" \
    -D CMAKE_BUILD_TYPE=Release \
    -S "../lib/Vulkan-Loader" \
    -B "../build-windows"
    ```

    `cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake -DUPDATE_DEPS=ON -DCMAKE_INSTALL_PREFIX="$vulkan_loader_vendor_win_dir" -DCMAKE_BUILD_TYPE=Release -S"$vulkan_loader_source_dir" -B"$vulkan_loader_win_build_dir"`
- Run
    `make`
