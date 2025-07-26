#pragma once

#include "../CoreVulkan.hpp"
#include "../pipeline/GraphicsPipeline.hpp"

/**
 * @class CommandManager
 *
 * @brief Manages Vulkan command pool and primary command buffers for rendering.
 *
 * This class encapsulates the creation and destruction of a Vulkan command pool
 * and a set of primary command buffers, one per framebuffer. Each command buffer
 * is recorded at construction to perform the rendering of a frame, including:
 *
 * - beginning a render pass with clear values
 * - binding the graphics pipeline
 * - binding vertex and index buffers
 * - binding descriptor sets
 * - issuing a `vkCmdDrawIndexed` call
 *
 * The recorded command buffers can then be submitted each frame for rendering.
 */
class CommandManager {
private:
    /*
    A Vulkan object that manages the memory used to allocate and record command buffers.
    Command buffers are the actual "scripts" of GPU instructions that you submit to a queue.
    You allocate command buffers from a command pool.
    */
    VkCommandPool commandPool;

    /*
    The list of Vulkan command buffers, one for each framebuffer in the swapchain.
    Each command buffer records all the rendering commands for a single frame.
    These are allocated from the command pool and reused every frame.
    */
    std::vector<VkCommandBuffer> commandBuffers;

public:
    /**
     * @brief Constructs the CommandManager, creating a command pool and recording command buffers.
     *
     * @param graphicsQueueFamily The queue family index that supports graphics commands.
     * @param renderPass The Vulkan render pass used for rendering.
     * @param graphicsPipeline The graphics pipeline class.
     * @param framebuffers The list of framebuffers, one per swapchain image.
     * @param extent The rendering extent (resolution) of each framebuffer.
     * @param vertexBuffer The Vulkan vertex buffer to bind.
     * @param indexBuffer The Vulkan index buffer to bind.
     * @param indices The list of indices, to determine how many indices to draw.
     * @param descriptorSet The descriptor set to bind for each draw call.
     *
     * This constructor allocates and records one command buffer per framebuffer.
     */
    CommandManager(
        uint32_t graphicsQueueFamily,
        VkRenderPass renderPass,
        GraphicsPipeline* graphicsPipeline,
        const std::vector<VkFramebuffer>& framebuffers,
        VkExtent2D extent,
        VkBuffer vertexBuffer,
        VkBuffer indexBuffer,
        const std::vector<uint16_t>& indices,
        VkDescriptorSet descriptorSet);

    /**
     * @brief Destructor. Cleans up the Vulkan command pool and command buffers.
     */
    ~CommandManager();

    VkCommandPool getCommandPool() const { return commandPool; }
    const std::vector<VkCommandBuffer>& getCommandBuffers() const { return commandBuffers; }
};
