#pragma once

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "../CoreVulkan.hpp"
#include "../swapchain&framebuffer/CommandManager.hpp"

class UI {
private:
    GLFWwindow* window;
    VkDevice device;
    VkDescriptorPool descriptorPool;

public:
    class ImGuiCommandBufferRecorder;

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
    void cleanup();
};

class UI::ImGuiCommandBufferRecorder final
        : public CommandManager::ICommandBufferRecorder
    {
    public:
        static ImGuiCommandBufferRecorder& instance() {
            static ImGuiCommandBufferRecorder recorder;
            return recorder;
        }

        void record(VkCommandBuffer cmd) override {
            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
        }

    private:
        ImGuiCommandBufferRecorder() = default;
    };