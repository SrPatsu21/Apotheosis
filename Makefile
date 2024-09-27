DearImGui =  ./lib/Dear-ImGui/*.cpp -I./lib/Dear-ImGui/
GLFW = ./lib/glfw-3.4/src/vulkan.c -I./lib/glfw-3.4/include/
Vulkan = ./lib/VulkanSDK/x86_64/lib/* -I./lib/VulkanSDK/x86_64/include/

BuildGame: ./src/main.cpp
	echo "Building..."
	x86_64-linux-gnu-g++ ${DearImGui} ${GLFW} ${Vulkan} ./src/main.cpp -o ./build/BuildGame
	echo "Ready"