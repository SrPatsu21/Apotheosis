# Set Up for linux debian

## libs

### Dear ImGui

- All in one \
    `wget -P lib/ "https://github.com/ocornut/imgui/archive/refs/tags/v1.92.0.zip" && unzip lib/v1.92.0.zip -d lib/ && mv lib/imgui-1.92.0 lib/Dear-ImGui && rm lib/v1.92.0.zip`
- Download \
    `wget -P lib/ "https://github.com/ocornut/imgui/archive/refs/tags/v1.92.0.zip"`
- Unzip file \
    `unzip lib/v1.92.0.zip -d lib/`
- Rename \
    `mv lib/imgui-1.92.0 lib/Dear-ImGui`
- Remove zip file \
    `rm lib/v1.92.0.zip`

### GLFW

- All in one \
    `wget -P lib/ "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip" && unzip lib/3.4.zip -d lib/ && mv lib/glfw-3.4 lib/glfw && rm lib/3.4.zip && sudo apt install wayland-protocols libwayland-bin libwayland-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxkbcommon-dev pkg-config mingw-w64 mingw-w64-x86-64-dev libgl1-mesa-dev`
- Download \
    `wget -P lib/ "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip"`
- Unzip \
    `unzip lib/3.4.zip -d lib/`
- Rename \
    `mv lib/glfw-3.4 lib/glfw`
- Remove zip file \
    `rm lib/3.4.zip`
- lib
    `sudo apt install libgl-dev`
- Install wayland
    `sudo apt install wayland-protocols`
    `sudo apt install libwayland-bin`
    `sudo apt install libwayland-dev`
- Install xkbcommon
    `sudo apt install libxkbcommon-dev`
- Install libxrandr
    `sudo apt install libxrandr-dev`
- Install libxinerama
    `sudo apt install libxinerama-dev`
- Install libxcursor
    `sudo apt install libxcursor-dev`
- Install libxi
    `sudo apt install libxi-dev`
- Install pkg-config
    `sudo apt install pkg-config`
- Install mingw-w64
    `sudo apt install mingw-w64 mingw-w64-x86-64-dev`
- Install libgl1-mesa-dev
    `sudo apt install libgl1-mesa-dev`

### Vulkan

- All in one \
    `curl -fsSL https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.gpg > /dev/null && sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.296-noble.list "https://packages.lunarg.com/vulkan/1.3.296/lunarg-vulkan-1.3.296-noble.list && sudo apt install vulkan-utility-libraries-dev libvulkan-dev vulkan-tools vulkan-validationlayers && wget -P lib/ "https://sdk.lunarg.com/sdk/download/1.3.296.0/windows/VulkanSDK-1.3.296.0-Installer.exe" && sudo apt install 7zip && 7z x ./lib/VulkanSDK-1.3.296.0-Installer.exe -o./lib/vulkan-sdk-win && rm ./lib/VulkanSDK-1.3.296.0-Installer.exe`
- apt key \
    `curl -fsSL https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.gpg > /dev/null`
- apt sources \
    `sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.296-noble.list "https://packages.lunarg.com/vulkan/1.3.296/lunarg-vulkan-1.3.296-noble.list"`
- apt update
    `sudo apt update`
- install
    `sudo apt install vulkan-utility-libraries-dev libvulkan-dev vulkan-tools vulkan-validationlayers`
- download Vulkan for windows crosscomplile

    ```shell
        wget -P lib/ "https://sdk.lunarg.com/sdk/download/1.3.296.0/windows/VulkanSDK-1.3.296.0-Installer.exe"
        sudo apt install 7zip wine
        7z x ./lib/VulkanSDK-1.3.296.0-Installer.exe -o./lib/vulkan-sdk-win
        rm ./lib/VulkanSDK-1.3.296.0-Installer.exe
    ```

### GLM

- all in one
    `sudo apt install libglm-dev && wget -P lib/ "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip" && 7z x lib/1.0.1.zip -o./lib/glm && rm lib/1.0.1.zip`
- apt
    `sudo apt install libglm-dev`
- windows cross compile
    `wget -P lib/ "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip"`
    `7z x lib/1.0.1.zip -o./lib/glm`
    `rm lib/1.0.1.zip`

