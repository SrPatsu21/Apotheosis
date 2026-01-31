#pragma once

#include "../CoreVulkan.hpp"

    /**
    @class RenderPass
    @brief Manages the creation and lifetime of a Vulkan render pass.

    This class encapsulates a Vulkan render pass with a single subpass,
    which uses one color attachment and one depth-stencil attachment.
    The render pass is created at construction and automatically destroyed
    at destruction.
    */
class RenderPass {
private:
    /**
    @brief Vulkan render pass handle managed by this class.

    Holds the VkRenderPass created during construction, which defines the
    attachments, subpasses, and dependencies for rendering. It is valid
    for the lifetime of this object and destroyed in the destructor.

    Initialized to VK_NULL_HANDLE before creation.
    */
    VkDevice device;
    VkRenderPass renderPass{VK_NULL_HANDLE};

public:
    /**
    @brief Constructs a RenderPass object and creates the Vulkan render pass.

    Creates a render pass with:
    - One color attachment matching the swapchain image format.
    - One depth-stencil attachment using CoreVulkan::getDepthFormat().
    - A single graphics subpass using both attachments.
    - A dependency from the external subpass to the graphics subpass to ensure proper synchronization.

    @param swapchainImageFormat The format of the swapchain images.

    @throws std::runtime_error if vkCreateRenderPass fails.
    */
    RenderPass(VkDevice device, VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSamples, VkFormat depthFormat);

    /**
    @brief Destroys the Vulkan render pass and cleans up resources.
    */
    ~RenderPass();

    VkRenderPass get() const { return renderPass; }
};