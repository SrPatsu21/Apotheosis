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
    `wget -P lib/ "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip" && unzip lib/3.4.zip -d lib/ && mv lib/glfw-3.4 lib/glfw && rm lib/3.4.zip && sudo apt install wayland-protocols libwayland-bin libwayland-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev pkg-config mingw-w64 mingw-w64-x86-64-dev`
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
    `wget -qO - "https://packages.lunarg.com/lunarg-signing-key-pub.asc" | sudo apt-key add - && sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.296-noble.list "https://packages.lunarg.com/vulkan/1.3.296/lunarg-vulkan-1.3.296-noble.list" && sudo apt update && sudo apt install vulkan-sdk && wget -P lib/ "https://sdk.lunarg.com/sdk/download/1.3.296.0/windows/VulkanSDK-1.3.296.0-Installer.exe" && cd lib/ && sudo apt install 7zip && rm VulkanSDK-1.3.296.0-Installer.exe`
- apt key \
    `wget -qO - "https://packages.lunarg.com/lunarg-signing-key-pub.asc" | sudo apt-key add -`
- apt sources \
    `sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.296-noble.list "https://packages.lunarg.com/vulkan/1.3.296/lunarg-vulkan-1.3.296-noble.list"`
- apt update
    `sudo apt update`
- install
    `sudo apt install vulkan-sdk`
- download Vulkan for windows crosscomplile
    `wget -P lib/ "https://sdk.lunarg.com/sdk/download/1.3.296.0/windows/VulkanSDK-1.3.296.0-Installer.exe"`
    `cd lib/`
    `sudo apt install 7zip`
    `7z x VulkanSDK-1.3.296.0-Installer.exe -o./vulkan-sdk-win`
    `rm VulkanSDK-1.3.296.0-Installer.exe`

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
    ```
        cmake .. \
        -D CMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake
    ```
    <!-- could be util
        -D UPDATE_DEPS=ON
     -->
- Run
    `make`