### STD

- all in one
    `mkdir -p lib/stb && curl -o lib/stb/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h`
- downlaod (se precisar)
    `mkdir -p lib/stb`
    `curl -o lib/stb/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h`

### GLS Lang Validator

- apt
    `sudo apt install glslang-tools`

### Assimp

- all in one
    `sudo apt-get install libassimp-dev && wget -P lib/ https://github.com/assimp/assimp/archive/refs/tags/v5.3.1.zip && 7z x lib/v5.3.1.zip -o./lib/assimp && mv lib/assimp/assimp-5.3.1/* lib/assimp/ && rm -R lib/assimp/assimp-5.3.1 && rm lib/v5.3.1.zip`
- apt (optional)
    `sudo apt-get install libassimp-dev`
- download
    `wget -P lib/ https://github.com/assimp/assimp/archive/refs/tags/v5.3.1.zip`
    `7z x lib/v5.3.1.zip -o./lib/assimp`
    `mv lib/assimp/assimp-5.3.1/* lib/assimp/`
    `rm -R lib/assimp/assimp-5.3.1`
    `rm lib/v5.3.1.zip`

### Embedded libs

```shell
sudo apt install apt-file
sudo apt-file update
sudo apt install libdecor-0-0 libdecor-0-plugin-1-gtk libdecor-0-plugin-1-cairo
sudo apt install libgtk-3-0
sudo apt install gnome-themes-extra-data
```

```shell
DEST=lib/linux
mkdir -p "$DEST/share"

LIBS=(
    libgtk-3.so.0
    libgdk-3.so.0
    libdecor-0.so.0
)
for lib in "${LIBS[@]}"; do
    cp "/usr/lib/x86_64-linux-gnu/$lib" "$DEST/"
    ldd "/usr/lib/x86_64-linux-gnu/$lib" | awk '{print $3}' | grep -E '^/' | while read dep; do
        cp -u "$dep" "$DEST/" 2>/dev/null
    done
done

mkdir -p "$DEST/libdecor/plugins-1"
cp /usr/lib/x86_64-linux-gnu/libdecor/plugins-1/* "$DEST/libdecor/plugins-1/"
mkdir -p "$DEST/share/themes"
cp -r /usr/share/themes/Adwaita "$DEST/share/themes/"
mkdir -p "$DEST/share/gtk-3.0"
cp -r /usr/share/gtk-3.0/* "$DEST/share/gtk-3.0/"
mkdir -p "$DEST/share/glib-2.0/schemas"
cp -r /usr/share/glib-2.0/schemas/* "$DEST/share/glib-2.0/schemas/"
```

## Build

```shell
mkdir build
cd build
cmake ..
make
```

- fix wayland

```shell
cat << 'EOF' > run.sh
HERE="$(dirname "$(readlink -f "$0")")"

export GTK_THEME=Adwaita
export XDG_DATA_DIRS="$HERE/lib/linux/share:/usr/share"
export GDK_BACKEND=wayland,x11
export LD_LIBRARY_PATH="$HERE/lib/linux:$LD_LIBRARY_PATH"

"$HERE/ProjectD.out"
EOF
chmod +x run.sh
```

## Build for Windows

```bash
mkdir build-windows
cd build-windows
cmake .. -D CMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake
make
```
<!-- could be util
-D UPDATE_DEPS=ON
-->

## Build Release

```shell
mkdir build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release # for realease
make
```

- fix wayland

```shell
cat << 'EOF' > run.sh
HERE="$(dirname "$(readlink -f "$0")")"

export GTK_THEME=Adwaita
export XDG_DATA_DIRS="$HERE/lib/linux/share:/usr/share"
export GDK_BACKEND=wayland,x11
export LD_LIBRARY_PATH="$HERE/lib/linux:$LD_LIBRARY_PATH"

"$HERE/ProjectD.out"
EOF
chmod +x run.sh
```

## Build for Windows Release

```bash
mkdir build-windows-release
cd build-windows-release
cmake .. -D CMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake -DCMAKE_BUILD_TYPE=Release # for realease
make
```
<!-- could be util
-D UPDATE_DEPS=ON
-->
