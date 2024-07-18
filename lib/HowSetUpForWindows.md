# Set Up for linux debian

## libs

### Dear ImGui

- All in one \
    `wget "https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.zip" -OutFile ./lib/Dear-ImGui.zip ; tar -xf ./lib/Dear-ImGui.zip -C ./lib ; mv ./lib/imgui-1.90.9 ./lib/Dear-ImGui ; rm ./lib/Dear-ImGui.zip ; mv ./lib/Dear-ImGui/backends/imgui_impl_vulkan.h ./lib/Dear-ImGui/imgui_impl_vulkan.h ; mv ./lib/Dear-ImGui/backends/imgui_impl_vulkan.cpp ./lib/Dear-ImGui/imgui_impl_vulkan.cpp ; mv ./lib/Dear-ImGui/backends/imgui_impl_glfw.h ./lib/Dear-ImGui/imgui_impl_glfw.h ; mv ./lib/Dear-ImGui/backends/imgui_impl_glfw.cpp ./lib/Dear-ImGui/imgui_impl_glfw.cpp`

- Download \
    `wget "https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.zip" -OutFile ./lib/Dear-ImGui.zip`
- Unzip file \
    `tar -xf ./lib/Dear-ImGui.zip -C ./lib`
- Rename \
    `mv ./lib/imgui-1.90.9 ./lib/Dear-ImGui`
- Remove zip file \
    `rm ./lib/Dear-ImGui.zip`
- Move to right file \
    `mv ./lib/Dear-ImGui/backends/imgui_impl_vulkan.h ./lib/Dear-ImGui/imgui_impl_vulkan.h` \
    `mv ./lib/Dear-ImGui/backends/imgui_impl_vulkan.cpp ./lib/Dear-ImGui/imgui_impl_vulkan.cpp` \
    `mv ./lib/Dear-ImGui/backends/imgui_impl_glfw.h ./lib/Dear-ImGui/imgui_impl_glfw.h` \
    `mv ./lib/Dear-ImGui/backends/imgui_impl_glfw.cpp ./lib/Dear-ImGui/imgui_impl_glfw.cpp`

### GLFW

- All in one \
    `wget "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip" -OutFile ./lib/3.4.zip ; tar -xf ./lib/3.4.zip -C ./lib ; rm ./lib/3.4.zip`
- Download \
    `wget "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip" -OutFile ./lib/3.4.zip`
- Unzip \
    `tar -xf ./lib/3.4.zip -C ./lib`
- Remove zip file \
    `rm ./lib/3.4.zip`

### Vulkan

- Make sure you hava mingw-w64
- go to mingw-w64 terminal
- Run \
 `pacman -S mingw-w64-x86_64-vulkan-loader`
 `pacman -S mingw-w64-x86_64-vulkan-headers`
 `pacman -S mingw-w64-x86_64-vulkanscenegraph`
 `pacman -S mingw-w64-x86_64-vulkan-utility-libraries`
 `pacman -S mingw-w64-x86_64-vulkan-validation-layers`
