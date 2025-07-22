#include "FramebufferManager.hpp"

FramebufferManager::FramebufferManager(VkRenderPass renderPass, const SwapchainManager* swapchainManager, const DepthBufferManager* depthBufferManager) {
    const auto& swapchainImageViews = swapchainManager->getImageViews();
    VkImageView depthImageView = depthBufferManager->getDepthImageView();
    VkExtent2D extent = swapchainManager->getExtent();

    this->swapchainFramebuffers.resize(swapchainImageViews.size());

    for (size_t i = 0; i < swapchainImageViews.size(); ++i) {
        std::vector<VkImageView> attachments = {
            swapchainImageViews[i],
            depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(CoreVulkan::getDevice(), &framebufferInfo, nullptr, &this->swapchainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

FramebufferManager::~FramebufferManager() {
    for (auto framebuffer : this->swapchainFramebuffers) {
        vkDestroyFramebuffer(CoreVulkan::getDevice(), framebuffer, nullptr);
    }
}
