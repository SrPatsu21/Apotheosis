#include "CommandManager.hpp"
#include "../camera/UniformBufferObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

CommandManager::CommandManager(
    VkDevice device,
    uint32_t graphicsQueueFamily,
    const std::vector<VkFramebuffer>& framebuffers
):
    device(device)
{
    // Create command pool
    createCommandPool(graphicsQueueFamily);

    // Allocate command buffers
    allocateCommandbuffers(device, framebuffers);
}

void CommandManager::createCommandPool(uint32_t graphicsQueueFamily) {
    // Create command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsQueueFamily;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &this->commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void CommandManager::allocateCommandbuffers(VkDevice device, const std::vector<VkFramebuffer>& framebuffers){
    this->commandBuffers.resize(framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = this->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(this->commandBuffers.size());

    if (vkAllocateCommandBuffers(device, &allocInfo, this->commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void CommandManager::recordCommandBuffer(
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
) {
    #ifndef NDEBUG
        assert(imageIndex < commandBuffers.size());
    #endif

    //* Command Buffer Begin
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(this->commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    //* Starting a render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    std::vector<VkClearValue> clearValues;
    clearValues.reserve(expectedCount);
    clearValues[0].color = {{0.4f, 1.0f, 1.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = &clearValues[0];

    vkCmdBeginRenderPass(this->commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    //* Scene drawing commands
    vkCmdBindPipeline(this->commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipeline());

    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(this->commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(this->commandBuffers[imageIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(this->commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &graphicsPipeline->getViewport());
    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &graphicsPipeline->getScissor());

    vkCmdDrawIndexed(this->commandBuffers[imageIndex], indicesSize, 1, 0, 0, 0);

    //* Extra recording (e.g. ImGui)
    if (extraRecording) {
        extraRecording(this->commandBuffers[imageIndex]);
    }

    vkCmdEndRenderPass(this->commandBuffers[imageIndex]);

    if (vkEndCommandBuffer(this->commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

CommandManager::~CommandManager() {
    vkDestroyCommandPool(device, this->commandPool, nullptr);
    commandPool = VK_NULL_HANDLE;
}