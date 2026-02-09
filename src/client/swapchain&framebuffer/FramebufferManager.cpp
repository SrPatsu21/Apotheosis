#include "FramebufferManager.hpp"
#include <array>

FramebufferManager::FramebufferManager(
    VkDevice device,
    VkRenderPass renderPass,
    std::vector<VkImageView> swapchainImageViews,
    const VkImageView colorImageView,
    const VkImageView depthImageView,
    const VkExtent2D swapChainExtent
) :
    device(device)
{
    this->swapchainFramebuffers.resize(swapchainImageViews.size());

    for (size_t i = 0; i < swapchainImageViews.size(); ++i) {

        std::array<VkImageView, 3> attachments;
        attachments[0] = colorImageView; // MSAA color
        attachments[1] = depthImageView; // depth/stencil
        attachments[2] = swapchainImageViews[i]; // resolve / present

        #ifndef NDEBUG
            assert(renderPass != VK_NULL_HANDLE);
            assert(colorImageView != VK_NULL_HANDLE);
            assert(depthImageView != VK_NULL_HANDLE);
            assert(!swapchainImageViews.empty());
        #endif

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &this->swapchainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

FramebufferManager::~FramebufferManager() {
    for (VkFramebuffer& framebuffer : this->swapchainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
    swapchainFramebuffers.clear();
}
