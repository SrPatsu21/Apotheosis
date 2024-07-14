# Set Up for linux debian

## libs

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
    `wget -P lib/ https://github.com/glfw/glfw/archive/refs/tags/3.4.zip && unzip lib/3.4.zip -d lib/ && rm lib/3.4.zip`
- Download \
    `wget -P lib/ https://github.com/glfw/glfw/archive/refs/tags/3.4.zip`
- Unzip \
    `unzip lib/3.4.zip -d lib/`
- Remove zip file \
    `rm lib/3.4.zip`
