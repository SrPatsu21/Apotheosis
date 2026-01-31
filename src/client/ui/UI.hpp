#pragma once

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "../CoreVulkan.hpp"

class UI {
private:
    GLFWwindow* window;
    VkDevice device;
    VkDescriptorPool descriptorPool;

public:
    UI();
    ~UI();

    void initContext();
    void initSwapchainResources(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        QueueFamilyIndices graphicsQueueFamilyIndices,
        VkQueue GraphicsQueue,
        VkRenderPass renderPass,
        uint32_t imageCount,
        VkSampleCountFlagBits msaaSamples
    );
    void init(
        GLFWwindow* window,
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        QueueFamilyIndices graphicsQueueFamilyIndices,
        VkQueue GraphicsQueue,
        VkRenderPass renderPass,
        uint32_t imageCount,
        VkSampleCountFlagBits msaaSamples
    );

    void newFrame(); // start UI frame
    void build(); // build your UI widgets
    void render(VkCommandBuffer cmd); // record into Vulkan command buffer
    void cleanup();
};