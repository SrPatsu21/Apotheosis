@REM -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
g++ -L./lib/glfw-3.4/ -L./lib/Dear-ImGui/ -LC:\VulkanSDK\1.3.283.0\Lib -IC:\VulkanSDK\1.3.283.0\Include ./src/main.cpp -o ./build/BuildGame