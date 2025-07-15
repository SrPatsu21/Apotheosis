#pragma once

#include "../CoreVulkan.hpp"
#include "SwapchainManager.hpp"
#include "DepthBufferManager.hpp"
#include <cstring>

/**
 * @class FramebufferManager
 * @brief Manages the Vulkan framebuffers for the swapchain images.
 *
 * This class creates and destroys a set of framebuffers, each corresponding
 * to a swapchain image combined with the depth buffer image view.
 */
class FramebufferManager
{
private:
    /*
    A list of framebuffers — one for each swapchain image.
    A framebuffer is what your render pass writes into. It combines attachments:
    color (the swapchain image) and optionally depth/stencil.
    You need one framebuffer per swapchain image.
    */
    std::vector<VkFramebuffer> swapchainFramebuffers;

public:
    /**
     * @brief Constructs a FramebufferManager and creates all framebuffers.
     *
     * @param device The Vulkan logical device.
     * @param renderPass The render pass these framebuffers are compatible with.
     * @param swapchainManager Reference to the SwapchainManager for image views and extent.
     * @param depthBufferManager Reference to the DepthBufferManager for depth image view.
     */
    FramebufferManager(VkRenderPass renderPass, const SwapchainManager& swapchainManager, const DepthBufferManager& depthBufferManager);

    /**
     * @brief Destroys all Vulkan framebuffers.
     */
    ~FramebufferManager();

    const std::vector<VkFramebuffer>& getFramebuffers() const { return this->swapchainFramebuffers; }
};
