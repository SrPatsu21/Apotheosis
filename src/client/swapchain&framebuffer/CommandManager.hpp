#pragma once

#include <bits/stdc++.h>
#include "../CoreVulkan.hpp"
#include "../graphics_pipeline/GraphicsPipeline.hpp"
#include "../batch/instance/PushConstantObject.hpp"
#include "../batch/RenderBatchManager.hpp"

/**
 * @brief Manages Vulkan command buffers and their recording lifecycle.
 *
 * CommandManager owns a command pool and one primary command buffer per
 * framebuffer. It provides a flexible, modular recording pipeline using
 * provider-style interfaces for:
 *
 * - Clear values
 * - Viewport overrides
 * - Scissor overrides
 * - Additional custom command recording
 *
 * This design allows rendering behavior to be extended without modifying
 * the core command buffer logic.
 */
class CommandManager {
public:
    /**
     * @brief Interface for injecting custom Vulkan commands into a command buffer.
     *
     * Implementations can record arbitrary Vulkan commands
     * (e.g. ImGui, debug draws, post-processing).
     */
    struct ICommandBufferRecorder {
        virtual ~ICommandBufferRecorder() = default;
        virtual void record(VkCommandBuffer cmd) = 0;
    };

    /**
     * @brief Interface for contributing clear values to a render pass.
     *
     * Providers append VkClearValue entries in render-pass attachment order.
     */
    struct IClearValueProvider {
        virtual ~IClearValueProvider() = default;
        virtual void contribute(std::vector<VkClearValue>& clearValues) = 0;
    };

    /**
     * @brief Interface for optionally overriding the viewport.
     *
     * Returning true indicates the viewport was overridden
     * and no further providers should be consulted.
     */
    struct IViewportProvider {
        virtual ~IViewportProvider() = default;
        virtual bool overrideViewport(VkViewport& viewport) = 0;
    };

    /**
     * @brief Interface for optionally overriding the scissor rectangle.
     *
     * Returning true indicates the scissor was overridden
     * and no further providers should be consulted.
     */
    struct IScissorProvider {
        virtual ~IScissorProvider() = default;
        virtual bool overrideScissor(VkRect2D& scissor) = 0;
    };

private:
    VkDevice device;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    /**
     * @brief Creates the Vulkan command pool used to allocate command buffers.
     *
     * The command pool is created for the graphics queue family and configured
     * to allow individual command buffers to be reset and re-recorded.
     *
     * @param graphicsQueueFamily Queue family index that supports graphics commands.
     *
     * @throws std::runtime_error if the command pool creation fails.
     */
    void createCommandPool(
        uint32_t graphicsQueueFamily
    );

    /**
     * @brief Begins recording of a command buffer.
     *
     * This resets the command buffer state and prepares it for command recording.
     * The command buffer must not be in use by the GPU.
     *
     * @param cmd Command buffer to begin recording.
     *
     * @throws std::runtime_error if recording cannot be started.
     */
    void beginCommandBuffer(
        VkCommandBuffer cmd
    );

    /**
     * @brief Builds the list of clear values used when beginning a render pass.
     *
     * Clear value providers may contribute one or more VkClearValue entries.
     * If no providers contribute any values, a default color and depth clear
     * value are generated.
     *
     * @param providers Clear value providers.
     * @param clearValues Output vector filled with clear values.
     */
    void buildClearValues(
        const std::vector<IClearValueProvider*>& providers,
        std::vector<VkClearValue>& clearValues
    );

    /**
     * @brief Begins a render pass on the given command buffer.
     *
     * The render pass is started with the provided framebuffer, render area,
     * and clear values.
     *
     * @param cmd Command buffer being recorded.
     * @param renderPass Render pass to begin.
     * @param framebuffer Framebuffer used for rendering.
     * @param extent Render area extent.
     * @param clearValues Clear values matching the render pass attachments.
     */
    void beginRenderPass(
        VkCommandBuffer cmd,
        VkRenderPass renderPass,
        VkFramebuffer framebuffer,
        VkExtent2D extent,
        const std::vector<VkClearValue>& clearValues
    );

