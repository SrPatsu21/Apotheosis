#include "InstanceDescriptorManager.hpp"
#include <stdexcept>
#include <vector>
#include <cstring>

InstanceDescriptorManager::InstanceDescriptorManager(
    VkDevice device,
    BufferManager* bufferManager,
    uint32_t maxFramesInFlight,
    uint32_t maxInstancesPerFrame
) :
    device(device),
    maxInstances(maxInstancesPerFrame)
{
    VkDeviceSize bufferSize = sizeof(glm::mat4) * maxInstances;

    buffers.resize(maxFramesInFlight);
    memory.resize(maxFramesInFlight);
    mapped.resize(maxFramesInFlight);

    for (uint32_t i = 0; i < maxFramesInFlight; i++)
    {
        bufferManager->createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            buffers[i]
        );

        bufferManager->allocateBufferMemory(
            buffers[i],
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            memory[i]
        );

        vkBindBufferMemory(device, buffers[i], memory[i], 0);

        vkMapMemory(
            device,
            memory[i],
            0,
            bufferSize,
            0,
            &mapped[i]
        );
    }

    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &binding;

    if (vkCreateDescriptorSetLayout(
            device,
            &layoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance descriptor set layout");
    }

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = maxFramesInFlight;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = maxFramesInFlight;

    if (vkCreateDescriptorPool(
            device,
            &poolInfo,
            nullptr,
            &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance descriptor pool");
    }

    std::vector<VkDescriptorSetLayout> layouts(
        maxFramesInFlight,
        descriptorSetLayout
    );

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = maxFramesInFlight;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(maxFramesInFlight);

    if (vkAllocateDescriptorSets(
            device,
            &allocInfo,
            descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate instance descriptor sets");
    }

    for (uint32_t i = 0; i < maxFramesInFlight; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = bufferSize;

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = descriptorSets[i];
        write.dstBinding = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
    }
}

void InstanceDescriptorManager::update(
    uint32_t frameIndex,
    uint32_t baseInstance,
    const std::vector<InstanceData>& models
) {
    if (baseInstance + models.size() > maxInstances)
        throw std::runtime_error("Instance buffer overflow");

    std::memcpy(
        static_cast<char*>(mapped[frameIndex]) + baseInstance * sizeof(InstanceData),
        models.data(),
        models.size() * sizeof(InstanceData)
    );
}

InstanceDescriptorManager::~InstanceDescriptorManager()
{
    for (size_t i = 0; i < buffers.size(); i++)
    {
        if (mapped[i])
            vkUnmapMemory(device, memory[i]);

        if (buffers[i])
            vkDestroyBuffer(device, buffers[i], nullptr);

        if (memory[i])
            vkFreeMemory(device, memory[i], nullptr);
    }

    if (descriptorPool)
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);

    if (descriptorSetLayout)
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}
