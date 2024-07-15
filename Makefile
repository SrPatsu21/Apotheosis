DearImGui =  -L./lib/Dear-ImGui/
GLFW = -L./lib/glfw-3.4/
Vulkan = -L./lib/Vulkan/x86_64/lib/** -I./lib/Vulkan/x86_64/include/**

BuildGame: ./src/main.cpp
	echo "Building...\n"
	g++ ${DearImGui} ${GLFW} ${Vulkan} ./src/main.cpp -o ./build/BuildGame
	echo "Ready"