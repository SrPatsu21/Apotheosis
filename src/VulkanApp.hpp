#pragma once

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <vulkan/vulkan.h>

class VulkanApp {
public:
    bool Initialize();
    void RenderFrame();
    void Cleanup();
    GLFWwindow* GetWindow();

private:
    GLFWwindow* window;
    VkInstance instance;

    bool InitVulkan();
    bool InitWindow();
};