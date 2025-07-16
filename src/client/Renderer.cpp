#include "Renderer.hpp"

void Renderer::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(CoreVulkan::getDevice(), &semaphoreInfo, nullptr, &this->imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(CoreVulkan::getDevice(), &semaphoreInfo, nullptr, &this->renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(CoreVulkan::getDevice(), &fenceInfo, nullptr, &this->inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

Renderer::~Renderer(){
    vkDestroySemaphore(CoreVulkan::getDevice(), this->renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(CoreVulkan::getDevice(), this->imageAvailableSemaphore, nullptr);
    vkDestroyFence(CoreVulkan::getDevice(), inFlightFence, nullptr);
}
