#include "CommandManager.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../batch/instance/RenderInstance.hpp"

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

void CommandManager::beginCommandBuffer(
    VkCommandBuffer cmd
) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

void CommandManager::buildClearValues(
    const std::vector<IClearValueProvider*>& providers,
    std::vector<VkClearValue>& clearValues
) {
    for (auto* p : providers) {
        p->contribute(clearValues);
    }

    if (clearValues.empty()) {
        clearValues.reserve(2);
        VkClearValue color{};
        color.color = {{0.4f, 1.0f, 1.0f, 1.0f}};
        clearValues.emplace_back(color);

        VkClearValue depth{};
        depth.depthStencil = {1.0f, 0};
        clearValues.emplace_back(depth);
    }
}

void CommandManager::beginRenderPass(
    VkCommandBuffer cmd,
    VkRenderPass renderPass,
    VkFramebuffer framebuffer,
    VkExtent2D extent,
    const std::vector<VkClearValue>& clearValues
) {
    VkRenderPassBeginInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = renderPass;
    info.framebuffer = framebuffer;
    info.renderArea.extent = extent;
    info.clearValueCount = static_cast<uint32_t>(clearValues.size());
    info.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandManager::bindPipelineAndResources(
    VkCommandBuffer cmd,
    GraphicsPipeline* graphicsPipeline,
    VkBuffer vertexBuffer,
    VkBuffer indexBuffer,
    VkDescriptorSet globalDescriptorSet,
    VkDescriptorSet materialDescriptorSet
) {
    vkCmdBindPipeline(
        cmd,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphicsPipeline->getPipeline()
    );

    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(
        cmd,
        0,
        1,
        &vertexBuffer,
        offsets
    );

    vkCmdBindIndexBuffer(
        cmd,
        indexBuffer,
        0,
        VK_INDEX_TYPE_UINT32
    );

    vkCmdBindDescriptorSets(
        cmd,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphicsPipeline->getLayout(),
        0,
        1,
        &globalDescriptorSet,
        0,
        nullptr
    );

    vkCmdBindDescriptorSets(
        cmd,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphicsPipeline->getLayout(),
        1,
        1,
        &materialDescriptorSet,
        0,
        nullptr
    );
}

void CommandManager::setViewportAndScissor(
    VkCommandBuffer cmd,
    GraphicsPipeline* graphicsPipeline,
    const std::vector<IViewportProvider*>& viewportProviders,
    const std::vector<IScissorProvider*>& scissorProviders
) {
    // Viewport
    VkViewport viewport = graphicsPipeline->getViewport();
    for (auto* p : viewportProviders) {
        if (p->overrideViewport(viewport)) {
            break;
        }
    }
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    // Scissor
    VkRect2D scissor = graphicsPipeline->getScissor();
    for (auto* p : scissorProviders) {
        if (p->overrideScissor(scissor)) {
            break;
        }
    }
    vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void CommandManager::recordCommandBuffer(
    uint32_t imageIndex,
    uint32_t currentFrame,
    VkRenderPass renderPass,
    GraphicsPipeline* graphicsPipeline,
    const std::vector<VkFramebuffer>& framebuffers,
    VkExtent2D extent,
    GlobalDescriptorManager* globalDescriptorManager,
    InstanceDescriptorManager* instanceDescriptorManager,
    RenderBatchManager* renderBatchManager,
    const std::vector<IClearValueProvider*>& clearProviders,
    const std::vector<IViewportProvider*>& viewportProviders,
    const std::vector<IScissorProvider*>& scissorProviders,
    const std::vector<ICommandBufferRecorder*>& extraRecorders
) {
#ifndef NDEBUG
    assert(imageIndex < commandBuffers.size());
    assert(imageIndex < framebuffers.size());
#endif

    VkCommandBuffer cmd = commandBuffers[imageIndex];
    beginCommandBuffer(cmd);

    std::vector<VkClearValue> clearValues;
    buildClearValues(
        clearProviders,
        clearValues
    );

    beginRenderPass(
        cmd,
        renderPass,
        framebuffers[imageIndex],
        extent,
        clearValues
    );

    // Bind pipeline
    vkCmdBindPipeline(
        cmd,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphicsPipeline->getPipeline()
    );

    setViewportAndScissor(
        cmd,
        graphicsPipeline,
        viewportProviders,
        scissorProviders
    );

    // browse batches
    VkPipelineLayout layout = graphicsPipeline->getLayout();
    renderBatchManager->forEachBatch(
        [&](const RenderBatchManager::RenderBatch& batch)
        {
            const auto& key = batch.getKey();
            const auto& mesh = key.mesh;
            const auto& material = key.material;
            const auto& instances = batch.getInstances();

            uint32_t instanceCount = static_cast<uint32_t>(instances.size());

            // Bind vertex buffer
            VkBuffer vertexBuffer = mesh->getVertexBuffer();
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, offsets);

            // Bind index buffer
            vkCmdBindIndexBuffer(
                cmd,
                mesh->getIndexBuffer(),
                0,
                VK_INDEX_TYPE_UINT32
            );

            // Bind descriptor sets (set 0 & 1)
            VkDescriptorSet descriptorSets[] = {
                globalDescriptorManager->getDescriptorSets()[currentFrame],
                material->getDescriptorSet()
            };

            vkCmdBindDescriptorSets(
                cmd,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                layout,
                0,
                2,
                descriptorSets,
                0,
                nullptr
            );

            // Collect model matrices
            std::vector<InstanceData> models;
            models.reserve(instanceCount);

            for (const RenderInstance* instance : instances)
            {
                models.push_back(instance->getModelMatrix());
            }

            // Update storage buffer of the current frame.
            instanceDescriptorManager->update(
                currentFrame,
                models
            );

            // Bind descriptor set 2 (instances)
            VkDescriptorSet instanceSet = instanceDescriptorManager->getDescriptorSets()[currentFrame];

            vkCmdBindDescriptorSets(
                cmd,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                layout,
                2, // set index
                1,
                &instanceSet,
                0,
                nullptr
            );

            // Draw instanciado
            vkCmdDrawIndexed(
                cmd,
                mesh->getIndexCount(),
                instanceCount,
                0,
                0,
                0
            );
        }
    );

    // Extra recorders (ImGui, debug, etc)
    for (auto* r : extraRecorders) {
        r->record(cmd);
    }

    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

CommandManager::~CommandManager() {
    vkDestroyCommandPool(device, this->commandPool, nullptr);
    commandPool = VK_NULL_HANDLE;
}