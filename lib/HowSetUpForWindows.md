# Set Up for linux debian

## libs

### Auto set libs

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

- All in one \
    `wget -P lib/ "https://sdk.lunarg.com/sdk/download/1.3.283.0/linux/vulkansdk-linux-x86_64-1.3.283.0.tar.xz" && tar -xf lib/vulkansdk-linux-x86_64-1.3.283.0.tar.xz -C lib/ && mv lib/1.3.283.0 lib/Vulkan && rm lib/vulkansdk-linux-x86_64-1.3.283.0.tar.xz`
- Download
    `wget -P lib/ "https://sdk.lunarg.com/sdk/download/1.3.283.0/linux/vulkansdk-linux-x86_64-1.3.283.0.tar.xz"`
- Unzip \
    `tar -xf lib/vulkansdk-linux-x86_64-1.3.283.0.tar.xz -C lib/`
- Rename \
    `mv lib/1.3.283.0 lib/Vulkan`
- Remove tar \
    `rm lib/vulkansdk-linux-x86_64-1.3.283.0.tar.xz`