    /**
     * @brief Binds the graphics pipeline and required GPU resources.
     *
     * This function binds:
     * - The graphics pipeline
     * - Vertex buffer
     * - Index buffer
     * - Descriptor set
     *
     * @param cmd Command buffer being recorded.
     * @param graphicsPipeline Graphics pipeline to bind.
     * @param vertexBuffer Vertex buffer handle.
     * @param indexBuffer Index buffer handle.
     * @param descriptorSet Descriptor set to bind.
     */
    void bindPipelineAndResources(
        VkCommandBuffer cmd,
        GraphicsPipeline* graphicsPipeline,
        VkBuffer vertexBuffer,
        VkBuffer indexBuffer,
        VkDescriptorSet globalDescriptorSet,
        VkDescriptorSet materialDescriptorSet
    );

    /**
     * @brief Sets the viewport and scissor rectangles for rendering.
     *
     * The default viewport and scissor are taken from the graphics pipeline.
     * Providers may override these values; the first provider that returns true
     * stops further overrides.
     *
     * @param cmd Command buffer being recorded.
     * @param graphicsPipeline Graphics pipeline providing default viewport/scissor.
     * @param viewportProviders Viewport override providers.
     * @param scissorProviders Scissor override providers.
     */
    void setViewportAndScissor(
        VkCommandBuffer cmd,
        GraphicsPipeline* graphicsPipeline,
        const std::vector<IViewportProvider*>& viewportProviders,
        const std::vector<IScissorProvider*>& scissorProviders
    );

public:
    /**
     * @brief Allocates one primary command buffer per framebuffer.
     */
    void allocateCommandbuffers(
        VkDevice device,
        const std::vector<VkFramebuffer>& framebuffers
    );

    /**
     * @brief Creates the command pool and allocates command buffers.
     *
     * @param device Logical Vulkan device.
     * @param graphicsQueueFamily Queue family index for graphics commands.
     * @param framebuffers Swapchain framebuffers.
     */
    CommandManager(
        VkDevice device,
        uint32_t graphicsQueueFamily,
        const std::vector<VkFramebuffer>& framebuffers
    );

    /**
     * @brief Destroys the command pool and all allocated command buffers.
     */
    ~CommandManager();

    /**
     * @brief Records commands for a specific swapchain image.
     *
     * This function records:
     * - Render pass begin
     * - Pipeline and resource binding
     * - Viewport and scissor setup
     * - Indexed draw call
     * - Optional extra command recorders
     * - Render pass end
     *
     * @param imageIndex Index of the swapchain image.
     * @param renderPass Render pass used for drawing.
     * @param graphicsPipeline Graphics pipeline to bind.
     * @param framebuffers Swapchain framebuffers.
     * @param extent Render area extent.
     * @param vertexBuffer Vertex buffer handle.
     * @param indexBuffer Index buffer handle.
     * @param indicesSize Number of indices to draw.
     * @param descriptorSet Descriptor set to bind.
     * @param clearProviders Clear value providers.
     * @param viewportProviders Viewport override providers.
     * @param scissorProviders Scissor override providers.
     * @param extraRecorders Additional command buffer recorders.
     */
    void recordCommandBuffer(
        size_t imageIndex,
        VkRenderPass renderPass,
        GraphicsPipeline* graphicsPipeline,
        const std::vector<VkFramebuffer>& framebuffers,
        VkExtent2D extent,
        VkBuffer vertexBuffer,
        VkBuffer indexBuffer,
        uint32_t indicesSize,
        VkDescriptorSet globalDescriptorSet,
        VkDescriptorSet materialDescriptorSet,
        const std::vector<IClearValueProvider*>& clearProviders,
        const std::vector<IViewportProvider*>& viewportProviders,
        const std::vector<IScissorProvider*>& scissorProviders,
        const std::vector<ICommandBufferRecorder*>& extraRecorders
    );

    VkCommandPool getCommandPool() const { return commandPool; }
    const std::vector<VkCommandBuffer>& getCommandBuffers() const { return commandBuffers; }
};
