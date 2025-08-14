#include "CommandManager.hpp"
#include "../camera/UniformBufferObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

CommandManager::CommandManager(
    uint32_t graphicsQueueFamily, VkRenderPass renderPass, GraphicsPipeline* graphicsPipeline,
    const std::vector<VkFramebuffer>& framebuffers, VkExtent2D extent, VkBuffer vertexBuffer, VkBuffer indexBuffer,
    const std::vector<uint16_t>& indices, VkDescriptorSet descriptorSet)
    {

    // Create command pool
    createCommandPool(graphicsQueueFamily);

    // Allocate command buffers
    allocateCommandbuffers(framebuffers);

    // Record command buffers
    recordCommandBuffer(renderPass, graphicsPipeline, framebuffers, extent, vertexBuffer, indexBuffer, indices, descriptorSet);
}

void CommandManager::createCommandPool(uint32_t graphicsQueueFamily) {
    // Create command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsQueueFamily;

    if (vkCreateCommandPool(CoreVulkan::getDevice(), &poolInfo, nullptr, &this->commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void CommandManager::allocateCommandbuffers(const std::vector<VkFramebuffer>& framebuffers){
    this->commandBuffers.resize(framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = this->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(this->commandBuffers.size());

    if (vkAllocateCommandBuffers(CoreVulkan::getDevice(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void CommandManager::recordCommandBuffer(VkRenderPass renderPass, GraphicsPipeline* graphicsPipeline,
    const std::vector<VkFramebuffer>& framebuffers, VkExtent2D extent, VkBuffer vertexBuffer, VkBuffer indexBuffer,
    const std::vector<uint16_t>& indices, VkDescriptorSet descriptorSet)
    {
    for (size_t i = 0; i < this->commandBuffers.size(); ++i) {
        //* Command Buffer Begin
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(this->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        //* Starting a render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;

        // Clear screem color
        VkClearValue clearValues[2];
        clearValues[0].color = {{0.4f, 1.0f, 1.0f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(this->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        //* drawing commands
        vkCmdBindPipeline(this->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipeline());

        // Bind vertex/index buffers
        VkBuffer vertexBuffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(this->commandBuffers[i], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(this->commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

        // Bind descriptor sets
        vkCmdBindDescriptorSets(this->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getLayout(), 0, 1, &descriptorSet, 0, nullptr);

        // viewport and scissor dynamic
        VkViewport viewport = graphicsPipeline->getViewport();
        VkRect2D scissor = graphicsPipeline->getScissor();

        vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);

        // Draw
        vkCmdDrawIndexed(this->commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(this->commandBuffers[i]);

        if (vkEndCommandBuffer(this->commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

CommandManager::~CommandManager() {
    vkDestroyCommandPool(CoreVulkan::getDevice(), this->commandPool, nullptr);
}