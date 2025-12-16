#pragma once

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "../CoreVulkan.hpp"

class UI {
private:
    VkDescriptorPool descriptorPool;

public:
    UI();
    ~UI();

    void initContext(GLFWwindow* window);
    void initSwapchainResources(VkRenderPass renderPass, uint32_t imageCount);
    void init(GLFWwindow* window, VkRenderPass renderPass, uint32_t imageCount);

    void newFrame(); // start UI frame
    void build(); // build your UI widgets
    void render(VkCommandBuffer cmd); // record into Vulkan command buffer
    void cleanup();
};