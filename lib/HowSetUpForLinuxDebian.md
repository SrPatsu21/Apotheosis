# Set Up for linux debian

## libs

### Auto set libs

    wget -P lib/ "https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.zip" && unzip lib/v1.90.9.zip -d lib/ && mv lib/imgui-1.90.9 lib/Dear-ImGui && rm lib/v1.90.9.zip && mv lib/Dear-ImGui/backends/imgui_impl_vulkan.h lib/Dear-ImGui/imgui_impl_vulkan.h && mv lib/Dear-ImGui/backends/imgui_impl_vulkan.cpp lib/Dear-ImGui/imgui_impl_vulkan.cpp && mv lib/Dear-ImGui/backends/imgui_impl_glfw.h lib/Dear-ImGui/imgui_impl_glfw.h && mv lib/Dear-ImGui/backends/imgui_impl_glfw.cpp lib/Dear-ImGui/imgui_impl_glfw.cpp && wget -P lib/ "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip" && unzip lib/3.4.zip -d lib/ && rm lib/3.4.zip && wget -P lib/ "https://sdk.lunarg.com/sdk/download/1.3.283.0/linux/vulkansdk-linux-x86_64-1.3.283.0.tar.xz"

### Dear ImGui

- All in one \
    `wget -P lib/ "https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.zip" && unzip lib/v1.90.9.zip -d lib/ && mv lib/imgui-1.90.9 lib/Dear-ImGui && rm lib/v1.90.9.zip && mv lib/Dear-ImGui/backends/imgui_impl_vulkan.h lib/Dear-ImGui/imgui_impl_vulkan.h && mv lib/Dear-ImGui/backends/imgui_impl_vulkan.cpp lib/Dear-ImGui/imgui_impl_vulkan.cpp && mv lib/Dear-ImGui/backends/imgui_impl_glfw.h lib/Dear-ImGui/imgui_impl_glfw.h && mv lib/Dear-ImGui/backends/imgui_impl_glfw.cpp lib/Dear-ImGui/imgui_impl_glfw.cpp`
- Download \
    `wget -P lib/ "https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.zip"`
- Unzip file \
    `unzip lib/v1.90.9.zip -d lib/`
- Rename \
    `mv lib/imgui-1.90.9 lib/Dear-ImGui`
- Remove zip file \
    `rm lib/v1.90.9.zip`
- Move to right file \
    `mv lib/Dear-ImGui/backends/imgui_impl_vulkan.h lib/Dear-ImGui/imgui_impl_vulkan.h` \
    `mv lib/Dear-ImGui/backends/imgui_impl_vulkan.cpp lib/Dear-ImGui/imgui_impl_vulkan.cpp` \
    `mv lib/Dear-ImGui/backends/imgui_impl_glfw.h lib/Dear-ImGui/imgui_impl_glfw.h` \
    `mv lib/Dear-ImGui/backends/imgui_impl_glfw.cpp lib/Dear-ImGui/imgui_impl_glfw.cpp`

### GLFW

- All in one \
    `wget -P lib/ "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip" && unzip lib/3.4.zip -d lib/ && rm lib/3.4.zip`
- Download \
    `wget -P lib/ "https://github.com/glfw/glfw/archive/refs/tags/3.4.zip"`
- Unzip \
    `unzip lib/3.4.zip -d lib/`
- Remove zip file \
    `rm lib/3.4.zip`

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
