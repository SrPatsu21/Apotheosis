#pragma once

#include <bits/stdc++.h>
#include "../CoreVulkan.hpp"
#include "../graphics_pipeline/GraphicsPipeline.hpp"

class CommandManager {
private:
    VkDevice device;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    void createCommandPool(uint32_t graphicsQueueFamily);

public:
    struct ICommandBufferRecorder {
        virtual void record(VkCommandBuffer cmd) = 0;
    };

    struct IClearValueProvider {
        virtual void getClearValues(std::vector<VkClearValue>& out) = 0;
    };


    void allocateCommandbuffers(VkDevice device, const std::vector<VkFramebuffer>& framebuffers);

    CommandManager(
        VkDevice device,
        uint32_t graphicsQueueFamily,
        const std::vector<VkFramebuffer>& framebuffers
    );

    ~CommandManager();

    void recordCommandBuffer(
        size_t imageIndex,
        VkRenderPass renderPass,
        GraphicsPipeline* graphicsPipeline,
        const std::vector<VkFramebuffer>& framebuffers,
        VkExtent2D extent,
        VkBuffer vertexBuffer,
        VkBuffer indexBuffer,
        uint32_t indicesSize,
        VkDescriptorSet descriptorSet,
        std::function<void(VkCommandBuffer)> extraRecording
    );

    VkCommandPool getCommandPool() const { return commandPool; }
    const std::vector<VkCommandBuffer>& getCommandBuffers() const { return commandBuffers; }
};
