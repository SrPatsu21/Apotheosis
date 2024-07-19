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

- Download the VulkanSDK installer \
[Vulkan SDK](https://sdk.lunarg.com/sdk/download/1.3.283.0/windows/VulkanSDK-1.3.283.0-Installer.exe)
- Install VulkanSDK
- Make sure that Vulkan create an environment variabel named <VULKAN_SDK> \
    *if this file don't exist, include by yourself, the normal path is `C:\VulkanSDK\1.3.283.0`*
- Create a file on lib \
    `mkdir ./lib/VulkanSDK`
- Copy file to lib \
    `xcopy $Env:VULKAN_SDK\lib .\lib\VulkanSDK\lib\ /S`
    `xcopy $Env:VULKAN_SDK\Include .\lib\VulkanSDK\include\ /S`

## Souces

<https://www.rapidtables.com/code/linux/gcc/gcc-l.html>
<https://terminalroot.com.br/2023/10/aprenda-a-usar-o-dear-imgui-com-sdl2.html>
