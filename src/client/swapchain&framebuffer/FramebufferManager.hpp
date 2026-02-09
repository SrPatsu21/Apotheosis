#pragma once

#include "../CoreVulkan.hpp"
#include "SwapchainManager.hpp"
#include "DepthBufferManager.hpp"
#include <cstring>

class FramebufferManager
{
private:
    VkDevice device;
    std::vector<VkFramebuffer> swapchainFramebuffers;

public:
    FramebufferManager(
        VkDevice device,
        VkRenderPass renderPass,
        std::vector<VkImageView> swapchainImageViews,
        const VkImageView colorImageView,
        const VkImageView depthImageView,
        VkExtent2D swapChainExtent
    );

    ~FramebufferManager();

    const std::vector<VkFramebuffer>& getFramebuffers() const { return this->swapchainFramebuffers; }
};
